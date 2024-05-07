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
 * Revision 2.15.5.3  92/09/15  17:36:09  jeffreyh
 * 	Added support to vm_fault_page to pass back an error code if the
 * 	page has an error; this supports m_o_data_error.
 * 	[92/07/28            sjs]
 * 
 * Revision 2.15.5.2  92/03/03  16:25:59  jeffreyh
 * 	Check for both copy and shadow objects due to asymmetric
 * 	copy on write for temporary objects.
 * 	[92/02/19  14:13:33  dlb]
 * 
 * 	No more sharing maps.  Eliminated single_use argument
 * 	from vm_map_lookup() calls.
 * 	[92/01/07  11:03:34  dlb]
 * 	Changes from TRUNK
 * 	[92/02/26  12:33:58  jeffreyh]
 * 
 * Revision 2.16  92/01/14  16:47:44  rpd
 * 	Fixed some locking/assertion bugs in vm_fault_copy.
 * 	[92/01/06            rpd]
 * 
 * Revision 2.15  91/11/12  11:52:02  rvb
 * 	Added simple_lock_pause.
 * 	[91/11/12            rpd]
 * 
 * Revision 2.14  91/07/01  08:27:05  jsb
 * 	Changed remaining pager_* references to memory_object_*.
 * 	[91/06/29  16:26:22  jsb]
 * 
 * Revision 2.13  91/05/18  14:40:02  rpd
 * 	Added proper locking for vm_page_insert.
 * 	[91/04/21            rpd]
 * 	Changed vm_fault_page to use fictitious pages.
 * 	Increased vm_object_absent_max to 50.
 * 	[91/03/29            rpd]
 * 
 * Revision 2.12  91/05/14  17:48:42  mrt
 * 	Correcting copyright
 * 
 * Revision 2.11  91/03/16  15:04:49  rpd
 * 	Fixed vm_fault_page to give vm_pageout_page busy pages.
 * 	[91/03/11            rpd]
 * 	Added vm_fault_init, vm_fault_state_t.
 * 	[91/02/16            rpd]
 * 
 * 	Added resume, continuation arguments to vm_fault, vm_fault_page.
 * 	Added continuation argument to VM_PAGE_WAIT.
 * 	Added vm_fault_continue.
 * 	[91/02/05            rpd]
 * 
 * Revision 2.10  91/02/05  17:58:00  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:31:44  mrt]
 * 
 * Revision 2.9  91/01/08  16:44:45  rpd
 * 	Turned software_reference_bits on.
 * 	[90/12/29            rpd]
 * 	Added continuation argument to thread_block.
 * 	[90/12/08            rpd]
 * 
 * 	Changed VM_WAIT to VM_PAGE_WAIT.
 * 	[90/11/13            rpd]
 * 
 * Revision 2.8  90/10/25  14:49:52  rwd
 * 	Turn software_reference_bits off by default.
 * 	[90/10/25            rwd]
 * 
 * 	Extended software_reference_bits to vm_fault_page.
 * 	[90/10/24            rpd]
 * 	Fixed vm_fault_page to clear the modify bit on zero-filled pages.
 * 	[90/10/23            rpd]
 * 	Added watchpoint debugger support.
 * 	[90/10/16            rpd]
 * 	Added software_reference_bits runtime switch.
 * 	[90/10/13            rpd]
 * 
 * Revision 2.7  90/10/12  13:05:21  rpd
 * 	Added missing vm_map_verify_done calls to vm_fault and
 * 	vm_fault_copy.  From OSF.
 * 	[90/10/10            rpd]
 * 	Modified vm_fault_page to leave its result page on the pageout queues.
 * 	Only activate the pages returned by vm_fault_page if they aren't
 * 	already on a pageout queue.  In vm_fault, turn on the software
 * 	reference bit for the page.
 * 	[90/10/08            rpd]
 * 
 * Revision 2.6  90/06/02  15:10:32  rpd
 * 	Fixed vm_fault_copy to handle null source objects.
 * 	[90/04/24            rpd]
 * 	Converted to new IPC.
 * 	[90/03/26  23:11:58  rpd]
 * 
 * Revision 2.5  90/05/29  18:38:39  rwd
 * 	Picked up rfr debugging changes.
 * 	[90/04/12  13:47:40  rwd]
 * 
 * Revision 2.4  90/05/03  15:58:29  dbg
 * 	Pass 'flush' argument to vm_pageout_page.
 * 	[90/03/28            dbg]
 * 
 * Revision 2.3  90/02/22  20:05:21  dbg
 * 	Deactivate the copied-from page.
 * 	[90/02/09            dbg]
 * 	Add changes from mainline:
 * 		Assert that page is not busy before marking it busy.
 * 		[89/12/21            dlb]
 * 		Check for absent as well as busy before freeing a page when
 * 		pagein fails.
 * 		[89/12/13            dlb]
 * 		Change all occurrences of PAGE_WAKEUP to PAGE_WAKEUP_DONE to
 * 		reflect the fact that they clear the busy flag.  See
 * 		vm/vm_page.h.  Add PAGE_WAKEUP_DONE to vm_fault_unwire().
 * 		[89/12/13            dlb]
 * 		Break out of fault loop after zero filling in response to
 * 		finding an absent page; the zero filled page was either absent
 * 		or newly allocated and so can't be page locked.
 * 		[89/12/12            dlb]
 * 		Must recheck page to object relationship before freeing
 * 		page if pagein fails.
 * 		[89/12/11            dlb]
 * 
 * 		Use vme_start, vme_end when accessing map entries.
 * 		[89/08/31  21:10:05  rpd]
 * 
 * 		Add vm_fault_copy(), for overwriting a permanent object.
 * 		[89/07/28  16:14:27  mwyoung]
 * 
 * Revision 2.2  90/01/11  11:47:36  dbg
 * 	Add vm_fault_cleanup to save space.
 * 	[89/12/13            dbg]
 * 
 * 	Pick up changes from mainline:
 * 
 * 		Consider paging_offset when looking at external page state.
 * 		[89/10/16  15:31:17  af]
 * 
 * 		Only require read access for the original page once past the
 * 		top-level object... it will only be copied to a new page.
 * 		[89/05/19  17:45:05  mwyoung]
 * 
 * 		Also remove "absent" page from pageout queues before zero-filling.
 * 		[89/05/01            mwyoung]
 * 		When transforming an "absent" page into a placeholder page,
 * 		remove it from the page queues.
 * 		[89/04/22            mwyoung]
 * 
 * 		Fixed usage of must_be_resident in vm_fault_page when
 * 		descending down shadow chain.  Fixed corresponding
 * 		assertion in vm_fault.
 * 		[89/10/02  16:17:20  rpd]
 * 	Remove vm_fault_copy_entry and non-XP code.
 * 	[89/04/28            dbg]
 * 
 * Revision 2.1  89/08/03  16:44:50  rwd
 * Created.
 * 
 * Revision 2.17  89/05/06  02:58:43  rpd
 * 	Picked up fix from mwyoung for a COW-triggered page leak:
 * 	when copying from a copy-on-write page, activate the page
 * 	instead of deactivating it.  Also picked up two innocuous
 * 	VM_PAGE_QUEUES_REMOVE() additions in the "unavailable page" code.
 * 	[89/05/06            rpd]
 * 	Fixed the call to vm_fault_wire_fast in vm_fault_copy_entry.
 * 	[89/05/05            rpd]
 * 
 * Revision 2.16  89/04/18  21:25:12  mwyoung
 * 	Recent history:
 * 		Limit the number of outstanding page requests for
 * 		 non-internal objects.
 * 		Use hint to determine whether a page of temporary memory may
 * 		 have been written to backing storage.
 * 	History condensation:
 * 		Separate fault handling into separate routine (mwyoung).
 * 		Handle I/O errors (dbg, mwyoung).
 * 		Use map_verify technology (mwyoung).
 * 		Allow faults to be interrupted (mwyoung).
 * 		Optimized wiring code (dlb).
 * 		Initial external memory management (mwyoung, bolosky).
 * 		Original version (avie, mwyoung, dbg).
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
 *	File:	vm_fault.c
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 *
 *	Page fault handling module.
 */

#include <mach_pagemap.h>
#include <mach_kdb.h>
#include <norma_vm.h>

#include <vm/vm_fault.h>
#include <mach/kern_return.h>
#include <mach/message.h>	/* for error codes */
#include <kern/counters.h>
#include <kern/thread.h>
#include <kern/sched_prim.h>
#include <vm/vm_map.h>
#include <vm/vm_object.h>
#include <vm/vm_page.h>
#include <vm/pmap.h>
#include <mach/vm_statistics.h>
#include <vm/vm_pageout.h>
#include <mach/vm_param.h>
#include <mach/memory_object.h>
#include <mach/memory_object_user.h>
				/* For memory_object_data_{request,unlock} */
#include <kern/mach_param.h>
#include <kern/macro_help.h>
#include <kern/zalloc.h>
#include <kern/misc_protos.h>

#if	NORMA_VM
#include <kern/host.h>
#include <norma/xmm_server_rename.h>
#endif	/* NORMA_VM */

#define VM_FAULT_CLASSIFY	0
#define VM_FAULT_STATIC_CONFIG	1

int		vm_object_absent_max = 50;

int		vm_fault_debug = 0;

#if	!VM_FAULT_STATIC_CONFIG
boolean_t	vm_fault_dirty_handling = FALSE;
boolean_t	vm_fault_interruptible = FALSE;

boolean_t	software_reference_bits = TRUE;
#endif

#if	MACH_KDB
extern struct db_watchpoint *db_watchpoint_list;
#endif	/* MACH_KDB */

/* Forward declarations of internal routines. */
extern kern_return_t vm_fault_wire_fast(
				vm_map_t	map,
				vm_offset_t	va,
				vm_map_entry_t	entry);

extern void vm_fault_continue(void);

extern void vm_fault_copy_cleanup(
				vm_page_t	page,
				vm_page_t	top_page);

#if	VM_FAULT_CLASSIFY
extern void vm_fault_classify(vm_object_t	object,
			  vm_offset_t	offset,
			  vm_prot_t	fault_type);

extern void vm_fault_classify_init(void);
#endif

/*
 *	Routine:	vm_fault_init
 *	Purpose:
 *		Initialize our private data structures.
 */
void
vm_fault_init(void)
{
}

