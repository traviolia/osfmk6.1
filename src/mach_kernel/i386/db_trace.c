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
 * Revision 2.10.2.1  92/02/21  11:23:17  jsb
 * 	Changed db_numargs to use patchable variable db_numargs_default
 * 	when it needs to guess the number of arguments, instead of just
 * 	using the embedded constant '5'.
 * 	[92/02/09  12:09:54  jsb]
 * 
 * Revision 2.10  91/11/12  11:50:39  rvb
 * 	Removed redundant printing of the continuation.
 * 	Fixed the log.
 * 	[91/11/07            rpd]
 * 
 * Revision 2.9  91/10/09  16:06:49  af
 * 	Supported stack trace and register access of non current thread.
 * 	Supported user register access.
 * 	Fixed incorrect next frame check.
 * 	[91/08/29            tak]
 * 
 * 	Fixed stack tracing for threads without kernel stacks.  Yes, again.
 * 	[91/09/26            rpd]
 * 
 * Revision 2.8  91/08/28  11:11:34  jsb
 * 	In stack traces, print file/line info if available.
 * 	[91/08/13  18:15:57  jsb]
 * 
 * Revision 2.7  91/05/14  16:06:12  mrt
 * 	Correcting copyright
 * 
 * Revision 2.6  91/02/05  17:11:21  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:31:32  mrt]
 * 
 * Revision 2.5  91/01/09  19:55:27  rpd
 * 	Fixed stack tracing for threads without kernel stacks.
 * 	[91/01/09            rpd]
 * 
 * Revision 2.4  91/01/08  15:10:22  rpd
 * 	Reorganized the pcb.
 * 	[90/12/11            rpd]
 * 
 * Revision 2.3  90/11/05  14:27:07  rpd
 * 	If we can not guess the number of args to a function, use 5 vs 0.
 * 	[90/11/02            rvb]
 * 
 * Revision 2.2  90/08/27  21:56:20  dbg
 * 	Import db_sym.h.
 * 	[90/08/21            dbg]
 * 	Fix includes.
 * 	[90/08/08            dbg]
 * 	Created from rvb's code for new debugger.
 * 	[90/07/11            dbg]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
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

#include <strings.h>

#include <mach/boolean.h>
#include <vm/vm_map.h>
#include <kern/thread.h>
#include <kern/task.h>

#include <machine/db_machdep.h>
#include <machine/setjmp.h>
#include <mach/machine.h>

#include <ddb/db_access.h>
#include <ddb/db_sym.h>
#include <ddb/db_variables.h>
#include <ddb/db_command.h>
#include <ddb/db_task_thread.h>
#include <ddb/db_output.h>

extern jmp_buf_t *db_recover;
extern struct i386_saved_state *saved_state[];

struct i386_kernel_state ddb_null_kregs;

/*
 * Stack trace.
 */
#define	INKERNEL(va)	(((vm_offset_t)(va)) >= VM_MIN_KERNEL_ADDRESS)
extern vm_offset_t vm_min_inks_addr;	/* set by db_clone_symtabXXX */
#define INKSERVER(va)	(((vm_offset_t)(va)) >= vm_min_inks_addr)

struct i386_frame {
	struct i386_frame	*f_frame;
	int			f_retaddr;
	int			f_arg0;
};

#define	TRAP		1
#define	INTERRUPT	2
#define SYSCALL		3

db_addr_t	db_user_trap_symbol_value = 0;
db_addr_t	db_kernel_trap_symbol_value = 0;
db_addr_t	db_interrupt_symbol_value = 0;
db_addr_t	db_return_to_iret_symbol_value = 0;
db_addr_t	db_syscall_symbol_value = 0;
boolean_t	db_trace_symbols_found = FALSE;

struct i386_kregs {
	char	*name;
	int	offset;
} i386_kregs[] = {
	{ "ebx", (int)(&((struct i386_kernel_state *)0)->k_ebx) },
	{ "esp", (int)(&((struct i386_kernel_state *)0)->k_esp) },
	{ "ebp", (int)(&((struct i386_kernel_state *)0)->k_ebp) },
	{ "edi", (int)(&((struct i386_kernel_state *)0)->k_edi) },
	{ "esi", (int)(&((struct i386_kernel_state *)0)->k_esi) },
	{ "eip", (int)(&((struct i386_kernel_state *)0)->k_eip) },
	{ 0 },
};

/* Forward */

