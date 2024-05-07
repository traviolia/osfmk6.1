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
/* This file was formerly named kern/act.c; history is below:
 *
 * Revision 1.1.2.16  1994/02/26  17:38:36  bolinger
 * 	Add profiling initialization/finalization to activation
 * 	functions.
 * 	[1994/02/26  17:35:50  bolinger]
 *
 * Revision 1.1.2.15  1994/02/09  00:42:33  dwm
 * 	Put a variety of debugging code under MACH_ASSERT,
 * 	to enhance PROD performance a bit.
 * 	Promote switch_ktss() to act_machine_switch_pcb().
 * 	[1994/02/09  00:35:10  dwm]
 * 
 * Revision 1.1.2.14  1994/02/04  18:02:13  bolinger
 * 	Merge up to colo_shared.
 * 	[1994/02/04  17:58:20  bolinger]
 * 
 * 	Update AST context in act_attach() (as well as in act_detach());
 * 	check explicitly for null activation in user-callable interface
 * 	functions.  [dwm]
 * 	[1994/02/04  15:35:15  bolinger]
 * 
 * Revision 1.1.2.13  1994/02/04  03:46:08  condict
 * 	Return error in thread_terminate, if act is NULL.
 * 	[1994/02/04  03:41:40  condict]
 * 
 * 	Optimize act_attach/detach by removing act references.
 * 	Also, put if MACH_ASSERT around debugging code.
 * 	[1994/02/02  22:11:17  condict]
 * 
 * Revision 1.1.2.12  1994/02/02  18:58:52  bolinger
 * 	Add fix from Dave to check ast's in new current activation in
 * 	act_detach().  [dwm]
 * 	[1994/02/02  18:58:25  bolinger]
 * 
 * Revision 1.1.2.11  1994/01/31  16:30:07  bolinger
 * 	Make "pushing" and "popping" activations look more like
 * 	context switches (add pmap and kernel TSS manipulations).
 * 	[1994/01/31  16:29:47  bolinger]
 * 
 * Revision 1.1.2.10  1994/01/26  17:34:45  dwm
 * 	Change back to active_threads[] as root of current_*() access.
 * 	[1994/01/26  17:31:17  dwm]
 * 
 * Revision 1.1.2.9  1994/01/26  15:47:19  bolinger
 * 	Merge up to colo_shared.
 * 	[1994/01/25  22:57:40  bolinger]
 * 
 * 	Fix "kernel_loaded" handling for nested activations.  Update
 * 	kernel's cached idea of current kernel stack when necessary
 * 	in switching activations.
 * 	[1994/01/25  18:51:09  bolinger]
 * 
 * Revision 1.1.2.8  1994/01/25  18:38:01  dwm
 * 	move ast indicator into act from thread.
 * 	[1994/01/25  18:36:54  dwm]
 * 
 * Revision 1.1.2.7  1994/01/22  03:39:26  bolinger
 * 	Check for waiters (and awaken) when adding a thread to a
 * 	thread pool.
 * 	[1994/01/22  03:31:43  bolinger]
 * 
 * Revision 1.1.2.6  1994/01/21  23:45:35  dwm
 * 	Thread_pools are now embedded directly in ports/psets.
 * 	[1994/01/21  23:43:39  dwm]
 * 
 * Revision 1.1.2.5  1994/01/17  18:08:45  dwm
 * 	Tweak tracing code; nudge suspended act if it's terminated.
 * 	[1994/01/17  16:06:42  dwm]
 * 
 * Revision 1.1.2.4  1994/01/14  19:08:22  bolinger
 * 	In act_detach(), call thread_pool_wakeup() if anyone's waiting
 * 	for activations from the current pool.
 * 	[1994/01/14  19:07:57  bolinger]
 * 
 * Revision 1.1.2.2  1994/01/14  18:27:04  dwm
 * 	Coloc: clean up nudge() since we don't use kernel acts.
 * 	rearrange act_[gs]et_state to properly decide whether to
 * 	use blocking or non-blocking method, so (eg) signals work.
 * 	This need review if we start using kernel acts.
 * 	Comment out act_yank until it's both needed and finished.
 * 	[1994/01/14  17:59:20  dwm]
 * 
 * Revision 1.1.2.1  1994/01/12  17:53:07  dwm
 * 	Coloc: initial restructuring to follow Utah model.
 * 	removed MACH_IPC_COMPAT, added debug printf control, minor changes.
 * 	Some "in progress" code from Utah left in place for now.
 * 	[1994/01/12  17:15:16  dwm]
 */
/*
 * Copyright (c) 1993 The University of Utah and
 * the Computer Systems Laboratory (CSL).  All rights reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * THE UNIVERSITY OF UTAH AND CSL ALLOW FREE USE OF THIS SOFTWARE IN ITS "AS
 * IS" CONDITION.  THE UNIVERSITY OF UTAH AND CSL DISCLAIM ANY LIABILITY OF
 * ANY KIND FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * CSL requests users of this software to return to csl-dist@cs.utah.edu any
 * improvements that they make and grant CSL redistribution rights.
 *
 *	Author:	Bryan Ford, University of Utah CSS
 *
 *	Thread_Activation management routines
 */

#include <cpus.h>
#include <mach/kern_return.h>
#include <mach/alert.h>
#include <kern/mach_param.h>
#include <kern/zalloc.h>
#include <kern/thread.h>
#include <kern/task.h>
#include <kern/thread_act.h>
#include <kern/thread_pool.h>
#include <kern/misc_protos.h>
#include <kern/assert.h>
#include <kern/ipc_mig.h>
#include <kern/ipc_tt.h>
#include <kern/profile.h>
#include <kern/machine.h>
#include <kern/spl.h>
#include <kern/syscall_subr.h>
#include <kern/sync_lock.h>
#include <ipc/mach_msg.h>
#include <mach_prof.h>

/*
 * Debugging printf control
 */
#if	MACH_ASSERT
unsigned int	watchacts =	  0 /* WA_ALL */
				    ;	/* Do-it-yourself & patchable */
#endif

int *thread_states_counts;
int thread_flavor_max;

/*
 * Forward declarations for functions local to this file.
 */
void		special_handler(ReturnHandler *, thread_act_t);
void		install_special_handler(thread_act_t);
void		nudge(thread_act_t);
kern_return_t	act_set_state_locked(thread_act_t, int,
					thread_state_t,
					mach_msg_type_number_t);
kern_return_t	act_get_state_locked(thread_act_t, int,
					thread_state_t,
					mach_msg_type_number_t *);
void		act_set_apc(thread_act_t);
void		act_clr_apc(thread_act_t);
void		act_user_to_kernel(thread_act_t);
kern_return_t	act_disable_task_locked(thread_act_t);
void		act_ulock_release_all(thread_act_t thr_act);

kern_return_t	thread_halt(thread_act_t, int);
kern_return_t	thread_terminate_internal(thread_act_t, boolean_t);

static zone_t	thr_act_zone;

/*
 * Thread interfaces accessed via a thread_activation:
 */

/*
 * Terminate a thread, complete with deadlock checks.  Called with
 * nothing locked.  Returns same way.
 */
kern_return_t
thread_terminate(
	register thread_act_t	thr_act)
{
	kern_return_t		kr;

	if (thr_act == THR_ACT_NULL)
		return(KERN_INVALID_ARGUMENT);
	kr = thread_terminate_internal(thr_act, TRUE);
	return (kr);
}

/*
 * Terminate a thread with no deadlock checks.  Called with nothing
 * locked.  Returns same way.
 */
void
thread_force_terminate(
	register thread_act_t	thr_act)
{
	(void)thread_terminate_internal(thr_act, TRUE);
}

/*
 *	thread_terminate_internal:
 *
 *	Permanently stop execution of the specified thread.
 *
 *	A thread to be terminated must be allowed to clean up any state
 *	that it has before it exits.  The thread is broken out of any
 *	wait condition that it is in, and signalled to exit.  It then
 *	cleans up its state and calls thread_terminate_self (via its
 *	special_handler) on its way out of the kernel.  The caller waits
 *	here for the thread to halt. The target terminates its own IPC
 *	state.
 *
 *	If the caller is the current thread, it must still exit the kernel
 *	to clean up any state (thread and port references, messages, etc).
 *	When it exits the kernel, it then terminates its IPC state and
 *	queues itself for the reaper thread, which will wait for the thread
 *	to stop and then deallocate it.  (A thread cannot deallocate itself,
 *	since it needs a kernel stack to execute.)
 */
