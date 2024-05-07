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
#include <mach/kern_return.h>
#include <mach/rpc.h>
#include <mach/i386/vm_param.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_pset.h>
#include <ipc/ipc_space.h>
#include <kern/thread.h>
#include <kern/cpu_number.h>
#include <kern/misc_protos.h>
#include <kern/spl.h>
#include <i386/thread.h>
#include <i386/eflags.h>
#include <i386/proc_reg.h>
#include <i386/seg.h>
#include <i386/tss.h>
#include <i386/user_ldt.h>
#include <i386/fpu.h>
#include <mach_prof.h>
#include <machine/asm.h>


/*
 * Do a direct RPC with only simple arguments.  Used by the
 * MiG client when short-circuiting within the same address
 * space, to effect stack/thread switching and control transfer.
 * The client code processes complex args.
 *
 * We assume port/portset and current shuttle are not locked
 * by caller, and leave them unlocked on exit.  Internally,
 * must lock shuttle before port/portset.
 *
 * NB:  Cleanliness has been sacrificed to efficiency in handling
 * the variable argument list of which portp indicates the
 * start.  On principle, we should use stdarg to access the
 * added arguments, but instead we depend on their being above
 * the port on the stack.
 */
kern_return_t
machine_rpc_simple(
	int		routine_num,
	int		argc,
	void		*portp)
{
	int			*new_argv;
	ipc_port_t		port = *(ipc_port_t *)portp;
	ipc_space_t		receiver = port->ip_receiver;
	kern_return_t		kr;
	mig_impl_routine_t	server_func;
	rpc_subsystem_t		subsystem = port->ip_subsystem;
	thread_act_t		old_act, new_act;
	thread_t		cur_thread = current_thread();
	vm_offset_t		new_sp;

	new_act = THR_ACT_NULL;


	/*
	 * To save several compares in the course of this routine, we
	 * depend on the fact that the opening members of an ipc_pset
	 * and an ipc_port (ip{,s}_object, ip{,s}_act) are laid out
	 * the same way.  Hence we operate on a portset as if it were
	 * a port.
	 */
	if (port->ip_pset != IPS_NULL)
		port = (ipc_port_t)(port->ip_pset);
	ip_lock(port);
	ip_reference(port);

	/*
	 * If the port's target isn't the kernel, then
	 * obtain a thread into which to migrate the
	 * shuttle, and migrate it.
	 *
	 * We assume we're called in two cases:  a) On
	 * the syscall exception path, we're running on
	 * a kernel stack but in the client thread.
	 * b) When a server requests Mach services,
	 * we're running on the server shuttle's stack
	 * and in its thread.
	 *
	 * In case a), we switch threads and switch
	 * stacks (to the new thread's stack).  In case
	 * b), we don't switch threads, but switch
	 * stacks to the kernel stack for the current
	 * thread.
	 */
	if (receiver != ipc_space_kernel) {
		new_act = thread_pool_get_act(port);	/* may block */
		if (new_act == THR_ACT_NULL) {		/* port destroyed */
			ip_release(port);		
			ip_unlock(port);
			return(MACH_RCV_PORT_DIED);
		}
		rpc_lock(cur_thread);
		old_act = cur_thread->top_act;
		act_attach(new_act, cur_thread, 0);
		new_sp = new_act->user_stack;
		act_unlock(new_act);
	} else {
		rpc_lock(cur_thread);
		old_act = cur_thread->top_act;
		new_sp = kernel_stack[cpu_number()];
	}
	/*
	 * Now transfer the caller's arguments to the new stack.
	 * Note: gcc steadfastly considers new_sp and new_argv
	 * to be synonyms.  To keep new_sp untouched while
	 * (effectively) incrementing new_argv, the asm fragment
	 * below _must_ appear as it currently does -- i.e., must
	 * be new_argv into a register itself.  (Since new_argv
	 * isn't used following the asm, on principle it could be
	 * deleted and new_sp used instead.)
	 */
	new_argv = (int *)new_sp - argc;
	new_sp = (vm_offset_t)new_argv;
	/*
	 */
	__asm__ volatile(
		"movl %0,%%ecx; jecxz 1f; \
			movl %2, %%edx; 0: movl (%1),%%eax; addl $4, %1; \
			movl %%eax,(%%edx); addl $4, %%edx; loop 0b; 1:"
		: /* no outputs */
		: "g" (argc), "r" (portp), "g" (new_argv)
		: "%ecx", "%eax", "%edx");

	rpc_unlock(cur_thread);
	ip_unlock(port);
	act_unlock(new_act);

	/*
	 * Set the server function here.
	 */
	assert(subsystem != 0);
	server_func = subsystem->routine[routine_num -
					 subsystem->start].impl_routine;
	assert(server_func != 0);
	/*
	 * Switch stacks, call the function, and switch back.  Since
X	 * we're switching stacks, the asm fragment can't use auto's
	 * for temporary storage; use a callee-saves register instead.
	 */
	__asm__ volatile(
		"movl %0, %%ebx; xchgl %%ebx,%%esp; \
			call %1; movl %%ebx,%%esp; movl %%eax,%2"
		: /* no outputs */
		: "g" (new_sp), "r" (server_func), "g" (kr)
		: "%ebx", "%eax", "%ecx", "%edx", "cc", "memory");

	/*
	 * Release our reference to the port/portset.
	 */
	ip_lock(port);
	ip_release(port);
	rpc_lock(cur_thread);

	/*
	 * If we switched threads coming in, switch again going out.
	 */
	if (receiver != ipc_space_kernel) {
		assert(cur_thread->top_act == new_act);
		act_detach(new_act, TRUE);
		assert(cur_thread->top_act == old_act);
	}

	rpc_unlock(cur_thread);
	ip_unlock(port);
	/*
	 * Check here for MIG_NO_REPLY return from server, and honor it.
	 * If thread should already halt, it will terminate at return to
	 * user mode; otherwise, block it here indefinitely.
	 */
	if (kr == MIG_NO_REPLY) {
		while (!thread_should_halt(cur_thread)) {
			thread_block((void (*)(void))0);
		}
	}
	return (kr);
}

