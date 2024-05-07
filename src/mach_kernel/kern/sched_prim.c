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
 * Revision 2.18.4.5  92/04/30  11:59:17  bernadat
 * 	Increased MAX_STUCK_THREADS to 64
 * 	[92/03/19            bernadat]
 * 
 * Revision 2.18.4.4  92/03/28  10:10:15  jeffreyh
 * 	Pick up changes from MK71
 * 	[92/03/20  13:16:25  jeffreyh]
 * 
 * Revision 2.19  92/02/19  16:06:35  elf
 * 	Added argument to compute_priority.  We dont always want to do
 * 	a reschedule right away.
 * 	[92/01/19            rwd]
 * 
 * Revision 2.18.4.3  92/03/03  16:20:15  jeffreyh
 * 	Fix printf
 * 	[92/02/25            jeffreyh]
 * 
 * Revision 2.18.4.2  92/02/21  14:26:59  jsb
 * 	Removed spurious newline.
 * 
 * Revision 2.18.4.1  92/02/18  19:10:20  jeffreyh
 * 	Increased MAX_STUCK_THREADS to 64
 * 	[92/02/11  08:01:16  bernadat]
 * 
 * Revision 2.18  91/09/04  11:28:26  jsb
 * 	Add a temporary hack to thread_dispatch for i860 support:
 * 	don't panic if thread->state is TH_WAIT.
 * 	[91/09/04  09:24:43  jsb]
 * 
 * Revision 2.17  91/08/24  11:59:46  af
 * 	Final form of do_priority_computation was missing a pair of parenthesis.
 * 	[91/07/19            danner]
 * 
 * Revision 2.16  91/07/31  17:47:27  dbg
 * 	When re-invoking the running thread (thread_block, thread_run):
 * 	. Mark the thread interruptible.
 * 	. If there is a continuation, call it instead of returning.
 * 	[91/07/26            dbg]
 * 
 * 	Fix timeout race.
 * 	[91/05/23            dbg]
 * 
 * 	Revised scheduling state machine.
 * 	[91/05/22            dbg]
 * 
 * Revision 2.15  91/05/18  14:32:58  rpd
 * 	Added check_simple_locks to thread_block and thread_run.
 * 	[91/05/02            rpd]
 * 	Changed recompute_priorities to use a private timer.
 * 	Changed thread_timeout_setup to initialize depress_timer.
 * 	[91/03/31            rpd]
 * 
 * 	Updated thread_invoke to check stack_privilege.
 * 	[91/03/30            rpd]
 * 
 * Revision 2.14  91/05/14  16:46:16  mrt
 * 	Correcting copyright
 * 
 * Revision 2.13  91/05/08  12:48:33  dbg
 * 	Distinguish processor sets from run queues in choose_pset_thread!
 * 	Remove (long dead) 'someday' code.
 * 	[91/04/26  14:43:26  dbg]
 * 
 * Revision 2.12  91/03/16  14:51:09  rpd
 * 	Added idle_thread_continue, sched_thread_continue.
 * 	[91/01/20            rpd]
 * 
 * 	Allow swapped threads on the run queues.
 * 	Added thread_invoke, thread_select.
 * 	Reorganized thread_block, thread_run.
 * 	Changed the AST interface; idle_thread checks for ASTs now.
 * 	[91/01/17            rpd]
 * 
 * Revision 2.11  91/02/05  17:28:57  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:16:51  mrt]
 * 
 * Revision 2.10  91/01/08  15:16:38  rpd
 * 	Added KEEP_STACKS support.
 * 	[91/01/06            rpd]
 * 	Added thread_continue_calls counter.
 * 	[91/01/03  22:07:43  rpd]
 * 
 * 	Added continuation argument to thread_run.
 * 	[90/12/11            rpd]
 * 	Added continuation argument to thread_block/thread_continue.
 * 	Removed FAST_CSW conditionals.
 * 	[90/12/08            rpd]
 * 
 * 	Removed thread_swap_tick.
 * 	[90/11/11            rpd]
 * 
 * Revision 2.9  90/09/09  14:32:36  rpd
 * 	Removed do_pset_scan call from sched_thread.
 * 	[90/08/30            rpd]
 * 
 * Revision 2.8  90/08/07  22:22:54  rpd
 * 	Fixed casting of a volatile comparison: the non-volatile should be casted or else.
 * 	Removed silly set_leds() mips thingy.
 * 	[90/08/07  15:56:08  af]
 * 
 * Revision 2.7  90/08/07  17:58:55  rpd
 * 	Removed sched_debug; converted set_pri, update_priority,
 * 	and compute_my_priority to real functions.
 * 	Picked up fix for thread_block, to check for processor set mismatch.
 * 	Record last processor info on all multiprocessors.
 * 	[90/08/07            rpd]
 * 
 * Revision 2.6  90/06/02  14:55:49  rpd
 * 	Updated to new scheduling technology.
 * 	[90/03/26  22:16:03  rpd]
 * 
 * Revision 2.5  90/01/11  11:43:51  dbg
 * 	Check for cpu shutting down on exit from idle loop - next_thread
 * 	will be THREAD_NULL in this case.
 * 	[90/01/03            dbg]
 * 
 * 	Make sure cpu is marked active on all exits from idle loop.
 * 	[89/12/11            dbg]
 * 
 * 	Removed more lint.
 * 	[89/12/05            dbg]
 * 
 * 	DLB's scheduling changes in thread_block don't work if partially
 * 	applied; a thread can run in two places at once.  Revert to old
 * 	code, pending a complete merge.
 * 	[89/12/04            dbg]
 * 
 * Revision 2.4  89/11/29  14:09:11  af
 * 	On Mips, delay setting of active_threads inside load_context,
 * 	or we might take exceptions in an embarassing state.
 * 	[89/11/03  17:00:04  af]
 * 
 * 	Long overdue fix: the pointers that the idle thread uses to check
 * 	for someone to become runnable are now "volatile".  This prevents
 * 	smart compilers from overoptimizing (e.g. Mips).
 * 
 * 	While looking for someone to run in the idle_thread(), rotate
 * 	console lights on Mips to show we're alive [useful when machine
 * 	becomes catatonic].
 * 	[89/10/28            af]
 * 
 * Revision 2.3  89/09/08  11:26:22  dbg
 * 	Add extra thread state cases to thread_switch, since it may now
 * 	be called by a thread about to block.
 * 	[89/08/22            dbg]
 * 
 * Revision 2.16.2.1  91/08/19  13:45:35  danner
 * 	Final form of do_priority_computation was missing a pair of parenthesis.
 * 	[91/07/19            danner]
 * 
 * 19-Dec-88  David Golub (dbg) at Carnegie-Mellon University
 *	Changes for MACH_KERNEL:
 *	. Import timing definitions from sys/time_out.h.
 *	. Split uses of PMAP_ACTIVATE and PMAP_DEACTIVATE into
 *	  separate _USER and _KERNEL macros.
 *
 * Revision 2.8  88/12/19  02:46:33  mwyoung
 * 	Corrected include file references.  Use <kern/macro_help.h>.
 * 	[88/11/22            mwyoung]
 * 	
 * 	In thread_wakeup_with_result(), only lock threads that have the
 * 	appropriate wait_event.  Both the wait_event and the hash bucket
 * 	links are only modified with both the thread *and* hash bucket
 * 	locked, so it should be safe to read them with either locked.
 * 	
 * 	Documented the wait event mechanism.
 * 	
 * 	Summarized ancient history.
 * 	[88/11/21            mwyoung]
 * 
 * Revision 2.7  88/08/25  18:18:00  mwyoung
 * 	Corrected include file references.
 * 	[88/08/22            mwyoung]
 * 	
 * 	Avoid unsigned computation in wait_hash.
 * 	[88/08/16  00:29:51  mwyoung]
 * 	
 * 	Add priority check to thread_check; make queue index unsigned,
 * 	so that checking works correctly at all.
 * 	[88/08/11  18:47:55  mwyoung]
 * 
 * 11-Aug-88  David Black (dlb) at Carnegie-Mellon University
 *	Support ast mechanism for threads.  Thread from local_runq gets
 *	minimum quantum to start.
 *
 *  9-Aug-88  David Black (dlb) at Carnegie-Mellon University
 *	Moved logic to detect and clear next_thread[] dispatch to
 *	idle_thread() from thread_block().
 *	Maintain first_quantum field in thread instead of runrun.
 *	Changed preempt logic in thread_setrun.
 *	Avoid context switch if current thread is still runnable and
 *	processor would go idle as a result.
 *	Added scanner to unstick stuck threads.
 *
 * Revision 2.6  88/08/06  18:25:03  rpd
 * Eliminated use of kern/mach_ipc_defs.h.
 * 
 * 10-Jul-88  David Golub (dbg) at Carnegie-Mellon University
 *	Check for negative priority (BUG) in thread_setrun.
 *
 * Revision 2.5  88/07/20  16:39:35  rpd
 * Changed "NCPUS > 1" conditionals that were eliminating dead
 * simple locking code to MACH_SLOCKS conditionals.
 * 
 *  7-Jul-88  David Golub (dbg) at Carnegie-Mellon University
 *	Split uses of PMAP_ACTIVATE and PMAP_DEACTIVATE into separate
 *	_USER and _KERNEL macros.
 *
 * 15-Jun-88  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Removed excessive thread_unlock() occurrences in thread_wakeup.
 *	Problem discovered and solved by Richard Draves.
 *
 * Historical summary:
 *
 *	Redo priority recomputation. [dlb, 29 feb 88]
 *	New accurate timing. [dlb, 19 feb 88]
 *	Simplified choose_thread and thread_block. [dlb, 18 dec 87]
 *	Add machine-dependent hooks in idle loop. [dbg, 24 nov 87]
 *	Quantum scheduling changes. [dlb, 14 oct 87]
 *	Replaced scheduling logic with a state machine, and included
 *	 timeout handling. [dbg, 05 oct 87]
 *	Deactivate kernel pmap in idle_thread. [dlb, 23 sep 87]
 *	Favor local_runq in choose_thread. [dlb, 23 sep 87]
 *	Hacks for master processor handling. [rvb, 12 sep 87]
 *	Improved idle cpu and idle threads logic. [dlb, 24 aug 87]
 *	Priority computation improvements. [dlb, 26 jun 87]
 *	Quantum-based scheduling. [avie, dlb, apr 87]
 *	Improved thread swapper. [avie, 13 mar 87]
 *	Lots of bug fixes. [dbg, mar 87]
 *	Accurate timing support. [dlb, 27 feb 87]
 *	Reductions in scheduler lock contention. [dlb, 18 feb 87]
 *	Revise thread suspension mechanism. [avie, 17 feb 87]
 *	Real thread handling [avie, 31 jan 87]
 *	Direct idle cpu dispatching. [dlb, 19 jan 87]
 *	Initial processor binding. [avie, 30 sep 86]
 *	Initial sleep/wakeup. [dbg, 12 jun 86]
 *	Created. [avie, 08 apr 86]
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
 *	File:	sched_prim.c
 *	Author:	Avadis Tevanian, Jr.
 *	Date:	1986
 *
 *	Scheduling primitives
 *
 */

