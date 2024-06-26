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
 * Revision 2.7.3.1  92/03/03  16:20:25  jeffreyh
 * 	Changes from TRUNK
 * 	[92/02/26  11:55:18  jeffreyh]
 * 
 * Revision 2.8  92/01/03  20:14:49  dbg
 * 	Removed THREAD_SHOULD_TERMINATE (again).
 * 	[91/12/19            dbg]
 * 
 * Revision 2.7  91/05/18  14:33:14  rpd
 * 	Added recompute_priorities.
 * 	[91/03/31            rpd]
 * 
 * Revision 2.6  91/05/14  16:46:40  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/03/16  14:51:26  rpd
 * 	Added declarations of new functions.
 * 	[91/02/24            rpd]
 * 
 * Revision 2.4  91/02/05  17:29:09  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:17:24  mrt]
 * 
 * Revision 2.3  90/06/02  14:56:03  rpd
 * 	Updated to new scheduling technology.
 * 	[90/03/26  22:17:10  rpd]
 * 
 * Revision 2.2  90/01/11  11:44:04  dbg
 * 	Export thread_bind.
 * 	[89/12/06            dbg]
 * 
 * Revision 2.1  89/08/03  15:53:39  rwd
 * Created.
 * 
 * 29-Jun-88  David Golub (dbg) at Carnegie-Mellon University
 *	Removed THREAD_SHOULD_TERMINATE.
 *
 * 16-May-88  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Added thread_wakeup_with_result routine; thread_wakeup
 *	is a special case.
 *
 * 16-Apr-88  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Added THREAD_RESTART wait result value.
 *
 * 29-Feb-88  David Black (dlb) at Carnegie-Mellon University
 *	thread_setrun is now a real routine.
 *
 * 13-Oct-87  David Golub (dbg) at Carnegie-Mellon University
 *	Moved thread_will_wait and thread_go to sched_prim_macros.h,
 *	to avoid including thread.h everywhere.
 *
 *  5-Oct-87  David Golub (dbg) at Carnegie-Mellon University
 *	Created.  Moved thread_will_wait and thread_go here from
 *	mach_ipc.
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
 *	File:	sched_prim.h
 *	Author:	David Golub
 *
 *	Scheduling primitive definitions file
 *
 */

#ifndef	_KERN_SCHED_PRIM_H_
#define _KERN_SCHED_PRIM_H_

#include <mach/boolean.h>
#include <mach/machine/vm_types.h>
#include <mach/kern_return.h>
#include <kern/kern_types.h>
#include <kern/lock.h>
/*
 *	Possible results of assert_wait - returned in
 *	current_thread()->wait_result.
 */
#define THREAD_AWAKENED		0		/* normal wakeup */
#define THREAD_TIMED_OUT	1		/* timeout expired */
#define THREAD_INTERRUPTED	2		/* interrupted by clear_wait */
#define THREAD_RESTART		3		/* restart operation entirely */

typedef void (*unlock_fcn_t)(void * lock);

/*
 *	Exported interface to sched_prim.c.
 *	A few of these functions are actually defined in
 *	ipc_sched.c, for historical reasons.
 */

/* Initialize scheduler module */
extern void		sched_init(void);

/* Set timeout for current thread */
extern void		thread_set_timeout(
				int	t);	/* timeout interval in ticks */

/* Called when timeout expires at splsoftclock */
extern void		thread_timeout(
				thread_t thread);

/*
 * Set up thread timeout element when thread is created.
 */
extern void		thread_timeout_setup(
				thread_t	thread);

/* Declare thread will wait on a particular event */
extern void		assert_wait(
				int		event,
				boolean_t	interruptible);

/* Wake up thread directly, passing result */
extern void		clear_wait(
				register thread_t	thread,
				int			result,
				boolean_t		interrupt_only);

/* Wake up thread (or threads) waiting on a particular event */
extern void		thread_wakeup_prim(
				register int	event,
				boolean_t	one_thread,
				int		result,
				boolean_t	debug);

/* Have current thread sleep on a particular event.
 *
 * Combiniation of assert_wait, simple_unlock, thread block.
 */
