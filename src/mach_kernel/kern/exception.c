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
 * Revision 2.10.5.3  92/09/15  17:21:27  jeffreyh
 * 	On an iPSC860 drop into ddb instead of calling thread_kdb_return
 * 	until a real fix happens.
 * 	[92/09/15  15:23:53  jeffreyh]
 * 
 * Revision 2.10.5.2  92/03/28  10:10:01  jeffreyh
 * 	Pick up changes from MK71
 * 	[92/03/20  13:18:37  jeffreyh]
 * 
 * Revision 2.11  92/03/03  00:44:45  rpd
 * 	Changed debug_user_with_kdb to FALSE.
 * 	[92/03/02            rpd]
 * 
 * Revision 2.10.5.1  92/02/21  11:23:44  jsb
 * 	Don't convert exception kmsg to network format.
 * 	[92/02/21  09:02:59  jsb]
 * 
 * Revision 2.10  91/12/14  14:18:23  jsb
 * 	Removed ipc_fields.h hack.
 * 
 * Revision 2.9  91/12/13  13:41:38  jsb
 * 	Added NORMA_IPC support.
 * 
 * Revision 2.8  91/08/28  11:14:26  jsb
 * 	Added seqno arguments to ipc_mqueue_receive
 * 	and exception_raise_continue_slow.
 * 	[91/08/10            rpd]
 * 
 * 	Changed msgh_kind to msgh_seqno.
 * 	[91/08/10            rpd]
 * 
 * 	Changed exception_no_server to print an informative message
 * 	before invoking kdb, so this doesn't look like a kernel bug.
 * 	[91/08/09            rpd]
 * 
 * Revision 2.7  91/06/25  10:28:06  rpd
 * 	Fixed ikm_cache critical sections to avoid blocking operations.
 * 	[91/05/23            rpd]
 * 
 * Revision 2.6  91/05/14  16:41:02  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/03/16  14:49:47  rpd
 * 	Fixed assertion typo.
 * 	[91/03/08            rpd]
 * 	Replaced ipc_thread_switch with thread_handoff.
 * 	Replaced ith_saved with ikm_cache.
 * 	[91/02/16            rpd]
 * 
 * Revision 2.4  91/02/05  17:26:05  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:11:53  mrt]
 * 
 * Revision 2.3  91/01/08  15:15:36  rpd
 * 	Added KEEP_STACKS support.
 * 	[91/01/08  14:11:40  rpd]
 * 
 * 	Replaced thread_doexception with new, optimized exception path.
 * 	[90/12/22            rpd]
 * 
 * Revision 2.2  90/06/02  14:53:44  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  22:04:29  rpd]
 * 
 *
 * Condensed history:
 *	Changed thread_doexception to return boolean (dbg).
 *	Removed non-MACH code (dbg).
 *	Added thread_exception_abort (dlb).
 *	Use port_alloc, object_copyout (rpd).
 *	Removed exception-port routines (dbg).
 *	Check for thread halt condition if exception rpc fails (dlb).
 *	Switch to master before calling uprintf and exit (dbg).
 *	If rpc fails in thread_doexception, leave ports alone (dlb).
 *	Translate global port names to local port names (dlb).
 *	Rewrote for exc interface (dlb).
 *	Created (dlb).
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

#include <norma_ipc.h>
#include <mach_kdb.h>

#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/message.h>
#include <mach/port.h>
#include <mach/mig_errors.h>
#include <mach/thread_status.h>
#include <mach/exception.h>
#include <mach/mach_server.h>
#include <ipc/port.h>
#include <ipc/ipc_entry.h>
#include <ipc/ipc_object.h>
#include <ipc/ipc_notify.h>
#include <ipc/ipc_space.h>
#include <ipc/ipc_pset.h>
#include <ipc/mach_msg.h>
#include <kern/exception.h>
#include <kern/counters.h>
#include <kern/ipc_sched.h>
#include <kern/ipc_tt.h>
#include <kern/task.h>
#include <kern/thread.h>
#include <kern/processor.h>
#include <kern/sched.h>
#include <kern/sched_prim.h>
#include <kern/host.h>
#include <kern/misc_protos.h>
#include <string.h>
#include <mach/exc_user.h>

#if	MACH_KDB
#include <ddb/db_trap.h>
#endif	/* MACH_KDB */