#include <cpus.h>
#include <mach_kdb.h>
#include <simple_clock.h>
#include <mach_host.h>
#include <hw_footprint.h>
#include <fast_tas.h>
#include <power_save.h>

#include <ddb/db_output.h>
#include <mach/machine.h>
#include <kern/ast.h>
#include <kern/counters.h>
#include <kern/cpu_number.h>
#include <kern/lock.h>
#include <kern/macro_help.h>
#include <kern/machine.h>
#include <kern/misc_protos.h>
#include <kern/processor.h>
#include <kern/queue.h>
#include <kern/sched.h>
#include <kern/sched_prim.h>
#include <kern/ipc_sched.h>
#include <kern/syscall_subr.h>
#include <kern/thread.h>
#include <kern/time_out.h>
#include <vm/pmap.h>
#include <vm/vm_kern.h>
#include <vm/vm_map.h>
#include <mach/policy.h>

extern int hz;

int		min_quantum;	/* defines max context switch rate */

unsigned	sched_tick;

#if	SIMPLE_CLOCK
int		sched_usec;
#endif	/* SIMPLE_CLOCK */

thread_t	sched_thread_id;

/* Forwards */
void		wait_queue_init(void);

void		set_pri(
			thread_t	th,
			int		pri,
			int		resched);

thread_t	choose_pset_thread(
			processor_t		myprocessor,
			processor_set_t		pset);

#if	NCPUS > 1
thread_t	choose_thread(
			processor_t		myprocessor);

#endif	/*NCPUS > 1*/
int		run_queue_enqueue(
			run_queue_t	rq,
			thread_t	th,
			boolean_t	tail);

void		idle_thread_continue(void);
void		sched_thread_continue(void);
void		do_thread_scan(void);
boolean_t	do_runq_scan(
			run_queue_t	runq);

void		log_thread_action (char *, long, long, long);

#if	DEBUG
void		checkrq(
			run_queue_t	rq,
			char		*msg);

void		thread_check(
			thread_t	th,
			run_queue_t	rq);

#endif	/*DEBUG*/

timer_elt_data_t recompute_priorities_timer;

/*
 *	State machine
 *
 * states are combinations of:
 *  R	running
 *  W	waiting (or on wait queue)
 *  N	non-interruptible
 *
 * init	action 
 *	assert_wait thread_block    clear_wait 
 *
 * R	RW, RWN	    R;   setrun	    -	       
 * RN	RWN	    RN;  setrun	    -	       
 *
 * RW		    W		    R	       
 * RWN		    WN		    RN	       
 *
 * W				    R;   setrun
 * WN				    RN;  setrun
 *
 */

/*
 *	Waiting protocols and implementation:
 *
 *	Each thread may be waiting for exactly one event; this event
 *	is set using assert_wait().  That thread may be awakened either
 *	by performing a thread_wakeup_prim() on its event,
 *	or by directly waking that thread up with clear_wait().
 *
 *	The implementation of wait events uses a hash table.  Each
 *	bucket is queue of threads having the same hash function
 *	value; the chain for the queue (linked list) is the run queue
 *	field.  [It is not possible to be waiting and runnable at the
 *	same time.]
 *
 *	Locks on both the thread and on the hash buckets govern the
 *	wait event field and the queue chain field.  Because wakeup
 *	operations only have the event as an argument, the event hash
 *	bucket must be locked before any thread.
 *
 *	Scheduling operations may also occur at interrupt level; therefore,
 *	interrupts below splsched() must be prevented when holding
 *	thread or hash bucket locks.
 *
 *	The wait event hash table declarations are as follows:
 */

#define NUMQUEUES	59

queue_head_t		wait_queue[NUMQUEUES];
spinlock_t		wait_lock[NUMQUEUES];

#define wait_hash(event) \
	(((int)((event) < 0 ? ((event) ^ -1) : (event)))%NUMQUEUES)

void
sched_init(void)
{
	recompute_priorities_timer.fcn = (timeout_fcn_t)recompute_priorities;
	recompute_priorities_timer.param = (void *)0;

	min_quantum = hz / 10;		/* context switch 10 times/second */
	wait_queue_init();
	pset_sys_bootstrap();		/* initialize processer mgmt. */
	queue_init(&action_queue);
	spinlock_init(&action_lock);
	sched_tick = 0;
#if	SIMPLE_CLOCK
	sched_usec = 0;
#endif	/* SIMPLE_CLOCK */
	ast_init();
}

void
wait_queue_init(void)
{
	register int	i;

	for (i = 0; i < NUMQUEUES; i++) {
		queue_init(&wait_queue[i]);
		spinlock_init(&wait_lock[i]);
	}
}

/*
 *	Thread timeout routine, called when timer expires.
 *	Called at splsoftclock.
 */
void
thread_timeout(
	thread_t thread)
{
	assert(thread->timer.set == TELT_UNSET);

	clear_wait(thread, THREAD_TIMED_OUT, FALSE);
}

/*
 *	thread_set_timeout:
 *
 *	Set a timer for the current thread, if the thread
 *	is ready to wait.  Must be called between assert_wait()
 *	and thread_block().
 */
 
void
thread_set_timeout(
	int	t)	/* timeout interval in ticks */
{
	register thread_t	thread = current_thread();
	spl_t	s;

	s = splsched();
	thread_lock(thread);
	if ((thread->state & TH_WAIT) != 0) {
		set_timeout(&thread->timer, t);
	}
	thread_unlock(thread);
	splx(s);
}

/*
 * Set up thread timeout element when thread is created.
 */
void
thread_timeout_setup(
	register thread_t	thread)
{
	thread->timer.fcn = (timeout_fcn_t)thread_timeout;
	thread->timer.param = (void *)thread;
	thread->depress_timer.fcn = (timeout_fcn_t)thread_depress_timeout;
	thread->depress_timer.param = (void *)thread;
}

/*
 *	Routine:	thread_go
 *	Purpose:
 *		Start a thread running.
 *	Conditions:
 *		IPC locks may be held.
 */

void
thread_go(thread)
	thread_t thread;
{
	int	s, state;

	s = splsched();
	thread_lock(thread);

	reset_timeout_check(&thread->timer);

	if (thread->state & TH_WAIT) {
		thread->state &= ~TH_WAIT;
		if (!(thread->state & TH_RUN)) {
			thread->state |= TH_RUN;
			thread_setrun(thread, TRUE, TAIL_Q);
		}
		thread->wait_result = THREAD_AWAKENED;
	}

	thread_unlock(thread);
	splx(s);
}

/*
 *	Routine:	thread_will_wait
 *	Purpose:
 *		Assert that the thread intends to block.
 */

void
thread_will_wait(thread)
	thread_t thread;
{
	int	s;

	s = splsched();
	thread_lock(thread);

	assert(thread->wait_result = -1);	/* for later assertions */
	thread->state |= TH_WAIT;

	thread_unlock(thread);
	splx(s);
}

/*
 *	Routine:	thread_will_wait_with_timeout
 *	Purpose:
 *		Assert that the thread intends to block,
 *		with a timeout.
 */

void
thread_will_wait_with_timeout(thread, msecs)
	thread_t thread;
	mach_msg_timeout_t msecs;
{
	unsigned int ticks = convert_ipc_timeout_to_ticks(msecs);
	int s;

	s = splsched();
	thread_lock(thread);

	assert(thread->wait_result = -1);	/* for later assertions */
	thread->state |= TH_WAIT;

	set_timeout(&thread->timer, ticks);

	thread_unlock(thread);
	splx(s);
}

/*
 *	assert_wait:
 *
 *	Assert that the current thread is about to go to
 *	sleep until the specified event occurs.
 */
