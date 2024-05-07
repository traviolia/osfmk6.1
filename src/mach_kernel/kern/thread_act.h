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
 * Revision 1.1.2.8  1994/02/26  17:38:42  bolinger
 * 	Merge up to colo_shared.
 * 	[1994/02/26  17:31:59  bolinger]
 *
 * 	Move profiling data from thread to activation.
 * 	[1994/02/25  23:44:17  bolinger]
 *
 * Revision 1.1.2.7  1994/02/25  23:15:11  dwm
 * 	To cope with usage in MIG stubs, act_reference and
 * 	act_deallocate must guard against null.
 * 	[1994/02/25  23:15:04  dwm]
 * 
 * Revision 1.1.2.6  1994/02/09  00:42:59  dwm
 * 	Remove various unused future hooks to conserve wired memory.
 * 	[1994/02/09  00:35:22  dwm]
 * 
 * Revision 1.1.2.5  1994/01/26  15:47:27  bolinger
 * 	Merge up to colo_shared.
 * 	[1994/01/25  22:58:09  bolinger]
 * 
 * 	Keep prototypes current.
 * 	[1994/01/25  18:52:06  bolinger]
 * 
 * Revision 1.1.2.4  1994/01/25  18:37:39  dwm
 * 	Move ast here from shuttle.
 * 	[1994/01/25  18:34:29  dwm]
 * 
 * Revision 1.1.2.3  1994/01/17  18:09:08  dwm
 * 	add dump_act() proto - debugging.
 * 	[1994/01/17  16:07:03  dwm]
 * 
 * Revision 1.1.2.2  1994/01/14  18:27:34  dwm
 * 	Coloc: comment out unused act_yank
 * 	[1994/01/14  17:59:39  dwm]
 * 
 * Revision 1.1.2.1  1994/01/12  17:53:13  dwm
 * 	Coloc: initial restructuring to follow Utah model.
 * 	This file defines the activation structure.
 * 	[1994/01/12  17:15:19  dwm]
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
 *      Author: Bryan Ford, University of Utah CSL
 *
 *      File:   thread_act.h
 *
 *      thread activation definitions
 */

#ifndef	_KERN_THREAD_ACT_H_
#define _KERN_THREAD_ACT_H_

#include <mach/vm_param.h>
#include <kern/lock.h>
#include <kern/queue.h>
#include <machine/thread_act.h>
#include <kern/thread_pool.h>

/* Here is a description of the states an thread_activation may be in.
 *
 * An activation always has a valid task pointer, and it is always constant.
 * The activation is only linked onto the task's activation list until
 * the activation is terminated.
 *
 * An activation is in use or not, depending on whether its thread
 * pointer is nonzero.  If it is not in use, it is just sitting idly
 * waiting to be used by a thread.  The thread holds a reference on
 * the activation while using it.
 *
 * An activation lives on an thread_pool if its pool_port pointer is nonzero.
 * When in use, it can still live on an thread_pool, but it is not actually
 * linked onto the thread_pool's list of available activations.  In this case,
 * the act will return to its thread_pool as soon as it becomes unused.
 *
 * An activation is active until thread_terminate is called on it;
 * then it is inactive, waiting for all references to be dropped.
 * Future control operations on the terminated activation will fail,
 * with the exception that act_yank still works if the activation is
 * still on an RPC chain.  A terminated activation always has null
 * thread and pool_port pointers.
 *
 * An activation is suspended when suspend_count > 0.
 * A suspended activation can live on an thread_pool, but it is not
 * actually linked onto the thread_pool while suspended.
 *
 * Locking note:  access to data relevant to scheduling state (user_stop_count,
 * suspend_count, handlers, special_handler) is controlled by the combination
 * of locks acquired by act_lock_thread().  That is, not only must act_lock()
 * be held, but RPC through the activation must be frozen (so that the
 * thread pointer doesn't change).  If a shuttle is associated with the
 * activation, then its thread_lock() must also be acquired to change these
 * data.  Regardless of whether a shuttle is present, the data must be
 * altered at splsched().
 */