kern_return_t
thread_terminate_internal(
	register thread_act_t	thr_act,
	boolean_t		do_checks)
{
	register task_t		cur_task = current_task();
	register thread_act_t	cur_thr_act = current_act();
	register thread_act_t	new_thr_act;
	register thread_t	cur_thr = current_thread();
	register thread_t	thread;
	spl_t			s;
	task_t			task;

	/*
	 *	Break IPC control over the thread.
	 */
	ipc_thr_act_disable(thr_act);

	task = thr_act->task;
	task_lock(task);
	/*
	 *	Lock the target thread and the current thread now,
	 *	in case thread_halt() ends up being called below.
	 */
	thread = act_lock_thread(thr_act);
	if (thread == cur_thr)
		;
	else if ((int)thr_act < (int)cur_thr_act) {
		cur_thr = act_lock_thread(cur_thr_act);
	}
	else {
		act_unlock_thread(thr_act);
		cur_thr = act_lock_thread(cur_thr_act);
		thread = act_lock_thread(thr_act);
	}

	/*
	 * Check for terminating an empty thread_act -- detach it from
	 * its task, then deallocate its remaining refs here.
	 */
	if (thr_act->thread == THREAD_NULL) {
		act_disable_task_locked(thr_act);
		s = splsched();
		thread_ast_set(thr_act, AST_HALT);
		splx(s);
		act_unlock_thread(thr_act);
		if (thread != cur_thr)
			act_unlock_thread(cur_thr_act);
		task_unlock(task);
		if (thr_act->pool_port) {
		    ip_lock(thr_act->pool_port);
		    act_set_thread_pool(thr_act, IP_NULL);
		    ip_unlock(thr_act->pool_port);
		}
		act_deallocate(thr_act);
		return (KERN_SUCCESS);
	}

	if (do_checks) {
		/*
		 * We assume thread_force_terminate() is never called
		 * on the current thread_act.
		 */
		if (thr_act->thread == cur_thr) {
			act_disable_task_locked(thr_act);
			s = splsched();
			thread_ast_set(thr_act, AST_HALT);
			splx(s);
			act_unlock_thread(thr_act);
			task_unlock(task);
			return (KERN_SUCCESS);
		}
		/*
		 *	If the current thr_act is being terminated, help out.
		 */
		if ((!cur_task->active) || (!cur_thr_act->active)) {
			act_unlock_thread(thr_act);
			if (thread != cur_thr)
				act_unlock_thread(cur_thr_act);
			task_unlock(task);
			thread_terminate(cur_thr_act);
			return(KERN_FAILURE);
		}
		/*
		 *	Terminate victim thr_act.
		 */
		if (!thr_act->active) {
			/*
			 *	Someone else got there first.
			 */
			act_unlock_thread(thr_act);
			if (thread != cur_thr)
				act_unlock_thread(cur_thr_act);
			task_unlock(task);
			return(KERN_FAILURE);
		}
	}


#if	MACH_HOST
	/*
	 *	Reassign thread to default pset if needed.
	 */
	thread = thr_act->thread;
	thread_freeze(thr_act->thread);
	if (thr_act->thread->processor_set != &default_pset) {
		thread_doassign(thr_act->thread, &default_pset, FALSE);
		/*
		 * All thread-related locks released as of here.
		 */
		(void)act_lock_thread(thr_act);
		assert(thread == thr_act->thread);
	}
#endif	/* MACH_HOST */

	task_unlock(task);

	/*
	 *	Halt the victim at a clean point.
	 */
	(void) thread_halt(thr_act, THREAD_HALT_MUST_HALT);
	/*
	 *	cur_thr_act now unlocked
	 */
#if	MACH_HOST
	thread_unfreeze(thr_act->thread);
#endif	/* MACH_HOST */

	/*
	 * Avoid a race here -- deactivate thr_act before releasing
	 * act_lock().
	 */
	thr_act->active = FALSE;
	act_unlock_thread(thr_act);
	task_lock(task);
	act_lock_thread(thr_act);
	act_disable_task_locked(thr_act);
	act_unlock_thread(thr_act);
	task_unlock(task);
	return(KERN_SUCCESS);
}

/*
 *	Halt a thread at a clean point, leaving it suspended.
 *
 *	how_to_halt indicates how to halt the thread:
 *		THREAD_HALT_NORMAL - Make deadlock and interrupt checks.
 *		THREAD_HALT_MUST_HALT - No deadlock or interrupt checks,
 *			caller is responsible for deadlock checks.
 *		THREAD_HALT_SAFELY - Only halt in ways that are restartable.
 *			This is for support of task migration.
 *
 *	Called with thr_act locked "appropriately" (see act_lock_thread())
 *	to synchronize with RPC, and with current_act similarly locked.
 *	Returns with locks held only for thr_act.
 */
kern_return_t
thread_halt(
	register thread_act_t	thr_act,
	int			how_to_halt)
{
	register thread_act_t	cur_thr_act = current_act();
	register kern_return_t	ret;
	boolean_t		act_unlocked;
	int			s;
	thread_t		cur_thread;
	thread_t		lthread;
	thread_t		thread;

	if (thr_act->thread == cur_thr_act->thread)
		panic("thread_halt: trying to halt current thread");

	thread = thr_act->thread;
	cur_thread = cur_thr_act->thread;
	/*
	 *	If must_halt is FALSE, then a check must be made for
	 *	a cycle of halt operations.
	 */
	if (how_to_halt != THREAD_HALT_MUST_HALT) {

		assert(thread != THREAD_NULL);
		assert(cur_thread != THREAD_NULL);

		/*
		 *	If target thr_act is already halted, grab a hold
		 *	on it and return.
		 */
		if (thread->state & TH_HALTED) {
			s = splsched();
			thread_lock(thread);
			assert(thr_act->suspend_count);
			thr_act->suspend_count++;
			thread_unlock(thread);
			splx(s);
			if (thread != cur_thread)
				act_unlock_thread(cur_thr_act);
			return(KERN_SUCCESS);
		}

		/*
		 *	If someone is trying to halt us, we have a potential
		 *	halt cycle.  Break the cycle by interrupting anyone
		 *	who is trying to halt us, and causing this operation
		 *	to fail; retry logic will only retry operations
		 *	that cannot deadlock.  (If THREAD_HALT_MUST_HALT, this
		 *	operation can never cause a deadlock.)
		 */
		if (cur_thr_act->ast & AST_HALT) {
			if (thread != cur_thread)
				act_unlock_thread(cur_thr_act);
			thread_wakeup_with_result(
				(int)&cur_thr_act->thread->wake_active,
				THREAD_INTERRUPTED);
			return(KERN_FAILURE);
		}
	}
	else {
		/*
		 *	Lock thread and check whether it is already halted.
		 */
		thread = thr_act->thread;
		assert(thread != THREAD_NULL);

		if (thread->state & TH_HALTED) {
			s = splsched();
			thread_lock(thread);
			assert(thr_act->suspend_count);
			thr_act->suspend_count++;
			thread_unlock(thread);
			splx(s);
			if (thread != cur_thread)
				act_unlock_thread(cur_thr_act);
			return(KERN_SUCCESS);
		}
	}
	if (thread != cur_thread)
		act_unlock_thread(cur_thr_act);

	/*
	 *	Suspend thread - inline version of thread_hold() because
	 *	thr_act is already locked.
	 */
	s = splsched();
	thread_lock(thread);
	thr_act->suspend_count++;
	install_special_handler(thr_act);
	thread->state |= TH_SUSP;

	/*
	 *	If someone else is halting it, wait for that to complete.
	 *	Fail if wait interrupted and !THREAD_HALT_MUST_HALT.
	 */
	act_unlocked = FALSE;
	wake_lock(thread);
	while ((thr_act->ast & AST_HALT) && (!(thread->state & TH_HALTED))) {
		thread->wake_active = TRUE;
		if (act_unlocked == FALSE) {
		    act_unlock_thread(thr_act);
		    act_unlocked = TRUE;
		}
		assert_wait((int)&thread->wake_active, TRUE);
		wake_unlock(thread);
		thread_unlock(thread);
		thread_block((void (*)(void))0);
		if (thread->state & TH_HALTED) {
			(void)splx(s);
			return(KERN_SUCCESS);
		}
		if ((current_thread()->wait_result != THREAD_AWAKENED)
		    && (how_to_halt != THREAD_HALT_MUST_HALT)) {
			(void)splx(s);
			thread_release(thr_act);
			return(KERN_FAILURE);
		}
		thread_lock(thread);
		wake_lock(thread);
	}
	wake_unlock(thread);

	/*
	 *	Otherwise, have to do it ourselves.
	 */
		
	thread_ast_set(thr_act, AST_HALT);

	while (TRUE) {
		thread_unlock(thread);
		splx(s);
		if (act_unlocked == TRUE) {
		    lthread = act_lock_thread(thr_act);
		    assert(lthread != THREAD_NULL);
		    /*
		     * Have to change (maybe just remove) this once we actually
		     * clone shuttles. XXX
		     */
		    if (lthread != thread)
			    panic("thread_halt: awoke with different shuttle");
		    act_unlocked = FALSE;
		}

	  	/*
		 *	Wait for thread to stop.
		 */
		ret = thread_dowait(thr_act,
			(how_to_halt == THREAD_HALT_MUST_HALT));

		s = splsched();
		thread_lock(thread);

		/*
		 *	If the dowait failed, so do we.  Drop AST_HALT, and
		 *	wake up anyone else who might be waiting for it.
		 */
		if (ret != KERN_SUCCESS) {
			boolean_t	need_wakeup;

			wake_lock(thread);
			thread_ast_clear(thr_act, AST_HALT);
			need_wakeup = thread->wake_active;
			thread->wake_active = FALSE;
			wake_unlock(thread);
			thread_unlock(thread);
			splx(s);
			if (need_wakeup)
				thread_wakeup_with_result(
					(int)&thread->wake_active,
					THREAD_INTERRUPTED);
			thread_release(thr_act);
			return(ret);
		}

		/*
		 *	If the thread's at a clean point, we're done.
		 */
		if (thread->state & TH_HALTED) {
			thread_unlock(thread);
			splx(s);
			return(KERN_SUCCESS);
		}

		/*
		 *	If the thread is at certain safe points, we can
		 *	halt it "safely".
		 */
		if (thread->at_safe_point == SAFE_MACH_MSG_RECEIVE &&
				mach_msg_interruptible(thread) ||
			thread->at_safe_point != NOT_AT_SAFE_POINT &&
				thread->at_safe_point != SAFE_MACH_MSG_RECEIVE) {

			/*
			 *	Clear any interruptible wait, then set
			 *	halted state.
			 */
			thread->state |= TH_HALTED;
			thread_unlock(thread);
			clear_wait(thread, THREAD_INTERRUPTED, TRUE);

			thread_ast_clear(thr_act, AST_HALT);
			splx(s);

			return KERN_SUCCESS;
		}

		/*
		 *	In the HALT_SAFELY case, reaching this point is
		 *	a failure.  Ripping the thread out of an interruptible
		 *	wait is not guaranteed to be restartable.
		 */
		if (how_to_halt == THREAD_HALT_SAFELY) {
			boolean_t	need_wakeup;
			/*
			 *	Drop AST_HALT (and wake up anyone else
			 *	who might be waiting for it) and release
			 * 	the thread.
			 */
			wake_lock(thread);
			thread_ast_clear(thr_act, AST_HALT);
			need_wakeup = thread->wake_active;
			thread->wake_active = FALSE;
			wake_unlock(thread);
			thread_unlock(thread);
			splx(s);
			if (need_wakeup)
				thread_wakeup_with_result(
					(int)&thr_act->thread->wake_active,
					THREAD_INTERRUPTED);
			thread_release(thr_act);
			return(KERN_FAILURE);
		}

		thread_unlock(thread);
		splx(s);

		/*
		 *	Clear any interruptible wait.
		 */
		clear_wait(thread, THREAD_INTERRUPTED, TRUE);

		/*
		 *	Force the thread to stop at a clean
		 *	point, and arrange to wait for it.
		 *
		 *	Set it running, so it can notice.  Override
		 *	the suspend count.  We know that the thread
		 *	is suspended and not waiting.
		 *
		 *	Since the thread may hit an interruptible wait
		 *	before it reaches a clean point, we must force it
		 *	to wake us up when it does so.  This involves some
		 *	trickery:
		 *	  We mark the thread SUSPENDED so that thread_block
		 *	will suspend it and wake us up.
		 *	  We mark the thread RUNNING so that it will run.
		 *	  We mark the thread UN-INTERRUPTIBLE (!) so that
		 *	some other thread trying to halt or suspend it won't
		 *	take it off the run queue before it runs.  Since
		 *	dispatching a thread (the tail of thread_invoke) marks
		 *	the thread interruptible, it will stop at the next
		 *	context switch or interruptible wait.
		 */

		s = splsched();
		thread_lock(thread);
		if ((thread->state & TH_SCHED_STATE) != TH_SUSP)
			panic("thread_halt");
		thread->state |= TH_RUN | TH_UNINT;
		thread_setrun(thread, FALSE, TAIL_Q);

		/*
		 *	Continue loop and wait for thread to stop.
		 */
	}
}