void
assert_wait(
	int		event,
	boolean_t	interruptible)
{
	register queue_t	q;
	register int		index;
	register thread_t	thread;
	register spinlock_t	*lock;
	spl_t			s;

	thread = current_thread();
	if (thread->wait_event != NO_EVENT) {
		panic("assert_wait: already asserted event 0x%x\n",
			thread->wait_event);
	}

	s = splsched();
	if (event != NO_EVENT) {
		index = wait_hash(event);
		q = &wait_queue[index];
		lock = &wait_lock[index];
		spinlock_lock(lock);
		thread_lock(thread);
		enqueue_tail(q, (queue_entry_t) thread);
		thread->wait_event = event;
		if (interruptible)
			thread->state |= TH_WAIT;
		else
			thread->state |= TH_WAIT | TH_UNINT;
		thread_unlock(thread);
		spinlock_unlock(lock);
	}
	else {
		thread_lock(thread);
		if (interruptible)
			thread->state |= TH_WAIT;
		else
			thread->state |= TH_WAIT | TH_UNINT;
		thread_unlock(thread);
	}
	splx(s);
}

/*
 *	clear_wait:
 *
 *	Clear the wait condition for the specified thread.  Start the thread
 *	executing if that is appropriate.
 *
 *	parameters:
 *	  thread		thread to awaken
 *	  result		Wakeup result the thread should see
 *	  interrupt_only	Don't wake up the thread if it isn't
 *				interruptible.
 */
void
clear_wait(
	register thread_t	thread,
	int			result,
	boolean_t		interrupt_only)
{
	register int		index;
	register queue_t	q;
	register spinlock_t	*lock;
	register int		event;
	spl_t			s;

	s = splsched();
	thread_lock(thread);
	if (interrupt_only && (thread->state & TH_UNINT)) {
		/*
		 *	can`t interrupt thread
		 */
		thread_unlock(thread);
		splx(s);
		return;
	}

	event = thread->wait_event;

	/*
	** If the wait_event field is in the transitional state,
	** we're racing with someone in thread_wakeup_prim(),
	** who has unlocked the hash bucket lock, but hasn't yet
	** woken the thread.  We can just unlock and return.
	*/

	if (event == WAKING_EVENT) {
		thread_unlock(thread);
		splx(s);
		return;
	}

	if (event != NO_EVENT) {
		thread_unlock(thread);
		index = wait_hash(event);
		q = &wait_queue[index];
		lock = &wait_lock[index];
		spinlock_lock(lock);
		/*
		 *	If the thread is still waiting on that event,
		 *	then remove it from the list.  If it is waiting
		 *	on a different event, or no event at all, then
		 *	someone else did our job for us.
		 */
		thread_lock(thread);
		if (thread->wait_event == event) {
			remqueue(q, (queue_entry_t)thread);
			thread->wait_event = NO_EVENT;
			event = NO_EVENT;		/* cause to run below */
		}
		spinlock_unlock(lock);
	}
	if (event == NO_EVENT) {
		register int	state = thread->state;

		reset_timeout_check(&thread->timer);

		switch (state & TH_SCHED_STATE) {
		    case	  TH_WAIT | TH_SUSP | TH_UNINT:
		    case	  TH_WAIT	    | TH_UNINT:
		    case	  TH_WAIT:
			/*
			 *	Sleeping and not suspendable - put
			 *	on run queue.
			 */
			thread->state = (state &~ TH_WAIT) | TH_RUN;
			thread->wait_result = result;
			thread_setrun(thread, TRUE, TAIL_Q);
			break;

		    case	  TH_WAIT | TH_SUSP:
		    case TH_RUN | TH_WAIT:
		    case TH_RUN | TH_WAIT | TH_SUSP:
		    case TH_RUN | TH_WAIT	    | TH_UNINT:
		    case TH_RUN | TH_WAIT | TH_SUSP | TH_UNINT:
			/*
			 *	Either already running, or suspended.
			 */
			thread->state = state &~ TH_WAIT;
			thread->wait_result = result;
			break;

		    default:
			/*
			 *	Not waiting.
			 */
			break;
		}
	}
	thread_unlock(thread);
	splx(s);
}

/*
 *	thread_wakeup_prim:
 *
 *	Common routine for thread_wakeup, thread_wakeup_with_result,
 *	and thread_wakeup_one.
 *
 */
void
thread_wakeup_prim(
	register int	event,
	boolean_t	one_thread,
	int		result,
	boolean_t	debug)
{
	register queue_t	q;
	register int		index;
	register thread_t	thread, next_th;
	register spinlock_t	*lock;
	register int		state;
	queue_head_t		wake_queue;
	spl_t			s;

	index = wait_hash(event);
	q = &wait_queue[index];
	s = splsched();
	lock = &wait_lock[index];

	spinlock_lock(lock);
	thread = (thread_t) queue_first(q);
	queue_init (&wake_queue);

#if	MACH_LDEBUG
	if (debug) {
		log_thread_action ("thread_wakeup - entry", (long)event, 0, 0);
	}
#endif

	while (!queue_end(q, (queue_entry_t)thread)) {
		next_th = (thread_t) queue_next((queue_t) thread);

		if (thread->wait_event == event) {
		        remqueue(q, (queue_entry_t) thread);
			enqueue (&wake_queue, (queue_entry_t) thread);
			thread->wait_event = WAKING_EVENT;

			if (one_thread)
				break;
		}

		thread = next_th;
	}

	spinlock_unlock(lock);

	while (!queue_empty (&wake_queue)) {
		thread = (thread_t) dequeue (&wake_queue);

		thread_lock(thread);

#if	MACH_LDEBUG
		if (debug) {
			log_thread_action ("thread_wakeup - thread", (long)event,
					(long)thread, (long)(thread->state));
		}
#endif

		reset_timeout_check(&thread->timer);
		state = thread->state;
		switch (state & TH_SCHED_STATE) {

			    case	  TH_WAIT | TH_SUSP | TH_UNINT:
			    case	  TH_WAIT	    | TH_UNINT:
			    case	  TH_WAIT:
				/*
				 *	Sleeping and not suspendable - put
				 *	on run queue.
				 */
				thread->state = (state &~ TH_WAIT) | TH_RUN;
				thread->wait_result = result;
				thread_setrun(thread, TRUE, TAIL_Q);
				break;

			    case 	  TH_WAIT | TH_SUSP:
			    case TH_RUN | TH_WAIT:
			    case TH_RUN | TH_WAIT | TH_SUSP:
			    case TH_RUN | TH_WAIT	    | TH_UNINT:
			    case TH_RUN | TH_WAIT | TH_SUSP | TH_UNINT:
				/*
				 *	Either already running, or suspended.
				 */
				thread->state = state &~ TH_WAIT;
				thread->wait_result = result;
				break;

			default:
				panic("thread_wakeup");
				break;
		}

		thread->wait_event = NO_EVENT;

		thread_unlock(thread);
	}

	splx(s);
}

/*
 *	thread_sleep_spinlock:
 *
 *	Cause the current thread to wait until the specified event
 *	occurs.  The specified spinlock is unlocked before releasing
 *	the cpu.  (This is a convenient way to sleep without manually
 *	calling assert_wait).
 */
void
thread_sleep_spinlock(
	int		event,
	spinlock_t	*lock,
	boolean_t	interruptible)
{
	assert_wait(event, interruptible);	/* assert event */
	spinlock_unlock(lock);			/* release the lock */
	thread_block((void (*)(void)) 0);	/* block ourselves */
}

/*
 *	thread_sleep_mutex:
 *
 *	Cause the current thread to wait until the specified event
 *	occurs.  The specified mutex is unlocked before releasing
 *	the cpu.  (This is a convenient way to sleep without manually
 *	calling assert_wait).
 */

void
thread_sleep_mutex(
	int		event,
	mutex_t		*lock,
	boolean_t	interruptible)
{
	assert_wait(event, interruptible);	/* assert event */
	mutex_unlock(lock);			/* release the lock */
	thread_block((void (*)(void)) 0);	/* block ourselves */
}

/*
 *	thread_sleep_interlock:
 *
 *	Cause the current thread to wait until the specified event
 *	occurs.  The specified HW interlock is unlocked before releasing
 *	the cpu.  (This is a convenient way to sleep without manually
 *	calling assert_wait).
 */

void
thread_sleep_interlock(
	int		event,
	hw_lock_t	*lock,
	boolean_t	interruptible)
{
	assert_wait(event, interruptible);	/* assert event */
	interlock_unlock(lock);			/* release the lock */
	thread_block((void (*)(void)) 0);	/* block ourselves */
}

#if	NCPUS > 1
/*
 *	thread_bind:
 *
 *	Force a thread to execute on the specified processor.
 *	If the thread is currently executing, it may wait until its
 *	time slice is up before switching onto the specified processor.
 *
 *	A processor of PROCESSOR_NULL causes the thread to be unbound.
 *	xxx - DO NOT export this to users.
 */
void
thread_bind(
	register thread_t	thread,
	processor_t		processor)
{
	spl_t	s;

	s = splsched();
	thread_lock(thread);
	thread->bound_processor = processor;
	thread_unlock(thread);
	(void) splx(s);
}
#endif	/*NCPUS > 1*/

/*
 *	Select a thread for this processor (the current processor) to run.
 *	May select the current thread.
 *	Assumes splsched.
 */

