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
 * Revision 2.27.6.6  92/09/15  17:37:08  jeffreyh
 * 	Removed vm_object_pmap_remove.  From bruel@gr.osf.org.
 * 	[92/09/03            dlb]
 * 	Changes for new vm_fault_page interface to pass back an error
 * 	code.
 * 	[92/07/28            sjs]
 * 
 * Revision 2.27.6.5  92/06/25  13:58:48  jeffreyh
 * 	Restore call to vm_object_cache_unlock in vm_object_destroy that
 * 	was lost in some previous merge.
 * 	[92/06/25            jeffreyh]
 * 
 * Revision 2.27.6.4  92/03/28  10:17:21  jeffreyh
 * 	Change some uses of temporary to internal because external temporary
 * 	objects now exist.
 * 	[92/03/27            dlb]
 * 	Dead pager check should be ip_active, not IP_VALID.
 * 	[92/03/20            dlb]
 * 	Fix from dlb to check is a pager is active as well as valid.
 * 	[92/03/17  11:11:57  jeffreyh]
 * 
 * Revision 2.27.6.3  92/03/03  16:26:58  jeffreyh
 * 	Check for dead pager in vm_object_pager_wakeup.
 * 	[92/02/28            dlb]
 * 	Don't do pager wakeup from vm_object_destroy().
 * 	Don't set shadowed field in temporary object copied by
 * 	vm_object_copy_delayed().
 * 	[92/02/28  13:16:44  jeffreyh]
 * 
 * 	Remove dest_wired logic from vm_object_copy_slowly.
 * 	[92/02/21  10:16:35  dlb]
 * 
 * 	Maintain shadowed field in objects for use by new
 * 	VM_INHERIT_SHARE code (vm_map_fork).  Restore old
 * 	interface to vm_object_pmap_protect.
 * 	[92/02/19  14:28:12  dlb]
 * 
 * 	Use use_shared_copy instead of new copy strategy for
 * 	temporary objects.
 * 	[92/01/07  11:15:53  dlb]
 * 
 * 	Add asynchronous copy-on-write logic for temporary objects.
 * 	[92/01/06  16:24:59  dlb]
 * 	Finish jsb's IKOT_PAGER_TERMINATING changes:
 * 		- Call memory_object_release from vm_object_terminate.
 * 		- Split out wakeup code into separate function so
 * 			it can be called from vm_object_destroy()
 * 			and ipc_kobject_destroy().
 * 		- Change vm_object_enter to allow for port death.
 * 	[92/02/25            dlb]
 * 
 * 		Fixed vm_object_lookup to use IP_VALID, lock the port, and
 * 		allow only IKOT_PAGING_REQUEST, not IKOT_PAGER.
 * 		Removed vm_object_debug.
 * 		[91/12/31            rpd]
 * 
 * 		Split vm_object_init into vm_object_bootstrap and vm_object_init.
 * 		[91/12/28            rpd]
 * 
 * Revision 2.27.6.2  92/02/21  11:28:48  jsb
 * 	Use IKOT_PAGER_TERMINATING to solve terminate/init race.
 * 	No one in the history of Mach has ever written a pager that
 * 	can handle this race, including kernel-supplied pagers such
 * 	as the default pager (but excluding netmemory pagers which
 * 	handle the fully general case of multiple clients).
 * 	[92/02/16  16:22:50  jsb]
 * 
 * 	(Naively) handle temporary objects in vm_object_copy_strategically.
 * 	This now happens because of new MEMORY_OBJECT_COPY_TEMPORARY option.
 * 	[92/02/11  11:41:34  jsb]
 * 
 * 	Object->pager_request is now declared as a pager_request_t, which is
 * 	an xmm_obj_t in NORMA_VM systems, ipc_port_t otherwise.
 * 	Removed bogus vm_object_incr now that xmm_server uses vm_object_lookup.
 * 	Picked up rpd's vm_object_lookup, and conditionalized it for NORMA_VM
 * 	so that port must be a pager port (vs. standard case where it must
 * 	be a pager_request port). Added changes for pager_name being a send
 * 	right instead of a receive right. Cleaned up vm_object_enter logic,
 * 	thanks to elimination of pager kobject overloading. (It used to switch
 * 	between a vm_object and an xmm obj depending on whether it was mapped
 * 	locally or remotely first.) Change vm_object_collapse to simply move
 * 	pager_request from one object to the other (as in the non-NORMA_VM
 * 	case), instead of calling xmm_object_set.
 * 	[92/02/10  09:57:12  jsb]
 * 
 * Revision 2.27.6.1  92/02/18  19:25:11  jeffreyh
 * 	Free new page before exiting with an error in vm_object_copy_slowly
 * 	[92/02/14            dlb]
 * 
 * Revision 2.27  91/12/11  08:44:01  jsb
 * 	Changed vm_object_coalesce to also check for paging references.
 * 	This fixes a problem with page-list map-copy objects.
 * 	[91/12/03            rpd]
 * 
 * Revision 2.26  91/12/10  13:27:07  jsb
 * 	Comment out noisy printf. (Avoiding dealloc ...)
 * 	[91/12/10  12:50:34  jsb]
 * 
 * Revision 2.25  91/11/12  11:52:09  rvb
 * 	Added simple_lock_pause.
 * 	[91/11/12            rpd]
 * 
 * Revision 2.24  91/08/28  11:18:37  jsb
 * 	Handle dirty private pages correctly in vm_object_terminate().
 * 	[91/07/30  14:18:54  dlb]
 * 
 * 	single_use --> use_old_pageout in object initialization template.
 * 	Support precious pages in vm_object_terminate().
 * 	[91/07/03  14:18:07  dlb]
 * 
 * Revision 2.23  91/07/31  18:21:27  dbg
 * 	Add vm_object_page_map, to attach a set of physical pages to an
 * 	object.
 * 	[91/07/30  17:26:58  dbg]
 * 
 * Revision 2.22  91/07/01  08:27:52  jsb
 * 	Improved NORMA_VM support, including support for memory_object_create.
 * 	[91/06/29  17:06:11  jsb]
 * 
 * Revision 2.21  91/06/25  11:07:02  rpd
 * 	Fixed includes to avoid norma files unless they are really needed.
 * 	[91/06/25            rpd]
 * 
 * Revision 2.20  91/06/25  10:34:11  rpd
 * 	Changed memory_object_t to ipc_port_t where appropriate.
 * 	Removed extraneous casts.
 * 	[91/05/28            rpd]
 * 
 * Revision 2.19  91/06/17  15:49:25  jsb
 * 	Added NORMA_VM support.
 * 	[91/06/17  15:28:16  jsb]
 * 
 * Revision 2.18  91/05/18  14:41:22  rpd
 * 	Changed vm_object_deactivate_pages to call vm_page_deactivate
 * 	on inactivate pages, because that is no longer a no-op.
 * 	[91/04/20            rpd]
 * 
 * 	Fixed vm_object_pmap_protect, vm_object_pmap_remove,
 * 	vm_object_page_remove, vm_object_copy_call, vm_object_copy_delayed
 * 	to check for fictitious pages.
 * 	[91/04/10            rpd]
 * 	Fixed vm_object_terminate to allow for busy/absent pages.
 * 	[91/04/02            rpd]
 * 
 * 	Added VM_FAULT_FICTITIOUS_SHORTAGE.
 * 	[91/03/29            rpd]
 * 	Added vm/memory_object.h.
 * 	[91/03/22            rpd]
 * 
 * Revision 2.17  91/05/14  17:50:19  mrt
 * 	Correcting copyright
 * 
 * Revision 2.16  91/05/08  13:34:59  dbg
 * 	Rearrange initialization code in vm_object_enter to clearly
 * 	separate 'internal' case (and to avoid a vax GCC bug).
 * 	[91/04/17            dbg]
 * 
 * Revision 2.15  91/03/16  15:06:19  rpd
 * 	Changed vm_object_deactivate_pages and vm_object_terminate to
 * 	check for busy pages.  Changed how vm_object_terminate
 * 	interacts with the pageout daemon.
 * 	[91/03/12            rpd]
 * 	Fixed vm_object_page_remove to be smart about small regions.
 * 	[91/03/05            rpd]
 * 	Added resume, continuation arguments to vm_fault_page.
 * 	Added continuation argument to VM_PAGE_WAIT.
 * 	[91/02/05            rpd]
 * 
 * Revision 2.14  91/02/05  17:59:16  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:33:29  mrt]
 * 
 * Revision 2.13  91/01/08  16:45:32  rpd
 * 	Added continuation argument to thread_block.
 * 	[90/12/08            rpd]
 * 
 * 	Fixed vm_object_terminate to give vm_pageout_page busy pages.
 * 	[90/11/22            rpd]
 * 
 * 	Changed VM_WAIT to VM_PAGE_WAIT.
 * 	[90/11/13            rpd]
 * 
 * Revision 2.12  90/11/06  18:44:12  rpd
 * 	From dlb@osf.org:
 * 	If pager initialization is in progress (object->pager_created &&
 * 	!object->pager_initialized), then vm_object_deallocate must wait
 * 	for it to complete before terminating the object.  Because anything
 * 	can happen in the interim, it must recheck its decision to terminate
 * 	after the wait completes.
 * 
 * Revision 2.11  90/10/12  13:06:24  rpd
 * 	In vm_object_copy_slowly, only activate pages returned from
 * 	vm_fault_page if they aren't already on a pageout queue.
 * 	[90/10/08            rpd]
 * 
 * Revision 2.10  90/09/09  14:34:20  rpd
 * 	Fixed bug in vm_object_copy_slowly.  The pages in the new object
 * 	should be marked as dirty.
 * 	[90/09/08            rpd]
 * 
 * Revision 2.9  90/06/19  23:02:52  rpd
 * 	Picked up vm_submap_object.
 * 	[90/06/08            rpd]
 * 
 * Revision 2.8  90/06/02  15:11:31  rpd
 * 	Changed vm_object_collapse_bypass_allowed back to TRUE.
 * 	[90/04/22            rpd]
 * 	Converted to new IPC.
 * 	[90/03/26  23:16:54  rpd]
 * 
 * Revision 2.7  90/05/29  18:38:57  rwd
 * 	Rfr change to reflect change in vm_pageout_page.
 * 	[90/04/12  13:48:31  rwd]
 * 
 * Revision 2.6  90/05/03  15:53:08  dbg
 * 	Make vm_object_pmap_protect follow shadow chains if removing all
 * 	permissions.  Fix check for using pmap_protect on entire range.
 * 	[90/04/18            dbg]
 * 
 * 	Pass 'flush' argument to vm_pageout_page.
 * 	[90/03/28            dbg]
 * 
 * Revision 2.5  90/02/22  20:06:24  dbg
 * 	Add changes from mainline:
 * 
 * 		Fix comment on vm_object_delayed.
 * 		[89/12/18            dlb]
 * 		Revised use of PAGE_WAKEUP macros.  Don't clear busy flag when
 * 		restarting unlock requests.
 * 		[89/12/13            dlb]
 * 		Fix locking problems in vm_object_copy_slowly:
 * 		    1.  Must hold lock on object containing page being copied
 * 			from (result_page) object when activating that page.
 * 		    2.  Fix typo that tried to lock source object twice if
 * 		        vm_fault_page returned two objects; this rare case
 * 		        would hang a multiprocessor if it occurred.
 * 		[89/12/11            dlb]
 * 
 * 		Complete un-optimized overwriting implementation.  Move
 * 		futuristic implementation here.  Clean up and document
 * 		copy routines.
 * 		[89/08/05  18:01:22  mwyoung]
 * 		Add vm_object_pmap_protect() to perform pmap protection
 * 		changes more efficiently.
 * 		[89/07/07  14:06:34  mwyoung]
 * 		Split up vm_object_copy into one part that does not unlock the
 * 		object (i.e., does not sleep), and another that may.  Also, use
 * 		separate routines for each copy strategy.
 * 		[89/07/06  18:39:55  mwyoung]
 * 
 * Revision 2.4  90/01/11  11:47:58  dbg
 * 	Use vm_fault_cleanup.
 * 	[89/12/13            dbg]
 * 
 * 	Fix locking bug in vm_object_copy_slowly.
 * 	[89/12/13            dbg]
 * 
 * 	Pick up changes from mainline:
 * 		Remove size argument from vm_external_destroy.
 * 		Add paging_offset to size used in vm_external_create.
 * 		Account for existence_info movement in vm_object_collapse.
 * 		[89/10/16  15:30:16  af]
 * 
 * 		Remove XP conditionals.
 * 		Document global variables.
 * 		[89/10/10            mwyoung]
 * 
 * 		Added initialization for lock_in_progress, lock_restart fields.
 * 		[89/08/07            mwyoung]
 * 
 * 		Beautify dirty bit handling in vm_object_terminate().
 * 		Don't write back "error" pages in vm_object_terminate().
 * 		[89/04/22            mwyoung]
 * 
 * 		Removed vm_object_list, vm_object_count.
 * 		[89/08/31  19:45:43  rpd]
 * 
 * 		Optimization from NeXT: vm_object_deactivate_pages checks now
 * 		that the page is inactive before call vm_page_deactivate.  Also,
 * 		initialize the last_alloc field in vm_object_template.
 * 		[89/08/19  23:46:42  rpd]
 * 
 * Revision 2.3  89/10/16  15:22:22  rwd
 * 	In vm_object_collapse: leave paging_offset zero if the backing
 * 	object does not have a pager.
 * 	[89/10/12            dbg]
 * 
 * Revision 2.2  89/09/08  11:28:45  dbg
 * 	Add keep_wired parameter to vm_object_copy_slowly to wire new
 * 	pages.
 * 	[89/07/14            dbg]
 * 
 * 22-May-89  David Golub (dbg) at Carnegie-Mellon University
 *	Refuse vm_object_pager_create if no default memory manager.
 *
 * 28-Apr-89  David Golub (dbg) at Carnegie-Mellon University
 *	Changes for MACH_KERNEL:
 *	. Use port_alloc_internal to simplify port_alloc call.
 *	. Remove non-XP code.
 *
 * Revision 2.23  89/04/23  13:25:43  gm0w
 * 	Changed assert(!p->busy) to assert(!p->busy || p->absent) in
 * 	vm_object_collapse().  Change from jsb/mwyoung.
 * 	[89/04/23            gm0w]
 * 
 * Revision 2.22  89/04/18  21:26:29  mwyoung
 * 	Recent history [mwyoung]:
 * 		Use hints about pages not in main memory.
 * 		Get default memory manager port more carefully, as it
 * 		 may now be changed after initialization.
 * 		Use new event mechanism.
 * 		Correct vm_object_destroy() to actually abort requests.
 * 		Correct handling of paging offset in vm_object_collapse().
 * 	Condensed history:
 * 		Add copy strategy handling, including a routine
 * 		 (vm_object_copy_slowly()) to perform an immediate copy
 * 		 of a region of an object. [mwyoung]
 * 	 	Restructure the handling of the relationships among memory
 * 		 objects (the ports), the related fields in the vm_object
 * 		 structure, and routines that manipulate them [mwyoung].
 * 		Simplify maintenance of the unreferenced-object cache. [mwyoung]
 * 		Distinguish internal and temporary attributes. [mwyoung]
 * 		Reorganized and documented maintenance of the
 * 		 vm_object-to-memory_object assocations. [mwyoung]
 * 		Improved external memory management interface. [mwyoung]
 * 		Several reimplementations/fixes to the object cache
 * 		 and the port to object translation.  [mwyoung, avie, dbg]
 * 		Create a cache of objects that have no references
 * 		 so that their pages remain in memory (inactive).  [avie]
 * 		Collapse object tree when reference counts drop to one.
 * 		 Use "paging_in_progress" to prevent collapsing. [dbg]
 * 		Split up paging system lock, eliminate SPL handling.
 * 		 The only uses of vm_object code at interrupt level
 * 		 uses objects that are only touched at interrupt level. [dbg]
 * 		Use "copy object" rather than normal shadows for
 * 		 permanent objects.  [dbg]
 * 		Accomodate external pagers [mwyoung, bolosky].
 * 		Allow objects to be coalesced to avoid growing address
 * 		 maps during sequential allocations.  [dbg]
 * 		Optimizations and fixes to copy-on-write [avie, mwyoung, dbg].
 * 		Use only one object for all kernel data. [avie]
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
 *	File:	vm/vm_object.c
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 *
 *	Virtual memory object module.
 */

#include <norma_vm.h>
#include <mach_pagemap.h>
#include <mor.h>

#include <mach/memory_object.h>
#include <mach/memory_object_default.h>
#include <mach/memory_object_user.h>
#include <mach/mach_server.h>
#include <mach/vm_param.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_space.h>
#include <kern/assert.h>
#include <kern/lock.h>
#include <kern/queue.h>
#include <kern/xpr.h>
#include <kern/zalloc.h>
#include <kern/host.h>
#include <vm/memory_object.h>
#include <vm/vm_fault.h>
#include <vm/vm_map.h>
#include <vm/vm_object.h>
#include <vm/vm_page.h>
#include <vm/vm_pageout.h>
#include <kern/misc_protos.h>

#if	NORMA_VM
#include <norma/xmm_server_rename.h>
#endif	/* NORMA_VM */

/*
 *	Virtual memory objects maintain the actual data
 *	associated with allocated virtual memory.  A given
 *	page of memory exists within exactly one object.
 *
 *	An object is only deallocated when all "references"
 *	are given up.  Only one "reference" to a given
 *	region of an object should be writeable.
 *
 *	Associated with each object is a list of all resident
 *	memory pages belonging to that object; this list is
 *	maintained by the "vm_page" module, but locked by the object's
 *	lock.
 *
 *	Each object also records the memory object port
 *	that is used by the kernel to request and write
 *	back data (the memory object port, field "pager"),
 *	and the ports provided to the memory manager, the server that
 *	manages that data, to return data and control its
 *	use (the memory object control port, field "pager_request")
 *	and for naming (the memory object name port, field "pager_name").
 *
 *	Virtual memory objects are allocated to provide
 *	zero-filled memory (vm_allocate) or map a user-defined
 *	memory object into a virtual address space (vm_map).
 *
 *	Virtual memory objects that refer to a user-defined
 *	memory object are called "permanent", because all changes
 *	made in virtual memory are reflected back to the
 *	memory manager, which may then store it permanently.
 *	Other virtual memory objects are called "temporary",
 *	meaning that changes need be written back only when
 *	necessary to reclaim pages, and that storage associated
 *	with the object can be discarded once it is no longer
 *	mapped.
 *
 *	A permanent memory object may be mapped into more
 *	than one virtual address space.  Moreover, two threads
 *	may attempt to make the first mapping of a memory
 *	object concurrently.  Only one thread is allowed to
 *	complete this mapping; all others wait for the
 *	"pager_initialized" field is asserted, indicating
 *	that the first thread has initialized all of the
 *	necessary fields in the virtual memory object structure.
 *
 *	The kernel relies on a *default memory manager* to
 *	provide backing storage for the zero-filled virtual
 *	memory objects.  The memory object ports associated
 *	with these temporary virtual memory objects are only
 *	generated and passed to the default memory manager
 *	when it becomes necessary.  Virtual memory objects
 *	that depend on the default memory manager are called
 *	"internal".  The "pager_created" field is provided to
 *	indicate whether these ports have ever been allocated.
 *	
 *	The kernel may also create virtual memory objects to
 *	hold changed pages after a copy-on-write operation.
 *	In this case, the virtual memory object (and its
 *	backing storage -- its memory object) only contain
 *	those pages that have been changed.  The "shadow"
 *	field refers to the virtual memory object that contains
 *	the remainder of the contents.  The "shadow_offset"
 *	field indicates where in the "shadow" these contents begin.
 *	The "copy" field refers to a virtual memory object
 *	to which changed pages must be copied before changing
 *	this object, in order to implement another form
 *	of copy-on-write optimization.
 *
 *	The virtual memory object structure also records
 *	the attributes associated with its memory object.
 *	The "pager_ready", "can_persist" and "copy_strategy"
 *	fields represent those attributes.  The "cached_list"
 *	field is used in the implementation of the persistence
 *	attribute.
 *
 * ZZZ Continue this comment.
 */

