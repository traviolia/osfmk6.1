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
 * Revision 2.9.2.8  92/09/15  17:33:36  jeffreyh
 * 	Change order of operations in kserver_pageout_support_thread to
 * 	release memory before we use it. [With alanl]
 * 	Added missing timeout call in functionkserver_pageout_support_thread
 * 	[92/08/24            jeffreyh]
 * 
 * 	Declare type for netipc_able_continue_recv().
 * 	With jeffreyh:  made kserver_pageout_support_thread
 * 	activated by a timeout to send resume queries from
 * 	suspended nodes to their oppressors.  Perhaps there
 * 	should be a separate thread for just this purpose.
 * 	[92/06/11            alanl]
 * 
 * Revision 2.9.2.7  92/06/24  18:00:42  jeffreyh
 * 	Added another function to the kserver_pageout_support
 * 	thread to send resume messages to waiting nodes
 * 	if enough memory is available.
 * 	[92/06/10            jeffreyh]
 * 
 * Revision 2.9.2.6  92/05/26  18:22:34  jeffreyh
 * 	From time to time, it is necessary to replenish the
 * 	netipc system's pages.  This is an urgent situation,
 * 	now handled by the kserver_pageout_support thread.
 * 	[92/05/07            alanl]
 * 
 * 	Enhanced the kserver_pullrecv_thread to handled two special
 * 	cases for NORMA pageout.  Case 1 is the old pullrecv case.
 * 	Case 2 scans the netipc_safe_vm_map_copy_discard_list.  Normally,
 * 	netipc_thread scans this list but netipc_thread can't run when
 * 	memory is low.  [Alanl and Jeffreyh.]
 * 	Remove deadlock from pageout path by handling pull_receive
 * 	requests in a separate thread.  See comments in code.
 * 	Also:  consistently use spl w.r.t. kernel_kmsg_lock to
 * 	prevent TLB shootdown deadlocks on multiprocessors.
 * 	[92/04/22            sjs]
 * 
 * 	Added bounds check to the number of kservers threads possible.
 * 	[92/04/16            sjs]
 * 
 * Revision 2.9.2.5  92/04/08  15:45:30  jeffreyh
 * 	Temporary debugging logic.
 * 	[92/04/06            dlb]
 * 
 * 	Added logic to make sure we never run out of kserver threads.
 * 	 This fixes migration problems among many hops.
 * 	[92/04/02            sjs]
 * 
 * Revision 2.9.2.4  92/02/21  11:24:20  jsb
 * 	In norma_kserver_deliver, don't convert reply to network format.
 * 	[92/02/21  09:04:29  jsb]
 * 
 * Revision 2.9.2.3  92/01/21  21:51:15  jsb
 * 	De-linted.
 * 	[92/01/17  12:20:44  jsb]
 * 
 * Revision 2.9.2.2  92/01/09  18:45:24  jsb
 * 	Added kernel_kmsg_lock. Use splhigh/splx instead of sploff/splon.
 * 	[92/01/08  10:03:53  jsb]
 * 
 * Revision 2.9.2.1  92/01/03  16:37:23  jsb
 * 	Corrected log.
 * 	[91/12/24  14:34:11  jsb]
 * 
 * Revision 2.9  91/12/15  10:42:15  jsb
 * 	Added norma_ipc_finish_receiving call to support large in-line msgs.
 * 
 * Revision 2.8  91/12/14  14:34:23  jsb
 * 	Removed ipc_fields.h hack.
 * 
 * Revision 2.7  91/12/10  13:26:03  jsb
 * 	Use ipc_kmsg_copyout_to_network instead of ipc_kmsg_copyin_from_kernel.
 * 	[91/12/10  11:27:15  jsb]
 * 
 * Revision 2.6  91/11/14  16:52:24  rpd
 * 	Added ipc_fields.h hack.
 *	Use IP_NORMA_IS_PROXY macro instead of ipc_space_remote.
 *	Added missing argument to kernel_thread().
 * 	[91/11/00            jsb]
 * 
 * Revision 2.5  91/08/28  11:16:03  jsb
 * 	As a hack to avoid printfs from i860ipsc/spl.c,
 * 	defined sploff/splon as splsched/splx.
 * 	[91/08/27  21:59:34  jsb]
 * 
 * 	Renamed clport things to norma things.
 * 	[91/08/15  09:11:36  jsb]
 * 
 * Revision 2.4  91/08/03  18:19:22  jsb
 * 	Replaced spldcm/splx with sploff/splon.
 * 	[91/07/28  20:52:22  jsb]
 * 
 * 	Removed obsolete includes and vm and kmsg munging operations.
 * 	[91/07/17  14:14:11  jsb]
 * 
 * 	Moved MACH_MSGH_BITS_COMPLEX_{PORTS,DATA} to mach/message.h.
 * 	[91/07/04  13:12:09  jsb]
 * 
 * 	Use vm_map_copy_t page_lists instead of old style page_lists.
 * 	[91/07/04  10:20:35  jsb]
 * 
 * Revision 2.3  91/07/01  08:25:30  jsb
 * 	Changes for new vm_map_copy_t definition.
 * 	[91/06/29  16:38:27  jsb]
 * 
 * Revision 2.2  91/06/17  15:47:44  jsb
 * 	Moved here from ipc/ipc_clkobject.c.
 * 	[91/06/17  11:05:35  jsb]
 * 
 * Revision 2.2  91/06/06  17:05:23  jsb
 * 	First checkin.
 * 	[91/05/24  13:10:00  jsb]
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
 *	File:	norma/ipc_kserver.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Functions to support ipc between nodes in a single Mach cluster.
 */