#if MACH_PROF
/*
 * Put a "user" pc value into the current activation's PCB,
 * for later retrieval by profiling code.
 */
#define RECORD_USER_PC(act) 		\
MACRO_BEGIN 				\
	unsigned long _pc__; 		\
					\
	__asm__ volatile( 		\
		"movl 4(%%ebp),%0" 	\
		: "=r" (_pc__)); 	\
	user_pc(act) = _pc__; 		\
MACRO_END
#else
#define RECORD_USER_PC(act)
#endif

/*
 * XXX - need better way to access this
 */
vm_map_t port_name_to_map(mach_port_t);

#if MACH_ASSERT

#define switch_to_new_map(target, act, old, new)	\
	MACRO_BEGIN					\
	new = port_name_to_map(target);			\
	if (act->lower != THR_ACT_NULL)			\
		assert(new == act->lower->map);		\
	old = act->map;					\
	vm_map_ref_fast(new);				\
	act->map = new;					\
	PMAP_ACTIVATE_USER(act, cpu_number());		\
	MACRO_END

#define switch_to_old_map(act, old, new)		\
	MACRO_BEGIN					\
	PMAP_DEACTIVATE_USER(act, cpu_number());	\
	act->map = old;					\
	vm_map_dealloc_fast(new);			\
	vm_map_dealloc_fast(new);			\
	MACRO_END

#else	/* MACH_ASSERT */

#define switch_to_new_map(target, act, old, new)	\
	MACRO_BEGIN					\
	if (act->lower != THR_ACT_NULL)			\
		new = act->lower->map;			\
	else						\
		new = port_name_to_map(target);		\
	old = act->map;					\
	vm_map_ref_fast(new);				\
	act->map = new;					\
	PMAP_ACTIVATE_USER(act, cpu_number());		\
	MACRO_END

#define switch_to_old_map(act, old, new)		\
	MACRO_BEGIN					\
	PMAP_DEACTIVATE_USER(act, cpu_number());	\
	act->map = old;					\
	vm_map_dealloc_fast(new);			\
	if (act->lower == THR_ACT_NULL)			\
	    vm_map_dealloc_fast(new);			\
	MACRO_END

#endif	/* MACH_ASSERT */

/*
 * Wrap kernel's copyin function for use from a kernel-loaded server.
 * We assume that we're running in a server thread on its user stack,
 * so just switch to the thread's kernel stack "around" the call.
 */