/* Forward declarations for internal functions. */
extern vm_object_t	_vm_object_allocate(
				vm_size_t	size);

extern void		vm_object_terminate(
				vm_object_t	object);

extern vm_object_t	vm_object_lookup_name(
				ipc_port_t	port);

extern void		vm_object_remove(
				vm_object_t	object);

extern vm_object_t	vm_object_cache_trim(
				boolean_t called_from_vm_object_deallocate);

extern void		vm_object_deactivate_pages(
				vm_object_t	object);

extern void		vm_object_abort_activity(
				vm_object_t	object);

extern kern_return_t	vm_object_copy_call(
				vm_object_t	src_object,
				vm_object_t	*_result_object);

extern void		vm_object_do_collapse(
				vm_object_t	object,
				vm_object_t	backing_object);

extern void		vm_object_do_bypass(
				vm_object_t	object,
				vm_object_t	backing_object);

extern void		memory_object_release(
				ipc_port_t	pager,
				pager_request_t	pager_request,
				ipc_port_t	pager_name);

#if	NORMA_VM
extern boolean_t	vm_object_uncache(
				vm_object_t	object,
				boolean_t	may_uncache);

extern void		vm_object_declare_pages(
				vm_object_t	object);

extern void		vm_object_reaper_continue(void);

#else	/* NORMA_VM */
extern vm_object_t	vm_object_copy_delayed(
				vm_object_t	src_object);
#endif	/* NORMA_VM */

zone_t		vm_object_zone;		/* vm backing store zone */

/*
 *	All wired-down kernel memory belongs to a single virtual
 *	memory object (kernel_object) to avoid wasting data structures.
 */
vm_object_t	kernel_object;

/*
 *	Virtual memory objects that are not referenced by
 *	any address maps, but that are allowed to persist
 *	(an attribute specified by the associated memory manager),
 *	are kept in a queue (vm_object_cached_list).
 *
 *	When an object from this queue is referenced again,
 *	for example to make another address space mapping,
 *	it must be removed from the queue.  That is, the
 *	queue contains *only* objects with zero references.
 *
 *	The kernel may choose to terminate objects from this
 *	queue in order to reclaim storage.  The current policy
 *	is to permit a fixed maximum number of unreferenced
 *	objects (vm_object_cached_max).
 *
 *	A spin lock (accessed by routines
 *	vm_object_cache_{lock,lock_try,unlock}) governs the
 *	object cache.  It must be held when objects are
 *	added to or removed from the cache (in vm_object_terminate).
 *	The routines that acquire a reference to a virtual
 *	memory object based on one of the memory object ports
 *	must also lock the cache.
 *
 *	Ideally, the object cache should be more isolated
 *	from the reference mechanism, so that the lock need
 *	not be held to make simple references.
 */
queue_head_t	vm_object_cached_list;
int		vm_object_cached_count;
int		vm_object_cached_high;	/* highest # of cached objects */
int		vm_object_cached_max = 500;	/* may be patched*/

mutex_t		vm_object_cached_lock_data;

#define vm_object_cache_lock()		\
		mutex_lock(&vm_object_cached_lock_data)
#define vm_object_cache_lock_try()	\
		mutex_try(&vm_object_cached_lock_data)
#define vm_object_cache_unlock()	\
		mutex_unlock(&vm_object_cached_lock_data)

#if	NORMA_VM
/*
 * List of vm_objects to be asynchronously terminated.
 */
queue_head_t	vor_list;
spinlock_t	vor_lock;
#endif	/* NORMA_VM */

/*
 *	Virtual memory objects are initialized from
 *	a template (see vm_object_allocate).
 *
 *	When adding a new field to the virtual memory
 *	object structure, be sure to add initialization
 *	(see vm_object_init).
 */
vm_object_t	vm_object_template;

#define	VM_OBJECT_TRACKING	0
#if	VM_OBJECT_TRACKING
#define	MAX_OBJECT_TRACK	10000
vm_object_t	vm_object_track[MAX_OBJECT_TRACK];
int		vm_object_track_count = 0;
int		vm_object_count_warning = 9800;

static void
vm_object_track_alloc(object)
vm_object_t	object;
{
	vm_object_t	*o;

	for (o = vm_object_track; o < vm_object_track + MAX_OBJECT_TRACK; ++o)
		if (*o == VM_OBJECT_NULL) {
			*o = object;
			++vm_object_track_count;
			if (vm_object_track_count > vm_object_count_warning)
				assert(vm_object_track_count == 0);
			return;
		}
	panic("vm_object_track_alloc");
}

static void
vm_object_track_dealloc(object)
vm_object_t	object;
{
	vm_object_t	*o;

	for (o = vm_object_track; o < vm_object_track + MAX_OBJECT_TRACK; ++o)
		if (*o == object) {
			*o = VM_OBJECT_NULL;
			--vm_object_track_count;
			return;
		}
	panic("vm_object_track_dealloc");
}
#endif	/* VM_OBJECT_TRACKING */

/*
 *	vm_object_allocate:
 *
 *	Returns a new object with the given size.
 */

vm_object_t
_vm_object_allocate(
	vm_size_t	size)
{
	register vm_object_t object;

	object = (vm_object_t) zalloc(vm_object_zone);

	*object = *vm_object_template;
	queue_init(&object->memq);
	queue_init(&object->msr_q);
#if	MOR
	queue_init(&object->rep_head);
#endif
	vm_object_lock_init(object);
	object->size = size;
#if	VM_OBJECT_TRACKING
	vm_object_track_alloc(object);
#endif

	return object;
}

vm_object_t
vm_object_allocate(
	vm_size_t	size)
{
	register vm_object_t object;
	register ipc_port_t port;

	object = _vm_object_allocate(size);
	port = ipc_port_alloc_kernel();
	if (port == IP_NULL)
		panic("vm_object_allocate");
#if	NORMA_VM
	object->object_name = port;
#else	/* NORMA_VM */
	object->pager_name = port;
#endif	/* NORMA_VM */
	ipc_kobject_set(port, (ipc_kobject_t) object, IKOT_PAGING_NAME);

	return object;
}

#if	MOR
vm_object_rep_t 
vm_object_rep_allocate(
	void)
{
	vm_object_rep_t	repo;

	repo = (vm_object_rep_t) kalloc(sizeof (struct vm_object_rep));
       	if (repo == VM_OBJECT_REP_NULL) {
		return(repo);
	}

	repo->constituent = VM_OBJECT_NULL;

	queue_init(&repo->rep_list);

	return repo;
}
#endif	/* MOR */

/*
 *	vm_object_bootstrap:
 *
 *	Initialize the VM objects module.
 */
void
vm_object_bootstrap(void)
{
	vm_object_zone = zinit((vm_size_t) sizeof(struct vm_object),
				round_page(512*1024),
				round_page(12*1024),
				"objects");

	queue_init(&vm_object_cached_list);
	mutex_init(&vm_object_cached_lock_data);

	/*
	 *	Fill in a template object, for quick initialization
	 */

	vm_object_template = (vm_object_t) zalloc(vm_object_zone);
	bzero((char *) vm_object_template, sizeof *vm_object_template);

	vm_object_template->ref_count = 1;
	vm_object_template->size = 0;
	vm_object_template->frozen_size = 0;
	vm_object_template->resident_page_count = 0;
	vm_object_template->copy = VM_OBJECT_NULL;
	vm_object_template->shadow = VM_OBJECT_NULL;
	vm_object_template->shadow_offset = (vm_offset_t) 0;

	vm_object_template->pager = IP_NULL;
	vm_object_template->paging_offset = 0;
	vm_object_template->pager_request = PAGER_REQUEST_NULL;
	vm_object_template->pager_name = IP_NULL;
#if	NORMA_VM
	vm_object_template->object_name = IP_NULL;
#endif	/* NORMA_VM */

	vm_object_template->pager_created = FALSE;
	vm_object_template->pager_initialized = FALSE;
	vm_object_template->pager_ready = FALSE;

	vm_object_template->copy_strategy = MEMORY_OBJECT_COPY_SYMMETRIC;
	vm_object_template->shadowed = FALSE;

	vm_object_template->absent_count = 0;
	vm_object_template->all_wanted = 0; /* all bits FALSE */

	vm_object_template->paging_in_progress = 0;
	vm_object_template->can_persist = FALSE;
	vm_object_template->internal = TRUE;
	vm_object_template->temporary = TRUE;
	vm_object_template->private = FALSE;
	vm_object_template->alive = TRUE;
	vm_object_template->lock_in_progress = FALSE;
	vm_object_template->lock_restart = FALSE;
	vm_object_template->last_alloc = (vm_offset_t) 0;
#if	NORMA_VM
	vm_object_template->pagemap_exported = FALSE;
	vm_object_template->temporary_cached = FALSE;
	vm_object_template->temporary_caching = FALSE;
	vm_object_template->temporary_uncaching = FALSE;
	vm_object_template->completions_pending = 0;
#endif	/* NORMA_VM */
#if	MACH_PAGEMAP
	vm_external_nullify(&vm_object_template->existence_info);
#endif	/* MACH_PAGEMAP */
#if	MACH_ASSERT
	vm_object_template->paging_object = VM_OBJECT_NULL;
#endif	/* MACH_ASSERT */

	/*
	 *	Initialize the "kernel object"
	 */

	kernel_object = _vm_object_allocate(
		VM_MAX_KERNEL_ADDRESS - VM_MIN_KERNEL_ADDRESS);

	/*
	 *	Initialize the "submap object".  Make it as large as the
	 *	kernel object so that no limit is imposed on submap sizes.
	 */

	vm_submap_object = _vm_object_allocate(
		VM_MAX_KERNEL_ADDRESS - VM_MIN_KERNEL_ADDRESS);

#if	MACH_PAGEMAP
	vm_external_module_initialize();
#endif	/* MACH_PAGEMAP */

#if	NORMA_VM
	/*
	 * Initialize reaper_thread resources.
	 */
	queue_init(&vor_list);
	spinlock_init(&vor_lock);
#endif	/* NORMA_VM */
}

void
vm_object_init(void)
{
	register ipc_port_t port;

	/*
	 *	Finish initializing the kernel object.
	 *	The submap object doesn't need a name port.
	 */

	port = ipc_port_alloc_kernel();
	if (port == IP_NULL)
		panic("vm_object_init");
#if	NORMA_VM
	kernel_object->object_name = port;
#else	/* NORMA_VM */
	kernel_object->pager_name = port;
#endif	/* NORMA_VM */
	ipc_kobject_set(port, (ipc_kobject_t) kernel_object, IKOT_PAGING_NAME);
}

/*
 *	vm_object_reference:
 *
 *	Gets another reference to the given object.
 */
void
vm_object_reference(
	register vm_object_t	object)
{
	if (object == VM_OBJECT_NULL)
		return;

	vm_object_lock(object);
	assert(object->ref_count > 0);
	object->ref_count++;
	vm_object_unlock(object);
}

#define	MIGHT_NOT_CACHE_SHADOWS		1
#if	MIGHT_NOT_CACHE_SHADOWS
int cache_shadows = TRUE;
#endif	/* MIGHT_NOT_CACHE_SHADOWS */

#if	NORMA_VM

/*
 *	vm_object_deallocate_lazy:
 *
 *	Loose the reference. If other references remain, then we are done.
 *	If it is the last reference, then keep it and insert it into
 *	the vor_list queue to deallocate it under the vm_object_thread
 *	that is allowed to block. If the object is not alive and it is the
 *	last reference, then object is waiting for the last completion, and
 *	the thread waiting for that condition must be awaken.
 *
 *	Object is locked.
 */
void
vm_object_deallocate_lazy(
	vm_object_t	object)
{
	assert(object->ref_count > 0);
	if (object->alive)
		if (object->ref_count > 1)
			object->ref_count--;
		else {
			spinlock_lock(&vor_lock);
			queue_enter(&vor_list, object,
				    vm_object_t, cached_list);
			spinlock_unlock(&vor_lock);
			thread_wakeup((int)&vor_list);
		}
	else if (--object->ref_count == 0)
		vm_object_wakeup(object, VM_OBJECT_EVENT_COMPLETION);
}

#endif	/* NORMA_VM */

/*
 *	vm_object_deallocate:
 *
 *	Release a reference to the specified object,
 *	gained either through a vm_object_allocate
 *	or a vm_object_reference call.  When all references
 *	are gone, storage associated with this object
 *	may be relinquished.
 *
 *	No object may be locked.
 */
void
vm_object_deallocate(
	register vm_object_t	object)
{
	boolean_t retry_cache_trim = FALSE;
	vm_object_t shadow;

	while (object != VM_OBJECT_NULL) {

		/*
		 *	The cache holds a reference (uncounted) to
		 *	the object; we must lock it before removing
		 *	the object.
		 */

		vm_object_cache_lock();
		vm_object_lock(object);
		assert(object->alive);

		/*
		 *	Lose the reference. If other references
		 *	remain, then we are done, unless we need
		 *	to retry a cache trim.
		 *	If it is the last reference, then keep it
		 *	until any pending initialization is completed.
		 */

		assert(object->ref_count > 0);
		if (object->ref_count > 1) {
			object->ref_count--;
			vm_object_unlock(object);
			vm_object_cache_unlock();
			if (retry_cache_trim &&
			    ((object = vm_object_cache_trim(TRUE)) !=
			     VM_OBJECT_NULL)) {
				continue;
			}
			return;
		}

		/*
		 *	We have to wait for initialization
		 *	before destroying or caching the object.
		 */
		
		if (object->pager_created && ! object->pager_initialized) {
			assert(! object->can_persist);
			vm_object_assert_wait(object,
					      VM_OBJECT_EVENT_INITIALIZED,
					      FALSE);
			vm_object_unlock(object);
			vm_object_cache_unlock();
			thread_block((void (*)(void))0);
			continue;
		}

#if	NORMA_VM
		/*
		 * We have to synchronize m_o_caching() with
		 * m_o_set_attributes()/m_o_destroy() and vm_object_uncache().
		 */
		if (object->temporary_caching) {
			vm_object_assert_wait(object,
					      VM_OBJECT_EVENT_CACHING,
					      FALSE);
			vm_object_unlock(object);
			vm_object_cache_unlock();
			thread_block((void (*)(void)) 0);
			continue;
		}
#endif	/* NORMA_VM */

		/*
		 *	If this object can persist, then enter it in
		 *	the cache. Otherwise, terminate it.
		 */

		if (object->can_persist
#if	NORMA_VM
		    &&
		    (!object->temporary || object->resident_page_count > 0)
#endif	/* NORMA_VM */
		    ) {
#if	NORMA_VM
			/*
			 *	If we are about to cache a temporary object,
			 *	we must tell the memory manager, so that it
			 *	can coordinate a termination when appropriate.
			 *
			 *	We might, however, simply be returning
			 *	an object to the cache without having
			 *	asked to remove it, if we were pulled
			 *	out of the cache by vm_object_lookup
			 *	on behalf of a memory manager operation.
			 */
			if (object->temporary && ! object->temporary_cached) {
				assert(!object->temporary_caching);
				object->temporary_caching = TRUE;
				vm_object_unlock(object);
				vm_object_cache_unlock();
				memory_object_caching(object->pager,
						      object->pager_request);
				vm_object_cache_lock();
				vm_object_lock(object);
				object->temporary_cached = TRUE;
				object->temporary_caching = FALSE;
				vm_object_wakeup(object,
						 VM_OBJECT_EVENT_CACHING);
			}
#endif	/* NORMA_VM */
			/*
			 *	Now it is safe to decrement reference count,
			 *	and to return if reference count is > 0.
			 */
			if (--object->ref_count > 0) {
				vm_object_unlock(object);
				vm_object_cache_unlock();
				if (retry_cache_trim &&
				    ((object = vm_object_cache_trim(TRUE)) !=
				     VM_OBJECT_NULL)) {
					continue;
				}
				return;
			}

#if	MIGHT_NOT_CACHE_SHADOWS
			/*
			 *	Remove shadow now if we don't
			 *	want to cache shadows.
			 */
			if (! cache_shadows) {
				shadow = object->shadow;
				object->shadow = VM_OBJECT_NULL;
			}
#endif	/* MIGHT_NOT_CACHE_SHADOWS */

			/*
			 *	Enter the object onto the queue of
			 *	cached objects, and deactivate
			 *	all of its pages.
			 */

			vm_object_cached_count++;
			if (vm_object_cached_count > vm_object_cached_high)
				vm_object_cached_high = vm_object_cached_count;
			queue_enter(&vm_object_cached_list, object,
				vm_object_t, cached_list);
			vm_object_cache_unlock();
			vm_object_deactivate_pages(object);
			vm_object_unlock(object);

#if	MIGHT_NOT_CACHE_SHADOWS
			/*
			 *	If we have a shadow that we need
			 *	to deallocate, do so now, remembering
			 *	to trim the cache later.
			 */
			if (! cache_shadows && shadow != VM_OBJECT_NULL) {
				object = shadow;
				retry_cache_trim = TRUE;
				continue;
			}
#endif	/* MIGHT_NOT_CACHE_SHADOWS */

			/*
			 *	Trim the cache. If the cache trim
			 *	returns with a shadow for us to deallocate,
			 *	then remember to retry the cache trim
			 *	when we are done deallocating the shadow.
			 *	Otherwise, we are done.
			 */

			object = vm_object_cache_trim(TRUE);
			if (object == VM_OBJECT_NULL) {
				return;
			}
			retry_cache_trim = TRUE;

		} else {
			/*
			 *	This object is not cachable; terminate it.
			 */

#if	NORMA_VM
			/*
			 *	Wake up anyone waiting for a reply to an
			 *	uncache request. They will see that
			 *	object->pager is now null, indicating
			 *	that the object has destroyed and thus
			 *	that they have lost.
			 */
			if (object->temporary_uncaching) {
				assert(object->temporary_cached);
				object->temporary_cached = FALSE;
				object->temporary_uncaching = FALSE;
				vm_object_wakeup(object,
						 VM_OBJECT_EVENT_UNCACHING);
			}
#endif	/* NORMA_VM */
			/*
			 *	Terminate this object. If it had a shadow,
			 *	then deallocate it; otherwise, if we need
			 *	to retry a cache trim, do so now; otherwise,
			 *	we are done.
			 *
			 *	Keep ref_count to one in order to prevent
			 *	multiple calls to vm_object_deallocate().
			 */
			shadow = object->shadow;
			vm_object_terminate(object);
			if (shadow != VM_OBJECT_NULL) {
				object = shadow;
				continue;
			}
			if (retry_cache_trim &&
			    ((object = vm_object_cache_trim(TRUE)) !=
			     VM_OBJECT_NULL)) {
				continue;
			}
			return;
		}
	}
	assert(! retry_cache_trim);
}

/*
 *	Check to see whether we really need to trim
 *	down the cache. If so, remove an object from
 *	the cache, terminate it, and repeat.
 *
 *	Called with, and returns with, cache lock unlocked.
 */
vm_object_t
vm_object_cache_trim(
	boolean_t called_from_vm_object_deallocate)
{
	register vm_object_t object = VM_OBJECT_NULL;
	vm_object_t shadow;

	for (;;) {

		/*
		 *	If we no longer need to trim the cache,
		 *	then we are done.
		 */

		vm_object_cache_lock();
		if (vm_object_cached_count <= vm_object_cached_max) {
			vm_object_cache_unlock();
			return VM_OBJECT_NULL;
		}

		/*
		 *	We must trim down the cache, so remove
		 *	the first object in the cache.
		 */

		object = (vm_object_t) queue_first(&vm_object_cached_list);
		vm_object_lock(object);
		queue_remove(&vm_object_cached_list, object, vm_object_t,
			     cached_list);
		vm_object_cached_count--;

		/*
		 *	Since this object is in the cache, we know
		 *	that it is initialized and has no references.
		 *	Take a reference to avoid recursive deallocations.
		 */

		assert(object->pager_initialized);
		assert(object->ref_count == 0);
		object->ref_count++;

		/*
		 *	Terminate the object.
		 *	If the object had a shadow, we let
		 *	vm_object_deallocate deallocate it.
		 *	(We are careful here to limit recursion.)
		 */
		shadow = object->shadow;
		vm_object_terminate(object);
		if (shadow != VM_OBJECT_NULL) {
			if (called_from_vm_object_deallocate) {
				return shadow;
			} else {
				vm_object_deallocate(shadow);
			}
		}
	}
}