#include <machine/machparam.h>
#include <vm/vm_kern.h>
#include <vm/vm_page.h>
#include <mach/vm_param.h>
#include <mach/port.h>
#include <mach/message.h>
#include <mach/mach_host_server.h>
#include <mach/mach_server.h>
#include <kern/assert.h>
#include <kern/host.h>
#include <kern/sched_prim.h>
#include <kern/ipc_sched.h>
#include <kern/ipc_kobject.h>
#include <kern/zalloc.h>
#include <kern/misc_protos.h>
#include <ipc/ipc_mqueue.h>
#include <ipc/ipc_thread.h>
#include <ipc/ipc_kmsg.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_pset.h>
#include <ipc/ipc_space.h>
#include <norma/tr.h>

#define	NUM_KSERVER_THREADS		4	/* basic #threads */
#define	NUM_KSERVER_MP_THREADS		1	/* add log2(NCPU)/1 threads */
#define NUM_KSERVER_PAGEOUT_THREADS	1	/* basic #threads */
#define NUM_KSERVER_PAGEOUT_MP_THREADS	2	/* add log2(NCPU)/2 threads */
#define NUM_KSERVER_CRITICAL_THREADS	1	/* basic #threads */
#define NUM_KSERVER_CRITICAL_MP_THREADS	2	/* add log2(NCPU)/2 threads */

#define	NORMA_PROXY_PORT_QUEUE_LIMIT	1

/*
 * Problems:
 *	Kserver_awake should be used but isn't.
 *	We used to replenish here; should we still?
 *	Kernel_kmsg list is ugly; we should use a queue.
 */

/*
 * Locking.
 * kernel_kmsg_lock protects the following variables:
 *	kernel_kmsg		linked list of incoming messages
 *	kserver_awake
 *	kernel_pullrecv_kmsg	linked list of pullrecv msgs, only
 *	kserver_thread_count	count of active kserver threads
 */

spinlock_t kernel_kmsg_lock;
ipc_kmsg_t kernel_kmsg = (ipc_kmsg_t) 0;
ipc_kmsg_t kernel_kmsg_tail = (ipc_kmsg_t)0;

boolean_t kserver_awake = FALSE;
int kserver_awaken = 0;
int kserver_thread_count;
int kserver_thread_total;	/* count of kserver threads (debug) */
int kserver_thread_max = 64;	/* Max allowable kserver threads (debug) */
int kserver_thread_steady_state = 10;
int paging_critical_kmsg_count;	/* count of critical kmsgs */
boolean_t paging_critical_wakeup_needed = FALSE;

spinlock_t kserver_pageout_support_lock;
int kserver_pageout_support_needed;
int c_norma_drain_critical = 0;

/*
 * Forward.
 */
void		norma_kserver_deliver(
			ipc_kmsg_t	kmsg);

