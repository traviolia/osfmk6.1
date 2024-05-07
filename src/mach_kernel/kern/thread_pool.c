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
 * 
 */
/*
 * OSF Research Institute MK6.1 (unencumbered) 1/31/1995
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
 *	File:	thread_pool.c
 *
 *	thread_pool management routines
 *
 */
#include <kern/ipc_mig.h>
#include <kern/ipc_tt.h>
#include <kern/mach_param.h>
#include <kern/machine.h>
#include <kern/misc_protos.h>
#include <kern/processor.h>
#include <kern/queue.h>
#include <kern/sched.h>
#include <kern/sched_prim.h>
#include <kern/task.h>
#include <kern/thread_act.h>

#include <mach/kern_return.h>
#include <kern/thread_pool.h>
#include <kern/thread.h>
#include <kern/zalloc.h>
#include <kern/misc_protos.h>
#include <kern/sched_prim.h>


/* Initialize a new EMPTY thread_pool.  */
kern_return_t
thread_pool_init(thread_pool_t new_thread_pool)
{
	assert(new_thread_pool != THREAD_POOL_NULL);

	/* Start with one reference for the caller */
	new_thread_pool->thr_acts = (struct thread_activation *)0;
	return KERN_SUCCESS;
}


/*
 * Obtain an activation from a thread pool, blocking if
 * necessary.  Return the activation locked, since it's
 * in an inconsistent state (not in a pool, not attached
 * to a thread).
 *
 * Called with ip_lock() held for pool_port.  Returns
 * the same way.
 *
 * If the thread pool port is destroyed while we are blocked,
 * then return a null activation. Callers must check for this
 * error case.
 */
thread_act_t
thread_pool_get_act(ipc_port_t pool_port)
{
	thread_pool_t thread_pool = &pool_port->ip_thread_pool;
	thread_act_t thr_act;

#if	MACH_ASSERT
	assert(thread_pool != THREAD_POOL_NULL);
	if (watchacts & WA_ACT_LNK)
		printf("thread_pool_block: %x, waiting=%d\n",
		       thread_pool, thread_pool->waiting);
#endif

	while ((thr_act = thread_pool->thr_acts) == THR_ACT_NULL) {
		if (!ip_active(pool_port))
			return THR_ACT_NULL;
		thread_pool->waiting = 1;
		assert_wait((int)thread_pool, FALSE);
		ip_unlock(pool_port);
		thread_block((void (*)(void)) 0);       /* block self */
		ip_lock(pool_port);
	}
	assert(thr_act->thread == THREAD_NULL);
	assert(thr_act->suspend_count == 0);
	thread_pool->thr_acts = thr_act->thread_pool_next;
	act_lock(thr_act);

#if	MACH_ASSERT
	if (watchacts & WA_ACT_LNK)
		printf("thread_pool_block: return %x, next=%x\n",
		       thr_act, thread_pool->thr_acts);
#endif
	return thr_act;
}

/*
 * Called with ip_lock() held for port containing thread_pool.
 * Returns same way.
 */
void
thread_pool_wakeup(thread_pool_t thread_pool)
{
#if	MACH_ASSERT
	assert(thread_pool != THREAD_POOL_NULL);
	if (watchacts & WA_ACT_LNK)
		printf("thread_pool_wakeup: %x, waiting=%d, head=%x\n",
		   thread_pool, thread_pool->waiting, thread_pool->thr_acts);
#endif	/* MACH_ASSERT */

	if (thread_pool->waiting) {
		thread_wakeup((int)thread_pool);
		thread_pool->waiting = 0;
	}
}