typedef struct ReturnHandler {
	struct ReturnHandler *next;
	void (*handler)(struct ReturnHandler *rh,
				struct thread_activation *thr_act);
} ReturnHandler;

typedef struct thread_activation {

	/*** task linkage ***/

	/* Links for task's circular list of activations.  The activation
	 * is only on the task's activation list while active.  Must be
	 * first.
	 */
	queue_chain_t	thr_acts;

	/* Indicators for whether this activation is in the midst of
	 * resuming or has already been resumed in a kernel-loaded
	 * task -- these flags are basically for quick access to
	 * this information.
	 */
	boolean_t	kernel_loaded;	/* running in kernel-loaded task */
	boolean_t	kernel_loading;	/* about to run kernel-loaded */

	/*** Machine-dependent state ***/
	struct MachineThrAct	mact;

	/*** Consistency ***/
	mutex_t		lock;
	spinlock_t	sched_lock;
	int		ref_count;

	/* Reference to the task this activation is in.
	 * Constant for the life of the activation
	 */
	struct task	*task;
	vm_map_t	map;		/* cached current map */

	vm_offset_t	kernel_stack;

	/*** thread_pool-related stuff ***/
	/* Port containing the thread_pool this activation normally lives
	 * on, zero if none.  The port (really the thread_pool) holds a
	 * reference to the activation as long as this is nonzero (even when
	 * the activation isn't actually on the thread_pool's list).
	 */
	struct ipc_port	*pool_port;

	/* Link on the thread_pool's list of activations.
	 * The activation is only actually on the thread_pool's list
	 * (and hence this is valid) when not in use (thread == 0) and
	 * not suspended (suspend_count == 0).
	 */
	struct thread_activation *thread_pool_next;

	/* User stack for initialization on migrating RPCs */
	vm_offset_t	user_stack;
	/* User stack size: added for upcalls in RPC */
	vm_size_t	user_stack_size;

	/* RPC state */
	vm_offset_t		subsystem;
	vm_offset_t		port_flags;
	int			routine_num;
	vm_offset_t		sig_ptr;
	vm_size_t		sig_size;
	vm_offset_t		arg_buf;
	vm_offset_t		state;
	vm_offset_t		new_argv;

	/*** Thread linkage ***/
	/* Shuttle using this activation, zero if not in use.  The shuttle
	 * holds a reference on the activation while this is nonzero.
	 */
	struct thread_shuttle	*thread;

	/* The rest in this section is only valid when thread is nonzero.  */

	/* Next higher and next lower activation on the thread's activation
	 * stack.  For a topmost activation or the null_act, higher is
	 * undefined.  The bottommost activation is always the null_act.
	 */
	struct thread_activation *higher, *lower;

	/* Alert bits pending at this activation; some of them may have
	 * propagated from lower activations.
	 */
	unsigned	alerts;

	/* Mask of alert bits to be allowed to pass through from lower levels.
	 */
	unsigned	alert_mask;

	/*** Control information ***/

	/* Number of outstanding suspensions on this activation.  */
	int		suspend_count;

	/* User-visible scheduling state */
	int		user_stop_count;	/* outstanding stops */

	/* ast is needed - see ast.h */
	int		ast;

	/* This is normally true, but is set to false when the
	 * activation is terminated.
	 */
	int		active;

	/* Chain of return handlers to be called before the thread is
	 * allowed to return to this invocation
	 */
	ReturnHandler	*handlers;

	/* A special ReturnHandler attached to the above chain to
	 * handle suspension and such
	 */
	ReturnHandler	special_handler;

	/* Special ports attached to this activation */
	struct ipc_port *ith_self;	/* not a right, doesn't hold ref */
	struct ipc_port *ith_sself;	/* a send right */
	struct exception_action exc_actions[EXC_TYPES_COUNT];

	/* A list of ulocks (a lock set element) currently held by the thread
	 */
	queue_head_t	held_ulocks;

#if	MACH_PROF
	/* Profiling data structures */
	boolean_t	act_profiled;	/* is activation being profiled? */
	boolean_t	act_profiled_own;
					/* is activation being profiled 
					 * on its own ? */
	struct prof_data *profil_buffer;/* prof struct if either is so */
#endif	/* MACH_PROF */

} Thread_Activation;

