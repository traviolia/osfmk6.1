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
 * Revision 2.25.2.5  92/09/15  17:35:52  jeffreyh
 * 	Put NORMA_IPC code under NORMA_IPC conditional
 * 	[92/08/25            jeffreyh]
 * 
 * 	m_o_data_error now saves the reason into the vm_page structure.
 * 	[92/07/28            sjs]
 * 
 * 	Changed memory_object_data_supply to convert an incoming
 * 	object flavor copy object to a page list flavor copy object
 * 	when needed.  Added assertions checking vm map copy object
 * 	type and state.  Bug fixes to mo_data_supply:  can't assume
 * 	at end of copy loop that the resulting data_copy is valid;
 * 	can't assume at function exit that data_copy is valid.
 * 	[92/06/02            alanl]
 * 
 * Revision 2.25.2.4  92/03/03  16:25:43  jeffreyh
 * 	Bug fix to avoid duplicate vm_map_copy_t deallocate in
 * 	error case of memory_object_data_supply.
 * 	[92/02/19  17:38:17  dlb]
 * 	Merge in changes from dlb
 * 	[92/02/26  12:33:36  jeffreyh]
 * 
 * 	Merge up to TRUNK
 * 	[92/02/25  17:10:19  jeffreyh]
 * 
 * Revision 2.25.2.3.2.1  92/02/25  17:00:35  jeffreyh
 * 	[David L. Black 92/02/22  17:05:17  dlb@osf.org]
 * 	Implement no change of page lock functionality in
 * 	memory_object_lock_request.
 * 
 * Revision 2.26  92/01/23  15:21:33  rpd
 * 	Fixed memory_object_change_attributes.
 * 	Created memory_object_set_attributes_common.
 * 	[92/01/18            rpd]
 * 
 * Revision 2.25.2.3  92/02/21  11:28:38  jsb
 * 	Add MEMORY_OBJECT_COPY_TEMPORARY case to
 * 	memory_object_set_attributes_common.
 * 	[92/02/11  11:47:45  jsb]
 * 
 * 	NORMA_VM: don't define cover and backwards compatibility routines.
 * 	[92/02/09  14:21:20  jsb]
 * 
 * Revision 2.25.2.2  92/02/18  19:20:01  jeffreyh
 * 	Cleaned up incorrect comment
 * 	[92/02/18            jeffreyh]
 * 
 * Revision 2.25.2.1  92/01/21  21:55:10  jsb
 * 	Created memory_object_set_attributes_common which provides
 * 	functionality common to memory_object_set_attributes,
 * 	memory_object_ready, and memory_object_change_attributes.
 * 	Fixed memory_object_change_attributes to set use_old_pageout
 * 	false instead of true.
 * 	[92/01/21  18:40:20  jsb]
 * 
 * Revision 2.25  91/10/09  16:19:18  af
 * 	Fixed assertion in memory_object_data_supply.
 * 	[91/10/06            rpd]
 * 
 * 	Added vm_page_deactivate_hint.
 * 	[91/09/29            rpd]
 * 
 * Revision 2.24  91/08/28  11:17:50  jsb
 * 	Continuation bug fix.
 * 	[91/08/05  17:42:57  dlb]
 * 
 * 	Add vm_map_copy continuation support to memory_object_data_supply.
 * 	[91/07/30  14:13:59  dlb]
 * 
 * 	Turn on page lists by default: gut and remove body of memory_object_
 * 	data_provided -- it's now a wrapper for memory_object_data_supply.
 * 	Precious page support:
 * 	    Extensive modifications to to memory_object_lock_{request,page}
 * 	    Implement old version wrapper xxx_memory_object_lock_request
 * 	    Maintain use_old_pageout field in vm objects.
 * 	    Add memory_object_ready, memory_object_change_attributes.
 * 	[91/07/03  14:11:35  dlb]
 * 
 * Revision 2.23  91/08/03  18:19:51  jsb
 * 	For now, use memory_object_data_supply iff NORMA_IPC.
 * 	[91/07/04  13:14:50  jsb]
 * 
 * Revision 2.22  91/07/31  18:20:56  dbg
 * 	Removed explicit data_dealloc argument to
 * 	memory_object_data_supply.  MiG now handles a user-specified
 * 	dealloc flag.
 * 	[91/07/29            dbg]
 * 
 * Revision 2.21  91/07/01  09:17:25  jsb
 * 	Fixed remaining MACH_PORT references.
 * 
 * Revision 2.20  91/07/01  08:31:54  jsb
 * 	Changed mach_port_t to ipc_port_t in memory_object_data_supply.
 * 
 * Revision 2.19  91/07/01  08:26:53  jsb
 * 	21-Jun-91 David L. Black (dlb) at Open Software Foundation
 * 	Add memory_object_data_supply.
 * 	[91/06/29  16:36:43  jsb]
 * 
 * Revision 2.18  91/06/25  11:06:45  rpd
 * 	Fixed includes to avoid norma files unless they are really needed.
 * 	[91/06/25            rpd]
 * 
 * Revision 2.17  91/06/25  10:33:04  rpd
 * 	Changed memory_object_t to ipc_port_t where appropriate.
 * 	[91/05/28            rpd]
 * 
 * Revision 2.16  91/06/17  15:48:55  jsb
 * 	NORMA_VM: include xmm_server_rename.h, for interposition.
 * 	[91/06/17  11:09:52  jsb]
 * 
 * Revision 2.15  91/05/18  14:39:33  rpd
 * 	Fixed memory_object_lock_page to handle fictitious pages.
 * 	[91/04/06            rpd]
 * 	Changed memory_object_data_provided, etc,
 * 	to allow for fictitious pages.
 * 	[91/03/29            rpd]
 * 	Added vm/memory_object.h.
 * 	[91/03/22            rpd]
 * 
 * Revision 2.14  91/05/14  17:47:54  mrt
 * 	Correcting copyright
 * 
 * Revision 2.13  91/03/16  15:04:30  rpd
 * 	Removed the old version of memory_object_data_provided.
 * 	[91/03/11            rpd]
 * 	Fixed memory_object_data_provided to return success
 * 	iff it consumes the copy object.
 * 	[91/02/10            rpd]
 * 
 * Revision 2.12  91/02/05  17:57:25  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:30:46  mrt]
 * 
 * Revision 2.11  91/01/08  16:44:11  rpd
 * 	Added continuation argument to thread_block.
 * 	[90/12/08            rpd]
 * 
 * Revision 2.10  90/10/25  14:49:30  rwd
 * 	Clean and not flush pages to lock request get moved to the
 * 	inactive queue.
 * 	[90/10/24            rwd]
 * 
 * Revision 2.9  90/08/27  22:15:49  dbg
 * 	Fix error in initial assumptions: vm_pageout_setup must take a
 * 	BUSY page, to prevent the page from being scrambled by pagein.
 * 	[90/07/26            dbg]
 * 
 * Revision 2.8  90/08/06  15:08:16  rwd
 * 	Fix locking problems in memory_object_lock_request.
 * 	[90/07/12            rwd]
 * 	Fix memory_object_lock_request to only send contiguous pages as
 * 	one message.  If dirty pages were seperated by absent pages,
 * 	then the wrong thing was done.
 * 	[90/07/11            rwd]
 * 
 * Revision 2.7  90/06/19  23:01:38  rpd
 * 	Bring old single_page version of memory_object_data_provided up
 * 	to date.
 * 	[90/06/05            dbg]
 * 
 * 	Correct object locking in memory_object_lock_request.
 * 	[90/06/05            dbg]
 * 
 * Revision 2.6  90/06/02  15:10:14  rpd
 * 	Changed memory_object_lock_request/memory_object_lock_completed calls
 * 	to allow both send and send-once right reply-to ports.
 * 	[90/05/31            rpd]
 * 
 * 	Added memory_manager_default_port.
 * 	[90/04/29            rpd]
 * 	Converted to new IPC.  Purged MACH_XP_FPD.
 * 	[90/03/26  23:11:14  rpd]
 * 
 * Revision 2.5  90/05/29  18:38:29  rwd
 * 	New memory_object_lock_request from dbg.
 * 	[90/05/18  13:04:36  rwd]
 * 
 * 	Picked up rfr MACH_PAGEMAP changes.
 * 	[90/04/12  13:45:43  rwd]
 * 
 * Revision 2.4  90/05/03  15:58:23  dbg
 * 	Pass should_flush to vm_pageout_page: don't flush page if not
 * 	requested.
 * 	[90/03/28            dbg]
 * 
 * Revision 2.3  90/02/22  20:05:10  dbg
 * 	Pick up changes from mainline:
 * 
 * 		Fix placeholder page handling in memory_object_data_provided.
 * 		Old code was calling zalloc while holding a lock.
 * 		[89/12/13  19:58:28  dlb]
 * 
 * 		Don't clear busy flags on any pages in memory_object_lock_page
 * 		(from memory_object_lock_request)!!  Implemented by changing
 * 		PAGE_WAKEUP to not clear busy flag and using PAGE_WAKEUP_DONE
 * 		when it must be cleared.  See vm/vm_page.h.  With dbg's help.
 * 		[89/12/13            dlb]
 * 
 * 		Don't activate fictitious pages after freeing them in
 * 		memory_object_data_{unavailable,error}.  Add missing lock and
 * 		unlock of page queues when activating pages in same routines.
 * 		[89/12/11            dlb]
 * 		Retry lookup after using CLEAN_DIRTY_PAGES in
 * 		memory_object_lock_request().  Also delete old version of
 * 		memory_object_data_provided().  From mwyoung.
 * 		[89/11/17            dlb]
 * 
 * 		Save all page-cleaning operations until it becomes necessary
 * 		to block in memory_object_lock_request().
 * 		[89/09/30  18:07:16  mwyoung]
 * 
 * 		Split out a per-page routine for lock requests.
 * 		[89/08/20  19:47:42  mwyoung]
 * 
 * 		Verify that the user memory used in
 * 		memory_object_data_provided() is valid, even if it won't
 * 		be used to fill a page request.
 * 		[89/08/01  14:58:21  mwyoung]
 * 
 * 		Make memory_object_data_provided atomic, interruptible,
 * 		and serializable when handling multiple pages.  Add
 * 		optimization for single-page operations.
 * 		[89/05/12  16:06:13  mwyoung]
 * 
 * 		Simplify lock/clean/flush sequences memory_object_lock_request.
 * 		Correct error in call to pmap_page_protect() there.
 * 		Make error/absent pages eligible for pageout.
 * 		[89/04/22            mwyoung]
 * 
 * Revision 2.2  89/09/08  11:28:10  dbg
 * 	Pass keep_wired argument to vm_move.  Disabled
 * 	host_set_memory_object_default.
 * 	[89/07/14            dbg]
 * 
 * 28-Apr-89  David Golub (dbg) at Carnegie-Mellon University
 *	Clean up fast_pager_data option.  Remove pager_data_provided_inline.
 *
 * Revision 2.18  89/04/23  13:25:30  gm0w
 * 	Fixed typo to pmap_page_protect in memory_object_lock_request().
 * 	[89/04/23            gm0w]
 * 
 * Revision 2.17  89/04/22  15:35:09  gm0w
 * 	Commented out check/uprintf if memory_object_data_unavailable
 * 	was called on a permanent object.
 * 	[89/04/14            gm0w]
 * 
 * Revision 2.16  89/04/18  21:24:24  mwyoung
 * 	Recent history:
 * 	 	Add vm_set_default_memory_manager(),
 * 		 memory_object_get_attributes().
 * 		Whenever asked to clean a page, use pmap_is_modified, even
 * 		 if not flushing the data.
 * 		Handle fictitious pages when accepting data (or error or
 * 		 unavailable).
 * 		Avoid waiting in memory_object_data_error().
 * 
 * 	Previous history has been integrated into the documentation below.
 * 	[89/04/18            mwyoung]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
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
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 */
/*
 *	File:	vm/memory_object.c
 *	Author:	Michael Wayne Young
 *
 *	External memory management interface control functions.
 */