boolean_t
klcopyin(
	mach_port_t	target,
	void 		*argp)
{
	thread_act_t		act;
	boolean_t	rv;
	int		*new_argv;
	vm_map_t	oldmap, map;
	vm_offset_t	new_sp;

	assert(active_threads[cpu_number()]);
	act = current_act_fast(cpu_number());
	RECORD_USER_PC(act);
	switch_to_new_map(target, act, oldmap, map);
	new_sp = kernel_stack[cpu_number()];
	/*
	 * Now transfer the caller's arguments to the new stack.
	 * gcc doesn't seem to take the asm fragments below into
	 * account in doing def/ref calculations, so alter them
	 * at your own risk...
	 */
	new_argv = (int *)new_sp - 3;	/* XXX - count in "int's" */
	new_sp = (vm_offset_t)new_argv;
	__asm__ volatile(
		"movl %1, %%edx; \
			movl (%0),%%eax; addl $4, %0; \
			 movl %%eax,(%%edx); addl $4, %%edx; \
			movl (%0),%%eax; addl $4, %0; \
			 movl %%eax,(%%edx); addl $4, %%edx; \
			movl (%0),%%eax; movl %%eax,(%%edx)"
		: /* no outputs */
		: "r" (argp), "g" (new_argv)
		: "%eax", "%edx");
	/*
	 * Switch stacks, call the function, and switch back.  Since
	 * we're switching stacks, the asm fragment can't use auto's
	 * for temporary storage; use a callee-saves register instead.
	 */
	__asm__ volatile(
		"movl %0, %%ebx; xchgl %%ebx,%%esp; call " CC_SYM_PREFIX "copyin; \
			movl %%ebx,%%esp; movl %%eax,%1"
		: /* no outputs */
		: "g" (new_sp), "g" (rv)
		: "%ebx", "%eax", "%ecx", "%edx", "cc", "memory");
	switch_to_old_map(act, oldmap, map);
	return (rv);
}

/*
 * Wrap kernel's copyout function for use from a kernel-loaded server.
 * We assume that we're running in a server thread on its user stack,
 * so just switch to the thread's kernel stack "around" the call.
 */
boolean_t
klcopyout(
	mach_port_t	target,
	void 		*argp)
{
	thread_act_t		act;
	boolean_t	rv;
	int		*new_argv;
	vm_map_t	oldmap, map;
	vm_offset_t	new_sp;

	assert(active_threads[cpu_number()]);
	act = current_act_fast(cpu_number());
	RECORD_USER_PC(act);
	switch_to_new_map(target, act, oldmap, map);
	new_sp = kernel_stack[cpu_number()];
	/*
	 * Now transfer the caller's arguments to the new stack.
	 * gcc doesn't seem to take the asm fragments below into
	 * account in doing def/ref calculations, so alter them
	 * at your own risk...
	 */
	new_argv = (int *)new_sp - 3;	/* XXX - count in "int's" */
	new_sp = (vm_offset_t)new_argv;
	__asm__ volatile(
		"movl %1, %%edx; \
			movl (%0),%%eax; addl $4, %0; \
			 movl %%eax,(%%edx); addl $4, %%edx; \
			movl (%0),%%eax; addl $4, %0; \
			 movl %%eax,(%%edx); addl $4, %%edx; \
			movl (%0),%%eax; movl %%eax,(%%edx)"
		: /* no outputs */
		: "r" (argp), "g" (new_argv)
		: "%eax", "%edx");
	/*
	 * Switch stacks, call the function, and switch back.  Since
	 * we're switching stacks, the asm fragment can't use auto's
	 * for temporary storage; use a callee-saves register instead.
	 */
	__asm__ volatile(
		"movl %0, %%ebx; xchgl %%ebx,%%esp; call " CC_SYM_PREFIX "copyout; \
			movl %%ebx,%%esp; movl %%eax,%1"
		: /* no outputs */
		: "g" (new_sp), "g" (rv)
		: "%ebx", "%eax", "%ecx", "%edx", "cc", "memory");
	switch_to_old_map(act, oldmap, map);
	return (rv);
}

/*
 * Call the "kernel's" "copyin string" function from a kernel-loaded
 * server.  We assume that we're running in a server thread on its
 * user stack, so just switch to the thread's kernel stack "around"
 * the call.  The copyinstr function was created just for use from
 * this wrapper -- the kernel doesn't employ it.
 */
