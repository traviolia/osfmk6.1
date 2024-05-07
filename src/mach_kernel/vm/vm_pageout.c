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
 * Revision 2.21.5.3  92/09/15  17:37:26  jeffreyh
 * 	Add a few more pages to the reserved pool.
 * 	[92/08/28            jeffreyh]
 * 
 * 	Discard error pages if encountered (just like absent).
 * 	[92/08/21            dlb]
 * 
 * 	With Jeffreyh:  Surgery on the suspend/resume protocol demands
 * 	that we eliminate older wakeups of the kserver_pageout_support
 * 	thread.
 * 	[92/08/20            alanl]
 * 
 * Revision 2.21.5.2  92/06/24  18:04:11  jeffreyh
 * 	Wakeup the pagout_support_thread when there is enough memory
 * 	available to continue receiving kmsgs without filtering.
 * 	[92/06/10            jeffreyh]
 * 
 * 	Added netipc_able_continue_mgs() under NORMA_VM conditional. It
 * 	 is used to judge if a suspended norma message can be restarted.
 * 	[92/06/03            jeffreyh]
 * 
 * Revision 2.21.5.1  92/05/29  11:49:15  jeffreyh
 * 	NORMA only:  drastically increase the size
 * 	of the reserved VM pool to account for
 * 	deficiencies in the NORMA pageout path.
 * 	[92/05/07            alanl]
 * 
 * Revision 2.21  91/12/11  08:44:16  jsb
 * 	Added vm_pageout_active, vm_pageout_inactive,
 * 	and other measurement counters.  Fixed the log.
 * 	[91/11/24            rpd]
 * 
 * Revision 2.20  91/10/09  16:20:36  af
 * 	Added vm_pageout_pause_count, vm_pageout_pause_max technology
 * 	so that vm_pageout_burst_wait can decrease as well as increase.
 * 	[91/10/04            rpd]
 * 
 * Revision 2.19  91/08/28  11:18:54  jsb
 * 	Fixed vm_pageout_scan to send pages to the default pager
 * 	when memory gets very tight.  This is the same idea as the old
 * 	vm_pageout_reserved_external and vm_pageout_reserved_internal,
 * 	but with a different implementation that forcibly double-pages.
 * 	[91/08/07            rpd]
 * 	Precious page support: return precious pages on pageout, use
 * 	memory_object_data_return instead of memory_object_data_write
 * 	when appropriate,
 * 	[91/07/03  14:20:57  dlb]
 * 
 * Revision 2.18  91/07/01  08:28:13  jsb
 * 	Add missing includes of vm/vm_map.h and kern/thread.h.
 * 	[91/06/29  16:53:36  jsb]
 * 
 * Revision 2.17  91/06/17  15:49:37  jsb
 * 	NORMA_VM: declare definitions for memory_object_data_{initialize,write}
 * 	since they won't be provided by interposed-on memory_object_user.h.
 * 	[91/06/17  11:13:22  jsb]
 * 
 * Revision 2.16  91/05/18  14:41:49  rpd
 * 	Fixed vm_pageout_continue to always call vm_pageout_scan.
 * 	Revamped vm_pageout_scan.  Now it recalculates vm_page_inactive_target,
 * 	gradually moves pages from the active list to the inactive list,
 * 	looks at vm_page_free_wanted, handles absent and fictitious pages,
 * 	and blocks with a continuation (vm_pageout_scan_continue), which
 * 	uses vm_page_laundry_count to adjust vm_pageout_burst_wait.
 * 	[91/04/06            rpd]
 * 
 * 	Changed vm_page_free_wanted to unsigned int.
 * 	[91/04/05            rpd]
 * 	Added vm_page_grab_fictitious.
 * 	[91/03/29            rpd]
 * 	Changed vm_page_init.
 * 	[91/03/24            rpd]
 * 
 * Revision 2.15  91/05/14  17:50:59  mrt
 * 	Correcting copyright
 * 
 * Revision 2.14  91/03/16  15:06:50  rpd
 * 	Modified vm_pageout_scan for further vm_page_deactivate changes.
 * 	Also changed it to ignore pages in dead objects.
 * 	[91/03/11            rpd]
 * 	Added vm_pageout_continue.
 * 	[91/01/20            rpd]
 * 
 * Revision 2.13  91/02/05  17:59:57  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:34:17  mrt]
 * 
 * Revision 2.12  91/01/08  16:45:57  rpd
 * 	Added net_kmsg_collect.
 * 	[91/01/05            rpd]
 * 	Added consider_task_collect, consider_thread_collect.
 * 	[91/01/03            rpd]
 * 
 * 	Added stack_collect.
 * 	[90/12/31            rpd]
 * 	Added continuation argument to thread_block.
 * 	[90/12/08            rpd]
 * 
 * 	Ensure that vm_page_free_target is at least five pages
 * 	larger than vm_page_free_min, to avoid vm_page_wait deadlock.
 * 	[90/11/19            rpd]
 * 
 * 	Replaced swapout_threads with consider_zone_gc.
 * 	[90/11/11            rpd]
 * 
 * Revision 2.11  90/11/05  14:35:03  rpd
 * 	Modified vm_pageout_scan for new vm_page_deactivate protocol.
 * 	[90/11/04            rpd]
 * 
 * Revision 2.10  90/10/12  13:06:53  rpd
 * 	Fixed vm_pageout_page to take busy pages.
 * 	[90/10/09            rpd]
 * 
 * 	In vm_pageout_scan, check for new software reference bit
 * 	in addition to using pmap_is_referenced.  Remove busy pages
 * 	found on the active and inactive queues.
 * 	[90/10/08            rpd]
 * 
 * Revision 2.9  90/08/27  22:16:02  dbg
 * 	Fix error in initial assumptions: vm_pageout_setup must take a
 * 	BUSY page, to prevent the page from being scrambled by pagein.
 * 	[90/07/26            dbg]
 * 
 * Revision 2.8  90/06/19  23:03:22  rpd
 * 	Locking fix for vm_pageout_page from dlb/dbg.
 * 	[90/06/11            rpd]
 * 
 * 	Correct initial condition in vm_pageout_page (page is NOT busy).
 * 	Fix documentation for vm_pageout_page and vm_pageout_setup.
 * 	[90/06/05            dbg]
 * 
 * 	Fixed vm_object_unlock type in vm_pageout_page.
 * 	[90/06/04            rpd]
 * 
 * Revision 2.7  90/06/02  15:11:56  rpd
 * 	Removed pageout_task and references to kernel_vm_space.
 * 	[90/04/29            rpd]
 * 
 * 	Made vm_pageout_burst_max, vm_pageout_burst_wait tunable.
 * 	[90/04/18            rpd]
 * 	Converted to new IPC and vm_map_copyin_object.
 * 	[90/03/26  23:18:10  rpd]
 * 
 * Revision 2.6  90/05/29  18:39:52  rwd
 * 	Picked up new vm_pageout_page from dbg.
 * 	[90/05/17            rwd]
 * 	Rfr change to send multiple pages to pager at once.
 * 	[90/04/12  13:49:13  rwd]
 * 
 * Revision 2.5  90/05/03  15:53:21  dbg
 * 	vm_pageout_page flushes page only if asked; otherwise, it copies
 * 	the page.
 * 	[90/03/28            dbg]
 * 
 * 	If an object's pager is not initialized, don't page out to it.
 * 	[90/03/21            dbg]
 * 
 * Revision 2.4  90/02/22  20:06:48  dbg
 * 		PAGE_WAKEUP --> PAGE_WAKEUP_DONE to reflect the fact that it
 * 		clears the busy flag.
 * 		[89/12/13            dlb]
 * 
 * Revision 2.3  90/01/11  11:48:27  dbg
 * 	Pick up recent changes from mainline:
 * 
 * 		Eliminate page lock when writing back a page.
 * 		[89/11/09            mwyoung]
 * 
 * 		Account for paging_offset when setting external page state.
 * 		[89/10/16  15:29:08  af]
 * 
 * 		Improve reserve tuning... it was a little too low.
 * 
 * 		Remove laundry count computations, as the count is never used.
 * 		[89/10/10            mwyoung]
 * 
 * 		Only attempt to collapse if a memory object has not
 * 		been initialized.  Don't bother to PAGE_WAKEUP in
 * 		vm_pageout_scan() before writing back a page -- it
 * 		gets done in vm_pageout_page().
 * 		[89/10/10            mwyoung]
 * 
 * 		Don't reactivate a page when creating a new memory
 * 		object... continue on to page it out immediately.
 * 		[89/09/20            mwyoung]
 * 
 * 		Reverse the sensing of the desperately-short-on-pages tests.
 * 		[89/09/19            mwyoung]
 * 		Check for absent pages before busy pages in vm_pageout_page().
 * 		[89/07/10  00:01:22  mwyoung]
 * 
 * 		Allow dirty pages to be reactivated.
 * 		[89/04/22            mwyoung]
 * 
 * 		Don't clean pages that are absent, in error, or not dirty in
 * 		vm_pageout_page().  These checks were previously issued
 * 		elsewhere.
 * 		[89/04/22            mwyoung]
 * 
 * Revision 2.2  89/09/08  11:28:55  dbg
 * 	Reverse test for internal_only pages.  Old sense left pageout
 * 	daemon spinning.
 * 	[89/08/15            dbg]
 * 
 * 18-Jul-89  David Golub (dbg) at Carnegie-Mellon University
 *	Changes for MACH_KERNEL:
 *	. Removed non-XP code.
 *	Count page wiring when sending page to default pager.
 *	Increase reserved page count.
 *	Make 'internal-only' count LARGER than reserved page count.
 *
 * Revision 2.18  89/06/12  14:53:05  jsb
 * 	Picked up bug fix (missing splimps) from Sequent via dlb.
 * 	[89/06/12  14:39:28  jsb]
 * 
 * Revision 2.17  89/04/18  21:27:08  mwyoung
 * 	Recent history [mwyoung]:
 * 		Keep hint when pages are written out (call
 * 		 vm_external_state_set).
 * 		Use wired-down fictitious page data structure for "holding_page".
 * 	History condensation:
 * 		Avoid flooding memory managers by using timing [mwyoung].
 * 		New pageout daemon for external memory management
 * 		 system [mwyoung].
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
 *	File:	vm/vm_pageout.c
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 *	Date:	1985
 *
 *	The proverbial page-out daemon.
 */