#include <mor.h>
#include <norma_vm.h>
#include <norma_ipc.h>
#include <mach_pagemap.h>

/*
 *	Interface dependencies:
 */

#include <mach/std_types.h>	/* For pointer_t */
#include <mach/mach_types.h>

#include <mach/kern_return.h>
#include <vm/vm_object.h>
#include <mach/memory_object.h>
#include <mach/memory_object_user.h>
#include <mach/memory_object_default.h>
#include <mach/mach_server.h>
#include <mach/boolean.h>
#include <mach/vm_prot.h>
#include <mach/message.h>

/*
 *	Implementation dependencies:
 */
#include <string.h>		/* For memcpy() */

#include <vm/memory_object.h>
#include <vm/vm_page.h>
#include <vm/vm_pageout.h>
#include <vm/pmap.h>		/* For pmap_clear_modify */
#include <kern/thread.h>	/* For current_thread() */
#include <kern/host.h>
#include <vm/vm_kern.h>		/* For kernel_map, vm_move */
#include <vm/vm_map.h>		/* For vm_map_pageable */
#include <ipc/ipc_port.h>
#include <ipc/ipc_space.h>

#include <kern/misc_protos.h>

#if	NORMA_VM
#include <norma/xmm_server_rename.h>
#endif	/* NORMA_VM */
#if	MACH_PAGEMAP
#include <vm/vm_external.h>	
#endif	/* MACH_PAGEMAP */

static ipc_port_t	memory_manager_default = IP_NULL;
mutex_t			memory_manager_default_lock;

/*
 *	Important note:
 *		All of the external (user interface) routines gain a reference
 *		to the object (first argument) as part of the automatic
 *		argument conversion. Explicit deallocation is necessary.
 */

#if	!NORMA_VM
/*
 *	If successful, destroys the map copy object.
 */
kern_return_t
memory_object_data_provided(
	vm_object_t	object,
	vm_offset_t	offset,
	pointer_t	data,
	unsigned int	data_cnt,
	vm_prot_t	lock_value)
{
        return memory_object_data_supply(object, offset, data,
					 data_cnt, lock_value, FALSE, IP_NULL,
					 0);
}
#endif	/* !NORMA_VM */