/*
 * Forward declarations
 */
void exception_try_task(
	exception_type_t	exception,
	exception_data_t	code,
	int			codeCnt);

void exception_no_server(void);


#if	MACH_KDB

#include <ddb/db_output.h>

#if iPSC386 || iPSC860
boolean_t debug_user_with_kdb = TRUE;
#else
boolean_t debug_user_with_kdb = FALSE;
#endif

#endif	/* MACH_KDB */

unsigned long c_thr_exc_raise = 0;
unsigned long c_thr_exc_raise_state = 0;
unsigned long c_thr_exc_raise_state_id = 0;
unsigned long c_tsk_exc_raise = 0;
unsigned long c_tsk_exc_raise_state = 0;
unsigned long c_tsk_exc_raise_state_id = 0;

/*
 * We only use the machine-independent exception() routine
 * if a faster MD version isn't available.
 */
#ifndef MACHINE_FAST_EXCEPTION	/* from <machine/thread.h> if at all */

/*
 *	If continuations are not used/supported, the NOTREACHED comments
 *	below are incorrect.  The exception function is expected to return.
 *	So the return statements along the slow paths are important.
 */

/*
 *	Routine:	exception
 *	Purpose:
 *		The current thread caught an exception.
 *		We make an up-call to the thread's exception server.
 *	Conditions:
 *		Nothing locked and no resources held.
 *		Called from an exception context, so
 *		thread_exception_return and thread_kdb_return
 *		are possible.
 *	Returns:
 *		Doesn't return.
 */

void
exception(
	exception_type_t	exception,
	exception_data_t	code,
	int			codeCnt	)
{
	ipc_thread_t		self = current_thread();
	thread_act_t		a_self = self->top_act;
	ipc_port_t		exc_port;
	int			i;
	struct exception_action *excp = &a_self->exc_actions[exception];
	int			flavor;
	kern_return_t		kr;

	if (exception == KERN_SUCCESS)
		panic("exception");

	self->ith_scatter_list = MACH_MSG_BODY_NULL;

	/*
	 *	Optimized version of retrieve_thread_exception.
	 */

	act_lock(a_self);
	assert(a_self->ith_self != IP_NULL);
	exc_port = excp->port;
	if (!IP_VALID(exc_port)) {
		act_unlock(a_self);
		exception_try_task(exception, code, codeCnt);
		/*NOTREACHED*/
		return;
	}
	flavor = excp->flavor;

	ip_lock(exc_port);
	act_unlock(a_self);
	if (!ip_active(exc_port)) {
		ip_unlock(exc_port);
		exception_try_task(exception, code, codeCnt);
		/*NOTREACHED*/
		return;
	}

	/*
	 * Hold a reference to the port over the exception_raise_* calls
	 * so it can't be destroyed.  This seems like overkill, but keeps
	 * the port from disappearing between now and when
	 * ipc_object_copyin_from_kernel is finally called.
	 */
	ip_reference(exc_port);	
	/* exc_port->ip_srights++; ipc_object_copy_from_kernel does this */
	ip_unlock(exc_port);

	switch (excp->behavior) {
	case EXCEPTION_STATE: {
		unsigned int state_cnt;

		c_thr_exc_raise_state++;
		if (flavor == MACHINE_THREAD_STATE &&
			    is_is_kloaded_space(exc_port->ip_receiver)) {
			int *statep;
			/* Requested flavor is the same format in which
			 * we save state on this machine, so no copy is
			 * necessary.  Obtain direct pointer to saved state:
			 */
			statep = act_machine_state_ptr(self->top_act);
			state_cnt = MACHINE_THREAD_STATE_COUNT;
			kr = exception_raise_state(exc_port, exception,
				    code, codeCnt,
				    &flavor,
				    statep, state_cnt,
				    statep, &state_cnt);
			/* Server is required to return same flavor: */
			assert(flavor == MACHINE_THREAD_STATE);
		} else {
			int state[ THREAD_MACHINE_STATE_MAX ];

			state_cnt = state_count[flavor];
			kr = thread_getstatus(a_self, flavor, state, &state_cnt);
			if (kr == KERN_SUCCESS) {
			    kr = exception_raise_state(exc_port, exception,
					code, codeCnt,
					&flavor,
					state, state_cnt,
					state, &state_cnt);
			    if (kr == MACH_MSG_SUCCESS)
				kr = thread_setstatus(a_self, flavor, state, state_cnt);
			}
		}
		ip_lock(exc_port);
		ip_release(exc_port);	
		ip_unlock(exc_port);

		if (kr == KERN_SUCCESS ||
		    kr == MACH_RCV_PORT_DIED) {
			thread_exception_return();
			/* NOTREACHED */
			return;
		}
		} break;

	case EXCEPTION_DEFAULT:
		c_thr_exc_raise++;
		kr = exception_raise(exc_port,
				retrieve_act_self_fast(a_self),
				retrieve_task_self_fast(a_self->task),
				exception,
				code, codeCnt);
		ip_lock(exc_port);
		ip_release(exc_port);	
		ip_unlock(exc_port);

		if (kr == KERN_SUCCESS ||
		    kr == MACH_RCV_PORT_DIED) {
			thread_exception_return();
			/* NOTREACHED */
			return;
		}
		break;

	case EXCEPTION_STATE_IDENTITY: {
		unsigned int state_cnt;
		int state[ THREAD_MACHINE_STATE_MAX ];

		c_thr_exc_raise_state_id++;
		state_cnt = state_count[flavor];
		kr = thread_getstatus(a_self, flavor, state, &state_cnt);
		if (kr == KERN_SUCCESS) {
		    kr = exception_raise_state_identity(exc_port,
				retrieve_act_self_fast(a_self),
				retrieve_task_self_fast(a_self->task),
				exception,
				code, codeCnt,
				&flavor,
				state, state_cnt,
				state, &state_cnt);
		    if (kr == MACH_MSG_SUCCESS)
			kr = thread_setstatus(a_self, flavor, state, state_cnt);
		}
		ip_lock(exc_port);
		ip_release(exc_port);	
		ip_unlock(exc_port);

		if (kr == KERN_SUCCESS ||
		    kr == MACH_RCV_PORT_DIED) {
			thread_exception_return();
			/* NOTREACHED */
			return;
		}
		} break;
	default:
		panic ("bad behavior!");
	}/* switch */

	/*
	 * When a task is being terminated, it's no longer ripped
	 * directly out of the rcv from its "kill me" message, and
	 * so returns here.  The following causes it to return out
	 * to the glue code and clean itself up.
	 */
	if (thread_should_halt(self))
		thread_exception_return();

	exception_try_task(exception, code, codeCnt);
	/* NOTREACHED */
}
#endif /* defined MACHINE_FAST_EXCEPTION */