/*
 *	Routine:	vm_fault_cleanup
 *	Purpose:
 *		Clean up the result of vm_fault_page.
 *	Results:
 *		The paging reference for "object" is released.
 *		"object" is unlocked.
 *		If "top_page" is not null,  "top_page" is
 *		freed and the paging reference for the object
 *		containing it is released.
 *
 *	In/out conditions:
 *		"object" must be locked.
 */
void
vm_fault_cleanup(
	register vm_object_t	object,
	register vm_page_t	top_page)
{
	vm_object_paging_end(object);
	vm_object_unlock(object);

	if (top_page != VM_PAGE_NULL) {
	    object = top_page->object;
	    vm_object_lock(object);
	    VM_PAGE_FREE(top_page);
	    vm_object_paging_end(object);
	    vm_object_unlock(object);
	}
}

/*
 *	Routine:	vm_fault_page
 *	Purpose:
 *		Find the resident page for the virtual memory
 *		specified by the given virtual memory object
 *		and offset.
 *	Additional arguments:
 *		The required permissions for the page is given
 *		in "fault_type".  Desired permissions are included
 *		in "protection".
 *
 *		If the desired page is known to be resident (for
 *		example, because it was previously wired down), asserting
 *		the "unwiring" parameter will speed the search.
 *
 *		If the operation can be interrupted (by thread_abort
 *		or thread_terminate), then the "interruptible"
 *		parameter should be asserted.
 *
 *	Results:
 *		The page containing the proper data is returned
 *		in "result_page".
 *
 *	In/out conditions:
 *		The source object must be locked and referenced,
 *		and must donate one paging reference.  The reference
 *		is not affected.  The paging reference and lock are
 *		consumed.
 *
 *		If the call succeeds, the object in which "result_page"
 *		resides is left locked and holding a paging reference.
 *		If this is not the original object, a busy page in the
 *		original object is returned in "top_page", to prevent other
 *		callers from pursuing this same data, along with a paging
 *		reference for the original object.  The "top_page" should
 *		be destroyed when this guarantee is no longer required.
 *		The "result_page" is also left busy.  It is not removed
 *		from the pageout queues.
 */