thread_t
thread_select(
	register processor_t	myprocessor)
{
	register thread_t	thread;
	processor_set_t		pset;
#if	NCPUS > 1
	register run_queue_t	runq = &myprocessor->runq;
#endif	/* NCPUS > 1 */
	boolean_t		other_runnable;

	/*
	 *	Check for other non-idle runnable threads.
	 */
	myprocessor->first_quantum = TRUE;
	pset = myprocessor->processor_set;
	thread = current_thread();

#if     NCPUS > 1
	spinlock_lock(&runq->lock);
#endif  /* NCPUS > 1 */
	spinlock_lock(&pset->runq.lock);

	other_runnable = 
#if	NCPUS > 1
	    runq->count > 0 ||
#endif	/* NCPUS > 1 */
	    pset->runq.count > 0;

	if ((!other_runnable ||
#if	NCPUS > 1
	     runq->low > thread->sched_pri &&
#endif	/* NCPUS > 1 */
				pset->runq.low > thread->sched_pri
	    )		&&
#if	MACH_HOST
	    (thread->processor_set == pset) &&
#endif	/* MACH_HOST */
#if	NCPUS > 1
	    ((thread->bound_processor == PROCESSOR_NULL) ||
	     (thread->bound_processor == myprocessor)) &&
#endif	/* NCPUS > 1 */
	    (thread->state == TH_RUN)) {

		/* I am the highest priority runnable thread: */
		spinlock_unlock(&pset->runq.lock);
#if     NCPUS > 1
		spinlock_unlock(&runq->lock);
#endif  /* NCPUS > 1 */

		thread_lock(thread);
		if (thread->sched_stamp != sched_tick)
			update_priority(thread);
		thread_unlock(thread);
#if	NCPUS > 1
	} else if (other_runnable) {
		spinlock_unlock(&pset->runq.lock);
		spinlock_unlock(&runq->lock);
		thread = choose_thread(myprocessor);
#endif	/* NCPUS > 1 */
	} else {
#if     NCPUS > 1
		spinlock_unlock(&runq->lock);
#endif  /* NCPUS > 1 */

		/*
		 *	Nothing non-idle runnable, including myself.
		 *	Return if this
		 *	thread is still runnable on this processor.
		 *	Check for priority update if required.
		 */
		/* get an idle thread to run */
		thread = choose_pset_thread(myprocessor, pset);
	}
	if (thread->policy == POLICY_RR ||
	    thread->policy == POLICY_FIFO)
		myprocessor->quantum = thread->sched_data;
	else
#if	NCPUS > 1
		myprocessor->quantum = (thread->bound_processor ?
					min_quantum : pset->set_quantum);
#else	/* NCPUS > 1 */
		myprocessor->quantum = pset->set_quantum;
#endif	/* NCPUS > 1 */
	return (thread);
}


/*
 *	Stop running the current thread and start running the new thread.
 *	If continuation is non-zero, and the current thread is blocked,
 *	then it will resume by executing continuation on a new stack.
 *	Returns TRUE if the hand-off succeeds.
 *	The reason parameter == AST_QUANTUM if the thread blocked
 *	because its quantum expired.
 *	Assumes splsched.
 */

void
thread_invoke(
	register thread_t	old_thread,
	void			(*continuation)(void),
	register thread_t	new_thread,
	int			reason)
{
	continuation = (void (*)(void)) 0;

	/*
	 *	Mark thread interruptible.
	 */
	thread_lock(new_thread);
	new_thread->state &= ~TH_UNINT;
	thread_unlock(new_thread);

	/*
	 *	Check for invoking the same thread.
	 */
	if (old_thread == new_thread)
		return;

	/*
	 *	Thread is now interruptible.
	 */
#if	NCPUS > 1
	new_thread->last_processor = current_processor();
#endif	/* NCPUS > 1 */

	/*
	 *	Set up ast context of new thread and switch to its timer.
	 */
	ast_context(new_thread->top_act, cpu_number());
	timer_switch(&new_thread->system_timer);

	/*
	 *	switch_context is machine-dependent.  It does the
	 *	machine-dependent components of a context-switch, like
	 *	changing address spaces.  It updates active_threads.
	 *	It returns only if a continuation is not supplied.
	 */
#if	MACH_ASSERT
	if (watchacts & WA_SWITCH) {
	    vm_offset_t stack = new_thread->top_act->kernel_stack;
	    printf("thread_invoke(old=%x,cont=%x,new=%x) new.iks=%x\n",
			old_thread, continuation, new_thread,
			STACK_IKS(stack));
	    printf("\tcurrent_thr=%x swap_func = %x\n",
			current_thread(), new_thread->swap_func);
	}
#endif	/* MACH_ASSERT */

	old_thread->reason = reason;
	counter_always(c_thread_invoke_csw++);

	/*
	 * N.B. On return from the call to switch_context, 'old_thread'
	 * points at the thread that yielded to us.  Unfortunately, at
	 * this point, there are no spinlocks held, so if we are preempted
	 * before the call to thread_dispatch blocks preemption, it is
	 * possible for 'old_thread' to terminate, leaving us with a
	 * stale thread pointer.
	 */

	disable_preemption();

	old_thread = switch_context(old_thread, continuation, new_thread);

	/*
	 *	We're back.  Now old_thread is the thread that resumed
	 *	us, and we have to dispatch it.
	 *
	 * N.B.: collusion with thread_dispatch, avoid dispatching idle
	 * thread here.  No locking: RO access to invariant IDLE bit.
	 */ 
	if (!(old_thread->state & TH_IDLE))
		thread_dispatch(old_thread);

	enable_preemption();
}

/*
 *	thread_continue:
 *
 *	Called when the launching a new thread, at splsched();
 */
void
thread_continue(
	register thread_t old_thread)
{
	register thread_t self = current_thread();
	register void (*continuation)(void) = self->swap_func;

	/*
	 *	We must dispatch the old thread and then
	 *	call the current thread's continuation.
	 *	There might not be an old thread, if we are
	 *	the first thread to run on this processor.
	 */

	if (old_thread != THREAD_NULL)
		thread_dispatch(old_thread);
	self->at_safe_point = NOT_AT_SAFE_POINT;

	/*
	 * N.B. - the following is necessary, since thread_invoke()
	 * inhibits preemption on entry and reenables before it
	 * returns.  Unfortunately, the first time a newly-created
	 * thread executes, it magically appears here, and never
	 * executes the enable_preemption() call in thread_invoke().
	 */

	enable_preemption();
	spllo();

#if	MACH_ASSERT
	if (watchacts & WA_SCHED) {
		printf("thread_continue(old=%x) thr_self=%x, swap_func=%x\n",
			old_thread, self, continuation);
	}
#endif	/* MACH_ASSERT */

	(*continuation)();
	/*NOTREACHED*/
}

#define THREAD_LOG_SIZE		300

#include <i386/AT386/cyctm05.h>

struct {
	struct t64	stamp;
	thread_t	thread;
	long		info1;
	long		info2;
	long		info3;
	char		* action;
} thread_log[THREAD_LOG_SIZE];

int		thread_log_index;

void		check_thread_time(long n);

#if	MACH_KDB
#include <ddb/db_output.h>
void		db_show_thread_log(void);

void
db_show_thread_log(void)
{
	int	i;

	db_printf ("%s %s %s %s %s %s\n", " Thread ", "  Info1 ", "  Info2 ",
			"  Info3 ", "    Timestamp    ", "Action");

	for (i = 0; i < THREAD_LOG_SIZE; i++) {
		db_printf ("%08x %08x %08x %08x %08x/%08x %s\n",
			thread_log[i].thread,
			thread_log[i].info1,
			thread_log[i].info2,
			thread_log[i].info3,
			thread_log[i].stamp.h,
			thread_log[i].stamp.l,
			thread_log[i].action);
	}
}
#endif	/* MACH_KDB */

int	check_thread_time_crash;

#if 0
void
check_thread_time(long us)
{
	struct t64	temp;

	if (!check_thread_time_crash)
		return;

	temp = thread_log[0].stamp;
	cyctm05_diff (&thread_log[1].stamp, &thread_log[0].stamp, &temp);

	if (temp.l >= us && thread_log[1].info != 0x49) /* HACK!!! */
		panic ("check_thread_time");
}
#endif

void
log_thread_action(char * action, long info1, long info2, long info3)
{
	int	i;
	spl_t	x;
	static int	tstamp;

#if 0
	if (!cyctm05_initialized)
		return;
#endif

	x = sploff();

	for (i = THREAD_LOG_SIZE-1; i > 0; i--) {
		thread_log[i] = thread_log[i-1];
	}

#if 0
	do {
		cyctm05_stamp_masked (&thread_log[0].stamp);

	} while ((thread_log[0].stamp.l & 0x0000ffff) == 0);
#else
	thread_log[0].stamp.h = 0;
	thread_log[0].stamp.l = tstamp++;
#endif
	thread_log[0].thread = current_thread();
	thread_log[0].info1 = info1;
	thread_log[0].info2 = info2;
	thread_log[0].info3 = info3;
	thread_log[0].action = action;
/*	strcpy (&thread_log[0].action[0], action);*/

	splon(x);
}

/*
 *	thread_block_reason:
 *
 *	Block the current thread.  If the thread is runnable
 *	then someone must have woken it up between its request
 *	to sleep and now.  In this case, it goes back on a
 *	run queue.
 *
 *	If a continuation is specified, then thread_block will
 *	attempt to discard the thread's kernel stack.  When the
 *	thread resumes, it will execute the continuation function
 *	on a new kernel stack.
 */

void
thread_block_reason(
	void	(*continuation)(void),
	int	reason)
{
	register thread_t thread = current_thread();
	register processor_t myprocessor = cpu_to_processor(cpu_number());
	register thread_t new_thread;
	spl_t		s;
	int             ast_flags;

	s = splsched();

	thread_lock(thread);
	/* NB: dependency: NOT_AT_SAFE_POINT == (void (*)(void)) 0 */
	thread->at_safe_point = (int) continuation;
	thread_unlock(thread);
	continuation = (void (*)(void)) 0;

#if	FAST_TAS
	{
		extern int recover_ras();

		if (csw_needed(thread, myprocessor))
			recover_ras(thread);
	}
#endif	/* FAST_TAS */

	/* Unconditionally remove either | both */
	ast_off(cpu_number(), (AST_QUANTUM|AST_BLOCK|AST_URGENT));

	new_thread = thread_select(myprocessor);
	assert(new_thread);
	thread_invoke(thread, continuation, new_thread, reason);

	splx(s);
}

