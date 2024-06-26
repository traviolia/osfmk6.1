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
 * Revision 2.9  91/10/09  16:02:08  af
 * 	 Revision 2.8.2.1  91/10/05  13:07:06  jeffreyh
 * 	 	Added task parameter for user space break point, and changed
 * 	 	 db_find_breakpoint_here to db_find_thread_breakpoint_here
 * 	 	 to support thread based break point.
 * 	 	Changed db_{set,clear}_single_step to db_{set,clear}_task_single
 * 	 	 _step to add task paramter and to maintain compatibility.
 * 	 	[91/08/29            tak]
 * 
 * Revision 2.8.2.1  91/10/05  13:07:06  jeffreyh
 * 	Added task parameter for user space break point, and changed
 * 	 db_find_breakpoint_here to db_find_thread_breakpoint_here
 * 	 to support thread based break point.
 * 	Changed db_{set,clear}_single_step to db_{set,clear}_task_single
 * 	 _step to add task paramter and to maintain compatibility.
 * 	[91/08/29            tak]
 * 
 * Revision 2.8  91/07/09  23:16:01  danner
 * 	Added logic to db_set_single_step not to set a breakpoint at the
 * 	 next sequential instruction if the current instruction is an
 * 	 unconditional transfer of flow of control instruction. This
 * 	 avoids problems with the debugger overwriting data or clobbering
 * 	 routines that the debugger itself might need. This is determined
 * 	 by calling the predicate inst_unconditional_flow_transfer. This
 * 	 predicate now needs to be defined for all architectures using
 * 	 the software single step. 
 * 
 * 	     Added include of ddb/db_run.h, where all the STEP defines have been
 * 	      moved.
 * 	 
 * 	[91/07/08            danner]
 * 
 * Revision 2.7  91/06/06  17:03:51  jsb
 * 	Removed redundant newlines and tabs.
 * 	Added delta to instruction count printf.
 * 	[91/05/25  10:55:01  jsb]
 * 
 * Revision 2.6  91/05/14  15:35:39  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/02/05  17:06:58  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  16:19:05  mrt]
 * 
 * Revision 2.4  91/01/08  15:09:10  rpd
 * 	Fixed bug in db_restart_at_pc.
 * 	[90/12/07            rpd]
 * 	Added STEP_COUNT and count option to db_continue_cmd.
 * 	Changed db_stop_at_pc to return (modified) is_breakpoint.
 * 	Fixed db_stop_at_pc to print newlines in the right places.
 * 	[90/11/27            rpd]
 * 
 * Revision 2.3  90/10/25  14:43:59  rwd
 * 	Changed db_find_breakpoint to db_find_breakpoint_here.
 * 	[90/10/18            rpd]
 * 
 * 	Fixed db_set_single_step to pass regs to branch_taken.
 * 	Added watchpoint argument to db_restart_at_pc.
 * 	[90/10/17            rpd]
 * 	Generalized the watchpoint support.
 * 	[90/10/16            rwd]
 * 	Added watchpoint support.
 * 	[90/10/16            rpd]
 * 
 * Revision 2.2  90/08/27  21:51:59  dbg
 * 	Fixed names for single-step functions.
 * 	[90/08/20            af]
 * 	Reduce lint.
 * 	[90/08/07            dbg]
 * 	Created.
 * 	[90/07/25            dbg]
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
/*
 * 	Author: David B. Golub, Carnegie Mellon University
 *	Date:	7/90
 */

/*
 * Commands to run process.
 */
#include <mach/boolean.h>
#include <machine/db_machdep.h>

#include <ddb/db_lex.h>
#include <ddb/db_break.h>
#include <ddb/db_access.h>
#include <ddb/db_run.h>
#include <ddb/db_cond.h>
#include <ddb/db_examine.h>
#include <ddb/db_output.h>		/* For db_printf() */
#include <ddb/db_watch.h>
#include <kern/misc_protos.h>

int	db_run_mode;

boolean_t	db_sstep_print;
int		db_loop_count;
int		db_call_depth;