vm_fault_return_t
vm_fault_page(
	/* Arguments: */
	vm_object_t	first_object,	/* Object to begin search */
	vm_offset_t	first_offset,	/* Offset into object */
	vm_prot_t	fault_type,	/* What access is requested */
	boolean_t	must_be_resident,/* Must page be resident? */
	boolean_t	interruptible,	/* May fault be interrupted? */
	/* Modifies in place: */
	vm_prot_t	*protection,	/* Protection for mapping */
	/* Returns: */
	vm_page_t	*result_page,	/* Page found, if successful */
	vm_page_t	*top_page,	/* Page in top object, if
					 * not result_page.  */
	/* More arguments: */
	kern_return_t	*error_code,	/* code if page is in error */
	boolean_t	no_zero_fill)	/* don't zero fill absent pages */
{
	register
	vm_page_t	m;
	register
	vm_object_t	object;
	register
	vm_offset_t	offset;
	vm_page_t	first_m;
	vm_object_t	next_object;
#if	!NORMA_VM
	vm_object_t	copy_object;
#endif	/* !NORMA_VM */
	boolean_t	look_for_page;
	vm_prot_t	access_required;
	vm_prot_t	wants_copy_flag;
	thread_t	thread = current_thread();

	vm_stat.faults++;		/* needs lock XXX */

/*
 *	Recovery actions
 */
#define RELEASE_PAGE(m)					\
	MACRO_BEGIN					\
	PAGE_WAKEUP_DONE(m);				\
	vm_page_lock_queues();				\
	if (!m->active && !m->inactive)			\
		vm_page_activate(m);			\
	vm_page_unlock_queues();			\
	MACRO_END




#if	!VM_FAULT_STATIC_CONFIG
	if (vm_fault_dirty_handling
#if	MACH_KDB
		/*
		 *	If there are watchpoints set, then
		 *	we don't want to give away write permission
		 *	on a read fault.  Make the task write fault,
		 *	so that the watchpoint code notices the access.
		 */
	    || db_watchpoint_list
#endif	/* MACH_KDB */
	    ) {
		/*
		 *	If we aren't asking for write permission,
		 *	then don't give it away.  We're using write
		 *	faults to set the dirty bit.
		 */
		if (!(fault_type & VM_PROT_WRITE))
			*protection &= ~VM_PROT_WRITE;
	}

	if (!vm_fault_interruptible)
		interruptible = FALSE;
#else	/* STATIC_CONFIG */
#if	MACH_KDB
		/*
		 *	If there are watchpoints set, then
		 *	we don't want to give away write permission
		 *	on a read fault.  Make the task write fault,
		 *	so that the watchpoint code notices the access.
		 */
	    if (db_watchpoint_list) {
		/*
		 *	If we aren't asking for write permission,
		 *	then don't give it away.  We're using write
		 *	faults to set the dirty bit.
		 */
		if (!(fault_type & VM_PROT_WRITE))
			*protection &= ~VM_PROT_WRITE;
	}

	interruptible = FALSE;  /* vm_fault_interruptible */
#endif	/* MACH_KDB */
#endif	/* STATIC_CONFIG */

	/*
	 *	INVARIANTS (through entire routine):
	 *
	 *	1)	At all times, we must either have the object
	 *		lock or a busy page in some object to prevent
	 *		some other thread from trying to bring in
	 *		the same page.
	 *
	 *		Note that we cannot hold any locks during the
	 *		pager access or when waiting for memory, so
	 *		we use a busy page then.
	 *
	 *		Note also that we aren't as concerned about more than
	 *		one thread attempting to memory_object_data_unlock
	 *		the same page at once, so we don't hold the page
	 *		as busy then, but do record the highest unlock
	 *		value so far.  [Unlock requests may also be delivered
	 *		out of order.]
	 *
	 *	2)	To prevent another thread from racing us down the
	 *		shadow chain and entering a new page in the top
	 *		object before we do, we must keep a busy page in
	 *		the top object while following the shadow chain.
	 *
	 *	3)	We must increment paging_in_progress on any object
	 *		for which we have a busy page, to prevent
	 *		vm_object_collapse from removing the busy page
	 *		without our noticing.
	 *
	 *	4)	We leave busy pages on the pageout queues.
	 *		If the pageout daemon comes across a busy page,
	 *		it will remove the page from the pageout queues.
	 */

	/*
	 *	Search for the page at object/offset.
	 */

	object = first_object;
	offset = first_offset;
	first_m = VM_PAGE_NULL;
	access_required = fault_type;

	/*
	 *	See whether this page is resident
	 */

	while (TRUE) {
		m = vm_page_lookup(object, offset);
		if (m != VM_PAGE_NULL) {
			/*
			 *	If the page is being brought in,
			 *	wait for it and then retry.
			 *
			 *	A possible optimization: if the page
			 *	is known to be resident, we can ignore
			 *	pages that are absent (regardless of
			 *	whether they're busy).
			 */

			if (m->busy) {
				kern_return_t	wait_result;

				PAGE_ASSERT_WAIT(m, interruptible);
				vm_object_unlock(object);
				counter(c_vm_fault_page_block_busy_kernel++);
				thread_block((void (*)(void))0);

				wait_result = thread->wait_result;
				vm_object_lock(object);
				if (wait_result != THREAD_AWAKENED) {
					vm_fault_cleanup(object, first_m);
					if (wait_result == THREAD_RESTART)
						return(VM_FAULT_RETRY);
					else
						return(VM_FAULT_INTERRUPTED);
				}
				continue;
			}

			/*
			 *	If the page is in error, give up now.
			 */

			if (m->error) {
				if (error_code)
					*error_code = m->page_error;
				VM_PAGE_FREE(m);
				vm_fault_cleanup(object, first_m);
				return(VM_FAULT_MEMORY_ERROR);
			}

			/*
			 *	If the pager wants us to restart
			 *	at the top of the chain,
			 *	typically because it has moved the
			 *	page to another pager, then do so.
			 */

			if (m->restart) {
				VM_PAGE_FREE(m);
				vm_fault_cleanup(object, first_m);
				return(VM_FAULT_RETRY);
			}

			/*
			 *	If the page isn't busy, but is absent,
			 *	then it was deemed "unavailable".
			 */

			if (m->absent) {
				/*
				 * Remove the non-existent page (unless it's
				 * in the top object) and move on down to the
				 * next object (if there is one).
				 */

				access_required = VM_PROT_READ;
				next_object = object->shadow;
				if (next_object == VM_OBJECT_NULL) {
					vm_page_t real_m;

					assert(!must_be_resident);

					/*
					 * Absent page at bottom of shadow
					 * chain; zero fill the page we left
					 * busy in the first object, and flush
					 * the absent page.  But first we
					 * need to allocate a real page.
					 */

					real_m = vm_page_grab();
					if (real_m == VM_PAGE_NULL) {
						vm_fault_cleanup(object, first_m);
						return(VM_FAULT_MEMORY_SHORTAGE);
					}

					if (object != first_object) {
						VM_PAGE_FREE(m);
						vm_object_paging_end(object);
						vm_object_unlock(object);
						object = first_object;
						offset = first_offset;
						m = first_m;
						first_m = VM_PAGE_NULL;
						vm_object_lock(object);
					}

					VM_PAGE_FREE(m);
					assert(real_m->busy);
					vm_page_lock_queues();
					vm_page_insert(real_m, object, offset);
					vm_page_unlock_queues();
					m = real_m;

					/*
					 *  Drop the lock while zero filling
					 *  page.  Then break because this
					 *  is the page we wanted.  Checking
					 *  the page lock is a waste of time;
					 *  this page was either absent or
					 *  newly allocated -- in both cases
					 *  it can't be page locked by a pager.
					 */
					if (!no_zero_fill) {
						vm_object_unlock(object);
						vm_page_zero_fill(m);
						vm_stat.zero_fill_count++;
						vm_object_lock(object);
					}
					pmap_clear_modify(m->phys_addr);
					break;
				} else {
					if (must_be_resident) {
						vm_object_paging_end(object);
					} else if (object != first_object) {
						vm_object_paging_end(object);
						VM_PAGE_FREE(m);
					} else {
						first_m = m;
						m->absent = FALSE;
						m->unusual = FALSE;
						vm_object_absent_release(object);
						m->busy = TRUE;

						vm_page_lock_queues();
						VM_PAGE_QUEUES_REMOVE(m);
						vm_page_unlock_queues();
					}
					offset += object->shadow_offset;
					vm_object_lock(next_object);
					vm_object_unlock(object);
					object = next_object;
					vm_object_paging_begin(object);
					continue;
				}
			}

			/*
			 *	If the desired access to this page has
			 *	been locked out, request that it be unlocked.
			 */

			if (access_required & m->page_lock) {
				if ((access_required & m->unlock_request) != access_required) {
					vm_prot_t	new_unlock_request;
					kern_return_t	rc;
					
					if (!object->pager_ready) {
						vm_object_assert_wait(object,
							VM_OBJECT_EVENT_PAGER_READY,
							interruptible);
						goto block_and_backoff;
					}

					new_unlock_request = m->unlock_request =
						(access_required | m->unlock_request);
					vm_object_unlock(object);
					if ((rc = memory_object_data_unlock(
						object->pager,
						object->pager_request,
						offset + object->paging_offset,
						PAGE_SIZE,
						new_unlock_request))
					     != KERN_SUCCESS) {
						if (vm_fault_debug)
					     	    printf("vm_fault: memory_object_data_unlock failed\n");
						vm_object_lock(object);
						vm_fault_cleanup(object, first_m);
						return((rc == MACH_SEND_INTERRUPTED) ?
							VM_FAULT_INTERRUPTED :
							VM_FAULT_MEMORY_ERROR);
					}
					vm_object_lock(object);
					continue;
				}

				PAGE_ASSERT_WAIT(m, interruptible);
				goto block_and_backoff;
			}
#if	NORMA_VM
			if (object != first_object &&
			    object->copy_strategy == MEMORY_OBJECT_COPY_CALL) {
				assert(access_required == VM_PROT_READ);

				if (m->unlock_request != VM_PROT_NONE) {
					/*
					 * Page is being unlocked. Wait for the
					 * lock completed which will copy the
					 * page in the copy object.
					 */
					counter(c_vm_fault_wait_on_unlock++);
					PAGE_ASSERT_WAIT(m, interruptible);
					goto block_and_backoff;
				}

				if (m->lock_supplied &&
				    (m->page_lock & VM_PROT_WRITE) == 0) {
					/* 
					 * Page has probably been copied to
					 * copy object by pager via copy_call.
					 * This can happen when 2 threads fault
					 * concurrently. The first one scans
					 * the shadow chain until it reaches
					 * the real page. But at the same time
					 * the other thread faults (Write
					 * access) on the real page and the
					 * pager pushes the original page into
					 * the copy object but the first thread
					 * didn't have a chance to see it.
					 */
					vm_fault_cleanup(object, first_m);
					counter(c_vm_fault_retry_on_w_prot++);
					return(VM_FAULT_RETRY);
				}
			}
#endif	/* NORMA_VM */
			/*
			 *	We mark the page busy and leave it on
			 *	the pageout queues.  If the pageout
			 *	deamon comes across it, then it will
			 *	remove the page.
			 */

#if	!VM_FAULT_STATIC_CONFIG
			if (!software_reference_bits) {
				vm_page_lock_queues();
				if (m->inactive)
					vm_stat.reactivations++;

				VM_PAGE_QUEUES_REMOVE(m);
				vm_page_unlock_queues();
			}
#endif
			assert(!m->busy);
			m->busy = TRUE;
			assert(!m->absent);
			break;
		}

		look_for_page =
			(object->pager_created)
#if	MACH_PAGEMAP
			&& (vm_external_state_get(&object->existence_info,
						  offset +
						  object->paging_offset) !=
			 VM_EXTERNAL_STATE_ABSENT)
#endif	/* MACH_PAGEMAP */
			 ;

		if ((look_for_page || (object == first_object))
				 && !must_be_resident) {
			/*
			 *	Allocate a new page for this object/offset
			 *	pair.
			 */

			m = vm_page_grab_fictitious();
			if (m == VM_PAGE_NULL) {
				vm_fault_cleanup(object, first_m);
				return(VM_FAULT_FICTITIOUS_SHORTAGE);
			}

			vm_page_lock_queues();
			vm_page_insert(m, object, offset);
			vm_page_unlock_queues();
		}

		if (look_for_page && !must_be_resident) {
			kern_return_t	rc;

			/*
			 *	If the memory manager is not ready, we
			 *	cannot make requests.
			 */
			if (!object->pager_ready) {
				vm_object_assert_wait(object,
					VM_OBJECT_EVENT_PAGER_READY,
					interruptible);
				VM_PAGE_FREE(m);
				goto block_and_backoff;
			}

			if (object->internal) {
				/*
				 *	Requests to the default pager
				 *	must reserve a real page in advance,
				 *	because the pager's data-provided
				 *	won't block for pages.
				 */

				if (m->fictitious && !vm_page_convert(m)) {
					VM_PAGE_FREE(m);
					vm_fault_cleanup(object, first_m);
					return(VM_FAULT_MEMORY_SHORTAGE);
				}
			} else if (object->absent_count >
						vm_object_absent_max) {
				/*
				 *	If there are too many outstanding page
				 *	requests pending on this object, we
				 *	wait for them to be resolved now.
				 */

				vm_object_absent_assert_wait(object, interruptible);
				VM_PAGE_FREE(m);
				goto block_and_backoff;
			}

			/*
			 *	Indicate that the page is waiting for data
			 *	from the memory manager.
			 */

			m->absent = TRUE;
			m->unusual = TRUE;
			object->absent_count++;

			/*
			 *	We have a busy page, so we can
			 *	release the object lock.
			 */
			vm_object_unlock(object);

			/*
			 *	Call the memory manager to retrieve the data.
			 */

			vm_stat.pageins++;

			/*
			 *	If this object uses a copy_call strategy,
			 *	and we are interested in a copy of this object
			 *	(having gotten here only by following a
			 *	shadow chain), then tell the memory manager
			 *	via a flag added to the desired_access
			 *	parameter, so that it can detect a race
			 *	between our walking down the shadow chain
			 *	and its pushing pages up into a copy of
			 *	the object that it manages.
			 */

			if (object->copy_strategy == MEMORY_OBJECT_COPY_CALL &&
			    object != first_object) {
				wants_copy_flag = VM_PROT_WANTS_COPY;
			} else {
				wants_copy_flag = VM_PROT_NONE;
			}

			rc = memory_object_data_request(object->pager, 
							object->pager_request,
							m->offset +
							object->paging_offset, 
							PAGE_SIZE,
							access_required |
							wants_copy_flag);
			if (rc != KERN_SUCCESS) {
				if (rc != MACH_SEND_INTERRUPTED
				    && vm_fault_debug)
					printf("%s(0x%x, 0x%x, 0x%x, 0x%x, 0x%x) failed, rc=%d, object=0x%x\n",
						"memory_object_data_request",
						object->pager,
						object->pager_request,
						m->offset + object->paging_offset, 
						PAGE_SIZE, access_required,
						rc, object);
				/*
				 *	Don't want to leave a busy page around,
				 *	but the data request may have blocked,
				 *	so check if it's still there and busy.
				 */
				vm_object_lock(object);
				if (m == vm_page_lookup(object,offset) &&
				    m->absent && m->busy)
					VM_PAGE_FREE(m);
				vm_fault_cleanup(object, first_m);
				return((rc == MACH_SEND_INTERRUPTED) ?
					VM_FAULT_INTERRUPTED :
					VM_FAULT_MEMORY_ERROR);
			}
			
			/*
			 * Retry with same object/offset, since new data may
			 * be in a different page (i.e., m is meaningless at
			 * this point).
			 */
			vm_object_lock(object);
			continue;
		}

		/*
		 * The only case in which we get here is if
		 * object has no pager (or unwiring).  If the pager doesn't
		 * have the page this is handled in the m->absent case above
		 * (and if you change things here you should look above).
		 */
		if (object == first_object)
			first_m = m;
		else
			assert(m == VM_PAGE_NULL);

		/*
		 *	Move on to the next object.  Lock the next
		 *	object before unlocking the current one.
		 */
		access_required = VM_PROT_READ;

		offset += object->shadow_offset;
		next_object = object->shadow;
		if (next_object == VM_OBJECT_NULL) {
			assert(!must_be_resident);

			/*
			 *	If there's no object left, fill the page
			 *	in the top object with zeros.  But first we
			 *	need to allocate a real page.
			 */

			if (object != first_object) {
				vm_object_paging_end(object);
				vm_object_unlock(object);

				object = first_object;
				offset = first_offset;
				vm_object_lock(object);
			}

			m = first_m;
			assert(m->object == object);
			first_m = VM_PAGE_NULL;

			if (m->fictitious && !vm_page_convert(m)) {
				VM_PAGE_FREE(m);
				vm_fault_cleanup(object, VM_PAGE_NULL);
				return(VM_FAULT_MEMORY_SHORTAGE);
			}

			if (!no_zero_fill) {
				vm_object_unlock(object);
				vm_page_zero_fill(m);
				vm_stat.zero_fill_count++;
				vm_object_lock(object);
			}
			pmap_clear_modify(m->phys_addr);
			break;
		}
		else {
			vm_object_lock(next_object);
			if ((object != first_object) || must_be_resident)
				vm_object_paging_end(object);
			vm_object_unlock(object);
			object = next_object;
			vm_object_paging_begin(object);
		}
	}

	/*
	 *	PAGE HAS BEEN FOUND.
	 *
	 *	This page (m) is:
	 *		busy, so that we can play with it;
	 *		not absent, so that nobody else will fill it;
	 *		possibly eligible for pageout;
	 *
	 *	The top-level page (first_m) is:
	 *		VM_PAGE_NULL if the page was found in the
	 *		 top-level object;
	 *		busy, not absent, and ineligible for pageout.
	 *
	 *	The current object (object) is locked.  A paging
	 *	reference is held for the current and top-level
	 *	objects.
	 */

#if	EXTRA_ASSERTIONS
	assert(m->busy && !m->absent);
	assert((first_m == VM_PAGE_NULL) ||
		(first_m->busy && !first_m->absent &&
		 !first_m->active && !first_m->inactive));
#endif	/* EXTRA_ASSERTIONS */

	/*
	 *	If the page is being written, but isn't
	 *	already owned by the top-level object,
	 *	we have to copy it into a new page owned
	 *	by the top-level object.
	 */

	if (object != first_object) {
	    	/*
		 *	We only really need to copy if we
		 *	want to write it.
		 */

	    	if (fault_type & VM_PROT_WRITE) {
			vm_page_t copy_m;

			assert(!must_be_resident);

			/*
			 *	If we try to collapse first_object at this
			 *	point, we may deadlock when we try to get
			 *	the lock on an intermediate object (since we
			 *	have the bottom object locked).  We can't
			 *	unlock the bottom object, because the page
			 *	we found may move (by collapse) if we do.
			 *
			 *	Instead, we first copy the page.  Then, when
			 *	we have no more use for the bottom object,
			 *	we unlock it and try to collapse.
			 *
			 *	Note that we copy the page even if we didn't
			 *	need to... that's the breaks.
			 */

			/*
			 *	Allocate a page for the copy
			 */
			copy_m = vm_page_grab();
			if (copy_m == VM_PAGE_NULL) {
				RELEASE_PAGE(m);
				vm_fault_cleanup(object, first_m);
				return(VM_FAULT_MEMORY_SHORTAGE);
			}

			vm_object_unlock(object);

			vm_page_copy(m, copy_m);
			vm_object_lock(object);

			/*
			 *	If another map is truly sharing this
			 *	page with us, we have to flush all
			 *	uses of the original page, since we
			 *	can't distinguish those which want the
			 *	original from those which need the
			 *	new copy.
			 *
			 *	XXXO If we know that only one map has
			 *	access to this page, then we could
			 *	avoid the pmap_page_protect() call.
			 */

			vm_page_lock_queues();
			pmap_page_protect(m->phys_addr, VM_PROT_NONE);
			vm_page_deactivate(m);
			vm_page_unlock_queues();

			/*
			 *	We no longer need the old page or object.
			 */

			PAGE_WAKEUP_DONE(m);
			vm_object_paging_end(object);
			vm_object_unlock(object);

			vm_stat.cow_faults++;
			object = first_object;
			offset = first_offset;

			vm_object_lock(object);
			VM_PAGE_FREE(first_m);
			first_m = VM_PAGE_NULL;
			assert(copy_m->busy);
			vm_page_lock_queues();
			vm_page_insert(copy_m, object, offset);
			vm_page_unlock_queues();
			m = copy_m;

			/*
			 *	Now that we've gotten the copy out of the
			 *	way, let's try to collapse the top object.
			 *	But we have to play ugly games with
			 *	paging_in_progress to do that...
			 */

			vm_object_paging_end(object);
			vm_object_collapse(object);
			vm_object_paging_begin(object);
		}
		else {
		    	*protection &= (~VM_PROT_WRITE);
		}
	}
	
#if	NORMA_VM
	/*
	 *	If the old page was in use by any users of any copy-call
	 *	copies of the object, the page must be removed from all
	 *	pmaps.  (We can't know which pmaps use it.)
	 */
	if (object->copy_strategy == MEMORY_OBJECT_COPY_CALL) {

		/*
		 *	If the page is being written, but hasn't been
		 *	copied to the copy-object, we have to copy it there.
		 */
		if ((fault_type & VM_PROT_WRITE) == 0) {
			*protection &= ~VM_PROT_WRITE;

		} else if (!must_be_resident && !m->lock_supplied) {
			vm_prot_t	new_unlock_request;
			kern_return_t	rc;

			vm_page_lock_queues();
			pmap_page_protect(m->phys_addr, VM_PROT_NONE);
			vm_page_unlock_queues();

			/*
			 *	If the page was guaranteed to be resident or
			 *	has been supplied by the pager, it must already
			 *	have been copied. Otherwise, it is mandatory to
			 *	call m_o_data_unlock so that the pager may ask
			 *	for the content of the page if it really needs
			 *	to copy it. To be considered successful, the
			 *	vm_fault_page needs to be restarted to wait for
			 *	the pager answer (m_o_lock_request).
			 */

			assert(object->pager_ready);
			new_unlock_request = m->unlock_request |= fault_type;
			m->page_lock = VM_PROT_WRITE;
			m->unusual = TRUE;
			PAGE_WAKEUP_DONE(m);
			vm_object_unlock(object);
			rc = memory_object_data_unlock(object->pager,
						       object->pager_request,
						       offset +
						       object->paging_offset,
						       PAGE_SIZE,
						       new_unlock_request);
			vm_object_lock(object);
			vm_fault_cleanup(object, first_m);
			if (rc == KERN_SUCCESS)
				return(VM_FAULT_RETRY);

			if (vm_fault_debug)
				printf("vm_fault_page: memory_object_data_unlock failed\n");
			return((rc == MACH_SEND_INTERRUPTED) ?
			       VM_FAULT_INTERRUPTED :
			       VM_FAULT_MEMORY_ERROR);
		}
	}

#else	/* NORMA_VM */

	/*
	 *	Now check whether the page needs to be pushed into the
	 *	copy object.  The use of asymmetric copy on write for
	 *	shared temporary objects means that we may do two copies to
	 *	satisfy the fault; one above to get the page from a
	 *	shadowed object, and one here to push it into the copy.
	 */

	while (first_object->copy_strategy == MEMORY_OBJECT_COPY_DELAY &&
	       (copy_object = first_object->copy) != VM_OBJECT_NULL) {
		vm_offset_t	copy_offset;
		vm_page_t	copy_m;

		/*
		 *	If the page is being written, but hasn't been
		 *	copied to the copy-object, we have to copy it there.
		 */

		if ((fault_type & VM_PROT_WRITE) == 0) {
			*protection &= ~VM_PROT_WRITE;
			break;
		}

		/*
		 *	If the page was guaranteed to be resident,
		 *	we must have already performed the copy.
		 */

		if (must_be_resident)
			break;

		/*
		 *	Try to get the lock on the copy_object.
		 */
		if (!vm_object_lock_try(copy_object)) {
			vm_object_unlock(object);

			mutex_pause();	/* wait a bit */

			vm_object_lock(object);
			continue;
		}

		/*
		 *	Make another reference to the copy-object,
		 *	to keep it from disappearing during the
		 *	copy.
		 */
		assert(copy_object->ref_count > 0);
		copy_object->ref_count++;
		/*
		 *	Does the page exist in the copy?
		 */
		copy_offset = first_offset - copy_object->shadow_offset;
		copy_m = vm_page_lookup(copy_object, copy_offset);
		if (copy_m != VM_PAGE_NULL) {
			if (copy_m->busy) {
				/*
				 *	If the page is being brought
				 *	in, wait for it and then retry.
				 */
				PAGE_ASSERT_WAIT(copy_m, interruptible);
				RELEASE_PAGE(m);
				copy_object->ref_count--;
				assert(copy_object->ref_count > 0);
				vm_object_unlock(copy_object);
				goto block_and_backoff;
			}
		}
		else {
			/*
			 *	Allocate a page for the copy
			 */
			copy_m = vm_page_alloc(copy_object, copy_offset);
			if (copy_m == VM_PAGE_NULL) {
				RELEASE_PAGE(m);
				copy_object->ref_count--;
				assert(copy_object->ref_count > 0);
				vm_object_unlock(copy_object);
				vm_fault_cleanup(object, first_m);
				return(VM_FAULT_MEMORY_SHORTAGE);
			}

			/*
			 *	Must copy page into copy-object.
			 */

			vm_page_copy(m, copy_m);
			
			/*
			 *	If the old page was in use by any users
			 *	of the copy-object, it must be removed
			 *	from all pmaps.  (We can't know which
			 *	pmaps use it.)
			 */

			vm_page_lock_queues();
			pmap_page_protect(m->phys_addr, VM_PROT_NONE);
			copy_m->dirty = TRUE;
			vm_page_unlock_queues();

			/*
			 *	If there's a pager, then immediately
			 *	page out this page, using the "initialize"
			 *	option.  Else, we use the copy.
			 */

		 	if (!copy_object->pager_created) {
				vm_page_lock_queues();
				vm_page_activate(copy_m);
				vm_page_unlock_queues();
				PAGE_WAKEUP_DONE(copy_m);
			} else {
				assert(copy_m->busy == TRUE);

				/*
				 *	The page is already ready for pageout:
				 *	not on pageout queues and busy.
				 *	Unlock everything except the
				 *	copy_object itself.
				 */

				vm_object_unlock(object);

				/*
				 *	Write the page to the copy-object,
				 *	flushing it from the kernel.
				 */

				vm_pageout_page(copy_m, TRUE, TRUE);

				/*
				 *	Since the pageout may have
				 *	temporarily dropped the
				 *	copy_object's lock, we
				 *	check whether we'll have
				 *	to deallocate the hard way.
				 */

				if ((copy_object->shadow != object) ||
				    (copy_object->ref_count == 1)) {
					vm_object_unlock(copy_object);
					vm_object_deallocate(copy_object);
					vm_object_lock(object);
					continue;
				}

				/*
				 *	Pick back up the old object's
				 *	lock.  [It is safe to do so,
				 *	since it must be deeper in the
				 *	object tree.]
				 */

				vm_object_lock(object);
			}

			/*
			 *	Because we're pushing a page upward
			 *	in the object tree, we must restart
			 *	any faults that are waiting here.
			 *	[Note that this is an expansion of
			 *	PAGE_WAKEUP that uses the THREAD_RESTART
			 *	wait result].  Can't turn off the page's
			 *	busy bit because we're not done with it.
			 */
			 
			if (m->wanted) {
				m->wanted = FALSE;
				thread_wakeup_with_result((int) m,
					THREAD_RESTART);
			}
		}

		/*
		 *	The reference count on copy_object must be
		 *	at least 2: one for our extra reference,
		 *	and at least one from the outside world
		 *	(we checked that when we last locked
		 *	copy_object).
		 */
		copy_object->ref_count--;
		assert(copy_object->ref_count > 0);
		vm_object_unlock(copy_object);

		break;
	}
#endif	/* NORMA_VM */

	*result_page = m;

	*top_page = first_m;

	/*
	 *	If the page can be written, assume that it will be.
	 *	[Earlier, we restrict the permission to allow write
	 *	access only if the fault so required, so we don't
	 *	mark read-only data as dirty.]
	 */

#if	!VM_FAULT_STATIC_CONFIG
	if (vm_fault_dirty_handling && (*protection & VM_PROT_WRITE))
		m->dirty = TRUE;
#endif
	return(VM_FAULT_SUCCESS);

    block_and_backoff:
	vm_fault_cleanup(object, first_m);

	counter(c_vm_fault_page_block_backoff_kernel++);
	thread_block((void (*)(void))0);

	if (thread->wait_result == THREAD_AWAKENED)
		return VM_FAULT_RETRY;
	else
		return VM_FAULT_INTERRUPTED;

#undef	RELEASE_PAGE
}

