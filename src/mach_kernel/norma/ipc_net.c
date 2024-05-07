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
 *
 * Revision 2.10.2.11  92/09/15  17:33:53  jeffreyh
 * 	Add debugging assert
 * 	[92/07/02            jeffreyh]
 * 
 * 	Use pageable copy objects for large NORMA messages.
 * 	Small messages continue to be handled using page list
 * 	technology at interrupt level -- in theory, minimizing
 * 	latency.  Pages belonging to messages larger than a
 * 	cutoff limit are sent to the netipc_thread for reassembly.
 * 	[92/06/02            alanl]
 * 
 * Revision 2.10.2.10  92/06/24  18:01:04  jeffreyh
 * 	Changed netipc_reserve_page to turn down packets from all
 * 	if memory gets short.
 * 
 * 	Moved check for vm_privilege to top of netipc_replenish to stop
 * 	netipc_output_replenish from being run by the default pager. It
 * 	has caused deadlocks.
 * 	[92/06/10            jeffreyh]
 * 
 * Revision 2.10.2.9  92/05/27  00:49:26  jeffreyh
 * 	Fixed argument to thread_wakeup_one in netipc_page_grab
 * 	[92/05/11            jeffreyh]
 * 
 * Revision 2.10.2.8.1.1  92/05/08  10:43:50  jeffreyh
 * 	Implement reservation system for netipc_page_list memory.
 * 	Users first reserve a page then allocate the page using
 * 	netipc_page_grab.  Reservations can be cancelled.  If a
 * 	reservation fails, special-case allocation can be done
 * 	using a forcible reservation.  If memory runs low, additional
 * 	pages can be stolen from the VM system's reserved page pool.
 * 	When returning memory to the netipc_page_list, excess pages
 * 	will be returned to the VM system.
 * 	[92/05/07            alanl]
 * 
 * Revision 2.10.2.8  92/04/09  14:24:17  jeffreyh
 * 	Do not set ch_timestamp on machines other then iPSC860
 * 
 * Revision 2.10.2.7  92/04/08  15:45:45  jeffreyh
 * 	Removed i860 debugging ifdefs.  Cleaned up history buffers.
 * 	[92/04/08            andyp]
 * 
 * Revision 2.10.2.6  92/03/28  10:11:43  jeffreyh
 * 	Have netipc thread clean up undeliverable kmsgs.
 * 	[92/03/20  14:12:41  jsb]
 * 
 * Revision 2.10.2.5  92/02/21  11:24:34  jsb
 * 	Moved spl{on,off} definitions earlier in the file (before all uses).
 * 	[92/02/18  08:03:01  jsb]
 * 
 * 	Removed accidently included lint declarations of panic, etc.
 * 	[92/02/16  11:17:48  jsb]
 * 
 * 	Eliminated netipc_thread_wakeup/netipc_replenish race.
 * 	[92/02/09  14:16:24  jsb]
 * 
 * Revision 2.10.2.4  92/02/18  19:14:43  jeffreyh
 * 	[intel] added support for callhere debug option of iPSC.
 * 	[92/02/13  13:02:21  jeffreyh]
 * 
 * Revision 2.10.2.3  92/01/21  21:51:30  jsb
 * 	From sjs@osf.org: moved node_incarnation declaration here from
 * 	ipc_ether.c.
 * 	[92/01/17  14:37:03  jsb]
 * 
 * 	New implementation of netipc lock routines which uses sploff/splon
 * 	and which releases spl before calling interrupt handlers (but after
 * 	taking netipc lock).
 * 	[92/01/16  22:20:30  jsb]
 * 
 * 	Removed panic in netipc_copy_grab: callers can now deal with failure.
 * 	[92/01/14  21:59:10  jsb]
 * 
 * 	In netipc_drain_intr_request, decrement request counts before calling
 * 	interrupt routines, not after. This preserves assertion that there is
 * 	at most one outstanding send or receive interrupt.
 * 	[92/01/13  20:17:18  jsb]
 * 
 * 	De-linted.
 * 	[92/01/13  10:15:50  jsb]
 * 
 * 	Now contains locking, allocation, and debugging printf routines.
 * 	Purged log.
 * 	[92/01/11  17:38:28  jsb]
 * 
 */ 
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1992 Carnegie Mellon University
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
 *	File:	norma/ipc_net.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Routines for reliable delivery and flow control for NORMA_IPC.
 */

#include <kern/spl.h>
#include <kern/misc_protos.h>
#include <mach/mach_host_server.h>
#include <vm/vm_pageout.h>
#include <norma/ipc_net.h>
#include <norma/ipc_node.h>
#include <norma/tr.h>

#if	CBUS
#include	<kern/processor.h>
#include	<cbus/cbus.h>
#endif	/* CBUS */

unsigned long node_incarnation = 1;		/* should never be zero */

spinlock_t netipc_lock;

thread_t netipc_lock_owner = THREAD_NULL;
#define	THREAD_INTR	((thread_t) 1)

int send_intr_request = 0;
int recv_intr_request = 0;
int timeout_intr_request = 0;
int resume_intr_request = 0;
spinlock_t netipc_intr_request_lock;

#if	iPSC386 || iPSC860
#if	iPSC860
#include <i860ipsc/nodehw.h>
#endif	/* iPSC860 */
extern void	netipc_called_here();
#endif	/* iPSC386 || iPSC860 */

/*
 * Forward.
 */
void			netipc_page_put_reserved(
				vm_page_t 	m);

#if	MACH_ASSERT
static boolean_t	netipc_page_list_state_valid(void);
#endif