#include <mach_pagemap.h>
#include <norma_vm.h>
#include <norma_ipc.h>
#include <mach_kdb.h>

#include <mach/mach_types.h>
#include <mach/memory_object.h>
#include <mach/memory_object_default.h>
#include <mach/memory_object_user.h>
#include <mach/mach_host_server.h>
#include <mach/vm_param.h>
#include <mach/vm_statistics.h>
#include <kern/counters.h>
#include <kern/ipc_sched.h>
#include <kern/thread.h>
#include <vm/pmap.h>
#include <vm/vm_map.h>
#include <vm/vm_object.h>
#include <vm/vm_page.h>
#include <vm/vm_pageout.h>
#include <machine/vm_tuning.h>
#if	NORMA_VM
#include <kern/ipc_sched.h>		/* for convert_ipc_timeout_to_ticks */
#include <norma/xmm_server_rename.h>	/* for memory_object_data_return */
#endif
#include <kern/misc_protos.h>
#include <device/net_io.h>

#if	NORMA_VM
/*
 *	Number of pageouts to insert into a paging
 *	tree at any one time.
 */
#ifndef	VM_PAGEOUT_INTERNAL_RESERVATION_MAX
#define	VM_PAGEOUT_INTERNAL_RESERVATION_MAX	10
#endif

#ifndef	VM_PAGEOUT_EXTERNAL_RESERVATION_MAX
#define	VM_PAGEOUT_EXTERNAL_RESERVATION_MAX	10
#endif

#ifndef	VM_PAGEOUT_LOCAL_RESERVATION_MAX
#define	VM_PAGEOUT_LOCAL_RESERVATION_MAX	1500
#endif

#ifndef	VM_PAGEOUT_LOCK_REQUEST_RESERVATION_MAX
#define	VM_PAGEOUT_LOCK_REQUEST_RESERVATION_MAX	10
#endif

#ifndef	VM_PAGEOUT_SYNCHRONIZE_RESERVATION_MAX
#define	VM_PAGEOUT_SYNCHRONIZE_RESERVATION_MAX	15
#endif
#endif	/* NORMA_VM */

#ifndef	VM_PAGEOUT_BURST_MAX
#define	VM_PAGEOUT_BURST_MAX	10		/* number of pages */
#endif	/* VM_PAGEOUT_BURST_MAX */

#ifndef	VM_PAGEOUT_BURST_MIN
#define	VM_PAGEOUT_BURST_MIN	5		/* number of pages */
#endif	/* VM_PAGEOUT_BURST_MIN */

#ifndef	VM_PAGEOUT_BURST_WAIT
#define	VM_PAGEOUT_BURST_WAIT	30		/* milliseconds per page */
#endif	/* VM_PAGEOUT_BURST_WAIT */

#ifndef	VM_PAGEOUT_EMPTY_WAIT
#define VM_PAGEOUT_EMPTY_WAIT	200		/* milliseconds */
#endif	/* VM_PAGEOUT_EMPTY_WAIT */

#ifndef	VM_PAGEOUT_PAUSE_MAX
#define	VM_PAGEOUT_PAUSE_MAX	10		/* number of pauses */
#endif	/* VM_PAGEOUT_PAUSE_MAX */

/*
 *	To obtain a reasonable LRU approximation, the inactive queue
 *	needs to be large enough to give pages on it a chance to be
 *	referenced a second time.  This macro defines the fraction
 *	of active+inactive pages that should be inactive.
 *	The pageout daemon uses it to update vm_page_inactive_target.
 *
 *	If vm_page_free_count falls below vm_page_free_target and
 *	vm_page_inactive_count is below vm_page_inactive_target,
 *	then the pageout daemon starts running.
 */

#ifndef	VM_PAGE_INACTIVE_TARGET
#define	VM_PAGE_INACTIVE_TARGET(avail)	((avail) * 2 / 3)
#endif	/* VM_PAGE_INACTIVE_TARGET */

/*
 *	Once the pageout daemon starts running, it keeps going
 *	until vm_page_free_count meets or exceeds vm_page_free_target.
 */

#ifndef	VM_PAGE_FREE_TARGET
#define	VM_PAGE_FREE_TARGET(free)	(15 + (free) / 80)
#endif	/* VM_PAGE_FREE_TARGET */

/*
 *	The pageout daemon always starts running once vm_page_free_count
 *	falls below vm_page_free_min.
 */

#ifndef	VM_PAGE_FREE_MIN
#define	VM_PAGE_FREE_MIN(free)	(10 + (free) / 100)
#endif	/* VM_PAGE_FREE_MIN */

/*
 *	When vm_page_free_count falls below vm_page_free_reserved,
 *	only vm-privileged threads can allocate pages.  vm-privilege
 *	allows the pageout daemon and default pager (and any other
 *	associated threads needed for default pageout) to continue
 *	operation by dipping into the reserved pool of pages.
 */

#ifndef	VM_PAGE_FREE_RESERVED
#if	NORMA_VM
/*
 *	XXX Due to deficiencies in the norma pageout path, the
 *	reserved pool must be fairly large.  For each pending
 *	pageout, a page may be needed for a kmsg and another
 *	one for the data.  An additional page may be required
 *	for a reply to norma_ipc_pull_receive.  Then multiply
 *	by the number of threads in the default pager (3),
 *	and add a bit of slop (10).  This computation does not
 *	scale for multiprocessors, nor for additional threads
 *	in the default pager, not anything else.  It is a big
 *	HACK.  XXX
 */
/*
 *    Once this formula is understood/corrected, COMMENT IT. XXXX
 */
#define	VM_PAGE_FREE_RESERVED	(vm_pageout_burst_max*3)
#else	/* NORMA_VM */
#define	VM_PAGE_FREE_RESERVED	(15 + NCPUS)
#endif	/* NORMA_VM */
#endif	/* VM_PAGE_FREE_RESERVED */

/*
 *	When vm_page_free_count falls below vm_pageout_reserved_internal,
 *	the pageout daemon no longer trusts external pagers to clean pages.
 *	External pagers are probably all wedged waiting for a free page.
 *	It forcibly double-pages dirty pages belonging to external objects,
 *	getting the pages to the default pager to clean.
 */

#ifndef	VM_PAGEOUT_RESERVED_INTERNAL
#define	VM_PAGEOUT_RESERVED_INTERNAL(reserve)	((reserve) - 5)
#endif	/* VM_PAGEOUT_RESERVED_INTERNAL */