boolean_t	vm_object_terminate_remove_all = FALSE;

#if	NORMA_VM
#define	SYNCHRONOUS_TERMINATION	0
#if	MACH_ASSERT
int	c_terminate_reservations = 0;
int	c_terminate_pending_completions = 0;
#endif	/* MACH_ASSERT */
#endif	/* NORMA_VM */

/*
 *	Routine:	vm_object_terminate
 *	Purpose:
 *		Free all resources associated with a vm_object.
 *	In/out conditions:
 *		Upon entry, the object and the cache must be locked,
 *		and the object must have exactly one reference.
 *
 *		The shadow object reference is left alone.
 *
 *		Upon exit, the cache will be unlocked, and the
 *		object will cease to exist.
 */
void
vm_object_terminate(
	register vm_object_t	object)
{
	register vm_page_t	p;
	vm_object_t		shadow_object;
        vm_object_rep_t 	repo;
#if	NORMA_VM
	unsigned int		obj_type;
#endif

	assert(object->ref_count == 1);

	/*
	 *	Make sure the object isn't already being terminated
	 */

	assert(object->alive);
	object->alive = FALSE;

	/*
	 *	Make sure no one can look us up now.
	 */

	vm_object_remove(object);
	vm_object_cache_unlock();

	/*
	 *	Detach the object from its shadow if we are the shadow's
	 *	copy.
	 */
	if ((shadow_object = object->shadow) != VM_OBJECT_NULL) {
		vm_object_lock(shadow_object);
		assert((shadow_object->copy == object) ||
		       (shadow_object->copy == VM_OBJECT_NULL));
		shadow_object->copy = VM_OBJECT_NULL;
		vm_object_unlock(shadow_object);
	}

	/*
	 *	The pageout daemon might be playing with our pages.
	 *	Now that the object is dead, it won't touch any more
	 *	pages, but some pages might already be on their way out.
	 *	Hence, we wait until the active paging activities have ceased.
	 */

	vm_object_paging_wait(object, FALSE);
#if	!NORMA_VM
	object->ref_count--;
#endif	/* !NORMA_VM */

	/*
	 *	Clean or free the pages, as appropriate.
	 *	It is possible for us to find busy/absent pages,
	 *	if some faults on this object were aborted.
	 */

	if (object->temporary && ! object->can_persist ||
	    object->pager == IP_NULL) {
		while (!queue_empty(&object->memq)) {
			p = (vm_page_t) queue_first(&object->memq);

			VM_PAGE_CHECK(p);

			if (p->busy && !p->absent)
				panic("vm_object_terminate.2 0x%x 0x%x",
				      object, p);

			VM_PAGE_FREE(p);
		}
	} else while (!queue_empty(&object->memq)) {
		p = (vm_page_t) queue_first(&object->memq);

		VM_PAGE_CHECK(p);

		if (p->busy && !p->absent)
			panic("vm_object_terminate.3 0x%x 0x%x", object, p);

		vm_page_lock_queues();
		VM_PAGE_QUEUES_REMOVE(p);
		vm_page_unlock_queues();

		if (p->absent || p->private) {

			/*
			 *	For private pages, VM_PAGE_FREE just
			 *	leaves the page structure around for
			 *	its owner to clean up.  For absent
			 *	pages, the structure is returned to
			 *	the appropriate pool.
			 */

			goto free_page;
		}

		if (p->fictitious)
			panic("vm_object_terminate.4 0x%x 0x%x", object, p);

		if (!p->dirty)
			p->dirty = pmap_is_modified(p->phys_addr);

		if (p->dirty || p->precious) {
			p->busy = TRUE;
#if	NORMA_VM
			/*
			 *	Clean up dirty page.  This is not time
			 *	critical, so let the pages go into the
			 *	lock_request reservation pool.  Wait
			 *	until we acquire a reservation.  Busy
			 *	the page first to prevent the pageout
			 *	daemon from stealing it out from under
			 *	us.
			 */
			assert(object->pager != IP_NULL);
			if (object->internal)
				obj_type = PAGEOUT_RESERVE_INTERNAL;
			else if (IP_NORMA_IS_PROXY(object->pager))
				obj_type = PAGEOUT_RESERVE_EXTERNAL;
			else
				obj_type = PAGEOUT_RESERVE_LOCAL;
			while (vm_pageout_reserve(object, p->offset, page_size,
						  obj_type) == FALSE) {
#if	MACH_ASSERT
				int old_resident_count =
					object->resident_page_count;
#endif	/* MACH_ASSERT */
				vm_object_unlock(object);
				vm_pageout_await_completed(obj_type);
				vm_object_lock(object);

				assert(!object->alive);
				assert(!object->paging_in_progress);
				assert(old_resident_count >=
				       object->resident_page_count);
			}
#if	MACH_ASSERT
			++c_terminate_reservations;
#endif	/* MACH_ASSERT */
#endif	/* NORMA_VM */
			vm_pageout_page(p, FALSE, TRUE); /* flush page */
		} else {
		    free_page:
		    	VM_PAGE_FREE(p);
		}
	}

	assert(object->paging_in_progress == 0);

#if	NORMA_VM && SYNCHRONOUS_TERMINATION
	assert(object->completions_pending >= 0);
	while (object->completions_pending > 0) {
		/*
		 *	Wait for the data_write_completed
		 *	messages to return.
		 */
		vm_object_wait(object, VM_OBJECT_EVENT_COMPLETION, FALSE);
		vm_object_lock(object);
#if	MACH_ASSERT
		++c_terminate_pending_completions;
#endif
		continue;
	}
	assert(object->completions_pending == 0);
	assert(object->ref_count == 1);
#endif

#if	MOR
	/*
	 *	Tear down the list of representatives associated with the
	 *	object. Since the interface does not provide for returning 
	 *	rep port rights back to the memory manager, we'll deallocate
	 *	them here.
	 *
	 *	XXX Interface should probably provide for returning rep port
	 *	rights to the memory manager via memory_object_terminate.
	 */

	queue_iterate(&object->rep_head, repo, vm_object_rep_t, rep_list) {
		queue_remove(&object->rep_head, repo, vm_object_rep_t, 
			     rep_list);
		assert(ip_kotype(repo->rep) == IKOT_NONE);
		kfree((vm_offset_t)repo, sizeof (struct vm_object_rep));
	}
#endif	/* MOR */

	/*
	 *	Throw away port rights... note that they may
	 *	already have been thrown away (by vm_object_destroy
	 *	or memory_object_destroy).
	 *
	 *	Instead of destroying the control and name ports,
	 *	we send all rights off to the memory manager instead,
	 *	using memory_object_terminate.
	 */

	if (object->pager != IP_NULL) {
		/* consumes our rights for pager, pager_request, pager_name */
		vm_object_unlock(object);
		memory_object_release(object->pager,
				      object->pager_request,
				      object->pager_name);
#if	NORMA_VM
		/*
		 *	Before erasing pending reservations, inform
		 *	the XMM server layer that no more page-
		 *	outs are forthcoming.  The server layer will
		 *	forget about the object and won't be able
		 *	to find it through vm_object_lookup, so any
		 *	remaining reservations will be orphaned.
		 *	We explicitly clean up these orphans.
		 *	XXX Much prefer to synchronously await
		 *	all completion messages.  XXX
		 */
		vm_object_lock(object);
		assert(object->pager != IP_NULL);
		if (object->completions_pending != 0) {
			vm_pageout_reserve_terminate(object);
#if	SYNCHRONOUS_TERMINATION
			printf("vm_object_terminate: reserve_terminate!\n");
#endif
		}
		assert(object->completions_pending == 0);
#else	/* NORMA_VM */
	} else {
		vm_object_unlock(object);
		if (object->pager_name != IP_NULL) {
			/* consumes our right for pager_name */
			ipc_port_dealloc_kernel(object->pager_name);
		}
#endif	/* NORMA_VM */
	}

#if	NORMA_VM
	/*
	 * If there are outstanding m_o_data_write_completed(), wait for
	 *	them to complete and finally release vm_object lock.
	 */
	object->ref_count--;
	while (object->ref_count != 0) {
		vm_object_wait(object, VM_OBJECT_EVENT_COMPLETION, FALSE);
		vm_object_lock(object);
	}
	vm_object_unlock(object);

	assert(ip_kotype(object->object_name) == IKOT_PAGING_NAME);
	ipc_kobject_set(object->object_name, IKO_NULL, IKOT_NONE);
	ipc_port_dealloc_kernel(object->object_name);
#endif	/* NORMA_VM */

#if	MACH_PAGEMAP
	vm_external_destroy(&object->existence_info);
#endif	/* MACH_PAGEMAP */

	/*
	 *	Free the space for the object.
	 */

#if	VM_OBJECT_TRACKING
	vm_object_track_dealloc(object);
#endif
	zfree(vm_object_zone, (vm_offset_t) object);
}

/*
 *	Routine:	vm_object_pager_wakeup
 *	Purpose:	Wake up anyone waiting for IKOT_PAGER_TERMINATING
 */

void
vm_object_pager_wakeup(
	ipc_port_t	pager)
{
	boolean_t someone_waiting;

	/*
	 *	If anyone was waiting for the memory_object_terminate
	 *	to be queued, wake them up now.
	 */
	vm_object_cache_lock();
	assert(ip_kotype(pager) == IKOT_PAGER_TERMINATING);
	someone_waiting = (pager->ip_kobject != IKO_NULL);
	if (ip_active(pager))
		ipc_kobject_set(pager, IKO_NULL, IKOT_NONE);
	vm_object_cache_unlock();
	if (someone_waiting) {
		thread_wakeup((int) pager);
	}
}

/*
 *	Routine:	memory_object_release
 *	Purpose:	Terminate the pager and release port rights,
 *			just like memory_object_terminate, except
 *			that we wake up anyone blocked in vm_object_enter
 *			waiting for termination message to be queued
 *			before calling memory_object_init.
 */
void
memory_object_release(
	ipc_port_t	pager,
	pager_request_t	pager_request,
	ipc_port_t	pager_name)
{

	/*
	 *	Keep a reference to pager port;
	 *	the terminate might otherwise release all references.
	 */
	ipc_port_copy_send(pager);

	/*
	 *	Terminate the pager.
	 */
	(void) memory_object_terminate(pager, pager_request, pager_name);

	/*
	 *	Wakeup anyone waiting for this terminate
	 */
	vm_object_pager_wakeup(pager);

	/*
	 *	Release reference to pager port.
	 */
	ipc_port_release_send(pager);
}

/*
 *	Routine:	vm_object_abort_activity [internal use only]
 *	Purpose:
 *		Abort paging requests pending on this object.
 *	In/out conditions:
 *		The object is locked on entry and exit.
 */
void
vm_object_abort_activity(
	vm_object_t	object)
{
	register
	vm_page_t	p;
	vm_page_t	next;

	/*
	 *	Abort all activity that would be waiting
	 *	for a result on this memory object.
	 *
	 *	We could also choose to destroy all pages
	 *	that we have in memory for this object, but
	 *	we don't.
	 */

	p = (vm_page_t) queue_first(&object->memq);
	while (!queue_end(&object->memq, (queue_entry_t) p)) {
		next = (vm_page_t) queue_next(&p->listq);

		/*
		 *	If it's being paged in, destroy it.
		 *	If an unlock has been requested, start it again.
		 */

		if (p->busy && p->absent) {
			VM_PAGE_FREE(p);
		}
		 else {
		 	if (p->unlock_request != VM_PROT_NONE)
			 	p->unlock_request = VM_PROT_NONE;
			PAGE_WAKEUP(p);
		}
		
		p = next;
	}

	/*
	 *	Wake up threads waiting for the memory object to
	 *	become ready.
	 */

	object->pager_ready = TRUE;
	vm_object_wakeup(object, VM_OBJECT_EVENT_PAGER_READY);
}

/*
 *	Routine:	memory_object_destroy [user interface]
 *	Purpose:
 *		Shut down a memory object, despite the
 *		presence of address map (or other) references
 *		to the vm_object.
 */
kern_return_t
memory_object_destroy(
	register vm_object_t	object,
	kern_return_t		reason)
{
	ipc_port_t	old_object, old_pager_name;
	pager_request_t	old_pager_request;

#ifdef	lint
	reason++;
#endif	/* lint */

	if (object == VM_OBJECT_NULL)
		return(KERN_SUCCESS);

	/*
	 *	Remove the port associations immediately.
	 *
	 *	This will prevent the memory manager from further
	 *	meddling.  [If it wanted to flush data or make
	 *	other changes, it should have done so before performing
	 *	the destroy call.]
	 */

	vm_object_cache_lock();
	vm_object_lock(object);
	vm_object_remove(object);
	object->can_persist = FALSE;
	vm_object_cache_unlock();

#if 	NORMA_VM
	/*
	 * Destroy calls are asynchronous, so finding
	 * completions_pending would not be unusual.
	 */
	if (object->completions_pending)
		vm_pageout_reserve_terminate(object);
	assert(object->completions_pending == 0);
#endif	/* NORMA_VM */

	/*
	 *	Rip out the ports from the vm_object now... this
	 *	will prevent new memory_object calls from succeeding.
	 */

	old_object = object->pager;
	old_pager_request = object->pager_request;
	old_pager_name = object->pager_name;

	object->pager = IP_NULL;
	object->pager_request = PAGER_REQUEST_NULL;
	object->pager_name = IP_NULL;

#if	NORMA_VM
	/*
	 *	Wake up anyone waiting for a reply to an uncache request.
	 *	They will see that object->pager is now null, indicating
	 *	that the object has destroyed and thus that they have lost.
	 */
	if (object->temporary_uncaching) {
		assert(object->temporary_cached);
		object->temporary_cached = FALSE;
		object->temporary_uncaching = FALSE;
		vm_object_wakeup(object, VM_OBJECT_EVENT_UNCACHING);
	}
#endif

	/*
	 *	Wait for existing paging activity (that might
	 *	have the old ports) to subside.
	 */

	vm_object_paging_wait(object, FALSE);
	vm_object_unlock(object);

	/*
	 *	Shut down the ports now.
	 *
	 *	[Paging operations may be proceeding concurrently --
	 *	they'll get the null values established above.]
	 */

	if (old_object != IP_NULL) {
		/* consumes our rights for object, control, name */
		memory_object_release(old_object, old_pager_request,
					     old_pager_name);
#if	!NORMA_VM
	} else if (old_pager_name != IP_NULL) {
		/* consumes our right for name */
		ipc_port_dealloc_kernel(old_pager_name);
#endif	/* !NORMA_VM */
	}

	/*
	 *	Lose the reference that was donated for this routine
	 */

	vm_object_deallocate(object);

	return(KERN_SUCCESS);
}

#if	NORMA_VM
/*
 *	Routine:	vm_object_uncache
 *	Purpose:
 *		Uncache a temporary object, or kill it if it
 *		is no longer uncachable. Return TRUE iff
 *		we succeeded in uncaching the object.
 *	Notes:
 *		Called with object locked and not in vm_object_cache,
 *		but with object->temporary_cached TRUE.
 *		Returns with object locked if it is still alive.
 *
 *		Should consume object ref_count iff failure.
 */
boolean_t
vm_object_uncache(
	vm_object_t	object,
	boolean_t	may_uncache)
{
	assert(object->temporary);
	assert(object->temporary_cached);
	assert(object->can_persist);
	assert(object->ref_count > 0);

	/*
	 * If no one else has sent an uncache request, then do so now.
	 */
	if (! object->temporary_uncaching) {
		object->temporary_uncaching = TRUE;
		vm_object_unlock(object);
		memory_object_uncaching(object->pager, object->pager_request);
		vm_object_lock(object);
	}

	/*
	 * Wait for the response to the uncache request.
	 */
	while (object->temporary_cached) {
		vm_object_wait(object, VM_OBJECT_EVENT_UNCACHING, FALSE);
		vm_object_lock(object);
	}

	/*
	 * If the object was destroyed, the deallocate object
	 * and return failure; otherwise, return success.
	 */
	if (object->pager == IP_NULL) {
		assert(object->ref_count > 0);
		vm_object_unlock(object);
		vm_object_deallocate(object);
		return FALSE;
	} else {
		return TRUE;
	}
}

kern_return_t
memory_object_uncaching_permitted(
	vm_object_t	object)
{
	assert(object != VM_OBJECT_NULL);

	vm_object_lock(object);
	assert(object->temporary_cached);
	assert(object->temporary_uncaching);
	object->temporary_cached = FALSE;
	object->temporary_uncaching = FALSE;
	vm_object_wakeup(object, VM_OBJECT_EVENT_UNCACHING);
	vm_object_unlock(object);

	return KERN_SUCCESS;
}
#endif	/* NORMA_VM */

/*
 *	vm_object_deactivate_pages
 *
 *	Deactivate all pages in the specified object.  (Keep its pages
 *	in memory even though it is no longer referenced.)
 *
 *	The object must be locked.
 */
void
vm_object_deactivate_pages(
	register vm_object_t	object)
{
	register vm_page_t	p;

	queue_iterate(&object->memq, p, vm_page_t, listq) {
		vm_page_lock_queues();
		if (!p->busy)
			vm_page_deactivate(p);
		vm_page_unlock_queues();
	}
}


/*
 *	Routine:	vm_object_pmap_protect
 *
 *	Purpose:
 *		Reduces the permission for all physical
 *		pages in the specified object range.
 *
 *		If removing write permission only, it is
 *		sufficient to protect only the pages in
 *		the top-level object; only those pages may
 *		have write permission.
 *
 *		If removing all access, we must follow the
 *		shadow chain from the top-level object to
 *		remove access to all pages in shadowed objects.
 *
 *		The object must *not* be locked.  The object must
 *		be temporary/internal.  
 *
 *              If pmap is not NULL, this routine assumes that
 *              the only mappings for the pages are in that
 *              pmap.
 */
boolean_t vm_object_pmap_protect_by_page = FALSE;

void
vm_object_pmap_protect(
	register vm_object_t	object,
	register vm_offset_t	offset,
	vm_offset_t		size,
	pmap_t			pmap,
	vm_offset_t		pmap_start,
	vm_prot_t		prot)
{
	if (object == VM_OBJECT_NULL)
	    return;

	vm_object_lock(object);

	assert(object->copy_strategy == MEMORY_OBJECT_COPY_SYMMETRIC);

	while (TRUE) {
	    if (object->resident_page_count > atop(size) / 2 &&
		    pmap != PMAP_NULL) {
		vm_object_unlock(object);
		pmap_protect(pmap, pmap_start, pmap_start + size, prot);
		return;
	    }

	    {
		register vm_page_t	p;
		register vm_offset_t	end;

		end = offset + size;

		queue_iterate(&object->memq, p, vm_page_t, listq) {
		    if (!p->fictitious &&
			(offset <= p->offset) &&
			(p->offset < end)) {
			if ((pmap == PMAP_NULL) ||
			    vm_object_pmap_protect_by_page) {
			    pmap_page_protect(p->phys_addr,
					      prot & ~p->page_lock);
			} else {
			    vm_offset_t	start =
					pmap_start +
					(p->offset - offset);

			    pmap_protect(pmap,
					 start,
					 start + PAGE_SIZE,
					 prot);
			}
		    }
		}
	    }

	    if (prot == VM_PROT_NONE) {
		/*
		 * Must follow shadow chain to remove access
		 * to pages in shadowed objects.
		 */
		register vm_object_t	next_object;

		next_object = object->shadow;
		if (next_object != VM_OBJECT_NULL) {
		    offset += object->shadow_offset;
		    vm_object_lock(next_object);
		    vm_object_unlock(object);
		    object = next_object;
		}
		else {
		    /*
		     * End of chain - we are done.
		     */
		    break;
		}
	    }
	    else {
		/*
		 * Pages in shadowed objects may never have
		 * write permission - we may stop here.
		 */
		break;
	    }
	}

	vm_object_unlock(object);
}