/*
 *	thread_hold:
 *
 *	Suspend execution of the specified thread.
 *	This is a recursive-style suspension of the thread, a count of
 *	suspends is maintained.
 *
 *	Called with thr_act locked "appropriately" for synchrony with
 *	RPC (see act_lock_thread()).  Returns same way.
 */
void
thread_hold(
	register thread_act_t	thr_act)
{
	int			s;
	thread_t		thread = thr_act->thread;

	if (thread != THREAD_NULL) {
		s = splsched();
		thread_lock(thread);
	}
	thr_act->suspend_count++;
	if (thread && thr_act == thread->top_act)
		thread->state |= TH_SUSP;
	install_special_handler(thr_act);
	if (thread != THREAD_NULL) {
		thread_unlock(thread);
		splx(s);
	}
}

/*
 *	thread_dowait:
 *
 *	Wait for a thread to actually enter stopped state.
 *
 *	must_halt argument indicates if this may fail on interruption.
 *	This is FALSE only if called from thread_abort via thread_halt.
 *
 *	Called with thr_act locked "appropriately" for synchrony with
 *	RPC (see act_lock_thread()).  Returns same way.
 */
kern_return_t
thread_dowait(
	register thread_act_t	thr_act,
	boolean_t		must_halt)
{
	register boolean_t	need_wakeup;
	register kern_return_t	ret = KERN_SUCCESS;
	boolean_t		unlock_act;
	spl_t			s;
	thread_t		thread = thr_act->thread;
	thread_t		lthread;

	s = splsched();
	thread_lock(thread);
	assert(thread != THREAD_NULL);
	assert(thr_act != current_act());

	/* 
	 *	Just return success if the target activation is not 
	 *	its shuttle's top activation.
	 */
	if (thr_act != thread->top_act) {
		thread_unlock(thread);
		splx(s);
		return(ret);
	}

	/*
	 *	If a thread is not interruptible, it may not be suspended
	 *	until it becomes interruptible.  In this case, we wait for
	 *	the thread to stop itself, and indicate that we are waiting
	 *	for it to stop so that it can wake us up when it does stop.
	 *
	 *	If the thread is interruptible, we may be able to suspend
	 *	it immediately.  There are several cases:
	 *
	 *	1) The thread is already stopped (trivial)
	 *	2) The thread is runnable (marked RUN and on a run queue).
	 *	   We pull it off the run queue and mark it stopped.
	 *	3) The thread is running.  We wait for it to stop.
	 */

	need_wakeup = FALSE;
	unlock_act = TRUE;

	for (;;) {
	    switch (thread->state & TH_SCHED_STATE) {
		case			TH_SUSP:
		    /*
		     * The target thread may have been already stopped,
		     * and not had a chance to see if it should be halting.
		     * Check here on its behalf, and give thread_abort* a
		     * better chance of success.
		     */
		    if (thr_act->ast & AST_HALT) {
			    thread->state |= TH_HALTED;
			    thread_ast_clear(thr_act, AST_HALT);
		    }
		    /* fall through */
		case	      TH_WAIT | TH_SUSP:
		    /*
		     *	Thread is already suspended, or sleeping in an
		     *	interruptible wait.  We win!
		     */
		    break;

		case TH_RUN	      | TH_SUSP:
		    /*
		     *	The thread is interruptible.  If we can pull
		     *	it off a runq, stop it here.
		     */
		    if (rem_runq(thread) != RUN_QUEUE_NULL) {
			thread->state &= ~TH_RUN;
			wake_lock(thread);
			need_wakeup = thread->wake_active;
			thread->wake_active = FALSE;
			wake_unlock(thread);
			/* check for halt request, as commented above */
			if (thr_act->ast & AST_HALT) {
				thread->state |= TH_HALTED;
				thread_ast_clear(thr_act, AST_HALT);
			}
			break;
		    }
#if	NCPUS > 1
		    /*
		     *	The thread must be running, so make its
		     *	processor execute ast_check().  This
		     *	should cause the thread to take an ast and
		     *	context switch to suspend for us.
		     */
		    cause_ast_check(thread->last_processor);
#endif	/* NCPUS > 1 */

		    /*
		     *	Fall through to wait for thread to stop.
		     */

		case TH_RUN	      | TH_SUSP | TH_UNINT:
		case TH_RUN | TH_WAIT | TH_SUSP:
		case TH_RUN | TH_WAIT | TH_SUSP | TH_UNINT:
		case	      TH_WAIT | TH_SUSP | TH_UNINT:
		    /*
		     *	Wait for the thread to stop, or sleep interruptibly
		     *	(thread_block will stop it in the latter case).
		     *	Check for failure if interrupted.
		     */
		    wake_lock(thread);
		    thread->wake_active = TRUE;
		    thread_unlock(thread);
		    if (unlock_act) {
			act_unlock_thread(thr_act);
			unlock_act = FALSE;
		    }
		    assert_wait((int)&thread->wake_active, TRUE);
		    wake_unlock(thread);
		    thread_block((void (*)(void))0);
		    thread_lock(thread);
		    if ((current_thread()->wait_result != THREAD_AWAKENED) &&
			    !must_halt) {
			ret = KERN_FAILURE;
			break;
		    }

		    /*
		     *	Repeat loop to check thread`s state.
		     */
		    continue;
	    }
	    /*
	     *	Thread is stopped at this point.
	     */
	    break;
	}

	thread_unlock(thread);
	splx(s);
	if (unlock_act == FALSE) {
		/*
		 * We blocked above -- have to re-lock thread.
		 */
		lthread = act_lock_thread(thr_act);
		/*
		 * Need to fix this when we really clone shuttles...  XXX
		 */
		if (lthread != thread)
			panic("thread_dowait: awoke with different shuttle");
	}
	if (need_wakeup)
	    thread_wakeup((int) &thread->wake_active);

	return(ret);
}

/*
 * Decrement internal suspension count for thr_act, setting thread
 * runnable when count falls to zero.
 *
 * Called with thr_act locked "appropriately" for synchrony
 * with RPC (see act_lock_thread()).
 */
void
thread_release(
	register thread_act_t	thr_act)
{
	spl_t			s;
	thread_t		thread = thr_act->thread;

	s = splsched();
	thread_lock(thread);
	if (--thr_act->suspend_count == 0) {
		thread->state &= ~(TH_SUSP | TH_HALTED);
		if ((thread->state & (TH_WAIT | TH_RUN)) == 0) {
			/* was only suspended */
			thread->state |= TH_RUN;
			thread_setrun(thread, TRUE, TAIL_Q);
		}
	}
	thread_unlock(thread);
	(void) splx(s);
}