/*
 *	When vm_page_free_count falls below vm_pageout_reserved_really,
 *	the pageout daemon stops work entirely to let the default pager
 *	catch up (assuming the default pager has pages to clean).
 *	Beyond this point, it is too dangerous to consume memory
 *	even for memory_object_data_write messages to the default pager.
 */

#ifndef	VM_PAGEOUT_RESERVED_REALLY
#define	VM_PAGEOUT_RESERVED_REALLY(reserve)	((reserve) - 10)
#endif	/* VM_PAGEOUT_RESERVED_REALLY */

/*
 * Forward declarations for internal routines.
 */
extern void vm_pageout_continue(void);
extern void vm_pageout_scan(void);
extern void vm_pageout_scan_continue(void);

unsigned int vm_pageout_reserved_internal = 0;
unsigned int vm_pageout_reserved_really = 0;

unsigned int vm_pageout_burst_max = 0;
unsigned int vm_pageout_burst_min = 0;
unsigned int vm_pageout_burst_wait = 0;		/* milliseconds per page */
unsigned int vm_pageout_empty_wait = 0;		/* milliseconds */
unsigned int vm_pageout_pause_count = 0;
unsigned int vm_pageout_pause_max = 0;

#if	NORMA_VM
/*
 *	Track pageout reservations.   In a NORMA system,
 *	the vm_page_laundry_count mechanism does not
 *	suffice to throttle pageout traffic.
 *
 *	In the first place, NORMA IPC frees up pages as
 *	soon as they move off-node but long before the
 *	target pager receives (much less handles) the
 *	pageout messages.  Thus, it is possible for one
 *	or more clients to swamp a common vnode pager
 *	with pageout requests.  We remedy this problem
 *	by mandating a data_write_completed message in
 *	response to every data_write, data_initialize
 *	or data_return.  In theory, we could then alter
 *	NORMA IPC to defer freeing pageout messages and
 *	let data_write_completed do that job, thus once
 *	again taking advantage of the laundry count scheme.
 *	However, the laundry count scheme does not take
 *	into account pageouts generated by lock_request
 *	and pageouts sent to an external pager that do
 *	not involve double-paging.
 *
 *	The pageout reservation system is a way to extend
 *	the laundry count mechanism for use by clients
 *	other than the pageout scan path.  Moreover,
 *	the pageout reservation system could easily be
 *	generalized to support per-node or per-pager
 *	reservation quotas.
 *
 *	N.B.  The reservation variables are simple integers
 *	to catch underflow problems.
 *
 *	N.B.  As of this date, 8 April 1993, this code has
 *	not been tested on a shared-memory multiprocessor.
 */
#define PAGEOUT_RESERVE_FREE	(-1)
#define	RESERVATIONS(type)						\
	vm_pageout_reservations[(type)]
#define	RESERVATION_AVAILABLE(type)					\
	(vm_pageout_reservations[(type)] <				\
	 vm_pageout_reservation_max[(type)])
#define	vm_pageout_reservation_wait(type)				\
	assert_wait((int)&vm_pageout_reservation_needed[(type)], FALSE);

mutex_t		vm_pageout_reservation_lock;
#define	RESERVATION_LOCK	mutex_lock(&vm_pageout_reservation_lock)
#define	RESERVATION_UNLOCK	mutex_unlock(&vm_pageout_reservation_lock)

unsigned int	vm_pageout_reservation_max[PAGEOUT_RESERVE_MAX];
unsigned int	vm_pageout_reservations[PAGEOUT_RESERVE_MAX];
unsigned int	vm_pageout_reservation_needed[PAGEOUT_RESERVE_MAX];

#define	RESERVATION_DEBUG	MACH_ASSERT
#if	RESERVATION_DEBUG
void	vm_pageout_reservation_assertions(int line);
#else	/* RESERVATION_DEBUG */
#define	vm_pageout_reservation_assertions(line_num)
#endif	/* RESERVATION_DEBUG */

#define	RESERVATION_COUNTERS	1
#if	RESERVATION_COUNTERS
unsigned int	c_vm_pageout_reserve_granted[PAGEOUT_RESERVE_MAX];
unsigned int	c_vm_pageout_reserve_denied[PAGEOUT_RESERVE_MAX];
unsigned int	c_vm_pageout_reserve_noflow[PAGEOUT_RESERVE_MAX];
unsigned int	c_vm_pageout_completed[PAGEOUT_RESERVE_MAX];
unsigned int	c_vm_pageout_needed_max[PAGEOUT_RESERVE_MAX];
unsigned int	c_vm_pageout_wakeups[PAGEOUT_RESERVE_MAX];
unsigned int	c_vm_pageout_big_wakeups[PAGEOUT_RESERVE_MAX];
unsigned int	c_vm_pageout_terminated[PAGEOUT_RESERVE_MAX];
unsigned int	c_vm_pageout_reservation_total = 0;
unsigned int	c_vm_pageout_total_completed = 0;
#define	RC(op)	op
#else
#define	RC(op)
#endif	/* RESERVATION_COUNTERS */
#endif	/* NORMA_VM */

/*
 *	These variables record the pageout daemon's actions:
 *	how many pages it looks at and what happens to those pages.
 *	No locking needed because only one thread modifies the variables.
 */

unsigned int vm_pageout_active = 0;		/* debugging */
unsigned int vm_pageout_inactive = 0;		/* debugging */
unsigned int vm_pageout_inactive_nolock = 0;	/* debugging */
unsigned int vm_pageout_inactive_avoid = 0;	/* debugging */
unsigned int vm_pageout_inactive_busy = 0;	/* debugging */
unsigned int vm_pageout_inactive_absent = 0;	/* debugging */
unsigned int vm_pageout_inactive_used = 0;	/* debugging */
unsigned int vm_pageout_inactive_clean = 0;	/* debugging */
unsigned int vm_pageout_inactive_dirty = 0;	/* debugging */
unsigned int vm_pageout_inactive_double = 0;	/* debugging */
#if	NORMA_VM
unsigned int vm_pageout_reservation_miss = 0;	/* debugging */

#define	PENDING_TRACKING	(MACH_ASSERT || MACH_KDB)

typedef struct pending_record {
	ipc_port_t	pager;
	vm_offset_t	offset;
	int		type;
#if	PENDING_TRACKING
	vm_object_t	object;
#endif
} pending_record;

#define	VM_PAGEOUT_PENDING_MAX	(VM_PAGEOUT_INTERNAL_RESERVATION_MAX     + \
				 VM_PAGEOUT_EXTERNAL_RESERVATION_MAX     + \
				 VM_PAGEOUT_LOCAL_RESERVATION_MAX        + \
				 VM_PAGEOUT_LOCK_REQUEST_RESERVATION_MAX + \
				 VM_PAGEOUT_SYNCHRONIZE_RESERVATION_MAX)

pending_record	vm_pageout_pending[VM_PAGEOUT_PENDING_MAX];

int	vm_pageout_pending_max = VM_PAGEOUT_PENDING_MAX;

/*
 *	Attempt to reserve a pageout slot for
 *	a range of pages.
 *
 *	MP notes: vm_object is locked.
 */
boolean_t
vm_pageout_reserve(
	vm_object_t	object,
	vm_offset_t	offset,
	vm_size_t	size,
	int		type)
{
	pending_record	*p;

	/*
	 *	The current system is limited to pageouts
	 *	of one page at a time.  Perhaps someday
	 *	we will make this function smart enough to
	 *	take into account what pager is being targeted
	 *	and (for NORMA) what node.
	 */
	assert(size == page_size);
	assert(object->pager != IP_NULL);
	assert(ip_kotype(object->pager) == IKOT_PAGER ||
	       ip_kotype(object->pager) == IKOT_PAGER_TERMINATING);

	RESERVATION_LOCK;
	assert(vm_pageout_reservations[type] >= 0);
	assert(vm_pageout_reservations[type] <=
	       vm_pageout_reservation_max[type]);

	/*
	 *	Risky:  support older pagers by always
	 *	allowing pageouts to them.  The intended
	 *	purpose is to allow clusters to continue
	 *	operating with a first BSD server and
	 *	a second AD server.  In general, of course,
	 *	this check defeats the purpose of paging
	 *	flow control.
	 */
	if (object->pager_modwc == FALSE) {
		RESERVATION_UNLOCK;
		RC(++c_vm_pageout_reserve_noflow[type]);
		return TRUE;
	}

	/*
	 *	Deny a reservation attempt if too many reservations
	 *	are already pending.
	 */
	if (!RESERVATION_AVAILABLE(type)) {
		RESERVATION_UNLOCK;
		RC(++c_vm_pageout_reserve_denied[type]);
		return FALSE;
 	}

	++vm_pageout_reservations[type];
	++object->completions_pending;
	RC(++c_vm_pageout_reservation_total);
	RC(++c_vm_pageout_reserve_granted[type]);

	for (p = vm_pageout_pending;
	     p < vm_pageout_pending + vm_pageout_pending_max;
	     ++p) {
		if (p->type != PAGEOUT_RESERVE_FREE)
			continue;
		p->pager = object->pager;
		p->offset = offset;
		p->type = type;
#if	PENDING_TRACKING
		p->object = object;
#endif
		RESERVATION_UNLOCK;
		return TRUE;
	}
	panic("vm_pageout_reserve");
	return FALSE;
}