/*
 *	Routine:	vm_object_copy_slowly
 *
 *	Description:
 *		Copy the specified range of the source
 *		virtual memory object without using
 *		protection-based optimizations (such
 *		as copy-on-write).  The pages in the
 *		region are actually copied.
 *
 *	In/out conditions:
 *		The caller must hold a reference and a lock
 *		for the source virtual memory object.  The source
 *		object will be returned *unlocked*.
 *
 *	Results:
 *		If the copy is completed successfully, KERN_SUCCESS is
 *		returned.  If the caller asserted the interruptible
 *		argument, and an interruption occurred while waiting
 *		for a user-generated event, MACH_SEND_INTERRUPTED is
 *		returned.  Other values may be returned to indicate
 *		hard errors during the copy operation.
 *
 *		A new virtual memory object is returned in a
 *		parameter (_result_object).  The contents of this
 *		new object, starting at a zero offset, are a copy
 *		of the source memory region.  In the event of
 *		an error, this parameter will contain the value
 *		VM_OBJECT_NULL.
 */
kern_return_t
vm_object_copy_slowly(
	register vm_object_t	src_object,
	vm_offset_t		src_offset,
	vm_size_t		size,
	boolean_t		interruptible,
	vm_object_t		*_result_object)	/* OUT */
{
	vm_object_t	new_object;
	vm_offset_t	new_offset;

	if (size == 0) {
		vm_object_unlock(src_object);
		*_result_object = VM_OBJECT_NULL;
		return(KERN_INVALID_ARGUMENT);
	}

	/*
	 *	Prevent destruction of the source object while we copy.
	 */

	assert(src_object->ref_count > 0);
	src_object->ref_count++;
	vm_object_unlock(src_object);

	/*
	 *	Create a new object to hold the copied pages.
	 *	A few notes:
	 *		We fill the new object starting at offset 0,
	 *		 regardless of the input offset.
	 *		We don't bother to lock the new object within
	 *		 this routine, since we have the only reference.
	 */

	new_object = vm_object_allocate(size);
	new_offset = 0;

	assert(size == trunc_page(size));	/* Will the loop terminate? */

	for ( ;
	    size != 0 ;
	    src_offset += PAGE_SIZE, new_offset += PAGE_SIZE, size -= PAGE_SIZE
	    ) {
		vm_page_t	new_page;
		vm_fault_return_t result;

		while ((new_page = vm_page_alloc(new_object, new_offset))
				== VM_PAGE_NULL) {
			VM_PAGE_WAIT();
		}

		do {
			vm_prot_t	prot = VM_PROT_READ;
			vm_page_t	_result_page;
			vm_page_t	top_page;
			register
			vm_page_t	result_page;
			kern_return_t	error_code;

			vm_object_lock(src_object);
			vm_object_paging_begin(src_object);

			result = vm_fault_page(src_object, src_offset,
				VM_PROT_READ, FALSE, interruptible,
				&prot, &_result_page, &top_page,
				&error_code, FALSE);

			switch(result) {
				case VM_FAULT_SUCCESS:
					result_page = _result_page;

					/*
					 *	We don't need to hold the object
					 *	lock -- the busy page will be enough.
					 *	[We don't care about picking up any
					 *	new modifications.]
					 *
					 *	Copy the page to the new object.
					 *
					 *	POLICY DECISION:
					 *		If result_page is clean,
					 *		we could steal it instead
					 *		of copying.
					 */

					vm_object_unlock(result_page->object);
					vm_page_copy(result_page, new_page);

					/*
					 *	Let go of both pages (make them
					 *	not busy, perform wakeup, activate).
					 */

					new_page->busy = FALSE;
					new_page->dirty = TRUE;
					vm_object_lock(result_page->object);
					PAGE_WAKEUP_DONE(result_page);

					vm_page_lock_queues();
					if (!result_page->active &&
					    !result_page->inactive)
						vm_page_activate(result_page);
					vm_page_activate(new_page);
					vm_page_unlock_queues();

					/*
					 *	Release paging references and
					 *	top-level placeholder page, if any.
					 */

					vm_fault_cleanup(result_page->object,
							top_page);

					break;
				
				case VM_FAULT_RETRY:
					break;

				case VM_FAULT_MEMORY_SHORTAGE:
					VM_PAGE_WAIT();
					break;

				case VM_FAULT_FICTITIOUS_SHORTAGE:
					vm_page_more_fictitious();
					break;

				case VM_FAULT_INTERRUPTED:
					vm_page_free(new_page);
					vm_object_deallocate(new_object);
					vm_object_deallocate(src_object);
					*_result_object = VM_OBJECT_NULL;
					return(MACH_SEND_INTERRUPTED);

				case VM_FAULT_MEMORY_ERROR:
					/*
					 * A policy choice:
					 *	(a) ignore pages that we can't
					 *	    copy
					 *	(b) return the null object if
					 *	    any page fails [chosen]
					 */

					vm_page_free(new_page);
					vm_object_deallocate(new_object);
					vm_object_deallocate(src_object);
					*_result_object = VM_OBJECT_NULL;
					return(error_code ? error_code:
						KERN_MEMORY_ERROR);
			}
		} while (result != VM_FAULT_SUCCESS);
	}

	/*
	 *	Lose the extra reference, and return our object.
	 */

	vm_object_deallocate(src_object);
	*_result_object = new_object;
	return(KERN_SUCCESS);
}

/*
 *	Routine:	vm_object_copy_quickly
 *
 *	Purpose:
 *		Copy the specified range of the source virtual
 *		memory object, if it can be done without waiting
 *		for user-generated events.
 *
 *	Results:
 *		If the copy is successful, the copy is returned in
 *		the arguments; otherwise, the arguments are not
 *		affected.
 *
 *	In/out conditions:
 *		The object should be unlocked on entry and exit.
 */

boolean_t
vm_object_copy_quickly(
	vm_object_t	*_object,		/* INOUT */
	vm_offset_t	*_offset,		/* INOUT */
	boolean_t	*_src_needs_copy,	/* OUT */
	boolean_t	*_dst_needs_copy)	/* OUT */
{
	vm_object_t	object = *_object;

#ifdef	lint
	++*_offset;
#endif	/* lint */

	if (object == VM_OBJECT_NULL) {
		*_src_needs_copy = FALSE;
		*_dst_needs_copy = FALSE;
		return(TRUE);
	}

	vm_object_lock(object);

	if (object->copy_strategy == MEMORY_OBJECT_COPY_SYMMETRIC) {

		/*
		 *	Symmetric copy strategy.
		 *	Make another reference to the object.
		 *	Leave object/offset unchanged.
		 */

		assert(object->ref_count > 0);
		object->ref_count++;
		object->shadowed = TRUE;
		vm_object_unlock(object);

		/*
		 *	Both source and destination must make
		 *	shadows, and the source must be made
		 *	read-only if not already.
		 */

		*_src_needs_copy = TRUE;
		*_dst_needs_copy = TRUE;

		return(TRUE);
	}

#if	!NORMA_VM
	if (object->copy_strategy == MEMORY_OBJECT_COPY_DELAY) {

		/*
		 *	Asymmetric copy strategy.  Destination
		 *	must be copied (to allow copy object reuse).
		 *	Source is unaffected.
		 */

		vm_object_unlock(object);
		object = vm_object_copy_delayed(object);
		*_object = object;
		*_src_needs_copy = FALSE;
		*_dst_needs_copy = TRUE;
		return(TRUE);
	}
#endif	/* !NORMA_VM */

	vm_object_unlock(object);
	return(FALSE);
}

int copy_call_count = 0;
int copy_call_sleep_count = 0;
int copy_call_restart_count = 0;

/*
 *	Routine:	vm_object_copy_call [internal]
 *
 *	Description:
 *		Copy the source object (src_object), using the
 *		user-managed copy algorithm.
 *
 *	In/out conditions:
 *		The source object must be locked on entry.  It
 *		will be *unlocked* on exit.
 *
 *	Results:
 *		If the copy is successful, KERN_SUCCESS is returned.
 *		A new object that represents the copied virtual
 *		memory is returned in a parameter (*_result_object).
 *		If the return value indicates an error, this parameter
 *		is not valid.
 */
kern_return_t
vm_object_copy_call(
	vm_object_t	src_object,
	vm_object_t	*_result_object)	/* OUT */
{
	kern_return_t	kr;

	/*
	 *	If a copy is already in progress, wait and retry.
	 *
	 *	XXX
	 *	Consider making this call interruptable, as Mike
	 *	intended it to be.
	 *
	 *	XXXO
	 *	Need a counter or version or something to allow
	 *	us to use the copy that the currently requesting
	 *	thread is obtaining -- is it worth adding to the
	 *	vm object structure? Depends how common this case it.
	 */
	copy_call_count++;
	while (vm_object_wanted(src_object, VM_OBJECT_EVENT_COPY_CALL)) {
		vm_object_wait(src_object, VM_OBJECT_EVENT_COPY_CALL, FALSE);
		vm_object_lock(src_object);
		copy_call_restart_count++;
	}

	/*
	 *	Indicate (for the benefit of memory_object_create_copy)
	 *	that we want a copy for src_object. (Note that we cannot
	 *	do a real assert_wait before calling memory_object_copy,
	 *	so we simply set the flag.)
	 */

	vm_object_set_wanted(src_object, VM_OBJECT_EVENT_COPY_CALL);
	vm_object_unlock(src_object);

	/*
	 *	Ask the memory manager to give us a memory object
	 *	which represents a copy of the src object.
	 *	The memory manager may give us a memory object
	 *	which we already have, or it may give us a
	 *	new memory object. This memory object will arrive
	 *	via memory_object_create_copy.
	 */

#if	NORMA_VM
	kr = memory_object_copy(src_object->pager, src_object->pager_request);
#else	/* NORMA_VM */
	kr = KERN_FAILURE;	/* XXX need to change memory_object.defs */
#endif	/* NORMA_VM */
	if (kr != KERN_SUCCESS) {
		return kr;
	}

	/*
	 *	Wait for the copy to arrive.
	 */
	vm_object_lock(src_object);
	while (vm_object_wanted(src_object, VM_OBJECT_EVENT_COPY_CALL)) {
		vm_object_wait(src_object, VM_OBJECT_EVENT_COPY_CALL, FALSE);
		vm_object_lock(src_object);
		copy_call_sleep_count++;
	}
	vm_object_unlock(src_object);

	/*
	 *	Return the copy.
	 */
	assert(src_object->copy != VM_OBJECT_NULL);
	*_result_object = src_object->copy;
	return KERN_SUCCESS;
}

#if	!NORMA_VM
/*
 *	Routine:	vm_object_copy_delayed [internal]
 *
 *	Description:
 *		Copy the specified virtual memory object, using
 *		the asymmetric copy-on-write algorithm.
 *
 *	In/out conditions:
 *		The object must be unlocked on entry.
 *
 *		This routine will not block waiting for user-generated
 *		events.  It is not interruptible.
 */
vm_object_t
vm_object_copy_delayed(
	vm_object_t	src_object)
{
	vm_object_t	new_copy;
	vm_object_t	old_copy;
	vm_page_t	p;

	/*
	 *	The user-level memory manager wants to see
	 *	all of the changes to this object, but it
	 *	has promised not to make any changes on its own.
	 *
	 *	Perform an asymmetric copy-on-write, as follows:
	 *		Create a new object, called a "copy object"
	 *		 to hold pages modified by the new mapping
	 *		 (i.e., the copy, not the original mapping).
	 *		Record the original object as the backing
	 *		 object for the copy object.  If the
	 *		 original mapping does not change a page,
	 *		 it may be used read-only by the copy.
	 *		Record the copy object in the original
	 *		 object.  When the original mapping causes
	 *		 a page to be modified, it must be copied
	 *		 to a new page that is "pushed" to the
	 *		 copy object.
	 *		Mark the new mapping (the copy object)
	 *		 copy-on-write.  This makes the copy
	 *		 object itself read-only, allowing it
	 *		 to be reused if the original mapping
	 *		 makes no changes, and simplifying the
	 *		 synchronization required in the "push"
	 *		 operation described above.
	 *
	 *	The copy-on-write is said to be assymetric because
	 *	the original object is *not* marked copy-on-write.
	 *	A copied page is pushed to the copy object, regardless
	 *	which party attempted to modify the page.
	 *
	 *	Repeated asymmetric copy operations may be done.
	 *	If the original object has not been changed since
	 *	the last copy, its copy object can be reused.
	 *	Otherwise, a new copy object can be inserted
	 *	between the original object and its previous
	 *	copy object.  Since any copy object is read-only,
	 *	this cannot affect the contents of the previous copy
	 *	object.
	 *
	 *	Note that a copy object is higher in the object
	 *	tree than the original object; therefore, use of
	 *	the copy object recorded in the original object
	 *	must be done carefully, to avoid deadlock.
	 */

	/*
	 *	Allocate a new copy object before locking, even
	 *	though we may not need it later.
	 */

	new_copy = vm_object_allocate(src_object->size);

	vm_object_lock(src_object);

	/*
	 *	See whether we can reuse the result of a previous
	 *	copy operation.
	 */
 Retry:
	old_copy = src_object->copy;
	if (old_copy != VM_OBJECT_NULL) {
		/*
		 *	Try to get the locks (out of order)
		 */
		if (!vm_object_lock_try(old_copy)) {
			vm_object_unlock(src_object);

			mutex_pause();	/* wait a bit */

			vm_object_lock(src_object);
			goto Retry;
		}

		/*
		 *	Determine whether the old copy object has
		 *	been modified.
		 */

		if (old_copy->resident_page_count == 0 &&
		    !old_copy->pager_created) {
			/*
			 *	It has not been modified.
			 *
			 *	Return another reference to
			 *	the existing copy-object.
			 */
			assert(old_copy->ref_count > 0);
			old_copy->ref_count++;
			vm_object_unlock(old_copy);
			vm_object_unlock(src_object);

			vm_object_deallocate(new_copy);

			return(old_copy);
		}

		/*
		 *	The copy-object is always made large enough to
		 *	completely shadow the original object, since
		 *	it may have several users who want to shadow
		 *	the original object at different points.
		 */

		assert((old_copy->shadow == src_object) &&
		    (old_copy->shadow_offset == (vm_offset_t) 0));

		/*
		 *	Make the old copy-object shadow the new one.
		 *	It will receive no more pages from the original
		 *	object.
		 */

		src_object->ref_count--;	/* remove ref. from old_copy */
		assert(src_object->ref_count > 0);
		old_copy->shadow = new_copy;
		assert(new_copy->ref_count > 0);
		new_copy->ref_count++;
		vm_object_unlock(old_copy);	/* done with old_copy */
	}

	/*
	 *	Point the new copy at the existing object.
	 */

	new_copy->shadow = src_object;
	new_copy->shadow_offset = 0;
	new_copy->shadowed = TRUE;	/* caller must set needs_copy */
	assert(src_object->ref_count > 0);
	src_object->ref_count++;
	src_object->copy = new_copy;

	/*
	 *	Mark all pages of the existing object copy-on-write.
	 *	This object may have a shadow chain below it, but
	 *	those pages will already be marked copy-on-write.
	 */

	queue_iterate(&src_object->memq, p, vm_page_t, listq) {
	    if (!p->fictitious)
		pmap_page_protect(p->phys_addr, 
				  (VM_PROT_ALL & ~VM_PROT_WRITE &
				   ~p->page_lock));
	}

	vm_object_unlock(src_object);
	
	return(new_copy);
}
#endif	/* !NORMA_VM */

/*
 *	Routine:	vm_object_copy_strategically
 *
 *	Purpose:
 *		Perform a copy according to the source object's
 *		declared strategy.  This operation may block,
 *		and may be interrupted.
 */
kern_return_t
vm_object_copy_strategically(
	register vm_object_t	src_object,
	vm_offset_t		src_offset,
	vm_size_t		size,
	vm_object_t		*dst_object,	/* OUT */
	vm_offset_t		*dst_offset,	/* OUT */
	boolean_t		*dst_needs_copy) /* OUT */
{
	boolean_t	result;
	boolean_t	interruptible = TRUE; /* XXX */
	memory_object_copy_strategy_t copy_strategy;

	if (src_object == VM_OBJECT_NULL) {
		return KERN_MEMORY_RESTART_COPY;
	}
	vm_object_lock(src_object);

	/*
	 *	The copy strategy is only valid if the memory manager
	 *	is "ready".
	 */

	while (!src_object->pager_ready) {
		vm_object_wait(	src_object,
				VM_OBJECT_EVENT_PAGER_READY,
				interruptible);
		if (interruptible &&
		    (current_thread()->wait_result != THREAD_AWAKENED)) {
			*dst_object = VM_OBJECT_NULL;
			*dst_offset = 0;
			*dst_needs_copy = FALSE;
			return(MACH_SEND_INTERRUPTED);
		}
		vm_object_lock(src_object);
	}

	copy_strategy = src_object->copy_strategy;

	/*
	 *	Use the appropriate copy strategy.
	 */

	switch (copy_strategy) {
	    case MEMORY_OBJECT_COPY_NONE:
		result = vm_object_copy_slowly(src_object, src_offset, size,
					       interruptible, dst_object);
		if (result == KERN_SUCCESS) {
			*dst_offset = 0;
			*dst_needs_copy = FALSE;
		}
		break;

	    case MEMORY_OBJECT_COPY_CALL:
		/*
		 * Adjust the object to the correct size; this will
		 * make pager object creation work out.
		 */
		if (src_object->size < size)
			src_object->size = size;
		result = vm_object_copy_call(src_object, dst_object);
		if (result == KERN_SUCCESS) {
			*dst_offset = src_offset;
			*dst_needs_copy = TRUE;
		}
		break;

#if	!NORMA_VM
	    case MEMORY_OBJECT_COPY_DELAY:
		vm_object_unlock(src_object);
		*dst_object = vm_object_copy_delayed(src_object);
		*dst_offset = src_offset;
		*dst_needs_copy = TRUE;
		result = KERN_SUCCESS;
		break;
#endif	/* !NORMA_VM */

	    case MEMORY_OBJECT_COPY_SYMMETRIC:
		vm_object_unlock(src_object);
		result = KERN_MEMORY_RESTART_COPY;
		break;

	    default:
		panic("copy_strategically: bad strategy");
		result = KERN_INVALID_ARGUMENT;
	}

	return(result);
}

#if	NORMA_VM
/*
 *	Called (as a synchronous RPC) because some kernel called
 *	memory_object_copy and a new copy needed to be created,
 *	or in response to this kernel calling memory_object_copy.
 *
 *	If the copy is being waited for, we leave a reference around
 *	to the copy for the waiting thread to pick up.
 */