kern_return_t
thread_suspend(
	register thread_act_t	thr_act)
{
	register boolean_t	hold, top;
	spl_t			spl;
	thread_t		thread;

	if (thr_act == THR_ACT_NULL) {
		return(KERN_INVALID_ARGUMENT);
	}
	thread = act_lock_thread(thr_act);
	if (!thr_act->active) {
		act_unlock_thread(thr_act);
		return(KERN_TERMINATED);
	}

	hold = FALSE;
	top = TRUE;
	if (thread) {
		spl = splsched();
		thread_lock(thread);
	}
	if (thr_act->user_stop_count++ == 0) {
		hold = TRUE;
		thr_act->suspend_count++;
		if (thread) {
			if (thr_act != thread->top_act)
				top = FALSE;
			else {
				thread->state |= TH_SUSP;
				/* XXXX remove from thread_pool? XXXX */
			}
		}
		install_special_handler(thr_act);
	}
	if (thread) {
		thread_unlock(thread);
		splx(spl);
	}
	if (thread) {
		/*
		 *	Now wait for the thread if necessary.
		 */
		if (hold && top) {
			if (thread != current_thread())
				(void)thread_dowait(thr_act, TRUE);
		}
	}
	act_unlock_thread(thr_act);
	return(KERN_SUCCESS);
}

kern_return_t
thread_resume(
	register thread_act_t	thr_act)
{
	register kern_return_t	ret;
	spl_t			s;
	thread_t		thread;

	if (thr_act == THR_ACT_NULL)
		return(KERN_INVALID_ARGUMENT);
	thread = act_lock_thread(thr_act);
	ret = KERN_SUCCESS;

	if (thread) {
		s = splsched();
		thread_lock(thread);
	}
	if (thr_act->active) {
		if (thr_act->user_stop_count > 0) {
		    	if (--thr_act->user_stop_count == 0 &&
				--thr_act->suspend_count == 0) {
				if (thread && thr_act == thread->top_act) {
					thread->state &= ~(TH_SUSP | TH_HALTED);
					if ((thread->state &
						(TH_WAIT | TH_RUN)) == 0) {
						/* was only suspended */
						thread->state |= TH_RUN;
						thread_setrun(thread,
							TRUE, TAIL_Q);
					}
					/* XXX return to thread_pool */
				}
			}
		}
		else {
			ret = KERN_FAILURE;
		}
	}
	else {
		ret = KERN_TERMINATED;
	}
	if (thread) {
		thread_unlock(thread);
		splx(s);
	}
	act_unlock_thread(thr_act);
	return(ret);
}

kern_return_t
thread_abort(
	register thread_act_t	thr_act)
{
	thread_t		thread;
	thread_t		cur_thr = current_thread();
	thread_act_t		cur_thr_act = current_act();

	if (thr_act == THR_ACT_NULL || thr_act == current_act())
		return (KERN_INVALID_ARGUMENT);
	/*
	 *	Lock the target thread and the current thread now,
	 *	in case thread_halt() ends up being called below.
	 */
	thread = act_lock_thread(thr_act);
	if (thread == cur_thr)
		;
	else if ((int)thr_act < (int)cur_thr_act) {
		(void)act_lock_thread(cur_thr_act);
	}
	else {
		act_unlock_thread(thr_act);
		(void)act_lock_thread(cur_thr_act);
		thread = act_lock_thread(thr_act);
	}

	if (thread == THREAD_NULL) {
		act_unlock_thread(thr_act);
		if (thread != cur_thr)
			act_unlock_thread(cur_thr_act);
		return (KERN_INVALID_ARGUMENT);
	}


	/*
	 *	Try to force the thread to a clean point.
	 *	If the halt operation fails return KERN_ABORTED --
	 *	ipc code will convert this to an ipc interrupted error code.
	 */
	if (thread_halt(thr_act, THREAD_HALT_NORMAL) != KERN_SUCCESS) {
		/*
		 * cur_thr_act now unlocked
		 */
		act_unlock_thread(thr_act);
		return(KERN_ABORTED);
	}
	/*
	 * cur_thr_act now unlocked
	 */

	/*
	 *	If the thread was in an exception, abort that too.
	 */
	mach_msg_abort_rpc(thread);

	/*
	 *	Also abort any depression.
	 */
	if (thread->depress_priority != -1)
		thread_depress_abort_fast(thread);

	/*
	 *	Then set it going again.
	 */
	thread_release(thr_act);

	act_unlock_thread(thr_act);
	return(KERN_SUCCESS);
}

kern_return_t
thread_abort_safely(
	register thread_act_t	thr_act)
{
	thread_t		cur_thr = current_thread();
	thread_t		thread;
	thread_act_t		cur_thr_act = current_act();

	if (thr_act == THR_ACT_NULL || thr_act == current_act())
		return (KERN_INVALID_ARGUMENT);
	/*
	 *	Lock the target thread and the current thread now,
	 *	in case thread_halt() ends up being called below.
	 */
	thread = act_lock_thread(thr_act);
	if (thread == cur_thr)
		;
	else if ((int)thr_act < (int)cur_thr_act) {
		(void)act_lock_thread(cur_thr_act);
	}
	else {
		act_unlock_thread(thr_act);
		(void)act_lock_thread(cur_thr_act);
		thread = act_lock_thread(thr_act);
	}

	if (thread == THREAD_NULL) {
		act_unlock_thread(thr_act);
		if (thread != cur_thr)
			act_unlock_thread(cur_thr_act);
		return (KERN_INVALID_ARGUMENT);
	}

	/*
	 *	Try to force the thread to a clean point -- we assume that
	 *	if it's engaged in a migrating RPC (and we're not the
	 *	migratee), we can't do so.
	 */
	if (thr_act != thread->top_act) {
		if (thread != cur_thr)
			act_unlock_thread(cur_thr_act);
		act_unlock_thread(thr_act);
		return(KERN_FAILURE);
	}
	if (thread_halt(thr_act, THREAD_HALT_SAFELY) != KERN_SUCCESS) {
		/*
		 * cur_thr_act now unlocked
		 */
		act_unlock_thread(thr_act);
		return(KERN_FAILURE);
	}
	/*
	 * cur_thr_act now unlocked
	 */

	/*
	 *	Then set it going again.
	 */
	thread_release(thr_act);
	act_unlock_thread(thr_act);
	return(KERN_SUCCESS);
}

/*** backward compatibility hacks ***/
#include <mach/thread_info.h>
#include <mach/thread_special_ports.h>
#include <ipc/ipc_port.h>
#include <mach/mach_server.h>

kern_return_t
thread_info(
	thread_act_t		thr_act,
	int			flavor,
	thread_info_t		thread_info_out,
	mach_msg_type_number_t	*thread_info_count)
{
	thread_t		thread;
	kern_return_t	kr;

	if (thr_act == THR_ACT_NULL)
		return (KERN_INVALID_ARGUMENT);
	thread = act_lock_thread(thr_act);
	if (thread == THREAD_NULL) {
		act_unlock_thread(thr_act);
		return (KERN_INVALID_ARGUMENT);
	}
	kr = Thread_info(thread, flavor, thread_info_out,
			    (unsigned int *)thread_info_count);
	act_unlock_thread(thr_act);
	return (kr);
}

/*
 *	Routine:	thread_get_special_port [kernel call]
 *	Purpose:
 *		Clones a send right for one of the thread's
 *		special ports.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		KERN_SUCCESS		Extracted a send right.
 *		KERN_INVALID_ARGUMENT	The thread is null.
 *		KERN_FAILURE		The thread is dead.
 *		KERN_INVALID_ARGUMENT	Invalid special port.
 */

kern_return_t
thread_get_special_port(
	thread_act_t	thr_act,
	int		which,
	ipc_port_t	*portp)
{
	ipc_port_t	*whichp;
	ipc_port_t	port;
	thread_t	thread;

#if	MACH_ASSERT
	if (watchacts & WA_PORT)
	    printf("thread_get_special_port(thr_act=%x, which=%x port@%x=%x\n",
		thr_act, which, portp, (portp ? *portp : 0));
#endif	/* MACH_ASSERT */

	if (!thr_act)
		return KERN_INVALID_ARGUMENT;
 	thread = act_lock_thread(thr_act);
	switch (which) {
		case THREAD_KERNEL_PORT:
			whichp = &thr_act->ith_sself;
			break;

		default:
			act_unlock_thread(thr_act);
			return KERN_INVALID_ARGUMENT;
	}

	if (thr_act->ith_self == IP_NULL) {
		act_unlock_thread(thr_act);
		return KERN_FAILURE;
	}

	port = ipc_port_copy_send(*whichp);
	act_unlock_thread(thr_act);

	*portp = port;
	return KERN_SUCCESS;
}

/*
 *	Routine:	thread_set_special_port [kernel call]
 *	Purpose:
 *		Changes one of the thread's special ports,
 *		setting it to the supplied send right.
 *	Conditions:
 *		Nothing locked.  If successful, consumes
 *		the supplied send right.
 *	Returns:
 *		KERN_SUCCESS		Changed the special port.
 *		KERN_INVALID_ARGUMENT	The thread is null.
 *		KERN_FAILURE		The thread is dead.
 *		KERN_INVALID_ARGUMENT	Invalid special port.
 */

kern_return_t
thread_set_special_port(
	thread_act_t	thr_act,
	int		which,
	ipc_port_t	port)
{
	ipc_port_t	*whichp;
	ipc_port_t	old;
	thread_t	thread;

#if	MACH_ASSERT
	if (watchacts & WA_PORT)
		printf("thread_set_special_port(thr_act=%x,which=%x,port=%x\n",
			thr_act, which, port);
#endif	/* MACH_ASSERT */

	if (thr_act == 0)
		return KERN_INVALID_ARGUMENT;

	thread = act_lock_thread(thr_act);
	switch (which) {
		case THREAD_KERNEL_PORT:
			whichp = &thr_act->ith_self;
			break;

		default:
			act_unlock_thread(thr_act);
			return KERN_INVALID_ARGUMENT;
	}

	if (thr_act->ith_self == IP_NULL) {
		act_unlock_thread(thr_act);
		return KERN_FAILURE;
	}

	old = *whichp;
	*whichp = port;
	act_unlock_thread(thr_act);

	if (IP_VALID(old))
		ipc_port_release_send(old);
	return KERN_SUCCESS;
}