kern_return_t
memory_object_data_supply(
	register
        vm_object_t		object,
	register
	vm_offset_t		offset,
	vm_offset_t		data,
	mach_msg_type_number_t	data_cnt,
	vm_prot_t		lock_value,
	boolean_t		precious,
	ipc_port_t		reply_to,
	mach_msg_type_name_t	reply_to_type)
{
	vm_map_copy_t	data_copy = (vm_map_copy_t)data;
	kern_return_t	result = KERN_SUCCESS;
	vm_offset_t	error_offset = 0;
	register
	vm_page_t	m;
	register
	vm_page_t	data_m;
	vm_size_t	original_length;
	vm_offset_t	original_offset;
	vm_page_t	*page_list;
	boolean_t	was_absent;
	vm_map_copy_t	orig_copy = data_copy;

	/*
	 *	Look for bogus arguments
	 */

	if (object == VM_OBJECT_NULL) {
		return(KERN_INVALID_ARGUMENT);
	}

	if (lock_value & ~VM_PROT_ALL) {
		vm_object_deallocate(object);
		return(KERN_INVALID_ARGUMENT);
	}

	if (! page_aligned(data_cnt)) {
	    vm_object_deallocate(object);
	    return(KERN_INVALID_ARGUMENT);
	}

	/*
	 *	Adjust the offset from the memory object to the offset
	 *	within the vm_object.
	 */

	original_length = data_cnt;
	original_offset = offset;

#if	NORMA_IPC
	if (data_copy->type != VM_MAP_COPY_PAGE_LIST) {
		assert(data_copy->type == VM_MAP_COPY_OBJECT);
		result = vm_map_object_to_page_list(&data_copy);
		orig_copy = data_copy;
		assert(result == KERN_SUCCESS);
	}
#endif	/* NORMA_IPC */
	assert(data_copy->type == VM_MAP_COPY_PAGE_LIST);
	page_list = &data_copy->cpy_page_list[0];

	vm_object_lock(object);
	vm_object_paging_begin(object);
	offset -= object->paging_offset;

	/*
	 *	Loop over copy stealing pages for pagein.
	 */

	for (; data_cnt > 0 ; data_cnt -= PAGE_SIZE, offset += PAGE_SIZE) {

		assert(data_copy != VM_MAP_COPY_NULL);
		assert(data_copy->type == VM_MAP_COPY_PAGE_LIST);
		assert(data_copy->cpy_npages > 0);
		data_m = *page_list;

		assert( !(data_m == VM_PAGE_NULL || data_m->tabled ||
		    data_m->error || data_m->absent || data_m->fictitious ||
		    data_m->restart));

		/*
		 *	Look up target page and check its state.
		 */

retry_lookup:
		m = vm_page_lookup(object,offset);
		if (m == VM_PAGE_NULL) {
		    was_absent = FALSE;
		}
		else {
		    if (m->absent) {

			/*
			 *	Page was requested.  Free the busy
			 *	page waiting for it.  Insertion
			 *	of new page happens below.
			 */

			if (m->busy)
			    VM_PAGE_FREE(m);
			was_absent = TRUE;
		    }
		    else {

			/*
			 *	Have to wait for page that is busy and
			 *	not absent.  This is probably going to
			 *	be an error, but go back and check.
			 */
			if (m->busy) {
				PAGE_ASSERT_WAIT(m, FALSE);
				vm_object_unlock(object);
				thread_block((void (*)(void))0);
				vm_object_lock(object);
				goto retry_lookup;
			}

			/*
			 *	Page already present; error.
			 *	This is an error if data is precious.
			 */
			result = KERN_MEMORY_PRESENT;
			error_offset = offset + object->paging_offset;

			break;
		    }
		}

		/*
		 *	Ok to pagein page.  Target object now has no page
		 *	at offset.  Set the page parameters, then drop
		 *	in new page and set up pageout state.  Object is
		 *	still locked here.
		 *
		 *	Must clear busy bit in page before inserting it.
		 *	Ok to skip wakeup logic because nobody else
		 *	can possibly know about this page.
		 */

		data_m->lock_supplied = TRUE;
		data_m->busy = FALSE;
		data_m->dirty = FALSE;
		pmap_clear_modify(data_m->phys_addr);

		data_m->page_lock = lock_value;
		if (lock_value != VM_PROT_NONE)
			data_m->unusual = TRUE;
		else
			data_m->unusual = FALSE;

		data_m->unlock_request = VM_PROT_NONE;
		data_m->precious = precious;

		vm_page_lock_queues();
		vm_page_insert(data_m, object, offset);

		if (was_absent)
			vm_page_activate(data_m);
		else
			vm_page_deactivate(data_m);

		vm_page_unlock_queues();

		/*
		 *	Null out this page list entry, and advance to next
		 *	page.
		 */

		*page_list++ = VM_PAGE_NULL;

		if (--(data_copy->cpy_npages) == 0 &&
		    vm_map_copy_has_cont(data_copy)) {
			vm_map_copy_t	new_copy;

			vm_object_unlock(object);
			vm_map_copy_invoke_cont(data_copy, &new_copy, &result);
			vm_object_lock(object);

			if (result == KERN_SUCCESS) {

			    /*
			     *	Consume on success requires that
			     *	we keep the original vm_map_copy
			     *	around in case something fails.
			     *	Free the old copy if it's not the original
			     */
			    if (data_copy != orig_copy) {
				vm_map_copy_discard(data_copy);
			    }

			    if ((data_copy = new_copy) != VM_MAP_COPY_NULL)
				page_list = &data_copy->cpy_page_list[0];
			}
			else {
			    error_offset = offset + object->paging_offset +
						PAGE_SIZE;
			    break;
			}
		}
	}

	/*
	 *	Send reply if one was requested.
	 */
	vm_object_paging_end(object);
	vm_object_unlock(object);

	if (data_copy != VM_MAP_COPY_NULL &&
	    vm_map_copy_has_cont(data_copy))
		vm_map_copy_abort_cont(data_copy);

	if (IP_VALID(reply_to)) {
#if	NORMA_VM
		panic("XMM does not support memory_object_supply_completed\n");
#else	/* NORMA_VM */
		memory_object_supply_completed(
				reply_to, reply_to_type,
				object->pager_request,
				original_offset,
				original_length,
				result,
				error_offset);
#endif	/* NORMA_VM */
	}

	vm_object_deallocate(object);

	/*
	 *	Consume on success:  The final data copy must be
	 *	discarded if it is not the original.  The original
	 *	gets discarded only if this routine succeeds.
	 */
	if (data_copy != VM_MAP_COPY_NULL &&
	    data_copy != orig_copy)
		vm_map_copy_discard(data_copy);
	if (result == KERN_SUCCESS)
		vm_map_copy_discard(orig_copy);


	return(result);
}

kern_return_t
memory_object_data_error(
	vm_object_t	object,
	vm_offset_t	offset,
	vm_size_t	size,
	kern_return_t	error_value)
{
	if (object == VM_OBJECT_NULL)
		return(KERN_INVALID_ARGUMENT);

	if (size != round_page(size)) {
		vm_object_deallocate(object);
		return(KERN_INVALID_ARGUMENT);
	}

	vm_object_lock(object);
	offset -= object->paging_offset;

	while (size != 0) {
		register vm_page_t m;

		m = vm_page_lookup(object, offset);
		if ((m != VM_PAGE_NULL) && m->busy && m->absent) {
			if (error_value == KERN_MEMORY_DATA_MOVED) {
				m->restart = TRUE;
			} else {
				if (error_value > KERN_RETURN_MAX) {
					m->page_error = error_value;
				} else {
					m->page_error = KERN_MEMORY_ERROR;
				}
				m->error = TRUE;
			}

			/*
			 *	m->unusual was true because page was absent.
			 *	It remains true because page is either error
			 *	or restart.
			 */
			m->absent = FALSE;
			vm_object_absent_release(object);

			PAGE_WAKEUP_DONE(m);

			vm_page_lock_queues();
			vm_page_activate(m);
			vm_page_unlock_queues();
		}

		size -= PAGE_SIZE;
		offset += PAGE_SIZE;
	 }
	vm_object_unlock(object);

	vm_object_deallocate(object);
	return(KERN_SUCCESS);
}

kern_return_t
memory_object_data_unavailable(
	vm_object_t	object,
	vm_offset_t	offset,
	vm_size_t	size)
{
	if (object == VM_OBJECT_NULL)
		return(KERN_INVALID_ARGUMENT);

	if (size != round_page(size)) {
		vm_object_deallocate(object);
		return(KERN_INVALID_ARGUMENT);
	}

	vm_object_lock(object);
	offset -= object->paging_offset;

	while (size != 0) {
		register vm_page_t m;

		/*
		 *	We're looking for pages that are both busy and
		 *	absent (waiting to be filled), converting them
		 *	to just absent.
		 *
		 *	Pages that are just busy can be ignored entirely.
		 */

		m = vm_page_lookup(object, offset);
		if ((m != VM_PAGE_NULL) && m->busy && m->absent) {
			PAGE_WAKEUP_DONE(m);

			vm_page_lock_queues();
			vm_page_activate(m);
			vm_page_unlock_queues();
		}
		size -= PAGE_SIZE;
		offset += PAGE_SIZE;
	}

	vm_object_unlock(object);

	vm_object_deallocate(object);
	return(KERN_SUCCESS);
}

/*
 *	Routine:	memory_object_should_return_page
 *
 *	Description:
 *		Determine whether the given page should be returned,
 *		based on the page's state and on the given return policy.
 *
 *		We should return the page if one of the following is true:
 *
 *		1. Page is dirty and should_return is not RETURN_NONE.
 *		2. Page is precious and should_return is RETURN_ALL.
 *		3. Should_return is RETURN_ANYTHING.
 *
 *		As a side effect, m->dirty will be made consistent
 *		with pmap_is_modified(m), if should_return is not
 *		MEMORY_OBJECT_RETURN_NONE.
 */

#define	memory_object_should_return_page(m, should_return) \
    (should_return != MEMORY_OBJECT_RETURN_NONE && \
     (((m)->dirty || ((m)->dirty = pmap_is_modified((m)->phys_addr))) || \
      ((m)->precious && (should_return) == MEMORY_OBJECT_RETURN_ALL) || \
      (should_return) == MEMORY_OBJECT_RETURN_ANYTHING))

typedef	int	memory_object_lock_result_t;

#define MEMORY_OBJECT_LOCK_RESULT_DONE          0
#define MEMORY_OBJECT_LOCK_RESULT_MUST_BLOCK    1
#define MEMORY_OBJECT_LOCK_RESULT_MUST_CLEAN    2
#define MEMORY_OBJECT_LOCK_RESULT_MUST_RETURN   3

memory_object_lock_result_t memory_object_lock_page(
				vm_page_t		m,
				memory_object_return_t	should_return,
				boolean_t		should_flush,
				vm_prot_t		prot);

/*
 *	Routine:	memory_object_lock_page
 *
 *	Description:
 *		Perform the appropriate lock operations on the
 *		given page.  See the description of
 *		"memory_object_lock_request" for the meanings
 *		of the arguments.
 *
 *		Returns an indication that the operation
 *		completed, blocked, or that the page must
 *		be cleaned.
 */