/*
 *	Routine:	vm_fault
 *	Purpose:
 *		Handle page faults, including pseudo-faults
 *		used to change the wiring status of pages.
 *	Returns:
 *		Explicit continuations have been removed.
 *	Implementation:
 *		vm_fault and vm_fault_page save mucho state
 *		in the moral equivalent of a closure.  The state
 *		structure is allocated when first entering vm_fault
 *		and deallocated when leaving vm_fault.
 */

kern_return_t
vm_fault(
	vm_map_t	map,
	vm_offset_t	vaddr,
	vm_prot_t	fault_type,
	boolean_t	change_wiring)
{
	vm_map_version_t	version;	/* Map version for verificiation */
	boolean_t		wired;		/* Should mapping be wired down? */
	vm_object_t		object;		/* Top-level object */
	vm_offset_t		offset;		/* Top-level offset */
	vm_prot_t		prot;		/* Protection for mapping */
#if	!NORMA_VM
	vm_object_t		old_copy_object; /* Saved copy object */
#endif	/* !NORMA_VM */
	vm_page_t		result_page;	/* Result of vm_fault_page */
	vm_page_t		top_page;	/* Placeholder page */
	kern_return_t		kr;

	register
	vm_page_t		m;	/* Fast access to result_page */
	kern_return_t		error_code;	/* page error reasons */
     	thread_t		thread = current_thread();
	register
	vm_object_t		cur_object;
	register
	vm_offset_t		cur_offset;
	vm_page_t		cur_m;
	vm_object_t		new_object;

    RetryFault: ;

	/*
	 *	Find the backing store object and offset into
	 *	it to begin the search.
	 */

	vm_map_lock_read(map);
	kr = vm_map_lookup_locked(&map, vaddr, fault_type, &version,
				&object, &offset,
				&prot, &wired);

	if (kr != KERN_SUCCESS) {
		vm_map_unlock_read(map);
		goto done;
	}

	/*
	 *	If the page is wired, we must fault for the current protection
	 *	value, to avoid further faults.
	 */

	if (wired)
		fault_type = prot;

#if	VM_FAULT_CLASSIFY
	/*
	 *	Temporary data gathering code
	 */
	vm_fault_classify(object, offset, fault_type);
#endif

	/*
	 *	Fast fault code.  The basic idea is to do as much as
	 *	possible while holding the map lock and object locks.
	 *      Busy pages are not used until the object lock has to
	 *	be dropped to do something (copy, zero fill, pmap enter).
	 *	Similarly, paging references aren't acquired until that
	 *	point, and object references aren't used.
	 *
	 *	If we can figure out what to do
	 *	(zero fill, copy on write, pmap enter) while holding
	 *	the locks, then it gets done.  Otherwise, we give up,
	 *	and use the original fault path (which doesn't hold
	 *	the map lock, and relies on busy pages).
	 *	The give up cases include:
	 * 		- Have to talk to pager.
	 *		- Page is busy, absent or in error.
	 *		- Pager has locked out desired access.
	 *		- Fault needs to be restarted.
	 *		- Have to push page into copy object.
	 *
	 *	The code is an infinite loop that moves one level down
	 *	the shadow chain each time.  cur_object and cur_offset
	 * 	refer to the current object being examined. object and offset
	 *	are the original object from the map.  The loop is at the
	 *	top level if and only if object and cur_object are the same.
	 *
	 *	Invariants:  Map lock is held throughout.  Lock is held on
	 *		original object and cur_object (if different) when
	 *		continuing or exiting loop.
	 *
	 */

	cur_object = object;
	cur_offset = offset;

	while (TRUE) {
		m = vm_page_lookup(cur_object, cur_offset);
		if (m != VM_PAGE_NULL) {
			if (m->busy)
				break;

			if (m->unusual && ( m->error || m->restart ||
			    m->absent || fault_type & m->page_lock)) {

				/*
				 *	Unusual case. Give up.
				 */
				break;
			}

			/*
			 *	Two cases of map in faults:
			 *	    - At top level w/o copy object.
			 *	    - Read fault anywhere.
			 *		--> must disallow write.
			 */

			if (object == cur_object &&
			    object->copy == VM_OBJECT_NULL)
				goto FastMapInFault;

			if ((fault_type & VM_PROT_WRITE) == 0) {

				prot &= ~VM_PROT_WRITE;

				/*
				 *	Set up to map the page ...
				 *	mark the page busy, drop
				 *	locks and take a paging reference
				 *	on the object with the page.
				 */	

			  	if (object != cur_object) {
					vm_object_unlock(object);
					object = cur_object;
				}
FastMapInFault:
				m->busy = TRUE;

				vm_object_paging_begin(object);
				vm_object_unlock(object);

FastPmapEnter:
				/*
				 *	Check a couple of global reasons to
				 *	be conservative about write access.
				 *	Then do the pmap_enter.
				 */
#if	!VM_FAULT_STATIC_CONFIG
				if (vm_fault_dirty_handling
#if	MACH_KDB
				    || db_watchpoint_list
#endif
				    && (fault_type & VM_PROT_WRITE) == 0)
					prot &= ~VM_PROT_WRITE;
#else	/* STATIC_CONFIG */
#if	MACH_KDB
				if (db_watchpoint_list
				    && (fault_type & VM_PROT_WRITE) == 0)
					prot &= ~VM_PROT_WRITE;
#endif	/* MACH_KDB */
#endif	/* STATIC_CONFIG */

				PMAP_ENTER(vm_map_pmap(map), vaddr, m,
					   prot, wired);

				/*
				 *	Grab the object lock to manipulate
				 *	the page queues.  Change wiring
				 *	case is obvious.  In soft ref bits
				 *	case activate page only if it fell
				 *	off paging queues, otherwise just
				 *	activate it if it's inactive.
				 *
				 *	NOTE: original vm_fault code will
				 *	move active page to back of active
				 *	queue.  This code doesn't.
				 */

				vm_object_lock(object);
				vm_page_lock_queues();
				if (change_wiring) {
					if (wired)
						vm_page_wire(m);
					else
						vm_page_unwire(m);
				}
#if VM_FAULT_STATIC_CONFIG
				else {
					if (!m->active && !m->inactive)
						vm_page_activate(m);
					m->reference = TRUE;
				}
#else				
				else if (software_reference_bits) {
					if (!m->active && !m->inactive)
						vm_page_activate(m);
					m->reference = TRUE;
				}
				else if (!m->active) {
					vm_page_activate(m);
				}
#endif
				vm_page_unlock_queues();

				/*
				 *	That's it, clean up and return.
				 */

				PAGE_WAKEUP_DONE(m);
				vm_object_paging_end(object);
				vm_object_unlock(object);
				vm_map_unlock_read(map);
				return KERN_SUCCESS;
			}

			/*
			 *	Copy on write fault.  If objects match, then
			 *	object->copy must not be NULL (else control
			 *	would be in previous code block), and we
			 *	have a potential push into the copy object
			 *	with which we won't cope here.
			 */

			if (cur_object == object)
				break;
#if	NORMA_VM
			/*
			 *	If this page is in a copy call object and
			 *	is writeable, then we lost a race with the
			 *	pager and the page we want is a level up.
			 *	If the page is being unlocked we have to
			 *	block.  In both cases, bail out, and allow
			 *	the slow fault code to retry.
			 */
			if (cur_object->copy_strategy ==
						MEMORY_OBJECT_COPY_CALL &&
				 cur_object->copy != VM_OBJECT_NULL &&
				 (m->page_lock & VM_PROT_WRITE) == 0 ||
				 m->unlock_request != VM_PROT_NONE)
			 		break;
#endif /* NORMA_VM */

			/*
			 *	This is now a shadow based copy on write
			 *	fault -- it requires a copy up the shadow
			 *	chain.
			 *
			 *	Allocate a page in the original top level
			 *	object. Give up if allocate fails.  Also
			 *	need to remember current page, as it's the
			 *	source of the copy.
			 */

			cur_m = m;
			m = vm_page_alloc(object, offset);
			if (m == VM_PAGE_NULL) {
				break;
			}

			/*
			 *	Now do the copy.  Mark the source busy
			 *	and take out paging references on both
			 *	objects.
			 *
			 *	NOTE: This code holds the map lock across
			 *	the page copy.
			 */

			cur_m->busy = TRUE;

			vm_object_paging_begin(cur_object);
			vm_object_unlock(cur_object);
			vm_object_paging_begin(object);
			vm_object_unlock(object);

			vm_page_copy(cur_m, m);
			vm_stat.cow_faults++;

			/*
			 *	Now cope with the source page and object
			 *	If the top object has a ref count of 1
			 *	then no other map can access it, and hence
			 *	it's not necessary to do the pmap_page_protect.
			 */

			vm_object_lock(object);
			vm_object_lock(cur_object);

			vm_page_lock_queues();
			vm_page_deactivate(cur_m);
			if (object->ref_count != 1) 
				pmap_page_protect(cur_m->phys_addr,
						  VM_PROT_NONE);
			vm_page_unlock_queues();

			PAGE_WAKEUP_DONE(cur_m);
			vm_object_paging_end(cur_object);
			vm_object_unlock(cur_object);

			/*
			 *	Slight hack to call vm_object collapse
			 *	and then reuse common map in code.
			 *	note that the object lock was taken above.
			 */

			vm_object_paging_end(object);
			vm_object_collapse(object);
			vm_object_paging_begin(object);
			vm_object_unlock(object);

			goto FastPmapEnter;
		}
		else {

			/*
			 *	No page at cur_object, cur_offset
			 */

			if (cur_object->pager_created) {

				/*
				 *	Have to talk to the pager.  Give up.
				 */

				break;
			}

			if (cur_object->shadow == VM_OBJECT_NULL) {

				/*
				 *	Zero fill fault.  Page gets
				 *	filled in top object. Insert
				 *	page, then drop any lower lock.
				 *	Give up if no page.
				 */

				m = vm_page_alloc(object, offset);
				if (m == VM_PAGE_NULL) {
					break;
				}

			  	if (cur_object != object)
					vm_object_unlock(cur_object);

				vm_object_paging_begin(object);
				vm_object_unlock(object);

				/*
				 *	Now zero fill page and map it.
				 *	The pmap_clear modify is cheap,
				 *	even though the page is probably
				 *	going to be written soon.
				 *
				 *	NOTE: This code holds the map
				 *	lock across the zero fill.
				 */

				if (!map->no_zero_fill) {
					vm_page_zero_fill(m);
					vm_stat.zero_fill_count++;
					pmap_clear_modify(m->phys_addr);
				}

				goto FastPmapEnter;
		        }

			/*
			 *	On to the next level
			 */

			cur_offset += cur_object->shadow_offset;
			new_object = cur_object->shadow;
			vm_object_lock(new_object);
			if (cur_object != object)
				vm_object_unlock(cur_object);
			cur_object = new_object;

			continue;
		}
	}

	/*
	 *	Cleanup from fast fault failure.  Drop any object
	 *	lock other than original and drop map lock.
	 */

	if (object != cur_object)
		vm_object_unlock(cur_object);
	vm_map_unlock_read(map);

   	/*
	 *	Make a reference to this object to
	 *	prevent its disposal while we are messing with
	 *	it.  Once we have the reference, the map is free
	 *	to be diddled.  Since objects reference their
	 *	shadows (and copies), they will stay around as well.
	 */

	assert(object->ref_count > 0);
	object->ref_count++;
	vm_object_paging_begin(object);

	kr = vm_fault_page(object, offset, fault_type,
				   (change_wiring && !wired), !change_wiring,
				   &prot, &result_page, &top_page,
				   &error_code, map->no_zero_fill);

	/*
	 *	If we didn't succeed, lose the object reference immediately.
	 */

	if (kr != VM_FAULT_SUCCESS)
		vm_object_deallocate(object);

	/*
	 *	See why we failed, and take corrective action.
	 */

	switch (kr) {
		case VM_FAULT_SUCCESS:
			break;
		case VM_FAULT_RETRY:
			goto RetryFault;
		case VM_FAULT_INTERRUPTED:
			kr = KERN_SUCCESS;
			goto done;
		case VM_FAULT_MEMORY_SHORTAGE:
			VM_PAGE_WAIT();
			goto RetryFault;
		case VM_FAULT_FICTITIOUS_SHORTAGE:
			vm_page_more_fictitious();
			goto RetryFault;
		case VM_FAULT_MEMORY_ERROR:
			if (error_code)
				kr = error_code;
			else
				kr = KERN_MEMORY_ERROR;
			goto done;
	}

	m = result_page;

	assert((change_wiring && !wired) ?
	       (top_page == VM_PAGE_NULL) :
	       ((top_page == VM_PAGE_NULL) == (m->object == object)));

	/*
	 *	How to clean up the result of vm_fault_page.  This
	 *	happens whether the mapping is entered or not.
	 */

#define UNLOCK_AND_DEALLOCATE				\
	MACRO_BEGIN					\
	vm_fault_cleanup(m->object, top_page);		\
	vm_object_deallocate(object);			\
	MACRO_END

	/*
	 *	What to do with the resulting page from vm_fault_page
	 *	if it doesn't get entered into the physical map:
	 */

#define RELEASE_PAGE(m)					\
	MACRO_BEGIN					\
	PAGE_WAKEUP_DONE(m);				\
	vm_page_lock_queues();				\
	if (!m->active && !m->inactive)			\
		vm_page_activate(m);			\
	vm_page_unlock_queues();			\
	MACRO_END

	/*
	 *	We must verify that the maps have not changed
	 *	since our last lookup.
	 */

#if	!NORMA_VM
	old_copy_object = m->object->copy;
#endif	/* !NORMA_VM */

	vm_object_unlock(m->object);
	while (!vm_map_verify(map, &version)) {
		vm_object_t	retry_object;
		vm_offset_t	retry_offset;
		vm_prot_t	retry_prot;

		/*
		 *	To avoid trying to write_lock the map while another
		 *	thread has it read_locked (in vm_map_pageable), we
		 *	do not try for write permission.  If the page is
		 *	still writable, we will get write permission.  If it
		 *	is not, or has been marked needs_copy, we enter the
		 *	mapping without write permission, and will merely
		 *	take another fault.
		 */
		vm_map_lock_read(map);
		kr = vm_map_lookup_locked(&map, vaddr,
				   fault_type & ~VM_PROT_WRITE, &version,
				   &retry_object, &retry_offset, &retry_prot,
				   &wired);
		vm_map_unlock_read(map);

		if (kr != KERN_SUCCESS) {
			vm_object_lock(m->object);
			RELEASE_PAGE(m);
			UNLOCK_AND_DEALLOCATE;
			goto done;
		}

		vm_object_unlock(retry_object);
		vm_object_lock(m->object);

		if ((retry_object != object) ||
		    (retry_offset != offset)) {
			RELEASE_PAGE(m);
			UNLOCK_AND_DEALLOCATE;
			goto RetryFault;
		}

		/*
		 *	Check whether the protection has changed or the object
		 *	has been copied while we left the map unlocked.
		 */
		prot &= retry_prot;
		vm_object_unlock(m->object);
	}
	vm_object_lock(m->object);

#if	NORMA_VM
	/*
	 * Write access grant to pages of COPY_CALL objects must have
	 * page_lock supplied by the pager.
	 */
	assert(m->object->copy_strategy != MEMORY_OBJECT_COPY_CALL ||
	       (prot & VM_PROT_WRITE) == 0 || m->lock_supplied);
#else	/* NORMA_VM */
	/*
	 *	If the copy object changed while the top-level object
	 *	was unlocked, then we must take away write permission.
	 */

	if (m->object->copy != old_copy_object)
		prot &= ~VM_PROT_WRITE;
#endif	/* NORMA_VM */

	/*
	 *	If we want to wire down this page, but no longer have
	 *	adequate permissions, we must start all over.
	 */

	if (wired && (prot != fault_type)) {
		vm_map_verify_done(map, &version);
		RELEASE_PAGE(m);
		UNLOCK_AND_DEALLOCATE;
		goto RetryFault;
	}

	/*
	 *	It's critically important that a wired-down page be faulted
	 *	only once in each map for which it is wired.
	 */
	vm_object_unlock(m->object);

	/*
	 *	Put this page into the physical map.
	 *	We had to do the unlock above because pmap_enter
	 *	may cause other faults.  The page may be on
	 *	the pageout queues.  If the pageout daemon comes
	 *	across the page, it will remove it from the queues.
	 */

	PMAP_ENTER(map->pmap, vaddr, m, prot, wired);

	/*
	 *	If the page is not wired down and isn't already
	 *	on a pageout queue, then put it where the
	 *	pageout daemon can find it.
	 */
	vm_object_lock(m->object);
	vm_page_lock_queues();
	if (change_wiring) {
		if (wired)
			vm_page_wire(m);
		else
			vm_page_unwire(m);
	}
#if	VM_FAULT_STATIC_CONFIG
	else {
		if (!m->active && !m->inactive)
			vm_page_activate(m);
		m->reference = TRUE;
	}
#else
	else if (software_reference_bits) {
		if (!m->active && !m->inactive)
			vm_page_activate(m);
		m->reference = TRUE;
	} else {
		vm_page_activate(m);
	}
#endif
	vm_page_unlock_queues();

	/*
	 *	Unlock everything, and return
	 */

	vm_map_verify_done(map, &version);
	PAGE_WAKEUP_DONE(m);
	kr = KERN_SUCCESS;
	UNLOCK_AND_DEALLOCATE;

#undef	UNLOCK_AND_DEALLOCATE
#undef	RELEASE_PAGE

    done:
	return(kr);
}