/*
 *	Routine:	exception_try_task
 *	Purpose:
 *		The current thread caught an exception.
 *		We make an up-call to the task's exception server.
 *	Conditions:
 *		Nothing locked and no resources held.
 *		Called from an exception context, so
 *		thread_exception_return and thread_kdb_return
 *		are possible.
 *	Returns:
 *		Doesn't return.
 */

void
exception_try_task(
	exception_type_t	exception,
	exception_data_t	code,
	int			codeCnt	)
{
	thread_act_t	a_self = current_act();
	ipc_thread_t self = a_self->thread;
	register task_t task = a_self->task;
	register ipc_port_t exc_port;
	int flavor;
	kern_return_t kr;

	self->ith_scatter_list = MACH_MSG_BODY_NULL;

	/*
	 *	Optimized version of retrieve_task_exception.
	 */

	itk_lock(task);
	assert(task->itk_self != IP_NULL);
	exc_port = task->exc_actions[exception].port;
	if (exception == EXC_MACH_SYSCALL && exc_port == realhost.host_self) {
		itk_unlock(task);
		restart_mach_syscall();		/* magic ! */
		/* NOTREACHED */
	}
	if (!IP_VALID(exc_port)) {
		itk_unlock(task);
		exception_no_server();
		/*NOTREACHED*/
		return;
	}
	flavor = task->exc_actions[exception].flavor;

	ip_lock(exc_port);
	itk_unlock(task);
	if (!ip_active(exc_port)) {
		ip_unlock(exc_port);
		exception_no_server();
		/*NOTREACHED*/
		return;
	}

	/*
	 * Hold a reference to the port over the exception_raise_* calls
	 * (see longer comment in exception())
	 */
	ip_reference(exc_port);
	/* exc_port->ip_srights++; */
	ip_unlock(exc_port);

	switch (task->exc_actions[exception].behavior) {
	case EXCEPTION_STATE: {
		unsigned int state_cnt;
		int state[ THREAD_MACHINE_STATE_MAX ];

		c_tsk_exc_raise_state++;
		state_cnt = state_count[flavor];
		kr = thread_getstatus(a_self, flavor, state, &state_cnt);
		if (kr == KERN_SUCCESS) {
		    kr = exception_raise_state(exc_port, exception,
				code, codeCnt,
				&flavor,
				state, state_cnt,
				state, &state_cnt);
		    if (kr == KERN_SUCCESS)
			kr = thread_setstatus(a_self, flavor, state, state_cnt);
		}
		ip_lock(exc_port);
		ip_release(exc_port);
		ip_unlock(exc_port);

		if (kr == MACH_MSG_SUCCESS || kr == MACH_RCV_PORT_DIED) {
		    thread_exception_return();
		    /*NOTREACHED*/
		    return;
		}
		} break;

	case EXCEPTION_DEFAULT:
		c_tsk_exc_raise++;
		kr = exception_raise(exc_port,
				retrieve_act_self_fast(a_self),
				retrieve_task_self_fast(a_self->task),
				exception, code, codeCnt);
		ip_lock(exc_port);
		ip_release(exc_port);
		ip_unlock(exc_port);

		if (kr == KERN_SUCCESS ||
		    kr == MACH_RCV_PORT_DIED) {
			thread_exception_return();
			/* NOTREACHED */
			return;
		}
		break;

	case EXCEPTION_STATE_IDENTITY: {
		unsigned int state_cnt;
		int state[ THREAD_MACHINE_STATE_MAX ];

		c_tsk_exc_raise_state_id++;
		state_cnt = state_count[flavor];
		kr = thread_getstatus(a_self, flavor, state, &state_cnt);
		if (kr == KERN_SUCCESS) {
		    kr = exception_raise_state_identity(exc_port,
				retrieve_act_self_fast(a_self),
				retrieve_task_self_fast(a_self->task),
				exception,
				code, codeCnt,
				&flavor,
				state, state_cnt,
				state, &state_cnt);
		    if (kr == KERN_SUCCESS)
			kr = thread_setstatus(a_self, flavor, state, state_cnt);
		}
		ip_lock(exc_port);
		ip_release(exc_port);
		ip_unlock(exc_port);

		if (kr == MACH_MSG_SUCCESS || kr == MACH_RCV_PORT_DIED) {
		    thread_exception_return();
		    /*NOTREACHED*/
		    return;
		}
		} break;

	default:
		panic ("bad behavior!");
	}/* switch */

	exception_no_server();
	/*NOTREACHED*/
}