/*
 *	Update pageout reservation data to
 *	account for the completion of paging
 *	on a range.
 *
 *	In/Out conditions:
 *		object must be locked.
 */
void
vm_pageout_completed(
	vm_object_t	object,
	vm_offset_t	offset,
	vm_size_t	size)
{
	pending_record	*p;
	boolean_t	found;
	int 		found_type;

	/*
	 *	Should receive a completion length
	 *	that is a multiple of our page size;
	 *	and in the current system, we are
	 *	limited to one page at a time.
	 */
	assert(size % page_size == 0);
	assert(size == page_size);
	assert(object != VM_OBJECT_NULL);

	RC(++c_vm_pageout_total_completed);
	RESERVATION_LOCK;
	vm_pageout_reservation_assertions(__LINE__);

	found = FALSE;
	for (p = vm_pageout_pending;
	     p < vm_pageout_pending + vm_pageout_pending_max;
	     ++p) {
		if (p->type == PAGEOUT_RESERVE_FREE ||
		    p->pager != object->pager || p->offset != offset)
			continue;
		assert(found == FALSE);
		found = TRUE;
		assert(vm_pageout_reservations[p->type] > 0);
		--vm_pageout_reservations[p->type];
		RC(++c_vm_pageout_completed[p->type]);
		object->completions_pending--;
		if (object->completions_pending == 0)
			vm_object_wakeup(object, VM_OBJECT_EVENT_COMPLETION);
		found_type = p->type;
		p->type = PAGEOUT_RESERVE_FREE;
		break;
	}
	if (found == FALSE)
		panic("vm_pageout_completed:  no pending record");

	/*
	 *	It may be a better idea to only wake
	 *	up one waiter at a time. XXX
	 */
	if (vm_pageout_reservation_needed[found_type] > 0) {
#if	RESERVATION_COUNTERS
		++c_vm_pageout_wakeups[found_type];
		if (vm_pageout_reservation_needed[found_type] > 1)
			++c_vm_pageout_big_wakeups[found_type];
#endif
		thread_wakeup((int)&vm_pageout_reservation_needed[found_type]);
	}

	RESERVATION_UNLOCK;
}


void
vm_pageout_await_completed(
	int	type)
{
	RESERVATION_LOCK;
	assert(type > PAGEOUT_RESERVE_FREE && type < PAGEOUT_RESERVE_MAX);
	assert(vm_pageout_reservations[type] >= 0);
	vm_pageout_reservation_assertions(__LINE__);
	while (vm_pageout_reservations[type] >=
	       vm_pageout_reservation_max[type]) {
		assert(vm_pageout_reservation_needed[type] >= 0);
		++vm_pageout_reservation_needed[type];
#if	RESERVATION_COUNTERS
		if (vm_pageout_reservation_needed[type] >
		    c_vm_pageout_needed_max[type])
			c_vm_pageout_needed_max[type] =
				vm_pageout_reservation_needed[type];
#endif
		vm_pageout_reservation_wait(type);
		RESERVATION_UNLOCK;
		thread_block((void (*)(void)) 0);
		RESERVATION_LOCK;
		--vm_pageout_reservation_needed[type];
		assert(vm_pageout_reservation_needed[type] >= 0);
	}
	RESERVATION_UNLOCK;
}


/*
 *	Release all of the reservations held by an object.
 *	When this routine is invoked, the object port has
 *	been torn down and it cannot be reached.
 *
 *	MP notes: vm_object is locked.
 */
void
vm_pageout_reserve_terminate(
	vm_object_t	object)
{
	pending_record	*p;
	int		type;

	assert(object != VM_OBJECT_NULL);
	RESERVATION_LOCK;
	vm_pageout_reservation_assertions(__LINE__);
	for (p = vm_pageout_pending;
	     p < vm_pageout_pending + vm_pageout_pending_max;
	     ++p) {
		if (p->pager != object->pager ||
		    (type = p->type) == PAGEOUT_RESERVE_FREE)
		    continue;
		p->type = PAGEOUT_RESERVE_FREE;
		assert(vm_pageout_reservations[type] > 0);
		--vm_pageout_reservations[type];
		RC(++c_vm_pageout_completed[type]);
		object->completions_pending--;
#if	0
		if (object->completions_pending == 0)
			vm_object_wakeup(object, VM_OBJECT_EVENT_COMPLETION);
#endif
		if (vm_pageout_reservation_needed[type] > 0) {
			RC(++c_vm_pageout_wakeups[type]);
			thread_wakeup_one((int)
				  &vm_pageout_reservation_needed[type]);
		}
		RC(++c_vm_pageout_terminated[type]);
	}
	RESERVATION_UNLOCK;
}


#if	RESERVATION_DEBUG
/*
 *	Check the state of the page reservation system.
 */
void
vm_pageout_reservation_assertions(
	int	line)
{
	int		type;
	boolean_t	reservation;

	reservation = FALSE;
	for (type = 0; type < PAGEOUT_RESERVE_MAX; ++type) {
		if (!reservation)
			reservation = (vm_pageout_reservations[type] > 0);
		assert(vm_pageout_reservations[type] <=
		       vm_pageout_reservation_max[type]);
	}
	assert(reservation);
}
#endif	/* RESERVATION_DEBUG */

#define	NORMA_VM_PRECIOUS_RETURN_HACK	1
#endif	/* NORMA_VM */

/*
 *	Routine:	vm_pageout_setup
 *	Purpose:
 *		Set up a page for pageout.
 *
 *		Move or copy the page to a new object, as part
 *		of which it will be sent to its memory manager
 *		in a memory_object_data_write or memory_object_initialize
 *		message.
 *
 *		The "paging_offset" argument specifies the offset
 *		of the page within its external memory object.
 *
 *		The "new_object" and "new_offset" arguments
 *		indicate where the page should be moved.
 *
 *		The "flush" argument specifies whether the page
 *		should be flushed from its object.  If not, a
 *		copy of the page is moved to the new object.
 *
 *	In/Out conditions:
 *		The page in question must not be on any pageout queues,
 *		and must be busy.  The object to which it belongs
 *		must be unlocked, and the caller must hold a paging
 *		reference to it.  The new_object must not be locked.
 *
 *		If the page is flushed from its original object,
 *		this routine returns a pointer to a place-holder page,
 *		inserted at the same offset, to block out-of-order
 *		requests for the page.  The place-holder page must
 *		be freed after the data_write or initialize message
 *		has been sent.  If the page is copied,
 *		the holding page is VM_PAGE_NULL.
 *
 *		The original page is put on a paging queue and marked
 *		not busy on exit.
 */