/*
 *	vm_fault_wire:
 *
 *	Wire down a range of virtual addresses in a map.
 */
kern_return_t
vm_fault_wire(
	vm_map_t	map,
	vm_map_entry_t	entry)
{

	register vm_offset_t	va;
	register pmap_t		pmap;
	register vm_offset_t	end_addr = entry->vme_end;
	register kern_return_t	rc;

	assert(entry->in_transition);
	pmap = vm_map_pmap(map);

	/*
	 *	Inform the physical mapping system that the
	 *	range of addresses may not fault, so that
	 *	page tables and such can be locked down as well.
	 */

	pmap_pageable(pmap, entry->vme_start, end_addr, FALSE);

	/*
	 *	We simulate a fault to get the page and enter it
	 *	in the physical map.
	 */

	for (va = entry->vme_start; va < end_addr; va += PAGE_SIZE) {
		if ((rc = vm_fault_wire_fast(map, va, entry)) != KERN_SUCCESS) {
			rc = vm_fault(map, va, VM_PROT_NONE, TRUE);
		}

		if (rc != KERN_SUCCESS) {
			struct vm_map_entry	tmp_entry = *entry;

			/* unwire wired pages */
			tmp_entry.vme_end = va;
			vm_fault_unwire(map, &tmp_entry, FALSE);

			return rc;
		}
	}
	return KERN_SUCCESS;
}