extern int *	db_lookup_i386_kreg(
			char			*name,
			int			*kregp);
extern int	db_i386_reg_value(
			struct db_variable	* vp,
			db_expr_t		* val,
			int			flag,
			db_var_aux_param_t	ap);
extern void	db_find_trace_symbols(void);
extern int	db_numargs(
			struct i386_frame	*fp,
			task_t			task);
extern void	db_nextframe(
			struct i386_frame	**lfp,
			struct i386_frame	**fp,
			db_addr_t		*ip,
			int			frame_type,
			thread_act_t		thr_act);
extern int	_setjmp(
			jmp_buf_t		* jb);

/*
 * Machine register set.
 */
struct db_variable db_regs[] = {
	{ "cs",	(int *)&ddb_regs.cs,  db_i386_reg_value, 0, 0, 0, 0, TRUE },
	{ "ds",	(int *)&ddb_regs.ds,  db_i386_reg_value, 0, 0, 0, 0, TRUE },
	{ "es",	(int *)&ddb_regs.es,  db_i386_reg_value, 0, 0, 0, 0, TRUE },
	{ "fs",	(int *)&ddb_regs.fs,  db_i386_reg_value, 0, 0, 0, 0, TRUE },
	{ "gs",	(int *)&ddb_regs.gs,  db_i386_reg_value, 0, 0, 0, 0, TRUE },
	{ "ss",	(int *)&ddb_regs.ss,  db_i386_reg_value, 0, 0, 0, 0, TRUE },
	{ "eax",(int *)&ddb_regs.eax, db_i386_reg_value, 0, 0, 0, 0, TRUE },
	{ "ecx",(int *)&ddb_regs.ecx, db_i386_reg_value, 0, 0, 0, 0, TRUE },
	{ "edx",(int *)&ddb_regs.edx, db_i386_reg_value, 0, 0, 0, 0, TRUE },
	{ "ebx",(int *)&ddb_regs.ebx, db_i386_reg_value, 0, 0, 0, 0, TRUE },
	{ "esp",(int *)&ddb_regs.uesp,db_i386_reg_value, 0, 0, 0, 0, TRUE },
	{ "ebp",(int *)&ddb_regs.ebp, db_i386_reg_value, 0, 0, 0, 0, TRUE },
	{ "esi",(int *)&ddb_regs.esi, db_i386_reg_value, 0, 0, 0, 0, TRUE },
	{ "edi",(int *)&ddb_regs.edi, db_i386_reg_value, 0, 0, 0, 0, TRUE },
	{ "eip",(int *)&ddb_regs.eip, db_i386_reg_value, 0, 0, 0, 0, TRUE },
	{ "efl",(int *)&ddb_regs.efl, db_i386_reg_value, 0, 0, 0, 0, TRUE },
};
struct db_variable *db_eregs = db_regs + sizeof(db_regs)/sizeof(db_regs[0]);

int *
db_lookup_i386_kreg(
	char	*name,
	int	*kregp)
{
	register struct i386_kregs *kp;

	for (kp = i386_kregs; kp->name; kp++) {
	    if (strcmp(name, kp->name) == 0)
		return((int *)((int)kregp + kp->offset));
	}
	return(0);
}
	