void		kserver_thread(void);

void		kserver_pageout_support_thread(void);

void		kserver_paging_critical_thread(void);

/*
 * Service request, perhaps blocking; send reply, if any.
 */
void
norma_kserver_deliver(
	ipc_kmsg_t	kmsg)
{
	ipc_port_t port;

	norma_ipc_finish_receiving(&kmsg);
	kmsg = ipc_kobject_server(kmsg);
	if (kmsg != IKM_NULL) {
		port = (ipc_port_t) kmsg->ikm_header.msgh_remote_port;
		ip_lock(port);
		if (! IP_NORMA_IS_PROXY(port)) {
			ip_unlock(port);
			ipc_mqueue_send_always(kmsg);
		} else if (norma_ipc_send(kmsg
#if	NORMA_PROXY_PORT_QUEUE_LIMIT
					  , MACH_SEND_ALWAYS, 0
#endif
					  ) == KERN_NOT_RECEIVER) {
			ipc_mqueue_send_always(kmsg);
		}
	}
}

unsigned int	c_kserver_activations = 0;
unsigned int	c_kserver_disappointed = 0;


void
kserver_thread(void)
{
	ipc_kmsg_t	kmsg;
	ipc_kmsg_t	*prev_ptr;
	ipc_kmsg_t	fkmsg;
	ipc_port_t	dest_port;
	int		no_work = 1;

	thread_set_own_priority(BASEPRI_KERNEL+3);

	for (;;) {
		kserver_awaken++;
		++c_kserver_activations;

		spinlock_lock(&kernel_kmsg_lock);
		while (kernel_kmsg) {
			/*
			 * If we manage to exhaust all of the threads,
			 * allocate another.
			 */
			if (--kserver_thread_count <= 0 &&
			    kserver_thread_total < kserver_thread_max) {
				++kserver_thread_total;
				/*
				 *	Pretend that this thread and
				 *	the one it is creating are
				 *	already available for service.
				 *	Otherwise, we create threads
				 *	faster than we need to.
				 */
			        kserver_thread_count += 2;
				spinlock_unlock(&kernel_kmsg_lock);
				(void) kernel_thread(kernel_task,
						     kserver_thread,
						     (char *) 0);
				spinlock_lock(&kernel_kmsg_lock);
				continue;
			}

			/*
			 *	Maintain message ordering.  Incoming,
			 *	kernel-bound messages are still ordered
			 *	by the kernel_kmsg list but once taken
			 *	off the list by a kserver thread ordering
			 *	becomes trickier.  A kserver thread can
			 *	block, especially due to memory allocation.
			 *	In such a case, a subsequent message bound
			 *	for the same kernel object can race ahead
			 *	if, for whatever reason, its kserver thread
			 *	doesn't also block.  We prevent this delivery
			 *	race by setting a flag on the destination
			 *	port for the message.  If we see this flag
			 *	set, we skip over any messages associated
			 *	with the port.  The kserver thread delivering
			 *	a message to that port will clear the flag
			 *	in ipc_kobject_server, immediately before
			 *	calling into the requested kernel operation.
			 *
			 *	There are still two exceptions:
			 *		- critical messages, processed by
			 *		the kserver_pageout_support_thread,
			 *		can race ahead of non-critical messages
			 *		- under certain load conditions,
			 *		messages relating to paging flow
			 *		control can be accelerated ahead of
			 *		normal messages
			 */
			kmsg = kernel_kmsg;
			prev_ptr = &kernel_kmsg;
			fkmsg = IKM_NULL;

			do {
				dest_port = (ipc_port_t)
       					kmsg->ikm_header.msgh_remote_port;
				ip_lock(dest_port);

				if (!dest_port->ip_norma_kserver_active) {
					assert(!dest_port->
					       ip_norma_kserver_waiting);
					dest_port->ip_norma_kserver_active =
						TRUE;
					ip_unlock(dest_port);
					break;
				}

				/*
				 *	kserver already active on this port;
				 *	skip this kmsg.
				 */
				prev_ptr = &kmsg->ikm_next;
				fkmsg = kmsg;
				kmsg = kmsg->ikm_next;
				dest_port->ip_norma_kserver_waiting = TRUE;
				ip_unlock(dest_port);

			} while (kmsg != IKM_NULL);

			/*
			 *	If we skipped to the end of the queue,
			 *	or if this is a critical kmsg, get out
			 *	of this loop.
			 */
			if (kmsg == IKM_NULL || kmsg->ikm_critical) {
				kserver_thread_count++;
				break;
			}

			*prev_ptr = kmsg->ikm_next;
			kmsg->ikm_next = IKM_BOGUS; /* XXX */
			if (kernel_kmsg_tail == kmsg)
			    kernel_kmsg_tail = fkmsg;
			spinlock_unlock(&kernel_kmsg_lock);

			no_work = 0;
			norma_kserver_deliver(kmsg);

			spinlock_lock(&kernel_kmsg_lock);
			kserver_thread_count++;
		}

		/*
		 * If there are more than a steady_state number of
		 * threads waiting for something to do, exit the
		 * system.
		 */
		if (kserver_thread_count > kserver_thread_steady_state) {
#if 0
			/*
			 * "Kernel" threads can't currently terminate themselves.
		 	 * This could be made to work, of course, but I don't
			 * much motivation to do so.
			 */
			kserver_thread_count--;
			spinlock_unlock(&kernel_kmsg_lock);
			thread_terminate_self();
			/*NOTREACHED*/
#endif
		}

		kserver_awake = FALSE;
		if (no_work)
			++c_kserver_disappointed;
		assert_wait((int) &kserver_awake, FALSE);
		spinlock_unlock(&kernel_kmsg_lock);
		thread_block((void (*)(void)) 0);
	}
}