/*
 *	vm_fault_unwire:
 *
 *	Unwire a range of virtual addresses in a map.
 */
void
vm_fault_unwire(
	vm_map_t	map,
	vm_map_entry_t	entry,
	boolean_t	deallocate)
{
	register vm_offset_t	va;
	register pmap_t		pmap;
	register vm_offset_t	end_addr = entry->vme_end;
	vm_object_t		object;

	pmap = vm_map_pmap(map);

	object = (entry->is_sub_map)
			? VM_OBJECT_NULL : entry->object.vm_object;

	/*
	 *	Since the pages are wired down, we must be able to
	 *	get their mappings from the physical map system.
	 */

	for (va = entry->vme_start; va < end_addr; va += PAGE_SIZE) {
		pmap_change_wiring(pmap, va, FALSE);

		if (object == VM_OBJECT_NULL) {
			(void) vm_fault(map, va, VM_PROT_NONE, TRUE);
		} else {
		 	vm_prot_t	prot;
			vm_page_t	result_page;
			vm_page_t	top_page;
			vm_object_t	result_object;
			vm_fault_return_t result;

			do {
				prot = VM_PROT_NONE;

				vm_object_lock(object);
				vm_object_paging_begin(object);
			 	result = vm_fault_page(object,
						entry->offset +
						  (va - entry->vme_start),
						VM_PROT_NONE, TRUE,
						FALSE, &prot,
						&result_page,
						&top_page,
						0, map->no_zero_fill);
			} while (result == VM_FAULT_RETRY);

			if (result != VM_FAULT_SUCCESS)
				panic("vm_fault_unwire: failure");

			result_object = result_page->object;
			if (deallocate) {
				assert(!result_page->fictitious);
				pmap_page_protect(result_page->phys_addr,
						VM_PROT_NONE);
				VM_PAGE_FREE(result_page);
			} else {
				vm_page_lock_queues();
				vm_page_unwire(result_page);
				vm_page_unlock_queues();
				PAGE_WAKEUP_DONE(result_page);
			}

			vm_fault_cleanup(result_object, top_page);
		}
	}

	/*
	 *	Inform the physical mapping system that the range
	 *	of addresses may fault, so that page tables and
	 *	such may be unwired themselves.
	 */

	pmap_pageable(pmap, entry->vme_start, end_addr, TRUE);

}