/*
 *	thread_block:
 *
 *	Now calls thread_block_reason() which forwards the
 *	the reason parameter to thread_invoke() so it can
 *	do the right thing if the thread's quantum expired.
 */
void
thread_block(
	void	(*continuation)(void))
{
	thread_block_reason(continuation, 0);
}

/*
 *	thread_run:
 *
 *	Switch directly from the current thread to a specified
 *	thread.  Both the current and new threads must be
 *	runnable.
 */
void
thread_run(
	void			(*continuation)(void),
	register thread_t	new_thread)
{
	register thread_t thread = current_thread();
	register processor_t myprocessor = cpu_to_processor(cpu_number());
	spl_t	s;

#if	MACH_ASSERT
	if (watchacts & WA_SWITCH)
		printf("thread_run(cont=%x,thr=%x) self=%x\n",
			continuation, new_thread, thread);
#endif	/* MACH_ASSERT */

	s = splsched();
	thread_invoke(thread, continuation, new_thread, 0);
	splx(s);

#if	MACH_ASSERT
	if (watchacts & WA_SWITCH)
		printf("\tthread_run BACK AS thr=%x\n", current_thread());
#endif	/* MACH_ASSERT */
}

/*
 *	Dispatches a running thread that is not	on a runq.
 *	Called at splsched.
 */

void
thread_dispatch(
	register thread_t	thread)
{
	/*
	 *	If we are discarding the thread's stack, we must do it
	 *	before the thread has a chance to run.
	 */

#if	MACH_ASSERT
	if (watchacts & WA_SWITCH)
		printf("\tthread_dispatch(thr=%x)\n", thread);
#endif	/* MACH_ASSERT */

	thread_lock(thread);

	assert( thread->swap_func == (void (*)(void)) 0 );

	switch (thread->state &~ TH_SWAP_STATE) {
	    case TH_RUN		  | TH_SUSP:
	    case TH_RUN		  | TH_SUSP | TH_HALTED:
	    case TH_RUN | TH_WAIT | TH_SUSP:
		/*
		 *	Suspend the thread
		 */
		thread->state &= ~TH_RUN;
		wake_lock(thread);
		if (thread->wake_active) {
			thread->wake_active = FALSE;
			wake_unlock(thread);
			thread_unlock(thread);
			thread_wakeup((int)&thread->wake_active);
			return;
		}
		wake_unlock(thread);
		break;

	    case TH_RUN		  | TH_SUSP | TH_UNINT:
	    case TH_RUN			    | TH_UNINT:
	    case TH_RUN:
		/*
		 *	No reason to stop.  Put back on a run queue.
		 */
		thread_setrun(thread, FALSE, 
			      (thread->reason & AST_QUANTUM) ? TAIL_Q : HEAD_Q);
		break;

	    case TH_RUN | TH_WAIT | TH_SUSP | TH_UNINT:
	    case TH_RUN | TH_WAIT	    | TH_UNINT:
	    case TH_RUN | TH_WAIT:
	    case	  TH_WAIT:			/* this happens! */
	
		/*
		 *	Waiting, and not suspended.
		 */
		thread->state &= ~TH_RUN;
		break;

	    case TH_RUN | TH_IDLE:
		/*
		 *	Drop idle thread -- it is already in
		 *	idle_thread_array.
		 */
		break;

	    default:
		panic("State 0x%x \n",thread->state);
	}
	thread_unlock(thread);
}

/*
 *	Define shifts for simulating (5/8)**n
 */

shift_data_t	wait_shift[32] = {
	{1,1},{1,3},{1,-3},{2,-7},{3,5},{3,-5},{4,-8},{5,7},
	{5,-7},{6,-10},{7,10},{7,-9},{8,-11},{9,12},{9,-11},{10,-13},
	{11,14},{11,-13},{12,-15},{13,17},{13,-15},{14,-17},{15,19},{16,18},
	{16,-19},{17,22},{18,20},{18,-20},{19,26},{20,22},{20,-22},{21,-27}};

/*
 *	do_priority_computation:
 *
 *	Calculate new priority for thread based on its base priority plus
 *	accumulated usage.  PRI_SHIFT and PRI_SHIFT_2 convert from
 *	usage to priorities.  SCHED_SHIFT converts for the scaling
 *	of the sched_usage field by SCHED_SCALE.  This scaling comes
 *	from the multiplication by sched_load (thread_timer_delta)
 *	in sched.h.  sched_load is calculated as a scaled overload
 *	factor in compute_mach_factor (mach_factor.c).
 */
#ifdef	PRI_SHIFT_2
#if	PRI_SHIFT_2 > 0
#define do_priority_computation(th, pri)				\
	MACRO_BEGIN							\
	(pri) = (th)->priority	/* start with base priority */		\
	    + ((th)->sched_usage >> (PRI_SHIFT + SCHED_SHIFT))		\
	    + ((th)->sched_usage >> (PRI_SHIFT_2 + SCHED_SHIFT));	\
	if ((pri) > LPRI) (pri) = LPRI;					\
	MACRO_END
#else	/* PRI_SHIFT_2 */
#define do_priority_computation(th, pri)				\
	MACRO_BEGIN							\
	(pri) = (th)->priority	/* start with base priority */		\
	    + ((th)->sched_usage >> (PRI_SHIFT + SCHED_SHIFT))		\
	    - ((th)->sched_usage >> (SCHED_SHIFT - PRI_SHIFT_2));	\
	if ((pri) > LPRI) (pri) = LPRI;					\
	MACRO_END
#endif	/* PRI_SHIFT_2 */
#else	/* defined(PRI_SHIFT_2) */
#define do_priority_computation(th, pri)				\
	MACRO_BEGIN							\
	(pri) = (th)->priority	/* start with base priority */		\
	    + ((th)->sched_usage >> (PRI_SHIFT + SCHED_SHIFT));		\
	if ((pri) > LPRI) (pri) = LPRI;					\
	MACRO_END
#endif	/* defined(PRI_SHIFT_2) */

/*
 *	compute_priority:
 *
 *	Compute the effective priority of the specified thread.
 *	The effective priority computation is as follows:
 *
 *	Take the base priority for this thread and add
 *	to it an increment derived from its cpu_usage.
 *
 *	The thread *must* be locked by the caller. 
 */

void
compute_priority(
	register thread_t	thread,
        int			resched)
{
	register int	pri;

	if (thread->policy == POLICY_TIMESHARE) {
	    do_priority_computation(thread, pri);
	    if (thread->depress_priority < 0)
		set_pri(thread, pri, resched);
	    else
		thread->depress_priority = pri;
	}
	else {
	    set_pri(thread, thread->priority, resched);
	}
}

/*
 *	compute_my_priority:
 *
 *	Version of compute priority for current thread or thread
 *	being manipulated by scheduler (going on or off a runq).
 *	Only used for priority updates.  Policy or priority changes
 *	must call compute_priority above.  Caller must have thread
 *	locked and know it is timesharing and not depressed.
 */

void
compute_my_priority(
	register thread_t	thread)
{
	register int temp_pri;

	do_priority_computation(thread,temp_pri);
	assert(thread->runq == RUN_QUEUE_NULL);
	thread->sched_pri = temp_pri;
}

/*
 *	recompute_priorities:
 *
 *	Update the priorities of all threads periodically.
 */
void
recompute_priorities(void)
{
	register thread_t	thread;

#if	SIMPLE_CLOCK
	int			new_usec;
#endif	/* SIMPLE_CLOCK */

	sched_tick++;		/* age usage one more time */
	set_timeout(&recompute_priorities_timer, hz);
#if	SIMPLE_CLOCK
	/*
	 *	Compensate for clock drift.  sched_usec is an
	 *	exponential average of the number of microseconds in
	 *	a second.  It decays in the same fashion as cpu_usage.
	 */
	new_usec = sched_usec_elapsed();
	sched_usec = (5*sched_usec + 3*new_usec)/8;
#endif	/* SIMPLE_CLOCK */
	/*
	 *	Wakeup scheduler thread.
	 */
	thread = sched_thread_id;
	if (thread != THREAD_NULL) {
		clear_wait(thread, THREAD_AWAKENED, FALSE);
		if (sched_tick - thread->sched_stamp > 1) {
			spl_t	s = splsched();
			thread_lock(thread);
			if (sched_tick - thread->sched_stamp > 1) {
				update_priority(thread);
			}
			thread_unlock(thread);
			splx(s);
		}
	}
}

/*
 *	update_priority
 *
 *	Cause the priority computation of a thread that has been 
 *	sleeping or suspended to "catch up" with the system.  Thread
 *	*MUST* be locked by caller.  If thread is running, then this
 *	can only be called by the thread on itself.
 */