kern_return_t
memory_object_create_copy(
	vm_object_t	src_object,
	ipc_port_t	new_copy_pager,
	kern_return_t	*result)
{
	vm_page_t	p;
	vm_object_t	new_copy;
	boolean_t	handoff_reference;
	boolean_t	internal;

	/*
	 *	Source object must be a valid object.
	 */
	if (src_object == VM_OBJECT_NULL) {
		if (IP_VALID(new_copy_pager))
			ipc_port_release_send(new_copy_pager);
		*result = KERN_INVALID_ARGUMENT;
		return KERN_SUCCESS;
	}

	/*
	 *	New copy pager must be a valid port.
	 */
	if (! IP_VALID(new_copy_pager)) {
		*result = KERN_INVALID_ARGUMENT;
		vm_object_deallocate(src_object);
		return KERN_SUCCESS;
	}

#if	NORMA_IPC
	internal = !new_copy_pager->ip_norma_is_proxy;
#else	/* NORMA_IPC */
	internal = TRUE;
#endif	/* NORMA_IPC */

	/*
	 *	If we aren't waiting for a copy, then we can return
	 *	without doing anything if we don't have an old copy
	 *	or if the new copy is the same as the old copy.
	 *
	 *	If we are waiting for a copy, then we can return if
	 *	we have an old copy and it is the same as the new copy.
	 */

	vm_object_lock(src_object);
	if (! vm_object_wanted(src_object, VM_OBJECT_EVENT_COPY_CALL)) {
		if (src_object->copy == VM_OBJECT_NULL ||
		    src_object->copy->pager == new_copy_pager) {
			vm_object_unlock(src_object);
			vm_object_deallocate(src_object);
			ipc_port_release_send(new_copy_pager);
			*result = KERN_ABORTED;
			return KERN_SUCCESS;
		}
		vm_object_unlock(src_object);
		handoff_reference = FALSE;
	} else {
		/*
		 * Try to get src_object and its copy locks (out of order),
		 *	and increment copy reference count, so that it won't
		 *	be removed during vm_enter_enter() call.
		 */
		while (src_object->copy != VM_OBJECT_NULL &&
		       src_object->copy->pager == new_copy_pager) {
			if (!vm_object_lock_try(src_object->copy)) {
				vm_object_unlock(src_object);
				mutex_pause();    /* wait a bit */
				vm_object_lock(src_object);
				continue;
			}
			new_copy = src_object->copy;
			new_copy->ref_count++;
			assert(new_copy->pager_initialized);
			vm_object_unlock(new_copy);
			vm_object_unlock(src_object);

			/*
			 *	Do a full-fledged vm_object_enter
			 *	to handle any setting of copy_strategy
			 *	and temporary for src_object->copy,
			 *	allowing general memory manager use
			 *	of copy_call, even though we know
			 *	(as of this writing) that the xmm
			 *	system creates permanent noncachable
			 *	copies.
			 *
			 *	The reference created by the vm_object_enter
			 *	is consumed by the thread waiting for
			 *	VM_OBJECT_COPY_CALL_EVENT.
			 */
#if	MACH_ASSERT
			assert(vm_object_enter(src_object->copy->pager,
					       src_object->copy->size,
					       internal, FALSE) ==
			       src_object->copy);
#else	/* MACH_ASSERT */
			(void) vm_object_enter(src_object->copy->pager,
					       src_object->copy->size,
					       internal, FALSE);
#endif	/* MACH_ASSERT */
			vm_object_lock(src_object);
			vm_object_wakeup(src_object,
					 VM_OBJECT_EVENT_COPY_CALL);
			vm_object_unlock(src_object);
			vm_object_deallocate(src_object);
			vm_object_deallocate(new_copy);
			ipc_port_release_send(new_copy_pager);
			*result = KERN_SUCCESS;
			return KERN_SUCCESS;
		}

		/*
		 *	The pager port has already been used as the pager port
		 *	of a copy object that does not exist anymore. It is an
		 *	invalid argument, because it won't be able to finish by
		 *	itself (in xmm_object_terminate), and thus it will lead
		 *	to a deadlock (with the forthcoming xmm_object_wait
		 *	indirectly called by vm_object_enter).
		 */
		vm_object_unlock(src_object);
		vm_object_cache_lock();
		if (ip_kotype(new_copy_pager) != IKOT_NONE) {
			vm_object_cache_unlock();
			ipc_port_release_send(new_copy_pager);
			*result = KERN_INVALID_ARGUMENT;
			return KERN_SUCCESS;
		}
		vm_object_cache_unlock();
		handoff_reference = TRUE;
	}

	/*
	 *	Create an object for new_copy_pager. If we fail,
	 *	or if already shadows an object other than src_object,
	 *	then return failure. (The latter can only happen
	 *	if we allow objects to hold onto their shadows while
	 *	being cached.) Otherwise, make new_copy shadow src_object
	 *	if it does not do so already.
	 */
	new_copy = vm_object_enter(new_copy_pager,
				   src_object->size, internal, FALSE);
	ipc_port_release_send(new_copy_pager);
	if (new_copy == VM_OBJECT_NULL ||
	    (new_copy->shadow != VM_OBJECT_NULL &&
	     new_copy->shadow != src_object)) {
		vm_object_deallocate(src_object);
		vm_object_deallocate(new_copy);
		*result = KERN_FAILURE;
		return KERN_FAILURE;
	}
	vm_object_lock(new_copy);

	/*
	 *	pager_ready flag of internal vm_object isn't set by
	 *	vm_object_enter on a newly created vm_object.
	 */
	if (new_copy->internal && !new_copy->pager_ready) {
		new_copy->pager_ready = TRUE;
		vm_object_wakeup(new_copy, VM_OBJECT_EVENT_PAGER_READY);
	}

        /*
         * Try to get vm_object and its oldcopy locks (out of order).
         */
        vm_object_lock(src_object);
	while (src_object->copy != VM_OBJECT_NULL) {
		if (vm_object_lock_try(src_object->copy))
			break;
		vm_object_unlock(src_object);
		mutex_pause();    /* wait a bit */
		vm_object_lock(src_object);
	}

	/*
	 *	Make new copy shadow src_object if it does not do so already.
	 *	(It might already if it was cached.)
	 */
	if (new_copy->shadow != src_object) {
		new_copy->shadow = src_object;
		new_copy->shadow_offset = 0;
		src_object->shadowed = TRUE;
		assert(src_object->ref_count > 0);
		src_object->ref_count++;
	}

	/*
	 *	If we have an old copy, make it shadow the new copy
	 *	instead of src_object.
	 */
	if (src_object->copy != VM_OBJECT_NULL) {
		src_object->copy->shadow = new_copy;
		new_copy->shadowed = TRUE;
		new_copy->ref_count++;
		src_object->ref_count--;
		assert(src_object->ref_count > 0);
		vm_object_unlock(src_object->copy);
	}

	/*
	 *	Adjust source object's copy pointer.
	 */
	src_object->copy = new_copy;
	vm_object_unlock(new_copy);

	/*
	 *	Remove write access from all of the pages of
	 *	the source object that we can.
	 */

	queue_iterate(&src_object->memq, p, vm_page_t, listq) {
		if (p->fictitious) {
			continue;
		}
		if (! (p->page_lock & VM_PROT_WRITE)) {
			p->page_lock |= VM_PROT_WRITE;
			p->unusual = TRUE;
			pmap_page_protect(p->phys_addr,
					  VM_PROT_ALL & ~p->page_lock);
		}
	}

	/*
	 *	Wake up anyone waiting for this copy, and return.
	 */
	if (handoff_reference) {
		assert(vm_object_wanted(src_object,
					VM_OBJECT_EVENT_COPY_CALL));
		vm_object_wakeup(src_object, VM_OBJECT_EVENT_COPY_CALL);
		vm_object_unlock(src_object);
	} else {
		vm_object_unlock(src_object);
		vm_object_deallocate(new_copy);
	}
	vm_object_deallocate(src_object);
	*result = KERN_SUCCESS;
	return KERN_SUCCESS;
}
#endif	/* NORMA_VM */

/*
 *	vm_object_shadow:
 *
 *	Create a new object which is backed by the
 *	specified existing object range.  The source
 *	object reference is deallocated.
 *
 *	The new object and offset into that object
 *	are returned in the source parameters.
 */
boolean_t vm_object_shadow_check = FALSE;

boolean_t
vm_object_shadow(
	vm_object_t	*object,	/* IN/OUT */
	vm_offset_t	*offset,	/* IN/OUT */
	vm_size_t	length)
{
	register vm_object_t	source;
	register vm_object_t	result;

	source = *object;
	assert(source->copy_strategy == MEMORY_OBJECT_COPY_SYMMETRIC);

	/*
	 *	Determine if we really need a shadow.
	 */

	if (vm_object_shadow_check && source->ref_count == 1 &&
	    (source->shadow == VM_OBJECT_NULL ||
	     source->shadow->copy == VM_OBJECT_NULL))
	{
		source->shadowed = FALSE;
		return FALSE;
	}

	/*
	 *	Allocate a new object with the given length
	 */

	if ((result = vm_object_allocate(length)) == VM_OBJECT_NULL)
		panic("vm_object_shadow: no object for shadowing");

	/*
	 *	The new object shadows the source object, adding
	 *	a reference to it.  Our caller changes his reference
	 *	to point to the new object, removing a reference to
	 *	the source object.  Net result: no change of reference
	 *	count.
	 */
	result->shadow = source;
	
	/*
	 *	Store the offset into the source object,
	 *	and fix up the offset into the new object.
	 */

	result->shadow_offset = *offset;

	/*
	 *	Return the new things
	 */

	*offset = 0;
	*object = result;
	return TRUE;
}

/*
 *	The relationship between vm_object structures and
 *	the memory_object ports requires careful synchronization.
 *
 *	All associations are created by vm_object_enter.  All three
 *	port fields are filled in, as follows:
 *		pager:	the memory_object port itself, supplied by
 *			the user requesting a mapping (or the kernel,
 *			when initializing internal objects); the
 *			kernel simulates holding send rights by keeping
 *			a port reference;
 *		pager_request:
 *		pager_name:
 *			the memory object control and name ports,
 *			created by the kernel; the kernel holds
 *			receive (and ownership) rights to these
 *			ports, but no other references.
 *	All of the ports are referenced by their global names.
 *
 *	When initialization is complete, the "initialized" field
 *	is asserted.  Other mappings using a particular memory object,
 *	and any references to the vm_object gained through the
 *	port association must wait for this initialization to occur.
 *
 *	In order to allow the memory manager to set attributes before
 *	requests (notably virtual copy operations, but also data or
 *	unlock requests) are made, a "ready" attribute is made available.
 *	Only the memory manager may affect the value of this attribute.
 *	Its value does not affect critical kernel functions, such as
 *	internal object initialization or destruction.  [Furthermore,
 *	memory objects created by the kernel are assumed to be ready
 *	immediately; the default memory manager need not explicitly
 *	set the "ready" attribute.]
 *
 *	[Both the "initialized" and "ready" attribute wait conditions
 *	use the "pager" field as the wait event.]
 *
 *	The port associations can be broken down by any of the
 *	following routines:
 *		vm_object_terminate:
 *			No references to the vm_object remain, and
 *			the object cannot (or will not) be cached.
 *			This is the normal case, and is done even
 *			though one of the other cases has already been
 *			done.
 *		vm_object_destroy:
 *			The memory_object port has been destroyed,
 *			meaning that the kernel cannot flush dirty
 *			pages or request new data or unlock existing
 *			data.
 *		memory_object_destroy:
 *			The memory manager has requested that the
 *			kernel relinquish rights to the memory object
 *			port.  [The memory manager may not want to
 *			destroy the port, but may wish to refuse or
 *			tear down existing memory mappings.]
 *	Each routine that breaks an association must break all of
 *	them at once.  At some later time, that routine must clear
 *	the vm_object port fields and release the port rights.
 *	[Furthermore, each routine must cope with the simultaneous
 *	or previous operations of the others.]
 *
 *	In addition to the lock on the object, the vm_object_cache_lock
 *	governs the port associations.  References gained through the
 *	port association require use of the cache lock.
 *
 *	Because the port fields may be cleared spontaneously, they
 *	cannot be used to determine whether a memory object has
 *	ever been associated with a particular vm_object.  [This
 *	knowledge is important to the shadow object mechanism.]
 *	For this reason, an additional "created" attribute is
 *	provided.
 *
 *	During various paging operations, the port values found in the
 *	vm_object must be valid.  To prevent these port rights from being
 *	released, and to prevent the port associations from changing
 *	(other than being removed, i.e., made null), routines may use
 *	the vm_object_paging_begin/end routines [actually, macros].
 *	The implementation uses the "paging_in_progress" and "wanted" fields.
 *	[Operations that alter the validity of the port values include the
 *	termination routines and vm_object_collapse.]
 */

#if	NORMA_VM
#define	IKOT_PAGER_LOOKUP_TYPE	IKOT_PAGER
#else	/* NORMA_VM */
#define	IKOT_PAGER_LOOKUP_TYPE	IKOT_PAGING_REQUEST
#endif	/* NORMA_VM */

vm_object_t
vm_object_lookup(
	ipc_port_t	port)
{
	vm_object_t	object = VM_OBJECT_NULL;

	if (IP_VALID(port)) {
		vm_object_cache_lock();
		ip_lock(port);
		if (ip_active(port) &&
		    (ip_kotype(port) == IKOT_PAGER_LOOKUP_TYPE)) {
			object = (vm_object_t) port->ip_kobject;
			vm_object_lock(object);

			assert(object->alive);

			if (object->ref_count == 0) {
				queue_remove(&vm_object_cached_list, object,
					     vm_object_t, cached_list);
				vm_object_cached_count--;
			}

			object->ref_count++;
			vm_object_unlock(object);
		}
		ip_unlock(port);
		vm_object_cache_unlock();
	}

	return object;
}

vm_object_t
vm_object_lookup_name(
	ipc_port_t	port)
{
	vm_object_t	object = VM_OBJECT_NULL;

	if (IP_VALID(port)) {
		vm_object_cache_lock();
		ip_lock(port);
		if (ip_active(port) &&
		    (ip_kotype(port) == IKOT_PAGING_NAME)) {
			object = (vm_object_t) port->ip_kobject;
			vm_object_lock(object);

			assert(object->alive);

			if (object->ref_count == 0) {
				queue_remove(&vm_object_cached_list, object,
					     vm_object_t, cached_list);
				vm_object_cached_count--;
			}

			object->ref_count++;
			vm_object_unlock(object);
		}
		ip_unlock(port);
		vm_object_cache_unlock();
	}

	return object;
}

#if	NORMA_VM
/*
 *	Special case:  object is terminating but pageouts
 *	are in progress against this object. Sometimes, it
 *	is still necessary to find the object when delivering
 *	memory_object_data_write_completed messages.
 */
vm_object_t
vm_object_lookup_terminating(
	ipc_port_t	port)
{
	vm_object_t	object = VM_OBJECT_NULL;

	if (IP_VALID(port)) {
		vm_object_cache_lock();
		ip_lock(port);
		if (ip_active(port) &&
		    (ip_kotype(port) == IKOT_PAGER_TERMINATING)) {
			object = (vm_object_t) port->ip_kobject;
			vm_object_lock(object);
			assert(!object->alive);
			if (object->completions_pending == 0) {
				vm_object_unlock(object);
				object = VM_OBJECT_NULL;
			} else {
				object->ref_count++;
				vm_object_unlock(object);
			}
		}
		ip_unlock(port);
		vm_object_cache_unlock();
	}

	return object;
}
#endif	/* NORMA_VM */

void
vm_object_destroy(
	ipc_port_t	pager)
{
	vm_object_t	object;
	pager_request_t	old_pager_request;
	ipc_port_t	old_pager_name;

	/*
	 *	Perform essentially the same operations as in vm_object_lookup,
	 *	except that this time we look up based on the memory_object
	 *	port, not the control port.
	 */
	vm_object_cache_lock();
	if (ip_kotype(pager) != IKOT_PAGER) {
		vm_object_cache_unlock();
		return;
	}

	object = (vm_object_t) pager->ip_kobject;
	vm_object_lock(object);
	if (object->ref_count == 0) {
		queue_remove(&vm_object_cached_list, object,
				vm_object_t, cached_list);
		vm_object_cached_count--;
	}
	object->ref_count++;

	object->can_persist = FALSE;

	assert(object->pager == pager);

	/*
	 *	Remove the port associations.
	 *
	 *	Note that the memory_object itself is dead, so
	 *	we don't bother with it.
	 */

	object->pager = IP_NULL;
	vm_object_remove(object);

	old_pager_request = object->pager_request;
	old_pager_name = object->pager_name;

	object->pager_request = PAGER_REQUEST_NULL;
	object->pager_name = IP_NULL;

	vm_object_unlock(object);
	vm_object_cache_unlock();

	/*
	 *	Clean up the port references.  Note that there's no
	 *	point in trying the memory_object_terminate call
	 *	because the memory_object itself is dead.
	 */

	ipc_port_release_send(pager);
#if	NORMA_VM
	if (old_pager_name != IP_NULL)
		ipc_port_release_send(old_pager_name);
#else	/* NORMA_VM */
	if (old_pager_name != IP_NULL)
		ipc_port_dealloc_kernel(old_pager_name);

	if (old_pager_request != IP_NULL)
		ipc_port_dealloc_kernel(old_pager_request);
#endif	/* NORMA_VM */

	/*
	 *	Restart pending page requests
	 */

	vm_object_abort_activity(object);

	/*
	 *	Lose the object reference.
	 */

	vm_object_deallocate(object);
}

#if	MOR
void
vm_object_rep_destroy(
	ipc_port_t	rep)
{
	vm_object_t	object;
	vm_object_rep_t	repo;

	if (ip_kotype(rep) != IKOT_PAGER_REP)
		return;

	repo = (vm_object_rep_t) rep->ip_kobject;

	if (repo->all_wanted != 0)
		panic("vm_object_rep_destroy: rep wanted");
	if (repo->constituent == VM_OBJECT_NULL)
		panic("vm_object_rep_destroy: null pager");

	object = repo->constituent;

	vm_object_lock(object);
	queue_remove(&object->rep_head, repo, vm_object_rep_t, rep_list);
	vm_object_unlock(object);

	ipc_kobject_set(repo->rep, IKO_NULL, IKOT_NONE);
	kfree((vm_offset_t)repo, sizeof (struct vm_object_rep));

	return;
}
#endif	/* MOR */

boolean_t	vm_object_accept_old_init_protocol = FALSE;

/*
 *	Routine:	vm_object_enter
 *	Purpose:
 *		Find a VM object corresponding to the given
 *		pager; if no such object exists, create one,
 *		and initialize the pager.
 */
