/*
 * Copyright (c) 1995, 1994, 1993, 1992, 1991, 1990  
 * Open Software Foundation, Inc. 
 *  
 * Permission to use, copy, modify, and distribute this software and 
 * its documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appears in all copies and 
 * that both the copyright notice and this permission notice appear in 
 * supporting documentation, and that the name of ("OSF") or Open Software 
 * Foundation not be used in advertising or publicity pertaining to 
 * distribution of the software without specific, written prior permission. 
 *  
 * OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE 
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL OSF BE LIABLE FOR ANY 
 * SPECIAL, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN 
 * ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING 
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE 
 */
/*
 * OSF Research Institute MK6.1 (unencumbered) 1/31/1995
 */
/* CMU_HIST */
/*
 *	Joseph S. Barrera III
 *	Routines for copying pages while in copy-object form.
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1992 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */

#include <kern/misc_protos.h>
#include <vm/vm_fault.h>
#include <norma/xmm_svm.h>

/*
 * Forward.
 */
vm_page_t	svm_find_page(
			vm_object_t	object,
			vm_offset_t	offset);

/*
 * Make unpageable pages that are being kept by the vm_map_copy.
 */
void
svm_prepare_copy(
	vm_map_copy_t	copy)
{
	vm_page_t	m;

	assert(copy->size == PAGE_SIZE);

	switch(copy->type) {
	case VM_MAP_COPY_OBJECT:
		assert(copy->cpy_object != VM_OBJECT_NULL);
		m = svm_find_page(copy->cpy_object, copy->offset);
		assert(m);
		vm_object_lock(m->object);
		vm_page_lock_queues();
		vm_page_wire(m);
		vm_page_unlock_queues();
		PAGE_WAKEUP_DONE(m);
		vm_object_paging_end(m->object);
		vm_object_unlock(m->object);
		break;

	case VM_MAP_COPY_PAGE_LIST:
		assert(!vm_map_copy_has_cont(copy));
		m = copy->cpy_page_list[0];
		assert((!m->active && !m->inactive) || m->gobbled);
		break;

	default:
		panic("svm_prepare_copy: incorrect vm_map_copy type");
	}
}

/*
 * Find page at offset in object.
 * Returns page busy and with paging reference to its object.
 *
 * XXX
 * In the general form, we should return VM_PAGE_NULL if data_unavailable.
 * This requires a change to vm_fault_page interface.
 */
vm_page_t
svm_find_page(
	vm_object_t	object,
	vm_offset_t	offset)
{
	vm_page_t m;
	vm_prot_t result_prot;
	vm_page_t top_page;
	kern_return_t kr;

	/*
	 * Find the page, first trying vm_page_lookup, then vm_fault_page.
	 */
	vm_object_lock(object);
	vm_object_paging_begin(object);
	m = vm_page_lookup(object, offset);
	if (m != VM_PAGE_NULL &&
	    !m->busy && !m->fictitious && !m->absent && m->error ) {
		m->busy = TRUE;
	} else {
		for (;;) {
			result_prot = VM_PROT_READ;
			kr = vm_fault_page(object, offset, VM_PROT_READ,
					   FALSE, FALSE, &result_prot,
					   &m, &top_page, 0, FALSE);
			switch (kr) {
			case KERN_SUCCESS:
				break;

			case VM_FAULT_MEMORY_SHORTAGE:
				VM_PAGE_WAIT();
				vm_object_lock(object);
				vm_object_paging_begin(object);
				continue;
				
			case VM_FAULT_FICTITIOUS_SHORTAGE:
				vm_page_more_fictitious();
				vm_object_lock(object);
				vm_object_paging_begin(object);
				continue;
				
			case VM_FAULT_INTERRUPTED: /* ??? */
			case VM_FAULT_RETRY:
				vm_object_lock(object);
				vm_object_paging_begin(object);
				continue;
				
			default:
				assert(kr == VM_FAULT_MEMORY_ERROR);
				return m;
			}
			break;
		}
		if (top_page != VM_PAGE_NULL) {
			vm_object_lock(object);
			VM_PAGE_FREE(top_page);
			vm_object_paging_end(object);
			vm_object_unlock(object);
		}
	}
	assert(m);
	assert(m->busy);
	assert(m->object);
	assert(m->object->paging_in_progress);
	vm_object_unlock(m->object);
	return m;
}

vm_map_copy_t
svm_copy_page(
	vm_map_copy_t	old_copy)
{
	vm_map_copy_t new_copy;
	vm_page_t old_m, new_m;

	/*
	 * Allocate a new copy object.
	 */
	new_copy = (vm_map_copy_t) zalloc(vm_map_copy_zone);
	if (new_copy == VM_MAP_COPY_NULL) {
		panic("svm_copy: zalloc");
	}
	new_copy->type = VM_MAP_COPY_PAGE_LIST;
	new_copy->cpy_npages = 1;
	new_copy->offset = 0;
	new_copy->size = PAGE_SIZE;
	new_copy->cpy_cont = VM_MAP_COPY_CONT_NULL;
	new_copy->cpy_cont_args = VM_MAP_COPYIN_ARGS_NULL;

	/*
	 * Allocate a new page and insert it into new copy object.
	 */
	while ((new_m = vm_page_grab()) == VM_PAGE_NULL) {
		VM_PAGE_WAIT();
	}
	vm_page_gobble(new_m); /* mark as consumed internally */
	new_copy->cpy_page_list[0] = new_m;

	/*
	 * Find old page, and copy into new.
	 */
	assert(old_copy->size == PAGE_SIZE);
	assert(page_aligned(old_copy->offset));
	if (old_copy->type == VM_MAP_COPY_PAGE_LIST) {
		old_m = old_copy->cpy_page_list[0];
		pmap_copy_page(old_m->phys_addr, new_m->phys_addr);
	} else {
		register vm_object_t object = old_copy->cpy_object;

		assert(old_copy->type == VM_MAP_COPY_OBJECT);

		old_m = svm_find_page(object, old_copy->offset);
		pmap_copy_page(old_m->phys_addr, new_m->phys_addr);
		vm_object_lock(object);
		PAGE_WAKEUP_DONE(old_m);
		vm_object_paging_end(object);
		vm_object_unlock(object);
	}

	/*
	 * Copy old page into new, and return new copy object.
	 */
	return new_copy;
}