int		db_inst_count;
int		db_last_inst_count;
int		db_load_count;
int		db_store_count;
int		db_max_inst_count = 1000;

#ifndef db_set_single_step
void db_set_task_single_step(
	register db_regs_t	*regs,
	task_t		   	task);
#else
#define	db_set_task_single_step(regs,task)	db_set_single_step(regs)
#endif
#ifndef db_clear_single_step
void db_clear_task_single_step(
	db_regs_t	*regs,
	task_t	  	task);
#else
#define db_clear_task_single_step(regs,task)	db_clear_single_step(regs)
#endif

extern jmp_buf_t *db_recover;
boolean_t db_step_again(void);

boolean_t
db_stop_at_pc(
	boolean_t	*is_breakpoint,
	task_t		task,
	task_t		space)
{
	register  db_addr_t	pc;
	register  db_thread_breakpoint_t bkpt;

	db_clear_task_single_step(DDB_REGS, space);
	db_clear_breakpoints();
	db_clear_watchpoints();
	pc = PC_REGS(DDB_REGS);

#ifdef	FIXUP_PC_AFTER_BREAK
	if (*is_breakpoint) {
	    /*
	     * Breakpoint trap.  Fix up the PC if the
	     * machine requires it.
	     */
	    FIXUP_PC_AFTER_BREAK
	    pc = PC_REGS(DDB_REGS);
	}
#endif

	/*
	 * Now check for a breakpoint at this address.
	 */
	bkpt = db_find_thread_breakpoint_here(space, pc);
	if (bkpt) {
	    if (db_cond_check(bkpt)) {
		*is_breakpoint = TRUE;
		return (TRUE);	/* stop here */
	    }
	}
	*is_breakpoint = FALSE;

	if (db_run_mode == STEP_INVISIBLE) {
	    db_run_mode = STEP_CONTINUE;
	    return (FALSE);	/* continue */
	}
	if (db_run_mode == STEP_COUNT) {
	    return (FALSE); /* continue */
	}
	if (db_run_mode == STEP_ONCE) {
	    if (--db_loop_count > 0) {
		if (db_sstep_print) {
		    db_print_loc_and_inst(pc, task);
		}
		return (FALSE);	/* continue */
	    }
	}
	if (db_run_mode == STEP_RETURN) {
	    jmp_buf_t *prev;
	    jmp_buf_t db_jmpbuf;
	    db_expr_t ins = db_get_task_value(pc, sizeof(int), FALSE, space);

	    /* continue until matching return */

	    prev = db_recover;
	    if (_setjmp(db_recover = &db_jmpbuf) == 0) {
	    	if (!inst_trap_return(ins) &&
		    (!inst_return(ins) || --db_call_depth != 0)) {
			if (db_sstep_print) {
		    	    if (inst_call(ins) || inst_return(ins)) {
				register int i;

				db_printf("[after %6d /%4d] ",
					  db_inst_count,
					  db_inst_count - db_last_inst_count);
				db_last_inst_count = db_inst_count;
				for (i = db_call_depth; --i > 0; )
				    db_printf("  ");
				db_print_loc_and_inst(pc, task);
				db_printf("\n");
		    	    }
		        }
			if (inst_call(ins))
			    db_call_depth++;
			db_recover = prev;
			if (db_step_again())
				return (FALSE);	/* continue */
	        }
	    }
	    db_recover = prev;
	}
	if (db_run_mode == STEP_CALLT) {
	    db_expr_t ins = db_get_task_value(pc, sizeof(int), FALSE, space);

	    /* continue until call or return */

	    if (!inst_call(ins) &&
		!inst_return(ins) &&
		!inst_trap_return(ins)) {
			if (db_step_again())
				return (FALSE);	/* continue */
	    }
	}
	if (db_find_breakpoint_here(space, pc))
		return(FALSE);
	db_run_mode = STEP_NONE;
	return (TRUE);
}