vm_object_t
vm_object_enter(
	ipc_port_t	pager,
	vm_size_t	size,
	boolean_t	internal,
	boolean_t	init)
{
	register
	vm_object_t	object;
	vm_object_t	new_object;
	boolean_t	must_init;
	ipc_kobject_type_t po;

restart:
	if (!IP_VALID(pager))
		return(vm_object_allocate(size));

	new_object = VM_OBJECT_NULL;
	must_init = init;

	/*
	 *	Look for an object associated with this port.
	 */

	vm_object_cache_lock();
	for (;;) {
		po = ip_kotype(pager);

		/*
		 *	If a previous object is being terminated,
		 *	we must wait for the termination message
		 *	to be queued.
		 *
		 *	We set kobject to a non-null value to let the
		 *	terminator know that someone is waiting.
		 *	Among the possibilities is that the port
		 *	could die while we're waiting.  Must restart
		 *	instead of continuing the loop.
		 */

		if (po == IKOT_PAGER_TERMINATING) {
			pager->ip_kobject = (ipc_kobject_t) pager;
			assert_wait((int) pager, FALSE);
			vm_object_cache_unlock();
			thread_block((void (*)(void))0);
			goto restart;
		}

		/*
		 *	Bail if there is already a kobject associated
		 *	with the pager port.
		 */
		if (po != IKOT_NONE) {
			break;
		}

		/*
		 *	We must unlock to create a new object;
		 *	if we do so, we must try the lookup again.
		 */

		if (new_object == VM_OBJECT_NULL) {
			vm_object_cache_unlock();
			new_object = vm_object_allocate(size);
			vm_object_cache_lock();
		} else {
			/*
			 *	Lookup failed twice, and we have something
			 *	to insert; set the object.
			 */

			ipc_kobject_set(pager,
					(ipc_kobject_t) new_object,
					IKOT_PAGER);
			new_object = VM_OBJECT_NULL;
			must_init = TRUE;
		}
	}

	/*
	 *	It's only good if it's a VM object!
	 */

	object = (po == IKOT_PAGER) ? (vm_object_t) pager->ip_kobject : VM_OBJECT_NULL;

	if (object != VM_OBJECT_NULL && !must_init) {
		vm_object_lock(object);
		assert(object->pager_created);
		assert(!internal || object->internal);
		if (object->ref_count == 0) {
			queue_remove(&vm_object_cached_list, object,
				     vm_object_t, cached_list);
			vm_object_cached_count--;
		}
		object->ref_count++;
		vm_object_unlock(object);

		vm_stat.hits++;
	} 

	vm_stat.lookups++;

	vm_object_cache_unlock();

	/*
	 *	If we raced to create a vm_object but lost, let's
	 *	throw away ours.
	 */

	if (new_object != VM_OBJECT_NULL)
		vm_object_deallocate(new_object);

	if (object == VM_OBJECT_NULL)
		return(object);

	if (must_init) {
		/*
		 *	Copy the naked send right we were given.
		 */

		pager = ipc_port_copy_send(pager);
		if (!IP_VALID(pager))
			panic("vm_object_enter: port died"); /* XXX */

		object->pager_created = TRUE;
		object->pager = pager;

#if	NORMA_VM

		/*
		 *	Let the xmm system know that we want to use the pager.
		 *
		 *	Name port will be provided by the xmm system
		 *	when set_attributes_common is called.
		 *
		 *	For internal objects, pager_ready will be set after
		 * 	pages have been declared; otherwise, it will be set
		 *	via set_attributes_common.
		 */

		object->internal = internal;
		assert(object->pager_name == IP_NULL);
		if (! internal) {
			object->copy_strategy = MEMORY_OBJECT_COPY_INVALID;
		} else {
			object->pager_modwc = TRUE;
		}

		(void) xmm_memory_object_init(object);
#else	/* NORMA_VM */

		/*
		 *	Allocate request port.
		 */

		object->pager_request = ipc_port_alloc_kernel();
		if (object->pager_request == IP_NULL)
			panic("vm_object_enter: pager request alloc");

		ipc_kobject_set(object->pager_request,
				(ipc_kobject_t) object,
				IKOT_PAGING_REQUEST);

		/*
		 *	Let the pager know we're using it.
		 */

		if (internal) {
			/* acquire a naked send right for the DMM */
			ipc_port_t DMM = memory_manager_default_reference();

			/* mark the object internal */
			object->internal = TRUE;
			assert(object->temporary);

			/* default-pager objects are ready immediately */
			object->pager_ready = TRUE;

			/* consumes the naked send right for DMM */
			(void) memory_object_create(DMM,
				pager,
				object->size,
				object->pager_request,
				object->pager_name,
				PAGE_SIZE);
		} else {
			/* the object is external */
			object->internal = FALSE;

			/* user pager objects are not ready until marked so */
			object->pager_ready = FALSE;

			/* copy strategy invalid until set by memory manager */
			object->copy_strategy = MEMORY_OBJECT_COPY_INVALID;

			(void) memory_object_init(pager,
				object->pager_request,
				object->pager_name,
				PAGE_SIZE);

		}
#endif	/* NORMA_VM */

		vm_object_lock(object);
		object->pager_initialized = TRUE;

		if (vm_object_accept_old_init_protocol)
			object->pager_ready = TRUE;

		vm_object_wakeup(object, VM_OBJECT_EVENT_INITIALIZED);
	} else {
		vm_object_lock(object);


#if	NORMA_VM
		if (!internal) {
			/*
			 *	We have to synchronize m_o_caching() and
			 *	m_o_set_attributes()/m_o_destroy().
			 */
			while (object->temporary_caching) {
				vm_object_assert_wait(object,
						      VM_OBJECT_EVENT_CACHING,
						      FALSE);
				vm_object_unlock(object);
				thread_block((void (*)(void)) 0);
				vm_object_lock(object);
			}

			if (object->temporary_cached) {
				if (! vm_object_uncache(object, TRUE)) {
					goto restart;
				}
			}
		}
#endif	/* NORMA_VM */
	}
	/*
	 *	[At this point, the object must be locked]
	 */

	/*
	 *	Wait for the work above to be done by the first
	 *	thread to map this object.
	 */

	while (!object->pager_initialized) {
		vm_object_wait(	object,
				VM_OBJECT_EVENT_INITIALIZED,
				FALSE);
		vm_object_lock(object);
	}
	vm_object_unlock(object);

	return(object);
}

#if	MOR
/*
 *	Routine:	vm_object_enter_ext
 *	Purpose:
 *      	Find an external vm_object corresponding to the given 
 *		pager. If no such object exists, create one, and init-
 *		ialize the pager. This is similiar to vm_object_enter, 
 *		except we handle memory object representatives and the
 *		object must be external. The degenerate or backward-
 *		compatible no-representative case is also handled.
 */
vm_object_t	
vm_object_enter_ext(
	ipc_port_t      pager,
        vm_object_rep_t repo)
{
        vm_object_t     	object;
        vm_object_t     	new_object;
        boolean_t       	must_init;
        ipc_kobject_type_t 	po;

restart:
	if (!IP_VALID(pager)) 
                return(vm_object_allocate(0));

        must_init = FALSE;
        new_object = VM_OBJECT_NULL;

        /*
         *      Look for an object associated with this port.
         */

        vm_object_cache_lock();
        for (;;) {
                po = ip_kotype(pager);

                /*
                 *      If a previous object is being terminated,
                 *      we must wait for the termination message
                 *      to be queued.
                 *
                 *      We set kobject to a non-null value to let the
                 *      terminator know that someone is waiting.
                 *      Among the possibilities is that the port
                 *      could die while we're waiting.  Must restart
                 *      instead of continuing the loop.
                 */

                if (po == IKOT_PAGER_TERMINATING) {
                        pager->ip_kobject = (ipc_kobject_t) pager;
                        assert_wait((int) pager, FALSE);
                        vm_object_cache_unlock();
			thread_block((void (*)(void))0);
                        goto restart;
                }

                /*
                 *      Bail if there is already a kobject associated
                 *      with the pager port.
                 */
                if (po != IKOT_NONE) {
                        break;
                }

                /*
                 *      We must unlock to create a new object;
                 *      if we do so, we must try the lookup again.
                 */

                if (new_object == VM_OBJECT_NULL) {
                        vm_object_cache_unlock();
                        new_object = vm_object_allocate(0);
                        vm_object_cache_lock();
                } else {
                        /*
                         *      Lookup failed twice, and we have something
                         *      to insert; set the object.
                         */

                        ipc_kobject_set(pager,
                                        (ipc_kobject_t) new_object,
                                        IKOT_PAGER);
                        new_object = VM_OBJECT_NULL;
                        must_init = TRUE;
                }
	}

        /*
         *      It's only good if it's a VM object!
         */

        object = (po == IKOT_PAGER) ? 
		 (vm_object_t) pager->ip_kobject : VM_OBJECT_NULL;

        if ((object != VM_OBJECT_NULL) && !must_init) {
                vm_object_lock(object);
                if (object->ref_count == 0) {
                        queue_remove(&vm_object_cached_list, object,
                                     vm_object_t, cached_list);
                        vm_object_cached_count--;
                }
                object->ref_count++;
                vm_object_unlock(object);

                vm_stat.hits++;
        }
        assert((object == VM_OBJECT_NULL) || (object->ref_count > 0));

        vm_stat.lookups++;

        vm_object_cache_unlock();

        /*
         *      If we raced to create a vm_object but lost, let's
         *      throw away ours.
         */

        if (new_object != VM_OBJECT_NULL)
                vm_object_deallocate(new_object);

        if (object == VM_OBJECT_NULL) {
                return(object);
	}

        if (must_init) {
                /*
                 *      Copy the naked send right we were given.
                 */

                pager = ipc_port_copy_send(pager);
                if (!IP_VALID(pager))
                        panic("vm_object_enter_ext: port died"); 

		/*
		 *	Initialize new vm_object. Enter the rep onto obj's list
		 *	of representatives if a rep was used in the vm_map call
		 *	rather than the pager port itself (i.e. if pager port 
                 *      and rep port differ). Should not get here without a 
		 *	representative structure.
		 */
                object->pager = pager;
                object->pager_created = TRUE;
		assert (repo != VM_OBJECT_REP_NULL);
		object->pager_request = repo->pager_request; 	/* XXX */
		if (repo->rep != pager) {
			queue_enter(&object->rep_head, repo, vm_object_rep_t, 
				    rep_list);
		}

                /*
		 *	Fill in the kernel object for pager_request.
                 */

		if (object->pager_request == PAGER_REQUEST_NULL)
                        panic("vm_object_enter_ext: pager request alloc");
#if	!NORMA_VM
                ipc_kobject_set(object->pager_request,
                                (ipc_kobject_t) object,
                                IKOT_PAGING_REQUEST);
#endif	/* !NORMA_VM */
                /*
                 *      Initialize the pager.
                 */
               	object->internal = FALSE;	/* object is external */
               	object->pager_ready = FALSE;	/* not until marked so */

               	/* copy strategy invalid until set by memory manager */
              	object->copy_strategy = MEMORY_OBJECT_COPY_INVALID;

                vm_object_lock(object);
                object->pager_initialized = TRUE;

                if (vm_object_accept_old_init_protocol)
                        object->pager_ready = TRUE;

                vm_object_wakeup(object, VM_OBJECT_EVENT_INITIALIZED);
        } else {
		/*
		 *	If we have a representative, associate it with its
		 *	underlying pager. Memory_object_representative uses 
		 *	this path.
		 */
                vm_object_lock(object);
		if (repo != VM_OBJECT_REP_NULL) {
        		queue_enter(&object->rep_head, repo, vm_object_rep_t, 
				    rep_list);
		}
        }

        /*
         *      [At this point, the object must be locked]
         */

        /*
         *      Wait for the work above to be done by the first
         *      thread to map this object.
         */

        while (!object->pager_initialized) {
                vm_object_wait( object,
                                VM_OBJECT_EVENT_INITIALIZED,
                                FALSE);
                vm_object_lock(object);
        }
        vm_object_unlock(object);

	return(object);
}

/*
 *      Routine:        vm_object_port_check
 *      Purpose:	Setup the representative, initializing if necessary.
 *			Notify pager of new client. Note that the rep may be
 *			the pager port itself.
 */
vm_object_t     
vm_object_port_check(
        ipc_port_t      rep)
{
        vm_object_t     	object;
	vm_object_rep_t		repo;
        ipc_kobject_type_t 	po;
	pager_request_t		request;
	kern_return_t		kr;

	printf("vm_object_port_check\n");

	if (!IP_VALID(rep))
		panic("vm_object_port_check: invalid rep");

	po = ip_kotype(rep);

	/*
	 *	The representative is actually the pager port itself.
	 *	Just enter the vm_object and return. Currently, this 
	 *	is the most common case.
	 */
	if (po == IKOT_PAGER) {
		object = vm_object_enter_ext(rep, VM_OBJECT_REP_NULL);
		return(object);
	}

	/* 
 	 *	Have used this rep before; just dig out the vm_object.
	 */
	if (po == IKOT_PAGER_REP) {
		repo = (vm_object_rep_t) rep->ip_kobject;
                while (!REP_READY(repo)) {
                        vm_object_rep_wait(repo, VM_OBJECT_EVENT_REP_READY);
                }
                assert(repo->constituent != VM_OBJECT_NULL);
		object = repo->constituent;
		return(object);
	}

	/*
	 *	Never heard of this rep before; synchronously consult pager.
	 *	Can't fill in the pager_request's kobject until we get the 
	 *	memory_object from pager (in memory_object_establish). 
	 *
	 *	If it turns out that this is the first mapping of a "repre-
	 *	sentative" which is actually the pager port itself, we will
	 *	discover this in memory_object_establish and recognize it
	 *	here because rep will be converted to kobj type IKOT_PAGER.
	 */
	if (po == IKOT_NONE) {
		repo = vm_object_rep_allocate();
		if (repo == VM_OBJECT_REP_NULL) {
			printf("vm_object_port_check: allocate failed\n");
			return(VM_OBJECT_NULL);
		}

		repo->rep = rep;
        	ipc_kobject_set(rep, (ipc_kobject_t) repo, IKOT_PAGER_REP);

#if	NORMA_VM
		(void) xmm_memory_object_rep_init(repo);
#else	/* NORMA_VM */
                request = ipc_port_alloc_kernel();
                if (request == PAGER_REQUEST_NULL)
                        panic("vm_object_port_check: pager request alloc");

		repo->pager_request = request;
        	ipc_kobject_set(request, IKO_NULL, IKOT_NONE);

		kr = memory_object_notify(rep, request, 
					 ipc_port_make_send(realhost.host_self),
                                         PAGE_SIZE);
		if (kr != KERN_SUCCESS) {
			printf("vm_object_port_check: notify failed %d\n", kr);
			return(VM_OBJECT_NULL);
		}
#endif	/* NORMA_VM */

		/*
		 * 	Wait for the memory manager to reply with a memory
 		 *	object port. 
		 */
		while (!REP_READY(repo)) {
			vm_object_rep_wait(repo, VM_OBJECT_EVENT_REP_READY);
		}

		assert(repo->constituent != VM_OBJECT_NULL);
                object = repo->constituent;

		/*
		 *	Deallocate repo if rep was actually the pager port 
		 *	itself.
		 */
		if (ip_kotype(rep) == IKOT_PAGER) {
			assert(repo->constituent == 
			       (vm_object_t) rep->ip_kobject);
			kfree((vm_offset_t)repo, sizeof(struct vm_object_rep));
		}

		assert(ip_kotype(rep) != IKOT_PAGER_REP || 
 		       ((vm_object_rep_t) rep->ip_kobject) == repo);

                return(object);
	}

	return(VM_OBJECT_NULL);
}
#endif	/* MOR */

#if	NORMA_VM
/*
 * We have made a new region of the object copy-on-write.
 * If the whole object is now copy-on-write, then we tell
 * the svm layer that the object is frozen, passing it
 * a pagemap that can be sent to other kernels to avoid
 * unnecessary data_requests.
 */
void
vm_object_export_pagemap_try(
	vm_object_t	object,
	vm_size_t	size)
{
#if	MACH_PAGEMAP
	vm_page_t m;

	vm_object_lock(object);
	if (object->pagemap_exported) {
		vm_object_unlock(object);
		return;
	}

	/*
	 * XXX
	 * Do I need to synchronize between pager_created
	 * and handle copy-on-write obligations?
	 */

	assert(object->internal);
	assert(object->copy_strategy == MEMORY_OBJECT_COPY_SYMMETRIC);

	if (vm_external_null(&object->existence_info)) {
		vm_object_unlock(object);
		return;
	}
	assert(object->pager_created);

	/*
	 * Check the size.  It may be the case that only part of the object
	 * has been write protected, so there are pages in it that may
	 * not get properly shared.  Once the entire object is present,
	 * go ahead and share it.
	 */
	if (size == object->size)
		object->frozen_size = size;
	else
		object->frozen_size += size;

	if (object->frozen_size != object->size) {
		if (object->frozen_size < object->size) {
			vm_object_unlock(object);
			return;
		}
		/*
		 * XXX Can we do this??  Enough of the frozen
		 * XXX object has been frozen that we may be able
		 * XXX to freeze the whole thing...is this strictly
		 * XXX true?
		 */
		object->frozen_size = object->size;
	}

	/*
	 * Remember that we have given the pager a frozen pagemap.
	 */
	object->pagemap_exported = TRUE;

	/*
	 * Synchronize declaration of vm_object pages with current pagination
	 * work on vm_object pages.
	 */
	vm_object_paging_wait(object, FALSE);
	vm_object_paging_begin(object);
	vm_object_unlock(object);

#if 1
	/*
	 * Declare pages now that the svm layer is going to be interested
	 * in what we do.
	 */
	vm_object_declare_pages(object);
#else

	/*
	 * Merge resident page information into the object's
	 * existence info. This is safe (spurious 1's are okay;
	 * spurious 0's are not); furthermore, it shouldn't
	 * be inefficient, since the kernel won't look at these
	 * 1's as long as the pages are resident, and it will
	 * set the 1's as it makes them not resident anyway
	 * (since all of these pages are probably dirty or precious).
	 *
	 * XXXO
	 * There will often be lots of zeros in the existence info.
	 * We could compress this information with a more
	 * complex representation, if it seems worthwhile.
	 */
	queue_iterate(&object->memq, m, vm_page_t, listq) {
		assert(m->offset < object->size);
		vm_external_state_set(&object->existence_info, m->offset);
	}

	/*
	 * Pass pagemap to pager.
	 */
	memory_object_freeze(object->pager, object->pager_request,
			     object->existence_info.existence_map,
			     object->existence_info.existence_size);
#endif
	vm_object_lock(object);
	vm_object_paging_end(object);
	vm_object_unlock(object);
#endif	/* MACH_PAGEMAP */
}

/*
 *	For use only by vm_object_declare_pages.
 *	If a page is about to be paged in, then
 *	continue on to the next page; otherwise,
 *	mark the page dirty, and allow it to be
 *	declared.
 *
 *	A page handed to this routine cannot have
 *	the absent, error, or restart bits set,
 *	since such bits are only set upon receiving
 *	a reply from the pager, and since the object
 *	is not ready, we have not even sent it any
 *	requests. A page can, however, be fictitious,
 *	since a fault may be in progress, blocked
 *	on the pager_ready object attribute.
 */
#define	SET_DIRTY_OR_SKIP(m)					\
{								\
	assert(! (m)->error && ! (m)->restart);			\
	if ((m)->absent || (m)->fictitious) {			\
		continue;					\
	}							\
	(m)->dirty = TRUE;					\
}

/*
 *	Tell the svm layer about the pages that we have.
 *	The svm layer doesn't know about these pages
 *	because we didn't ask for them because the svm
 *	object for this object didn't exist until we
 *	created the pager and therefore there was no
 *	one to ask. However, the svm layer needs to know
 *	about them now so that it knows what pages to
 *	ask us for if another kernel maps the object.
 *
 *	We will try to give the svm layer a pagemap that
 *	represents this object in a frozen state that
 *	the svm layer can then hand to other kernels,
 *	allowing useless data_requests to be avoided
 *	(those that will always be answered with
 *	data_unavailable). However, if the object is
 *	too large to create a pagemap for, we tell the
 *	kernel about the pages we have, one by one,
 *	avoiding the creation of a pagemap; if the
 *	object is not too large to create a pagemap for,
 *	but it isn't frozen, then we indicate such when
 *	sending the pagemap to the svm layer, so that it
 *	will use it only to see what pages we have,
 *	discarding the pagemap when it is done.
 *	(We can tell that the object is "too large" by
 *	seeing that vm_external_create failed to create
 *	a pagemap for the object.)
 *
 *	Every page here is likely to be dirty or to become dirty.
 *	Mark every page dirty, so that the pager can count on
 *	the kernel to hold a copy. No pages should be being
 *	paged in, since the object is not yet ready.
 *
 *	XXXO
 *	If MACH_PAGEMAP is enabled, we could use pagemaps to
 *	declare chunks of a very large object via multiple
 *	calls to declare_pages.
 */
