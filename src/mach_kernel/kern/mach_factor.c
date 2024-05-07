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
 * Revision 2.5  91/05/14  16:44:18  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:27:51  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:14:56  mrt]
 * 
 * Revision 2.3  90/06/02  14:55:09  rpd
 * 	Removed host_load; host_info/HOST_LOAD_INFO supercedes it.
 * 	[90/06/02            rpd]
 * 
 * 	Updated to new processor set and scheduling technology.
 * 	[90/03/26  22:11:20  rpd]
 * 
 * Revision 2.2  90/03/14  21:10:38  rwd
 * 	Added host_load call to get avenrun.
 * 	[90/01/28            rwd]
 * 
 * Revision 2.1  89/08/03  15:46:37  rwd
 * Created.
 * 
 * 20-Oct-88  David Golub (dbg) at Carnegie-Mellon University
 *	Fixed for MACH_KERNEL.
 *
 * 25-Mar-88  David Black (dlb) at Carnegie-Mellon University
 *	Added sched_load calculation.
 *
 *  4-Dec-87  David Black (dlb) at Carnegie-Mellon University
 *	Fix calculation to correctly account for threads that are
 *	actually on cpus.  This used to work by accident because if a
 *	processor is not idle, its idle thread was on the local runq;
 *	this is no longer the case.
 *
 * 18-Nov-87  Avadis Tevanian (avie) at Carnegie-Mellon University
 *	Removed conditionals, compute every second.
 *
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
 *	File:	kern/mach_factor.c
 *	Author:	Avadis Tevanian, Jr.
 *	Date:	1986
 *
 *	Compute the Mach Factor.
 */

#include <cpus.h>

#include <mach/machine.h>
#include <mach/processor_info.h>
#include <kern/sched.h>
#include <kern/assert.h>
#include <kern/processor.h>
#include <kern/thread.h>
#include <kern/time_out.h>
#if	MACH_KERNEL
#include <mach/kern_return.h>
#include <mach/port.h>
#endif	/* MACH_KERNEL */

long	avenrun[3] = {0, 0, 0};
long	mach_factor[3] = {0, 0, 0};

/*
 * Values are scaled by LOAD_SCALE, defined in processor_info.h
 */
static	long	fract[3] = {
	800,			/* (4.0/5.0) 5 second average */
	966,			/* (29.0/30.0) 30 second average */
	983,			/* (59.0/60.) 1 minute average */
};

void
compute_mach_factor(void)
{
	register processor_set_t	pset;
	register thread_t		thread;
#if	NCPUS > 1
	register processor_t		processor;
#endif	/*NCPUS > 1*/
	register int		ncpus;
	register int		nthreads;
	register long		factor_now;
	register long		average_now;
	register long		load_now;

	mutex_lock(&all_psets_lock);
	pset = (processor_set_t) queue_first(&all_psets);
	while (!queue_end(&all_psets, (queue_entry_t)pset)) {

	    /*
	     *	If no processors, this pset is in suspended animation.
	     *	No load calculations are performed.
	     */
	    pset_lock(pset);
	    if((ncpus = pset->processor_count) > 0) {

		/*
		 *	Count number of threads.
		 */
		nthreads = pset->runq.count;

		/* 	But, don't count depressed priority threads --
		 *	they are like the idle thread.
		 */
		thread = (thread_t) queue_first(&(pset->runq.runq[DEPRESSPRI]));
		while (!queue_end(&(pset->runq.runq[DEPRESSPRI]),
		    (queue_entry_t)thread)) {
			if (thread->sched_pri == DEPRESSPRI) {
			    assert(nthreads > 0);
			    nthreads--;
			}
			thread = (thread_t) queue_next(&thread->links);
		}

		/*
		 * account for threads on cpus.
		 */
		nthreads += ncpus - pset->idle_count; 

#if	NCPUS > 1
		processor = (processor_t) queue_first(&pset->processors);
		while (!queue_end(&pset->processors,
		    (queue_entry_t)processor)) {

			/* Count all threads on runq */
			nthreads += processor->runq.count;

			/* Exclude "idle" depresspri threads on runq */
			thread = (thread_t)
			    queue_first(&(processor->runq.runq[DEPRESSPRI]));
			while (!queue_end(&(processor->runq.runq[DEPRESSPRI]),
						    (queue_entry_t)thread)) {
			    if (thread->sched_pri == DEPRESSPRI) {
				assert(nthreads > 0);
				nthreads--;
			    }
			    thread = (thread_t) queue_next(&thread->links);
			}

			/* Exclude "idle" depresspri active threads */
			thread = active_threads[ processor->slot_num ];
			if (thread && thread->sched_pri == DEPRESSPRI) {
			    assert(nthreads > 0);
			    nthreads--;
			}

			processor =
			    (processor_t) queue_next(&processor->processors);
		}
#endif	/*NCPUS > 1*/

		/*
		 *	The current thread (running this calculation)
		 *	doesn't count; it's always in the default pset.
		 */
		if (pset == &default_pset)
		   nthreads -= 1;
		assert(nthreads >= 0);

		if (nthreads > ncpus) {
			factor_now = (ncpus * LOAD_SCALE) / (nthreads + 1);
			load_now = (nthreads << SCHED_SHIFT) / ncpus;
		}
		else {
			factor_now = (ncpus - nthreads) * LOAD_SCALE;
			load_now = SCHED_SCALE;
		}

		/*
		 *	Load average and mach factor calculations for
		 *	those that ask about these things.
		 */

		average_now = nthreads * LOAD_SCALE;

		pset->mach_factor =
			((pset->mach_factor << 2) + factor_now)/5;
		pset->load_average =
			((pset->load_average << 2) + average_now)/5;

		/*
		 *	And some ugly stuff to keep w happy.
		 */
		if (pset == &default_pset) {
		    register int i;

		    for (i = 0; i < 3; i++) {
			mach_factor[i] = ( (mach_factor[i]*fract[i])
				 + (factor_now*(LOAD_SCALE-fract[i])) )
				/ LOAD_SCALE;
			avenrun[i] = ( (avenrun[i]*fract[i])
				 + (average_now*(LOAD_SCALE-fract[i])) )
				/ LOAD_SCALE;
		    }
		}

		/*
		 *	sched_load is the only thing used by scheduler.
		 *	It is always at least 1 (i.e. SCHED_SCALE).
		 */
		pset->sched_load = (pset->sched_load + load_now) >> 1;
	    } /* end if ncpus > 0 */

	    pset_unlock(pset);
	    pset = (processor_set_t) queue_next(&pset->all_psets);
	} /* end foreach pset */

	mutex_unlock(&all_psets_lock);
}