void
kserver_pageout_support_wakeup(void)
{
	spinlock_lock(&kserver_pageout_support_lock);
	assert(kserver_pageout_support_needed >= 0);
	++kserver_pageout_support_needed;
	thread_wakeup_one((int) kserver_pageout_support_thread);
	spinlock_unlock(&kserver_pageout_support_lock);
}


/*
 * Avoiding remote pageout deadlock.
 *
 * The kserver_pageout_support_thread exists only to avoid deadlocking
 * the kernel when paging out to a remote node.  There are two cases
 * of interest.  Case 1:
 * The message sequence looks like this:
 *	kernel2 sends kernel1 a pageout request
 *	[ possible intervening requests ]
 *	kernel1 sends kernel2 a pull_receive request
 * Handling pull_receive requires wiring the thread so a stack
 * will always be available for it -- after all, the request will
 * arrive during a memory shortage.  Wiring a thread implies
 * vm_privilege as well as stack_privilege.  Adding vm_privilege
 * to kserver_thread is a bad idea because intervening requests
 * (e.g., a vm_object_copy_slowly) can soak up all available vm
 * and hang the thread so that the pull_receive can't be processed.
 *
 * kernel_pageout_support_thread must be wired so it will always have a
 * stack but will not get in trouble with vm_privilege because it
 * doesn't handle any other kinds of requests.
 *
 * Case 2:
 * Freeing memory.  In some cases, we want to be sure that memory
 * will be freed in a timely fashion.  In others, freeing memory may
 * also require allocating memory (although less than that being freed).
 * In all cases, a wired thread is required to guarantee that we can
 * carry out these duties even in low-memory situations.
 */

int	kserver_pageout_thread_priority_init = BASEPRI_KERNEL+3;

