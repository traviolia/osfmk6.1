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
 *      Joseph S. Barrera III
 *      Functions to drive lazy evaluation of norma operations.
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1992 Carnegie Mellon University
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

#include <kern/lock.h>
#include <kern/task.h>
#include <kern/thread.h>
#include <kern/time_out.h>
#include <norma/ipc_node.h>
#include <norma/xmm_obj.h>
#include <machine/mach_param.h>	/* HZ */

#define	NORMA_LAZY_MSEC		1000

boolean_t			norma_lazy_thread_awake = FALSE;
boolean_t			norma_lazy_thread_reawaken = FALSE;
spinlock_t			norma_lazy_thread_lock;

int norma_lazy_ticks = 		(NORMA_LAZY_MSEC * HZ) / 1000;
struct timer_elt		norma_lazy_timer_elt;

/*
 * Forward.
 */
void		norma_lazy_thread(void);

void		norma_lazy_timeout(void);

void
norma_lazy_thread(void)
{
	for (;;) {
		/*
		 * Record that we are awake.
		 * Look out for new awaken requests while we are out working.
		 */
		spinlock_lock(&norma_lazy_thread_lock);
		norma_lazy_thread_awake = TRUE;
		norma_lazy_thread_reawaken = FALSE;
		spinlock_unlock(&norma_lazy_thread_lock);

		/*
		 * Call out to the various modules.
		 */
		norma_transit_yield_threadcall();
		xmm_server_caching_threadcall();

		/*
		 * Go to sleep again.
		 */
		spinlock_lock(&norma_lazy_thread_lock);
		norma_lazy_thread_awake = FALSE;
		assert_wait((int) &norma_lazy_thread_awake, FALSE);
		spinlock_unlock(&norma_lazy_thread_lock);
		thread_block((void (*)(void)) 0);
	}
}

void
norma_lazy_timeout(void)
{
	boolean_t wakeup_requested;

	/*
	 * Call everyone's timeout routine, and remember whether
	 * anyone requests a thread wakeup.
	 */
	wakeup_requested = norma_transit_yield_timeout();
	wakeup_requested |= xmm_server_caching_timeout();

	/*
	 * If a thread wakeup was requested, then do it.
	 */
	if (wakeup_requested) {
		spinlock_lock(&norma_lazy_thread_lock);
		if (norma_lazy_thread_awake) {
			norma_lazy_thread_reawaken = TRUE;
			spinlock_unlock(&norma_lazy_thread_lock);
		} else {
			spinlock_unlock(&norma_lazy_thread_lock);
			thread_wakeup((int) &norma_lazy_thread_awake);
		}
	}

	/*
	 * Rearm timer.
	 */
	set_timeout(&norma_lazy_timer_elt, norma_lazy_ticks);
}

void
norma_lazy_init(void)
{
	spinlock_init(&norma_lazy_thread_lock);
	norma_lazy_thread_awake = TRUE;
	(void) kernel_thread(kernel_task, norma_lazy_thread, (char *) 0);

	set_timeout_setup(&norma_lazy_timer_elt,
			  (timeout_fcn_t)norma_lazy_timeout,
			  0, norma_lazy_ticks, PROCESSOR_NULL);
}
