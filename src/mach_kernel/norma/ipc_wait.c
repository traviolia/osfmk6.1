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
 * Revision 2.7.2.5  92/05/27  00:53:37  jeffreyh
 * 	Caller must now acquire and release netipc_thread_lock().
 * 		Bail out if netipc ast is pending.
 * 	[92/05/12            dlb]
 * 
 * Revision 2.7.2.4.1.1  92/05/06  17:42:36  jeffreyh
 * 	Add assertion to norma_ipc_kmsg_accept.
 * 	[92/05/05            dlb]
 * 
 * Revision 2.7.2.4  92/04/08  15:46:34  jeffreyh
 * 	Disabled norma_kmsg_disable.
 * 	[92/04/08            andyp]
 * 
 * Revision 2.7.2.3  92/02/18  19:17:09  jeffreyh
 * 	[intel] optionally execute optimized norma_ipc_kmsg_accept().
 * 	[92/02/13  13:06:38  jeffreyh]
 * 
 * Revision 2.7.2.2  92/01/21  21:53:09  jsb
 * 	De-linted.
 * 	[92/01/17  12:21:06  jsb]
 * 
 * Revision 2.7.2.1  92/01/09  18:46:05  jsb
 * 	Use netipc_thread_{lock,unlock} instead of spls.
 * 	[92/01/08  10:23:36  jsb]
 * 
 * Revision 2.7  91/12/14  14:35:32  jsb
 * 	Removed private assert definition.
 * 
 * Revision 2.6  91/09/04  11:28:45  jsb
 * 	Use splhigh/splx instead of sploff/splon for now.
 * 	[91/09/04  09:46:45  jsb]
 * 
 * Revision 2.5  91/08/28  11:16:12  jsb
 * 	Renamed ast_clport things to ast_netipc things.
 * 	[91/08/15  09:12:09  jsb]
 * 
 * 	Fixed, and added counters.
 * 	[91/08/14  19:22:35  jsb]
 * 
 * Revision 2.4  91/08/03  18:19:32  jsb
 * 	Fixed include.
 * 	[91/07/17  14:06:39  jsb]
 * 
 * Revision 2.3  91/06/17  15:48:02  jsb
 * 	Changed norma include.
 * 	[91/06/17  11:01:02  jsb]
 * 
 * Revision 2.2  91/06/06  17:56:02  jsb
 * 	First checkin.
 * 	[91/06/06  17:51:41  jsb]
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
 *	File:	norma/ipc_wait.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 */

#include <mach_host.h>

#include <mach/port.h>
#include <mach/message.h>
#include <kern/assert.h>
#include <kern/counters.h>
#include <kern/lock.h>
#include <kern/sched_prim.h>
#include <kern/ipc_sched.h>
#include <kern/ipc_kobject.h>
#include <kern/thread.h>
#include <ipc/ipc_mqueue.h>
#include <ipc/ipc_thread.h>
#include <ipc/ipc_kmsg.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_pset.h>
#include <ipc/ipc_space.h>
#include <norma/ipc_wait.h>

/*
 * XXX Needs locking to be multiprocessor safe.
 * XXX We probably might also want per-processor spinning,
 * XXX although this will complicate the sending code.
 *
 * We signal that we are waiting by setting handoff_mqueue nonzero.
 * Our sender specifies that something has changed by setting msg nonzero.
 * We then signal that we are releasing this module by setting msg zero.
 *
 * YYY We've added locking so some of the comments above are out of data.
 *
 * XXX Should try having this loop handle asts?
 */
ipc_mqueue_t	norma_ipc_handoff_mqueue;
ipc_kmsg_t	norma_ipc_handoff_msg;
mach_msg_size_t	norma_ipc_handoff_max_size;
mach_msg_size_t	norma_ipc_handoff_msg_size;
spinlock_t	norma_ipc_handoff_lock;

int c_break_reset = 0;
int c_break_handoff = 0;
int c_break_thread = 0;
int c_break_gcount = 0;
#if NCPUS > 1
int c_break_lcount = 0;
int c_break_wcount = 0;
#endif /* NCPUS > 1 */
int c_break_ast = 0;
int c_break_ast_terminate = 0;
int c_break_ast_halt = 0;
int c_break_ast_block = 0;
int c_break_ast_network = 0;
int c_break_ast_netipc = 0;

int	norma_ipc_kmsg_accept_disabled = 0;

/*
 * Spin until something else is runnable or until a kmsg shows up.
 *
 * Called holding norma_ipc_handoff_lock.  Will release it while waiting
 * and reacquire before returning.
 */