void
kserver_pageout_support_thread(void)
{
	kern_return_t	kr;
	int		s;
	thread_t			thread;
	processor_set_t			pset;
	kern_return_t			ret;
	policy_base_t			base;
	policy_limit_t			limit;
	policy_fifo_base_data_t		fifo_base;
	policy_fifo_limit_data_t	fifo_limit;
	ipc_kmsg_t	kmsg;
	TR_DECL("kserver_pageout_support_thread");

	/*
	 * Set thread privileges.
	 */
	thread = current_thread();
	thread_wire(&realhost, thread->top_act, TRUE);
	stack_privilege(thread);

	/*
	 * Set thread priority and scheduling policy.
	 */
	pset = thread->processor_set;
	base = (policy_base_t) &fifo_base; 
	limit = (policy_limit_t) &fifo_limit;
	fifo_base.base_priority = kserver_pageout_thread_priority_init;
	fifo_limit.max_priority = kserver_pageout_thread_priority_init;
	ret = thread_set_policy(thread->top_act, pset, POLICY_FIFO, 
				base, POLICY_FIFO_BASE_COUNT,
				limit, POLICY_FIFO_LIMIT_COUNT);
	if (ret != KERN_SUCCESS)
		printf("WARNING: %s is being TIMESHARED!\n",
		       "kserver_pageout_thread ");
	
	kserver_pageout_support_needed = 0;

	/*
	 * Logic for NORMA pageout special cases.
	 *	1.  The netipc subsystem may be running low on
	 *	memory for receiving messages.  Give the subsystem
	 *	a chance to pump itself back up -- this thread
	 *	has vm_privilege, so only a few pages should be
	 *	allocated at a time.
	 *	2.  It is also necessary from time to time to
	 *	scan the netipc_safe_vm_map_copy_discard_list,
	 *	freeing up discarded pages.
	 *	3.  Acknowledged kmsgs must be freed from a thread that
	 *	can itself allocate memory -- potentially there are
	 *	notifications generated while freeing up the kmsg.
	 *	4.  The pull_receive message requires a separate,
	 *	wired but non-VM-abusing case.
	 */

	for (;;) {
	    spinlock_lock(&kserver_pageout_support_lock);
	    tr2("active, needed 0x%x", kserver_pageout_support_needed);

	    while (kserver_pageout_support_needed > 0) {
		--kserver_pageout_support_needed;
		spinlock_unlock(&kserver_pageout_support_lock);

		netipc_replenish_fallbacks();

		netipc_output_replenish_pages();

		netipc_safe_ikm_reclaim();

		spinlock_lock(&kserver_pageout_support_lock);
	    }

	    assert(kserver_pageout_support_needed == 0);
	    assert_wait((int) kserver_pageout_support_thread, FALSE);
	    spinlock_unlock(&kserver_pageout_support_lock);
	    thread_block((void (*)(void)) 0);
	}
	/* NOTREACHED */
}

void
kserver_paging_critical_thread(void)
{
	kern_return_t	kr;
	int		s;
	thread_t			thread;
	processor_set_t			pset;
	kern_return_t			ret;
	policy_base_t			base;
	policy_limit_t			limit;
	policy_fifo_base_data_t		fifo_base;
	policy_fifo_limit_data_t	fifo_limit;
	ipc_kmsg_t			kmsg, *listp, fkmsg;
	TR_DECL("kserver_paging_critical_thread");

	/*
	 * Set thread privileges.
	 */
	thread = current_thread();
	thread_wire(&realhost, thread->top_act, TRUE);
	stack_privilege(thread);

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
		printf("WARNING: %s is being TIMESHARED!\n",
		       "kserver_pageout_thread ");
	
	paging_critical_kmsg_count = 0;

	/*
	 *	Special purpose thread that runs at high priority
	 *	and vm_privilege to process paging critical threads.
	 *	It tries to process all available critical messages
	 *	before sleeping; care is taken to maintain the
	 *	insertion point into the kernel_kmsg queue.
	 */

	for (;;) {
	    tr2("active, count 0x%x", paging_critical_kmsg_count);

	    /*
	     * kmsg can be null here, which means we got
	     * called for nothing.
	     */
	    spinlock_lock(&kernel_kmsg_lock);
	    listp = &kernel_kmsg;
	    kmsg = kernel_kmsg;
	    fkmsg = IKM_NULL;
	    /*
	     * After a large number of runs, I have not
	     * found the depth in kernel_kmsg to a critical
	     * message to be more than 3, so sequentially
	     * processing the queue appears adequate.
	     */
	    while (kmsg && paging_critical_kmsg_count) {
		if (kmsg->ikm_critical) {
			paging_critical_kmsg_count--;
			*listp = kmsg->ikm_next;
			kmsg->ikm_next = IKM_BOGUS; /* XXX */
			/*
			 * Watch the queue insertion pointer
			 */
			if (kmsg == kernel_kmsg_tail)
				kernel_kmsg_tail = fkmsg;
			spinlock_unlock(&kernel_kmsg_lock);
			tr5("kmsg 0x%x node 0x%x remport 0x%x msgh_id 0x%x",
			    kmsg, kmsg->ikm_source_node,
			    kmsg->ikm_header.msgh_remote_port,
			    kmsg->ikm_header.msgh_id);
			norma_kserver_deliver(kmsg);
 			spinlock_lock(&kernel_kmsg_lock);
			++c_norma_drain_critical;
			listp = &kernel_kmsg;
			fkmsg = IKM_NULL;
		} else {
			listp = &kmsg->ikm_next;
			fkmsg = kmsg;
		}
		kmsg = *listp;
	    }

	    /*
	     * If there are other, non-critical kmsgs on the queue
	     * which need to take care of non-critical messages,
	     * wakeup a kserver thread to take care of them.
	     */
	    if (kernel_kmsg)
		thread_wakeup_one((int) &kserver_awake);

	    assert_wait((int) kserver_paging_critical_thread, FALSE);
	    paging_critical_wakeup_needed = TRUE;
	    spinlock_unlock(&kernel_kmsg_lock);

	    thread_block((void (*)(void)) 0);
	}
	/* NOTREACHED */
}