int
db_i386_reg_value(
	struct	db_variable	*vp,
	db_expr_t		*valuep,
	int			flag,
	db_var_aux_param_t	ap)
{
	extern char		etext;
	int			*dp = 0;
	db_expr_t		null_reg = 0;
	register thread_act_t	thr_act = ap->thr_act;
	extern unsigned		int_stack_high;
	int			cpu;

	if (db_option(ap->modif, 'u')) {
	    if (thr_act == THR_ACT_NULL) {
		if ((thr_act = current_act()) == THR_ACT_NULL)
		    db_error("no user registers\n");
	    }
	    if (thr_act == current_act()) {
		if (IS_USER_TRAP(&ddb_regs, &etext))
		    dp = vp->valuep;
		else if (ddb_regs.ebp < int_stack_high)
		    db_error("cannot get/set user registers in nested interrupt\n");
	    }
	} else {
	    if (thr_act == THR_ACT_NULL || thr_act == current_act()) {
		dp = vp->valuep;
	    } else {
	      if (thr_act->thread && !(thr_act->thread->state & TH_SWAPPED) && 
			thr_act->kernel_stack) {
		int cpu;

		for (cpu = 0; cpu < NCPUS; cpu++) {
		    if (machine_slot[cpu].running == TRUE &&
			active_threads[cpu] == thr_act->thread && saved_state[cpu]) {
			dp = (int *) (((int)saved_state[cpu]) +
				      (((int) vp->valuep) -
				       (int) &ddb_regs));
			break;
		    }
		}
		if (dp == 0 && thr_act && thr_act->thread)
		    dp = db_lookup_i386_kreg(vp->name,
				 (int *)(STACK_IKS(thr_act->kernel_stack)));
		if (dp == 0)
		    dp = &null_reg;
	      } else if (thr_act->thread&&(thr_act->thread->state&TH_SWAPPED)){
		/* only EIP is valid */
		if (vp->valuep == (int *) &ddb_regs.eip) {
		    dp = (int *)(&thr_act->thread->swap_func);
		} else {
		    dp = &null_reg;
		}
	      }
	    }
	}
	if (dp == 0) {
	    int cpu;

	    if (!db_option(ap->modif, 'u')) {
		for (cpu = 0; cpu < NCPUS; cpu++) {
		    if (machine_slot[cpu].running == TRUE &&
		    	active_threads[cpu] == thr_act->thread && saved_state[cpu]) {
		    	    dp = (int *) (((int)saved_state[cpu]) +
					  (((int) vp->valuep) -
					   (int) &ddb_regs));
			    break;
		    }
		}
	    }
	    if (dp == 0) {
		if (!thr_act || thr_act->mact.pcb == 0)
		    db_error("no pcb\n");
		dp = (int *)((int)(&thr_act->mact.pcb->iss) + 
			     ((int)vp->valuep - (int)&ddb_regs));
	    }
	}
	if (flag == DB_VAR_SET)
	    *dp = *valuep;
	else
	    *valuep = *dp;
	return(0);
}

void
db_find_trace_symbols(void)
{
	db_expr_t	value;
	if (db_value_of_name("_user_trap", &value))
	    db_user_trap_symbol_value = (db_addr_t) value;
	if (db_value_of_name("_kernel_trap", &value))
	    db_kernel_trap_symbol_value = (db_addr_t) value;
	if (db_value_of_name("_interrupt", &value))
	    db_interrupt_symbol_value = (db_addr_t) value;
	if (db_value_of_name("_return_to_iret", &value))
	    db_return_to_iret_symbol_value = (db_addr_t) value;
	if (db_value_of_name("_syscall", &value))
	    db_syscall_symbol_value = (db_addr_t) value;
	db_trace_symbols_found = TRUE;
}

/*
 * Figure out how many arguments were passed into the frame at "fp".
 */
int db_numargs_default = 5;

int
db_numargs(
	struct i386_frame	*fp,
	task_t			task)
{
	int	*argp;
	int	inst;
	int	args;
	extern char	etext;

	argp = (int *)db_get_task_value((int)&fp->f_retaddr, 4, FALSE, task);
	if (argp < (int *)VM_MIN_KERNEL_ADDRESS || (char *)argp > &etext)
	    args = db_numargs_default;
	else if (!DB_CHECK_ACCESS((int)argp, 4, task))
	    args = db_numargs_default;
	else {
	    inst = db_get_task_value((int)argp, 4, FALSE, task);
	    if ((inst & 0xff) == 0x59)	/* popl %ecx */
		args = 1;
	    else if ((inst & 0xffff) == 0xc483)	/* addl %n, %esp */
		args = ((inst >> 16) & 0xff) / 4;
	    else
		args = db_numargs_default;
	}
	return (args);
}

struct interrupt_frame {
	struct i386_frame *if_frame;	/* point to next frame */
	int		  if_retaddr;	/* return address to _interrupt */
	int		  if_unit;	/* unit number */
	int		  if_spl;	/* saved spl */
	int		  if_iretaddr;	/* _return_to_{iret,iret_i} */
	int		  if_edx;	/* old sp(iret) or saved edx(iret_i) */
	int		  if_ecx;	/* saved ecx(iret_i) */
	int		  if_eax;	/* saved eax(iret_i) */
	int		  if_eip;	/* saved eip(iret_i) */
	int		  if_cs;	/* saved cs(iret_i) */
	int		  if_efl;	/* saved efl(iret_i) */
};