/*
 * Called with interrupts not explicitly disabled but with lock held.
 * Returns with interrupts as they were and with lock released.
 *
 * Right now, there is only one thread to handle all events. This thread
 * should not block, because no other event could be taken into account
 * during that time. More threads may be added safely, but I am not sure
 * that anything could be won, except the fact that these threads may block
 * (but not the last one, since there should be always at least one running
 * thread).
 */
void
netipc_drain_intr_request(void)
{
	thread_t			thread;
	processor_set_t			pset;
	kern_return_t			ret;
	policy_base_t			base;
	policy_limit_t			limit;
	policy_fifo_base_data_t		fifo_base;
	policy_fifo_limit_data_t	fifo_limit;
	int 				s;
	netipc_netarray_t		np;

	/*
	 * Set thread privileges.
	 */
	thread = current_thread();
	thread_wire(&realhost, thread->top_act, TRUE);
	stack_privilege(thread);

#if	CBUS
	/*
	 * XXX TEMPORARY HACK. Since it is impossible to start I/O from
	 * an AST run on an other processor than the master_processor,
	 * the thread which starts the AST needs to be bound on the
	 * master_processor.
	 */
	thread_bind(thread, master_processor);
#endif	/* CBUS */

	/*
	 * Set thread priority and scheduling policy.
	 */
	pset = thread->processor_set;
	base = (policy_base_t) &fifo_base; 
	limit = (policy_limit_t) &fifo_limit;
	fifo_base.base_priority = BASEPRI_KERNEL+2;
	fifo_limit.max_priority = BASEPRI_KERNEL+2;
	ret = thread_set_policy(thread->top_act, pset, POLICY_FIFO,
				base, POLICY_FIFO_BASE_COUNT,
				limit, POLICY_FIFO_LIMIT_COUNT);
	if (ret != KERN_SUCCESS)
		printf("WARNING: netipc_drain_thread is being TIMESHARED!\n");

	for (;;) {

	    s = splimp();
	    spinlock_lock(&netipc_intr_request_lock);
	    while (send_intr_request > 0 ||
	       recv_intr_request > 0 ||
	       timeout_intr_request > 0 ||
	       resume_intr_request > 0) {
		/*
		 * Send and receive interrupts are counting interrupts.
		 * Many timeout interrupts map into one.
		 */
		if (send_intr_request > 0) {
			send_intr_request--;
			spinlock_unlock(&netipc_intr_request_lock);
			splx(s);
			netipc_thread_lock();
			_netipc_send_intr();

		} else if (recv_intr_request > 0) {
			recv_intr_request--;
			spinlock_unlock(&netipc_intr_request_lock);
			netipc_netarray_lock();
			queue_iterate(&netarray_busy, np,
				      netipc_netarray_t, queue) {
				netipc_array_lock(np);
				if (np->state == NETARRAY_COMPLETED) {
					np->state = NETARRAY_BUSY;
					netipc_array_unlock(np);
					break;
				}
				netipc_array_unlock(np);
			}
			assert(!queue_end(&netarray_busy, (queue_entry_t)np));
			netipc_netarray_unlock();
			splx(s);
			assert(np != (struct netarray *)0);
			netipc_thread_lock();
			_netipc_recv_intr(np);

		} else if (timeout_intr_request > 0)  {
			timeout_intr_request = 0;
			spinlock_unlock(&netipc_intr_request_lock);
			splx(s);
			netipc_thread_lock();
			_netipc_timeout_intr();

		} else {
			assert(resume_intr_request > 0);
			resume_intr_request = 0;
			spinlock_unlock(&netipc_intr_request_lock);
			splx(s);
			netipc_thread_lock();
			netipc_resume_timeout();
		}
		netipc_thread_unlock();
		s = splimp();
		spinlock_lock(&netipc_intr_request_lock);
	    }
	    assert_wait((int)netipc_drain_intr_request, FALSE);
	    spinlock_unlock(&netipc_intr_request_lock);
	    splx(s);
	    thread_block((void (*)(void)) 0);
      }
      /*NOTREACHED*/
}


void
netipc_send_intr(void)
{
	int s;

	s = splimp();
	spinlock_lock(&netipc_intr_request_lock);
	send_intr_request++;
	thread_wakeup_one((int)netipc_drain_intr_request);
	spinlock_unlock(&netipc_intr_request_lock);
	splx(s);
}

void
netipc_recv_intr(void)
{
	int s;

	s = splimp();
	spinlock_lock(&netipc_intr_request_lock);
	recv_intr_request++;
	thread_wakeup_one((int)netipc_drain_intr_request);
	spinlock_unlock(&netipc_intr_request_lock);
	splx(s);
}

void
netipc_timeout_intr(void)
{
	int s;

	s = splimp();
	spinlock_lock(&netipc_intr_request_lock);
	timeout_intr_request = 1;
	thread_wakeup_one((int)netipc_drain_intr_request);
	spinlock_unlock(&netipc_intr_request_lock);
	splx(s);
}

void
netipc_resume_intr(void)
{
	int s;

	s = splimp();
	spinlock_lock(&netipc_intr_request_lock);
	resume_intr_request = 1;
	thread_wakeup_one((int)netipc_drain_intr_request);
	spinlock_unlock(&netipc_intr_request_lock);
	splx(s);
}

zone_t		netarray_zone;
boolean_t	netarray_wanted;	/* a rcv packet got no buffers */
unsigned	netarray_count;		/* cur # of rcv fragment buffers */
unsigned	netarray_max;		/* max # of rcv fragment buffers */
queue_head_t	netarray_free;		/* rcv buffers free list */
queue_head_t	netarray_busy;		/* rcv buffers busy list */
spinlock_t	netipc_netarray_lock;