/*
 *	vm_fault_wire_fast:
 *
 *	Handle common case of a wire down page fault at the given address.
 *	If successful, the page is inserted into the associated physical map.
 *	The map entry is passed in to avoid the overhead of a map lookup.
 *
 *	NOTE: the given address should be truncated to the
 *	proper page address.
 *
 *	KERN_SUCCESS is returned if the page fault is handled; otherwise,
 *	a standard error specifying why the fault is fatal is returned.
 *
 *	The map in question must be referenced, and remains so.
 *	Caller has a read lock on the map.
 *
 *	This is a stripped version of vm_fault() for wiring pages.  Anything
 *	other than the common case will return KERN_FAILURE, and the caller
 *	is expected to call vm_fault().
 */
kern_return_t
vm_fault_wire_fast(
	vm_map_t	map,
	vm_offset_t	va,
	vm_map_entry_t	entry)
{
	vm_object_t		object;
	vm_offset_t		offset;
	register vm_page_t	m;
	vm_prot_t		prot;

	vm_stat.faults++;		/* needs lock XXX */
/*
 *	Recovery actions
 */

#undef	RELEASE_PAGE
#define RELEASE_PAGE(m)	{				\
	PAGE_WAKEUP_DONE(m);				\
	vm_page_lock_queues();				\
	vm_page_unwire(m);				\
	vm_page_unlock_queues();			\
}


#undef	UNLOCK_THINGS
#define UNLOCK_THINGS	{				\
	object->paging_in_progress--;			\
	vm_object_unlock(object);			\
}

#undef	UNLOCK_AND_DEALLOCATE
#define UNLOCK_AND_DEALLOCATE	{			\
	UNLOCK_THINGS;					\
	vm_object_deallocate(object);			\
}
/*
 *	Give up and have caller do things the hard way.
 */

#define GIVE_UP {					\
	UNLOCK_AND_DEALLOCATE;				\
	return(KERN_FAILURE);				\
}


	/*
	 *	If this entry is not directly to a vm_object, bail out.
	 */
	if (entry->is_sub_map)
		return(KERN_FAILURE);

	/*
	 *	Find the backing store object and offset into it.
	 */

	object = entry->object.vm_object;
	offset = (va - entry->vme_start) + entry->offset;
	prot = entry->protection;

   	/*
	 *	Make a reference to this object to prevent its
	 *	disposal while we are messing with it.
	 */

	vm_object_lock(object);
	assert(object->ref_count > 0);
	object->ref_count++;
	object->paging_in_progress++;

	/*
	 *	INVARIANTS (through entire routine):
	 *
	 *	1)	At all times, we must either have the object
	 *		lock or a busy page in some object to prevent
	 *		some other thread from trying to bring in
	 *		the same page.
	 *
	 *	2)	Once we have a busy page, we must remove it from
	 *		the pageout queues, so that the pageout daemon
	 *		will not grab it away.
	 *
	 */

	/*
	 *	Look for page in top-level object.  If it's not there or
	 *	there's something going on, give up.
	 */
	m = vm_page_lookup(object, offset);
	if ((m == VM_PAGE_NULL) || (m->busy) || 
	    (m->unusual && ( m->error || m->restart || m->absent ||
				prot & m->page_lock))) {

		GIVE_UP;
	}

	/*
	 *	Wire the page down now.  All bail outs beyond this
	 *	point must unwire the page.  
	 */

	vm_page_lock_queues();
	vm_page_wire(m);
	vm_page_unlock_queues();

	/*
	 *	Mark page busy for other threads.
	 */
	assert(!m->busy);
	m->busy = TRUE;
	assert(!m->absent);

#if	!NORMA_VM
	/*
	 *	Give up if the page is being written and there's a copy object
	 */
	if ((object->copy != VM_OBJECT_NULL) && (prot & VM_PROT_WRITE)) {
		RELEASE_PAGE(m);
		GIVE_UP;
	}
#endif	/* !NORMA_VM */

	/*
	 *	Put this page into the physical map.
	 *	We have to unlock the object because pmap_enter
	 *	may cause other faults.   
	 */
	vm_object_unlock(object);

	PMAP_ENTER(map->pmap, va, m, prot, TRUE);

	/*
	 *	Must relock object so that paging_in_progress can be cleared.
	 */
	vm_object_lock(object);

	/*
	 *	Unlock everything, and return
	 */

	PAGE_WAKEUP_DONE(m);
	UNLOCK_AND_DEALLOCATE;

	return(KERN_SUCCESS);

}

/*
 *	Routine:	vm_fault_copy_cleanup
 *	Purpose:
 *		Release a page used by vm_fault_copy.
 */

void
vm_fault_copy_cleanup(
	vm_page_t	page,
	vm_page_t	top_page)
{
	vm_object_t	object = page->object;

	vm_object_lock(object);
	PAGE_WAKEUP_DONE(page);
	vm_page_lock_queues();
	if (!page->active && !page->inactive)
		vm_page_activate(page);
	vm_page_unlock_queues();
	vm_fault_cleanup(object, top_page);
}

/*
 *	Routine:	vm_fault_copy
 *
 *	Purpose:
 *		Copy pages from one virtual memory object to another --
 *		neither the source nor destination pages need be resident.
 *
 *		Before actually copying a page, the version associated with
 *		the destination address map wil be verified.
 *
 *	In/out conditions:
 *		The caller must hold a reference, but not a lock, to
 *		each of the source and destination objects and to the
 *		destination map.
 *
 *	Results:
 *		Returns KERN_SUCCESS if no errors were encountered in
 *		reading or writing the data.  Returns KERN_INTERRUPTED if
 *		the operation was interrupted (only possible if the
 *		"interruptible" argument is asserted).  Other return values
 *		indicate a permanent error in copying the data.
 *
 *		The actual amount of data copied will be returned in the
 *		"copy_size" argument.  In the event that the destination map
 *		verification failed, this amount may be less than the amount
 *		requested.
 */
