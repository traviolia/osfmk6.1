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
 * Revision 2.16.4.5  92/09/15  17:21:33  jeffreyh
 * 	Added kernel task & threads profiling.
 * 	[92/07/17            bernadat]
 * 
 * Revision 2.16.4.4  92/04/30  11:59:08  bernadat
 * 	Consider user pc when profiling a user thread in kernel mode
 * 	[92/04/14            bernadat]
 * 
 * Revision 2.16.4.3  92/02/21  14:23:10  jsb
 * 	Removed include of norma_ipc.h (again).
 * 
 * Revision 2.16.4.2  92/02/18  19:09:02  jeffreyh
 * 	Added call to profile in clock_interrupt()
 * 	(Bernard Tabib & Andrei Danes @ gr.osf.org)
 * 	[92/01/02            bernadat]
 * 
 * Revision 2.16.4.1  92/01/21  21:50:49  jsb
 * 	Removed NORMA_IPC code.
 * 	[92/01/17  11:38:55  jsb]
 * 
 * Revision 2.16  91/08/03  18:18:56  jsb
 * 	NORMA_IPC: added call to netipc_timeout in hardclock.
 * 	[91/07/24  22:30:22  jsb]
 * 
 * Revision 2.15  91/07/31  17:45:57  dbg
 * 	Fixed timeout race.  Implemented host_adjust_time.
 * 	[91/07/30  17:03:54  dbg]
 * 
 * Revision 2.14  91/05/18  14:32:29  rpd
 * 	Fixed timeout/untimeout to use a fixed-size array of timers
 * 	instead of a zone.
 * 	[91/03/31            rpd]
 * 	Fixed host_set_time to update the mapped time value.
 * 	Changed the mapped time value to include a check field.
 * 	[91/03/19            rpd]
 * 
 * Revision 2.13  91/05/14  16:44:06  mrt
 * 	Correcting copyright
 * 
 * Revision 2.12  91/03/16  14:50:45  rpd
 * 	Updated for new kmem_alloc interface.
 * 	[91/03/03            rpd]
 * 	Use counter macros to track thread and stack usage.
 * 	[91/03/01  17:43:15  rpd]
 * 
 * Revision 2.11  91/02/05  17:27:45  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:14:47  mrt]
 * 
 * Revision 2.10  91/01/08  15:16:22  rpd
 * 	Added continuation argument to thread_block.
 * 	[90/12/08            rpd]
 * 
 * Revision 2.9  90/11/05  14:31:27  rpd
 * 	Unified untimeout and untimeout_try.
 * 	[90/10/29            rpd]
 * 
 * Revision 2.8  90/10/12  18:07:29  rpd
 * 	Fixed calls to thread_bind in host_set_time.
 * 	Fix from Philippe Bernadat.
 * 	[90/10/10            rpd]
 * 
 * Revision 2.7  90/09/09  14:32:18  rpd
 * 	Use decl_simple_lock_data.
 * 	[90/08/30            rpd]
 * 
 * Revision 2.6  90/08/27  22:02:48  dbg
 * 	Add untimeout_try for multiprocessors.  Reduce lint.
 * 	[90/07/17            dbg]
 * 
 * Revision 2.5  90/06/02  14:55:04  rpd
 * 	Converted to new IPC and new host port technology.
 * 	[90/03/26  22:10:04  rpd]
 * 
 * Revision 2.4  90/01/11  11:43:31  dbg
 * 	Switch to master CPU in host_set_time.
 * 	[90/01/03            dbg]
 * 
 * Revision 2.3  89/08/09  14:33:09  rwd
 * 	Include mach/vm_param.h and use PAGE_SIZE instead of NBPG.
 * 	[89/08/08            rwd]
 * 	Removed timemmap to machine/model_dep.c
 * 	[89/08/08            rwd]
 * 
 * Revision 2.2  89/08/05  16:07:11  rwd
 * 	Added mappable time code.
 * 	[89/08/02            rwd]
 * 
 * 14-Jan-89  David Golub (dbg) at Carnegie-Mellon University
 *	Split into two new files: mach_clock (for timing) and priority
 *	(for priority calculation).
 *
 *  8-Dec-88  David Golub (dbg) at Carnegie-Mellon University
 *	Use sentinel for root of timer queue, to speed up search loops.
 *
 * 30-Jun-88  David Golub (dbg) at Carnegie-Mellon University
 *	Created.
 *
 */ 
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988 Carnegie Mellon University
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
 *	File:	clock_prim.c
 *	Author:	Avadis Tevanian, Jr.
 *	Date:	1986
 *
 *	Clock primitives.
 */
#include <cpus.h>
#include <stat_time.h>
#include <mach_prof.h>