void
update_priority(
	register thread_t	thread)
{
	register unsigned int	ticks;
	register shift_t	shiftp;
	register int		temp_pri;

	ticks = sched_tick - thread->sched_stamp;
	assert(ticks != 0);

	/*
	 *	If asleep for more than 30 seconds forget all
	 *	cpu_usage, else catch up on missed aging.
	 *	5/8 ** n is approximated by the two shifts
	 *	in the wait_shift array.
	 */
	thread->sched_stamp += ticks;
	thread_timer_delta(thread);
	if (ticks >  30) {
		thread->cpu_usage = 0;
		thread->sched_usage = 0;
	}
	else {
		thread->cpu_usage += thread->cpu_delta;
		thread->sched_usage += thread->sched_delta;
		shiftp = &wait_shift[ticks];
		if (shiftp->shift2 > 0) {
		    thread->cpu_usage =
			(thread->cpu_usage >> shiftp->shift1) +
			(thread->cpu_usage >> shiftp->shift2);
		    thread->sched_usage =
			(thread->sched_usage >> shiftp->shift1) +
			(thread->sched_usage >> shiftp->shift2);
		}
		else {
		    thread->cpu_usage =
			(thread->cpu_usage >> shiftp->shift1) -
			(thread->cpu_usage >> -(shiftp->shift2));
		    thread->sched_usage =
			(thread->sched_usage >> shiftp->shift1) -
			(thread->sched_usage >> -(shiftp->shift2));
		}
	}
	thread->cpu_delta = 0;
	thread->sched_delta = 0;

	/*
	 *	Recompute priority if appropriate.
	 */
	if (
	    (thread->policy == POLICY_TIMESHARE) &&
	    (thread->depress_priority < 0)) {
		run_queue_t	rq;

		do_priority_computation(thread, temp_pri);
		rq = rem_runq(thread);
		thread->sched_pri = temp_pri;
		if (rq != RUN_QUEUE_NULL)
			thread_setrun(thread, TRUE, TAIL_Q);
	}
}

/*
 * Enqueue thread on run_queue.
 */
int
run_queue_enqueue(
	register run_queue_t	rq,
	register thread_t	th,
	boolean_t		tail)
{
	register unsigned int	whichq;
	int			oldrqcount;

	whichq = (th)->sched_pri;
	if (whichq < 0 || whichq > MINPRI) {
		panic("run_queue_enqueue: bad pri (%d)\n", whichq);
	}

	spinlock_lock(&(rq)->lock);	/* lock the run queue */
#if	DEBUG
	checkrq((rq), "run_queue_enqueue: before adding thread");
#endif	/* DEBUG */
	assert(th->runq == RUN_QUEUE_NULL);
	if ( tail ) {
		enqueue_tail(&(rq)->runq[whichq], (queue_entry_t) (th));
	} else {
		enqueue_head(&(rq)->runq[whichq], (queue_entry_t) (th));
	}
	setbit(whichq, (rq)->bitmap);
	if (whichq < (rq)->low) {
		(rq)->low = whichq;
	}
	oldrqcount = (rq)->count++;
	(th)->runq = (rq);
#if	DEBUG
	thread_check((th), (rq));
	checkrq((rq), "run_queue_enqueue: after adding thread");
#endif	/* DEBUG */
	spinlock_unlock(&(rq)->lock);
	return( oldrqcount );
}

/*
 *	thread_setrun:
 *
 *	Make thread runnable; dispatch directly onto an idle processor
 *	if possible.  Else put on appropriate run queue (processor
 *	if bound, else processor set.  Caller must have lock on thread.
 *	This is always called at splsched.
 *	The tail parameter, if TRUE || TAIL_Q, indicates that the 
 *	thread should be placed at the tail of the runq. If 
 *	FALSE || HEAD_Q the thread will be placed at the head of the 
 *      appropriate runq.
 */
void
thread_setrun(
	register thread_t	th,
	boolean_t		may_preempt,
	boolean_t		tail)
{
	register processor_t	processor;
	register run_queue_t	rq;
#if	NCPUS > 1
	thread_t		cur_th;
	register processor_set_t	pset;
#endif	/* NCPUS > 1 */
	ast_t			ast_flags = AST_BLOCK;

	/*
	 *	Update priority if needed.
	 */
	if (th->sched_stamp != sched_tick) {
		update_priority(th);
	}

	assert(th->runq == RUN_QUEUE_NULL);

	th->at_safe_point = NOT_AT_SAFE_POINT;
	
#if MACH_RT
        if ((th->policy == POLICY_FIFO || th->policy == POLICY_RR) &&
			th->priority < BASEPRI_SYSTEM) {
		ast_flags |= AST_URGENT;
#if 0
		log_thread_action ((long)th, "thread_setrun");
#endif
	}
#endif /* MACH_RT */

#if	NCPUS > 1
	/*
	 *	Try to dispatch the thread directly onto an idle processor.
	 */
	if ((processor = th->bound_processor) == PROCESSOR_NULL) {
	    /*
	     *	Not bound, any processor in the processor set is ok.
	     */
	    pset = th->processor_set;
#if	HW_FOOTPRINT
	    /*
	     *	But first check the last processor it ran on.
	     */
	    processor = th->last_processor;
	    if (processor->state == PROCESSOR_IDLE) {
		    spinlock_lock(&processor->lock);
		    spinlock_lock(&pset->idle_lock);
		    if ((processor->state == PROCESSOR_IDLE)
#if	MACH_HOST
			&& (processor->processor_set == pset)
#endif	/* MACH_HOST */
			) {
			    queue_remove(&pset->idle_queue, processor,
			        processor_t, processor_queue);
			    pset->idle_count--;
			    processor->next_thread = th;
			    processor->state = PROCESSOR_DISPATCHING;
			    spinlock_unlock(&pset->idle_lock);
			    spinlock_unlock(&processor->lock);
		            return;
		    }
		    spinlock_unlock(&pset->idle_lock);
		    spinlock_unlock(&processor->lock);
	    }
#endif	/* HW_FOOTPRINT */

	    if (pset->idle_count > 0) {
		spinlock_lock(&pset->idle_lock);
		if (pset->idle_count > 0) {
		    processor = (processor_t) queue_first(&pset->idle_queue);
		    queue_remove(&(pset->idle_queue), processor, processor_t,
				processor_queue);
		    pset->idle_count--;
		    processor->next_thread = th;
		    processor->state = PROCESSOR_DISPATCHING;
		    spinlock_unlock(&pset->idle_lock);
		    return;
		}
		spinlock_unlock(&pset->idle_lock);
	    }
	    rq = &(pset->runq);
	    (void)run_queue_enqueue(rq, th, tail);
	    /*
	     * Preempt check
	     */
	    processor = current_processor();
	    if (may_preempt &&
#if	MACH_HOST
		(pset == processor->processor_set) &&
#endif	/* MACH_HOST */
		((cur_th = current_thread())->sched_pri > th->sched_pri)) {

		    /*
		     * XXX if we have a non-empty local runq or are
		     * XXX running a bound thread, ought to check for
		     * XXX another cpu running lower-pri thread to preempt.
		     *
		     *	Turn off first_quantum to allow csw.
		     */
		    processor->first_quantum = FALSE;
		    ast_on(cpu_number(), ast_flags);
	    }
	}
	else {
	    int		oldrqcount;
	    /*
	     *	Bound, can only run on bound processor.  Have to lock
	     *  processor here because it may not be the current one.
	     */
	    if (processor->state == PROCESSOR_IDLE) {
		spinlock_lock(&processor->lock);
		pset = processor->processor_set;
		spinlock_lock(&pset->idle_lock);
		if (processor->state == PROCESSOR_IDLE) {
		    queue_remove(&pset->idle_queue, processor,
			processor_t, processor_queue);
		    pset->idle_count--;
		    processor->next_thread = th;
		    processor->state = PROCESSOR_DISPATCHING;
		    spinlock_unlock(&pset->idle_lock);
		    spinlock_unlock(&processor->lock);
		    return;
		}
		spinlock_unlock(&pset->idle_lock);
		spinlock_unlock(&processor->lock);
	    }
	    rq = &(processor->runq);
	    oldrqcount = run_queue_enqueue(rq, th, tail);

	    /*
	     * Cause ast on processor if processor is on line, and
	     * the currently executing thread on that cpu is not bound
	     * to that processor (bound threads have implicit priority
	     * over non-bound threads).  We automatically avoid the
	     * idle_thread this way, since it's bound.
	     */
	    if (processor == current_processor()) {
		    ast_on(cpu_number(), ast_flags);
	    } else {    thread_t th;
		    if ( oldrqcount == 0
			    && processor->state != PROCESSOR_OFF_LINE
			    && (th = active_threads[processor->slot_num])
			    && (th->bound_processor != processor)) {
				ast_on(processor->slot_num, ast_flags);
				cause_ast_check(processor);
		    }
	    }
	}
#else	/* NCPUS > 1 */
	/*
	 *	XXX should replace queue with a boolean in this case.
	 */
	if (default_pset.idle_count > 0) {
	    processor = (processor_t) queue_first(&default_pset.idle_queue);
	    queue_remove(&default_pset.idle_queue, processor,
		processor_t, processor_queue);
	    default_pset.idle_count--;
	    processor->next_thread = th;
	    processor->state = PROCESSOR_DISPATCHING;
	    return;
	}

	rq = &(default_pset.runq);
	(void)run_queue_enqueue(rq, th, tail);

	/*
	 * Preempt check
	 */
	if (may_preempt && (current_thread()->sched_pri > th->sched_pri)) {
		/*
		 *	Turn off first_quantum to allow context switch.
		 */
		current_processor()->first_quantum = FALSE;
		ast_on(cpu_number(), ast_flags);
	}
#endif	/* NCPUS > 1 */
}

/*
 *	set_pri:
 *
 *	Set the priority of the specified thread to the specified
 *	priority.  This may cause the thread to change queues.
 *
 *	The thread *must* be locked by the caller.
 */

void
set_pri(
	thread_t	th,
	int		pri,
	int		resched)
{
	register struct run_queue	*rq;

	rq = rem_runq(th);
	assert(th->runq == RUN_QUEUE_NULL);
	th->sched_pri = pri;
	if (rq != RUN_QUEUE_NULL) {
	    if (resched)
		thread_setrun(th, TRUE, TAIL_Q);
	    else
		(void)run_queue_enqueue(rq, th, TAIL_Q);
	}
}