unsigned	netarray_pages;		/* cur # of reserved rcv pages */
unsigned	netarray_min;		/* min # of reserved rcv pages */
static spinlock_t	netipc_netarray_page_lock;

/*
 * netipc_netarray management routines.
 */
void
netipc_netarray_init_max(void)
{
	unsigned i;
	int s;

	/*
	 * If #nodes < NUM_NETARRAY_INIT, then netipc_netarray = #nodes.
	 * If #nodes > NUM_NETARRAY_INIT, then netipc_netarray =
	 *	NUM_NETARRAY_INIT + sqrt(#nodes - NUM_NETARRAY_INIT).
	 *
	 * Don't forget to multiply by two, since we are allowed to receive
	 * a control packet and may be a message packet, but nothing after
	 * a message packet.
	 */
	s = splimp();
	spinlock_lock(&netipc_netarray_lock);
	if (node_is_valid(NUM_NETARRAY_INIT)) {
		netarray_max = NUM_NETARRAY_INIT - 1;
		while (!node_is_valid(netarray_max))
			netarray_max--;
	} else {
		netarray_max = NUM_NETARRAY_INIT;
		for (i = 1; node_is_valid(i * i + NUM_NETARRAY_INIT - 1); i++)
			netarray_max++;
	}
	netarray_max *= 2;
	spinlock_unlock(&netipc_netarray_lock);
	splx(s);
}

netipc_netarray_t
netipc_netarray_allocate(
	boolean_t	can_block)
{
	netipc_netarray_t np;
	unsigned i;

	if (can_block)
		np = (netipc_netarray_t)zalloc(netarray_zone);
	else {
		np = (netipc_netarray_t)zget(netarray_zone);
		if (np == (netipc_netarray_t)0)
			return (netipc_netarray_t)0;
	}
	np->hdr.type = NETIPC_TYPE_INVALID;
	np->vec[0].addr = KVTODEV(&np->hdr);
	np->vec[0].size = sizeof(struct netipc_hdr);
	np->vec[1].size = sizeof(np->data);
	np->vec[1].addr = KVTODEV(&np->data);
	np->page[0] = VM_PAGE_NULL;
	np->drop = (boolean_t (*)(netipc_netarray_t))0;
	for (i = 2; i < NUM_NETARRAY_VEC; i++) {
		np->vec[i].size = 0;
		np->page[i-1] = VM_PAGE_NULL;
	}
	np->nvec = NUM_NETARRAY_VEC;
	netipc_array_lock_init(np);
	return np;
}

void
netipc_netarray_drop(
	netipc_netarray_t	np)
{
	unsigned i;
	int s;

	if (np->drop && (*np->drop)(np))
		return;

	s = splimp();
	netipc_array_lock(np);
	assert(np->state == NETARRAY_BUSY || np->state == NETARRAY_ERROR);

	if (np->page[0] != VM_PAGE_NULL) {
		spinlock_lock(&netipc_netarray_page_lock);
		if (netarray_pages < netarray_min) {
			netipc_page_put_reserved(np->page[0]);
			netarray_pages++;
		} else
			netipc_page_put(np->page[0], FALSE,
					"netipc_netarray_drop");
		np->page[0] = VM_PAGE_NULL;
		np->vec[1].size = sizeof(np->data);
		np->vec[1].addr = KVTODEV(&np->data);

		for (i = 2; i < np->nvec; i++) {
			if (np->page[i-1] != VM_PAGE_NULL) {
				if (netarray_pages < netarray_min) {
					netipc_page_put_reserved(np->page[i-1]);
					netarray_pages++;
				} else
					netipc_page_put(np->page[i-1], FALSE,
							"netipc_netarray_drop");
				np->page[i-1] = VM_PAGE_NULL;
			}
			np->vec[i].size = 0;
		}
		spinlock_unlock(&netipc_netarray_page_lock);
	}
	netipc_array_unlock(np);

	netipc_netarray_lock();
	queue_remove(&netarray_busy, np, netipc_netarray_t, queue);
	queue_enter(&netarray_free, np, netipc_netarray_t, queue);
	netipc_netarray_unlock();
	splx(s);
}

/*
 * Allocate a page for reception at index.
 */
boolean_t
netipc_netarray_page(
	netipc_netarray_t	np,
	unsigned		index)
{
	vm_page_t m;

	assert(index > 0 && index < np->nvec);
	assert(np->page[index-1] == VM_PAGE_NULL);

	spinlock_lock(&netipc_netarray_page_lock);
	if (netarray_pages > 0)
		netarray_pages--;
	else if (!netipc_reserve_page()) {
		spinlock_unlock(&netipc_netarray_page_lock);
		return FALSE;
	}
	spinlock_unlock(&netipc_netarray_page_lock);

	m = netipc_page_grab();
	assert(m != VM_PAGE_NULL);
	np->page[index-1] = m;
	np->vec[index].size = PAGE_SIZE;
	np->vec[index].addr = VPTODEV(m);
	return TRUE;
}

boolean_t	netipc_thread_awake = FALSE;
boolean_t	netipc_thread_reawaken = FALSE;
int		netipc_thread_awaken = 0;
spinlock_t	netipc_thread_awake_lock;

