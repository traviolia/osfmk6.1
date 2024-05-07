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
 * Revision 2.1.2.8  92/09/15  17:35:17  jeffreyh
 * 	Add on_lruq field to track whether buffer is on lru queue.
 * 	Add asserts to catch failures; most require the addition of code.
 * 		If xmm_buffer_copy can't allocate a page, wait for one.
 * 	[92/09/10            dlb]
 * 	Changed data type buffer to xmm_buffer to avoid profile declarations.
 * 	[92/08/12            sjs]
 * 
 * 	In xmm_buffer_copy(), undo effects of xmm_buffer_find_page()
 * 	by unbusying page and release paging reference on its object.
 * 	[92/07/06            dlb]
 * 
 * Revision 2.1.2.7  92/06/24  18:02:14  jeffreyh
 * 	Calling sequence changes for XMM framework simplification.
 * 	[92/06/24            dlb]
 * 
 * 	Convert use_old_pageout to use_routine.  Add logic to drop
 * 	data_initialize request if data is already in a buffer.
 * 	[92/06/04            dlb]
 * 
 * Revision 2.1.2.6  92/03/28  10:12:29  jeffreyh
 * 	In xmm_buffer_find_page m can not be a register variable.
 * 	[92/03/26            andyp]
 * 	Changed data_write to data_write_return(), deleted data_return
 * 	 method, save new parameters into the buffer layer
 * 	[92/03/20            sjs]
 * 	Pick up changes from revision 2.2.1.1 to fix xmm_buffer_find_page.
 * 	[92/03/19            jeffreyh]
 * 
 * Revision 2.1.2.5  92/02/21  11:25:30  jsb
 * 	Let xmm_kobj_link handle multiple init detection.
 * 	[92/02/18  08:04:06  jsb]
 * 
 * 	Explicitly provide name parameter to xmm_decl macro.
 * 	Changed debugging printf. Changed termination logic.
 * 	[92/02/16  15:18:10  jsb]
 * 
 * 	First real implementation.
 * 	[92/02/09  14:17:44  jsb]
 * 
 * Revision 2.1.2.3  92/01/21  21:53:42  jsb
 * 	De-linted. Supports new (dlb) memory object routines.
 * 	Supports arbitrary reply ports to lock_request, etc.
 * 	Converted mach_port_t (and port_t) to ipc_port_t.
 * 	[92/01/20  17:18:38  jsb]
 * 
 * Revision 2.1.2.2  92/01/03  16:38:41  jsb
 * 	First checkin.
 * 	[91/12/31  17:26:56  jsb]
 * 
 * Revision 2.1.2.1  92/01/03  08:57:47  jsb
 * 	First NORMA branch checkin.
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
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
/*
 */
/*
 *	File:	xmm_buffer.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Xmm layer which buffers a small amount of data_written data.
 */

#ifdef	MACH_KERNEL
#include <kern/queue.h>
#include <norma/xmm_obj.h>
#include <mach/vm_param.h>
#include <vm/vm_fault.h>
#include <vm/vm_page.h>
#else	/* MACH_KERNEL */
#include <xmm_obj.h>
#endif	/* MACH_KERNEL */

#define	dprintf	xmm_buffer_dprintf

typedef struct xmm_buffer	*xmm_buffer_t;
#define	BUFFER_NULL	((xmm_buffer_t) 0)

struct xmm_buffer {
	queue_chain_t	lruq;
	queue_chain_t	mobjq;
	boolean_t	free;
	int		busy_count;
	vm_map_copy_t	copy;
	xmm_obj_t	mobj;
	vm_offset_t	offset;
	xmm_buffer_t	next_free;
	boolean_t	dirty;
	boolean_t	kernel_copy;
	int		use_routine;
};

struct mobj {
	struct xmm_obj	obj;
	queue_head_t	buffers;
	boolean_t	ready;
};