/*
 *	rem_runq:
 *
 *	Remove a thread from its run queue.
 *	The run queue that the process was on is returned
 *	(or RUN_QUEUE_NULL if not on a run queue).  Thread *must* be locked
 *	before calling this routine.  Unusual locking protocol on runq
 *	field in thread structure makes this code interesting; see thread.h.
 */

run_queue_t
rem_runq(
	thread_t	th)
{
	register struct run_queue	*rq;

	rq = th->runq;
	/*
	 *	If rq is RUN_QUEUE_NULL, the thread will stay out of the
	 *	run_queues because the caller locked the thread.  Otherwise
	 *	the thread is on a runq, but could leave.
	 */
	if (rq != RUN_QUEUE_NULL) {
		thread_t	t;
		int		whichq;

		spinlock_lock(&rq->lock);
#if	DEBUG
		checkrq(rq, "rem_runq: at entry");
#endif	/* DEBUG */
		if (rq == th->runq) {
			/*
			 *	Thread is in a runq and we have a lock on
			 *	that runq.
			 */
#if	DEBUG
			checkrq(rq, "rem_runq: before removing thread");
			thread_check(th, rq);
#endif	/* DEBUG */
			remqueue(&rq->runq[0], (queue_entry_t) th);
			rq->count--;
#if	DEBUG
			/* find in which queue the thread was */
			for (t = (thread_t) th->links.next;
			     (queue_head_t *) t < &rq->runq[0] ||
			     (queue_head_t *) t > &rq->runq[MINPRI];
			     t = (thread_t) t->links.next);
			whichq = ((queue_head_t *) t) - &rq->runq[0];
			if (whichq != th->sched_pri) {
				panic("rem_runq: whichq %d != sched_pri %d\n",
				      whichq, th->sched_pri);
			}
#endif	/* DEBUG */
			if (queue_empty(rq->runq + th->sched_pri)) {
				/* update run queue status */
				clrbit(th->sched_pri, rq->bitmap);
				rq->low = ffsbit(rq->bitmap);
			}
#if	DEBUG
			checkrq(rq, "rem_runq: after removing thread");
#endif	/* DEBUG */
			th->runq = RUN_QUEUE_NULL;
			spinlock_unlock(&rq->lock);
		}
		else {
			/*
			 *	The thread left the runq before we could
			 * 	lock the runq.  It is not on a runq now, and
			 *	can't move again because this routine's
			 *	caller locked the thread.
			 */
			assert(th->runq == RUN_QUEUE_NULL);
			spinlock_unlock(&rq->lock);
			rq = RUN_QUEUE_NULL;
		}
	}

	return(rq);
}

#if	NCPUS > 1
/*
 *	choose_thread:
 *
 *	Choose a thread to execute.  The thread chosen is removed
 *	from its run queue.  Note that this requires only that the runq
 *	lock be held.
 *
 *	Strategy:
 *		Check processor runq first; if anything found, run it.
 *		Else check pset runq; if nothing found, return idle thread.
 *
 *	Second line of strategy is implemented by choose_pset_thread.
 *	This is only called on processor startup and when thread_block
 *	thinks there's something in the processor runq.
 */

thread_t
choose_thread(
	processor_t		myprocessor)
{
	thread_t		th;
	register queue_t	q;
	register run_queue_t	runq;
	processor_set_t		pset;

	runq = &myprocessor->runq;
	pset = myprocessor->processor_set;

	spinlock_lock(&runq->lock);
	if (runq->count > 0 && runq->low <= pset->runq.low) {
		q = runq->runq + runq->low;
#if	MACH_ASSERT
		if (!queue_empty(q)) {
#endif	/*MACH_ASSERT*/
			th = (thread_t)q->next;
			((queue_entry_t)th)->next->prev = q;
			q->next = ((queue_entry_t)th)->next;
			th->runq = RUN_QUEUE_NULL;
			runq->count--;
			if (queue_empty(q)) {
				clrbit(runq->low, runq->bitmap);
				runq->low = ffsbit(runq->bitmap);
			}
			spinlock_unlock(&runq->lock);
			return(th);
#if	MACH_ASSERT
		}
		panic("choose_thread");
#endif	/*MACH_ASSERT*/
		/*NOTREACHED*/
	}
	spinlock_unlock(&runq->lock);
	spinlock_lock(&pset->runq.lock);
	return(choose_pset_thread(myprocessor, pset));
}
#endif	/*NCPUS > 1*/

/*
 *	choose_pset_thread:  choose a thread from processor_set runq or
 *		set processor idle and choose its idle thread.
 *
 *	Caller must be at splsched and have a lock on the runq.  This
 *	lock is released by this routine.  myprocessor is always the current
 *	processor, and pset must be its processor set.
 *	This routine chooses and removes a thread from the runq if there
 *	is one (and returns it), else it sets the processor idle and
 *	returns its idle thread.
 */

thread_t
choose_pset_thread(
	register processor_t	myprocessor,
	processor_set_t		pset)
{
	register run_queue_t	runq;
	register thread_t	th;
	register queue_t	q;
	int			i;

	runq = &pset->runq;
	if (runq->count > 0) {
		q = runq->runq + runq->low;
#if	MACH_ASSERT
		if (!queue_empty(q)) {
#endif	/*MACH_ASSERT*/
			th = (thread_t)q->next;
			((queue_entry_t)th)->next->prev = q;
			q->next = ((queue_entry_t)th)->next;
			th->runq = RUN_QUEUE_NULL;
			runq->count--;
			if (queue_empty(q)) {
				clrbit(runq->low, runq->bitmap);
				runq->low = ffsbit(runq->bitmap);
			}
			spinlock_unlock(&runq->lock);
			return(th);
#if	MACH_ASSERT
		}
		panic("choose_pset_thread");
#endif	/*MACH_ASSERT*/
		/*NOTREACHED*/
	}
	spinlock_unlock(&runq->lock);

	/*
	 *	Nothing is runnable, so set this processor idle if it
	 *	was running.  If it was in an assignment or shutdown,
	 *	leave it alone.  Return its idle thread.
	 */
	spinlock_lock(&pset->idle_lock);
	if (myprocessor->state == PROCESSOR_RUNNING) {
	    myprocessor->state = PROCESSOR_IDLE;
	    /*
	     *	XXX Until it goes away, put master on end of queue, others
	     *	XXX on front so master gets used last.
	     */
	    if (myprocessor == master_processor) {
		queue_enter(&(pset->idle_queue), myprocessor,
			processor_t, processor_queue);
	    }
	    else {
		queue_enter_first(&(pset->idle_queue), myprocessor,
			processor_t, processor_queue);
	    }

	    pset->idle_count++;
	}
	spinlock_unlock(&pset->idle_lock);
	return(myprocessor->idle_thread);

#undef	pset
}

/*
 *	no_dispatch_count counts number of times processors go non-idle
 *	without being dispatched.  This should be very rare.
 */
int	no_dispatch_count = 0;

/*
 *	This is the idle thread, which just looks for other threads
 *	to execute.
 */

void
idle_thread_continue(void)
{
	register processor_t myprocessor;
	register volatile thread_t *threadp;
	register volatile int *gcount;
#if	NCPUS > 1
	register volatile int *lcount;
#endif	/*NCPUS > 1*/
	register thread_t new_thread;
	register int state;
	int	mycpu;
	spl_t	s;
	extern spl_t    curr_ipl[];

	mycpu = cpu_number();
	myprocessor = current_processor();
	threadp = (volatile thread_t *) &myprocessor->next_thread;
#if	NCPUS > 1
	lcount = (volatile int *) &myprocessor->runq.count;
#endif	/*NCPUS > 1*/

	while (TRUE) {
#if 0
		if (curr_ipl[cpu_number()])
			panic ("Idle thread at spl > 0?");
#endif
#ifdef	MARK_CPU_IDLE
		MARK_CPU_IDLE(mycpu);
#endif	/* MARK_CPU_IDLE */

#if	MACH_HOST
		gcount = (volatile int *)
				&myprocessor->processor_set->runq.count;
#else	/* MACH_HOST */
		gcount = (volatile int *) &default_pset.runq.count;
#endif	/* MACH_HOST */

/*
 *	This cpu will be dispatched (by thread_setrun) by setting next_thread
 *	to the value of the thread to run next.  Also check runq counts.
 */
		while ((*threadp == (volatile thread_t)THREAD_NULL) &&
		       (*gcount == 0) 
#if	NCPUS > 1
		       && (*lcount == 0)
#endif	/*NCPUS > 1*/
		       ) {

			/* check for ASTs while we wait */

			if (need_ast[mycpu] &~ AST_SCHEDULING) {
				s = splsched();
				/* don't allow scheduling ASTs */
				need_ast[mycpu] &= ~AST_SCHEDULING;
				ast_taken(FALSE, AST_ALL, s);
				/* back at spllo */
			}

			/*
			 * machine_idle is a machine dependent function,
			 * to conserver power.
			 */
#if	POWER_SAVE
			machine_idle(mycpu);
#endif /* POWER_SAVE */
		}

#ifdef	MARK_CPU_ACTIVE
		MARK_CPU_ACTIVE(mycpu);
#endif	/* MARK_CPU_ACTIVE */

		s = splsched();

		/*
		 *	This is not a switch statement to avoid the
		 *	bounds checking code in the common case.
		 */
retry:
		state = myprocessor->state;
		if (state == PROCESSOR_DISPATCHING) {
			/*
			 *	Commmon case -- cpu dispatched.
			 */
			new_thread = (thread_t) *threadp;
			*threadp = (volatile thread_t) THREAD_NULL;
			myprocessor->state = PROCESSOR_RUNNING;
			/*
			 *	set up quantum for new thread.
			 */
			if (new_thread->policy == POLICY_TIMESHARE) {
				/*
				 *  Just use set quantum.  No point in
				 *  checking for shorter local runq quantum;
				 *  csw_needed will handle correctly.
				 */
#if	MACH_HOST
				myprocessor->quantum = new_thread->
					processor_set->set_quantum;
#else	/* MACH_HOST */
				myprocessor->quantum =
					default_pset.set_quantum;
#endif	/* MACH_HOST */
			}
			else {
				/*
				 *	POLICY_RR || POLICY_FIFO
				 */
				myprocessor->quantum = new_thread->sched_data;
			}
			myprocessor->first_quantum = TRUE;
			counter(c_idle_thread_handoff++);
			thread_run((void(*)(void))0, new_thread);
		}
		else if (state == PROCESSOR_IDLE) {
			register processor_set_t pset;

			pset = myprocessor->processor_set;
			spinlock_lock(&pset->idle_lock);
			if (myprocessor->state != PROCESSOR_IDLE) {
				/*
				 *	Something happened, try again.
				 */
				spinlock_unlock(&pset->idle_lock);
				goto retry;
			}
			/*
			 *	Processor was not dispatched (Rare).
			 *	Set it running again.
			 */
			no_dispatch_count++;
			pset->idle_count--;
			queue_remove(&pset->idle_queue, myprocessor,
				processor_t, processor_queue);
			myprocessor->state = PROCESSOR_RUNNING;
			spinlock_unlock(&pset->idle_lock);
			counter(c_idle_thread_block++);
			thread_block((void(*)(void))0);
		}
		else if ((state == PROCESSOR_ASSIGN) ||
			 (state == PROCESSOR_SHUTDOWN)) {
			/*
			 *	Changing processor sets, or going off-line.
			 *	Release next_thread if there is one.  Actual
			 *	thread to run in on a runq.
			 */
			if ((new_thread = (thread_t)*threadp)!= THREAD_NULL) {
				*threadp = (volatile thread_t) THREAD_NULL;
				thread_setrun(new_thread, FALSE, TAIL_Q);
			}

			counter(c_idle_thread_block++);
			thread_block((void(*)(void))0);
		}
		else {
			printf(" Bad processor state %d (Cpu %d)\n",
				cpu_state(mycpu), mycpu);
			panic("idle_thread");
		}

		(void) splx(s);
	}
}