/*
 *	XXX lame, non-blocking ways to get/set state.
 *	Return thread's machine-dependent state.
 *	Blocking variant really should use the "standard" hold + dowait
 *	code, now that it's back in the kernel.  XXXXX
 */
kern_return_t
thread_get_state(
	register thread_act_t	thr_act,
	int			flavor,
	thread_state_t		state,	/* pointer to OUT array */
	unsigned int		*state_count)	/*IN/OUT*/
{
	kern_return_t		ret;
	thread_t		thread;
	int			s;

	if (thr_act == THR_ACT_NULL)
		return (KERN_INVALID_ARGUMENT);
	thread = act_lock_thread(thr_act);
	/*
	 * We have no kernel activations, so Utah's MO fails for signals etc.
	 *
	 * If we're blocked in the kernel, use non-blocking method, else
	 * pass locked thr_act+thread in to "normal" act_[gs]et_state().
	 */
	if (thread == THREAD_NULL		/* no thread here	*/
	    || thread->top_act != thr_act	/* || not top thr_act	*/
	    || (!(thread->state & (TH_RUN|TH_UNINT)))	/* || blocked	*/
	    || (thr_act == current_act())) {	/* || stupid		*/
		ret = act_machine_get_state(thr_act, flavor,
					    state, state_count);
		act_unlock_thread(thr_act);
	} else {
		assert(thr_act);
		assert(thr_act->thread);
		assert(thr_act != current_act());
		ret = act_get_state_locked(thr_act, flavor, state, state_count);
	}
	return(ret);
}

/*
 *	Change thread's machine-dependent state.  Called with nothing
 *	locked.  Returns same way.
 */
kern_return_t
thread_set_state(
	register thread_act_t	thr_act,
	int			flavor,
	thread_state_t		state,
	unsigned int		state_count)
{
	kern_return_t		ret;
	thread_t		thread;
	int			s;

	if (thr_act == THR_ACT_NULL)
		return (KERN_INVALID_ARGUMENT);
	/*
	 * We have no kernel activations, so Utah's MO fails for signals etc.
	 *
	 * If we're blocked in the kernel, use non-blocking method, else
	 * pass locked thr_act+thread in to "normal" act_[gs]et_state().
	 */
	thread = act_lock_thread(thr_act);
	if (thread == THREAD_NULL		/* no thread here	*/
	    || thread->top_act != thr_act	/* || not top thr_act	*/
	    || (!(thread->state & (TH_RUN|TH_UNINT)))	/* || blocked	*/
	    || (thr_act == current_act())) {	/* || stupid		*/
		ret = act_machine_set_state(thr_act, flavor,
					    state, state_count);
		act_unlock_thread(thr_act);
	} else {
		assert(thr_act);
		assert(thr_act->thread);
		assert(thr_act != current_act());
		ret = act_set_state_locked(thr_act, flavor, state, state_count);
	}
	return(ret);
}

/*
 * Kernel-internal "thread" interfaces used outside this file:
 */

/*
 * Initialize thread_states_counts[]
 */
void
thread_states_counts_init(thread_act_t template)
{
	thread_state_t		state;
	thread_state_flavor_t	flavor;
	unsigned int		state_count;

	state = (thread_state_t)kalloc(THREAD_STATE_MAX * sizeof(int));
	state_count = THREAD_STATE_MAX;
	(void) thread_getstatus(template, THREAD_STATE_FLAVOR_LIST,
				state, &state_count);
	assert(state_count <= THREAD_STATE_MAX);

	for (flavor = 0; flavor < state_count; flavor++)
		if (*(state+flavor) > thread_flavor_max)
			thread_flavor_max = *(state+flavor);

	thread_states_counts =
                 (int *)kalloc((thread_flavor_max+1)*sizeof(int));

	for (flavor = 0; flavor <= thread_flavor_max; flavor++) {
	        state_count = THREAD_STATE_MAX;
		if (thread_getstatus(template, flavor, state,
				     &state_count) == KERN_SUCCESS) {
			assert(state_count <= THREAD_STATE_MAX);
			*(thread_states_counts+flavor) = state_count;
		}
	}
	kfree((vm_offset_t)state, THREAD_STATE_MAX * sizeof(int));
}

/*
 *	thread_setstatus:
 *
 *	Set the status of the specified thread.
 *	Called with (and returns with) no locks held.
 */
kern_return_t
thread_setstatus(
	thread_act_t		thr_act,
	int			flavor,
	thread_state_t		tstate,
	unsigned int		count)
{
	kern_return_t		kr;
	thread_t		thread;

	thread = act_lock_thread(thr_act);
	assert(thread);
	assert(thread->top_act == thr_act);
	kr = act_machine_set_state(thr_act, flavor, tstate, count);
	act_unlock_thread(thr_act);
	return(kr);
}

/*
 *	thread_getstatus:
 *
 *	Get the status of the specified thread.
 */
kern_return_t
thread_getstatus(
	thread_act_t		thr_act,
	int			flavor,
	thread_state_t		tstate,
	unsigned int		*count)
{
	kern_return_t		kr;
	thread_t		thread;

	thread = act_lock_thread(thr_act);
	assert(thread);
	assert(thread->top_act == thr_act);
	kr = act_machine_get_state(thr_act, flavor, tstate, count);
	act_unlock_thread(thr_act);
	return(kr);
}

/*
 * Kernel-internal thread_activation interfaces used outside this file:
 */

/*
 * act_init()	- Initialize activation handling code
 */
void
act_init()
{
	thr_act_zone = zinit(
			sizeof(struct thread_activation),
			ACT_MAX * sizeof(struct thread_activation), /* XXX */
			ACT_CHUNK * sizeof(struct thread_activation),
			"activations");
	act_machine_init();
}


/*
 * act_create	- Create a new activation in a specific task.
 */
kern_return_t
act_create(task_t task, vm_offset_t user_stack, vm_size_t stack_size,
	   thread_act_t *new_act)
{
	thread_act_t thr_act;
	int rc;

	thr_act = (thread_act_t)zalloc(thr_act_zone);
	if (thr_act == 0)
		return(KERN_RESOURCE_SHORTAGE);

#if	MACH_ASSERT
	if (watchacts & WA_ACT_LNK)
		printf("act_create(task=%x,stk=%x,thr_act@%x=%x)\n",
			task, user_stack, new_act, thr_act);
#endif	/* MACH_ASSERT */

	/* Start by zeroing everything; then init non-zero items only */
	bzero((const char *)thr_act, sizeof(*thr_act));

	/* Start with one reference for being active, another for the caller */
	act_lock_init(thr_act);
	thr_act->ref_count = 2;

	/* Latch onto the task.  */
	thr_act->task = task;
	task_reference(task);
	thr_act->active = 1;
	thr_act->user_stack = user_stack;
	thr_act->user_stack_size = stack_size;

	/* special_handler will always be last on the returnhandlers list.  */
	thr_act->special_handler.next = 0;
	thr_act->special_handler.handler = special_handler;

#if	MACH_PROF
	thr_act->act_profiled = FALSE;
	thr_act->act_profiled_own = FALSE;
	thr_act->profil_buffer = NULLPROFDATA;
#endif

	/* Initialize the held_ulocks queue as empty */
	queue_init(&thr_act->held_ulocks);

	/* Inherit the profiling status of the parent task */
	act_prof_init(thr_act, task);

	ipc_thr_act_init(task, thr_act);
	act_machine_create(task, thr_act);

	/*
	 * If thr_act created in kernel-loaded task, alter its saved
	 * state to so indicate
	 */
	if (task->kernel_loaded) {
		act_user_to_kernel(thr_act);
		/*
		 * If a user_stack was given, assume this is not a "base"
		 * activation -- assert that it's fully kernel-loaded
		 * already.
		 */
		if (user_stack) {
			thr_act->kernel_loading = FALSE;
			thr_act->kernel_loaded = TRUE;
		}
	}

	task_lock(task);

	/* Chain the thr_act onto the task's list */
	thr_act->thr_acts.next = task->thr_acts.next;
	thr_act->thr_acts.prev = &task->thr_acts;
	task->thr_acts.next->prev = &thr_act->thr_acts;
	task->thr_acts.next = &thr_act->thr_acts;
	task->thr_act_count++;

	task_unlock(task);

	/* Cache the task's map and take a reference to it */
	thr_act->map = task->map;
	vm_map_reference(task->map);

	*new_act = thr_act;
	return KERN_SUCCESS;
}

/*
 * act_free	- called when an thr_act's ref_count drops to zero.
 *
 * This can only happen when thread is zero (not in use),
 * thread_pool is zero (not attached to any thread_pool),
 * and active is false (terminated).
 */
#if	MACH_ASSERT
int	dangerous_bzero = 1;	/* paranoia & safety */
#endif