boolean_t
klcopyinstr(
	mach_port_t	target,
	void 		*argp)
{
	thread_act_t		act;
	boolean_t	rv;
	int		*new_argv;
	vm_map_t	oldmap, map;
	vm_offset_t	new_sp;

	assert(active_threads[cpu_number()]);
	act = current_act_fast(cpu_number());
	RECORD_USER_PC(act);
	switch_to_new_map(target, act, oldmap, map);
	new_sp = kernel_stack[cpu_number()];
	/*
	 * Now transfer the caller's arguments to the new stack.
	 * gcc doesn't seem to take the asm fragments below into
	 * account in doing def/ref calculations, so alter them
	 * at your own risk...
	 */
	new_argv = (int *)new_sp - 4;	/* XXX - count in "int's" */
	new_sp = (vm_offset_t)new_argv;
	__asm__ volatile(
		"movl %1, %%edx; \
			movl (%0),%%eax; addl $4, %0; \
			 movl %%eax,(%%edx); addl $4, %%edx; \
			movl (%0),%%eax; addl $4, %0; \
			 movl %%eax,(%%edx); addl $4, %%edx; \
			movl (%0),%%eax; addl $4, %0; \
			 movl %%eax,(%%edx); addl $4, %%edx; \
			movl (%0),%%eax; movl %%eax,(%%edx)"
		: /* no outputs */
		: "r" (argp), "g" (new_argv)
		: "%eax", "%edx");
	/*
	 * Switch stacks, call the function, and switch back.  Since
	 * we're switching stacks, the asm fragment can't use auto's
	 * for temporary storage; use a callee-saves register instead.
	 */
	__asm__ volatile(
		"movl %0, %%ebx; xchgl %%ebx,%%esp; call "CC_SYM_PREFIX "copyinstr; \
			movl %%ebx,%%esp; movl %%eax,%1"
		: /* no outputs */
		: "g" (new_sp), "g" (rv)
		: "%ebx", "%eax", "%ecx", "%edx", "cc", "memory");
	switch_to_old_map(act, oldmap, map);
	return (rv);
}

/*
 * Call the kernel's thread_switch function from a kernel-loaded
 * server.  We assume that we're running in a server thread on its
 * user stack, so just switch to the thread's kernel stack "around"
 * the call.
 */
kern_return_t
klthread_switch(
	void *argp)
{
	boolean_t	rv;
	int		*new_argv;
	vm_offset_t	new_sp;

	RECORD_USER_PC(current_act_fast(cpu_number()));
	new_sp = kernel_stack[cpu_number()];
	/*
	 * Now transfer the caller's arguments to the new stack.
	 * gcc doesn't seem to take the asm fragments below into
	 * account in doing def/ref calculations, so alter them
	 * at your own risk...
	 */
	new_argv = (int *)new_sp - 3;	/* XXX - count in "int's" */
	new_sp = (vm_offset_t)new_argv;
	__asm__ volatile(
		"movl %1, %%edx; \
			movl (%0),%%eax; addl $4, %0; \
			 movl %%eax,(%%edx); addl $4, %%edx; \
			movl (%0),%%eax; addl $4, %0; \
			 movl %%eax,(%%edx); addl $4, %%edx; \
			movl (%0),%%eax; movl %%eax,(%%edx)"
		: /* no outputs */
		: "r" (argp), "g" (new_argv)
		: "%eax", "%edx");
	/*
	 * Switch stacks, call the function, and switch back.  Since
	 * we're switching stacks, the asm fragment can't use auto's
	 * for temporary storage; use a callee-saves register instead.
	 */
	__asm__ volatile(
		"movl %0, %%ebx; xchgl %%ebx,%%esp; \
			call " CC_SYM_PREFIX "syscall_thread_switch; \
			movl %%ebx,%%esp; movl %%eax,%1"
		: /* no outputs */
		: "g" (new_sp), "g" (rv)
		: "%ebx", "%eax", "%ecx", "%edx", "cc", "memory");
	return (rv);
}

/*
 * Call the kernel's thread_depress_abort function from a kernel-loaded
 * server.  We assume that we're running in a server thread on its
 * user stack, so just switch to the thread's kernel stack "around"
 * the call.
 */