/*
 *	The netipc_page_list contains pages set aside
 *	for the netipc system.  By using a separate list,
 *	pages can be allocated at interrupt level.
 *
 *	The list has three important levels:
 *		list_high	maximum number of pages allowed on list
 *		list_low	the system attempts to keep at least
 *				this many pages on the list
 *		list_fallback_limit
 *				minimum number of pages kept around
 *				to satisfy special requests
 *
 *	These counters, adjusted dynamically, are also important:
 *		list_avail	number of pages not reserved
 *		list_count	number of pages on the list
 *
 *	In normal operation, pages are "reserved" from the
 *	list and then actually allocated from the list at a
 *	future time.  (Alternately, the reservation may be
 *	cancelled.)  A normal reservation will be denied
 *	if there is no page available to satisfy it.  A forced
 *	reservation causes the system to take the fallback
 *	pool into account before denying a request.
 *
 *	The fallback pool exists for handling special messages.
 *	These messages are critical to the pageout paths both
 *	for the local node and for remote nodes.  See the function
 *	ipc_unreliable.c:netipc_special_kmsg() for details.
 *
 *	In normal operation, the list is replenished with pages
 *	returned from previous allocation requests or by pages
 *	fed onto the list by the netipc_thread.  When memory is
 *	low, we use the kserver_pageout_support_thread to trickle
 *	enough pages back onto the list so that fallback requests
 *	can be granted.
 *
 *	The values for list_low and list_high should take
 *	into account the number of pages set aside for the
 *	fallback pool.
 *
 *	The entire set of list-related variables is protected by
 *	a single simple lock, the netipc_page_list_lock.  This lock
 *	must always be taken with interrupts disabled.  I have
 *	chosen sploff (splhigh) but only because the rest of the netipc
 *	system seems to run this way; I would have preferred splnet.
 *
 *	netipc_page_list_{low,high,refill} will be bootime adjusted if the
 *	current node is the root of a paging tree.
 */

vm_page_t		netipc_page_list = VM_PAGE_NULL;
int			netipc_page_list_count = 0;
int			netipc_page_list_avail = 0;
int			netipc_page_list_fallbacks = 3;
int			netipc_page_list_refill = 23;
int			netipc_page_list_owed = 0;
int			netipc_page_list_low = 23;
int			netipc_page_list_high = 33;
int			netipc_page_list_extra = 0;
boolean_t		netipc_page_special_only = FALSE;
static spinlock_t	netipc_page_list_lock;

/*
 *	Counters for netipc page allocation subsystem.
 *	A denied request is one rejected while there were
 *	still pages left in the fallback pool.  The impossible
 *	counter tallies attempts that failed because no pages
 *	at all were left on the list.
 */
int			c_netipc_reserve_denied = 0;
int			c_netipc_reserve_impossible = 0;
int			c_netipc_page_list_borrowed = 0;
int			c_netipc_page_list_returned = 0;
int			c_netipc_replenishments = 0;
int			c_netipc_grabs = 0;
int			c_netipc_ungrabs = 0;
int			c_netipc_replenish_fallbacks = 0;

vm_map_copy_t		netipc_vm_map_copy_list = VM_MAP_COPY_NULL;
int			netipc_vm_map_copy_count = 0;
static spinlock_t	netipc_vm_map_list_lock;

/*
 *	Add or return a page to the netipc_page_list pool.
 *	Detect transitions from empty list to fallbacks available
 *	and from fallbacks available to general pages available;
 *	there may be netipc unacks pending if allocation requests
 *	failed while memory was low.
 *
 *	This function also handles the case when someone attempts
 *	to add a page to the list and exceed the highwater mark.
 *	This problem can arise on multiprocessors, when the thread
 *	refilling the list races with other processors that may be
 *	returning previously allocated pages to the list.
 *	Four choices:
 *		- force all callers to do grody things like check
 *		the list variables under lock (which has to be dropped
 *		across calls into the VM system!), and maybe return
 *		pages to the VM system if the variables change
 *		- have netipc_page_put reject pages that exceed the
 *		highwater mark (caller responsible for freeing page)
 *		- have netipc_page_put return excess pages to the VM system
 *		- have netipc_page_put adjust the highwater mark
 *
 *	We implement the third option, returning excess pages to the
 *	VM system.  Also, netipc_page_put counts how many fallback pages
 *	are added to the netipc_page_list by borrowing from the reserved
 *	VM pool.  When the fallback pool has been repopulated, additional
 *	pages are returned to the VM system.
 */
void
netipc_page_put(
	vm_page_t 	m,
	boolean_t	borrowed_reserve_vm,
	char		*caller)
{
	boolean_t	resume_connections;
	int		s;
	TR_DECL("netipc_page_put");

	s = splimp();
	spinlock_lock(&netipc_page_list_lock);
	assert(netipc_page_list_state_valid());
	assert(!m->tabled);

	if (borrowed_reserve_vm == TRUE) {
		tr5("BORROWED count 0x%x owed 0x%x avail 0x%x caller %s",
		    netipc_page_list_count, netipc_page_list_owed,
		    netipc_page_list_avail, caller);
	} else {
		tr5("count 0x%x owed 0x%x avail 0x%x caller %s",
		    netipc_page_list_count, netipc_page_list_owed,
		    netipc_page_list_avail, caller);
	}

	/*
	 *	1.  Account for fallback pages being added to the list.
	 *	These pages were acquired by kserver_pageout_support_thread.
	 */
	if (borrowed_reserve_vm == TRUE) {
		++netipc_page_list_owed;
		++c_netipc_page_list_borrowed;
	}

	/*
	 *	2.  If we borrowed from the reserved VM pool, or if somehow
	 *	we are trying to stuff pages onto the netipc_page_list beyond
	 *	the highwater mark, return the page to the VM system.
	 *	We will return borrowed pages to the system in preference to
	 *	adding pages beyond the fallback limit.  Thus, memory in the
	 *	netipc_page_list will remain tight (and we will continue
	 *	rejecting all but special-case messages) until the overall VM
	 *	situation improves.
	 */
	if ((netipc_page_list_owed > 0 &&
	    netipc_page_list_count >= netipc_page_list_fallbacks)
	    || netipc_page_list_count >= netipc_page_list_high) {
		if (netipc_page_list_owed)
			--netipc_page_list_owed;
		if (netipc_page_list_extra)
			--netipc_page_list_extra;
		spinlock_unlock(&netipc_page_list_lock);
		splx(s);
		vm_page_free(m);
		++c_netipc_page_list_returned;
		return;
	}

	/*
	 *	3.  The page really should be saved.
	 */
	* (vm_page_t *) &m->pageq.next = netipc_page_list;
	netipc_page_list = m;

	/*
	 *	4.  On transitions from pool empty to pool non-empty,
	 *	and from only fallback pages available to at least one
	 *	general page available, process any pending unacks.
	 */
	++netipc_page_list_avail;
	++netipc_page_list_count;

	assert(netipc_page_list_state_valid());
	spinlock_unlock(&netipc_page_list_lock);
	splx(s);
}