#undef	KOBJ
#define	KOBJ	((struct mobj *) kobj)

#define	m_buffer_copy			m_interpose_copy
#define	m_buffer_data_unlock		m_interpose_data_unlock
#define	m_buffer_lock_completed		m_interpose_lock_completed
#define	m_buffer_supply_completed	m_interpose_supply_completed
#define	m_buffer_change_completed	m_interpose_change_completed

#define	k_buffer_data_unavailable	k_interpose_data_unavailable
#define	k_buffer_get_attributes		k_interpose_get_attributes
#define	k_buffer_lock_request		k_interpose_lock_request
#define	k_buffer_data_error		k_interpose_data_error
#define	k_buffer_destroy		k_interpose_destroy
#define	k_buffer_data_supply		k_interpose_data_supply
#define	k_buffer_release		k_invalid_release

xmm_decl(buffer, "buffer", sizeof(struct mobj));

#define	XMM_BUFFER_COUNT	512

struct xmm_buffer xmm_buffers[XMM_BUFFER_COUNT];
xmm_buffer_t xmm_buffer_free_list;
queue_head_t xmm_buffer_lru;

xmm_buffer_t
xmm_buffer_get()
{
	register xmm_buffer_t buffer;
	kern_return_t kr;
	xmm_obj_t mobj;

	/*
	 * First check the free list.
	 */
	buffer = xmm_buffer_free_list;
	if (buffer != BUFFER_NULL) {
		xmm_buffer_free_list = buffer->next_free;
		return buffer;
	}

	/*
	 * There's nothing on the free list, so take the oldest element
	 * from the lru queue, if any.
	 */
	assert(!queue_empty(&xmm_buffer_lru));

	/*
	 * Remove buffer off of its obj queue as well, and write out its data.
	 */
	buffer = (xmm_buffer_t) queue_first(&xmm_buffer_lru);
	while (buffer->busy_count != 0) {
		buffer = (xmm_buffer_t) queue_next(&buffer->lruq);
		if (buffer == (xmm_buffer_t) &xmm_buffer_lru)
			return (BUFFER_NULL);
	}
	assert(buffer != BUFFER_NULL);
	assert(!buffer->free && buffer->busy_count == 0);
	buffer->busy_count++;
	return buffer;
}

void
xmm_buffer_free(buffer)
	register xmm_buffer_t buffer;
{
	buffer->free = TRUE;
	buffer->next_free = xmm_buffer_free_list;
	xmm_buffer_free_list = buffer;
}

xmm_buffer_init()
{
	int i;
	xmm_buffer_t buffer;

	queue_init(&xmm_buffer_lru);
	for (i = 0; i < XMM_BUFFER_COUNT; i++) {
		buffer = &xmm_buffers[i];
		queue_init(&buffer->mobjq);
		queue_init(&buffer->lruq);
		xmm_buffer_free(buffer);
	}
}

kern_return_t
xmm_buffer_create(old_mobj, new_mobj)
	xmm_obj_t old_mobj;
	xmm_obj_t *new_mobj;
{
	xmm_obj_t mobj;
	kern_return_t kr;
	
	kr = xmm_obj_allocate(&buffer_class, old_mobj, &mobj);
	if (kr != KERN_SUCCESS) {
		return kr;
	}
	queue_init(&MOBJ->buffers);
	MOBJ->ready = FALSE;
	*new_mobj = mobj;
	return KERN_SUCCESS;
}

kern_return_t
m_buffer_init(mobj, pagesize, internal, size)
	xmm_obj_t	mobj;
	vm_size_t	pagesize;
	boolean_t	internal;
	vm_size_t	size;
{
#ifdef	lint
	M_INIT(mobj, pagesize, internal, size);
#endif	lint
	assert(pagesize == PAGE_SIZE);
	M_INIT(mobj, pagesize, internal, size);
	return KERN_SUCCESS;
}