vm_page_t
vm_pageout_setup(
	register vm_page_t	m,
	vm_offset_t		paging_offset,
	register vm_object_t	new_object,
	vm_offset_t		new_offset,
	boolean_t		flush)
{
	register vm_object_t	old_object = m->object;
	kern_return_t	rc;
	register vm_page_t	holding_page;
	register vm_page_t	new_m = VM_PAGE_NULL;

	assert(m->busy && !m->absent && !m->fictitious && !m->error &&
		!m->restart);
#if	MACH_ASSERT
	if (new_object->paging_object == VM_OBJECT_NULL)
		new_object->paging_object = old_object;
	else
		assert(new_object->paging_object == old_object);
#endif

	/*
	 *	If we are not flushing the page, allocate a
	 *	page in the object.  If we cannot get the
	 *	page, wait for one.  This can't cause pageout
	 *	problems, because the pageout daemon always
	 *	wants to flush.
	 */
	if (!flush) {
		while (new_m == VM_PAGE_NULL) {
			vm_object_lock(new_object);
			new_m = vm_page_alloc(new_object, new_offset);
			vm_object_unlock(new_object);
			if (new_m == VM_PAGE_NULL) {
				VM_PAGE_WAIT();
			}
		}
	}

	if (flush) {
		/*
		 *	Create a place-holder page where the old one was,
		 *	to prevent anyone from attempting to page in this
		 *	page while we`re unlocked.
		 */
		while ((holding_page = vm_page_grab_fictitious())
							== VM_PAGE_NULL)
			vm_page_more_fictitious();

		vm_object_lock(old_object);
		vm_page_lock_queues();
		vm_page_remove(m);
		vm_page_unlock_queues();
		PAGE_WAKEUP_DONE(m);

		vm_page_lock_queues();
		vm_page_insert(holding_page, old_object, m->offset);
		vm_page_unlock_queues();

		/*
		 *	Record that this page has been written out
		 */
#if	MACH_PAGEMAP
		vm_external_state_set(&old_object->existence_info,
				      paging_offset);
#endif	/* MACH_PAGEMAP */

		vm_object_unlock(old_object);

		vm_object_lock(new_object);

		/*
		 *	Move this page into the new object
		 */

		vm_page_lock_queues();
		vm_page_insert(m, new_object, new_offset);
		vm_page_unlock_queues();

		m->dirty = TRUE;
		m->precious = FALSE;
		m->page_lock = VM_PROT_NONE;
		m->unusual = FALSE;
		m->unlock_request = VM_PROT_NONE;
	}
	else {
		/*
		 *	Copy the data into the new page,
		 *	and mark the new page as clean.
		 */
		vm_page_copy(m, new_m);

		vm_object_lock(old_object);
		m->dirty = FALSE;
		pmap_clear_modify(m->phys_addr);

#if	NORMA_VM_PRECIOUS_RETURN_HACK
		/*
		 *	Mark the page precious to preserve the xmm svm
		 *	invariant that if a page has been dirtied, then
		 *	all kernels that have copies of the page have
		 *	precious copies of the page.
		 */
		m->precious = TRUE;
#endif	/* NORMA_VM_PRECIOUS_RETURN_HACK */

		/*
		 *	Deactivate old page.
		 */
		vm_page_lock_queues();
		vm_page_deactivate(m);
		vm_page_unlock_queues();

		PAGE_WAKEUP_DONE(m);

		/*
		 *	Record that this page has been written out
		 */

#if	MACH_PAGEMAP
		vm_external_state_set(&old_object->existence_info,
				      paging_offset);
#endif	/* MACH_PAGEMAP */

		vm_object_unlock(old_object);

		vm_object_lock(new_object);

		/*
		 *	Use the new page below.
		 */
		m = new_m;
		m->dirty = TRUE;
		assert(!m->precious);
		PAGE_WAKEUP_DONE(m);
	}

	/*
	 *	Make the old page eligible for replacement again; if a
	 *	user-supplied memory manager fails to release the page,
	 *	it will be paged out again to the default memory manager.
	 *
	 *	Note that pages written to the default memory manager
	 *	must be wired down -- in return, it guarantees to free
	 *	this page, rather than reusing it.
	 */

	vm_page_lock_queues();
	vm_stat.pageouts++;
	if (m->laundry) {
		/*
		 *	vm_pageout_scan is telling us to put this page
		 *	at the front of the inactive queue, so it will
		 *	be immediately paged out to the default pager.
		 */

		assert(!old_object->internal);
		m->laundry = FALSE;

		queue_enter_first(&vm_page_queue_inactive, m,
				  vm_page_t, pageq);
		m->inactive = TRUE;
		vm_page_inactive_count++;
	} else if (old_object->internal) {
		m->laundry = TRUE;
		vm_page_laundry_count++;

		vm_page_wire(m);
	} else
		vm_page_activate(m);
	vm_page_unlock_queues();

	/*
	 *	Since IPC operations may block, we drop locks now.
	 *	[The placeholder page is busy, and we still have
	 *	paging_in_progress incremented.]
	 */

	vm_object_unlock(new_object);

	/*
	 *	Return the placeholder page to simplify cleanup.
	 */
	return (flush ? holding_page : VM_PAGE_NULL);
}

/*
 *	Routine:	vm_pageout_page
 *	Purpose:
 *		Causes the specified page to be written back to
 *		the appropriate memory object.
 *
 *		The "initial" argument specifies whether this
 *		data is an initialization only, and should use
 *		memory_object_data_initialize instead of
 *		memory_object_data_write.
 *
 *		The "flush" argument specifies whether the page
 *		should be flushed from the object.  If not, a
 *		copy of the data is sent to the memory object.
 *
 *	In/out conditions:
 *		The page in question must not be on any pageout queues.
 *		The object to which it belongs must be locked.
 *	Implementation:
 *		Move this page to a completely new object, if flushing;
 *		copy to a new page in a new object, if not.
 */
void
vm_pageout_page(
	register vm_page_t	m,
	boolean_t		initial,
	boolean_t		flush)
{
	vm_map_copy_t		copy;
	register vm_object_t	old_object;
	register vm_object_t	new_object;
	register vm_page_t	holding_page;
	vm_offset_t		paging_offset;
	kern_return_t		rc;
	boolean_t		precious_clean;

	assert(m->busy);

	/*
	 *	Cleaning but not flushing a clean precious page is a
	 *	no-op.  Remember whether page is clean and precious now
	 *	because vm_pageout_setup will mark it dirty and not precious.
	 *
	 * XXX Check if precious_clean && !flush can really happen.
	 */
	precious_clean = (!m->dirty) && m->precious;
	if (precious_clean && !flush) {
		PAGE_WAKEUP_DONE(m);
		panic("reservation without pageout?"); /* alan */
		return;
	}

	/*
	 *	Verify that we really want to clean this page.
	 */
	if (m->absent || m->error || m->restart ||
	    (!m->dirty && !m->precious)) {
		VM_PAGE_FREE(m);
		panic("reservation without pageout?"); /* alan */
		return;
	}

	/*
	 *	Create a paging reference to let us play with the object.
	 */
	old_object = m->object;
	paging_offset = m->offset + old_object->paging_offset;
	vm_object_paging_begin(old_object);
	vm_object_unlock(old_object);

	/*
	 *	Allocate a new object into which we can put the page.
	 */
	new_object = vm_object_allocate(PAGE_SIZE);

	/*
	 *	Move the page into the new object.
	 */
	holding_page = vm_pageout_setup(m,
				paging_offset,
				new_object,
				0,		/* new offset */
				flush);		/* flush */

	rc = vm_map_copyin_object(new_object, 0, PAGE_SIZE, &copy);
	assert(rc == KERN_SUCCESS);

#if	NORMA_VM
	assert(! initial);
	rc = memory_object_data_return(old_object->pager,
				       old_object->pager_request,
				       paging_offset, copy,
				       PAGE_SIZE, !precious_clean, !flush);
#else	/* NORMA_VM */
	if (initial) {
		rc = memory_object_data_initialize(old_object->pager,
						   old_object->pager_request,
						   paging_offset,
						   (pointer_t) copy,
						   PAGE_SIZE);
	} else {
		rc = memory_object_data_return(old_object->pager,
					       old_object->pager_request,
					       paging_offset,
					       (pointer_t) copy, PAGE_SIZE,
					       !precious_clean, !flush);
	}
#endif	/* NORMA_VM */

	if (rc != KERN_SUCCESS)
		vm_map_copy_discard(copy);

	/*
	 *	Clean up.
	 */
	vm_object_lock(old_object);
	if (holding_page != VM_PAGE_NULL)
	    VM_PAGE_FREE(holding_page);
	vm_object_paging_end(old_object);
}

/*
 *	vm_pageout_scan does the dirty work for the pageout daemon.
 *	It returns with vm_page_queue_free_lock held and
 *	vm_page_free_wanted == 0.
 */