ipc_kmsg_t
norma_ipc_kmsg_accept(
	volatile ipc_mqueue_t	mqueue,
	mach_msg_size_t		max_size,
	mach_msg_size_t		*msg_size)
{
	register processor_t myprocessor;
	register volatile thread_t *threadp;
	register volatile int *gcount;
#if	NCPUS > 1
	register volatile int *lcount;
	register volatile boolean_t *wake_active;
#endif	/* NCPUS > 1 */
	int mycpu;

/*	assert(simple_lock_held(&norma_ipc_handoff_lock)); */
	if (norma_ipc_kmsg_accept_disabled) {
		return IKM_NULL;
	}

#if	1
	if (c_break_reset) {
		c_break_reset = 0;
		c_break_handoff = 0;
		c_break_thread = 0;
		c_break_gcount = 0;
#if	NCPUS > 1
		c_break_lcount = 0;
		c_break_wcount = 0;
#endif	/* NCPUS > 1 */
		c_break_ast = 0;
		c_break_ast_halt = 0;
		c_break_ast_terminate = 0;
		c_break_ast_block = 0;
		c_break_ast_network = 0;
		c_break_ast_netipc = 0;
	}
#endif
	/*
	 * Indicate that we are spinning on this queue.
	 *
	 * Nonzero norma_ipc_handoff_mqueue keeps other receivers away.
	 * Nonzero norma_ipc_handoff_msg keeps other senders away.
	 */
	if (norma_ipc_handoff_mqueue != IMQ_NULL) {
		return IKM_NULL;
	}

	mycpu = cpu_number();
	myprocessor = current_processor();
	threadp = (volatile thread_t *) &myprocessor->next_thread;
#if	MACH_HOST
	gcount = (volatile int *) &myprocessor->processor_set->runq.count;
#else	/* MACH_HOST */
	gcount = (volatile int *) &default_pset.runq.count;
#endif	/* MACH_HOST */

#if	NCPUS > 1
	lcount = (volatile int *) &myprocessor->runq.count;
	wake_active = &current_thread()->wake_active;
#endif	/* NCPUS > 1 */

	/*
	 * Don't mark cpu idle; we still like our pmap.  Use VIDLE instead,
	 * which is same as RUNNING, except for CPU statistics.
	 *
	 * XXX Will myprocessor->next_thread ever get set?
	 */
	assert(myprocessor->state == PROCESSOR_RUNNING);
	myprocessor->state = PROCESSOR_VIDLE;

#if	PARAGON860
	paragon_mark_cpu_idle(cpu_number());
#endif	/* PARAGON860 */

	/*
	 * While we were on our way in, a kmsg could have been queued for
	 * this port.  Bail out in this case.
	 * XXX AST_NETIPC check is overly general.
	 */
	if (need_ast[mycpu] & AST_NETIPC) {
	        c_break_ast_netipc++;
	}

	assert(norma_ipc_handoff_msg == 0);
	norma_ipc_handoff_max_size = max_size;
	norma_ipc_handoff_msg_size = 0;
	norma_ipc_handoff_mqueue = mqueue;
	assert(ipc_kmsg_queue_first(&mqueue->imq_messages) == IKM_NULL);

	spinlock_unlock(&norma_ipc_handoff_lock);
	imq_unlock(mqueue);

	/*
	 * Spin until reschedule or kmsg handoff. Do asts in the meantime.
	 */

	for (;;) {

		if (norma_ipc_handoff_msg != IKM_NULL) {
			c_break_handoff++;
			break;
		}
		if (need_ast[mycpu]) {
			if (need_ast[mycpu] & AST_HALT) {
				c_break_ast_halt++;
			}
			if (need_ast[mycpu] & AST_TERMINATE) {
				c_break_ast_terminate++;
			}
			if (need_ast[mycpu] & AST_BLOCK) {
				c_break_ast_block++;
			}
			if (need_ast[mycpu] & AST_NETWORK) {
				c_break_ast_network++;
			}
			if (need_ast[mycpu] & AST_NETIPC) {
				c_break_ast_netipc++;
			}
			c_break_ast++;
			break;
		}
		if (*threadp != (volatile thread_t) THREAD_NULL) {
			c_break_thread++;
			break;
		}
		if (*gcount != 0) {
			c_break_gcount++;
			break;
		}
#if	NCPUS > 1
		if (*lcount != 0) {
			c_break_lcount++;
			break;
		}
		if (*wake_active) {
			c_break_wcount++;
			break;
		}
#endif	/* NCPUS > 1 */
	}

	/* We're not idle anymore: */
	myprocessor->state = PROCESSOR_RUNNING;

#if	PARAGON860
	paragon_mark_cpu_active(cpu_number());
#endif	/* PARAGON860 */

	/*
	 * Before we release mqueue, we must check for a delivered message.
	 */
	imq_lock(mqueue);
	spinlock_lock(&norma_ipc_handoff_lock);
	norma_ipc_handoff_mqueue = IMQ_NULL;
	if (norma_ipc_handoff_msg != IKM_NULL) {
		/*
		 * Someone left us a message,
		 * or an indication of a message that was too large.
		 */
		if (norma_ipc_handoff_msg_size) {
			*msg_size = norma_ipc_handoff_msg_size;
			norma_ipc_handoff_msg = IKM_NULL;
			return IKM_NULL;
		} else {
			register ipc_kmsg_t kmsg;
			kmsg = norma_ipc_handoff_msg;
			norma_ipc_handoff_msg = IKM_NULL;
			return kmsg;
		}
	}
	assert(ipc_kmsg_queue_first(&mqueue->imq_messages) == IKM_NULL);
	return IKM_NULL;
}