kern_return_t
m_buffer_terminate(mobj, release)
	xmm_obj_t	mobj;
	boolean_t 	release;
{
	xmm_buffer_t buffer;
	kern_return_t kr;
	vm_offset_t offset;
	vm_map_copy_t copy;
	boolean_t dirty;
	boolean_t kernel_copy;
	boolean_t use_routine;

#ifdef	lint
	M_TERMINATE(mobj, release);
#endif	lint
	dprintf("xmm_buffer_terminate\n");
	while (! queue_empty(&MOBJ->buffers)) {
		queue_remove_first(&MOBJ->buffers, buffer,xmm_buffer_t, mobjq);
		assert(buffer != BUFFER_NULL);
		if (buffer->busy_count != 0)
			panic("m_buffer_terminate: busy buffer");

		assert(!buffer->free);
		queue_remove(&xmm_buffer_lru, buffer, xmm_buffer_t, lruq);
		offset = buffer->offset;
		copy = buffer->copy;
		dirty = buffer->dirty;
		kernel_copy = buffer->kernel_copy;
		use_routine = buffer->use_routine;
		dprintf("dealloc 0x%x copy 0x%x\n", offset, copy);
		xmm_buffer_free(buffer);
		kr = M_DATA_WRT_RTN (mobj, offset, (vm_offset_t) copy,
				  PAGE_SIZE, dirty,
				  kernel_copy, use_routine);
		if (kr != KERN_SUCCESS) {
			/*
			 * XXX
			 * What do we do here? (eternal buffering problem)
			 */
			printf("xmm_buffer_terminate: kr=%d/0x%x\n", kr, kr);
		}
	}
	dprintf("terminate done\n");
	return M_TERMINATE(mobj, release);
}

void
m_buffer_deallocate(mobj)
	xmm_obj_t mobj;
{
}

xmm_buffer_t
xmm_buffer_lookup(mobj, offset)
	xmm_obj_t	mobj;
	vm_offset_t	offset;
{
	xmm_buffer_t buffer;

	/*
	 * Search through buffers associated with this mobj.
	 * There are typically very few buffers associated
	 * with any given object, so it's not worth having
	 * a hash table or anything tricky.
	 */
	queue_iterate(&MOBJ->buffers, buffer, xmm_buffer_t, mobjq) {
		if (buffer->offset == offset) {
			return buffer;
		}
	}
	return BUFFER_NULL;
}

#if 0
vm_page_t
xmm_buffer_find_page(object, offset)
	vm_object_t object;
	vm_offset_t offset;
{
	vm_page_t m;
	
	/*
	 *	Try to find the page of data.
	 */
	vm_object_lock(object);
	vm_object_paging_begin(object);
	m = vm_page_lookup(object, offset);
	if ((m != VM_PAGE_NULL) && !m->busy && !m->fictitious &&
	    !m->absent && !m->error) {
	} else {
		vm_prot_t result_prot;
		vm_page_t top_page;
		kern_return_t kr;
		
		for (;;) {
			result_prot = VM_PROT_READ;
			kr = vm_fault_page(object, offset,
					   VM_PROT_READ, FALSE, FALSE,
					   &result_prot, &m, &top_page,
					   0, FALSE);
			if (kr == VM_FAULT_MEMORY_SHORTAGE) {
				VM_PAGE_WAIT();
				vm_object_lock(object);
				vm_object_paging_begin(object);
				continue;
			}
			if (kr != VM_FAULT_SUCCESS) {
				/* XXX what about data_error? */
				vm_object_lock(object);
				vm_object_paging_begin(object);
				continue;
			}
			if (top_page != VM_PAGE_NULL) {
				vm_object_lock(object);
				VM_PAGE_FREE(top_page);
				vm_object_paging_end(object);
				vm_object_unlock(object);
			}
			break;
		}
	}
	assert(m);
	assert(! m->busy);
	vm_object_paging_end(object);
	vm_object_unlock(object);
	return m;
}
#else
/*
 * Find page at offset in object.
 * Returns page busy and with paging reference to its object.
 *
 * XXX
 * In the general form, we should return VM_PAGE_NULL if data_unavailable.
 * This requires a change to vm_fault_page interface.
 */