extern void		thread_sleep_spinlock(
				int		event,
				spinlock_t	*lock,
				boolean_t	interruptible);

extern void		thread_sleep_mutex(
				int		event,
				mutex_t		*lock,
				boolean_t	interruptible);

extern void		thread_sleep_interlock(
				int		event,
				hw_lock_t	*lock,
				boolean_t	interruptible);

#if	NCPUS > 1
/* Bind thread to a particular processor */
extern void		thread_bind(
				thread_t	thread,
				processor_t	processor);
#else	/*NCPUS > 1*/
#define thread_bind(thread,processor)
#endif	/*NCPUS > 1*/

/* Select a thread to run on a particular processor */
extern thread_t		thread_select(
				processor_t	myprocessor);

/* Stop old thread and run new thread */
extern void		thread_invoke(
				thread_t	old_thread,
				void		(*continuation)(void),
				thread_t	new_thread,
				int		reason);

/* Called when current thread is given new stack */
extern void		thread_continue(
				thread_t	old_thread);

/* Block current thread, indicating reason (Block or Quantum expiration) */
extern void		thread_block_reason(
				void	(*continuation)(void),
				int	reason);

/* Block current thread (Block reason) */
extern void		thread_block(
				void	(*continuation)(void));

/* Switch directly to a particular thread */
extern void		thread_run(
				void	(*continuation)(void),
				thread_t	new_thread);

/* Dispatch a thread not on a run queue */
extern void		thread_dispatch(
				thread_t	thread);

/* Make thread runnable */
extern void		thread_setrun(
				thread_t	th,
				boolean_t	may_preempt,
				boolean_t	tail);

/* Invoke continuation */
extern void		call_continuation(
				void		(*continuation)(void));

/* Compute effective priority of the specified thread */
extern void		compute_priority(
				thread_t	thread,
			        int		resched);

/* Version of compute_priority for current thread or
 * thread being manipuldated by scheduler.
 */
extern void		compute_my_priority(
				thread_t	thread);

/* Recompute priorities of all threads (done periodically) */
extern void		recompute_priorities(void);

/* Update priority of thread that has been sleeping or suspended.
 * Used to "catch up" with the system.
 */
extern void		update_priority(
				thread_t	th);

/* Idle thread loop */
extern void		idle_thread(void);

/* Scheduling thread loop */
extern void		sched_thread(void);


/*
 *	Flags for thread_setrun()
 */

#define HEAD_Q	0	/* FALSE */
#define TAIL_Q	1	/* TRUE */

/*
 *	Routines defined as macros
 */

#define thread_wakeup(x)						\
		thread_wakeup_prim((x), FALSE, THREAD_AWAKENED, FALSE)
#define thread_wakeup_with_result(x, z)					\
		thread_wakeup_prim((x), FALSE, (z), FALSE)
#define thread_wakeup_one(x)						\
		thread_wakeup_prim((x), TRUE, THREAD_AWAKENED, FALSE)
#define	thread_wakeup_debug(x)						\
		thread_wakeup_prim((x), FALSE, THREAD_AWAKENED, TRUE);

/*
 *	Machine-dependent code must define these functions.
 */

/* Start thread running */
extern void	thread_bootstrap_return(void);

/* Return from exception */
extern void	thread_exception_return(void);

extern thread_t	switch_context(
			thread_t	old_thread,
			void		(*continuation)(void),
			thread_t	new_thread);

/* Attach stack to thread */
extern void	machine_kernel_stack_init(
			thread_t	thread,
			void		(*continuation)(void));

extern void	load_context(
			thread_t	thread);

/*
 *	These functions are either defined in kern/thread.c
 *	or are defined directly by machine-dependent code.
 */

/* Allocate an activation stack */
extern vm_offset_t stack_alloc(void);

/* Free an activation stack */
extern void	stack_free(
			vm_offset_t	stack);

/* Collect excess kernel stacks */
extern void	stack_collect(void);

#endif	/* _KERN_SCHED_PRIM_H_ */