void
db_restart_at_pc(
	boolean_t	watchpt,
	task_t	  	task)
{
	register db_addr_t pc = PC_REGS(DDB_REGS);

	if ((db_run_mode == STEP_COUNT) ||
	    (db_run_mode == STEP_RETURN) ||
	    (db_run_mode == STEP_CALLT)) {
	    db_expr_t		ins;

	    /*
	     * We are about to execute this instruction,
	     * so count it now.
	     */

	    ins = db_get_task_value(pc, sizeof(int), FALSE, task);
	    db_inst_count++;
	    db_load_count += db_inst_load(ins);
	    db_store_count += db_inst_store(ins);
#ifdef	SOFTWARE_SSTEP
	    /* XXX works on mips, but... */
	    if (inst_branch(ins) || inst_call(ins)) {
		ins = db_get_task_value(next_instr_address(pc,1,task),
				   sizeof(int), FALSE, task);
		db_inst_count++;
		db_load_count += db_inst_load(ins);
		db_store_count += db_inst_store(ins);
	    }
#endif	/* SOFTWARE_SSTEP */
	}

	if (db_run_mode == STEP_CONTINUE) {
	    if (watchpt || db_find_breakpoint_here(task, pc)) {
		/*
		 * Step over breakpoint/watchpoint.
		 */
		db_run_mode = STEP_INVISIBLE;
		db_set_task_single_step(DDB_REGS, task);
	    } else {
		db_set_breakpoints();
		db_set_watchpoints();
	    }
	} else {
	    db_set_task_single_step(DDB_REGS, task);
	}
}

/*
 * 'n' and 'u' commands might never return.
 * Limit the maximum number of steps.
 */

boolean_t
db_step_again(void)
{
	if (db_inst_count && !(db_inst_count%db_max_inst_count)) {
		char c;
		db_printf("%d instructions, continue ? (y/n) ",
			  db_inst_count);
	        c = cngetc();
		db_printf("\n");
		if(c == 'n')
			return(FALSE);
	}
	return(TRUE);
}

void
db_single_step(
	db_regs_t	*regs,
	task_t	  	task)
{
	if (db_run_mode == STEP_CONTINUE) {
	    db_run_mode = STEP_INVISIBLE;
	    db_set_task_single_step(regs, task);
	}
}

#ifdef	SOFTWARE_SSTEP
/*
 *	Software implementation of single-stepping.
 *	If your machine does not have a trace mode
 *	similar to the vax or sun ones you can use
 *	this implementation, done for the mips.
 *	Just define the above conditional and provide
 *	the functions/macros defined below.
 *
 * extern boolean_t
 *	inst_branch(),		returns true if the instruction might branch
 * extern unsigned
 *	branch_taken(),		return the address the instruction might
 *				branch to
 *	db_getreg_val();	return the value of a user register,
 *				as indicated in the hardware instruction
 *				encoding, e.g. 8 for r8
 *			
 * next_instr_address(pc,bd,task) returns the address of the first
 *				instruction following the one at "pc",
 *				which is either in the taken path of
 *				the branch (bd==1) or not.  This is
 *				for machines (mips) with branch delays.
 *
 *	A single-step may involve at most 2 breakpoints -
 *	one for branch-not-taken and one for branch taken.
 *	If one of these addresses does not already have a breakpoint,
 *	we allocate a breakpoint and save it here.
 *	These breakpoints are deleted on return.
 */			
db_breakpoint_t	db_not_taken_bkpt = 0;
db_breakpoint_t	db_taken_bkpt = 0;