#include <mach/boolean.h>
#include <mach/machine.h>
#include <mach/time_value.h>
#include <mach/vm_param.h>
#include <mach/vm_prot.h>
#include <kern/counters.h>
#include <kern/cpu_number.h>
#include <kern/host.h>
#include <kern/lock.h>
#include <kern/mach_param.h>
#include <kern/misc_protos.h>
#include <kern/processor.h>
#include <kern/profile.h>
#include <kern/posixtime.h>
#include <kern/sched.h>
#include <kern/sched_prim.h>
#include <kern/spl.h>
#include <kern/thread.h>
#include <kern/time_out.h>
#include <vm/vm_kern.h>
#include <sys/time.h>
#include <machine/mach_param.h>	/* HZ */

#include <mach/mach_server.h>
#include <mach/mach_host_server.h>

#include <gprof.h>
#if     GPROF
/* all a hack to get necessary globals, etc */
extern  unsigned long   s_textsize;
extern  int             profiling;
extern  unsigned short  *kcount;
extern  char            *s_lowpc;
#define HISTFRACTION 2	/* must be consistent with other defs */
#endif

#if	STAT_TIME
#define	TICKBUMP(t)	timer_bump(t, (1000000/HZ))
#else
#define	TICKBUMP(t)
#endif

/* Forwards */
void	softclock(void);
void	timeout_tick(void);

/*
 * Hertz rate clock interrupt servicing. Primarily used to
 * update CPU statistics, recompute thread priority, and to
 * do profiling
 */
void
hertz_tick(
	boolean_t	usermode,	/* executing user code */
	unsigned int	pc)
{
	extern char		etext;
	thread_act_t		thr_act;
	boolean_t		inkernel;
	register int		my_cpu = cpu_number();
	register thread_t	thread = current_thread();
	int			state;

	inkernel = !usermode && (pc < (unsigned int)&etext);

	/*
	 * Hertz processing performed by all processors
	 * includes statistics gathering, state tracking,
	 * and quantum updating.
	 */
	counter(c_clock_ticks++);

	if (usermode) {
		if (thread->sched_pri == DEPRESSPRI) {
			/* Treat depressed pri threads as if they were the
			 * the idle thread running in user mode:
			 */
			TICKBUMP(&current_processor()->idle_thread->
								user_timer);
			state = CPU_STATE_IDLE;
		} else {
			TICKBUMP(&thread->user_timer);
			state = CPU_STATE_USER;
		}
	} else {
		if (thread->sched_pri == DEPRESSPRI) {
			/* Treat depressed pri threads as if they were the
			 * the idle thread running in kernel mode:
			 */
			TICKBUMP(&current_processor()->idle_thread->
								system_timer);
			state = CPU_STATE_IDLE;
		}
		else {
			switch(processor_ptr[my_cpu]->state) {
			case PROCESSOR_VIDLE:
				/*
				 * If we are a user thread doing an idle loop (VIDLE
				 * state), donate our time to the idle thread for
				 * this processor:
				 */
				TICKBUMP(&processor_ptr[my_cpu]->
					 idle_thread->system_timer);
				state = CPU_STATE_IDLE;
				break;

			case PROCESSOR_IDLE:
				TICKBUMP(&thread->system_timer);
				state = CPU_STATE_IDLE;
				break;

			default:
				TICKBUMP(&thread->system_timer);
				state = CPU_STATE_SYSTEM;
				break;
			}
		}
#if     GPROF
		{	int s = (char *)pc - s_lowpc;
			if (profiling < 2 && s < s_textsize)
				kcount[s / (HISTFRACTION * sizeof (*kcount))]++;
		}
#endif
	}
	machine_slot[my_cpu].cpu_ticks[state]++;
	thread_quantum_update(my_cpu, thread, 1, state);

	/*
	 * Hertz processing performed by the master-cpu
	 * exclusively.
	 */
	if (my_cpu == master_cpu) {
		timeout_tick();		/* update kernel timeout list   */
		utime_tick();		/* update Unix (universal) time */
	}

#if	MACH_PROF
	thr_act = thread->top_act;
	if (thr_act->act_profiled &&
	    processor_ptr[my_cpu]->state != PROCESSOR_VIDLE) {
		if (inkernel && thr_act->map != kernel_map)
			/* 
			 * Non-kernel thread running in kernel
			 * Register user pc (mach_msg, vm_allocate ...)
			 */
		  	profile(user_pc(thr_act), thr_act->profil_buffer);
		else
			/*
			 * User thread and user mode or
			 * user (server) thread in kernel-loaded server or
			 * kernel thread and kernel mode
			 * register interrupted pc
			 */
			profile(pc, thr_act->profil_buffer);
	}
	if (kernel_task->task_profiled &&
	    processor_ptr[my_cpu]->state != PROCESSOR_VIDLE) {
		if (inkernel && thr_act->map != kernel_map)
		  	/*
			 * User thread not profiled in kernel mode,
			 * kernel task profiled, register kernel pc
			 * for kernel task
			 */
			profile(pc, kernel_task->profil_buffer);
	}
#endif	/* MACH_PROF */
}