/* 
 * Figure out the next frame up in the call stack.  
 * For trap(), we print the address of the faulting instruction and 
 *   proceed with the calling frame.  We return the ip that faulted.
 *   If the trap was caused by jumping through a bogus pointer, then
 *   the next line in the backtrace will list some random function as 
 *   being called.  It should get the argument list correct, though.  
 *   It might be possible to dig out from the next frame up the name
 *   of the function that faulted, but that could get hairy.
 */
void
db_nextframe(
	struct i386_frame	**lfp,		/* in/out */
	struct i386_frame	**fp,		/* in/out */
	db_addr_t		*ip,		/* out */
	int			frame_type,	/* in */
	thread_act_t		thr_act)	/* in */
{
	extern char *	trap_type[];
	extern int	TRAP_TYPES;

	struct i386_saved_state *saved_regs;
	struct interrupt_frame *ifp;
	struct i386_interrupt_state *isp;
	task_t task = (thr_act != THR_ACT_NULL)? thr_act->task: TASK_NULL;

	switch(frame_type) {
	case TRAP:
	    /*
	     * We know that trap() has 1 argument and we know that
	     * it is an (strcut i386_saved_state *).
	     */
	    saved_regs = (struct i386_saved_state *)
			db_get_task_value((int)&((*fp)->f_arg0),4,FALSE,task);
	    if (saved_regs->trapno >= 0 && saved_regs->trapno < TRAP_TYPES) {
		db_printf(">>>>> %s trap at ",
			trap_type[saved_regs->trapno]);
	    } else {
		db_printf(">>>>> trap (number %d) at ",
			saved_regs->trapno & 0xffff);
	    }
	    db_task_printsym(saved_regs->eip, DB_STGY_PROC, task);
	    db_printf(" <<<<<\n");
	    *fp = (struct i386_frame *)saved_regs->ebp;
	    *ip = (db_addr_t)saved_regs->eip;
	    break;
	case INTERRUPT:
	    if (*lfp == 0) {
		db_printf(">>>>> interrupt <<<<<\n");
		goto miss_frame;
	    }
	    db_printf(">>>>> interrupt at "); 
	    ifp = (struct interrupt_frame *)(*lfp);
	    *fp = ifp->if_frame;
	    if (ifp->if_iretaddr == db_return_to_iret_symbol_value)
		*ip = ((struct i386_interrupt_state *) ifp->if_edx)->eip;
	    else
		*ip = (db_addr_t) ifp->if_eip;
	    db_task_printsym(*ip, DB_STGY_PROC, task);
	    db_printf(" <<<<<\n");
	    break;
	case SYSCALL:
	    if (thr_act != THR_ACT_NULL && thr_act->mact.pcb) {
		*ip = (db_addr_t) thr_act->mact.pcb->iss.eip;
		*fp = (struct i386_frame *) thr_act->mact.pcb->iss.ebp;
		break;
	    }
	    /* falling down for unknown case */
	default:
	miss_frame:
	    *ip = (db_addr_t)
		db_get_task_value((int)&(*fp)->f_retaddr, 4, FALSE, task);
	    *lfp = *fp;
	    *fp = (struct i386_frame *)
		db_get_task_value((int)&(*fp)->f_frame, 4, FALSE, task);
	    break;
	}
}