memory_object_lock_result_t
memory_object_lock_page(
	vm_page_t		m,
	memory_object_return_t	should_return,
	boolean_t		should_flush,
	vm_prot_t		prot)
{
	/*
	 *	Don't worry about pages for which the kernel
	 *	does not have any data.
	 */

	if (m->absent || m->error || m->restart)
		return(MEMORY_OBJECT_LOCK_RESULT_DONE);

	/*
	 *	If we cannot change access to the page,
	 *	either because a mapping is in progress
	 *	(busy page) or because a mapping has been
	 *	wired, then give up.
	 */

	if (m->busy)
		return(MEMORY_OBJECT_LOCK_RESULT_MUST_BLOCK);

	assert(!m->fictitious);

	if (m->wire_count != 0) {
		/*
		 *	If no change would take place
		 *	anyway, return successfully.
		 *
		 *	No change means:
		 *		Not flushing AND
		 *		No change to page lock [2 checks]  AND
		 *		Should not return page
		 *
		 * XXX	This doesn't handle sending a copy of a wired
		 * XXX	page to the pager, but that will require some
		 * XXX	significant surgery.
		 */
		if (!should_flush &&
		    (m->page_lock == prot || prot == VM_PROT_NO_CHANGE) &&
		    ! memory_object_should_return_page(m, should_return)) {

			/*
			 *	Restart page unlock requests,
			 *	even though no change took place.
			 *	[Memory managers may be expecting
			 *	to see new requests.]
			 */
			m->unlock_request = VM_PROT_NONE;
			PAGE_WAKEUP(m);

			return(MEMORY_OBJECT_LOCK_RESULT_DONE);
		}

		return(MEMORY_OBJECT_LOCK_RESULT_MUST_BLOCK);
	}

	/*
	 *	If the page is to be flushed, allow
	 *	that to be done as part of the protection.
	 */

	if (should_flush)
		prot = VM_PROT_ALL;

	/*
	 *	Set the page lock.
	 *
	 *	If we are decreasing permission, do it now;
	 *	let the fault handler take care of increases
	 *	(pmap_page_protect may not increase protection).
	 */

	if (prot != VM_PROT_NO_CHANGE) {
		if ((m->page_lock ^ prot) & prot) {
			pmap_page_protect(m->phys_addr, VM_PROT_ALL & ~prot);
		}
		m->page_lock = prot;
		m->lock_supplied = TRUE;
		if (prot != VM_PROT_NONE)
			m->unusual = TRUE;
		else
			m->unusual = FALSE;

		/*
		 *	Restart any past unlock requests, even if no
		 *	change resulted.  If the manager explicitly
		 *	requested no protection change, then it is assumed
		 *	to be remembering past requests.
		 */

		m->unlock_request = VM_PROT_NONE;
		PAGE_WAKEUP(m);
	}

	/*
	 *	Handle page returning.
	 */

	if (memory_object_should_return_page(m, should_return)) {

		/*
		 *	If we weren't planning
		 *	to flush the page anyway,
		 *	we may need to remove the
		 *	page from the pageout
		 *	system and from physical
		 *	maps now.
		 */
		
		vm_page_lock_queues();
		VM_PAGE_QUEUES_REMOVE(m);
		vm_page_unlock_queues();

		if (!should_flush)
			pmap_page_protect(m->phys_addr, VM_PROT_NONE);

		if (m->dirty)
			return(MEMORY_OBJECT_LOCK_RESULT_MUST_CLEAN);
		else
			return(MEMORY_OBJECT_LOCK_RESULT_MUST_RETURN);
	}

	/*
	 *	Handle flushing
	 */

	if (should_flush) {
		VM_PAGE_FREE(m);
	} else {
		extern boolean_t vm_page_deactivate_hint;

		/*
		 *	XXX Make clean but not flush a paging hint,
		 *	and deactivate the pages.  This is a hack
		 *	because it overloads flush/clean with
		 *	implementation-dependent meaning.  This only
		 *	happens to pages that are already clean.
		 */

		if (vm_page_deactivate_hint &&
		    (should_return != MEMORY_OBJECT_RETURN_NONE)) {
			vm_page_lock_queues();
			vm_page_deactivate(m);
			vm_page_unlock_queues();
		}
	}

	return(MEMORY_OBJECT_LOCK_RESULT_DONE);
}

/*
 *	Routine:	memory_object_lock_request [user interface]
 *
 *	Description:
 *		Control use of the data associated with the given
 *		memory object.  For each page in the given range,
 *		perform the following operations, in order:
 *			1)  restrict access to the page (disallow
 *			    forms specified by "prot");
 *			2)  return data to the manager (if "should_return"
 *			    is RETURN_DIRTY and the page is dirty, or
 * 			    "should_return" is RETURN_ALL and the page
 *			    is either dirty or precious); and,
 *			3)  flush the cached copy (if "should_flush"
 *			    is asserted).
 *		The set of pages is defined by a starting offset
 *		("offset") and size ("size").  Only pages with the
 *		same page alignment as the starting offset are
 *		considered.
 *
 *		A single acknowledgement is sent (to the "reply_to"
 *		port) when these actions are complete.  If successful,
 *		the naked send right for reply_to is consumed.
 */

kern_return_t
memory_object_lock_request(
	register vm_object_t	object,
	register vm_offset_t	offset,
	register vm_size_t	size,
	memory_object_return_t	should_return,
	boolean_t		should_flush,
	vm_prot_t		prot,
	ipc_port_t		reply_to,
	mach_msg_type_name_t	reply_to_type)
{
	register vm_page_t	m;
	vm_page_t		holding_page;
	vm_offset_t		original_offset = offset;
	vm_size_t		original_size = size;
	vm_offset_t		paging_offset = 0;
	vm_object_t		new_object = VM_OBJECT_NULL;
	vm_offset_t		new_offset = 0;
	vm_offset_t		last_offset = offset;
	memory_object_lock_result_t	page_lock_result;
	memory_object_lock_result_t	pageout_action;
	vm_page_t		holding_pages[DATA_WRITE_MAX];

	/*
	 *	Check for bogus arguments.
	 */
	if (object == VM_OBJECT_NULL)
		return (KERN_INVALID_ARGUMENT);
	if ((prot & ~VM_PROT_ALL) != 0 && prot != VM_PROT_NO_CHANGE) {
		vm_object_deallocate(object);
		return (KERN_INVALID_ARGUMENT);
	}

	size = round_page(size);

	/*
	 *	Lock the object, and acquire a paging reference to
	 *	prevent the memory_object and control ports from
	 *	being destroyed.
	 */

	vm_object_lock(object);
	vm_object_paging_begin(object);
	offset -= object->paging_offset;

	/*
	 *	To avoid blocking while scanning for pages, save
	 *	dirty pages to be cleaned all at once.
	 *
	 *	XXXO A similar strategy could be used to limit the
	 *	number of times that a scan must be restarted for
	 *	other reasons.  Those pages that would require blocking
	 *	could be temporarily collected in another list, or
	 *	their offsets could be recorded in a small array.
	 */

	/*
	 * XXX	NOTE: May want to consider converting this to a page list
	 * XXX	vm_map_copy interface.  Need to understand object
	 * XXX	coalescing implications before doing so.
	 */

	for (;
	     size != 0;
	     size -= PAGE_SIZE, offset += PAGE_SIZE)
	{
	    /*
	     *	Limit the number of pages to be cleaned at once.
	     */
	    if (new_object != VM_OBJECT_NULL &&
		    new_offset >= PAGE_SIZE * DATA_WRITE_MAX)
	    {
 		PAGEOUT_PAGES(object, new_object, new_offset, pageout_action,
				paging_offset);
	    }

	    while ((m = vm_page_lookup(object, offset)) != VM_PAGE_NULL) {
		switch ((page_lock_result = memory_object_lock_page(m,
					should_return,
					should_flush,
					prot)))
		{
		    case MEMORY_OBJECT_LOCK_RESULT_DONE:
			/*
			 *	End of a cluster of dirty pages.
			 */
			if (new_object != VM_OBJECT_NULL) {
 	    		    PAGEOUT_PAGES(object, new_object, new_offset,
					pageout_action, paging_offset);
			    continue;
			}
			break;

		    case MEMORY_OBJECT_LOCK_RESULT_MUST_BLOCK:
			/*
			 *	Since it is necessary to block,
			 *	clean any dirty pages now.
			 */
			if (new_object != VM_OBJECT_NULL) {
 			    PAGEOUT_PAGES(object, new_object, new_offset,
					pageout_action, paging_offset);
			    continue;
			}

			PAGE_ASSERT_WAIT(m, FALSE);
			vm_object_unlock(object);
			thread_block((void (*)(void))0);
			vm_object_lock(object);
			continue;

		    case MEMORY_OBJECT_LOCK_RESULT_MUST_CLEAN:
		    case MEMORY_OBJECT_LOCK_RESULT_MUST_RETURN:
			/*
			 * The clean and return cases are similar.
			 *
			 * Mark the page busy since we unlock the
			 * object below.
			 */
			m->busy = TRUE;

			/*
			 * if this would form a discontiguous block,
			 * clean the old pages and start anew.
			 *
			 * NOTE: The first time through here, new_object
			 * is null, hiding the fact that pageout_action
			 * is not initialized.
			 */
			if (new_object != VM_OBJECT_NULL &&
			    (last_offset != offset ||
			     pageout_action != page_lock_result)) {
 	    			PAGEOUT_PAGES(object, new_object, new_offset,
						pageout_action, paging_offset);
			}
#if	NORMA_VM
			/*
			 *	Attempt to reserve a pageout slot.
			 *	If denied, restore the state of this
			 *	page, discharge accumulated pages
			 *	and block.
			 */
			if (vm_pageout_reserve(object,
			       m->offset, page_size,
			       PAGEOUT_RESERVE_LOCK_REQUEST) == FALSE) {
				m->busy = FALSE;
				if (new_object != VM_OBJECT_NULL) {
					PAGEOUT_PAGES(object, new_object,
						      new_offset,
						      pageout_action,
						      paging_offset);
				}
				vm_object_unlock(object);
				vm_pageout_await_completed(
					PAGEOUT_RESERVE_LOCK_REQUEST);
				vm_object_lock(object);
				continue;
			}
#endif
			vm_object_unlock(object);

			/*
			 *	If we have not already allocated an object
			 *	for a range of pages to be written, do so
			 *	now.
			 */
			if (new_object == VM_OBJECT_NULL) {
			    new_object = vm_object_allocate(original_size);
			    new_offset = 0;
			    paging_offset = m->offset +
					object->paging_offset;
			    pageout_action = page_lock_result;
			}

			/*
			 *	Move or copy the dirty page into the
			 *	new object.
			 */
			holding_page = vm_pageout_setup(m,
					m->offset + object->paging_offset,
					new_object,
					new_offset,
					should_flush);

			/*
			 *	Save the holding page if there is one.
			 */
			holding_pages[atop(new_offset)] = holding_page;
			new_offset += PAGE_SIZE;
			last_offset = offset + PAGE_SIZE;

			vm_object_lock(object);
			break;
		}
		break;
	    }
	}

	/*
	 *	We have completed the scan for applicable pages.
	 *	Clean any pages that have been saved.
	 */
	if (new_object != VM_OBJECT_NULL) {
 	    PAGEOUT_PAGES(object, new_object, new_offset, pageout_action,
			paging_offset);
	}

	if (IP_VALID(reply_to)) {
		vm_object_unlock(object);

		/* consumes our naked send-once/send right for reply_to */
		(void) memory_object_lock_completed(reply_to, reply_to_type,
			object->pager_request, original_offset, original_size);

		vm_object_lock(object);
	}

	vm_object_paging_end(object);
	vm_object_unlock(object);
	vm_object_deallocate(object);

	return (KERN_SUCCESS);
}