/*
 *	Routine:	exception_no_server
 *	Purpose:
 *		The current thread took an exception,
 *		and no exception server took responsibility
 *		for the exception.  So good bye, charlie.
 *	Conditions:
 *		Nothing locked and no resources held.
 *		Called from an exception context, so
 *		thread_kdb_return is possible.
 *	Returns:
 *		Doesn't return.
 */

void
exception_no_server(void)
{
	register ipc_thread_t self = current_thread();

	/*
	 *	If this thread is being terminated, cooperate.
	 *
	 * When a task is dying, it's no longer ripped
	 * directly out of the rcv from its "kill me" message, and
	 * so returns here.  The following causes it to return out
	 * to the glue code and clean itself up.
	 */
	if (thread_should_halt(self)) {
		thread_exception_return();
		panic("exception_no_server - 1");
	}


	if (self->top_act->task == kernel_task)
		panic("kernel task terminating\n");

#if	MACH_KDB
	if (debug_user_with_kdb) {
		/*
		 *	Debug the exception with kdb.
		 *	If kdb handles the exception,
		 *	then thread_kdb_return won't return.
		 */
		db_printf("No exception server, calling kdb...\n");
#if	iPSC860
		db_printf("Dropping into ddb, avoiding thread_kdb_return\n");
		gimmeabreak();
#endif
		thread_kdb_return();
	}
#endif	/* MACH_KDB */

	/*
	 *	All else failed; terminate task.
	 */

	(void) task_terminate(self->top_act->task);
	thread_terminate_self();
	/*NOTREACHED*/
	panic("exception_no_server: returning!");
}