void
act_free(thread_act_t thr_act)
{
#if	MACH_ASSERT
	if (watchacts & WA_EXIT)
		printf("act_free(%x(%d)) thr=%x tsk=%x(%d) pport=%x%sactive\n",
			thr_act, thr_act->ref_count, thr_act->thread,
			thr_act->task,
			thr_act->task ? thr_act->task->ref_count : 0,
			thr_act->pool_port,
			thr_act->active ? " " : " !");
#endif	/* MACH_ASSERT */

	act_machine_destroy(thr_act);
	ipc_thr_act_terminate(thr_act);

	act_prof_deallocate(thr_act);

	/* Drop the cached map reference */
	vm_map_deallocate(thr_act->map);

	/* Drop the task reference.  */
	task_deallocate(thr_act->task);

#if	MACH_ASSERT
	if (dangerous_bzero)	/* dangerous if we're still using it! */
		bzero((const char *)thr_act, sizeof(*thr_act));
#endif	/* MACH_ASSERT */
	/* Put the thr_act back on the thr_act zone */
	zfree(thr_act_zone, (vm_offset_t)thr_act);
}

/*
 * act_attach	- Attach an thr_act to the top of a thread ("push the stack").
 *
 * The thread_shuttle must be either the current one or a brand-new one.
 * Assumes the thr_act is active but not in use, also, that if it is
 * attached to an thread_pool (i.e. the thread_pool pointer is nonzero),
 * the thr_act has already been taken off the thread_pool's list.
 *
 * Already locked: thr_act plus "appropriate" thread-related locks
 * (see act_lock_thread()).
 */
void act_attach(
	thread_act_t	thr_act,
	thread_t	thread,
	unsigned	init_alert_mask)
{
	int		s;
	thread_act_t	lower;

#if	MACH_ASSERT
	assert(thread == current_thread() || thread->top_act == THR_ACT_NULL);
	if (watchacts & WA_ACT_LNK)
		printf("act_attach(thr_act %x(%d) thread %x(%d) mask %d)\n",
		       thr_act, thr_act->ref_count, thread, thread->ref_count,
		       init_alert_mask);
#endif	/* MACH_ASSERT */

	/* Chain the thr_act onto the thread's thr_act stack.  */
	/* Set mask and auto-propagate alerts from below */
	thr_act->thread = thread;
	thr_act->higher = THR_ACT_NULL;  /*safety*/
	thr_act->alerts = 0;
	thr_act->alert_mask = init_alert_mask;
	lower = thr_act->lower = thread->top_act;
	/*
	 * Distinguish here between initial (base) activation and
	 * later ones.  For later ones, we manipulate kernel-cached
	 * state relative to the current kernel stack, and update
	 * the kernel stack stored in the thread.
	 */
	if (lower != THR_ACT_NULL) {
		vm_offset_t stack;

		lower->higher = thr_act;
		thr_act->alerts = (lower->alerts & init_alert_mask);
		stack = thr_act->kernel_stack;
		assert(stack != 0);
		/*
		 * We're changing the current activation, so need to
		 * update the kernel's cached values for it.  Ideally,
		 * the change and update would be atomic, but we can't
		 * arrange that.  WARNING: this code really can't be
		 * single-stepped through (XXX).
		 */
		assert(active_threads[cpu_number()] == thread);
		active_kloaded[cpu_number()] = (thr_act->kernel_loaded)
								? thr_act : 0;
		MACHINE_STACK_STASH(stack);
		ast_context(thr_act, cpu_number());
#if 1
		PMAP_DEACTIVATE_USER(lower, cpu_number());
		PMAP_ACTIVATE_USER(thr_act, cpu_number());
		act_machine_switch_pcb(thr_act);
#endif
	}
	/*
	 * Probably need check only user_stop_count -- no one's going to
	 * thread_hold() an empty thread_act.
	 */
	if (thr_act->user_stop_count || thr_act->suspend_count) {
		s = splsched();
		thread_lock(thread);
		thread->state |= TH_SUSP;
		thread_unlock(thread);
		splx(s);
	}
	thread->top_act = thr_act;
}

/*
 * act_detach	- remove the current thr_act from the top of the current thread.
 *		  (ie, "pop the stack").
 * Return it to the thread_pool it lives on, if any.
 *
 * Already locked: thr_act plus "appropriate" thread-related locks
 * (see act_lock_thread()).
 */
void act_detach(
	thread_act_t	cur_act,
	boolean_t	is_current)
{
	int		s;
	thread_t	cur_thread = cur_act->thread;
	thread_pool_t	cur_pool;

	if (cur_act->pool_port)
	    cur_pool = &cur_act->pool_port->ip_thread_pool;
	else
	    cur_pool = THREAD_POOL_NULL;
#if	MACH_ASSERT
	if (watchacts & (WA_EXIT|WA_ACT_LNK))
		printf("act_detach: thr_act %x(%d), thrd %x(%d) task=%x(%d)\n",
		       cur_act, cur_act->ref_count,
		       cur_thread, cur_thread->ref_count,
		       cur_act->task,
		       cur_act->task ? cur_act->task->ref_count : 0);
#endif	/* MACH_ASSERT */

	/* Unlink the thr_act from the thread's thr_act stack */
	cur_thread->top_act = cur_act->lower;
	cur_act->thread = 0;
#if	MACH_ASSERT
	cur_act->lower = cur_act->higher = THR_ACT_NULL; /* mostly for DEBUG */
#endif	/* MACH_ASSERT */

	if (cur_thread->top_act) {
		thread_act_t old_act = cur_thread->top_act;

#if	MACH_ASSERT
		old_act->higher = THR_ACT_NULL;	/* DEBUG hygiene */
#endif	/* MACH_ASSERT */
		if (is_current) {
			vm_offset_t old_stack = old_act->kernel_stack;

			assert(old_stack != 0);
			/*
			 * We're changing the current activation, so need to
			 * update the kernel's cached value for it.  Ideally,
			 * the change and update would be atomic, but we can't
			 * arrange that.  WARNING: this code really can't be
			 * single-stepped through (XXX).
			 */
			assert(active_threads[cpu_number()] == cur_thread);
			active_kloaded[cpu_number()] =
				(old_act->kernel_loaded) ? old_act : 0;
			MACHINE_STACK_STASH(old_stack);
			ast_context(old_act, cpu_number());
			PMAP_DEACTIVATE_USER(cur_act, cpu_number());
			PMAP_ACTIVATE_USER(old_act, cpu_number());
			act_machine_switch_pcb(old_act);
		}
		if (old_act->user_stop_count || old_act->suspend_count) {
			s = splsched();
			thread_lock(cur_thread);
			cur_thread->state |= TH_SUSP;
			thread_unlock(cur_thread);
			splx(s);
		}
	}

	/* Return it to the thread_pool's list, if it is still alive: */
	if (cur_act->active) {
		assert(cur_pool);
		cur_act->thread_pool_next = cur_pool->thr_acts;
		cur_pool->thr_acts = cur_act;
		if (cur_pool->waiting)
			thread_pool_wakeup(cur_pool);
	} else if (cur_pool) {
		/* Remove it from its thread_pool, which will
		 * deallocate it and destroy it.  Lock the pool_port
		 * only if we're not already using it (in RPC, in
		 * which case it will be locked).
		 */
		assert(cur_act->pool_port);
		if (!is_current)
			ip_lock(cur_act->pool_port);
		act_set_thread_pool(cur_act, IP_NULL);
		if (!is_current)
			ip_unlock(cur_act->pool_port);
	}
}

/*
 * Synchronize a thread operation with RPC.  Called with nothing
 * locked.   Returns with thr_act locked, plus one of three
 * combinations of other locks held:
 *	rpc_lock(thr_act->thread) only - for base activation (one
 *		without pool_port)
 *	ip_lock(thr_act->pool_port) only - for empty activation (one
 *		with no associated shuttle)
 *	both locks - for "active" activation (has shuttle, lives
 *		on thread_pool)
 * If thr_act has an associated shuttle, this function returns
 * its address.  Otherwise it returns zero.
 */
thread_t
act_lock_thread(
	thread_act_t thr_act)
{
	/*
	 * Allow the shuttle cloning code (q.v., when it
	 * exists :-}) to obtain ip_lock()'s while holding
	 * an rpc_lock().
	 */
	act_lock(thr_act);
	if (!thr_act->pool_port) {
		assert(thr_act->thread);
		while (1) {
			if (rpc_lock_try(thr_act->thread))
				break;
			act_unlock(thr_act);
#if (NCPUS > 1) || MACH_RT
			mutex_pause();
#else
			panic("rpc_lock_try-1 failed");
#endif
			act_lock(thr_act);
		}
	}
	else {
		while (1) {
			ip_lock(thr_act->pool_port);
			if (!thr_act->thread)
				break;
			if (rpc_lock_try(thr_act->thread))
				break;
			ip_unlock(thr_act->pool_port);
			act_unlock(thr_act);
#if (NCPUS > 1) || MACH_RT
			mutex_pause();
#else
			panic("rpc_lock_try-2 failed");
#endif
			act_lock(thr_act);
		}
	}
	return (thr_act->thread);
}

/*
 * Synchronize with RPC given a pointer to a shuttle (instead of an
 * activation).  Called with nothing locked; returns with all
 * "appropriate" thread-related locks held (see act_lock_thread()).
 */
thread_act_t
thread_lock_act(
	thread_t	thread)
{
	thread_act_t	thr_act;

	while (1) {
		rpc_lock(thread);
		thr_act = thread->top_act;
		if (!act_lock_try(thr_act)) {
			rpc_unlock(thread);
			delay(100);
			continue;
		}
		if (thr_act->pool_port &&
			!ip_lock_try(thr_act->pool_port)) {
			rpc_unlock(thread);
			act_unlock(thr_act);
			delay(100);
			continue;
		}
		break;
	}
	return (thr_act);
}

/*
 * Unsynchronize with RPC (i.e., undo an act_lock_thread() call).
 * Called with thr_act locked, plus thread locks held that are
 * "correct" for thr_act's state.  Returns with nothing locked.
 */