/*
 *	Add or return a page to the netipc_page_list pool
 *	which will be immediately reserved.
 */
void
netipc_page_put_reserved(
	vm_page_t 	m)
{
	int	s;

	s = splimp();
	spinlock_lock(&netipc_page_list_lock);
	assert(netipc_page_list_state_valid());
	assert(!m->tabled);

	if (netipc_page_list_count >= netipc_page_list_high) {
		if (netipc_page_list_avail > 0) {
			netipc_page_list_avail--;
			spinlock_unlock(&netipc_page_list_lock);
			splx(s);
			vm_page_free(m);
			++c_netipc_page_list_returned;
			return;
		}
		netipc_page_list_extra++;
	}

	* (vm_page_t *) &m->pageq.next = netipc_page_list;
	netipc_page_list = m;
	++netipc_page_list_count;

	spinlock_unlock(&netipc_page_list_lock);
	splx(s);
}

/*
 *	Reserve a page for future use.  The request
 *	will be granted if the number of pages available
 *	exceeds the number of pages reserved for special-
 *	case situations.  The reservation must be balanced
 *	later by a call to netipc_page_grab, to actually
 *	allocate the page from the list, or by a call to
 *	netipc_reserve_cancel, to give up the reservation.
 */
boolean_t
netipc_reserve_page(void)
{
	int	success;
	int	s;
	TR_DECL("netipc_reserve_page");

	s = splimp();
	spinlock_lock(&netipc_page_list_lock);
	assert(netipc_page_list_state_valid());
	tr5((netipc_page_special_only ? 
	     "SPEC %s avail 0x%x vmfree 0x%x wire 0x%x" :
	     "!spec %s avail 0x%x vmfree 0x%x wire 0x%x"),
	    netipc_able_continue_recv() ? "able" : "!ABLE",
	    netipc_page_list_avail, vm_page_free_count, vm_page_wire_count);

	/*
	 *	1.  Assume failure.
	 */
	success = FALSE;

	/*
	 *	2.  In low-memory situations, we reject all
	 *	incoming pages until memory has been freed.
	 *	Check for plentiful memory.
	 */
	if (netipc_page_special_only && netipc_able_continue_recv())
			netipc_page_special_only = FALSE;

	/*
	 *	3.  If memory seems to be generally available,
	 *	check for pages in the netipc page pool.  If we
	 *	can't find any pages in the pool, declare a
	 *	memory shortage and give up.
	 *
	 *	N.B.  The pool availability check, below, is duplicated
	 *	in netipc_page_put.
	 */
	if (netipc_page_special_only == FALSE) {
		if (netipc_page_list_avail > netipc_page_list_fallbacks) {
			--netipc_page_list_avail;
			success = TRUE;
		} else {
			++c_netipc_reserve_denied;
			netipc_page_special_only = TRUE;
		}
	} else {
		++c_netipc_reserve_denied;
	}

	spinlock_unlock(&netipc_page_list_lock);
	splx(s);
	return success;
}


/*
 * This function used to be just the vm_netipc_able_continue_recv
 * part.  This is not sufficient.  It is necessary to also check that
 * we have pages in the netipc_page_list or else we will just immediately
 * suspend again.
 */
boolean_t
netipc_able_continue_recv(void)
{
    return ((netipc_page_list_avail >= netipc_page_list_low) &&
	    vm_netipc_able_continue_recv());
}

/*
 *	Force a page reservation if at all possible.
 *	This call should be used only for reserving
 *	memory to permit processing messages of
 *	unusual priority.  In other words, messages
 *	critical to keeping the system running.
 */
boolean_t
netipc_reserve_page_force(void)
{
	int	success;
	int	s;
	TR_DECL("netipc_reserve_page_force");

	s = splimp();
	spinlock_lock(&netipc_page_list_lock);
	assert(netipc_page_list_state_valid());
	tr3("avail 0x%x vmp_free 0x%x", netipc_page_list_avail,
	    vm_page_free_count);

	if (netipc_page_list_avail > 0) {
		--netipc_page_list_avail;
		success = TRUE;
	} else {
		++c_netipc_reserve_impossible;
		success = FALSE;
	}

	spinlock_unlock(&netipc_page_list_lock);
	splx(s);
	return success;
}


/*
 *	Cancel a previously reserved page.
 */
void
netipc_reserve_cancel(void)
{
	int s;
	TR_DECL("netipc_reserve_cancel");

	s = splimp();
	spinlock_lock(&netipc_page_list_lock);
	tr2("avail 0x%x", netipc_page_list_avail);

	++netipc_page_list_avail;
	assert(netipc_page_list_state_valid());

	spinlock_unlock(&netipc_page_list_lock);
	splx(s);
	return;
}


#if	MACH_ASSERT
/*
 *	Debugging routine.  Check various pointers
 *	and counters to validate state of the
 *	netipc page subsystem.  Must be called
 *	while holding netipc_page_list_lock.
 */