kern_return_t
klthread_depress_abort(
	mach_port_t		name)
{
	boolean_t	rv;
	int		*new_argv;
	vm_offset_t	new_sp;

	RECORD_USER_PC(current_act_fast(cpu_number()));
	new_sp = kernel_stack[cpu_number()];
	/*
	 * Now transfer the caller's arguments to the new stack.
	 * gcc doesn't seem to take the asm fragments below into
	 * account in doing def/ref calculations, so alter them
	 * at your own risk...
	 */
	new_argv = (int *)new_sp - 1;	/* XXX - count in "int's" */
	new_sp = (vm_offset_t)new_argv;
	__asm__ volatile(
		"movl %0, %%edx; movl %%edx, %1"
		: /* no outputs */
		: "g" (name), "g" (*new_argv)
		: "%edx");
	/*
	 * Switch stacks, call the function, and switch back.  Since
	 * we're switching stacks, the asm fragment can't use auto's
	 * for temporary storage; use a callee-saves register instead.
	 */
	__asm__ volatile(
		"movl %0, %%ebx; xchgl %%ebx,%%esp; \
			call " CC_SYM_PREFIX "syscall_thread_depress_abort; \
			movl %%ebx,%%esp; movl %%eax,%1"
		: /* no outputs */
		: "g" (new_sp), "g" (rv)
		: "%ebx", "%eax", "%ecx", "%edx", "cc", "memory");
	return (rv);
}

#ifdef MACHINE_FAST_EXCEPTION	/* from <machine/thread.h> if at all */
/*
 * This is the fast, MD code, with lots of stuff in-lined.
 */

#include <mach/boolean.h>
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
#include <ipc/mach_msg.h>
#include <kern/exception.h>
#include <kern/counters.h>
#include <kern/ipc_sched.h>
#include <kern/ipc_tt.h>
#include <kern/task.h>
#include <kern/processor.h>
#include <kern/sched.h>
#include <kern/sched_prim.h>
#include <kern/host.h>
#include <kern/misc_protos.h>
#include <string.h>
#include <mach/exc_user.h>

/*
 * Forward declarations from kern/exception.c
 */
void exception_try_task(
	exception_type_t	exception,
	exception_data_t	code,
	int			codeCnt);

void exception_no_server(void);

extern unsigned long c_thr_exc_raise;
extern unsigned long c_thr_exc_raise_state;
extern unsigned long c_thr_exc_raise_state_id;

/*
 * Temporary: controls the syscall copyin optimization.
 * If TRUE, the exception function will copy in the first n
 * words from the stack of the user thread and store it in
 * the saved state, so that the server doesn't have to do
 * this.
 */