void
act_unlock_thread(thread_act_t	thr_act)
{
	if (!thr_act->pool_port) {
		assert(thr_act->thread);
		rpc_unlock(thr_act->thread);
	}
	else {
		if (thr_act->thread)
			rpc_unlock(thr_act->thread);
		ip_unlock(thr_act->pool_port);
	}
	act_unlock(thr_act);
}

/*
 * install_special_handler
 *	Install the special returnhandler that handles suspension and
 *	termination, if it hasn't been installed already.
 *
 * Already locked: RPC-related locks for thr_act
 */
void
install_special_handler(
	thread_act_t	thr_act)
{
	ReturnHandler	**rh;

#if	MACH_ASSERT
	if (watchacts & WA_ACT_HDLR)
	    printf("act_%x: install_special_hdlr(%x)\n", current_act(), thr_act);
#endif	/* MACH_ASSERT */

	/* The work handler must always be the last ReturnHandler on the list,
	   because it can do tricky things like detach the thr_act.  */
	for (rh = &thr_act->handlers; *rh; rh = &(*rh)->next)
		/* */ ;
	if (rh != &thr_act->special_handler.next) {
		*rh = &thr_act->special_handler;
	}
	act_set_apc(thr_act);
}

/*
 * act_set_thread_pool	- Assign an activation to a specific thread_pool.
 * Fails if the activation is already assigned to another pool.
 * If thread_pool == 0, we remove the thr_act from its thread_pool.
 *
 * Called the port containing thread_pool already locked.
 * Returns the same way.
 */
kern_return_t act_set_thread_pool(
	thread_act_t	thr_act,
	ipc_port_t	pool_port)
{
	thread_pool_t	thread_pool;

#if	MACH_ASSERT
	if (watchacts & WA_ACT_LNK)
		printf("act_set_thread_pool: %x(%d) -> %x\n",
			thr_act, thr_act->ref_count, thread_pool);
#endif	/* MACH_ASSERT */

	if (pool_port == 0) {
		thread_act_t *lact;

		if (thr_act->pool_port == 0)
			return KERN_SUCCESS;
		thread_pool = &thr_act->pool_port->ip_thread_pool;

		for (lact = &thread_pool->thr_acts; *lact;
					lact = &((*lact)->thread_pool_next)) {
			if (thr_act == *lact) {
				*lact = thr_act->thread_pool_next;
				break;
			}
		}

		thr_act->pool_port = 0;
		act_deallocate(thr_act);
		return KERN_SUCCESS;
	}
	if (thr_act->pool_port != pool_port) {
		thread_pool = &pool_port->ip_thread_pool;
		if (thr_act->pool_port != 0) {
#if	MACH_ASSERT
			if (watchacts & WA_ACT_LNK)
			    printf("act_set_thread_pool found %x!\n",
							thr_act->pool_port);
#endif	/* MACH_ASSERT */
			return(KERN_FAILURE);
		}
		thr_act->pool_port = pool_port;

		/* The pool gets a ref to the activation -- have
		 * to inline operation because thr_act is already
		 * locked.
		 */
		act_reference(thr_act);

		/* If it is available,
		 * add it to the thread_pool's available-activation list.
		 */
		if ((thr_act->thread == 0) && (thr_act->suspend_count == 0)) {
			thr_act->thread_pool_next = thread_pool->thr_acts;
			pool_port->ip_thread_pool.thr_acts = thr_act;
			if (thread_pool->waiting)
				thread_pool_wakeup(thread_pool);
		}
	}

	return KERN_SUCCESS;
}

/*
 * Activation control support routines internal to this file:
 */

/*
 * act_execute_returnhandlers()	- does just what the name says
 *
 * This is called by system-dependent code when it detects that
 * thr_act->handlers is non-null while returning into user mode.
 * Activations linked onto an thread_pool always have null thr_act->handlers,
 * so RPC entry paths need not check it.
 */
void act_execute_returnhandlers(
	void)
{
	int		s;
	thread_t	thread;
	thread_act_t	thr_act = current_act();

#if	MACH_ASSERT
	if (watchacts & WA_ACT_HDLR)
		printf("execute_rtn_hdlrs: thr_act=%x\n", thr_act);
#endif	/* MACH_ASSERT */

	act_clr_apc(thr_act);
	while (1) {
		ReturnHandler *rh;

		/* Grab the next returnhandler */
		thread = act_lock_thread(thr_act);
		s = splsched();
		thread_lock(thread);
		rh = thr_act->handlers;
		if (!rh) {
			thread_unlock(thread);
			splx(s);
			act_unlock_thread(thr_act);
			return;
		}
		thr_act->handlers = rh->next;
		thread_unlock(thread);
		splx(s);
		act_unlock_thread(thr_act);

#if	MACH_ASSERT
		if (watchacts & WA_ACT_HDLR)
		    printf( (rh == &thr_act->special_handler) ?
			"\tspecial_handler\n" : "\thandler=%x\n",
				    rh->handler);
#endif	/* MACH_ASSERT */

		/* Execute it */
		(*rh->handler)(rh, thr_act);
	}
}

/*
 * special_handler	- handles suspension, termination.  Called
 * with nothing locked.  Returns (if it returns) the same way.
 */
void
special_handler(
	ReturnHandler	*rh,
	thread_act_t	cur_act)
{
	int		s;
	thread_t	lthread;
	thread_t	thread = act_lock_thread(cur_act);

	assert(thread != THREAD_NULL);
#if	MACH_ASSERT
	if (watchacts & WA_ACT_HDLR)
	    printf("\t\tspecial_handler(thr_act=%x(%d))\n", cur_act,
				(cur_act ? cur_act->ref_count : 0));
#endif	/* MACH_ASSERT */

	s = splsched();
	act_clr_apc(cur_act);
	/*
	 * If someone has killed this invocation,
	 * invoke the return path with a terminated exception.
	 */
	if (!cur_act->active) {
		(void)splx(s);
		act_unlock_thread(cur_act);
		act_machine_return(KERN_TERMINATED);
	}

	/*
	 * If we're suspended, go to sleep and wait for someone to wake us up.
	 */
	if (cur_act->suspend_count) {
		/*
		 * Handle case where we've been asked to halt (via thread_abort()),
		 * which "looks like" a suspension request.
		 */
		thread_lock(thread);
		if (cur_act->ast & AST_HALT) {
			thread->state |= TH_HALTED;
			thread_ast_clear(cur_act, AST_HALT);
		}
		thread->state |= TH_SUSP;	/* undo what nudge() may have done */
		thread_unlock(thread);
		(void)splx(s);
		act_unlock_thread(cur_act);
		thread_block((void (*)(void))SAFE_SUSPENDED); /* block self */

		/*
		 * If we're still (or again) suspended, go to sleep again
		 * after executing any new handlers that may have appeared.
		 */
		lthread = act_lock_thread(cur_act);
		/*
		 * Need to fix (or just remove) this when we really
		 * clone shuttles. XXX
		 */
		if (lthread != thread)
			panic("special_handler: awoke with different shuttle");
		if (cur_act->suspend_count) {
			if (thread) {
			    s = splsched();
			    thread_lock(thread);
			}
			install_special_handler(cur_act);
			if (thread) {
			    thread_unlock(thread);
			    splx(s);
			}
		}
	}
	else
		(void)splx(s);
	act_unlock_thread(cur_act);
}

/*
 * Try to nudge a thr_act into executing its returnhandler chain.
 * Ensures that the activation will execute its returnhandlers
 * before it next executes any of its user-level code.
 *
 * Called with thr_act's act_lock() and "appropriate" thread-related
 * locks held.  (See act_lock_thread().)  Returns same way.
 */
void
nudge(thread_act_t	thr_act)
{
	int		s;

#if	MACH_ASSERT
	if (watchacts & WA_ACT_HDLR)
	    printf("\tact_%x: nudge(%x)\n", current_act(), thr_act);
#endif	/* MACH_ASSERT */

	/*
	 * Don't need to do anything at all if this thr_act isn't the topmost.
	 * Note that we muck here only with TH_SUSP -- we leave suspend_count
	 * with its current (presumably non-zero) value, so that
	 * special_handler() will treat the thr_act's thread as suspended
	 * when the function gets re-executed.
	 */
	if (thr_act->thread && thr_act->thread->top_act == thr_act) {
		/*
		 * If it's suspended, wake it up. 
		 * This should nudge it even on another CPU.
		 */
		s = splsched();
		thread_lock(thr_act->thread);
		if ((thr_act->thread->state & (TH_WAIT|TH_RUN|TH_SUSP)) == TH_SUSP) {
			/* was only suspended */
			thr_act->thread->state &= ~(TH_SUSP|TH_HALTED);
			thr_act->thread->state |= TH_RUN;
			thread_setrun(thr_act->thread, TRUE, TAIL_Q);
		}
		thread_unlock(thr_act->thread);
		splx(s);
	}
}

/*
 * Update activation that belongs to a task created via kernel_task_create().
 */
void
act_user_to_kernel(
	thread_act_t	thr_act)
{
	pcb_user_to_kernel(thr_act->mact.pcb);
	thr_act->kernel_loading = TRUE;
}

/*
 * Already locked: thr_act->task, RPC-related locks for thr_act
 *
 * Detach an activation from its task, and prepare it to terminate
 * itself.
 */
