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
 * Revision 2.1.1.1.1.1  92/03/03  16:15:22  jeffreyh
 * 	Pick up from TRUNK
 * 	[92/02/26  11:09:38  jeffreyh]
 * 
 * Revision 2.3  92/02/19  15:08:04  elf
 * 	Added fwait()
 * 	[92/01/19            jvh]
 * 
 * Revision 2.2  92/01/03  20:05:49  dbg
 * 	Created.
 * 	[91/12/23  16:32:15  dbg]
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

#ifndef	_I386_FPU_H_
#define	_I386_FPU_H_

/*
 * Macro definitions for routines to manipulate the
 * floating-point processor.
 */

#include <cpus.h>
#include <fpe.h>
#include <i386/proc_reg.h>
#include <i386/thread.h>
#include <kern/kern_types.h>
#include <mach/i386/kern_return.h>
#include <mach/i386/thread_status.h>

/*
 * FPU instructions.
 */
#define	fninit() \
	__asm__ volatile("fninit")

#define	fnstcw(control) \
	__asm__("fnstcw %0" : "=m" (*(unsigned short *)(control)))

#define	fldcw(control) \
	__asm__ volatile("fldcw %0" : : "m" (*(unsigned short *) &(control)) )

extern unsigned short		fnstsw(void);

extern __inline__ unsigned short fnstsw(void)
{
	unsigned short status;
	__asm__ volatile("fnstsw %0" : "=ma" (status));
	return(status);
}

#define	fnclex() \
	__asm__ volatile("fnclex")

#define	fnsave(state) \
	__asm__ volatile("fnsave %0" : "=m" (*state))

#define	frstor(state) \
	__asm__ volatile("frstor %0" : : "m" (state))

#define fwait() \
    	__asm__("fwait");

/*
 * If floating-point instructions are emulated,
 * we must load the floating-point register selector
 * when switching to a new thread.
 */
#if	FPE
extern void	disable_fpe();
extern void	enable_fpe();

#define	fpu_save_context(thread) \
    { \
	if (fp_kind == FP_SOFT) \
	    disable_fpe(); \
	else \
	    set_ts(); \
    }

#define	fpu_load_context(pcb) \
    { \
	register struct i386_fpsave_state *ifps; \
	if (fp_kind == FP_SOFT && (ifps = pcb->ims.ifps) != 0) \
	    enable_fpe(ifps); \
    }

#else	/* no FPE */

#define	fpu_load_context(pcb)

/*
 * Save thread`s FPU context.
 * If only one CPU, we just set the task-switched bit,
 * to keep the new thread from using the coprocessor.
 * If multiple CPUs, we save the entire state.
 */
#if	NCPUS > 1
#define	fpu_save_context(thread) \
    { \
	register struct i386_fpsave_state *ifps; \
	ifps = (thread)->top_act->mact.pcb->ims.ifps; \
	if (ifps != 0 && !ifps->fp_valid) { \
	    /* registers are in FPU - save to memory */ \
	    ifps->fp_valid = TRUE; \
	    fnsave(&ifps->fp_save_state); \
	} \
	set_ts(); \
    }
	    
#else	/* NCPUS == 1 */
#define	fpu_save_context(thread) \
    { \
	    set_ts(); \
    }

#endif	/* NCPUS == 1 */

#endif	/* no FPE */

extern int	fp_kind;

extern void		init_fpu(void);
extern void		fpu_module_init(void);
extern void		fp_free(
				struct i386_fpsave_state	* fps);
extern kern_return_t	fpu_set_state(
				thread_act_t			thr_act,
				struct i386_float_state		* st);
extern kern_return_t	fpu_get_state(
				thread_act_t			thr_act,
				struct i386_float_state		* st);
extern void		fpnoextflt(void);
extern void		fpextovrflt(void);
extern void		fpexterrflt(void);
extern void		fp_state_alloc(void);
extern void		fpintr(void);
extern void		fpflush(thread_act_t);

#endif	/* _I386_FPU_H_ */