void
idle_thread(void)
{
	register thread_t	self = current_thread();
	spl_t			s;

	stack_privilege(self);

	s = splsched();
	self->priority = IDLEPRI;
	self->sched_pri = IDLEPRI;
	self->policy = POLICY_RR;

	/*
	 *	Set the idle flag to indicate that this is an idle thread,
	 *	enter ourselves in the idle array, and thread_block() to get
	 *	out of the run queues (and set the processor idle when we
	 *	run next time).
	 */
	thread_lock(self);
	self->state |= TH_IDLE;
	current_processor()->idle_thread = self;
	thread_unlock(self);
	(void) splx(s);

	counter(c_idle_thread_block++);
	thread_block((void(*)(void))0);
	idle_thread_continue();
	/*NOTREACHED*/

	panic("idle_thread_continue!");
}

/*
 *	sched_thread: scheduler thread.
 *
 *	This thread handles periodic calculations in the scheduler that
 *	we don't want to do at interrupt level.  This allows us to
 *	avoid blocking 
 */
void
sched_thread(void)
{
    sched_thread_id = current_thread();

    /*
     *	Sleep on event NO_EVENT, recompute_priorities() will awaken
     *	us by calling clear_wait().
     */
#if	MACH_ASSERT
    if (watchacts & WA_BOOT)
		printf("sched_thread RUNNING\n");
#endif	/* MACH_ASSERT */

    assert_wait(0, FALSE);
    counter(c_sched_thread_block++);
    thread_block((void(*)(void))0);

    while (TRUE) {
	(void) compute_mach_factor();

	/*
	 *	Check for stuck threads.  This can't be done off of
	 *	the callout queue because it requires operations that
	 *	can't be used from interrupt level.
	 */
	if (sched_tick & 1)
	    	do_thread_scan();

	assert_wait(0, FALSE);
	counter(c_sched_thread_block++);
	thread_block((void(*)(void))0);
    }
    /*NOTREACHED*/
}

#define	MAX_STUCK_THREADS	64

/*
 *	do_thread_scan: scan for stuck threads.  A thread is stuck if
 *	it is runnable but its priority is so low that it has not
 *	run for several seconds.  Its priority should be higher, but
 *	won't be until it runs and calls update_priority.  The scanner
 *	finds these threads and does the updates.
 *
 *	Scanner runs in two passes.  Pass one squirrels likely
 *	thread ids away in an array  (takes out references for them).
 *	Pass two does the priority updates.  This is necessary because
 *	the run queue lock is required for the candidate scan, but
 *	cannot be held during updates [set_pri will deadlock].
 *
 *	Array length should be enough so that restart isn't necessary,
 *	but restart logic is included.  Does not scan processor runqs.
 *
 */

thread_t		stuck_threads[MAX_STUCK_THREADS];
int			stuck_count = 0;

/*
 *	do_runq_scan is the guts of pass 1.  It scans a runq for
 *	stuck threads.  A boolean is returned indicating whether
 *	it ran out of space.
 */

boolean_t
do_runq_scan(
	run_queue_t	runq)
{
	spl_t			s;
	register queue_t	q;
	register thread_t	thread;
	register int		count;

	s = splsched();
	spinlock_lock(&runq->lock);
	if((count = runq->count) > 0) {
	    q = runq->runq + runq->low;
	    while (count > 0) {
		queue_iterate(q, thread, thread_t, links) {
		    if ((thread->state & (TH_WAIT|TH_SUSP)) == 0 &&
			sched_tick - thread->sched_stamp > 1) {
			    /*
			     *	Stuck, save its id for later.
			     */
			    if (stuck_count == MAX_STUCK_THREADS) {
				/*
				 *	!@#$% No more room.
				 */
				spinlock_unlock(&runq->lock);
				splx(s);
				return(TRUE);
			    }
			    /*
			     *	Inline version of thread_reference
			     */
			    thread_lock(thread);
			    thread->ref_count++;
			    thread_unlock(thread);
			    stuck_threads[stuck_count++] = thread;
		    }
		    count--;
		}
		q++;
	    }
	}
	spinlock_unlock(&runq->lock);
	splx(s);

	return(FALSE);
}

void
do_thread_scan(void)
{
	spl_t			s;
	register boolean_t	restart_needed = 0;
	register thread_t	thread;
#if	MACH_HOST
	register processor_set_t	pset;
#endif	/* MACH_HOST */

	do {
#if	MACH_HOST
	    mutex_lock(&all_psets_lock);
	    queue_iterate(&all_psets, pset, processor_set_t, all_psets) {
		if (restart_needed = do_runq_scan(&pset->runq))
			break;
	    }
	    mutex_unlock(&all_psets_lock);
#else	/* MACH_HOST */
	    restart_needed = do_runq_scan(&default_pset.runq);
#endif	/* MACH_HOST */
#if	NCPUS > 1
	    if (!restart_needed)
	    	restart_needed = do_runq_scan(&master_processor->runq);
#endif	/*NCPUS > 1*/

	    /*
	     *	Ok, we now have a collection of candidates -- fix them.
	     */

	    while (stuck_count > 0) {
		thread = stuck_threads[--stuck_count];
		stuck_threads[stuck_count] = THREAD_NULL;
		s = splsched();
		thread_lock(thread);
		if ((thread->state & (TH_WAIT|TH_SUSP)) == 0 &&
		    sched_tick - thread->sched_stamp > 1) {
			update_priority(thread);
		}
		thread_unlock(thread);
		splx(s);
		thread_deallocate(thread);
	    }

	} while (restart_needed);
}
		
/*
 *	Just in case someone doesn't use the macro
 */
#undef	thread_wakeup
void
thread_wakeup(
	int	x);

void
thread_wakeup(
	register int	x)
{
	thread_wakeup_with_result(x, THREAD_AWAKENED);
}

#if	DEBUG

void
checkrq(
	run_queue_t	rq,
	char		*msg)
{
	register queue_t	q1;
	register int		i, j;
	register queue_entry_t	e;
	register int		low;

	low = -1;
	j = 0;
	q1 = rq->runq;
	for (i = 0; i < NRQS; i++) {
	    if (q1->next == q1) {
		if (q1->prev != q1) {
		    panic("checkrq: empty at %s", msg);
	        }
	    }
	    else {
		if (low == -1)
		    low = i;
		
		for (e = q1->next; e != q1; e = e->next) {
		    j++;
		    if (e->next->prev != e)
			panic("checkrq-2 at %s", msg);
		    if (e->prev->next != e)
			panic("checkrq-3 at %s", msg);
		}
	    }
	    q1++;
	}
	if (j != rq->count)
	    panic("checkrq: count wrong at %s", msg);
	if (rq->count != 0 && low < rq->low)
	    panic("checkrq: low wrong at %s", msg);
}

void
thread_check(
	register thread_t	th,
	register run_queue_t	rq)
{
	register unsigned int 	whichq;

	whichq = th->sched_pri;
	if (whichq > MINPRI) {
		printf("thread_check: priority too high\n");
		whichq = MINPRI;
	}
	if ((th->links.next == &rq->runq[whichq]) &&
		(rq->runq[whichq].prev != (queue_entry_t)th))
			panic("thread_check");
}
#endif	/* DEBUG */