boolean_t syscall_exc_copyin = TRUE;

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

	/* For this flavor, we must copy in the first few procedure call
	 * args from the user's stack, since that is part of the important
	 * state in a syscall exception (this is for performance -- we
	 * can do the copyin much faster than the server, even if it is
	 * kernel-loaded):
	 */
	if (flavor == i386_SAVED_STATE) {
		struct i386_saved_state *statep = (struct i386_saved_state *)
					act_machine_state_ptr(self->top_act);
		statep->argv_status = FALSE;
		if (syscall_exc_copyin && copyin((char *)statep->uesp,
			   (char *)statep->argv,
			   i386_SAVED_ARGV_COUNT * sizeof(int)) == 0) {
			/* Indicate success for the server: */
			statep->argv_status = TRUE;
		}
	}

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
	    rpc_subsystem_t subsystem = ((ipc_port_t)exc_port)->ip_subsystem;

	    if (flavor == MACHINE_THREAD_STATE
			&& subsystem /* && subsystem->start == 2400 */
			&& is_fast_space(exc_port->ip_receiver)) {

		int *statep = act_machine_state_ptr(self->top_act);

		state_cnt = MACHINE_THREAD_STATE_COUNT;
		/*
		 * In-lined expansion of
		 * kr = exception_raise_state(exc_port, exception,
		 *		code, codeCnt,
		 *		&flavor, statep, state_cnt,
		 *		statep, &state_cnt);
		 * and machine_rpc_simple(port, 2402, 9, (9 E_R_S args))
		 * which comprises the bulk of E_R_S.
		 */
		{
		thread_act_t		old_act, new_act = THR_ACT_NULL;
		ipc_port_t		port = exc_port;
		mig_impl_routine_t	server_func;
		int *			new_sp;


		/*
		 * To save several compares in the course of this routine, we
		 * depend on the fact that the opening members of an ipc_pset
		 * and an ipc_port (ip{,s}_object, ip{,s}_act) are laid out
		 * the same way.  Hence we operate on a portset as if it were
		 * a port.
		 */
		if (port->ip_pset != IPS_NULL)
			port = (ipc_port_t)(port->ip_pset);
		ip_lock(port);
		ip_reference(port);

		/*
		 * If the port's target isn't the kernel, then
		 * obtain a thread into which to migrate the
		 * shuttle, and migrate it.
		 *
		 * In this in-lined routine, we're called only on the
		 * syscall exception path, we're running on a kernel
		 * stack but in the client thread.  So, we switch threads
		 * and switch stacks (to the new thread's stack).
		 */
#if 1
		/* InLine expansion of:
		 * new_act = thread_pool_get_act(port);
		 */
		{   thread_pool_t thread_pool = &port->ip_thread_pool;
		    while ((new_act = thread_pool->thr_acts) == THR_ACT_NULL) {
                	    if (!ip_active(port)) {
                        	break;
			    }
			    thread_pool->waiting = 1;
			    assert_wait((int)thread_pool, FALSE);
			    ip_unlock(port);
			    thread_block((void (*)(void)) 0);
			    ip_lock(port);
		    }
		    if (new_act != THR_ACT_NULL) {
		    	assert(new_act->thread == THREAD_NULL);
		    	assert(new_act->suspend_count == 0);
		    	thread_pool->thr_acts = new_act->thread_pool_next;
		    	act_lock(new_act);
		    }
		} /* end InLine expansion of thread_pool_get_act */
#else
		new_act = thread_pool_get_act(port);
#endif
                if (new_act == THR_ACT_NULL) {		/* port destroyed */
                        ip_release(port);               
                        ip_release(port);               
                        ip_unlock(port);
                        thread_exception_return();
			/* NOTREACHED*/
                }

		rpc_lock(self);
		old_act = self->top_act;

		/*
		 * In-line expansion of act_attach(new_act, self, 0);
		 */
		{	thread_act_t lower;
			vm_offset_t stack;

		/* Chain the act onto the thread's act stack.  */
		/* Set mask and auto-propagate alerts from below */
		new_act->thread = self;
		new_act->higher = THR_ACT_NULL;  /*safety*/
		new_act->alerts = 0;
		new_act->alert_mask = 0; /* init_alert_mask; */
		lower = new_act->lower = self->top_act;
		lower->higher = new_act;
		new_act->alerts = 0; /* (lower->alerts & init_alert_mask); */
		stack = new_act->kernel_stack;
		assert(stack != 0);
		/*
		 * We're changing the current activation, so need to
		 * update the kernel's cached values for it.  Ideally,
		 * the change and update would be atomic, but we can't
		 * arrange that.  WARNING: this code really can't be
		 * single-stepped through (XXX).
		 */
		assert(active_threads[cpu_number()] == self);
		active_kloaded[cpu_number()] = (new_act->kernel_loaded)
							? new_act : 0;
		MACHINE_STACK_STASH(stack);
		ast_context(new_act, cpu_number());
#if NOT
		PMAP_DEACTIVATE_USER(lower, cpu_number());
		PMAP_ACTIVATE_USER(new_act, cpu_number());
		act_machine_switch_pcb(new_act);
#endif
		self->top_act = new_act;
		} /* end in-line expansion of act_attach */

		new_sp = (int *) new_act->user_stack;

		/*
		 * Now transfer the caller's arguments to the new stack.
		 */
		new_sp -= 9; /* argc */
		new_sp[0] = (int) exc_port->ip_receiver_name;
		new_sp[1] = (int) exception;
		new_sp[2] = (int) code;
		new_sp[3] = (int) codeCnt;
		new_sp[4] = (int) &flavor;
		new_sp[5] = (int) statep;
		new_sp[7] = (int) statep;
		new_sp[6] = (int) MACHINE_THREAD_STATE_COUNT;
		new_sp[8] = (int) &state_cnt;

		ip_unlock(port);
		rpc_unlock(self);
		act_unlock(new_act);

		/*
		 * Set the server function,
		 * Switch stacks, call the function, and switch back.  Since
		 * we're switching stacks, the asm fragment can't use auto's
		 * for temporary storage; use a callee-saves register instead.
		 */
		server_func = subsystem->routine[ 2 ].impl_routine;
		assert(server_func != 0);
		__asm__ volatile(
		 "movl %0, %%ebx; xchgl %%ebx,%%esp; call %1; \
		  movl %%ebx,%%esp; movl %%eax,%2"
			: /* no outputs */
			: "g" (new_sp), "r" (server_func), "g" (kr)
			: "%ebx", "%eax", "%ecx", "%edx", "cc", "memory");

		/*
		 * Since we switched threads coming in, switch again going out.
		 */
		ip_lock(port);
		ip_release(port);
		/*
		 * Inline Expansion of  act_detach(new_act, TRUE);
		 */
		rpc_lock(self);

		/* Unlink the act from the thread's act stack */
		self->top_act = new_act->lower;
		new_act->thread = 0;
#if	MACH_ASSERT
		new_act->lower = new_act->higher = THR_ACT_NULL;
#endif
		{
		    thread_act_t old_act = self->top_act;
		    vm_offset_t old_stack = old_act->kernel_stack;

#if	MACH_ASSERT
		    old_act->higher = THR_ACT_NULL;
		    assert(old_stack != 0);
#endif	/* MACH_ASSERT */
		    /*
		     * We're changing the current activation, so need to
		     * update the kernel's cached value for it.  Ideally,
		     * the change and update would be atomic, but we can't
		     * arrange that.  WARNING: this code really can't be
		     * single-stepped through (XXX).
		     */
		    assert(active_threads[cpu_number()] == self);
		    active_kloaded[cpu_number()] =
					(old_act->kernel_loaded) ? old_act : 0;
		    MACHINE_STACK_STASH(old_stack);
		    ast_context(old_act, cpu_number());
		    PMAP_DEACTIVATE_USER(new_act, cpu_number());
		    PMAP_ACTIVATE_USER(old_act, cpu_number());
#if	0
		    /*
		     * InLine Expansion of act_machine_switch_pcb(old_act);
		     */
		    {
			pcb_t pcb = old_act->mact.pcb;
			assert( (pcb->iss.efl & EFL_VM) == 0 );
			curr_ktss(cpu_number())->esp0 =
					    (vm_offset_t) (&pcb->iss.v86_segs);
			set_ldt(KERNEL_LDT);
			fpu_load_context(pcb);
		    } /* End InLine Expansion of act_machine_switch_pcb */
#else
		    act_machine_switch_pcb(old_act);
#endif
			
		    if (old_act->user_stop_count || old_act->suspend_count) {
			    int	s;

			    s = splsched();
			    thread_lock(self);
			    self->state |= TH_SUSP;
			    thread_unlock(self);
			    splx(s);
		    }
		}

		/* Return it to the thread_pool's list, if it is still alive: */
		{
		    thread_pool_t cur_pool = &new_act->pool_port->ip_thread_pool;

		    assert(new_act->pool_port);
		    if (new_act->active) {
			assert(cur_pool);
			new_act->thread_pool_next = cur_pool->thr_acts;
			cur_pool->thr_acts = new_act;
			if (cur_pool->waiting)
				thread_pool_wakeup(cur_pool);
		    } else if (cur_pool) {
			/* Remove it from its thread_pool, which will
			 * deallocate it and destroy it:
			 */
			act_set_thread_pool(new_act, IP_NULL);
		    }
		}
		rpc_unlock(self);
		/*
		 * End Inline Expansion of act_detach(new_act, TRUE);
		 */

		ip_unlock(port);
		} /* end in-line expansion of E_R_S() and machine_rpc_simple */

		ip_lock(exc_port);
		ip_release(exc_port);	
		ip_unlock(exc_port);

		/*
		 * Check here for MIG_NO_REPLY return from server, and honor it.
		 * If thread should already halt, it will terminate at return to
		 * user mode; otherwise, block it here indefinitely.
		 */
		if (kr == MIG_NO_REPLY) {
			while (!thread_should_halt(self)) {
				thread_block((void (*)(void))0);
			}
		}
		thread_exception_return();
		/* NOTREACHED*/
		return;
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
	if (self->top_act && !self->top_act->active)
		thread_exception_return();

	exception_try_task(exception, code, codeCnt);
	/* NOTREACHED */
}
#endif /* defined MACHINE_FAST_EXCEPTION */