/*
 *	Routine:	memory_object_sync
 *
 *	Kernel internal function to synch out pages in a given
 *	range within an object to its memory manager.  Much the
 *	same as memory_object_lock_request but page protection
 *	is not changed.
 *
 *	If the should_flush and should_return flags are true pages
 *	are flushed, that is dirty & precious pages are written to
 *	the memory manager and then discarded.  If should_return
 *	is false, only precious pages are returned to the memory
 *	manager.
 *
 *	If should flush is false and should_return true, the memory
 *	manager's copy of the pages is updated.  If should_return
 *	is also false, only the precious pages are updated.  This
 *	last option is of limited utility.
 *
 *	Returns:
 *	FALSE		if no pages were returned to the pager
 *	TRUE		otherwise.
 */

boolean_t
memory_object_sync(
	vm_object_t	object,
	vm_offset_t	offset,
	vm_size_t	size,
	boolean_t	should_flush,
	boolean_t	should_return)
{
	register vm_page_t      m;
	vm_size_t		original_size = size;
	vm_offset_t		paging_offset = 0;
	vm_object_t		new_object = VM_OBJECT_NULL;
	vm_offset_t		new_offset = 0;
	vm_offset_t		last_offset = offset;
	memory_object_lock_result_t	page_lock_result;
	memory_object_lock_result_t	pageout_action;
	boolean_t		data_returned = FALSE;
	vm_page_t		holding_pages[DATA_WRITE_MAX];

	/*
	 * Acquire a paging reference to prevent the memory_object
	 * and control ports from being destroyed.
	 */
	vm_object_lock(object);
	vm_object_paging_begin(object);
	/*
	 * Lock the object, and acquire a paging reference to
	 * prevent the memory_object and control ports from
	 * being destroyed.
	 */
	for (; size != 0; size -= PAGE_SIZE, offset += PAGE_SIZE) {
	    /*
	     * Limit the number of pages to be cleaned at once.
	     */
	    if (new_object != VM_OBJECT_NULL &&
		    new_offset >= PAGE_SIZE * DATA_WRITE_MAX) {
		PAGEOUT_PAGES(object, new_object, new_offset,
			pageout_action, paging_offset);
	    }

	    while ((m = vm_page_lookup(object, offset)) != VM_PAGE_NULL) {
		switch ((page_lock_result = memory_object_lock_page(m,
					(should_return ?
					 MEMORY_OBJECT_RETURN_ALL :
					 MEMORY_OBJECT_RETURN_NONE),
					should_flush,
					VM_PROT_NO_CHANGE))) {
		    case MEMORY_OBJECT_LOCK_RESULT_DONE:
			/*
			 *	End of a cluster of dirty pages.
			 */
			if (new_object != VM_OBJECT_NULL) {
				PAGEOUT_PAGES(object, new_object, new_offset,
					pageout_action, paging_offset);
			    continue;
			}
			break;

		    case MEMORY_OBJECT_LOCK_RESULT_MUST_BLOCK:
			/*
			 *	Since it is necessary to block,
			 *	clean any dirty pages now.
			 */
			if (new_object != VM_OBJECT_NULL) {
				PAGEOUT_PAGES(object, new_object, new_offset,
					pageout_action, paging_offset);
			    continue;
			}

			PAGE_ASSERT_WAIT(m, FALSE);
			vm_object_unlock(object);
			thread_block((void (*)(void))0);
			vm_object_lock(object);
			continue;

		    case MEMORY_OBJECT_LOCK_RESULT_MUST_CLEAN:
		    case MEMORY_OBJECT_LOCK_RESULT_MUST_RETURN:
			/*
			 * The clean and return cases are similar.
			 *
			 * Mark the page busy since we unlock the
			 * object below.
			 */
			m->busy = TRUE;

			/*
			 * if this would form a discontiguous block,
			 * clean the old pages and start anew.
			 *
			 * NOTE: The first time through here, new_object
			 * is null, hiding the fact that pageout_action
			 * is not initialized.
			 */
			if (new_object != VM_OBJECT_NULL &&
				(last_offset != offset ||
				pageout_action != page_lock_result)) {
				PAGEOUT_PAGES(object, new_object, new_offset,
					pageout_action, paging_offset);
			}
#if	NORMA_VM
			/*
			 *	Attempt to reserve a pageout slot.
			 *	If denied, restore the state of this
			 *	page, discharge accumulated pages
			 *	and block.
			 */
			if (vm_pageout_reserve(object,
			       m->offset, page_size,
			       PAGEOUT_RESERVE_SYNCHRONIZE) == FALSE) {
				m->busy = FALSE;
				if (new_object != VM_OBJECT_NULL) {
					PAGEOUT_PAGES(object, new_object,
						       new_offset,
						       pageout_action,
						       paging_offset);
				}
				vm_object_unlock(object);
				vm_pageout_await_completed(
					PAGEOUT_RESERVE_SYNCHRONIZE);
				vm_object_lock(object);
				continue;
			}
#endif
			vm_object_unlock(object);

			/*
			 *	If we have not already allocated an object
			 *	for a range of pages to be written, do so
			 *	now.
			 */
			if (new_object == VM_OBJECT_NULL) {
			    new_object = vm_object_allocate(original_size);
			    new_offset = 0;
			    paging_offset = m->offset +
					object->paging_offset;
			    pageout_action = page_lock_result;
			}

			/*
			 *	Move or copy the dirty page into the
			 *	new object.
			 */
			m = vm_pageout_setup(m,
					m->offset + object->paging_offset,
					new_object,
					new_offset,
					should_flush);
			/*
			 *	Save the holding page if there is one.
			 */
			holding_pages[atop(new_offset)] = m;
			new_offset += PAGE_SIZE;
			last_offset = offset + PAGE_SIZE;
			data_returned = TRUE;

			vm_object_lock(object);
			break;
		}
		break;
	    }
	}

	/*
	 *	We have completed the scan for applicable pages.
	 *	Clean any pages that have been saved.
	 */
	if (new_object != VM_OBJECT_NULL) {
		PAGEOUT_PAGES(object, new_object, new_offset,
			pageout_action, paging_offset);
	}

	vm_object_paging_end(object);
	vm_object_unlock(object);
	return data_returned;
}

/*
 *	Routine:	memory_object_synchronize_completed [user interface]
 *
 *	Tell kernel that previously synchronized data
 *	(memory_object_synchronize) has been queue or placed on the
 *	backing storage.
 *
 *	Note: there may be multiple synchronize requests for a given
 *	memory object outstanding but they will not overlap.
 */