void
norma_ipc_kobject_send(
	ipc_kmsg_t	kmsg)
{
	boolean_t	critical;

	TR_DECL("norma_ipc_kobject_send");

	critical = norma_critical_message(kmsg->ikm_header.msgh_id, FALSE);

	tr4("critical=%d, kmsg 0x%x tail 0x%x",
	    critical, kmsg, kernel_kmsg_tail);

	spinlock_lock(&kernel_kmsg_lock);

	if (kernel_kmsg) {
		assert(kernel_kmsg_tail);
		kernel_kmsg_tail->ikm_next = kmsg;
	} else {
		kernel_kmsg = kmsg;
	}
	kernel_kmsg_tail = kmsg;
	kmsg->ikm_next = 0;
	kmsg->ikm_prev = IKM_BOGUS; /* XXX */

	if (critical)
		paging_critical_kmsg_count++;
	kmsg->ikm_critical = critical;
	assert(!critical || kmsg->ikm_critical);

	/*
	 * Wakeup the paging_critical thread if we need to, else
	 * get a standard kserver thread going if this is not a
	 * critical message.
	 */
	if (paging_critical_wakeup_needed) {
		thread_wakeup_one((int) kserver_paging_critical_thread);
		paging_critical_wakeup_needed = FALSE;
	}
	else if (kmsg->ikm_critical == FALSE)
		thread_wakeup_one((int) &kserver_awake);

	spinlock_unlock(&kernel_kmsg_lock);
}

void
norma_kserver_wakeup(
	ipc_port_t	port)
{
	if (port->ip_norma_kserver_active) {
		port->ip_norma_kserver_active = FALSE;
		if (port->ip_norma_kserver_waiting) {
			port->ip_norma_kserver_waiting = FALSE;
			thread_wakeup_one((int) &kserver_awake);
		}
	} else
		assert(!port->ip_norma_kserver_waiting);
}

void
norma_kserver_startup(void)
{
	int i;
	int cpus;

	spinlock_init(&kernel_kmsg_lock);
	spinlock_init(&kserver_pageout_support_lock);

	cpus = 0;
	for (i = NCPUS/2; i > 0; i >>= 1)
		cpus++;

	for (i = NUM_KSERVER_PAGEOUT_THREADS +
	     cpus / NUM_KSERVER_PAGEOUT_MP_THREADS; i > 0; i--) {
		(void) kernel_thread(kernel_task,
				     kserver_pageout_support_thread,
				     (char *) 0);
	}
	for (i = NUM_KSERVER_CRITICAL_THREADS +
	     cpus / NUM_KSERVER_CRITICAL_MP_THREADS; i > 0; i--) {
		(void) kernel_thread(kernel_task,
				     kserver_paging_critical_thread,
				     (char *) 0);
	}
	for (i = NUM_KSERVER_THREADS +
	     cpus / NUM_KSERVER_MP_THREADS; i > 0; i--) {
		(void) kernel_thread(kernel_task, kserver_thread, (char *) 0);
	}
	kserver_thread_total = kserver_thread_count =
		NUM_KSERVER_THREADS + cpus / NUM_KSERVER_MP_THREADS;
}