void
vm_object_declare_pages(
	vm_object_t	object)
{
	vm_page_t m;
	boolean_t frozen;
	boolean_t gotapage;
#if	MACH_PAGEMAP
	struct vm_external e0, *e = &e0;
#endif	/* MACH_PAGEMAP */

	assert(object->pager_created);
	assert(! object->pager_ready || object->size == object->frozen_size);
#if	MACH_PAGEMAP
	if (! vm_external_null(&object->existence_info)) {
		frozen = (object->size == object->frozen_size);
		/*
		 * Create a temporary existence map to collect the
		 * present pages in - doing a declare pages on pages
		 * the pager has will not work.
		 */
		vm_external_create(e, 
			 ptoa(object->existence_info.existence_size << 3));
		/*
		 *	Merge resident page information into the object's
		 *	existence info, as in vm_object_export_pagemap_try.
		 *
		 *	XXXO
		 *	While it is not incorrect to set bits in the pagemap,
		 *	it does make coalescing harder. Should use a temporary
		 *	pagemap instead.
		 */
		gotapage = FALSE;
		vm_object_lock(object);
		queue_iterate(&object->memq, m, vm_page_t, listq) {
			assert(m->offset < object->size);

			/*
			 * If the object is frozen, do not dirty rdonly
			 * pages.
			 */
			if (!(frozen && (m->page_lock & VM_PROT_WRITE)))
				SET_DIRTY_OR_SKIP(m);
			vm_external_state_set(&object->existence_info,
					      m->offset);
			vm_external_state_set(e, m->offset);
			if (!gotapage)
				gotapage = TRUE;
		}
		vm_object_unlock(object);
		
		/*
		 * Pass pagemap to pager.
		 */
		if (gotapage)
			memory_object_declare_pages(object->pager,
						    object->pager_request,
						    e->existence_map,
						    e->existence_size,
						    frozen);
		vm_external_destroy(e);
		return;
	}
#endif	/* MACH_PAGEMAP */

	/*
	 *	Declare each resident page, one by one.
	 */
	queue_iterate(&object->memq, m, vm_page_t, listq) {
		assert(m->offset < object->size);
		SET_DIRTY_OR_SKIP(m);
		memory_object_declare_page(object->pager,
					   object->pager_request,
					   m->offset,
					   PAGE_SIZE);
	}
}

#endif

/*
 *	Routine:	vm_object_pager_create
 *	Purpose:
 *		Create a memory object for an internal object.
 *	In/out conditions:
 *		The object is locked on entry and exit;
 *		it may be unlocked within this call.
 *	Limitations:
 *		Only one thread may be performing a
 *		vm_object_pager_create on an object at
 *		a time.  Presumably, only the pageout
 *		daemon will be using this routine.
 */
void
vm_object_pager_create(
	register vm_object_t	object)
{
	ipc_port_t	pager;

	if (object->pager_created) {
		/*
		 *	Someone else got to it first...
		 *	wait for them to finish initializing
		 */
#if	NORMA_VM
		while (!object->pager_ready) {
			vm_object_wait(	object,
				       VM_OBJECT_EVENT_PAGER_READY,
				       FALSE);
			vm_object_lock(object);
		}
#else	/* NORMA_VM */
		while (!object->pager_initialized) {
			vm_object_wait(	object,
				       VM_OBJECT_EVENT_INITIALIZED,
				       FALSE);
			vm_object_lock(object);
		}
#endif	/* NORMA_VM */
		return;
	}

	/*
	 *	Indicate that a memory object has been assigned
	 *	before dropping the lock, to prevent a race.
	 */

	object->pager_created = TRUE;
		
	/*
	 *	Prevent collapse or termination by
	 *	holding a paging reference
	 */

	vm_object_paging_begin(object);
	vm_object_unlock(object);

#if	MACH_PAGEMAP && !NORMA_VM  
	/*
	 * Don't create any pagemap under NORMA_VM since it will be provided
	 * by svm itself in memory_object_set_attributes_common().
	 */
	assert((object->size + object->paging_offset) >= object->size);
	vm_external_create(&object->existence_info,
			   object->size + object->paging_offset);
#endif	/* MACH_PAGEMAP && !NORMA_VM */

	/*
	 *	Create the pager, and associate with it
	 *	this object.
	 *
	 *	Note that we only make the port association
	 *	so that vm_object_enter can properly look up
	 *	the object to complete the initialization...
	 *	we do not expect any user to ever map this
	 *	object.
	 *
	 *	Since the kernel has the only rights to the
	 *	port, it's safe to install the association
	 *	without holding the cache lock.
	 */

	pager = ipc_port_alloc_kernel();
	if (pager == IP_NULL)
		panic("vm_object_pager_create: allocate pager port");

	(void) ipc_port_make_send(pager);
	ipc_kobject_set(pager, (ipc_kobject_t) object, IKOT_PAGER);

	/*
	 *	Initialize the rest of the paging stuff
	 */

	if (vm_object_enter(pager, object->size, TRUE, TRUE) != object)
		panic("vm_object_pager_create: mismatch");

	/*
	 *	Drop the naked send right taken above.
	 */
	ipc_port_release_send(pager);

#if	NORMA_VM
	/*
	 *	Only after declaring pages do we mark the object ready.
	 *	This avoids the question of how to declare absent pages.
	 *	Also tell NORMA XMM that this is a copy call object if
	 *	it is.
	 */

	vm_object_declare_pages(object);
	if (object->copy_strategy == MEMORY_OBJECT_COPY_CALL)
		memory_object_share(object->pager, object->pager_request);
	vm_object_lock(object);
	assert(! object->pager_ready);
	object->pager_ready = TRUE;
	vm_object_wakeup(object, VM_OBJECT_EVENT_PAGER_READY);
#else	/* NORMA_VM */
	vm_object_lock(object);
#endif	/* NORMA_VM */

	/*
	 *	Release the paging reference
	 */
	vm_object_paging_end(object);
}

/*
 *	Routine:	vm_object_remove
 *	Purpose:
 *		Eliminate the pager/object association
 *		for this pager.
 *	Conditions:
 *		The object cache must be locked.
 */
void
vm_object_remove(
	vm_object_t	object)
{
	ipc_port_t port;

	if ((port = object->pager) != IP_NULL) {
		if (ip_kotype(port) == IKOT_PAGER)
			ipc_kobject_set(port,
#if	NORMA_VM
					port->ip_kobject,
#else	/* NORMA_VM */
					IKO_NULL,
#endif	/* NORMA_VM */
					IKOT_PAGER_TERMINATING);
		 else if (ip_kotype(port) != IKOT_NONE)
			panic("vm_object_remove: bad object port");
	}

#if	!NORMA_VM
	if ((port = object->pager_name) != IP_NULL) {
		if (ip_kotype(port) == IKOT_PAGING_NAME)
			ipc_kobject_set(port, IKO_NULL, IKOT_NONE);
		 else if (ip_kotype(port) != IKOT_NONE)
			panic("vm_object_remove: bad name port");
	}
	if ((port = object->pager_request) != IP_NULL) {
		if (ip_kotype(port) == IKOT_PAGING_REQUEST)
			ipc_kobject_set(port, IKO_NULL, IKOT_NONE);
		 else if (ip_kotype(port) != IKOT_NONE)
			panic("vm_object_remove: bad request port");
	}
#endif	/* !NORMA_VM */

#if	MOR
	/*
	 *	Disassociate all reps from the object. Unfortunately,
	 *	we can't do this for any reps until we are terminating
	 *	the object, when we must do it for all reps. 
	 */
	if (!queue_empty(&object->rep_head)) {
		vm_object_rep_t repo;
		queue_iterate(&object->rep_head, repo,  
			      vm_object_rep_t, rep_list) {
			assert(repo != VM_OBJECT_REP_NULL);
			if ((port = repo->rep) != IP_NULL){
				if (ip_kotype(port) == IKOT_PAGER_REP)
					ipc_kobject_set(port, IKO_NULL, 
							IKOT_NONE);
				else if (ip_kotype(port) != IKOT_NONE)
					panic("vm_object_remove: bad rep port");
			}
		}
	}
#endif	/* MOR */

}

/*
 *	Global variables for vm_object_collapse():
 *
 *		Counts for normal collapses and bypasses.
 *		Debugging variables, to watch or disable collapse.
 */
long	object_collapses = 0;
long	object_bypasses  = 0;

boolean_t	vm_object_collapse_allowed = TRUE;
boolean_t	vm_object_bypass_allowed = TRUE;

/*
 *	vm_object_do_collapse:
 *
 *	Collapse an object with the object backing it.
 *	Pages in the backing object are moved into the
 *	parent, and the backing object is deallocated.
 *
 *	Both objects and the cache are locked; the page
 *	queues are unlocked.
 *
 */
void
vm_object_do_collapse(
	vm_object_t object,
	vm_object_t backing_object)
{
	vm_page_t p, pp;
	vm_offset_t new_offset, backing_offset;
	vm_size_t size;
#if	!NORMA_VM
	ipc_port_t      old_pager_name = IP_NULL;
#endif	/* !NORMA_VM */
#if	MOR
	vm_object_rep_t	repo;
#endif	/* MOR */

	backing_offset = object->shadow_offset;
	size = object->size;


	/*
	 *	Move all in-memory pages from backing_object
	 *	to the parent.  Pages that have been paged out
	 *	will be overwritten by any of the parent's
	 *	pages that shadow them.
	 */
	
	while (!queue_empty(&backing_object->memq)) {
		
		p = (vm_page_t) queue_first(&backing_object->memq);
		
		new_offset = (p->offset - backing_offset);
		
		assert(!p->busy || p->absent);
		
		/*
		 *	If the parent has a page here, or if
		 *	this page falls outside the parent,
		 *	dispose of it.
		 *
		 *	Otherwise, move it as planned.
		 */
		
		if (p->offset < backing_offset || new_offset >= size) {
			vm_page_lock_queues();
			vm_page_free(p);
			vm_page_unlock_queues();
		} else {
			pp = vm_page_lookup(object, new_offset);
			if (pp == VM_PAGE_NULL) {

				/*
				 *	Parent now has no page.
				 *	Move the backing object's page up.
				 */

				vm_page_rename(p, object, new_offset);
#if	MACH_PAGEMAP
			} else if (pp->absent) {

				/*
				 *	Parent has an absent page...
				 *	it's not being paged in, so
				 *	it must really be missing from
				 *	the parent.
				 *
				 *	Throw out the absent page...
				 *	any faults looking for that
				 *	page will restart with the new
				 *	one.
				 */

				vm_page_lock_queues();
				vm_page_free(pp);
				vm_page_unlock_queues();
				vm_page_rename(p, object, new_offset);
#endif	/* MACH_PAGEMAP */
			} else {
				assert(! pp->absent);

				/*
				 *	Parent object has a real page.
				 *	Throw away the backing object's
				 *	page.
				 */
				vm_page_lock_queues();
				vm_page_free(p);
				vm_page_unlock_queues();
			}
		}
	}
	
	assert(object->pager == IP_NULL || backing_object->pager == IP_NULL);

	if (backing_object->pager != IP_NULL) {

		/*
		 *	Move the pager from backing_object to object.
		 *
		 *	XXX We're only using part of the paging space
		 *	for keeps now... we ought to discard the
		 *	unused portion.
		 */

		object->pager = backing_object->pager;
		ipc_kobject_set(object->pager, (ipc_kobject_t) object,
				IKOT_PAGER);
		object->pager_initialized = backing_object->pager_initialized;
		object->pager_ready = backing_object->pager_ready;
		object->pager_created = backing_object->pager_created;
		object->pager_request = backing_object->pager_request;
		object->paging_offset =
		    backing_object->paging_offset + backing_offset;
#if	NORMA_VM
		object->pager_modwc = backing_object->pager_modwc;
		object->completions_pending = 
			backing_object->completions_pending;
		object->pager_name = backing_object->pager_name;
#else	/* NORMA_VM */
		if (object->pager_name != IP_NULL) {
			/* consumes our right for pager_name */
		    ipc_kobject_set(object->pager_name, IKO_NULL, IKOT_NONE);
		    old_pager_name = object->pager_name;
		    object->pager_name = IP_NULL;
		}
		object->pager_name = backing_object->pager_name;
		if (object->pager_name != IP_NULL) {
			ipc_kobject_set(object->pager_name,
					(ipc_kobject_t) object,
					IKOT_PAGING_NAME);
		}
		if (object->pager_request != IP_NULL) {
			ipc_kobject_set(object->pager_request,
					(ipc_kobject_t) object,
					IKOT_PAGING_REQUEST);
		}
#endif	/* NORMA_VM */
#if     MOR
		/* move backing object's rep list to object */
		assert(queue_empty(&object->rep_head));
		queue_new_head(&backing_object->rep_head, &object->rep_head,
			       vm_object_rep_t, rep_list);
		queue_iterate(&object->rep_head, repo, 
			      vm_object_rep_t, rep_list) {
			repo->constituent = object;	
		}
#endif  /* MOR */
#if	MACH_PAGEMAP
		assert(vm_external_null(&object->existence_info));
		object->existence_info = backing_object->existence_info;
		vm_external_nullify(&backing_object->existence_info);
#endif	/* MACH_PAGEMAP */
	} else {
#if	!NORMA_VM
		if (backing_object->pager_name != IP_NULL) {
		    /* consumes our right for pager_name */
		    ipc_kobject_set(backing_object->pager_name,
				    IKO_NULL, IKOT_NONE);
		    old_pager_name = backing_object->pager_name;
		    backing_object->pager_name = IP_NULL;
		}
#endif	/* !NORMA_VM */
	}

	vm_object_cache_unlock();
#if	!NORMA_VM
	if (old_pager_name != IP_NULL) {
		ipc_port_dealloc_kernel(old_pager_name);
	}
#endif	/* !NORMA_VM */


	/*
	 *	Object now shadows whatever backing_object did.
	 *	Note that the reference to backing_object->shadow
	 *	moves from within backing_object to within object.
	 */
	
	object->shadow = backing_object->shadow;
	object->shadow_offset += backing_object->shadow_offset;
	if (object->shadow != VM_OBJECT_NULL &&
	    object->shadow->copy != VM_OBJECT_NULL) {
		panic("vm_object_collapse: we collapsed a copy-object!");
	}

	/*
	 *	Discard backing_object.
	 *
	 *	Since the backing object has no pages, no
	 *	pager left, and no object references within it,
	 *	all that is necessary is to dispose of it.
	 */
	
#if	NORMA_VM
	assert(ip_kotype(backing_object->object_name) == IKOT_PAGING_NAME);
	ipc_kobject_set(backing_object->object_name, IKO_NULL, IKOT_NONE);
	ipc_port_dealloc_kernel(backing_object->object_name);
	assert(backing_object->completions_pending == 0);
#endif	/* NORMA_VM */

	assert((backing_object->ref_count == 1) &&
	       (backing_object->resident_page_count == 0) &&
	       (backing_object->paging_in_progress == 0));
#if	MACH_PAGEMAP
	assert(vm_external_null(&backing_object->existence_info));
#endif	/* MACH_PAGEMAP */

	assert(backing_object->alive);
	backing_object->alive = FALSE;
	vm_object_unlock(backing_object);
	
#if	VM_OBJECT_TRACKING
	vm_object_track_dealloc(backing_object);
#endif
	zfree(vm_object_zone, (vm_offset_t) backing_object);
	
	object_collapses++;
}

void
vm_object_do_bypass(
	vm_object_t object,
	vm_object_t backing_object)
{
	/*
	 *	Make the parent shadow the next object
	 *	in the chain.
	 */
	
	object->shadow = backing_object->shadow;
	vm_object_reference(object->shadow);
	object->shadow_offset += backing_object->shadow_offset;
	
	/*
	 *	Backing object might have had a copy pointer
	 *	to us.  If it did, clear it. 
	 */
	if (backing_object->copy == object) {
		backing_object->copy = VM_OBJECT_NULL;
	}
	
	/*
	 *	Drop the reference count on backing_object.
	 *	Don't call vm_object_deallocate unless
	 *	ref_count drops to zero.
	 *
	 *	The ref_count can drop to zero here if the
	 *	backing object could be bypassed but not
	 *	collapsed, such as when the backing object
	 *	is temporary and cachable.
	 */
	if (backing_object->ref_count > 1) {
		backing_object->ref_count--;
		vm_object_unlock(backing_object);
	} else {

		/*
		 *	Drop locks so that we can deallocate
		 *	the backing object.
		 */

		vm_object_unlock(object);
		vm_object_unlock(backing_object);
		vm_object_deallocate(backing_object);

		/*
		 *	Relock object. We don't have to reverify
		 *	its state since vm_object_collapse will
		 *	do that for us as it starts at the
		 *	top of its loop.
		 */

		vm_object_lock(object);
	}
	
	object_bypasses++;
}
		
/*
 *	vm_object_collapse:
 *
 *	Perform an object collapse or an object bypass if appropriate.
 *	The real work of collapsing and bypassing is performed in
 *	the routines vm_object_do_collapse and vm_object_do_bypass.
 *
 *	Requires that the object be locked and the page queues be unlocked.
 *
 */