void
vm_pageout_scan(void)
{
	unsigned int burst_count;
	unsigned int laundry_pages;
#if	NORMA_VM
	unsigned int reserve_try;
	unsigned int obj_type;
#endif	/* NORMA_VM */

	/*
	 *	We want to gradually dribble pages from the active queue
	 *	to the inactive queue.  If we let the inactive queue get
	 *	very small, and then suddenly dump many pages into it,
	 *	those pages won't get a sufficient chance to be referenced
	 *	before we start taking them from the inactive queue.
	 *
	 *	We must limit the rate at which we send pages to the pagers.
	 *	data_write messages consume memory, for message buffers and
	 *	for map-copy objects.  If we get too far ahead of the pagers,
	 *	we can potentially run out of memory.
	 *
	 *	We can use the laundry count to limit directly the number
	 *	of pages outstanding to the default pager.  A similar
	 *	strategy for external pagers doesn't work, because
	 *	external pagers don't have to deallocate the pages sent them,
	 *	and because we might have to send pages to external pagers
	 *	even if they aren't processing writes.  So we also
	 *	use a burst count to limit writes to external pagers.
	 *
	 *	When memory is very tight, we can't rely on external pagers to
	 *	clean pages.  They probably aren't running, because they
	 *	aren't vm-privileged.  If we kept sending dirty pages to them,
	 *	we could exhaust the free list.  However, we can't just ignore
	 *	pages belonging to external objects, because there might be no
	 *	pages belonging to internal objects.  Hence, we get the page
	 *	into an internal object and then immediately double-page it,
	 *	sending it to the default pager.
	 *
	 *	consider_zone_gc should be last, because the other operations
	 *	might return memory to zones.  When we pause we use
	 *	vm_pageout_scan_continue as our continuation, so we will
	 *	reenter vm_pageout_scan periodically and attempt to reclaim
	 *	internal memory even if we never reach vm_page_free_target.
	 */

    Restart:
	stack_collect();
	net_kmsg_collect();
	consider_task_collect();
	consider_thread_collect();
	consider_zone_gc();

#if	NORMA_VM
	/*
	 *	Set reserve_try to be reasonable number of pages that
	 *	can be denied before we go to sleep.
	 */
	reserve_try = vm_page_inactive_count;
#endif	/* NORMA_VM */
	for (burst_count = 0;;) {
		register vm_page_t m;
		register vm_object_t object;
		unsigned int free_count;

		/*
		 *	Recalculate vm_page_inactivate_target.
		 */

		vm_page_lock_queues();
		vm_page_inactive_target =
			VM_PAGE_INACTIVE_TARGET(vm_page_active_count +
						vm_page_inactive_count);

		/*
		 *	Move pages from active to inactive.
		 */

		while ((vm_page_inactive_count < vm_page_inactive_target) &&
		       !queue_empty(&vm_page_queue_active)) {
			register vm_object_t object;

			vm_pageout_active++;
			m = (vm_page_t) queue_first(&vm_page_queue_active);
			assert(m->active && !m->inactive);

			object = m->object;
			if (!vm_object_lock_try(object)) {
				/*
				 *	Move page to end and continue.
				 */

				queue_remove(&vm_page_queue_active, m,
					     vm_page_t, pageq);
				queue_enter(&vm_page_queue_active, m,
					    vm_page_t, pageq);
				vm_page_unlock_queues();
				vm_page_lock_queues();
				continue;
			}

			/*
			 *	If the page is busy, then we pull it
			 *	off the active queue and leave it alone.
			 */

			if (m->busy) {
				vm_object_unlock(object);
				queue_remove(&vm_page_queue_active, m,
					     vm_page_t, pageq);
				m->active = FALSE;
				vm_page_active_count--;
				continue;
			}

			/*
			 *	Deactivate the page while holding the object
			 *	locked, so we know the page is still not busy.
			 *	This should prevent races between pmap_enter
			 *	and pmap_clear_reference.  The page might be
			 *	absent or fictitious, but vm_page_deactivate
			 *	can handle that.
			 */

			vm_page_deactivate(m);
			vm_object_unlock(object);
		}

		/*
		 *	We are done if we have met our target *and*
		 *	nobody is still waiting for a page.
		 */

		mutex_lock(&vm_page_queue_free_lock);
		free_count = vm_page_free_count;
		if ((free_count >= vm_page_free_target) &&
		    (vm_page_free_wanted == 0)) {
			vm_page_unlock_queues();
			break;
		}
		mutex_unlock(&vm_page_queue_free_lock);

		/*
		 * Sometimes we have to pause:
		 *	1) No inactive pages - nothing to do.
		 *	2) Flow control - wait for pagers to catch up.
		 *	3) Extremely low memory - sending out dirty pages
		 *	consumes memory.  We don't take the risk of doing
		 *	this if the default pager already has work to do.
		 *
		 *	Attempt to formulate conditions equivalent to
		 *	cases 1-3 for NORMA_VM.
		 *
		 */

		if (queue_empty(&vm_page_queue_inactive) ||
		    (burst_count >= vm_pageout_burst_max) ||
#if	NORMA_VM
		    (reserve_try <= 0) ||
		    (!RESERVATION_AVAILABLE(PAGEOUT_RESERVE_INTERNAL)) ||
		    ((free_count < vm_pageout_reserved_really) &&
		     (RESERVATIONS(PAGEOUT_RESERVE_INTERNAL) > 0))
#else	/* NORMA_VM */
		    (vm_page_laundry_count >= vm_pageout_burst_max) ||
		    ((free_count < vm_pageout_reserved_really) &&
		     (vm_page_laundry_count > 0))
#endif	/* NORMA_VM */
		    ) {
			unsigned int pages, msecs;

			/*
			 *	vm_pageout_burst_wait is msecs/page.
			 *	If there is nothing for us to do, we wait
			 *	at least vm_pageout_empty_wait msecs.
			 */
#if	NORMA_VM
			RESERVATION_LOCK;
			laundry_pages =
				RESERVATIONS(PAGEOUT_RESERVE_INTERNAL);
			RESERVATION_UNLOCK;
#else	/* NORMA_VM */
			laundry_pages = vm_page_laundry_count;
#endif	/* NORMA_VM */

			if (laundry_pages > burst_count)
				pages = laundry_pages;
			else
				pages = burst_count;
			msecs = pages * vm_pageout_burst_wait;
#if	NORMA_VM
			if (reserve_try == 0 && msecs < vm_pageout_empty_wait)
				msecs = vm_pageout_empty_wait;
#endif	/* NORMA_VM */
			if (queue_empty(&vm_page_queue_inactive) &&
			    (msecs < vm_pageout_empty_wait))
				msecs = vm_pageout_empty_wait;
			vm_page_unlock_queues();
#if	NORMA_VM
			assert(msecs > 0);
#endif	/* NORMA_VM */
			thread_will_wait_with_timeout(current_thread(), msecs);
			counter(c_vm_pageout_scan_block++);

			/*
			 *	Unfortunately, we don't have call_continuation
			 *	so we can't rely on tail-recursion.
			 */
			thread_block((void (*)(void)) 0);
			vm_pageout_scan_continue();
			goto Restart;
		}

		vm_pageout_inactive++;
		m = (vm_page_t) queue_first(&vm_page_queue_inactive);
		assert(!m->active && m->inactive);
		object = m->object;

		/*
		 *	Try to lock object; since we've got the
		 *	page queues lock, we can only try for this one.
		 */

		if (!vm_object_lock_try(object)) {
			/*
			 *	Move page to end and continue.
			 */
			queue_remove(&vm_page_queue_inactive, m,
				     vm_page_t, pageq);
			queue_enter(&vm_page_queue_inactive, m,
				    vm_page_t, pageq);
			vm_page_unlock_queues();
			vm_pageout_inactive_nolock++;
			continue;
		}

		/*
		 *	Paging out pages of objects which pager is being
		 *	created by another thread must be avoided, because
		 *	this thread may claim for memory, thus leading to a
		 *	possible dead lock between it and the pageout thread
		 *	which will wait for pager creation, if such pages are
		 *	finally chosen. The remaining assumption is that there
		 *	will finally be enough available pages in the inactive
		 *	pool to page out in order to satisfy all memory claimed
		 *	by the thread which concurrently creates the pager.
		 */

		if (!object->pager_initialized && object->pager_created) {
			/*
			 *	Move page to end and continue, hoping that
			 *	there will be enough other inactive pages to
			 *	page out so that the thread which currently
			 *	initializes the pager will succeed.
			 */
			queue_remove(&vm_page_queue_inactive, m,
				     vm_page_t, pageq);
			queue_enter(&vm_page_queue_inactive, m,
				    vm_page_t, pageq);
			vm_page_unlock_queues();
			vm_object_unlock(object);
			vm_pageout_inactive_avoid++;
			continue;
		}

		/*
		 *	Remove the page from the inactive list.
		 */

		queue_remove(&vm_page_queue_inactive, m, vm_page_t, pageq);
		vm_page_inactive_count--;
		m->inactive = FALSE;

		if (m->busy || !object->alive) {
			/*
			 *	Somebody is already playing with this page.
			 *	Leave it off the pageout queues.
			 */

			vm_page_unlock_queues();
			vm_object_unlock(object);
			vm_pageout_inactive_busy++;
			continue;
		}

		/*
		 *	If it's absent or in error, we can reclaim the page.
		 */

		if (m->absent || m->error) {
			vm_pageout_inactive_absent++;
		    reclaim_page:
			vm_page_free(m);
			vm_page_unlock_queues();
			vm_object_unlock(object);
			continue;
		}

		/*
		 *	If it's being used, reactivate.
		 *	(Fictitious pages are either busy or absent.)
		 */

		assert(!m->fictitious);
		if (m->reference || pmap_is_referenced(m->phys_addr)) {
			vm_object_unlock(object);
			vm_page_activate(m);
			vm_stat.reactivations++;
			vm_page_unlock_queues();
			vm_pageout_inactive_used++;
			continue;
		}

		/*
		 *	Eliminate all mappings.
		 */

		m->busy = TRUE;
		pmap_page_protect(m->phys_addr, VM_PROT_NONE);
		if (!m->dirty)
			m->dirty = pmap_is_modified(m->phys_addr);

		/*
		 *	If it's clean and not precious, we can free the page.
		 */

		if (!m->dirty && !m->precious) {
			vm_pageout_inactive_clean++;
			goto reclaim_page;
		}

		/*
		 *	If we are very low on memory, then we can't
		 *	rely on an external pager to clean a dirty page,
		 *	because external pagers are not vm-privileged.
		 *
		 *	The laundry bit tells vm_pageout_setup to
		 *	put the page back at the front of the inactive
		 *	queue instead of activating the page.  Hence,
		 *	we will pick the page up again immediately and
		 *	resend it to the default pager.
		 */

		assert(!m->laundry);
		if ((free_count < vm_pageout_reserved_internal) &&
		    !object->internal) {
			m->laundry = TRUE;
			vm_pageout_inactive_double++;
		}
		vm_page_unlock_queues();

		/*
		 *	If there is no memory object for the page, create
		 *	one and hand it to the default pager.
		 *	[First try to collapse, so we don't create
		 *	one unnecessarily.]
		 */

		if (!object->pager_initialized)
			vm_object_collapse(object);
		if (!object->pager_initialized)
			vm_object_pager_create(object);
		if (!object->pager_initialized)
			panic("vm_pageout_scan");

		if (object->pager_initialized && object->pager == IP_NULL) {
			/*
			 * This pager has been destroyed by either
			 * memory_object_destroy or vm_object_destroy, and
			 * so there is nowhere for the page to go.
			 * Just free the page.
			 */
			VM_PAGE_FREE(m);
			vm_object_unlock(object);
			continue;
		}

#if	NORMA_VM
		/*
		 *	Get a pageout reservation now.  If there are
		 *	none available, fix up the page's state and restore
		 *	it to the inactive queue.  Go look for a new victim.
		 *
		 *	XXX When things are bad, we may miss on many
		 *	reservations.  In this case, we may be destroying
		 *	the LRU properties of the inactive queue.  XXX
		 */
		if (object->internal)
			obj_type = PAGEOUT_RESERVE_INTERNAL;
		else if (IP_NORMA_IS_PROXY(object->pager))
			obj_type = PAGEOUT_RESERVE_EXTERNAL;
		else
			obj_type = PAGEOUT_RESERVE_LOCAL;

		if (vm_pageout_reserve(object,
				       m->offset + object->paging_offset,
				       page_size,
				       obj_type) == FALSE) {
			vm_page_lock_queues();
			m->laundry = FALSE;
			m->inactive = TRUE;
			m->busy = FALSE;
			vm_page_inactive_count++;
			queue_enter(&vm_page_queue_inactive, m,
				    vm_page_t, pageq);
			vm_page_unlock_queues();
			vm_object_unlock(object);
			++vm_pageout_reservation_miss;
			--reserve_try;
			continue;
		}
#endif	/* NORMA_VM */
		vm_pageout_inactive_dirty++;
		vm_pageout_page(m, FALSE, TRUE);	/* flush it */
		vm_object_unlock(object);
		burst_count++;
	}
}