kern_return_t
vm_fault_copy(
	vm_object_t	src_object,
	vm_offset_t	src_offset,
	vm_size_t	*src_size,		/* INOUT */
	vm_object_t	dst_object,
	vm_offset_t	dst_offset,
	vm_map_t	dst_map,
	vm_map_version_t *dst_version,
	boolean_t	interruptible)
{
	vm_page_t		result_page;
	
	vm_page_t		src_page;
	vm_page_t		src_top_page;
	vm_prot_t		src_prot;

	vm_page_t		dst_page;
	vm_page_t		dst_top_page;
	vm_prot_t		dst_prot;

	vm_size_t		amount_left;
#if	!NORMA_VM
	vm_object_t		old_copy_object;
#endif	/* !NORMA_VM */
	kern_return_t		error = 0;

	vm_size_t		part_size;


#define	RETURN(x)					\
	MACRO_BEGIN					\
	*src_size -= amount_left;			\
	MACRO_RETURN(x);				\
	MACRO_END

	amount_left = *src_size;
	do { /* while (amount_left > 0) */
		/*
		 * There may be a deadlock if both source and destination
		 * pages are the same. To avoid this deadlock, the copy must
		 * start by getting the destination page in order to apply
		 * COW semantics if any. Then, getting the source page avoids
		 * the deadlock because vm_page_lookup()returns :
		 * 1) Either the same page as the destination: The copy can
		 *	start immediately (and there is only one busy page).
		 * 2) Or another page (perhaps NULL), and then vm_fault_page()
		 *	won't deadlock because both pages are from the same map.
		 */

	RetryDestinationFault: ;

		dst_prot = VM_PROT_WRITE|VM_PROT_READ;

		vm_object_lock(dst_object);
		vm_object_paging_begin(dst_object);

		switch (vm_fault_page(dst_object,
				      trunc_page(dst_offset),
				      VM_PROT_WRITE|VM_PROT_READ,
				      FALSE,
				      FALSE /* interruptible */,
				      &dst_prot,
				      &dst_page,
				      &dst_top_page,
				      &error,
				      dst_map->no_zero_fill)) {

		case VM_FAULT_SUCCESS:
			break;
		case VM_FAULT_RETRY:
			goto RetryDestinationFault;
		case VM_FAULT_INTERRUPTED:
			RETURN(MACH_SEND_INTERRUPTED);
		case VM_FAULT_MEMORY_SHORTAGE:
			VM_PAGE_WAIT();
			goto RetryDestinationFault;
		case VM_FAULT_FICTITIOUS_SHORTAGE:
			vm_page_more_fictitious();
			goto RetryDestinationFault;
		case VM_FAULT_MEMORY_ERROR:
			if (error)
				return (error);
			else
				return(KERN_MEMORY_ERROR);
		}
		assert ((dst_prot & VM_PROT_WRITE) != VM_PROT_NONE);

#if	!NORMA_VM
		old_copy_object = dst_page->object->copy;
#endif	/* !NORMA_VM */

		vm_object_unlock(dst_page->object);

	RetrySourceFault: ;

		if (src_object == VM_OBJECT_NULL) {
			/*
			 *	No source object.  We will just
			 *	zero-fill the page in dst_object.
			 */
			src_page = VM_PAGE_NULL;
		} else {
			vm_object_lock(src_object);
			src_page = vm_page_lookup(src_object,
						  trunc_page(src_offset));
			if (src_page == dst_page)
				src_prot = dst_prot;
			else {
				src_prot = VM_PROT_READ;
				vm_object_paging_begin(src_object);

				switch (vm_fault_page(src_object, 
						      trunc_page(src_offset),
						      VM_PROT_READ, 
						      FALSE, 
						      interruptible,
						      &src_prot, 
						      &result_page,
						      &src_top_page,
						      &error,
						      FALSE)) {

				case VM_FAULT_SUCCESS:
					break;
				case VM_FAULT_RETRY:
					goto RetrySourceFault;
				case VM_FAULT_INTERRUPTED:
					vm_fault_copy_cleanup(dst_page,
							      dst_top_page);
					RETURN(MACH_SEND_INTERRUPTED);
				case VM_FAULT_MEMORY_SHORTAGE:
					VM_PAGE_WAIT();
					goto RetrySourceFault;
				case VM_FAULT_FICTITIOUS_SHORTAGE:
					vm_page_more_fictitious();
					goto RetrySourceFault;
				case VM_FAULT_MEMORY_ERROR:
					vm_fault_copy_cleanup(dst_page,
							      dst_top_page);
					if (error)
						return (error);
					else
						return(KERN_MEMORY_ERROR);
				}

				src_page = result_page;

				assert((src_top_page == VM_PAGE_NULL) ==
				       (src_page->object == src_object));
			}
			assert ((src_prot & VM_PROT_READ) != VM_PROT_NONE);
			vm_object_unlock(src_page->object);
		}

		if (!vm_map_verify(dst_map, dst_version)) {
			if (src_page != VM_PAGE_NULL && src_page != dst_page)
				vm_fault_copy_cleanup(src_page, src_top_page);
			vm_fault_copy_cleanup(dst_page, dst_top_page);
			break;
		}

#if	!NORMA_VM
		vm_object_lock(dst_page->object);
		if (dst_page->object->copy != old_copy_object) {
			vm_object_unlock(dst_page->object);
			vm_map_verify_done(dst_map, dst_version);
			if (src_page != VM_PAGE_NULL && src_page != dst_page)
				vm_fault_copy_cleanup(src_page, src_top_page);
			vm_fault_copy_cleanup(dst_page, dst_top_page);
			break;
		}
		vm_object_unlock(dst_page->object);
#endif	/* !NORMA_VM */

		/*
		 *	Copy the page, and note that it is dirty
		 *	immediately.
		 */

		if (!page_aligned(src_offset) ||
			!page_aligned(dst_offset) ||
			!page_aligned(amount_left)) {

			vm_offset_t	src_po,
					dst_po;

			src_po = src_offset - trunc_page(src_offset);
			dst_po = dst_offset - trunc_page(dst_offset);

			if (dst_po > src_po) {
				part_size = PAGE_SIZE - dst_po;
			} else {
				part_size = PAGE_SIZE - src_po;
			}
			if (part_size > (amount_left)){
				part_size = amount_left;
			}

			if (src_page == VM_PAGE_NULL) {
				vm_page_part_zero_fill(dst_page,
							dst_po, part_size);
			} else {
				vm_page_part_copy(src_page, src_po,
					dst_page, dst_po, part_size);
			}
		} else {
			part_size = PAGE_SIZE;

			if (src_page == VM_PAGE_NULL)
				vm_page_zero_fill(dst_page);
			else
				vm_page_copy(src_page, dst_page);
		}

		dst_page->dirty = TRUE;
		/*
		 *	Unlock everything, and return
		 */

		vm_map_verify_done(dst_map, dst_version);

		if (src_page != VM_PAGE_NULL && src_page != dst_page)
			vm_fault_copy_cleanup(src_page, src_top_page);
		vm_fault_copy_cleanup(dst_page, dst_top_page);

		amount_left -= part_size;
		src_offset += part_size;
		dst_offset += part_size;
	} while (amount_left > 0);

	RETURN(KERN_SUCCESS);
#undef	RETURN

	/*NOTREACHED*/	
}

#ifdef	notdef

/*
 *	Routine:	vm_fault_page_overwrite
 *
 *	Description:
 *		A form of vm_fault_page that assumes that the
 *		resulting page will be overwritten in its entirety,
 *		making it unnecessary to obtain the correct *contents*
 *		of the page.
 *
 *	Implementation:
 *		XXX Untested.  Also unused.  Eventually, this technology
 *		could be used in vm_fault_copy() to advantage.
 */
vm_fault_return_t
vm_fault_page_overwrite(
	register
	vm_object_t	dst_object,
	vm_offset_t	dst_offset,
	vm_page_t	*result_page)	/* OUT */
{
	register
	vm_page_t	dst_page;
	thread_t	thread = current_thread();

#define	interruptible	FALSE	/* XXX */

	while (TRUE) {
		/*
		 *	Look for a page at this offset
		 */

		while ((dst_page = vm_page_lookup(dst_object, dst_offset))
				 == VM_PAGE_NULL) {
			/*
			 *	No page, no problem... just allocate one.
			 */

			dst_page = vm_page_alloc(dst_object, dst_offset);
			if (dst_page == VM_PAGE_NULL) {
				vm_object_unlock(dst_object);
				VM_PAGE_WAIT();
				vm_object_lock(dst_object);
				continue;
			}

			/*
			 *	Pretend that the memory manager
			 *	write-protected the page.
			 *
			 *	Note that we will be asking for write
			 *	permission without asking for the data
			 *	first.
			 */

			dst_page->overwriting = TRUE;
			dst_page->page_lock = VM_PROT_WRITE;
			dst_page->absent = TRUE;
			dst_page->unusual = TRUE;
			dst_object->absent_count++;

			break;

			/*
			 *	When we bail out, we might have to throw
			 *	away the page created here.
			 */

#define	DISCARD_PAGE						\
	MACRO_BEGIN						\
	vm_object_lock(dst_object);				\
	dst_page = vm_page_lookup(dst_object, dst_offset);	\
	if ((dst_page != VM_PAGE_NULL) && dst_page->overwriting) \
	   	VM_PAGE_FREE(dst_page);				\
	vm_object_unlock(dst_object);				\
	MACRO_END
		}

		/*
		 *	If the page is write-protected...
		 */

		if (dst_page->page_lock & VM_PROT_WRITE) {
			/*
			 *	... and an unlock request hasn't been sent
			 */

			if ( ! (dst_page->unlock_request & VM_PROT_WRITE)) {
				vm_prot_t	u;
				kern_return_t	rc;

				/*
				 *	... then send one now.
				 */

				if (!dst_object->pager_ready) {
					vm_object_assert_wait(dst_object,
						VM_OBJECT_EVENT_PAGER_READY,
						interruptible);
					vm_object_unlock(dst_object);
					thread_block((void (*)(void))0);
					if (thread->wait_result !=
					    THREAD_AWAKENED) {
						DISCARD_PAGE;
						return(VM_FAULT_INTERRUPTED);
					}
					continue;
				}

				u = dst_page->unlock_request |= VM_PROT_WRITE;
				vm_object_unlock(dst_object);

				if ((rc = memory_object_data_unlock(
						dst_object->pager,
						dst_object->pager_request,
						dst_offset + dst_object->paging_offset,
						PAGE_SIZE,
						u)) != KERN_SUCCESS) {
					if (vm_fault_debug)
				     	    printf("vm_object_overwrite: memory_object_data_unlock failed\n");
					DISCARD_PAGE;
					return((rc == MACH_SEND_INTERRUPTED) ?
						VM_FAULT_INTERRUPTED :
						VM_FAULT_MEMORY_ERROR);
				}
				vm_object_lock(dst_object);
				continue;
			}

			/* ... fall through to wait below */
		} else {
			/*
			 *	If the page isn't being used for other
			 *	purposes, then we're done.
			 */
			if ( ! (dst_page->busy || dst_page->absent ||
				dst_page->error || dst_page->restart) )
				break;
		}

		PAGE_ASSERT_WAIT(dst_page, interruptible);
		vm_object_unlock(dst_object);
		thread_block((void (*)(void))0);
		if (thread->wait_result != THREAD_AWAKENED) {
			DISCARD_PAGE;
			return(VM_FAULT_INTERRUPTED);
		}
	}

	*result_page = dst_page;
	return(VM_FAULT_SUCCESS);

#undef	interruptible
#undef	DISCARD_PAGE
}

#endif	/* notdef */

#if	VM_FAULT_CLASSIFY
/*
 *	Temporary statistics gathering support.
 */

/*
 *	Statistics arrays:
 */
#define VM_FAULT_TYPES_MAX	5
#define	VM_FAULT_LEVEL_MAX	8

int	vm_fault_stats[VM_FAULT_TYPES_MAX][VM_FAULT_LEVEL_MAX];

#define	VM_FAULT_TYPE_ZERO_FILL	0
#define	VM_FAULT_TYPE_MAP_IN	1
#define	VM_FAULT_TYPE_PAGER	2
#define	VM_FAULT_TYPE_COPY	3
#define	VM_FAULT_TYPE_OTHER	4


void
vm_fault_classify(vm_object_t	object,
		  vm_offset_t	offset,
		  vm_prot_t	fault_type)
{
	int		type, level = 0;
	vm_page_t	m;

	while (TRUE) {
		m = vm_page_lookup(object, offset);
		if (m != VM_PAGE_NULL) {		
			if (m->busy || m->error || m->restart || m->absent ||
			    fault_type & m->page_lock) {
				type = VM_FAULT_TYPE_OTHER;
				break;
			}
			if (((fault_type & VM_PROT_WRITE) == 0) ||
			    ((level == 0) && object->copy == VM_OBJECT_NULL)) {
				type = VM_FAULT_TYPE_MAP_IN;
				break;	
			}
			type = VM_FAULT_TYPE_COPY;
			break;
		}
		else {
			if (object->pager_created) {
				type = VM_FAULT_TYPE_PAGER;
				break;
			}
			if (object->shadow == VM_OBJECT_NULL) {
				type = VM_FAULT_TYPE_ZERO_FILL;
				break;
		        }

			offset += object->shadow_offset;
			object = object->shadow;
			level++;
			continue;
		}
	}

	if (level > VM_FAULT_LEVEL_MAX)
		level = VM_FAULT_LEVEL_MAX;

	vm_fault_stats[type][level] += 1;

	return;
}

/* cleanup routine to call from debugger */

void
vm_fault_classify_init(void)
{
	int type, level;

	for (type = 0; type < VM_FAULT_TYPES_MAX; type++) {
		for (level = 0; level < VM_FAULT_LEVEL_MAX; level++) {
			vm_fault_stats[type][level] = 0;
		}
	}

	return;
}
#endif