static boolean_t
netipc_page_list_state_valid(void)
{
	assert(netipc_page_list_fallbacks > 0);
	assert(netipc_page_list_avail >= 0);
	assert(netipc_page_list_avail <= netipc_page_list_count);
	assert(netipc_page_list_count >= 0);
	assert(netipc_page_list_count <=
	       netipc_page_list_high + netipc_page_list_extra);
	return TRUE;
}
#endif


/*
 *	Initialize state of netipc page subsystem.
 *	Note that there is no need to allocate pages
 *	for the subsystem here; the first invocation
 *	of netipc_relenish will do that.
 */
void
netipc_page_init(void)
{
	vm_page_t m;
	netipc_netarray_t np;
	unsigned i;
	int s;

	spinlock_init(&netipc_intr_request_lock);
	spinlock_init(&netipc_page_list_lock);
	spinlock_init(&netipc_vm_map_list_lock);
	spinlock_init(&netipc_thread_awake_lock);
	spinlock_init(&netipc_netarray_page_lock);
	queue_init(&netarray_busy);
	queue_init(&netarray_free);

	netipc_netarray_lock_init();
	netipc_netarray_init_max();
	netipc_thread_lock_init();

	netarray_zone = zinit(sizeof(netipc_netarray_data_t), PAGE_SIZE,
			      PAGE_SIZE, "netipc netarray");

	/*
	 * Initially allocate pages for at least one packet reception.
	 */
	for (i = 0; i < NUM_NETARRAY_VEC - 1; i++) {
		m = vm_page_grab();
		if (m == VM_PAGE_NULL)
			panic("Can't allocate initial receive pages\n");
		vm_page_init(m, m->phys_addr);
		vm_page_gobble(m); /* mark as consumed internally */
		netipc_page_put_reserved(m);
	}

	/*
	 * Initially allocate buffer for at least one packet reception.
	 */
	np = netipc_netarray_allocate(TRUE);
	if (np == (netipc_netarray_t)0)
		panic("Can't allocate initial receive buffer\n");

	s = splimp();
	spinlock_lock(&netipc_netarray_page_lock);
	netarray_min = netarray_pages = NUM_NETARRAY_VEC - 1;
	spinlock_unlock(&netipc_netarray_page_lock);
	netipc_netarray_lock();
	netarray_count = 1;
	queue_enter(&netarray_free, np, netipc_netarray_t, queue);
	netipc_netarray_unlock();
	splx(s);

	if (netipc_page_list_refill < netipc_page_list_low) {
		printf("netipc_page_init warning:  refill %d is lower",
		       netipc_page_list_refill);
		printf(" than low-water mark %d\n", netipc_page_list_low);
	}
}

vm_map_copy_t
netipc_copy_grab(void)
{
	vm_map_copy_t copy;

	spinlock_lock(&netipc_vm_map_list_lock);
	copy = netipc_vm_map_copy_list;
	if (copy != VM_MAP_COPY_NULL) {
		netipc_vm_map_copy_list = (vm_map_copy_t) copy->type;
		netipc_vm_map_copy_count--;
		c_netipc_grabs++;
		copy->type = VM_MAP_COPY_PAGE_LIST;
	}
	spinlock_unlock(&netipc_vm_map_list_lock);
	return copy;
}

void
netipc_copy_ungrab(
	vm_map_copy_t	copy)
{
	spinlock_lock(&netipc_vm_map_list_lock);
	copy->type = (int) netipc_vm_map_copy_list;
	netipc_vm_map_copy_list = copy;
	netipc_vm_map_copy_count++;
	c_netipc_ungrabs++;
	spinlock_unlock(&netipc_vm_map_list_lock);
}

void
netipc_thread_wakeup(void)
{
	int s;

	s = splimp();
	spinlock_lock(&netipc_thread_awake_lock);
	if (netipc_thread_awake) {
		netipc_thread_reawaken = TRUE;
	} else {
		thread_wakeup((int) &netipc_thread_awake);
	}
	spinlock_unlock(&netipc_thread_awake_lock);
	splx(s);
}

/*
 * XXX
 * The wakeup protocol for this loop is not quite correct...
 *
 * XXX
 * We should move the lists out all at once, not one elt at a time.
 */
void
netipc_replenish(
	boolean_t	always)
{
	vm_page_t	m;
	int		s;

	/*
	 *	Netipc_replenish could inadvertantly allocate
	 *	memory from the reserved VM pool if the function
	 *	is called by a vm-privileged thread.
	 */
	if (current_thread()->vm_privilege) {
		return;
	}

	netipc_output_replenish();	/* XXX move somewhere else */
	spinlock_lock(&netipc_vm_map_list_lock);
	while (netipc_vm_map_copy_count < 300) {
		vm_map_copy_t copy;

		c_netipc_replenishments++;
		copy = (vm_map_copy_t) zget(vm_map_copy_zone);
		if (copy == (vm_map_copy_t)0) {
			spinlock_unlock(&netipc_vm_map_list_lock);
			copy = (vm_map_copy_t) zalloc(vm_map_copy_zone);
			spinlock_lock(&netipc_vm_map_list_lock);
		}
		copy->type = (int) netipc_vm_map_copy_list;
		netipc_vm_map_copy_list = copy;
		netipc_vm_map_copy_count++;
	}
	spinlock_unlock(&netipc_vm_map_list_lock);

	/*
	 *	Cut corners here:  examine the list_count and list_high
	 *	variables without taking any locks.  Assumes that the
	 *	memory subsystem is well-behaved.  On a uniprocessor,
	 *	this is not a problem.  On a multiprocessor, even if we
	 *	did the right locking here we could still wind up
	 *	blowing extra pages into the pool while we raced other
	 *	processors returning previously allocated pages to the pool.
	 *	netipc_page_put handles this problem.
	 */
	s = splimp();
	spinlock_lock(&netipc_page_list_lock);
	while (netipc_page_list_count < netipc_page_list_high) {
		spinlock_unlock(&netipc_page_list_lock);
		splx(s);
		m = vm_page_grab();
		if (m == VM_PAGE_NULL) {
			s = splimp();
			spinlock_lock(&netipc_page_list_lock);
			break;
		}
		vm_page_init(m, m->phys_addr);
		vm_page_gobble(m); /* mark as consumed internally */
		netipc_page_put(m, FALSE, "netipc_replenish");
		s = splimp();
		spinlock_lock(&netipc_page_list_lock);
	}
	while (always && netipc_page_list_count < netipc_page_list_low) {
		spinlock_unlock(&netipc_page_list_lock);
		splx(s);
		while ((m = vm_page_grab()) == VM_PAGE_NULL) {
			VM_PAGE_WAIT();
		}
		vm_page_init(m, m->phys_addr);
		vm_page_gobble(m); /* mark as consumed internally */
		netipc_page_put(m, FALSE, "netipc_replenish2");
		s = splimp();
		spinlock_lock(&netipc_page_list_lock);
	}
	spinlock_unlock(&netipc_page_list_lock);
	splx(s);
}