kern_return_t
memory_object_synchronize_completed(
	vm_object_t	object,
	vm_offset_t	offset,
	vm_offset_t	length)
{
        msync_req_t             msr;

	/*
	 *      Look for bogus arguments
	 */

	if (object == VM_OBJECT_NULL) {
		return KERN_INVALID_ARGUMENT;
	}

	vm_object_lock(object);

/*
 *	search for sync request structure
 */
	queue_iterate(&object->msr_q, msr, msync_req_t, msr_q) {
 		if (msr->offset == offset && msr->length == length) {
			queue_remove(&object->msr_q, msr, msync_req_t, msr_q);
			break;
		}
        }/* queue_iterate */

	if (queue_end(&object->msr_q, (queue_entry_t)msr)) {
		panic("m_o_synchronize_completed: No sync request found\n");
		vm_object_unlock(object);
		return KERN_SUCCESS;
	}

	msr_lock(msr);
	vm_object_unlock(object);
	msr->flag = VM_MSYNC_DONE;
	msr_unlock(msr);
	thread_wakeup((int) msr);
	vm_object_deallocate(object);

	return KERN_SUCCESS;
}/* memory_object_synchronize_completed */
	  
int catch_map = 0;
kern_return_t
memory_object_set_attributes_common(
	vm_object_t	object,
	boolean_t	may_cache,
#if	NORMA_VM
	boolean_t	object_ready,
	boolean_t	write_completions,
	vm_size_t	cluster_size,
#endif	/* NORMA_VM */
	memory_object_copy_strategy_t copy_strategy,
	boolean_t	temporary,
	char		*existence_map,
	vm_size_t	existence_size)
{
	boolean_t	object_became_ready;
#if	MACH_PAGEMAP
	struct vm_external e0, *e = &e0;
#endif	/* MACH_PAGEMAP */

	if (object == VM_OBJECT_NULL)
		return(KERN_INVALID_ARGUMENT);

	/*
	 *	Verify the attributes of importance
	 */

	switch(copy_strategy) {
		case MEMORY_OBJECT_COPY_NONE:
#if	NORMA_VM
		case MEMORY_OBJECT_COPY_CALL:
		case MEMORY_OBJECT_COPY_SYMMETRIC:
#else	/* NORMA_VM */
		case MEMORY_OBJECT_COPY_DELAY:
#endif	/* NORMA_VM */
			break;
		default:
			vm_object_deallocate(object);
			return(KERN_INVALID_ARGUMENT);
	}

	if (may_cache)
		may_cache = TRUE;
	if (temporary)
		temporary = TRUE;

#if	MACH_PAGEMAP
	if (existence_size == 0) {
		vm_external_nullify(e);
	} else {
		vm_external_create(e, ptoa(existence_size << 3));
		if (! vm_external_null(e)) {
			memcpy(e->existence_map, existence_map, existence_size);
		}
	}
#endif	/* MACH_PAGEMAP */

	vm_object_lock(object);

	/*
	 *	Copy the attributes
	 */
#if	NORMA_VM
	if (object->internal)
		object_became_ready = FALSE;
	else {
		object_became_ready = !object->pager_ready && object_ready;
		object->copy_strategy = copy_strategy;
	}
#else	/* NORMA_VM */
	assert(!object->internal);
	object_became_ready = !object->pager_ready;
	object->copy_strategy = copy_strategy;
#endif	/* NORMA_VM */

	if (object_became_ready)
		object->pager_ready = TRUE;
	object->can_persist = may_cache;
	object->temporary = temporary;
#if	MACH_PAGEMAP
	if (vm_external_null(&object->existence_info)) {
		object->existence_info = *e;
	} else {
		/*
		 * XXX Assert that they match?
		 * XXX Try to avoid this situation?
		 */
#if 666
		if(catch_map)assert(FALSE);
#endif
		vm_external_destroy(e);
	}
#endif	/* MACH_PAGEMAP */

#if	NORMA_VM
	object->pager_modwc = write_completions;
	/*
	 * Cluster size operations aren't implemented yet.
	 *
	 * object->cluster_size = cluster_size;
	 */
	assert(cluster_size == page_size);
#endif

	/*
	 *	Wake up anyone waiting for the ready attribute
	 *	to become asserted.
	 */

	if (object_became_ready) {
		vm_object_wakeup(object, VM_OBJECT_EVENT_PAGER_READY);
	}

	vm_object_unlock(object);

	vm_object_deallocate(object);

	return(KERN_SUCCESS);
}

#if	NORMA_VM

/*
 *	Routine:	memory_object_data_write_completed
 *
 *	Purpose:
 *		Notify the kernel that a pager is done with a write request,
 *		and that pageins on these pages may proceed.
 */
kern_return_t
memory_object_data_write_completed(
	vm_object_t	object,
	vm_offset_t	offset,
	vm_size_t	size)
{
	assert(object != VM_OBJECT_NULL);
	if (object == VM_OBJECT_NULL)
		return KERN_INVALID_ARGUMENT;

	vm_object_lock(object);
	assert(object->ref_count > 0);

	if (object->alive) {
		assert(object->completions_pending > 0);
		vm_pageout_completed(object, offset, size);
	} else if (object->completions_pending > 0)
		vm_pageout_completed(object, offset, size);

	vm_object_deallocate_lazy(object);
	vm_object_unlock(object);
	return KERN_SUCCESS;
}

#else	/* NORMA_VM */

/*
 *	Set the memory object attribute as provided.
 *
 *	XXX This routine cannot be completed until the vm_msync, clean 
 *	     in place, and cluster work is completed. See ifdef notyet
 *	     below and note that memory_object_set_attributes_common()
 *	     may have to be expanded.
 */
kern_return_t
memory_object_change_attributes(
        vm_object_t             object,
        memory_object_flavor_t  flavor,
	memory_object_info_t	attributes,
	mach_msg_type_number_t	count,
        ipc_port_t              reply_to,
        mach_msg_type_name_t    reply_to_type)
{
        kern_return_t   		result = KERN_SUCCESS;
        boolean_t       		temporary;
        boolean_t       		may_cache;
        boolean_t       		invalidate;
        boolean_t       		object_ready;
        boolean_t       		write_completions;
	vm_size_t			cluster_size;
	memory_object_copy_strategy_t	copy_strategy;


	if (object == VM_OBJECT_NULL)
		return(KERN_INVALID_ARGUMENT);

	vm_object_lock(object);
	temporary = object->temporary;
	may_cache = object->can_persist;
	object_ready = object->pager_ready;
	copy_strategy = object->copy_strategy;
#if notyet
	invalidate = object->invalidate;
	cluster_size = object->cluster_size;
	write_completions = object->write_completions;
#endif
	vm_object_unlock(object);	

	switch (flavor) {
	    case MEMORY_OBJECT_BEHAVIOR_INFO:
	    {
                memory_object_behave_info_t     behave;

                if (count != MEMORY_OBJECT_BEHAVE_INFO_COUNT) {
                        result = KERN_INVALID_ARGUMENT;
                        break;
                }

                behave = (memory_object_behave_info_t) attributes;

		temporary = behave->temporary;
		invalidate = behave->invalidate;
		copy_strategy = behave->copy_strategy;
		write_completions = behave->write_completions;

		break;
	    }

	    case MEMORY_OBJECT_PERFORMANCE_INFO:
	    {
		memory_object_perf_info_t	perf;

                if (count != MEMORY_OBJECT_PERF_INFO_COUNT) {
                        result = KERN_INVALID_ARGUMENT;
                        break;
                }

                perf = (memory_object_perf_info_t) attributes;

		may_cache = perf->may_cache;
		cluster_size = perf->cluster_size;
	
		break;
	    }

	    case MEMORY_OBJECT_ATTRIBUTE_INFO:
	    {
		memory_object_attr_info_t	attr;

                if (count != MEMORY_OBJECT_ATTR_INFO_COUNT) {
                        result = KERN_INVALID_ARGUMENT;
                        break;
                }

		attr = (memory_object_attr_info_t) attributes;

                may_cache = attr->may_cache;
                object_ready = attr->object_ready;
                copy_strategy = attr->copy_strategy;

		break;
	    }

	    default:
		result = KERN_INVALID_ARGUMENT;
		break;
	}

	if (result != KERN_SUCCESS) {
		vm_object_deallocate(object);
		return(result);
	}

        if (copy_strategy == MEMORY_OBJECT_COPY_TEMPORARY) {
                copy_strategy = MEMORY_OBJECT_COPY_DELAY;
                temporary = TRUE;
        } else {
                temporary = FALSE;
        }

        /*
         *      Do the work and throw away our object reference.  It
         *      is important that the object reference be deallocated
         *      BEFORE sending the reply.  The whole point of the reply
         *      is that it shows up after the terminate message that
         *      may be generated by setting the object uncacheable.
         *
         * XXX  may_cache may become a tri-valued variable to handle
         * XXX  uncache if not in use.
         */
        result = memory_object_set_attributes_common(object,
                                                     may_cache,
						     copy_strategy,
                                                     temporary,
                                                     (char *) 0, 0);

        if (IP_VALID(reply_to)) {
                /* consumes our naked send-once/send right for reply_to */
                (void) memory_object_change_completed(reply_to, reply_to_type,
		       object->alive ? object->pager_request : IP_NULL,
		       flavor);
        }

        return(result);
}