#if	RESERVATION_COUNTERS
unsigned int	c_vm_pageout_scan_continue = 0;
unsigned int	c_vm_pageout_scan_continue_external = 0;
unsigned int	c_vm_pageout_scan_continue_internal = 0;
#endif

void
vm_pageout_scan_continue(void)
{
	/*
	 *	We just paused to let the pagers catch up.
	 *	If vm_page_laundry_count is still high,
	 *	then we aren't waiting long enough.
	 *	If we have paused some vm_pageout_pause_max times without
	 *	adjusting vm_pageout_burst_wait, it might be too big,
	 *	so we decrease it.
	 */

	vm_page_lock_queues();
#if	NORMA_VM
#if	RESERVATION_COUNTERS
	RC(++c_vm_pageout_scan_continue);
	if (RESERVATION_AVAILABLE(PAGEOUT_RESERVE_EXTERNAL))
		RC(++c_vm_pageout_scan_continue_external);
	if (RESERVATION_AVAILABLE(PAGEOUT_RESERVE_INTERNAL))
		RC(++c_vm_pageout_scan_continue_internal);
#endif	/* RESERVATION_COUNTERS */
	/*
	 *	Take RESERVATION_LOCK here?  Doesn't seem worth it.  XXX
	 */
	if (RESERVATIONS(PAGEOUT_RESERVE_INTERNAL) > vm_pageout_burst_min)
#else	/* NORMA_VM */
	if (vm_page_laundry_count > vm_pageout_burst_min)
#endif	/* NORMA_VM */
	{
		vm_pageout_burst_wait++;
		vm_pageout_pause_count = 0;
	} else if (++vm_pageout_pause_count > vm_pageout_pause_max) {
		vm_pageout_burst_wait = (vm_pageout_burst_wait * 3) / 4;
		if (vm_pageout_burst_wait < 1)
			vm_pageout_burst_wait = 1;
		vm_pageout_pause_count = 0;
	}
	vm_page_unlock_queues();
}

/*
 *	vm_pageout is the high level pageout daemon.
 */

void
vm_pageout_continue(void)
{
	/*
	 *	The pageout daemon is never done, so loop forever.
	 *	We should call vm_pageout_scan at least once each
	 *	time we are woken, even if vm_page_free_wanted is
	 *	zero, to check vm_page_free_target and
	 *	vm_page_inactive_target.
	 */
	for (;;) {
		vm_pageout_scan();
		/* we hold vm_page_queue_free_lock now */
		assert(vm_page_free_wanted == 0);
		assert_wait((int) &vm_page_free_wanted, FALSE);
		mutex_unlock(&vm_page_queue_free_lock);
		counter(c_vm_pageout_block++);
		thread_block((void (*)(void)) 0);
	}
}