/*
 *	This routine is called from the kserver_pageout_support_thread
 *	when the netipc_page_list is running so low it has even cut
 *	into the fallback page pool.  Because this routine is called
 *	from a thread with vm_privilege, be careful to only allocate
 *	as many pages as necessary to replenish the fallback pool.
 *
 *	XXX Concern:  what happens if pages get trapped in the pool and
 *	don't make it back to vm?
 */
void
netipc_replenish_fallbacks(void)
{
	vm_page_t	m;
	int		s;

	assert(current_thread()->vm_privilege);

	/*
	 * Allocate pages until the fallback pool has been restored
	 * to health.
	 *
	 * Cut corners:  don't bother locking around loop test.
	 */
	s = splimp();
	spinlock_lock(&netipc_page_list_lock);
	while (netipc_page_list_count < netipc_page_list_fallbacks) {
		spinlock_unlock(&netipc_page_list_lock);
		splx(s);
		++c_netipc_replenish_fallbacks;
		m = vm_page_grab();
		assert(m != VM_PAGE_NULL);
		vm_page_init(m, m->phys_addr);
		vm_page_gobble(m); /* mark as consumed internally */
		netipc_page_put(m, TRUE, "netipc_replenish_fallbacks");
		s = splimp();
		spinlock_lock(&netipc_page_list_lock);
	}
	spinlock_unlock(&netipc_page_list_lock);
	splx(s);
}


/*
 *	Grab a vm_page_t at interrupt level.
 *	Callers MUST have previously reserved a page!
 *	Given these rules, this routine should never fail.
 *
 *	If only fallback pages are left on the list, awaken
 *	the netipc_thread to replenish the list.  If even
 *	the fallback pages have vanished, kick-start the
 *	kserver_pageout_support_thread to replenish the list.
 *	The kserver_p_s_thread has vm_privilege.
 *
 *	XXX Waiting until the fallback pool is empty may
 *	not be a good idea.  On the other hand, allocating
 *	new fallback pages as soon as any of them disappear
 *	may not be a good idea, either.
 *
 *	Changed to add new refill threshold.
 */
vm_page_t
netipc_page_grab(void)
{
	vm_page_t	m;
	int		awaken_netipc_thread;
	int		awaken_kserver_thread;
	int		s;

	s = splimp();
	spinlock_lock(&netipc_page_list_lock);
	assert(netipc_page_list_state_valid());
	assert(netipc_page_list_count > 0);
	assert(netipc_page_list != VM_PAGE_NULL);
	assert(!netipc_page_list->tabled);

	m = netipc_page_list;
	netipc_page_list = (vm_page_t) m->pageq.next;
	--netipc_page_list_count;

	awaken_kserver_thread = (netipc_page_list_count == 0);
	awaken_netipc_thread = (netipc_page_list_count <=
				netipc_page_list_refill);

	spinlock_unlock(&netipc_page_list_lock);
	splx(s);
	if (awaken_kserver_thread)
		kserver_pageout_support_wakeup();
	if (awaken_netipc_thread)
		netipc_thread_wakeup();

	return m;
}


void
netipc_thread(void)
{
	int s;
	netipc_netarray_t np;

	thread_set_own_priority(BASEPRI_KERNEL+2);	/* high priority */

	while (TRUE) {
	    s = splimp();
	    spinlock_lock(&netipc_thread_awake_lock);
	    for (;;) {
		/*
		 * Record that we are awake.
		 * Look out for new awaken requests while we are out working.
		 */
		netipc_thread_awaken++;
		netipc_thread_awake = TRUE;
		netipc_thread_reawaken = FALSE;
		spinlock_unlock(&netipc_thread_awake_lock);
		splx(s);
		
		/*
		 * Call netipc_replenish to fill in pages.
		 */
		netipc_replenish(TRUE);

		/*
		 * Call norma_ipc_clean_receive_errors to clean up
		 * kmsgs that couldn't be delivered (interrupt and
		 * ast code can't do this).
		 */
		norma_ipc_clean_receive_errors();

		/*
		 * Add a new netipc_netarray if needed.
		 */
		s = splimp();
		netipc_netarray_lock();
		if (netarray_wanted) {
			assert(netarray_count < netarray_max);
			netipc_netarray_unlock();
			splx(s);
			np = netipc_netarray_allocate(TRUE);
			s = splimp();
			netipc_netarray_lock();
			if (np != (netipc_netarray_t)0) {
				queue_enter(&netarray_free, np,
					    netipc_netarray_t, queue);
				netarray_count++;
			}
			netarray_wanted = FALSE;
		}
		netipc_netarray_unlock();

		/*
		 *	If we don't yet have enough pages, or someone
		 *	came up with something new for us to do, then
		 *	do more work before going to sleep.
		 *
		 *	N.B.  Cutting corners:  not using a lock here.
		 *	Assumes memory architecture is kind.
		 */
		spinlock_lock(&netipc_thread_awake_lock);
		if (netipc_page_list_count < netipc_page_list_low ||
		    netipc_thread_reawaken) {
			continue;
		}

		/*
		 * Nothing left for us to do right now.  Go to sleep.
		 */
		netipc_thread_awake = FALSE;
		assert_wait((int) &netipc_thread_awake, FALSE);
		spinlock_unlock(&netipc_thread_awake_lock);
		splx(s);
		thread_block((void (*)(void)) 0);
	    }
	}
	/*NOTREACHED*/
}