/*
 * Kernel timeout services.
 */

unsigned long	timeout_ticks = 0;	/* ticks elapsed since bootup */
spinlock_t	timer_lock;		/* lock for ... */
timer_elt_data_t	timer_head;	/* ordered list of timeouts */
					/* (doubles as end-of-list) */
/*
 *	There is a nasty race between softclock and reset_timeout.
 *	For example, scheduling code looks at timer_set and calls
 *	reset_timeout, thinking the timer is set.  However, softclock
 *	has already removed the timer but hasn't called thread_timeout
 *	yet.
 *
 *	Interim solution:  We initialize timers after pulling
 *	them out of the queue, so a race with reset_timeout won't
 *	hurt.  The timeout functions (eg, thread_timeout,
 *	thread_depress_timeout) check timer_set/depress_priority
 *	to see if the timer has been cancelled and if so do nothing.
 *
 *	This still isn't correct.  For example, softclock pulls a
 *	timer off the queue, then thread_go resets timer_set (but
 *	reset_timeout does nothing), then thread_set_timeout puts the
 *	timer back on the queue and sets timer_set, then
 *	thread_timeout finally runs and clears timer_set, then
 *	thread_set_timeout tries to put the timer on the queue again
 *	and corrupts it.
 */

void
softclock(void)
{
	/*
	 *	Handle timeouts.
	 */
	spl_t	s;
	register timer_elt_t	telt;
	register void	(*fcn)(void *param);
	register void	*param;
#if	NCPUS > 1
	processor_t	processor;
	thread_t	cur_thread = current_thread();
#endif	/* NCPUS > 1 */

	for (;;) {
	    while (TRUE) {
		s = splsched();
		spinlock_lock(&timer_lock);
		telt = (timer_elt_t) queue_first(&timer_head.chain);
		if (telt->ticks > timeout_ticks) {
		    spinlock_unlock(&timer_lock);
		    splx(s);
		    break;
		}
		fcn = telt->fcn;
		param = telt->param;
#if	NCPUS > 1
		processor = telt->bound_processor;
#endif	/* NCPUS > 1 */

		remqueue(&timer_head.chain, (queue_entry_t)telt);
		telt->set = TELT_UNSET;
		spinlock_unlock(&timer_lock);
		splx(s);

		assert(fcn != 0);
#if	NCPUS > 1
		if (processor != PROCESSOR_NULL) {
		    thread_bind(current_thread(), processor);
		    if (current_processor() != processor)
			thread_block((void (*)(void)) 0);
		}
#endif	/* NCPUS > 1 */
		(*fcn)(param);
#if	NCPUS > 1
		if (processor != PROCESSOR_NULL) {
		    thread_bind(current_thread(), PROCESSOR_NULL);
		}
		if (csw_needed(cur_thread, current_processor())) {
		    /*
		     * Switch to another processor if some threads are waiting
		     * for this one.
		     */
		    thread_block((void (*)(void)) 0);
		}
#endif	/* NCPUS > 1 */
	    }
	    assert_wait((int)&timer_head, FALSE);
	    thread_block((void (*)(void)) 0);
	}
}

/*
 *	Set timeout.
 *
 *	Parameters:
 *		telt	 timer element.  Function and param are already set.
 *		interval time-out interval, in hz.
 */
void
set_timeout(
	register timer_elt_t	telt,	/* already loaded */
	register unsigned int	interval)
{
	spl_t	s;
	register timer_elt_t	next;

	s = splsched();
	spinlock_lock(&timer_lock);

	interval += timeout_ticks;

	for (next = (timer_elt_t)queue_first(&timer_head.chain);
	     ;
	     next = (timer_elt_t)queue_next((queue_entry_t)next)) {

	    if (next->ticks > interval)
		break;
	}
	telt->ticks = interval;
	/*
	 * Insert new timer element before 'next'
	 * (after 'next'->prev)
	 */
	insque((queue_entry_t) telt, ((queue_entry_t)next)->prev);
	telt->set = TELT_SET;
	spinlock_unlock(&timer_lock);
	splx(s);
}

boolean_t
reset_timeout(
	register timer_elt_t	telt)
{
	spl_t	s;

	s = splsched();
	spinlock_lock(&timer_lock);
	if (telt->set) {
	    remqueue(&timer_head.chain, (queue_entry_t)telt);
	    telt->set = TELT_UNSET;
	    spinlock_unlock(&timer_lock);
	    splx(s);
	    return TRUE;
	}
	else {
	    spinlock_unlock(&timer_lock);
	    splx(s);
	    return FALSE;
	}
}