void
vm_object_collapse(
	register vm_object_t	object)
{
	register vm_object_t	backing_object;
	register vm_offset_t	backing_offset;
	register vm_size_t	size;
	register vm_offset_t	new_offset;
	register vm_page_t	p;

	if (! vm_object_collapse_allowed && ! vm_object_bypass_allowed) {
		return;
	}

	while (TRUE) {
		/*
		 *	Verify that the conditions are right for either
		 *	collapse or bypass:
		 *
		 *	The object exists and no pages in it are currently
		 *	being paged out, and
		 */
		if (object == VM_OBJECT_NULL ||
		    object->paging_in_progress != 0 ||
		    object->absent_count != 0)
			return;

		/*
		 *	There is a backing object, and
		 */
	
		if ((backing_object = object->shadow) == VM_OBJECT_NULL)
			return;
	
		vm_object_lock(backing_object);

		/*
		 *	...
		 *		The backing object is not read_only,
		 *		and no pages in the backing object are
		 *		currently being paged out.
		 *		The backing object is internal.
		 *
		 *		XXX norma_vm: internal?
		 *
		 */
	
		if (!backing_object->internal ||
		    backing_object->paging_in_progress != 0) {
			vm_object_unlock(backing_object);
			return;
		}
	
		/*
		 *	The backing object can't be a copy-object:
		 *	the shadow_offset for the copy-object must stay
		 *	as 0.  Furthermore (for the 'we have all the
		 *	pages' case), if we bypass backing_object and
		 *	just shadow the next object in the chain, old
		 *	pages from that object would then have to be copied
		 *	BOTH into the (former) backing_object and into the
		 *	parent object.
		 */
		if (backing_object->shadow != VM_OBJECT_NULL &&
		    backing_object->shadow->copy != VM_OBJECT_NULL) {
			vm_object_unlock(backing_object);
			return;
		}

		/*
		 *	We can now try to either collapse the backing
		 *	object (if the parent is the only reference to
		 *	it) or (perhaps) remove the parent's reference
		 *	to it.
		 */

		backing_offset = object->shadow_offset;
		size = object->size;

		/*
		 *	If there is exactly one reference to the backing
		 *	object, we may be able to collapse it into the parent.
		 *
		 *	XXXO (norma vm):
		 *
		 *	The backing object must not have a pager
		 *	created for it, since collapsing an object
		 *	into a backing_object dumps new pages into
		 *	the backing_object that its pager doesn't
		 *	know about, and we've already declared pages.
		 *	This page dumping is deadly if other kernels
		 *	are shadowing this object; this is the
		 *	distributed equivalent of the ref_count == 1
		 *	condition.
		 *
		 *	With some work, we could downgrade this
		 *	restriction to the backing object must not
		 *	be cachable, since when a temporary object
		 *	is uncachable we are allowed to do anything
		 *	to it. We would have to do something like
		 *	call declare_pages again, and we would have
		 *	to be prepared for the memory manager
		 *	disabling temporary termination, which right
		 *	now is a difficult race to deal with, since
		 *	the memory manager currently assumes that
		 *	termination is the only possible failure
		 *	for disabling temporary termination.
		 */

		if (backing_object->ref_count == 1 &&
		    backing_object->temporary &&
		    ! object->pager_created &&
#if	NORMA_VM
		    ! backing_object->pager_created &&
		    backing_object->frozen_size == 0 &&
#endif	/* NORMA_VM */
		    vm_object_collapse_allowed) {

			/*
			 *	We need the cache lock for collapsing,
			 *	but we must not deadlock.
			 */
			
			if (! vm_object_cache_lock_try()) {
				vm_object_unlock(backing_object);
				return;
			}

			/*
			 *	Collapse the object with its backing
			 *	object, and try again with the object's
			 *	new backing object.
			 */

			vm_object_do_collapse(object, backing_object);
			continue;
		}


		/*
		 *	Collapsing the backing object was not possible
		 *	or permitted, so let's try bypassing it.
		 */

		if (! vm_object_bypass_allowed) {
			vm_object_unlock(backing_object);
			return;
		}

		/*
		 *	If the backing object has a pager but no pagemap,
		 *	then we cannot bypass it, because we don't know
		 *	what pages it has.
		 */
		if (backing_object->pager_created
#if	MACH_PAGEMAP
		    && vm_external_null(&backing_object->existence_info)
#endif	/* MACH_PAGEMAP */
		    ) {
			vm_object_unlock(backing_object);
			return;
		}

		/*
		 *	If all of the pages in the backing object are
		 *	shadowed by the parent object, the parent
		 *	object no longer has to shadow the backing
		 *	object; it can shadow the next one in the
		 *	chain.
		 *
		 *	If the backing object has existence info,
		 *	we must check examine its existence info
		 *	as well.
		 *
		 *	XXX
		 *	Should have a check for a 'small' number
		 *	of pages here.
		 */

		/*
		 *	First, check pages resident in the backing object.
		 */

		queue_iterate(&backing_object->memq, p, vm_page_t, listq) {
			
			/*
			 *	If the parent has a page here, or if
			 *	this page falls outside the parent,
			 *	keep going.
			 *
			 *	Otherwise, the backing_object must be
			 *	left in the chain.
			 */
			
			new_offset = (p->offset - backing_offset);
			if (p->offset < backing_offset || new_offset >= size) {

				/*
				 *	Page falls outside of parent.
				 *	Keep going.
				 */

				continue;
			}

			if ((vm_page_lookup(object, new_offset) ==
			     VM_PAGE_NULL)
#if	MACH_PAGEMAP
			    &&
			    (vm_external_state_get(&object->existence_info,
						   new_offset) !=
			     VM_EXTERNAL_STATE_EXISTS)
#endif	/* MACH_PAGEMAP */
			    ) {

				/*
				 *	Page still needed.
				 *	Can't go any further.
				 */

				vm_object_unlock(backing_object);
				return;
			}
		}

#if	MACH_PAGEMAP
		/*
		 *	Next, if backing object has been paged out,
		 *	we must check its existence info for pages
		 *	that the parent doesn't have.
		 */

		if (backing_object->pager_created) {
			assert(! vm_external_null(&backing_object->
						  existence_info));
			for (new_offset = 0; new_offset < object->size;
			     new_offset += PAGE_SIZE) {
				vm_offset_t
				offset = new_offset + backing_offset;

				/*
				 *	If this page doesn't exist in
				 *	the backing object's existence
				 *	info, then continue.
				 */

				if (vm_external_state_get(&backing_object->
							  existence_info,
							  offset) ==
				    VM_EXTERNAL_STATE_ABSENT) {
					continue;
				}

				/*
				 *	If this page is neither resident
				 *	in the parent nor paged out to
				 *	the parent's pager, then we cannot
				 *	bypass the backing object.
				 */

				if ((vm_page_lookup(object, new_offset) ==
				     VM_PAGE_NULL) &&
				    (vm_external_null(&object->
						      existence_info) ||
				     (vm_external_state_get(&object->
							    existence_info,
							    new_offset) ==
				      VM_EXTERNAL_STATE_ABSENT))) {
					vm_object_unlock(backing_object);
					return;
				}
			}
		}
#else	/* MACH_PAGEMAP */
		assert(! backing_object->pager_created);
#endif	/* MACH_PAGEMAP */

		/*
		 *	All interesting pages in the backing object
		 *	already live in the parent or its pager.
		 *	Thus we can bypass the backing object.
		 */

		vm_object_do_bypass(object, backing_object);

		/*
		 *	Try again with this object's new backing object.
		 */

		continue;
	}
}

/*
 *	Routine:	vm_object_page_remove: [internal]
 *	Purpose:
 *		Removes all physical pages in the specified
 *		object range from the object's list of pages.
 *
 *	In/out conditions:
 *		The object must be locked.
 */
unsigned int vm_object_page_remove_lookup = 0;
unsigned int vm_object_page_remove_iterate = 0;

void
vm_object_page_remove(
	register vm_object_t	object,
	register vm_offset_t	start,
	register vm_offset_t	end)
{
	register vm_page_t	p, next;

	/*
	 *	One and two page removals are most popular.
	 *	The factor of 16 here is somewhat arbitrary.
	 *	It balances vm_object_lookup vs iteration.
	 */

	if (atop(end - start) < (unsigned)object->resident_page_count/16) {
		vm_object_page_remove_lookup++;

		for (; start < end; start += PAGE_SIZE) {
			p = vm_page_lookup(object, start);
			if (p != VM_PAGE_NULL) {
				if (!p->fictitious)
					pmap_page_protect(p->phys_addr,
							  VM_PROT_NONE);
				vm_page_lock_queues();
				vm_page_free(p);
				vm_page_unlock_queues();
			}
		}
	} else {
		vm_object_page_remove_iterate++;

		p = (vm_page_t) queue_first(&object->memq);
		while (!queue_end(&object->memq, (queue_entry_t) p)) {
			next = (vm_page_t) queue_next(&p->listq);
			if ((start <= p->offset) && (p->offset < end)) {
				if (!p->fictitious)
				    pmap_page_protect(p->phys_addr,
						      VM_PROT_NONE);
				vm_page_lock_queues();
				vm_page_free(p);
				vm_page_unlock_queues();
			}
			p = next;
		}
	}
}

/*
 *	Routine:	vm_object_coalesce
 *	Function:	Coalesces two objects backing up adjoining
 *			regions of memory into a single object.
 *
 *	returns TRUE if objects were combined.
 *
 *	NOTE:	Only works at the moment if the second object is NULL -
 *		if it's not, which object do we lock first?
 *
 *	Parameters:
 *		prev_object	First object to coalesce
 *		prev_offset	Offset into prev_object
 *		next_object	Second object into coalesce
 *		next_offset	Offset into next_object
 *
 *		prev_size	Size of reference to prev_object
 *		next_size	Size of reference to next_object
 *
 *	Conditions:
 *	The object must *not* be locked.
 */

boolean_t
vm_object_coalesce(
	register vm_object_t	prev_object,
	vm_object_t		next_object,
	vm_offset_t		prev_offset,
	vm_offset_t		next_offset,
	vm_size_t		prev_size,
	vm_size_t		next_size)
{
	vm_size_t	newsize;

#ifdef	lint
	next_offset++;
#endif	/* lint */

	if (next_object != VM_OBJECT_NULL) {
		return(FALSE);
	}

	if (prev_object == VM_OBJECT_NULL) {
		return(TRUE);
	}

	vm_object_lock(prev_object);

	/*
	 *	Try to collapse the object first
	 */
	vm_object_collapse(prev_object);

	/*
	 *	Can't coalesce if pages not mapped to
	 *	prev_entry may be in use anyway:
	 *	. more than one reference
	 *	. paged out
	 *	. shadows another object
	 *	. has a copy elsewhere
	 *	. paging references (pages might be in page-list)
	 */

	if ((prev_object->ref_count > 1) ||
	    prev_object->pager_created ||
	    (prev_object->shadow != VM_OBJECT_NULL) ||
	    (prev_object->copy != VM_OBJECT_NULL) ||
	    (prev_object->paging_in_progress != 0)) {
		vm_object_unlock(prev_object);
		return(FALSE);
	}

	/*
	 *	Remove any pages that may still be in the object from
	 *	a previous deallocation.
	 */

	vm_object_page_remove(prev_object,
			prev_offset + prev_size,
			prev_offset + prev_size + next_size);

	/*
	 *	Extend the object if necessary.
	 */
	newsize = prev_offset + prev_size + next_size;
	if (newsize > prev_object->size) {
#if	MACH_PAGEMAP
		/*
		 *	We cannot extend an object that has existence info,
		 *	since the existence info might then fail to cover
		 *	the entire object.
		 *
		 *	This assertion must be true because the object
		 *	has no pager, and we only create existence info
		 *	for objects with pagers.
		 */
		assert(vm_external_null(&prev_object->existence_info));
#endif	/* MACH_PAGEMAP */
		prev_object->size = newsize;
	}

	vm_object_unlock(prev_object);
	return(TRUE);
}

/*
 *	Routine:	vm_object_name
 *	Purpose:
 *		Returns a naked send right to the "name" port associated
 *		with this object.
 */
ipc_port_t
vm_object_name(
	vm_object_t	object)
{
	ipc_port_t	p;

	if (object == VM_OBJECT_NULL)
		return(IP_NULL);

	vm_object_lock(object);

	while (object->shadow != VM_OBJECT_NULL) {
		vm_object_t	new_object = object->shadow;
		vm_object_lock(new_object);
		vm_object_unlock(object);
		object = new_object;
	}

#if	NORMA_VM
	p = object->object_name;
#else	/* NORMA_VM */
	p = object->pager_name;
#endif	/* NORMA_VM */
	if (p != IP_NULL)
		p = ipc_port_make_send(p);
	vm_object_unlock(object);

	return p;
}

/*
 *	Attach a set of physical pages to an object, so that they can
 *	be mapped by mapping the object.  Typically used to map IO memory.
 *
 *	The mapping function and its private data are used to obtain the
 *	physical addresses for each page to be mapped.
 */
void
vm_object_page_map_internal(
	vm_object_t	object,
	vm_offset_t	offset,
	vm_size_t	size,
	vm_offset_t	(*map_fn)(void *map_fn_data, vm_offset_t offset),
	void 		*map_fn_data,	/* private to map_fn */
     	boolean_t	wire,
	boolean_t	private)
{
	int	num_pages;
	int	i;
	vm_page_t	m;
	vm_page_t	old_page;
	vm_offset_t	addr;

	num_pages = atop(size);

	for (i = 0; i < num_pages; i++, offset += PAGE_SIZE) {

	    addr = (*map_fn)(map_fn_data, offset);

	    while ((m = vm_page_grab_fictitious()) == VM_PAGE_NULL)
		vm_page_more_fictitious();

	    vm_object_lock(object);
	    if ((old_page = vm_page_lookup(object, offset))
			!= VM_PAGE_NULL)
	    {
		vm_page_lock_queues();
		vm_page_free(old_page);
		vm_page_unlock_queues();
	    }

	    vm_page_init(m, addr);
	    m->private = private;		/* don`t free page */
	    m->wire_count = (wire ? 1: 0);
	    vm_page_lock_queues();
	    if (!wire)
		vm_page_wire_count--;
	    vm_page_insert(m, object, offset);
	    vm_page_unlock_queues();

	    PAGE_WAKEUP_DONE(m);
	    vm_object_unlock(object);
	}
}

#if	NORMA_VM
void
vm_object_reaper_continue(void)
{
	vm_object_t object;

	for (;;) {
		spinlock_lock(&vor_lock);
		while (!queue_empty(&vor_list)) {
			queue_remove_first(&vor_list, object,
					   vm_object_t, cached_list);
			spinlock_unlock(&vor_lock);
			vm_object_deallocate(object);
			spinlock_lock(&vor_lock);
		}
		assert_wait((int)&vor_list, FALSE);
		spinlock_unlock(&vor_lock);
		thread_block(vm_object_reaper_continue);
	}
}

void vm_object_thread()
{
	vm_object_reaper_continue();
}
#endif	/* NORMA_VM */

#include <mach_kdb.h>

#if	MACH_KDB

#if	NORMA_VM
#include <ddb/db_command.h>
#endif	/* NORMA_VM */
#include <ddb/db_output.h>
#include <vm/vm_print.h>

#define printf	kdbprintf

extern boolean_t	vm_object_cached(
				vm_object_t object);

extern void		print_bitstring(
				char byte);

boolean_t	vm_object_print_pages = FALSE;

void
print_bitstring(
	char byte)
{
	printf("%c%c%c%c%c%c%c%c",
	       ((byte & (1 << 0)) ? '1' : '0'),
	       ((byte & (1 << 1)) ? '1' : '0'),
	       ((byte & (1 << 2)) ? '1' : '0'),
	       ((byte & (1 << 3)) ? '1' : '0'),
	       ((byte & (1 << 4)) ? '1' : '0'),
	       ((byte & (1 << 5)) ? '1' : '0'),
	       ((byte & (1 << 6)) ? '1' : '0'),
	       ((byte & (1 << 7)) ? '1' : '0'));
}

boolean_t
vm_object_cached(
	register vm_object_t object)
{
	register vm_object_t o;

	queue_iterate(&vm_object_cached_list, o, vm_object_t, cached_list) {
		if (object == o) {
			return TRUE;
		}
	}
	return FALSE;
}

#if	MACH_PAGEMAP
/*
 *	vm_external_print:	[ debug ]
 */
void
vm_external_print(
	vm_external_t e)
{
	if (vm_external_null(e)) {
		printf("0");
	} else {
		printf("{ size=%d, map=[",
		       e->existence_size);
		if (e->existence_size > 0) {
			print_bitstring(e->existence_map[0]);
		}
		if (e->existence_size > 1) {
			printf("...");
			print_bitstring(e->existence_map[e->existence_size-1]);
		}
		printf("] }\n");
	}
	return;
}
#endif	/* MACH_PAGEMAP */

int
vm_follow_object(
	vm_object_t object)
{
	extern indent;

	int count = 1;

	if (object == VM_OBJECT_NULL)
		return 0;

	iprintf("object 0x%x", object);
	printf(", shadow=0x%x", object->shadow);
	printf(", copy=0x%x", object->copy);
	printf(", pager=0x%x", object->pager);
	printf(", ref=%d\n", object->ref_count);

	indent += 2;
	if (object->shadow)
	    count += vm_follow_object(object->shadow);

	indent -= 2;
	return count;
}

/*
 *	vm_object_print:	[ debug ]
 */
void
vm_object_print(
	vm_object_t	object,
	boolean_t	have_addr,
	int		arg_count,
	char		*modif)
{
	register vm_page_t p;
	extern indent;
	char *s;
#if	NORMA_VM
	boolean_t show_xmm_stack;
#endif	/* NORMA_VM */

	register int count;

	if (object == VM_OBJECT_NULL)
		return;

#if	NORMA_VM
	if (db_option(modif, 'x'))
		show_xmm_stack = TRUE;
	else
		show_xmm_stack = FALSE;
#endif	/* NORMA_VM */

	iprintf("object 0x%x\n", object);

	indent += 2;

	iprintf("size=0x%x", object->size);
	printf(", frozen=0x%x", object->frozen_size);
	printf(", ref_count=%d", object->ref_count);
	printf(", resident_page_count=%d\n", object->resident_page_count);

	iprintf("shadow=0x%x", object->shadow);
	if (object->shadow) {
		register i = 0;
		vm_object_t shadow = object;
		while(shadow = shadow->shadow)
			i++;
		printf(" (depth %d)", i);
	}
	printf(", copy=0x%x", object->copy);
	printf(", shadow_offset=0x%x", object->shadow_offset);
	printf(", last_alloc=0x%x\n", object->last_alloc);

	iprintf("pager=0x%x", object->pager);
	printf(", paging_offset=0x%x", object->paging_offset);
	printf(", pager_request=0x%x\n", object->pager_request);

	iprintf("pager_name=0x%x", object->pager_name);
	printf(", copy_strategy=%d[", object->copy_strategy);
	switch (object->copy_strategy) {
		case MEMORY_OBJECT_COPY_NONE:
		printf("copy_none");
		break;

		case MEMORY_OBJECT_COPY_CALL:
		printf("copy_call");
		break;

		case MEMORY_OBJECT_COPY_DELAY:
		printf("copy_delay");
		break;

		case MEMORY_OBJECT_COPY_SYMMETRIC:
		printf("copy_symmetric");
		break;

		case MEMORY_OBJECT_COPY_INVALID:
		printf("copy_invalid");
		break;

		default:
		printf("?");
	}
	printf("]");
	printf(", absent_count=%d\n", object->absent_count);

	iprintf("all_wanted=0x%x<", object->all_wanted);
	s = "";
	if (vm_object_wanted(object, VM_OBJECT_EVENT_INITIALIZED)) {
		printf("%sinit", s);
		s = ",";
	}
	if (vm_object_wanted(object, VM_OBJECT_EVENT_PAGER_READY)) {
		printf("%sready", s);
		s = ",";
	}
	if (vm_object_wanted(object, VM_OBJECT_EVENT_PAGING_IN_PROGRESS)) {
		printf("%spaging", s);
		s = ",";
	}
	if (vm_object_wanted(object, VM_OBJECT_EVENT_ABSENT_COUNT)) {
		printf("%sabsent", s);
		s = ",";
	}
	if (vm_object_wanted(object, VM_OBJECT_EVENT_LOCK_IN_PROGRESS)) {
		printf("%slock", s);
		s = ",";
	}
	if (vm_object_wanted(object, VM_OBJECT_EVENT_UNCACHING)) {
		printf("%suncaching", s);
		s = ",";
	}
	if (vm_object_wanted(object, VM_OBJECT_EVENT_COPY_CALL)) {
		printf("%scopy_call", s);
		s = ",";
	}
	if (vm_object_wanted(object, VM_OBJECT_EVENT_COMPLETION)) {
		printf("%scompletion", s);
		s = ",";
	}
	if (vm_object_wanted(object, VM_OBJECT_EVENT_CACHING)) {
		printf("%scaching", s);
		s = ",";
	}
	printf(">");
	printf(", paging_in_progress=%d\n", object->paging_in_progress);

	iprintf("%screated, %sinit, %sready, %spersist, %s, %s, %salive\n",
		(object->pager_created ? "" : "!"),
		(object->pager_initialized ? "" : "!"),
		(object->pager_ready ? "" : "!"),
		(object->can_persist ? "" : "!"),
		(object->internal ? "internal" : "external"),
		(object->temporary ? "temporary" : "permanent"),
		(object->alive ? "" : "!"));
	iprintf("%slock_in_progress, %slock_restart, %sshadowed, %scached, ",
		(object->lock_in_progress ? "" : "!"),
		(object->lock_restart ? "" : "!"),
		(object->shadowed ? "" : "!"),
		(vm_object_cached(object) ? "" : "!"));
	printf("%sprivate\n",
		(object->private ? "" : "!"));

#if	MACH_PAGEMAP
	iprintf("existence_info=");
	vm_external_print(&object->existence_info);
#endif	/* MACH_PAGEMAP */

#if	NORMA_VM
	iprintf("%spagemap_exported, %stemporary_cached\n",
		(object->pagemap_exported ? "" : "!"),
		(object->temporary_cached ? "" : "!"));
	iprintf("%stemporary_caching, %stemporary_uncaching, ",
		(object->temporary_caching ? "" : "!"),
		(object->temporary_uncaching ? "" : "!"));
	printf("object_name=0x%x\n", object->object_name);
	iprintf("%swrite_completions completions_pending %d",
		(object->pager_modwc ? "" : "!"),
		object->completions_pending);
#endif	/* NORMA_VM */
#if	MACH_ASSERT
	printf(", paging_object=0x%x", object->paging_object);
#endif	/* MACH_ASSERT */
	printf("\n");

	if (vm_object_print_pages) {
		count = 0;
		p = (vm_page_t) queue_first(&object->memq);
		while (!queue_end(&object->memq, (queue_entry_t) p)) {
			if (count == 0) {
				iprintf("memory:=");
			} else if (count == 4) {
				printf("\n");
				iprintf(" ...");
				count = 0;
			} else {
				printf(",");
			}
			count++;

			printf("(off=0x%X,page=0x%X)", p->offset, (int) p);
			p = (vm_page_t) queue_next(&p->listq);
		}
		if (count != 0) {
			printf("\n");
		}
	}
	indent -= 2;
#if	NORMA_VM
	if (show_xmm_stack && object->pager_request)
		xmm_object_stack_print(object->pager_request);
#endif	/* NORMA_VM */
}

#endif	/* MACH_KDB */