int Noise0 = 0;	/* print netipc packets */	
int Noise1 = 0;	/* notification and migration debugging */
int Noise2 = 0;	/* synch and timeout printfs */
int Noise3 = 0;	/* copy object continuation debugging */
int Noise4 = 0;	/* multiple out-of-line section debugging */
int Noise5 = 0;	/* obsolete acks */
int Noise6 = 0;	/* short print of rcvd packets, including msgh_id */
int Noise7 = 0; /* short print of rcvd packets, including msgh_id */

/* VARARGS1 */
void
printf1(const char* fmt, ...)
{
	va_list	listp;

	if (Noise1) {
		va_start(listp, fmt);
		_doprnt(fmt, &listp, cnputc, 0);
		va_end(listp);
	}
}

/* VARARGS1 */
void
printf2(const char* fmt, ...)
{
	va_list	listp;

	if (Noise2) {
		va_start(listp, fmt);
		_doprnt(fmt, &listp, cnputc, 0);
		va_end(listp);
	}
}

/* VARARGS1 */
void
printf3(const char *fmt, ...)
{
	va_list	listp;

	if (Noise3) {
		va_start(listp, fmt);
		_doprnt(fmt, &listp, cnputc, 0);
		va_end(listp);
	}
}

/* VARARGS1 */
void
printf4(const char *fmt, ...)
{
	va_list	listp;

	if (Noise4) {
		va_start(listp, fmt);
		_doprnt(fmt, &listp, cnputc, 0);
		va_end(listp);
	}
}

/* VARARGS1 */
void
printf5(const char *fmt, ...)
{
	va_list	listp;

	if (Noise5) {
		va_start(listp, fmt);
		_doprnt(fmt, &listp, cnputc, 0);
		va_end(listp);
	}
}

/* VARARGS1 */
void
printf6(const char *fmt, ...)
{
	va_list	listp;

	if (Noise6) {
		va_start(listp, fmt);
		_doprnt(fmt, &listp, cnputc, 0);
		va_end(listp);
	}
}

/* VARARGS1 */
void
printf7(const char *fmt, ...)
{
	va_list	listp;

	if (Noise7) {
		va_start(listp, fmt);
		_doprnt(fmt, &listp, cnputc, 0);
		va_end(listp);
	}
}

int fff = 1;
int mmm = 0;

/* VARARGS1 */
void
fret(const char *fmt, ...)
{
	va_list	listp;

	if (fff) {
		va_start(listp, fmt);
		_doprnt(fmt, &listp, cnputc, 0);
		va_end(listp);
	}
}

/* VARARGS1 */
void
mumble(const char *fmt, ...)
{
	va_list	listp;

	if (mmm) {
		va_start(listp, fmt);
		_doprnt(fmt, &listp, cnputc, 0);
		va_end(listp);
	}
}

#if	iPSC386 || iPSC860 || PARAGON860
#define	MAX_CALLS	256

struct netipc_call_history {
	char		*ch_filename;
	char		*ch_notation;
	int		ch_lineno;
	unsigned long	ch_timestamp;
};
struct netipc_call_history netipc_call_history[MAX_CALLS];
int	called_here_next = 0;

void netipc_called_here(filename, line, notation)
	char	*filename, *notation;
	int	line;
{
	struct netipc_call_history *ch;

	ch = &netipc_call_history[called_here_next++];
	if (called_here_next >= MAX_CALLS) {
		called_here_next = 0;
	}
	ch->ch_filename = filename;
	ch->ch_notation = notation;
	ch->ch_lineno = line;
#if	iPSC860
        ch->ch_timestamp = (unsigned long) inb(COUNTER_PORT);
#endif
	netipc_call_history[called_here_next].ch_filename = 0;
}


void db_show_netipc_called_here()
{
	int	i, j;
	char	*s, *slash;
	struct netipc_call_history *ch;

	db_printf(" #   Line             File     When Note\n");
	j = called_here_next - 1;
	for (i = 0; i < MAX_CALLS; i++) {
		if (j < 0) {
			j = MAX_CALLS - 1;
		}
		ch = &netipc_call_history[j];
		if (ch->ch_filename) {
			slash = 0;
			for (s = ch->ch_filename; *s; s++) {
				if (*s == '/') {
					slash = s + 1;
				}
			}
			db_printf("%3d %5d %16s %8u %s\n",
				j,
				ch->ch_lineno,
				slash ? slash : ch->ch_filename,
				ch->ch_timestamp,
				ch->ch_notation);
			j--;
		} else {
			return;
		}
	}
}

#endif	/* iPSC386 || iPSC860 */