#endif	/* NORMA_VM */

kern_return_t
memory_object_get_attributes(
        vm_object_t     	object,
        memory_object_flavor_t 	flavor,
	memory_object_info_t	attributes,	/* pointer to OUT array */
	mach_msg_type_number_t	*count)		/* IN/OUT */
{
	kern_return_t ret = KERN_SUCCESS;

        if (object == VM_OBJECT_NULL)
                return(KERN_INVALID_ARGUMENT);

        vm_object_lock(object);

	switch (flavor) {
	    case MEMORY_OBJECT_BEHAVIOR_INFO:
	    {
		memory_object_behave_info_t	behave;

		if (*count < MEMORY_OBJECT_BEHAVE_INFO_COUNT) {
			ret = KERN_INVALID_ARGUMENT;
			break;
		}

		behave = (memory_object_behave_info_t) attributes;
		behave->copy_strategy = object->copy_strategy;
		behave->temporary = object->temporary;
#if notyet	/* remove when vm_msync complies and clean in place fini */
                behave->invalidate = object->invalidate;
		behave->write_completions = object->write_completions;
#else
		behave->invalidate = FALSE;
		behave->write_completions = FALSE;
#endif

		*count = MEMORY_OBJECT_BEHAVE_INFO_COUNT;
		break;
	    }

	    case MEMORY_OBJECT_PERFORMANCE_INFO:
	    {
		memory_object_perf_info_t	perf;

		if (*count < MEMORY_OBJECT_PERF_INFO_COUNT) {
			ret = KERN_INVALID_ARGUMENT;
			break;
		}

		perf = (memory_object_perf_info_t) attributes;
#if notyet	/* remove when cluster code submitted */
		perf->cluster_size = object->cluster_size;
#else
		perf->cluster_size = 0;
#endif
		perf->may_cache = object->can_persist;

		*count = MEMORY_OBJECT_PERF_INFO_COUNT;
		break;
	    }

            case MEMORY_OBJECT_ATTRIBUTE_INFO:
            {
                memory_object_attr_info_t       attr;

                if (*count < MEMORY_OBJECT_ATTR_INFO_COUNT) {
                        ret = KERN_INVALID_ARGUMENT;
                        break;
                }

                attr = (memory_object_attr_info_t) attributes;
        	attr->may_cache = object->can_persist;
        	attr->object_ready = object->pager_ready;
        	attr->copy_strategy = object->copy_strategy;

                *count = MEMORY_OBJECT_ATTR_INFO_COUNT;
                break;
            }

	    default:
		ret = KERN_INVALID_ARGUMENT;
		break;
	}

        vm_object_unlock(object);

        vm_object_deallocate(object);

        return(ret);
}

/*
 *	If successful, consumes the supplied naked send right.
 */
kern_return_t
vm_set_default_memory_manager(
	host_t		host,
	ipc_port_t	*default_manager)
{
	ipc_port_t current_manager;
	ipc_port_t new_manager;
	ipc_port_t returned_manager;

	if (host == HOST_NULL)
		return(KERN_INVALID_HOST);

	new_manager = *default_manager;
	mutex_lock(&memory_manager_default_lock);
	current_manager = memory_manager_default;

	if (new_manager == IP_NULL) {
		/*
		 *	Retrieve the current value.
		 */

		returned_manager = ipc_port_copy_send(current_manager);
	} else {
		/*
		 *	Retrieve the current value,
		 *	and replace it with the supplied value.
		 *	We consume the supplied naked send right.
		 */

		returned_manager = current_manager;
		memory_manager_default = new_manager;

#if	NORMA_IPC || MACH_KDB
		/*
		 *	When NORMA runs low on VM, incoming messages
		 *	are rejected except when they are bound for the
		 *	default pager.  To detect these messages, we
		 *	need to remember the default pager's ipc space.
		 */
		assert(IP_VALID(new_manager));

		ip_lock(new_manager);
		if (ip_active(new_manager) &&
#if	NORMA_IPC
		    !IP_NORMA_IS_PROXY(new_manager) &&
#endif	/* NORMA_IPC */
		    new_manager->ip_receiver_name != MACH_PORT_NULL) {
			default_pager_space = new_manager->ip_receiver;
		}
		ip_unlock(new_manager);
#endif	/* NORMA_IPC || MACH_KDB */

		/*
		 *	In case anyone's been waiting for a memory
		 *	manager to be established, wake them up.
		 */

		thread_wakeup((int) &memory_manager_default);
	}

	mutex_unlock(&memory_manager_default_lock);

	*default_manager = returned_manager;
	return(KERN_SUCCESS);
}

/*
 *	Routine:	memory_manager_default_reference
 *	Purpose:
 *		Returns a naked send right for the default
 *		memory manager.  The returned right is always
 *		valid (not IP_NULL or IP_DEAD).
 */

ipc_port_t
memory_manager_default_reference(void)
{
	ipc_port_t current_manager;

	mutex_lock(&memory_manager_default_lock);

	while (current_manager = ipc_port_copy_send(memory_manager_default),
	       !IP_VALID(current_manager)) {
		thread_sleep_mutex((int) &memory_manager_default,
			&memory_manager_default_lock, FALSE);
		mutex_lock(&memory_manager_default_lock);
	}

	mutex_unlock(&memory_manager_default_lock);

	return current_manager;
}

/*
 *	Routine:	memory_manager_default_port
 *	Purpose:
 *		Returns true if the receiver for the port
 *		is the default memory manager.
 *
 *		This is a hack to let ds_read_done
 *		know when it should keep memory wired.
 */

boolean_t
memory_manager_default_port(
	ipc_port_t port)
{
	ipc_port_t current;
	boolean_t result;

	mutex_lock(&memory_manager_default_lock);
	current = memory_manager_default;
	if (IP_VALID(current)) {
		/*
		 *	There is no point in bothering to lock
		 *	both ports, which would be painful to do.
		 *	If the receive rights are moving around,
		 *	we might be inaccurate.
		 */

		result = port->ip_receiver == current->ip_receiver;
	} else
		result = FALSE;
	mutex_unlock(&memory_manager_default_lock);

	return result;
}

void
memory_manager_default_init(void)
{
	memory_manager_default = IP_NULL;
	mutex_init(&memory_manager_default_lock);
}


#if	MOR
/*
 *      Routine:        memory_object_establish
 *
 *      Description:
 *              Marks a memory object ready to receive paging operations.
 *		Calling this interface means the underlying memory object
 *	 	has not been initialized yet.
 */