#define THR_ACT_NULL ((thread_act_t)0)

/* Exported to world */
extern int *thread_states_counts;

extern kern_return_t	act_alert(thread_act_t, unsigned);
extern kern_return_t	act_alert_mask(thread_act_t, unsigned );
extern kern_return_t	act_create(struct task *, vm_offset_t, vm_size_t,
				   thread_act_t *);
extern kern_return_t	act_get_state(thread_act_t, int, thread_state_t,
				mach_msg_type_number_t *);
extern kern_return_t	act_set_state(thread_act_t, int, thread_state_t,
				mach_msg_type_number_t);
extern void		thread_states_counts_init(thread_act_t th);
extern kern_return_t	thread_abort(thread_act_t);
extern kern_return_t	thread_abort_safely(thread_act_t);
extern void		thread_force_terminate(thread_act_t);
extern void		thread_release(thread_act_t);
extern kern_return_t	thread_resume(thread_act_t);
extern kern_return_t	thread_suspend(thread_act_t);
extern kern_return_t	thread_terminate(thread_act_t);

/* Following are exported only to mach_host code */
extern kern_return_t	thread_dowait(thread_act_t, boolean_t);
extern void		thread_hold(thread_act_t);

#define	act_lock_init(thr_act)	mutex_init(&(thr_act)->lock)
#define	act_lock(thr_act)	mutex_lock(&(thr_act)->lock)
#define	act_lock_try(thr_act)	mutex_try(&(thr_act)->lock)
#define	act_unlock(thr_act)	mutex_unlock(&(thr_act)->lock)

#define		act_reference(thr_act)				\
		MACRO_BEGIN					\
		    if (thr_act) {				\
			act_lock(thr_act);			\
			(thr_act)->ref_count++;			\
			act_unlock(thr_act);			\
		    }						\
		MACRO_END

#define		act_deallocate(thr_act)				\
		MACRO_BEGIN					\
		    if (thr_act) {				\
			int new_value;				\
			act_lock(thr_act);			\
			new_value = --(thr_act)->ref_count;	\
			act_unlock(thr_act);			\
			if (new_value == 0){ act_free(thr_act); } \
		    }						\
		MACRO_END

extern void		act_init(void);
extern kern_return_t	act_create_kernel(task_t, vm_offset_t, vm_size_t,
					  thread_act_t *);
extern kern_return_t	act_set_thread_pool(thread_act_t, ipc_port_t);
extern kern_return_t	thread_get_special_port(thread_act_t, int,
					ipc_port_t *);
extern kern_return_t	thread_set_special_port(thread_act_t, int,
					ipc_port_t);
extern thread_t		act_lock_thread(thread_act_t);
extern void		act_unlock_thread(thread_act_t);
extern thread_act_t	thread_lock_act(thread_t);
extern void		act_attach(thread_act_t, thread_t, unsigned);
extern void		act_execute_returnhandlers(void);
extern void		act_detach(thread_act_t, boolean_t);
extern void		act_free(thread_act_t);

/* machine-dependent functions */
extern void		act_machine_return(kern_return_t);
extern void		act_machine_init(void);
extern kern_return_t	act_machine_create(struct task *, thread_act_t);
extern void		act_machine_stack_attach(thread_act_t);
extern void		act_machine_destroy(thread_act_t);
extern kern_return_t	act_machine_set_state(thread_act_t,
					int, int *, unsigned );
extern kern_return_t	act_machine_get_state(thread_act_t,
					int , int *, unsigned *);
extern void		act_machine_switch_pcb(thread_act_t);
extern int		dump_act(thread_act_t);	/* debugging */

#endif /* _KERN_THREAD_ACT_H_ */
