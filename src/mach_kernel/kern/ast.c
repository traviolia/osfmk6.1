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
 * Revision 2.10  91/08/28  11:14:16  jsb
 * 	Renamed AST_CLPORT to AST_NETIPC.
 * 	[91/08/14  21:39:25  jsb]
 * 
 * Revision 2.9  91/06/17  15:46:48  jsb
 * 	Renamed NORMA conditionals.
 * 	[91/06/17  10:48:46  jsb]
 * 
 * Revision 2.8  91/06/06  17:06:43  jsb
 * 	Added AST_CLPORT.
 * 	[91/05/13  17:34:31  jsb]
 * 
 * Revision 2.7  91/05/14  16:39:48  mrt
 * 	Correcting copyright
 * 
 * Revision 2.6  91/05/08  12:47:06  dbg
 * 	Add volatile declarations where needed.
 * 	[91/04/18            dbg]
 * 
 * 	Add missing argument to ast_on in assign/shutdown case.
 * 	[91/03/21            dbg]
 * 
 * Revision 2.5  91/03/16  14:49:23  rpd
 * 	Cleanup.
 * 	[91/02/13            rpd]
 * 	Changed the AST interface.
 * 	[91/01/17            rpd]
 * 
 * Revision 2.4  91/02/05  17:25:33  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:11:01  mrt]
 * 
 * Revision 2.3  90/06/02  14:53:30  rpd
 * 	Updated with new processor/processor-set technology.
 * 	[90/03/26  22:02:00  rpd]
 * 
 * Revision 2.2  90/02/22  20:02:37  dbg
 * 	Remove lint.
 * 	[90/01/29            dbg]
 * 
 * Revision 2.1  89/08/03  15:42:10  rwd
 * Created.
 * 
 *  2-Feb-89  David Golub (dbg) at Carnegie-Mellon University
 *	Moved swtch to this file.
 *
 * 23-Nov-88  David Black (dlb) at Carnegie-Mellon University
 *	Hack up swtch() again.  Drop priority just low enough to run
 *	something else if it's runnable.  Do missing priority updates.
 *	Make sure to lock thread and double check whether update is needed.
 *	Yet more cruft until I can get around to doing it right.
 *
 *  6-Sep-88  David Golub (dbg) at Carnegie-Mellon University
 *	Removed all non-MACH code.
 *
 * 11-Aug-88  David Black (dlb) at Carnegie-Mellon University
 *	csw_check is now the csw_needed macro in sched.h.  Rewrite
 *	ast_check for new ast mechanism.
 *
 *  9-Aug-88  David Black (dlb) at Carnegie-Mellon University
 *	Rewrote swtch to check runq counts directly.
 *
 *  9-Aug-88  David Black (dlb) at Carnegie-Mellon University
 *	Delete runrun.  Rewrite csw_check so it can become a macro.
 *
 *  4-May-88  David Black (dlb) at Carnegie-Mellon University
 *	Moved cpu not running check to ast_check().
 *	New preempt priority logic.
 *	Increment runrun if ast is for context switch.
 *	Give absolute priority to local run queues.
 *
 * 20-Apr-88  David Black (dlb) at Carnegie-Mellon University
 *	New signal check logic.
 *
 * 18-Nov-87  Avadis Tevanian (avie) at Carnegie-Mellon University
 *	Flushed conditionals, reset history.
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
 *
 *	This file contains routines to check whether an ast is needed.
 *
 *	ast_check() - check whether ast is needed for interrupt or context
 *	switch.  Usually called by clock interrupt handler.
 *
 */

#include <cpus.h>
#include <norma_ipc.h>
#include <platforms.h>
#if	(iPSC860 || PARAGON860)
#include <mcmsg.h>
#endif	/* (iPSC860 || PARAGON860) */

#include <kern/ast.h>
#include <kern/counters.h>
#include <kern/cpu_number.h>
#include <kern/misc_protos.h>
#include <kern/queue.h>
#include <kern/sched.h>
#include <kern/sched_prim.h>
#include <kern/thread.h>
#include <kern/processor.h>
#include <kern/spl.h>
#include <mach/policy.h>
#include <device/net_io.h>

#if	NORMA_IPC
#include <norma/ipc_net.h>
#endif	/* NORMA_IPC */

volatile ast_t need_ast[NCPUS];

#if	(NCPUS > 1) && defined(PARAGON860)

spinlock_t ast_cpu_lock;

int	other_reasons;

#if	MCMSG && (iPSC860 || PARAGON860)
#define	MO_REASONS (AST_NETIPC | AST_NETWORK | AST_MCMSG)
#else
#define	MO_REASONS (AST_NETIPC | AST_NETWORK )
#endif

#endif	/* (NCPUS > 1) && defined(PARAGON860) */

void
ast_init(void)
{
#ifndef	MACHINE_AST
	register int i;

	for (i=0; i<NCPUS; i++) {
		need_ast[i] = AST_NONE;
	}
#endif	/* MACHINE_AST */
#if	(NCPUS > 1) && defined(PARAGON860)
	spinlock_init(&ast_cpu_lock);
	other_reasons = 0;
#endif	/* (NCPUS > 1) && defined(PARAGON860) */
}