vm_page_t
xmm_buffer_find_page(object, offset)
	vm_object_t object;
	vm_offset_t offset;
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
	if (m != VM_PAGE_NULL && !m->busy && !m->fictitious && !m->absent &&
	    !m->error ) {
		m->busy = TRUE;
	} else {
		for (;;) {
			result_prot = VM_PROT_READ;
			kr = vm_fault_page(object, offset, VM_PROT_READ,
					   FALSE, FALSE, &result_prot, &m,
					   &top_page, 0, FALSE);
			if (kr == KERN_SUCCESS) {
				break;
			}
			switch (kr) {
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

#endif
vm_map_copy_t
xmm_buffer_copy(old_copy)
	vm_map_copy_t old_copy;
{
	vm_map_copy_t new_copy;
	vm_page_t old_m, new_m;
	extern zone_t vm_map_copy_zone;

	dprintf("xmm_buffer_copy 0x%x type %d\n", old_copy, old_copy->type);

	/*
	 * Allocate a new copy object.
	 */
	new_copy = (vm_map_copy_t) zalloc(vm_map_copy_zone);
	if (new_copy == VM_MAP_COPY_NULL) {
		panic("xmm_buffer_copy: zalloc");
	}
	new_copy->type = VM_MAP_COPY_PAGE_LIST;
	new_copy->cpy_npages = 1;
	new_copy->offset = 0;
	new_copy->size = PAGE_SIZE;
	new_copy->cpy_cont = ((kern_return_t (*)()) 0);
	new_copy->cpy_cont_args = (char *) VM_MAP_COPYIN_ARGS_NULL;

	/*
	 * Allocate a new page and insert it into new copy object.
	 */
	while ((new_m = vm_page_grab()) == VM_PAGE_NULL) {
		VM_PAGE_WAIT();
	}
	new_copy->cpy_page_list[0] = new_m;

	/*
	 * Find old page, and copy into new.
	 */
	assert(old_copy->size == PAGE_SIZE);
	assert(old_copy->offset == 0);
	if (old_copy->type == VM_MAP_COPY_PAGE_LIST) {
		old_m = old_copy->cpy_page_list[0];
		pmap_copy_page(old_m->phys_addr, new_m->phys_addr);
	} else {
		register vm_object_t	object = old_copy->cpy_object;

		assert(old_copy->type == VM_MAP_COPY_OBJECT);

		old_m = xmm_buffer_find_page(object, 0);
		pmap_copy_page(old_m->phys_addr, new_m->phys_addr);
		vm_object_lock(object);
		PAGE_WAKEUP_DONE(old_m);
		vm_object_paging_end(object);
		vm_object_unlock(object);
	}

	return new_copy;
}

kern_return_t
m_buffer_data_request(mobj, offset, length, desired_access)
	xmm_obj_t	mobj;
	vm_offset_t	offset;
	vm_size_t	length;
	vm_prot_t	desired_access;
{
	xmm_buffer_t buffer;
	vm_map_copy_t copy;

#ifdef	lint
	M_DATA_REQUEST(mobj, offset, length, desired_access);
#endif	lint
	/*
	 * If this page was not buffered, then pass the data request through.
	 */
	buffer = xmm_buffer_lookup(mobj, offset);
	if (buffer == BUFFER_NULL) {
		return M_DATA_REQUEST(mobj, offset, length,
				      desired_access);
	}

	/*
	 * The page was buffered. Move it to the front of the lru queue.
	 */
	queue_remove(&xmm_buffer_lru, buffer, xmm_buffer_t, lruq);
	queue_enter(&xmm_buffer_lru, buffer, xmm_buffer_t, lruq);
	assert(!buffer->free);
	buffer->busy_count++;

	/*
	 * This copy is unfortunate and could be avoided.
	 */
	copy = xmm_buffer_copy(buffer->copy);

	assert(!buffer->free && buffer->busy_count > 0);
	buffer->busy_count--;
	
	/*
	 * Return data.
	 */
	return K_DATA_SUPPLY(mobj, offset, (vm_offset_t) copy, length,
			     VM_PROT_NONE, FALSE, XMM_REPLY_NULL);
}

/*
 * Write data through to memory manager;
 * discard any corresponding buffered data.
 */
kern_return_t
m_buffer_data_write_return(mobj, offset, data, length,
			   dirty, kernel_copy, use_routine)
	xmm_obj_t	mobj;
	vm_offset_t	offset;
	vm_offset_t	data;
	vm_size_t	length;
	boolean_t	dirty;
	boolean_t	kernel_copy;
	int		use_routine;
{
	xmm_buffer_t buffer;

#ifdef	lint
	M_DATA_WRT_RTN (mobj, offset, data, length,
			dirty, kernel_copy, use_routine);
#endif	lint
	/*
	 * Find old data, if any.
	 */
	buffer = xmm_buffer_lookup(mobj, offset);
	if (buffer != BUFFER_NULL) {
		vm_map_copy_t copy = buffer->copy;

		if (use_routine == XMM_USE_DATA_INITIALIZE) {
			/*
			 *	Data is here, hence DATA_INITIALIZE
			 *	is a no-op.
			 */
			vm_map_copy_discard((vm_map_copy_t) data);
			return KERN_SUCCESS;
		}

		/*
		 *	New data overwrites old data
		 */
		if (buffer->busy_count != 0)
			panic("m_buffer_data_write_return: busy buffer");

		assert(!buffer->free);
		queue_remove(&MOBJ->buffers, buffer, xmm_buffer_t, mobjq);
		queue_remove(&xmm_buffer_lru, buffer, xmm_buffer_t, lruq);
		xmm_buffer_free(buffer);
		dprintf("discard copy=0x%x type=%d\n", copy, copy->type);
		vm_map_copy_discard(copy);
	}

	/*
	 * Write new data.
	 */
	return M_DATA_WRT_RTN (mobj, offset, data, PAGE_SIZE,
			dirty, kernel_copy, use_routine);
}

/*
 * Buffer data to be written;
 * replace any preexisting corresponding buffered data.
 */
m_buffer_data_write_buffered(mobj, offset, data, length,
		dirty, kernel_copy, use_routine)
	xmm_obj_t	mobj;
	vm_offset_t	offset;
	vm_offset_t	data;
	vm_size_t	length;
	boolean_t	dirty;
	boolean_t	kernel_copy;
	int		use_routine;
{
	xmm_buffer_t buffer;
	vm_map_copy_t copy = (vm_map_copy_t) data;
	kern_return_t kr;

#ifdef	lint
	M_DATA_WRT_RTN (mobj, offset, data, length,
		dirty, kernel_copy, use_routine);
#endif	lint
	/*
	 * Be assertive.
	 */
	assert(length == PAGE_SIZE);
	assert(copy->type == VM_MAP_COPY_OBJECT ||
	       copy->type == VM_MAP_COPY_PAGE_LIST);
	assert(copy->offset == 0);
	assert(copy->size == PAGE_SIZE);
	assert(use_routine != XMM_USE_DATA_INITIALIZE);

	/*
	 * Check to see whether we have old data for this page.
	 */
	buffer = xmm_buffer_lookup(mobj, offset);
	if (buffer != BUFFER_NULL) {
		/*
		 * Replace data in buffer.
		 */
		vm_map_copy_t old_copy = buffer->copy;
		buffer->copy = copy;
		buffer->dirty = dirty;
		buffer->kernel_copy = kernel_copy;
		buffer->use_routine = use_routine;
		dprintf("write_buffered: replace\n");

		/*
		 * Move buffer to head of lru queue.
		 */
		queue_remove(&xmm_buffer_lru, buffer, xmm_buffer_t, lruq);
		queue_enter(&xmm_buffer_lru, buffer, xmm_buffer_t, lruq);

		/*
		 * Discard data, and return.
		 */
		dprintf("replace copy=0x%x[type=%d] with copy=0x%x[%d]\n",
			 old_copy, old_copy->type,
			 copy, copy->type);
		vm_map_copy_discard(old_copy);
		return KERN_SUCCESS;
	}

	/*
	 * We don't have old data for this page, so allocate a new buffer
	 * and enter it in the queues.
	 */
	dprintf("write_buffered: new\n");
	buffer = xmm_buffer_get();
	if (buffer == BUFFER_NULL) {
		/*
		 *	Can't buffer, too bad.
		 */
		return M_DATA_WRT_RTN(mobj, offset, data, length,
				      dirty, kernel_copy, use_routine);
	}

	if (buffer->free) {
		/*
		 *	Easy case.  Just put the stuff in the
		 *	buffer and be done with it.
		 */
		assert(buffer != BUFFER_NULL);
		buffer->free = FALSE;
		buffer->mobj = mobj;
		buffer->copy = copy;
		buffer->offset = offset;
		buffer->dirty = dirty;
		buffer->kernel_copy = kernel_copy;
		buffer->use_routine = use_routine;
		queue_enter(&MOBJ->buffers, buffer, xmm_buffer_t, mobjq);
		queue_enter(&xmm_buffer_lru, buffer, xmm_buffer_t, lruq);
	}
	else {
		struct xmm_buffer temp_buffer;
		xmm_obj_t	new_mobj;

		/*
		 *	Hard case.  Have to swap buffer contents and write
		 *	out old contents, but leave new buffer in place
		 *	BEFORE write.  First copy the buffer we got
		 *	to a temporary.  xmm_buffer_get() marked the buffer
		 *	busy for us.
		 */
		assert(buffer->busy_count == 1);
		temp_buffer = *buffer;

		/*
		 *	Now fill the buffer with the new arguments.
		 */
		buffer->mobj = mobj;
		buffer->copy = copy;
		buffer->offset = offset;
		buffer->dirty = dirty;
		buffer->kernel_copy = kernel_copy;
		buffer->use_routine = use_routine;

		/*
		 *	Bang the buffer off and on the queues.
		 */
		queue_remove(&xmm_buffer_lru, buffer, xmm_buffer_t, lruq);
		mobj = temp_buffer.mobj;  /* still on old mobj's mobjq */
		queue_remove(&MOBJ->buffers, buffer, xmm_buffer_t, mobjq);

		mobj = buffer->mobj;  /* onto new mobj's mobjq */
		queue_enter(&MOBJ->buffers, buffer, xmm_buffer_t, mobjq);
		queue_enter(&xmm_buffer_lru, buffer, xmm_buffer_t, lruq);

		/*
		 *	The buffer is now usable.
		 */
		buffer->busy_count--;
		assert(buffer->busy_count == 0);

		/*
		 *	Write out contents of old buffer.  If this fails,
		 *	the data is lost.
		 */
		kr = M_DATA_WRT_RTN(temp_buffer.mobj, temp_buffer.offset,
				     (vm_offset_t) temp_buffer.copy,
				     PAGE_SIZE, temp_buffer.dirty,
				     temp_buffer.kernel_copy,
				     temp_buffer.use_routine);
		assert(kr == KERN_SUCCESS);
	}
	return KERN_SUCCESS;
}

kern_return_t
m_buffer_unbuffer_data(mobj, offset, length, should_clean, should_flush)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	boolean_t should_clean;
	boolean_t should_flush;
{
	xmm_buffer_t buffer;
	vm_map_copy_t copy;
	boolean_t dirty;
	boolean_t kernel_copy;
	int use_routine;

	/*
	 * If we have no data for this page, return.
	 */
	buffer = xmm_buffer_lookup(mobj, offset);
	if (buffer == BUFFER_NULL) {
		return KERN_SUCCESS;
	}
	
	/*
	 * Dequeue and free buffer.
	 */
	if (buffer->busy_count != 0)
		panic("m_buffer_unbuffer_data: busy buffer");

	assert(!buffer->free);
	copy = buffer->copy;
	dirty = buffer->dirty;
	kernel_copy = buffer->kernel_copy;
	use_routine = buffer->use_routine;
	queue_remove(&MOBJ->buffers, buffer, xmm_buffer_t, mobjq);
	queue_remove(&xmm_buffer_lru, buffer, xmm_buffer_t, lruq);
	xmm_buffer_free(buffer);

	/*
	 * Clean (write) or flush (deallocate) data.
	 */
	assert(should_clean || should_flush);
	dprintf("unbuffer: clean=%d flush=%d\n", should_clean, should_flush);
	if (should_clean) {
		return M_DATA_WRT_RTN(mobj, offset, (vm_offset_t) copy,
				     PAGE_SIZE, dirty, kernel_copy,
				     use_routine);
	} else {
		vm_map_copy_discard(copy);
		return KERN_SUCCESS;
	}
}

k_buffer_set_ready(kobj, object_ready, may_cache, copy_strategy,
		   use_routine, memory_object_name, reply)
	xmm_obj_t kobj;
	boolean_t object_ready;
	boolean_t may_cache;
	memory_object_copy_strategy_t copy_strategy;
	int use_routine;
	ipc_port_t memory_object_name;
	xmm_reply_t reply;
{
#ifdef	lint
	K_SET_READY(kobj, object_ready, may_cache, copy_strategy,
		    use_routine, memory_object_name, reply);
#endif	lint
	if (object_ready) {
		KOBJ->ready = TRUE;
	}
	K_SET_READY(kobj, object_ready, may_cache, copy_strategy,
		    use_routine, memory_object_name, reply);
	return KERN_SUCCESS;
}

/*
 * These won't work if we are interposing. Do we care?
 */

kern_return_t
M_BUFFERED_DATA_WRITE(mobj, offset, data, length,
		     dirty, kernel_copy, use_routine)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_offset_t data;
	vm_size_t length;
	boolean_t dirty;
	boolean_t kernel_copy;
	int use_routine;
{
	assert(use_routine != XMM_USE_DATA_INITIALIZE);

	if (mobj->m_mobj->class == &buffer_class) {
		return m_buffer_data_write_buffered(mobj->m_mobj, offset,
						    data, length, dirty, 
						    kernel_copy, use_routine);
	} else {
		return M_DATA_WRT_RTN (mobj, offset, data, length,
				      dirty, kernel_copy, use_routine);
	}
}

kern_return_t
M_UNBUFFER_DATA(mobj, offset, length, should_clean, should_flush)
	xmm_obj_t mobj;
	boolean_t should_clean;
	boolean_t should_flush;
	vm_offset_t offset;
	vm_size_t length;
{
	if (mobj->m_mobj->class == &buffer_class) {
		return m_buffer_unbuffer_data(mobj->m_mobj, offset, length,
					      should_clean, should_flush);
	} else {
		return KERN_SUCCESS;
	}
}

#include <sys/varargs.h>

int xmm_buffer_debug = 0;
extern cnputc();

/* VARARGS */
xmm_buffer_dprintf(fmt, va_alist)
	char *fmt;
	va_dcl
{
	va_list	listp;

	if (xmm_buffer_debug) {
		va_start(listp);
 		_doprnt(fmt, &listp, cnputc, 0);
		va_end(listp);
	}
}