void
vm_pageout(void)
{
	thread_t	thread;
	processor_set_t	pset;
	int		free_after_reserve;
        kern_return_t                   ret;
        policy_base_t                   base;
        policy_limit_t                  limit;
        policy_fifo_base_data_t         fifo_base;
        policy_fifo_limit_data_t        fifo_limit;
#if	NORMA_VM
	pending_record	*p;
#endif	/* NORMA_VM */

        /*
         * Set thread privileges.
         */
	thread = current_thread();
        thread->vm_privilege = TRUE;
        stack_privilege(thread);

	/*
	 * Set thread scheduling priority and policy.
	 */
	pset = thread->processor_set;
        base = (policy_base_t) &fifo_base;
        limit = (policy_limit_t) &fifo_limit;
        fifo_base.base_priority = BASEPRI_SYSTEM;
        fifo_limit.max_priority = BASEPRI_SYSTEM;
        ret = thread_set_policy(thread->top_act, pset, POLICY_FIFO, 
				base, POLICY_TIMESHARE_BASE_COUNT, 
				limit, POLICY_TIMESHARE_LIMIT_COUNT);
        if (ret != KERN_SUCCESS)
                printf("WARNING: vm_pageout_thread is being TIMESHARED!\n");

	/*
	 *	Initialize some paging parameters.
	 */

#if	NORMA_VM
	if (vm_pageout_reservation_max[PAGEOUT_RESERVE_INTERNAL] == 0)
		vm_pageout_reservation_max[PAGEOUT_RESERVE_INTERNAL] =
			VM_PAGEOUT_INTERNAL_RESERVATION_MAX;
	if (vm_pageout_reservation_max[PAGEOUT_RESERVE_EXTERNAL] == 0)
		vm_pageout_reservation_max[PAGEOUT_RESERVE_EXTERNAL] =
			VM_PAGEOUT_EXTERNAL_RESERVATION_MAX;
	if (vm_pageout_reservation_max[PAGEOUT_RESERVE_LOCAL] == 0)
		vm_pageout_reservation_max[PAGEOUT_RESERVE_LOCAL] =
			VM_PAGEOUT_LOCAL_RESERVATION_MAX;
	if (vm_pageout_reservation_max[PAGEOUT_RESERVE_LOCK_REQUEST] == 0)
		vm_pageout_reservation_max[PAGEOUT_RESERVE_LOCK_REQUEST] =
			VM_PAGEOUT_LOCK_REQUEST_RESERVATION_MAX;
	if (vm_pageout_reservation_max[PAGEOUT_RESERVE_SYNCHRONIZE] == 0)
		vm_pageout_reservation_max[PAGEOUT_RESERVE_SYNCHRONIZE] =
			VM_PAGEOUT_SYNCHRONIZE_RESERVATION_MAX;

	for (p = vm_pageout_pending;
	     p < vm_pageout_pending + vm_pageout_pending_max;
	     ++p)
       		p->type = PAGEOUT_RESERVE_FREE;
#endif

	if (vm_pageout_burst_max == 0)
		vm_pageout_burst_max = VM_PAGEOUT_BURST_MAX;

	if (vm_pageout_burst_min == 0)
		vm_pageout_burst_min = VM_PAGEOUT_BURST_MIN;

	if (vm_pageout_burst_wait == 0)
		vm_pageout_burst_wait = VM_PAGEOUT_BURST_WAIT;

	if (vm_pageout_empty_wait == 0)
		vm_pageout_empty_wait = VM_PAGEOUT_EMPTY_WAIT;

	if (vm_page_free_reserved == 0)
		vm_page_free_reserved = VM_PAGE_FREE_RESERVED;

	if (vm_pageout_pause_max == 0)
		vm_pageout_pause_max = VM_PAGEOUT_PAUSE_MAX;

	if (vm_pageout_reserved_internal == 0)
		vm_pageout_reserved_internal =
			VM_PAGEOUT_RESERVED_INTERNAL(vm_page_free_reserved);

	if (vm_pageout_reserved_really == 0)
		vm_pageout_reserved_really =
			VM_PAGEOUT_RESERVED_REALLY(vm_page_free_reserved);

	free_after_reserve = vm_page_free_count - vm_page_free_reserved;

	if (vm_page_free_min == 0)
		vm_page_free_min = vm_page_free_reserved +
			VM_PAGE_FREE_MIN(free_after_reserve);

	if (vm_page_free_target == 0)
		vm_page_free_target = vm_page_free_reserved +
			VM_PAGE_FREE_TARGET(free_after_reserve);

	if (vm_page_free_target < vm_page_free_min + 5)
		vm_page_free_target = vm_page_free_min + 5;

#if	NORMA_VM
	mutex_init(&vm_pageout_reservation_lock);
#endif	/* NORMA_VM */

	/*
	 *	vm_pageout_scan will set vm_page_inactive_target.
	 */

	vm_pageout_continue();
	/*NOTREACHED*/
}

#if	NORMA_IPC
/* 
 * This routine is used by the norma code to decide if it can tell a node
 * to continue accepting messages, not just vm priority ones. It NEEDS to be
 * tuned.
 * Little thought as to what is really the right thing to do has be done.
 */

boolean_t
vm_netipc_able_continue_recv(void)
{
	return (vm_page_free_count > 
		(vm_page_free_target -
		((vm_page_free_target - vm_page_free_reserved)/2)));
}

#endif /* NORMA_IPC */

#if	MACH_KDB
#include <ddb/db_output.h>
#include <ddb/db_print.h>
#include <vm/vm_print.h>

#if	NORMA_VM

/*
 * Forward.
 */
void	db_rc_dump(
		char		*prompt,
		unsigned int	carray[]);

char	*pageout_type_str(
		int		type);

void
db_rc_dump(
	char		*prompt,
	unsigned int	carray[])
{
	int	x;

	db_printf(prompt);
	for (x = 0; x < PAGEOUT_RESERVE_MAX; ++x)
		db_printf(" %9d", carray[x]);
}


#define	DB_PENDING_PAGEOUT_MAX	10
int db_pending_pageout_max = DB_PENDING_PAGEOUT_MAX;

char *
pageout_type_str(
	int	type)
{
	char	*name;

	switch (type) {
	    case PAGEOUT_RESERVE_INTERNAL:
		name = "Internal";
		break;
	    case PAGEOUT_RESERVE_EXTERNAL:
		name = "External";
		break;
	    case PAGEOUT_RESERVE_LOCAL:
		name = "Local";
		break;
	    case PAGEOUT_RESERVE_LOCK_REQUEST:
		name = "Lock Request";
		break;
	    case PAGEOUT_RESERVE_SYNCHRONIZE:
		name = "Synchronize";
		break;
	    default:
		name = "UNKNOWN";
		break;
	}
	return name;
}
#endif /* NORMA_VM */

void
db_vm(void)
{
#if	NORMA_VM
	pending_record	*p;
	int		type, count;
#endif
	extern int vm_page_gobble_count;

	db_printf("VM Statistics:\n");
	db_printf("    pages:\n");
	db_printf("\tactiv %5d  inact %5d  free  %5d",
		  vm_page_active_count, vm_page_inactive_count,
		  vm_page_free_count);
	db_printf("\twire  %5d  gobbl %5d\n",
		  vm_page_wire_count, vm_page_gobble_count);
	db_printf("\tlaund %5d\n", vm_page_laundry_count);

	db_printf("   target:\n");
	db_printf("\tmin   %5d  inact %5d  free   %5d",
		vm_page_free_min, vm_page_inactive_target,
		vm_page_free_target);
	db_printf("\tresrv %5d\n", vm_page_free_reserved);

	db_printf("    burst:\n");
	db_printf("\tmax   %5d  min   %5d  wait  %5d   empty %5d\n",
		  vm_pageout_burst_max, vm_pageout_burst_min,
		  vm_pageout_burst_wait, vm_pageout_empty_wait);

	db_printf("    pause:\n");
	db_printf("\tcount %5d  max   %5d\n",
		  vm_pageout_pause_count, vm_pageout_pause_max);
#if	NORMA_VM
	db_printf("reservations:\n\t\t");
	db_printf("Internal  External     Local      Lock      Sync\n");
	db_rc_dump("    current   ", vm_pageout_reservations);
	db_rc_dump("\n    maximum   ", vm_pageout_reservation_max);
	db_rc_dump("\n    needed    ", vm_pageout_reservation_needed);
	db_printf("\n");
#if	RESERVATION_COUNTERS
	db_rc_dump("    granted   ", c_vm_pageout_reserve_granted);
	db_rc_dump("\n    completed ", c_vm_pageout_completed);
	db_rc_dump("\n    denied    ", c_vm_pageout_reserve_denied);
	db_rc_dump("\n    terminated", c_vm_pageout_terminated);
#endif	/* RESERVATION_COUNTERS */
	db_printf("\n");
	for (type = count = 0; type < PAGEOUT_RESERVE_MAX; ++type)
		count += vm_pageout_reservations[type];
	if (count > 0) {
		if (count > db_pending_pageout_max)
			count = db_pending_pageout_max;
		db_printf("    Table:     Pager/Uid/Kmsgs");
		db_printf("     Object     Offset         Type\n");
		for (p = vm_pageout_pending;
		     p < vm_pageout_pending + vm_pageout_pending_max &&
		     count > 0;
		     ++p) {
			if (p->type != PAGEOUT_RESERVE_FREE) {
				db_printf("      0x%x/0x%x/%4d",
					  p->pager, p->pager->ip_norma_uid,
					  db_port_kmsg_count(p->pager));
				db_printf(" 0x%8x 0x%8x %12s\n",
					  p->object,
					  p->offset,
					  pageout_type_str(p->type));
				--count;
			}
		}
	}
#if	RESERVATION_COUNTERS
	db_printf("scan_continue\tint %8d\text %8d\tcall %8d\n",
		  c_vm_pageout_scan_continue_internal,
		  c_vm_pageout_scan_continue_external,
		  c_vm_pageout_scan_continue);
#endif	/* RESERVATION_COUNTERS */
#endif	/* NORMA_VM */
}
#endif	/* MACH_KDB */