void
ast_taken(
	boolean_t		preemption,
	ast_t			mask,
	spl_t			old_spl)
{
	register thread_t	self = current_thread();
	register processor_t	mypr = current_processor();
	register ast_t		reasons;
	register int		mycpu = cpu_number();

	/*
	 *	Interrupts are still disabled.
	 *	We must clear need_ast and then enable interrupts.
	 */

extern void	log_thread_action(thread_t, char *);

#if 0
	log_thread_action (current_thread(), "ast_taken");
#endif

#if	(NCPUS > 1) && defined(PARAGON860)

	/*
	 * Since norma is MP unsafe we CANNOT let CPU's other than the
	 * master process some types of AST's which eventually lead into
	 * unsafe NORMA code. Here we migrate the unsafe AST's back onto the
	 * master processor.
	 */
	spinlock_lock(&ast_cpu_lock);
	reasons = need_ast[mycpu] & mask;
	if ( mycpu != master_cpu ) {
		other_reasons |= (reasons & MO_REASONS); 
		reasons &= ~MO_REASONS;	/* clear one we don't want */
	}
	else {
		reasons |= other_reasons;
		other_reasons = 0;
	}
	need_ast[mycpu] &= ~reasons;
	spinlock_unlock(&ast_cpu_lock);

#else	/* (NCPUS > 1) && defined(PARAGON860) */

	reasons = need_ast[mycpu] & mask;
	need_ast[mycpu] &= ~reasons;

#endif	/* (NCPUS > 1) && defined(PARAGON860) */

	splx(old_spl);

	/*
	 *	These actions must not block.
	 */

#if MCMSG && (iPSC860 || PARAGON860)
	if (reasons & AST_MCMSG)
		mcmsg_ast();
#endif	/* (NCPUS > 1) && defined(PARAGON860) */

	if (reasons & AST_NETWORK)
		net_ast();

#if	NORMA_IPC
	if (reasons & AST_NETIPC)
		netipc_ast();
#endif	/* NORMA_IPC */

	/*
	 *	Make darn sure that we don't call thread_halt_self
	 *	or thread_block from the idle thread.
	 */

	/* XXX - this isn't currently right for the HALT case... */

	if (self == mypr->idle_thread) {
#if	NCPUS == 1
	    if (reasons & AST_URGENT) {
		if (!preemption)
		    panic("ast_taken: AST_URGENT for idle_thr w/o preemption");
	    }
#endif
	    return;
	}

	/* migration APC hook */
	if (reasons & AST_APC) {
		act_execute_returnhandlers();
		return;	/* auto-retry will catch anything new */
	}

	/* 
	 *	thread_block needs to know if the thread's quantum 
	 *	expired so the thread can be put on the tail of
	 *	run queue. One of the previous actions might well
	 *	have woken a high-priority thread, so we also use
	 *	csw_needed check.
	 */
	{   void (*safept)(void) = (void (*)(void))SAFE_EXCEPTION_RETURN;

	    if (reasons &= (AST_QUANTUM | AST_BLOCK | AST_URGENT)) {
		    if (preemption)
			    safept = (void (*)(void)) 0;
	    } else if (csw_needed(self, mypr)) {
		    reasons = (mypr->first_quantum ? AST_BLOCK : AST_QUANTUM);
	    }
	    if (reasons) {
		    counter(c_ast_taken_block++);
		    thread_block_reason(safept, reasons);
	    }
	}
}

void
ast_check(void)
{
	register int		mycpu = cpu_number();
	register processor_t	myprocessor;
	register thread_t	thread = current_thread();
	spl_t			s = splsched();

	/*
	 *	Check processor state for ast conditions.
	 */
	myprocessor = cpu_to_processor(mycpu);
	switch(myprocessor->state) {
	    case PROCESSOR_OFF_LINE:
	    case PROCESSOR_IDLE:
	    case PROCESSOR_DISPATCHING:
		/*
		 *	No ast.
		 */
	    	break;

#if	NCPUS > 1
	    case PROCESSOR_ASSIGN:
	    case PROCESSOR_SHUTDOWN:
	        /*
		 * 	Need ast to force action thread onto processor.
		 *
		 * XXX  Should check if action thread is already there.
		 */
		ast_on(mycpu, AST_BLOCK);
		break;
#endif	/* NCPUS > 1 */

	    case PROCESSOR_RUNNING:
	    case PROCESSOR_VIDLE:

		/*
		 *	Propagate thread ast to processor.  If we already
		 *	need an ast, don't look for more reasons.
		 */
		ast_propagate(current_act(), mycpu);
		if (ast_needed(mycpu))
			break;

		/*
		 *	Context switch check.
		 */
		if (csw_needed(thread, myprocessor)) {
			ast_on(mycpu, (myprocessor->first_quantum ?
			       AST_BLOCK : AST_QUANTUM));
		}
		break;

	    default:
	        panic("ast_check: Bad processor state");
	}
	(void) splx(s);
}