void
db_stack_trace_cmd(
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char		*modif)
{
	struct i386_frame *frame, *lastframe;
	int		*argp;
	db_addr_t	callpc;
	int		frame_type;
	boolean_t	kernel_only = TRUE;
	boolean_t	trace_thread = FALSE;
	boolean_t	trace_all_threads = FALSE;
	int		thcount = 0;
	char		*filename;
	int		linenum;
	task_t		task;
	thread_act_t	th;
	int		user_frame;
	int		frame_count;
	jmp_buf_t	*prev;
	jmp_buf_t	db_jmp_buf;
	queue_entry_t	act_list;
	extern unsigned	db_maxoff;

	if (!db_trace_symbols_found)
	    db_find_trace_symbols();

	{
	    register char *cp = modif;
	    register char c;

	    while ((c = *cp++) != 0) {
		if (c == 't')
		    trace_thread = TRUE;
		if (c == 'T') {
		    trace_all_threads = TRUE;
		    trace_thread = TRUE;
		}
		if (c == 'u')
		    kernel_only = FALSE;
	    }
	}

	if (trace_all_threads) {
	    if (!have_addr && !trace_thread) {
		have_addr = TRUE;
		trace_thread = TRUE;
		act_list = &(current_task()->thr_acts);
		addr = (db_expr_t) queue_first(act_list);
	    } else if (trace_thread) {
		if (have_addr) {
		    if (!db_check_act_address_valid((thread_act_t)addr)) {
			if (db_lookup_task((task_t)addr) == -1)
			    return;
			act_list = &(((task_t)addr)->thr_acts);
			addr = (db_expr_t) queue_first(act_list);
		    } else {
			act_list = &(((thread_act_t)addr)->task->thr_acts);
			thcount = db_lookup_task_act(((thread_act_t)addr)->task,
							(thread_act_t)addr);
		    }
		} else {
		    th = db_default_act;
		    if (th == THR_ACT_NULL)
			th = current_act();
		    if (th == THR_ACT_NULL) {
			db_printf("no active thr_act\n");
			return;
		    }
		    have_addr = TRUE;
		    act_list = &th->task->thr_acts;
		    addr = (db_expr_t) queue_first(act_list);
		}
	    }
	}

	if (count == -1)
	    count = 65535;

    next_thread:
	if (trace_all_threads)
	    db_printf("---------- Thread #%d (0x%x) ----------\n",
		      thcount, addr);
	user_frame = 0;
	frame_count = count;

	if (!have_addr && !trace_thread) {
	    frame = (struct i386_frame *)ddb_regs.ebp;
	    callpc = (db_addr_t)ddb_regs.eip;
	    th = current_act();
	    task = (th != THR_ACT_NULL)? th->task: TASK_NULL;
	} else if (trace_thread) {
	    if (have_addr) {
		th = (thread_act_t) addr;
		if (!db_check_act_address_valid(th))
		    return;
	    } else {
		th = db_default_act;
		if (th == THR_ACT_NULL)
		   th = current_act();
		if (th == THR_ACT_NULL) {
		   db_printf("no active thread\n");
		   return;
		}
	    }
	    task = th->task;
	    if (th == current_act()) {
	        frame = (struct i386_frame *)ddb_regs.ebp;
	        callpc = (db_addr_t)ddb_regs.eip;
	    } else {
		if (th->mact.pcb == 0) {
		    db_printf("thread has no pcb\n");
		    return;
		}
		if (!th->thread) {
		    register struct i386_saved_state *iss =
						&th->mact.pcb->iss;

		    db_printf("thread has no shuttle\n");
		    frame = (struct i386_frame *) (iss->ebp);
		    callpc = (db_addr_t) (iss->eip);
		}
		else if ((th->thread->state & TH_SWAPPED) ||
						th->kernel_stack == 0) {
		    register struct i386_saved_state *iss =
						&th->mact.pcb->iss;

		    db_printf("Continuation ");
		    db_task_printsym((db_expr_t)th->thread->swap_func,
							DB_STGY_PROC, task);
		    db_printf("\n");
		    frame = (struct i386_frame *) (iss->ebp);
		    callpc = (db_addr_t) (iss->eip);
		} else {
		    int cpu;

		    for (cpu = 0; cpu < NCPUS; cpu++) {
			if (machine_slot[cpu].running == TRUE &&
			    active_threads[cpu] == th->thread &&
			    saved_state[cpu]) {
			    db_printf(">>>>> active on cpu %d <<<<<\n",
				      cpu);
			    frame = (struct i386_frame *)
				saved_state[cpu]->ebp;
			    callpc = (db_addr_t) saved_state[cpu]->eip;
			    break;
			}
		    }
		    if (cpu == NCPUS) {
			register struct i386_kernel_state *iks;
			iks = STACK_IKS(th->kernel_stack);
			frame = (struct i386_frame *) (iks->k_ebp);
			callpc = (db_addr_t) (iks->k_eip);
		    }
	        }
	    }
	} else {
	    frame = (struct i386_frame *)addr;
	    th = (db_default_act)? db_default_act: current_act();
	    task = (th != THR_ACT_NULL)? th->task: TASK_NULL;
	    callpc = (db_addr_t)db_get_task_value((int)&frame->f_retaddr, 4, 
							FALSE, task);
	}

	if (!INKERNEL((unsigned)callpc) && !INKERNEL((unsigned)frame)) {
	    db_printf(">>>>> user space <<<<<\n");
	    if (kernel_only)
		goto thread_done;
	    user_frame++;
	} else if (INKSERVER(callpc) && INKSERVER(frame)) {
	    db_printf(">>>>> INKserver space <<<<<\n");
	}

	lastframe = 0;
	while (frame_count-- && frame != 0) {
	    int narg;
	    char *	name;
	    db_expr_t	offset;
	    db_addr_t call_func = 0;
	    int r;

	    db_symbol_values(db_search_task_symbol_and_line(callpc,
							    DB_STGY_XTRN, 
							    &offset,
							    &filename,
							    &linenum,
							    task,
							    &narg),
			     &name, (db_expr_t *)&call_func);
	    if (INKERNEL((unsigned)callpc) && user_frame == 0) {
		if (call_func == db_user_trap_symbol_value ||
		    call_func == db_kernel_trap_symbol_value) {
		    frame_type = TRAP;
		    narg = 1;
		} else if (call_func == db_interrupt_symbol_value) {
		    frame_type = INTERRUPT;
		    goto next_frame;
		} else if (call_func == db_syscall_symbol_value) {
		    frame_type = SYSCALL;
		    goto next_frame;
		} else {
		    frame_type = 0;
		    prev = db_recover;
		    if ((r = _setjmp(db_recover = &db_jmp_buf)) == 0) {
		    	if (narg < 0)
			    narg = db_numargs(frame, task);
			db_recover = prev;
		    } else {
			db_recover = prev;
			goto thread_done;
		    }
		}
	    } else if (INKERNEL((unsigned)callpc) ^ INKERNEL((unsigned)frame)) {
		frame_type = 0;
		narg = -1;
	    } else {
	    	frame_type = 0;
		prev = db_recover;
		if ((r = _setjmp(db_recover = &db_jmp_buf)) == 0) {
		    if (narg < 0)
			narg = db_numargs(frame, task);
		    db_recover = prev;
		} else {
		    db_recover = prev;
		    goto thread_done;
		}
	    }

	    if (name == 0 || offset > db_maxoff) {
		db_printf("0x%x 0x%x(", frame, callpc);
		offset = 0;
	    } else
	        db_printf("0x%x %s(", frame, name);

	    argp = &frame->f_arg0;
	    while (narg > 0) {
		int value;

		prev = db_recover;
		if ((r = _setjmp(db_recover = &db_jmp_buf)) == 0) {
		    value = db_get_task_value((int)argp,4,FALSE,task);
		} else {
		    db_recover = prev;
		    if (r == 2)		/* 'q' from db_more() */
			db_error(0);
		    db_printf("... <stack error>)");
		    if (offset)
			db_printf("+%x", offset);
		    if (filename) {
			db_printf(" [%s", filename);
			if (linenum > 0)
			    db_printf(":%d", linenum);
			db_printf("]");
		    }
		    db_printf("\n");
		    goto thread_done;
		}
		db_recover = prev;
		db_printf("%x", value);
		argp++;
		if (--narg != 0)
		    db_printf(",");
	    }
	    if (narg < 0)
		db_printf("...");
	    db_printf(")");
	    if (offset) {
		db_printf("+%x", offset);
            }
	    if (filename) {
		db_printf(" [%s", filename);
		if (linenum > 0)
		    db_printf(":%d", linenum);
		db_printf("]");
	    }
	    db_printf("\n");

	next_frame:
	    db_nextframe(&lastframe, &frame, &callpc, frame_type, th);

	    if (frame == 0) {
		/* end of chain */
		break;
	    }
	    if (!INKERNEL(lastframe) ||
		(!INKERNEL((unsigned)callpc) && !INKERNEL((unsigned)frame)))
		user_frame++;
	    if (user_frame == 1) {
		db_printf(">>>>> user space <<<<<\n");
		if (kernel_only)
		    break;
	    } else if (!INKSERVER(lastframe) &&
			(INKSERVER(callpc) && INKSERVER(frame))) {
		db_printf(">>>>> inkserver space <<<<<\n");
	    }
	    if (frame <= lastframe) {
		if (INKERNEL(lastframe) && !INKERNEL(frame))
		    continue;
		db_printf("Bad frame pointer: 0x%x\n", frame);
		break;
	    }
	}

    thread_done:
	if (trace_all_threads) {
	    th = (thread_act_t) queue_next(&th->thr_acts);
	    if (! queue_end(act_list, (queue_entry_t) th)) {
		db_printf("\n");
		addr = (db_expr_t) th;
		thcount++;
		goto next_thread;

	    }
	}
}