void
timeout_init(void)
{
	spinlock_init(&timer_lock);
	queue_init(&timer_head.chain);
	timer_head.ticks = ~0;	/* MAXUINT - sentinel */
	timeout_ticks = 0;
}

void
timeout_tick(void)
{
	register timer_elt_t	telt;
	boolean_t		do_wakeup = FALSE;
	spl_t			s;

	s = splsched();
	spinlock_lock(&timer_lock);
	timeout_ticks++;
	telt = (timer_elt_t)queue_first(&timer_head.chain);
	if (telt->ticks <= timeout_ticks)
		do_wakeup = TRUE;
	spinlock_unlock(&timer_lock);
	splx(s);

	if (do_wakeup)
		thread_wakeup((int)&timer_head);
}

/*
 *	Compatibility for device drivers.
 *	New code should use set_timeout/reset_timeout and private timers.
 *	These code can't use a zone to allocate timers, because
 *	it can be called from interrupt handlers.
 */

#define NTIMERS		20
timer_elt_data_t timeout_timers[NTIMERS];

/*
 *	Set timeout.
 *
 *	fcn:		function to call
 *	param:		parameter to pass to function
 *	interval:	timeout interval, in hz.
 */
void
timeout(
	timeout_fcn_t	fcn,
	void		*param,
	int		interval)
{
	spl_t	s;
	register timer_elt_t elt;

	s = splsched();
	spinlock_lock(&timer_lock);
	for (elt = &timeout_timers[0]; elt < &timeout_timers[NTIMERS]; elt++)
	    if (elt->set == TELT_UNSET)
		break;
	if (elt == &timeout_timers[NTIMERS])
	    panic("timeout");
	elt->fcn = fcn;
	elt->param = param;
	elt->set = TELT_ALLOC;

	/*	
	 * When invoking timeout() ask the softclock thread to bind itself on
	 * the master processor. This is mandatory for drivers on platforms 
	 * with assymetric i/o.
	 *
	 * We could fix all existing drivers and add thread_bind
	 * calls corresponding functions. This is a lot of work and
	 * would not solve the problem for contributed drivers.
	 *
	 * Instead of systematically binding to the master processor we
	 * could check if the calling thread is itself wired on a processor.
	 * Unfortunately, timeout can also be called interrupt time. It would
	 * be nice to have a generic call (with a machine dependant implementation)
	 * telling whether or not we are in interrupt mode or not.
	 */

#if	NCPUS > 1
	elt->bound_processor = master_processor;
#endif	/* NCPUS > 1 */
	spinlock_unlock(&timer_lock);
	splx(s);

	set_timeout(elt, (unsigned int)interval);
}

/*
 * Returns a boolean indicating whether the timeout element was found
 * and removed.
 */
boolean_t
untimeout(
	register timeout_fcn_t	fcn,
	register void		*param)
{
	spl_t	s;
	register timer_elt_t elt;

	s = splsched();
	spinlock_lock(&timer_lock);
	queue_iterate(&timer_head.chain, elt, timer_elt_t, chain) {

	    if ((fcn == elt->fcn) && (param == elt->param)) {
		/*
		 *	Found it.
		 */
		remqueue(&timer_head.chain, (queue_entry_t)elt);
		elt->set = TELT_UNSET;

		spinlock_unlock(&timer_lock);
		splx(s);
		return (TRUE);
	    }
	}
	spinlock_unlock(&timer_lock);
	splx(s);
	return (FALSE);
}

/*
 * Setup routine for timeout servicing thread. The timeout thread
 * is used to perform softclock() servicing.
 */
void
timeout_thread(void)
{
	thread_t			thread;
	processor_set_t			pset;
        kern_return_t                   ret;
        policy_base_t                   base;
        policy_limit_t                  limit;
        struct policy_fifo_base         fifo_base;
        struct policy_fifo_limit        fifo_limit;

        /*
         * Set thread privileges.
         */
	thread = current_thread();
        thread->vm_privilege = TRUE;
        stack_privilege(thread);

	/*
	 * Set thread priority and scheduling policy.
	 */
	pset = thread->processor_set;
        base = (policy_base_t) &fifo_base;
        limit = (policy_limit_t) &fifo_limit;
        fifo_base.base_priority = BASEPRI_KERNEL+1;
        fifo_limit.max_priority = BASEPRI_KERNEL+1;
        ret = thread_set_policy(thread->top_act, pset, POLICY_FIFO, 
				base, POLICY_FIFO_BASE_COUNT,
				limit, POLICY_FIFO_LIMIT_COUNT);
        if (ret != KERN_SUCCESS)
                printf("WARNING: softclock_thread is being TIMESHARED!\n");

	/*
	 * Run softclock() routine.
	 */
	softclock();
        /*NOTREACHED*/
}