void
db_set_task_single_step(
	register db_regs_t	*regs,
	task_t		   	task)
{
	db_addr_t pc = PC_REGS(regs);
	register unsigned	 inst, brpc;
	register boolean_t       unconditional;

	/*
	 *	User was stopped at pc, e.g. the instruction
	 *	at pc was not executed.
	 */
	inst = db_get_task_value(pc, sizeof(int), FALSE, task);
	if (inst_branch(inst) || inst_call(inst)) {
	    extern unsigned getreg_val();	/* XXX -- need prototype! */

	    brpc = branch_taken(inst, pc, getreg_val, (unsigned char*)regs);
	    if (brpc != pc) {	/* self-branches are hopeless */
		db_taken_bkpt = db_set_temp_breakpoint(task, brpc);
	    }
	    pc = next_instr_address(pc,1,task);
	} else 
	    pc = next_instr_address(pc,0,task);
	
	/* 
	 * check if this control flow instruction is an
	 * unconditional transfer
	 */

	unconditional = inst_unconditional_flow_transfer(inst);

	/* 
	  We only set the sequential breakpoint if previous instruction was not
	  an unconditional change of flow of control. If the previous instruction
	  is an unconditional change of flow of control, setting a breakpoint in the
	  next sequential location may set a breakpoint in data or in another routine,
	  which could screw up either the program or the debugger. 
	  (Consider, for instance, that the next sequential instruction is the 
	  start of a routine needed by the debugger.)
	*/
	if (!unconditional && db_find_breakpoint_here(task, pc) == 0 &&
	    (db_taken_bkpt == 0 || db_taken_bkpt->address != pc)) {
	    	db_not_taken_bkpt = db_set_temp_breakpoint(task, pc);
	} else
	    	db_not_taken_bkpt = 0;
}

void
db_clear_task_single_step(
	db_regs_t	*regs,
	task_t	  	task)
{
	if (db_taken_bkpt != 0) {
	    db_delete_temp_breakpoint(task, db_taken_bkpt);
	    db_taken_bkpt = 0;
	}
	if (db_not_taken_bkpt != 0) {
	    db_delete_temp_breakpoint(task, db_not_taken_bkpt);
	    db_not_taken_bkpt = 0;
	}
}

#endif	/* SOFTWARE_SSTEP */

extern int	db_cmd_loop_done;

/* single-step */
void
db_single_step_cmd(
	db_expr_t	addr,
	int		have_addr,
	db_expr_t	count,
	char *		modif)
{
	boolean_t	print = FALSE;

	if (count == -1)
	    count = 1;

	if (modif[0] == 'p')
	    print = TRUE;

	db_run_mode = STEP_ONCE;
	db_loop_count = count;
	db_sstep_print = print;
	db_inst_count = 0;
	db_last_inst_count = 0;
	db_load_count = 0;
	db_store_count = 0;

	db_cmd_loop_done = 1;
}

/* trace and print until call/return */
void
db_trace_until_call_cmd(
	db_expr_t	addr,
	int		have_addr,
	db_expr_t	count,
	char *		modif)
{
	boolean_t	print = FALSE;

	if (modif[0] == 'p')
	    print = TRUE;

	db_run_mode = STEP_CALLT;
	db_sstep_print = print;
	db_inst_count = 0;
	db_last_inst_count = 0;
	db_load_count = 0;
	db_store_count = 0;

	db_cmd_loop_done = 1;
}

void
db_trace_until_matching_cmd(
	db_expr_t	addr,
	int		have_addr,
	db_expr_t	count,
	char *		modif)
{
	boolean_t	print = FALSE;

	if (modif[0] == 'p')
	    print = TRUE;

	db_run_mode = STEP_RETURN;
	db_call_depth = 1;
	db_sstep_print = print;
	db_inst_count = 0;
	db_last_inst_count = 0;
	db_load_count = 0;
	db_store_count = 0;

	db_cmd_loop_done = 1;
}

/* continue */
void
db_continue_cmd(
	db_expr_t	addr,
	int		have_addr,
	db_expr_t	count,
	char *		modif)
{
	if (modif[0] == 'c')
	    db_run_mode = STEP_COUNT;
	else
	    db_run_mode = STEP_CONTINUE;
	db_inst_count = 0;
	db_last_inst_count = 0;
	db_load_count = 0;
	db_store_count = 0;

	db_cmd_loop_done = 1;
}

boolean_t
db_in_single_step(void)
{
	return(db_run_mode != STEP_NONE && db_run_mode != STEP_CONTINUE);
}