kern_return_t
act_disable_task_locked(
	thread_act_t	thr_act)
{
	int		s;
	thread_t	thread = thr_act->thread;

#if	MACH_ASSERT
	if (watchacts & WA_EXIT) {
		printf("act_%x: act_disable_tl(thr_act=%x(%d))%sactive task=%x(%d)",
			       current_act(), thr_act, thr_act->ref_count,
			       (thr_act->active ? " " : " !"),
			       thr_act->task, thr_act->task? thr_act->task->ref_count : 0);
		if (thr_act->pool_port)
			printf(", pool_port %x", thr_act->pool_port);
		printf("\n");
		(void) dump_act(thr_act);
	}
#endif	/* MACH_ASSERT */
	if (thr_act->thr_acts.next) {
		/* Unlink the thr_act from the task's thr_act list,
		 * so it doesn't appear in calls to task_threads and such.
		 * The thr_act still keeps its ref on the task, however.
		 */
		thr_act->thr_acts.next->prev = thr_act->thr_acts.prev;
		thr_act->thr_acts.prev->next = thr_act->thr_acts.next;
		thr_act->thr_acts.next = 0;
		thr_act->task->thr_act_count--;

		/* This will allow no more control operations on this thr_act */
		thr_act->active = 0;

		/* Clean-up any ulocks that are still owned by the thread
		 * activation (acquired but not released or handed-off).
		 */
		act_ulock_release_all(thr_act);

		s = splsched();
		thread_lock(thread);

		/* When the special_handler gets executed,
		 * it will see the terminated condition and exit immediately.
		 */
		install_special_handler(thr_act);
		thread_unlock(thread);
		splx(s);

		/* If the target happens to be suspended,
		 * give it a nudge so it can exit.
		 */
		if (thr_act->suspend_count) {
			nudge(thr_act);
		}
		/* Drop the thr_act reference taken for being active.
		 * (There is still at least one reference left:
		 * the one we were passed.)
		 * Inline the deallocate because thr_act is locked.
		 */
		--thr_act->ref_count;
	}

	return(KERN_SUCCESS);
}

/*
 * act_alert	- Register an alert from this activation.
 *
 * Each set bit is propagated upward from (but not including) this activation,
 * until the top of the chain is reached or the bit is masked.
 */
kern_return_t
act_alert(thread_act_t thr_act, unsigned alerts)
{
	thread_t thread = act_lock_thread(thr_act);

#if	MACH_ASSERT
	if (watchacts & WA_ACT_LNK)
		printf("act_alert %x: %x\n", thr_act, alerts);
#endif	/* MACH_ASSERT */

	if (thread) {
		thread_act_t act_up = thr_act;
		while ((alerts) && (act_up != thread->top_act)) {
			act_up = act_up->higher;
			alerts &= act_up->alert_mask;
			act_up->alerts |= alerts;
		}
		/*
		 * XXXX If we reach the top, and it is blocked in glue
		 * code, do something to kick it.  XXXX
		 */
	}
	act_unlock_thread(thr_act);

	return KERN_SUCCESS;
}

kern_return_t act_alert_mask(thread_act_t thr_act, unsigned alert_mask)
{
	panic("act_alert_mask NOT YET IMPLEMENTED\n");
	return KERN_SUCCESS;
}

typedef struct GetSetState {
	struct ReturnHandler rh;
	int flavor;
	void *state;
	int *pcount;
	int result;
} GetSetState;

/* Local Forward decls */
kern_return_t get_set_state(
			thread_act_t thr_act, int flavor,
			thread_state_t state, int *pcount,
			void (*handler)(ReturnHandler *rh, thread_act_t thr_act));
void get_state_handler(ReturnHandler *rh, thread_act_t thr_act);
void set_state_handler(ReturnHandler *rh, thread_act_t thr_act);

/*
 * get_set_state(thr_act ...)
 *
 * General code to install g/set_state handler.
 * Called with thr_act's act_lock() and "appropriate"
 * thread-related locks held.  (See act_lock_thread().)
 */
kern_return_t
get_set_state(thread_act_t thr_act, int flavor, thread_state_t state, int *pcount,
			void (*handler)(ReturnHandler *rh, thread_act_t thr_act))
{
	GetSetState gss;

	/* Initialize a small parameter structure */
	gss.rh.handler = handler;
	gss.flavor = flavor;
	gss.state = state;
	gss.pcount = pcount;
	gss.result = KERN_ABORTED;	/* iff wait below is interrupted */

	/* Add it to the thr_act's return handler list */
	gss.rh.next = thr_act->handlers;
	thr_act->handlers = &gss.rh;
	act_set_apc(thr_act);

#if	MACH_ASSERT
	if (watchacts & WA_ACT_HDLR) {
	    printf("act_%x: get_set_state(thr_act=%x flv=%x state=%x ptr@%x=%x)",
		    current_act(), thr_act, flavor, state,
		    pcount, (pcount ? *pcount : 0));
	    printf((handler == get_state_handler ? "get_state_hdlr\n" :
		    (handler == set_state_handler ? "set_state_hdlr\n" :
			"hndler=%x\n")), handler); 
	}
#endif	/* MACH_ASSERT */

	assert(thr_act->thread);	/* Callers must ensure these */
	assert(thr_act != current_act());
	nudge(thr_act);
	/*
	 * Wait must be interruptible to avoid deadlock (e.g.) with
	 * task_suspend() when caller and target of get_set_state()
	 * are in same task.
	 */
	assert_wait((int)&gss, TRUE);	
	act_unlock_thread(thr_act);
	thread_block((void(*)(void))0);	/* block self */

#if	MACH_ASSERT
	if (watchacts & WA_ACT_HDLR)
	    printf("act_%x: get_set_state returns %x\n",
			    current_act(), gss.result);
#endif	/* MACH_ASSERT */

	return gss.result;
}

void
set_state_handler(ReturnHandler *rh, thread_act_t thr_act)
{
	GetSetState *gss = (GetSetState*)rh;

#if	MACH_ASSERT
	if (watchacts & WA_ACT_HDLR)
		printf("act_%x: set_state_handler(rh=%x,thr_act=%x)\n",
			current_act(), rh, thr_act);
#endif	/* MACH_ASSERT */

	gss->result = act_machine_set_state(thr_act, gss->flavor,
						gss->state, *gss->pcount);
	thread_wakeup((int)gss);
}

void
get_state_handler(ReturnHandler *rh, thread_act_t thr_act)
{
	GetSetState *gss = (GetSetState*)rh;

#if	MACH_ASSERT
	if (watchacts & WA_ACT_HDLR)
		printf("act_%x: get_state_handler(rh=%x,thr_act=%x)\n",
			current_act(), rh, thr_act);
#endif	/* MACH_ASSERT */

	gss->result = act_machine_get_state(thr_act, gss->flavor,
					gss->state, (unsigned *)gss->pcount);
	thread_wakeup((int)gss);
}

kern_return_t
act_get_state_locked(thread_act_t thr_act, int flavor, thread_state_t state,
					mach_msg_type_number_t *pcount)
{
#if	MACH_ASSERT
    if (watchacts & WA_ACT_HDLR)
	printf("act_%x: act_get_state_L(thr_act=%x,flav=%x,st=%x,pcnt@%x=%x)\n",
		current_act(), thr_act, flavor, state, pcount,
		(pcount? *pcount : 0));
#endif	/* MACH_ASSERT */

    return(get_set_state(thr_act, flavor, state, (int*)pcount, get_state_handler));
}

kern_return_t
act_set_state_locked(thread_act_t thr_act, int flavor, thread_state_t state,
					mach_msg_type_number_t count)
{
#if	MACH_ASSERT
    if (watchacts & WA_ACT_HDLR)
	printf("act_%x: act_set_state_L(thr_act=%x,flav=%x,st=%x,pcnt@%x=%x)\n",
		current_act(), thr_act, flavor, state, count, count);
#endif	/* MACH_ASSERT */

    return(get_set_state(thr_act, flavor, state, (int*)&count, set_state_handler));
}

kern_return_t
act_set_state(thread_act_t thr_act, int flavor, thread_state_t state,
					mach_msg_type_number_t count)
{
    if (thr_act == THR_ACT_NULL || thr_act == current_act())
	    return(KERN_INVALID_ARGUMENT);

    if (act_lock_thread(thr_act) == THREAD_NULL) {
	    act_unlock(thr_act);
	    return(KERN_INVALID_ARGUMENT);
    }
    return(act_set_state_locked(thr_act, flavor, state, count));
    
}

kern_return_t
act_get_state(thread_act_t thr_act, int flavor, thread_state_t state,
					mach_msg_type_number_t *pcount)
{
    if (thr_act == THR_ACT_NULL || thr_act == current_act())
	    return(KERN_INVALID_ARGUMENT);

    if (act_lock_thread(thr_act) == THREAD_NULL) {
	    act_unlock(thr_act);
	    return(KERN_INVALID_ARGUMENT);
    }
    return(act_get_state_locked(thr_act, flavor, state, pcount));
}

/*
 * These two should be called at splsched()
 * Set/clear indicator to run APC (layered on ASTs)
 */
void
act_set_apc(thread_act_t thr_act)
{
	thread_ast_set(thr_act, AST_APC);
	if (thr_act == current_act())
		ast_propagate(thr_act, cpu_number());
}

void
act_clr_apc(thread_act_t thr_act)
{
	thread_ast_clear(thr_act, AST_APC);
}

void
act_ulock_release_all(thread_act_t thr_act)
{
	ulock_t	ulock;

	while (!queue_empty(&thr_act->held_ulocks)) {
		ulock = (ulock_t) queue_first(&thr_act->held_ulocks);
		(void) lock_make_unstable(ulock, thr_act);
		(void) lock_release_internal(ulock, thr_act);
	}
}