kern_return_t
memory_object_establish(
	host_t 			host,
	ipc_port_t		rep,
	ipc_port_t		ledger,
	pager_request_t		control,
	ipc_port_t		pager,
	vm_prot_t		prot,
	security_token_t	sec_token,
	memory_object_flavor_t	flavor,
	memory_object_info_t	behavior,
	mach_msg_type_number_t	count)
{
	kern_return_t		kr;
	vm_object_rep_t		repo;
        vm_object_t     	object;

	/*
	 *	Check input values.
	 */
	if (invalid_memory_object_flavor(flavor) || 
            host != (host_t)&realhost || (prot & ~VM_PROT_ALL)) {
		kr = KERN_INVALID_ARGUMENT;
		(void) memory_object_rejected(pager, control, kr);
		return(kr);
	}

	/*
	 *	Check that object corresponds to the representative
	 *	used in the request. 
	 */
        if (!IP_VALID(rep) || ip_kotype(rep) != IKOT_PAGER_REP) {
		(void) memory_object_rejected(pager, control, KERN_FAILURE);
		return(KERN_FAILURE);
	}
	repo = (vm_object_rep_t) rep->ip_kobject;
	assert(repo != VM_OBJECT_REP_NULL);
	assert(repo->rep == rep);

	/*
	 *	If we were not expecting an establish, send a rejected
  	 *	message to the memory manager.
	 */
	if (repo->pager_request != control || 
	    repo->constituent != VM_OBJECT_NULL) {
		kr = KERN_FAILURE;
		(void) memory_object_rejected(pager, control, kr);
		return(kr);
	}
#if 0
	if (INVALID_LEDGER(ledger)) {
		kr = KERN_FAILURE;
		(void) memory_object_rejected(pager, control, kr);
		return(kr);
        }
#endif

	/*
	 *	If the rep and the pager are identical, this means the pager
	 *	port itself, instead of a representative, has been used in 
	 *	the vm_map call. In this case, clear the pager port kobj type
	 *	so it will be initialized as a pager port when entered.
	 */
	if (rep == pager) {
		ipc_kobject_set(pager, IKO_NULL, IKOT_NONE);
	}

        /*
         *      Obtain the vm_object for this pager, associating it with the
         *      given representative.
         */
	object = vm_object_enter_ext(pager, repo);
	if (object == VM_OBJECT_NULL) {
		printf("mo_establish: vm_object_enter_ext rtns NULL\n");
		(void) memory_object_rejected(pager, control, KERN_FAILURE);
                return(KERN_FAILURE);
	}

	/*
	 *	Set the pager attributes as requested.
	 */
	vm_object_reference(object);

	if (flavor == MEMORY_OBJECT_ATTRIBUTE_INFO) {
		memory_object_attr_info_t	info;
		boolean_t			temporary;
		memory_object_copy_strategy_t	copy_strategy;

		if (count < MEMORY_OBJECT_ATTR_INFO_COUNT) {
			kr = KERN_FAILURE;
                	(void) memory_object_rejected(pager, control, kr);
			return(kr);
		}
		info = (memory_object_attr_info_t) behavior;
		if (info->object_ready != TRUE) {
			/* This flag is meaningless */
			kr = KERN_INVALID_ARGUMENT;
                	(void) memory_object_rejected(pager, control, kr);
			return(kr);
		}
		if (info->copy_strategy == MEMORY_OBJECT_COPY_TEMPORARY) {
			copy_strategy = MEMORY_OBJECT_COPY_DELAY;
			temporary = TRUE;
		} else {
			copy_strategy = info->copy_strategy;
			temporary = FALSE;
		}
		kr = memory_object_set_attributes_common(object, 
					          info->may_cache,
#if	NORMA_VM
					          TRUE, FALSE,
					          page_size,
#endif	/* NORMA_VM */
       						  copy_strategy,
						  temporary,
						  (char *)0, 0);
		if (kr != KERN_SUCCESS) {
			(void) memory_object_rejected(pager, control, kr);
			return(kr);
		}
	}
	else if (flavor == MEMORY_OBJECT_BEHAVIOR_INFO) {
		memory_object_behave_info_t	info;
		if (count < MEMORY_OBJECT_BEHAVE_INFO_COUNT) {
			kr = KERN_FAILURE;
			(void) memory_object_rejected(pager, control, kr);
			return(kr);
		}
		info = (memory_object_behave_info_t) behavior;

		printf("MEMORY_OBJECT_BEHAVIOR_INFO needs flavor version \n");
		printf("of memory_object_set_attributes() interface, which\n");
		printf("is not implemented yet\n");
	
		kr = KERN_INVALID_ARGUMENT;
		(void) memory_object_rejected(pager, control, kr);
		return(kr);
	}
        else if (flavor == MEMORY_OBJECT_PERFORMANCE_INFO) {
                memory_object_perf_info_t   	info;
                if (count < MEMORY_OBJECT_PERF_INFO_COUNT) {
			kr = KERN_FAILURE;
			(void) memory_object_rejected(pager, control, kr);
			return(kr);
		}
                info = (memory_object_perf_info_t) behavior;

                printf("MEMORY_OBJECT_PERF_INFO needs flavor version \n");
                printf("of memory_object_set_attributes() interface, which\n");
                printf("is not implemented yet\n");

		kr = KERN_INVALID_ARGUMENT;
		(void) memory_object_rejected(pager, control, kr);
		return(kr);
        }
	else {
		panic("mo_establish: flavor botch");
	}

	/*
         *      Wakeup any reps waiting for a ready memory object to 
	 *	associate themselves with.
	 */

	assert(ip_kotype(pager) == IKOT_PAGER);
	assert(((vm_object_t) pager->ip_kobject) != VM_OBJECT_NULL);

	repo->constituent = (vm_object_t) pager->ip_kobject;
	repo->protection = prot;

	vm_object_rep_wakeup(repo, VM_OBJECT_EVENT_REP_READY);

	return(KERN_SUCCESS);
}

/*
 *      Routine:        memory_object_representative
 *
 *      Description:
 *              Associates a memory object representative with an 
 *		already initialized memory object. That is, although
 *		the kernel does not know this rep, it knows the under-
 *		lying vm_object via another rep.
 */

/* XXX FIXME */
mutex_t	vm_object_cached_lock_data;
#define vm_object_cache_lock()	mutex_lock(&vm_object_cached_lock_data)
#define vm_object_cache_unlock() mutex_unlock(&vm_object_cached_lock_data)

kern_return_t
memory_object_representative(
	host_t			host,
        ipc_port_t		rep,
        pager_request_t		control,
        ipc_port_t		pager,
        vm_prot_t		prot,
        security_token_t	sec_token)
{
        kern_return_t           kr;
        vm_object_rep_t         repo;
        vm_object_t             object;


        /*
         *      Check input values.
         */
	if (host != (host_t)&realhost || (prot & ~VM_PROT_ALL)) {
                kr = KERN_INVALID_ARGUMENT;
                (void) memory_object_rejected(pager, control, kr);
                return(kr);
        }

        /*
         *      Check that object corresponds to the representative
         *      used in the request.
         */
        if (!IP_VALID(rep) || ip_kotype(rep) != IKOT_PAGER_REP) {
                (void) memory_object_rejected(pager, control, KERN_FAILURE);
                return(KERN_FAILURE);
        }
        repo = (vm_object_rep_t) rep->ip_kobject;
        assert(repo != VM_OBJECT_REP_NULL);

        /*
         *      If we were not expecting mo_representative, send a rejected
         *      message to the memory manager.
         */
        if (repo->pager_request != control || 
	    repo->constituent != VM_OBJECT_NULL) {
                kr = KERN_FAILURE;
                (void) memory_object_rejected(pager, control, kr);
                return(kr);
        }

	if (ip_kotype(pager) != IKOT_PAGER || pager->ip_kobject == IKO_NULL) {
		printf("mo_representative: bad pager port\n");
                (void) memory_object_rejected(pager, control, KERN_FAILURE);
                return(KERN_FAILURE);
	}

	/*
	 * 	The current message may race with memory object termination,
	 *	i.e. the memory manager may believe the object is initialized
	 *	but it has actually been terminated since then. In this case,
	 *	discard the current message and resend mo_notify. The manager
	 *	will then re-establish the object with attributes.
	 */ 
	vm_object_cache_lock();
	if (ip_kotype(pager) != IKOT_PAGER) {
#if	NORMA_VM
		(void) xmm_memory_object_rep_init(repo);
#else	/* NORMA_VM */
		(void) memory_object_notify(rep, control, 
					ipc_port_make_send(realhost.host_self),
					PAGE_SIZE);
#endif	/* NORMA_VM */
		vm_object_cache_unlock();
		return(KERN_FAILURE);
	}

	object = (vm_object_t) pager->ip_kobject;
	vm_object_lock(object);
	if (!object->pager_initialized) {
		vm_object_unlock(object);
#if	NORMA_VM
		(void) xmm_memory_object_rep_init(repo);
#else	/* NORMA_VM */
		(void) memory_object_notify(rep, control,
                                        ipc_port_make_send(realhost.host_self),
                                        PAGE_SIZE);
#endif	/* NORMA_VM */
		vm_object_cache_unlock();
                return(KERN_FAILURE);
	}
	vm_object_unlock(object);
	vm_object_cache_unlock();
	
        /*
         *      Obtain the vm_object for the pager, associating this
         *      representative with its underlying pager.
         */
        object = vm_object_enter_ext(pager, repo);
        if (object == VM_OBJECT_NULL) {
                printf("mo_represent: vm_object_enter_ext rtns NULL\n");
                (void) memory_object_rejected(pager, control, KERN_FAILURE);
                return(KERN_FAILURE);
        }

	/*
	 *	Destroy the redundant memory cache control port supplied.
	 */
	repo->pager_request = PAGER_REQUEST_NULL;
#if	!NORMA_VM
	ipc_kobject_set(control, IKO_NULL, IKOT_NONE);
	ipc_port_dealloc_kernel(control);
#endif	/* !NORMA_VM */

        /*
         *      Wakeup any reps waiting for a ready memory object to 
	 *	associate themselves with.
         */

        assert(ip_kotype(pager) == IKOT_PAGER);
        assert(((vm_object_t) pager->ip_kobject) != VM_OBJECT_NULL);

        repo->constituent = (vm_object_t) pager->ip_kobject;
        repo->protection = prot;

        vm_object_rep_wakeup(repo, VM_OBJECT_EVENT_REP_READY);

        return(KERN_SUCCESS);
}
#endif	/* MOR */
