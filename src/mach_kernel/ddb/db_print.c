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
 * Revision 2.11.3.2  92/04/08  15:43:10  jeffreyh
 * 	Added i option to show thread. This gives wait state information.
 * 	[92/04/08            sjs]
 * 
 * Revision 2.11.3.1  92/03/03  16:13:34  jeffreyh
 * 	Pick up changes from TRUNK
 * 	[92/02/26  11:00:01  jeffreyh]
 * 
 * Revision 2.13  92/02/20  18:34:28  elf
 * 	Fixed typo.
 * 	[92/02/20            elf]
 * 
 * Revision 2.12  92/02/19  15:07:47  elf
 * 	Added db_thread_fp_used, to avoid machine-dependent conditionals.
 * 	[92/02/19            rpd]
 * 
 * 	Added 'F' flag to db_thread_stat showing if the thread has a valid
 * 	FPU context. Tested on i386 and pmax.
 * 	[92/02/17            kivinen]
 * 
 * Revision 2.11  91/11/12  11:50:32  rvb
 * 	Added OPTION_USER ("/u") to db_show_all_threads, db_show_one_thread,
 * 	db_show_one_task.  Without it, we display old-style information.
 * 	[91/10/31            rpd]
 * 
 * Revision 2.10  91/10/09  16:01:48  af
 * 	Supported "show registers" for non current thread.
 * 	Changed display format of thread and task information.
 * 	Changed "show thread" to print current thread information 
 * 	  if no thread is specified.
 * 	Added "show_one_task" for "show task" command.
 * 	Added IPC port print routines for "show ipc_port" command.
 * 	[91/08/29            tak]
 * 
 * Revision 2.9  91/08/03  18:17:19  jsb
 * 	In db_print_thread, if the thread is swapped and there is a
 * 	continuation function, print the function name in parentheses
 * 	instead of '(swapped)'.
 * 	[91/07/04  09:59:27  jsb]
 * 
 * Revision 2.8  91/07/31  17:30:43  dbg
 * 	Revise scheduling state machine.
 * 	[91/07/30  16:43:42  dbg]
 * 
 * Revision 2.7  91/07/09  23:15:57  danner
 * 	Fixed a few printf that should be db_printfs. 
 * 	[91/07/08            danner]
 * 
 * Revision 2.6  91/05/14  15:35:25  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/02/05  17:06:53  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  16:18:56  mrt]
 * 
 * Revision 2.4  90/10/25  14:43:54  rwd
 * 	Changed db_show_regs to print unsigned.
 * 	[90/10/19            rpd]
 * 	Generalized the watchpoint support.
 * 	[90/10/16            rwd]
 * 
 * Revision 2.3  90/09/09  23:19:52  rpd
 * 	Avoid totally incorrect guesses of symbol names for small values.
 * 	[90/08/30  17:39:08  af]
 * 
 * Revision 2.2  90/08/27  21:51:49  dbg
 * 	Insist that 'show thread' be called with an explicit address.
 * 	[90/08/22            dbg]
 * 
 * 	Fix type for db_maxoff.
 * 	[90/08/20            dbg]
 * 
 * 	Do not dereference the "valuep" field of a variable directly,
 * 	call the new db_read/write_variable functions instead.
 * 	Reflected changes in symbol lookup functions.
 * 	[90/08/20            af]
 * 	Reduce lint.
 * 	[90/08/10  14:33:44  dbg]
 * 
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
 * Miscellaneous printing.
 */
#include <norma_ipc.h>

#include <string.h>			/* For strlen() */
#include <mach/port.h>
#include <kern/task.h>
#include <kern/thread.h>
#include <kern/queue.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_space.h>
#include <ipc/ipc_pset.h>
#include <vm/vm_print.h>		/* for db_vm() */

#include <machine/db_machdep.h>
#include <machine/thread.h>

#include <ddb/db_lex.h>
#include <ddb/db_variables.h>
#include <ddb/db_sym.h>
#include <ddb/db_task_thread.h>
#include <ddb/db_command.h>
#include <ddb/db_output.h>		/* For db_printf() */
#include <ddb/db_print.h>

#if	NORMA_IPC
#include <norma/ipc_net.h>
#endif	/* NORMA_IPC */

extern unsigned int	db_maxoff;

/* Prototypes for functions local to this file.  XXX -- should be static!
 */

char *db_act_stat(
	register thread_act_t	thr_act,
	char	 		*status);

void db_print_task(
	task_t	task,
	int	task_id,
	int	flag);

int db_port_iterate(
	thread_act_t	thr_act,
	int		is_pset);

ipc_port_t db_lookup_port(
	thread_act_t	thr_act,
	int		id);

static void db_print_port_id(
	int		id,
	ipc_port_t	port,
	unsigned	bits,
	int		n);

void db_print_act(
	thread_act_t 	thr_act,
	int		act_id,
	int	 	flag);

void db_print_space(
	task_t	task,
	int	task_id,
	int	flag);

void
db_show_regs(
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char		*modif)
{
	register struct db_variable *regp;
	db_expr_t	value, offset;
	char *		name;
	register	i;
	struct db_var_aux_param aux_param;
	task_t		task = TASK_NULL;

	aux_param.modif = modif;
	aux_param.thr_act = THR_ACT_NULL;
	if (db_option(modif, 't')) {
	    if (have_addr) {
		if (!db_check_act_address_valid((thread_act_t)addr))
		    return;
		aux_param.thr_act = (thread_act_t)addr;
	    } else
	        aux_param.thr_act = db_default_act;
	    if (aux_param.thr_act != THR_ACT_NULL)
		task = aux_param.thr_act->task;
	}
	for (regp = db_regs; regp < db_eregs; regp++) {
	    if (regp->max_level > 1) {
		db_printf("bad multi-suffixed register %s\n", regp->name);
		continue;
	    }
	    aux_param.level = regp->max_level;
	    for (i = regp->low; i <= regp->high; i++) {
		aux_param.suffix[0] = i;
	        db_read_write_variable(regp, &value, DB_VAR_GET, &aux_param);
		if (regp->max_level > 0)
		    db_printf("%s%d%*s", regp->name, i, 
				12-strlen(regp->name)-((i<10)?1:2), "");
		else
		    db_printf("%-12s", regp->name);
		db_printf("%#10n", value);
		db_find_xtrn_task_sym_and_offset((db_addr_t)value, &name, 
							&offset, task);
		if (name != 0 && offset <= db_maxoff && offset != value) {
		    db_printf("\t%s", name);
		    if (offset != 0)
			db_printf("+%#r", offset);
	    	}
		db_printf("\n");
	    }
	}
}

#define OPTION_LONG		0x001		/* long print option */
#define OPTION_USER		0x002		/* print ps-like stuff */
#define OPTION_INDENT		0x100		/* print with indent */
#define OPTION_THREAD_TITLE	0x200		/* print thread title */
#define OPTION_TASK_TITLE	0x400		/* print thread title */

#ifndef	DB_TASK_NAME
#define DB_TASK_NAME(task)			/* no task name */
#define DB_TASK_NAME_TITLE	""		/* no task name */
#endif	/* DB_TASK_NAME */

#ifndef	db_act_fp_used
#define db_act_fp_used(thr_act)	FALSE
#endif

char *
db_act_stat(
	register thread_act_t	thr_act,
	char	 		*status)
{
	register char *p = status;
	
	if (!thr_act->active) {
		*p++ = 'D',
		*p++ = 'y',
		*p++ = 'i',
		*p++ = 'n',
		*p++ = 'g';
		*p++ = ' ';
	} else if (!thr_act->thread) {
		*p++ = 'E',
		*p++ = 'm',
		*p++ = 'p',
		*p++ = 't',
		*p++ = 'y';
		*p++ = ' ';
	} else {
		thread_t athread = thr_act->thread;

		*p++ = (athread->state & TH_RUN)  ? 'R' : '.';
		*p++ = (athread->state & TH_WAIT) ? 'W' : '.';
		*p++ = (athread->state & TH_SUSP) ? 'S' : '.';
		*p++ = (athread->state & TH_SWAPPED) ? 'O' : '.';
		*p++ = (athread->state & TH_UNINT) ? 'N' : '.';
		/* show if the FPU has been used */
		*p++ = db_act_fp_used(thr_act) ? 'F' : '.';
	}
	*p++ = 0;
	return(status);
}

char	*policy_list[] = { "TS", "RR", "??", "FF" };

void
db_print_act(
	thread_act_t 	thr_act,
	int		act_id,
	int		flag)
{
	thread_t athread;

	if (!thr_act) {
	    db_printf("db_print_act(NULL)!\n");
	    return;
	}

	athread = thr_act->thread;
	if (flag & OPTION_USER) {
	    char status[8];
	    char *indent = "";
	    int	 policy;

	    if (flag & OPTION_LONG) {
		if (flag & OPTION_INDENT)
		    indent = "    ";
		if (flag & OPTION_THREAD_TITLE) {
		    db_printf("%s ID:   ACT     STAT   STACK    THREAD ", indent);
		    db_printf("  SUS  PRI  CONTINUE,WAIT_FUNC\n");
		}
		policy = (athread ? athread->policy : 2);
		db_printf("%s%3d%c %08x %s %08x %08x %3d %3d/%s ",
		    indent, act_id,
		    (thr_act == current_act())? '#': ':',
		    thr_act, db_act_stat(thr_act, status),
		    (athread ? thr_act->kernel_stack : 0),
		    athread,
		    thr_act->suspend_count,
		    (athread ? athread->sched_pri : 999), /* XXX */
		    policy_list[policy-1]);
		if (athread) {
		    if ((athread->state & TH_SWAPPED) && athread->swap_func) {
			db_task_printsym((db_addr_t)athread->swap_func,
					DB_STGY_ANY, kernel_task);
			db_printf(", ");
		    }
		    if (athread->state & TH_WAIT)
			db_task_printsym((db_addr_t)athread->wait_event,
						DB_STGY_ANY, kernel_task);
		}
		db_printf("\n");
	    } else {
		if (act_id % 3 == 0) {
		    if (flag & OPTION_INDENT)
			db_printf("\n    ");
		} else
		    db_printf(" ");
		db_printf("%3d%c(%08x,%s)", act_id, 
		    (thr_act == current_act())? '#': ':',
		    thr_act, db_act_stat(thr_act, status));
	    }
	} else {
	    if (flag & OPTION_INDENT)
		db_printf("            %3d (%08x) ", act_id, thr_act);
	    else
		db_printf("(%08x) ", thr_act);
	    if (athread) {
		db_printf("%c%c%c%c%c",
			(athread->state & TH_RUN)  ? 'R' : ' ',
			(athread->state & TH_WAIT) ? 'W' : ' ',
			(athread->state & TH_SUSP) ? 'S' : ' ',
			(athread->state & TH_UNINT)? 'N' : ' ',
			db_act_fp_used(thr_act) ? 'F' : ' ');
		if (athread->state & TH_SWAPPED) {
		    if (athread->swap_func) {
			db_printf("(");
			db_task_printsym((db_addr_t)athread->swap_func,
						DB_STGY_ANY, kernel_task);
			db_printf(")");
		    } else {
			db_printf("(swapped)");
		    }
		}
		if (athread->state & TH_WAIT) {
		    db_printf(" ");
		    db_task_printsym((db_addr_t)athread->wait_event,
						DB_STGY_ANY, kernel_task);
		}
		db_printf("\n");
	    }
	}
}

void
db_print_task(
	task_t	task,
	int	task_id,
	int	flag)
{
	thread_act_t thr_act;
	int act_id;

	if (flag & OPTION_USER) {
	    if (flag & OPTION_TASK_TITLE) {
		db_printf(" ID: TASK     MAP      THD SUS PR %s", 
			  DB_TASK_NAME_TITLE);
		if ((flag & OPTION_LONG) == 0)
		    db_printf("  THREADS");
		db_printf("\n");
	    }
	    db_printf("%3d: %08x %08x %3d %3d %2d ",
			    task_id, task, task->map, task->thr_act_count,
			    0/*task->suspend_count*/, task->priority);
	    DB_TASK_NAME(task);
	    if (flag & OPTION_LONG) {
		if (flag & OPTION_TASK_TITLE)
		    flag |= OPTION_THREAD_TITLE;
		db_printf("\n");
	    } else if (task->thr_act_count <= 1)
		flag &= ~OPTION_INDENT;
	    act_id = 0;
	    queue_iterate(&task->thr_acts, thr_act, thread_act_t, thr_acts) {
		db_print_act(thr_act, act_id, flag);
		flag &= ~OPTION_THREAD_TITLE;
		act_id++;
	    }
	    if ((flag & OPTION_LONG) == 0)
		db_printf("\n");
	} else {
	    if (flag & OPTION_TASK_TITLE)
		db_printf("    TASK        THREADS\n");
	    db_printf("%3d (%08x): ", task_id, task);
	    if (task->thr_act_count == 0) {
		db_printf("no threads\n");
	    } else {
		if (task->thr_act_count > 1) {
		    db_printf("%d threads: \n", task->thr_act_count);
		    flag |= OPTION_INDENT;
		} else
		    flag &= ~OPTION_INDENT;
		act_id = 0;
		queue_iterate(&task->thr_acts, thr_act,
			      thread_act_t, thr_acts)
		    db_print_act(thr_act, act_id++, flag);
	    }
	}
}

void
db_print_space(
	task_t	task,
	int	task_id,
	int	flag)
{
	ipc_space_t space;

	space = task->itk_space;
	db_printf("%3d: 0x%08x 0x%08x 0x%08x %sactive   %d\n",
		  task_id, task, space, task->map,
		  space->is_active? "":"!", space->is_table_size);
}

void
db_show_all_acts(
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char *		modif)
{
	task_t task;
	int task_id;
	int flag;
	processor_set_t pset;

	flag = OPTION_TASK_TITLE|OPTION_INDENT;
	if (db_option(modif, 'u'))
	    flag |= OPTION_USER;
	if (db_option(modif, 'l'))
	    flag |= OPTION_LONG;

	task_id = 0;
	queue_iterate(&all_psets, pset, processor_set_t, all_psets) {
	    queue_iterate(&pset->tasks, task, task_t, pset_tasks) {
		db_print_task(task, task_id, flag);
		flag &= ~OPTION_TASK_TITLE;
		task_id++;
	    }
	}
}

void
db_show_one_space(
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char *		modif)
{
	int		flag;
	int		task_id;
	task_t		task;

	flag = OPTION_TASK_TITLE;
	if (db_option(modif, 'u'))
	    flag |= OPTION_USER;
	if (db_option(modif, 'l'))
	    flag |= OPTION_LONG;

	if (!have_addr) {
	    task = db_current_task();
	    if (task == TASK_NULL) {
		db_error("No task\n");
		/*NOTREACHED*/
	    }
	} else
	    task = (task_t) addr;

	if ((task_id = db_lookup_task(task)) < 0) {
	    db_printf("bad task address 0x%x\n", addr);
	    db_error(0);
	    /*NOTREACHED*/
	}

	db_printf(" ID: TASK     SPACE    MAP               COUNT\n");
	db_print_space(task, task_id, flag);
}

void
db_show_all_spaces(
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char *		modif)
{
	task_t task;
	int task_id = 0;
	int flag;
	processor_set_t pset;

	flag = OPTION_TASK_TITLE|OPTION_INDENT;
	if (db_option(modif, 'u'))
	    flag |= OPTION_USER;
	if (db_option(modif, 'l'))
	    flag |= OPTION_LONG;

	db_printf(" ID: TASK     SPACE    MAP               COUNT\n");
	queue_iterate(&all_psets, pset, processor_set_t, all_psets) {
	    queue_iterate(&pset->tasks, task, task_t, pset_tasks) {
		    db_print_space(task, task_id, flag);
		    task_id++;
	    }
	}
}

db_addr_t
db_task_from_space(
	ipc_space_t	space,
	int		*task_id)
{
	task_t task;
	int tid = 0;
	processor_set_t pset;

	queue_iterate(&all_psets, pset, processor_set_t, all_psets) {
	    queue_iterate(&pset->tasks, task, task_t, pset_tasks) {
		    if (task->itk_space == space) {
			    *task_id = tid;
			    return (db_addr_t)task;
		    }
		    tid++;
	    }
	}
	*task_id = 0;
	return (0);
}

void
db_show_one_act(
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char *		modif)
{
	int		flag;
	int		act_id;
	thread_act_t		thr_act;

	flag = OPTION_THREAD_TITLE;
	if (db_option(modif, 'u'))
	    flag |= OPTION_USER;
	if (db_option(modif, 'l'))
	    flag |= OPTION_LONG;

	if (!have_addr) {
	    thr_act = current_act();
	    if (thr_act == THR_ACT_NULL) {
		db_error("No thr_act\n");
		/*NOTREACHED*/
	    }
	} else
	    thr_act = (thread_act_t) addr;

	if ((act_id = db_lookup_act(thr_act)) < 0) {
	    db_printf("bad thr_act address 0x%x\n", addr);
	    db_error(0);
	    /*NOTREACHED*/
	}

	if (flag & OPTION_USER) {
	    db_printf("TASK%d(%08x):\n",
		      db_lookup_task(thr_act->task), thr_act->task);
	    db_print_act(thr_act, act_id, flag);
	} else {
	    db_printf("task %d(%08x): thr_act    %d",
		      db_lookup_task(thr_act->task), thr_act->task, act_id);
	    db_print_act(thr_act, act_id, flag);
	}
	if (db_option(modif, 'i') &&  thr_act->thread &&
	    (thr_act->thread->state & TH_WAIT) && 
	    thr_act->kernel_stack == 0) {

	    db_printf("Wait State: option 0x%x object 0x%x mqueue 0x%x\n",
		thr_act->thread->ith_option,
		thr_act->thread->ith_object,
		thr_act->thread->ith_mqueue);
	}
}

void
db_show_one_task(
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char *		modif)
{
	int		flag;
	int		task_id;
	task_t		task;

	flag = OPTION_TASK_TITLE;
	if (db_option(modif, 'u'))
	    flag |= OPTION_USER;
	if (db_option(modif, 'l'))
	    flag |= OPTION_LONG;

	if (!have_addr) {
	    task = db_current_task();
	    if (task == TASK_NULL) {
		db_error("No task\n");
		/*NOTREACHED*/
	    }
	} else
	    task = (task_t) addr;

	if ((task_id = db_lookup_task(task)) < 0) {
	    db_printf("bad task address 0x%x\n", addr);
	    db_error(0);
	    /*NOTREACHED*/
	}

	db_print_task(task, task_id, flag);
}

void
db_show_shuttle(
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char *		modif)
{
	thread_shuttle_t	shuttle;
	thread_act_t		thr_act;

	if (have_addr)
	    shuttle = (thread_shuttle_t) addr;
	else {
	    thr_act = current_act();
	    if (thr_act == THR_ACT_NULL) {
		db_error("No thr_act\n");
		/*NOTREACHED*/
	    }
	    shuttle = thr_act->thread;
	    if (shuttle == THREAD_NULL) {
		db_error("No shuttle associated with current thr_act\n");
		/*NOTREACHED*/
	    }
	}
	db_printf("shuttle %x:\n", shuttle);
	if (shuttle->top_act == THR_ACT_NULL)
	    db_printf("  no activations\n");
	else {
	    db_printf("  activations:");
	    for (thr_act = shuttle->top_act; thr_act != THR_ACT_NULL;
		 thr_act = thr_act->lower) {
		if (thr_act != shuttle->top_act)
		    printf(" from");
		printf(" $task%d.%d(%x)", db_lookup_task(thr_act->task),
		       db_lookup_act(thr_act), thr_act);
	    }
	    db_printf("\n");
	}
}

#define	db_pset_kmsg_count(port) \
	(ipc_list_count((port)->ip_pset->ips_messages.imq_messages.ikmq_base))

int
db_port_kmsg_count(
	ipc_port_t	port)
{
	return (port->ip_pset ? db_pset_kmsg_count(port) : port->ip_msgcount);
}

int
db_port_iterate(
	thread_act_t	thr_act,
	int		is_pset)
{
	ipc_entry_t entry;
	int index;
	int n = 0;
	int size;
	ipc_space_t space;

	space = thr_act->task->itk_space;
	entry = space->is_table;
	size = space->is_table_size;
	for (index = 0; index < size; index++, entry++) {
	    if (entry->ie_bits & MACH_PORT_TYPE_PORT_RIGHTS) {
		ipc_port_t aport = (ipc_port_t)entry->ie_object;
		unsigned bits = entry->ie_bits;

		if (!is_pset){
		    if (n && n % 3 == 0)
			db_printf("\n");
		    db_printf("\t%s%d(%s,%x,%d)",
			    aport->ip_pset ? "pset" : "port",
			    index,
			    (bits & MACH_PORT_TYPE_RECEIVE)? "r":
			    (bits & MACH_PORT_TYPE_SEND)? "s": "S", aport,
			    db_port_kmsg_count(aport));
		    n++;
		} else if (aport->ip_pset) {
		    if (n && (n & 1) == 0)
			db_printf("\n");
		    db_printf("%sport%d(%s,%x,set=%x,%d)",
			    (n & 1) ? "\t" : "    ",  index,
			    (bits & MACH_PORT_TYPE_RECEIVE)? "r":
			    (bits & MACH_PORT_TYPE_SEND)? "s": "S",
			    aport, aport->ip_pset,
			    db_pset_kmsg_count(aport));
		    n++;
		}
	    }
	}
	return(n);
}

ipc_port_t
db_lookup_port(
	thread_act_t	thr_act,
	int		id)
{
	register ipc_space_t space;
	register ipc_entry_t entry;

	if (thr_act == THR_ACT_NULL)
	    return(0);
	space = thr_act->task->itk_space;
	if (id < 0 || id >= space->is_table_size)
	    return(0);
	entry = &space->is_table[id];
	if (entry->ie_bits & MACH_PORT_TYPE_PORT_RIGHTS)
	    return((ipc_port_t)entry->ie_object);
	return(0);
}

static void
db_print_port_id(
	int		id,
	ipc_port_t	port,
	unsigned	bits,
	int		n)
{
	if (n != 0 && n % 3 == 0)
	    db_printf("\n");
	db_printf("\tport%d(%s,%x)", id,
		(bits & MACH_PORT_TYPE_RECEIVE)? "r":
		(bits & MACH_PORT_TYPE_SEND)? "s": "S", port);
}

void
db_show_port_id(
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char *		modif)
{
	thread_act_t thr_act;

	if (!have_addr) {
	    thr_act = current_act();
	    if (thr_act == THR_ACT_NULL) {
		db_error("No thr_act\n");
		/*NOTREACHED*/
	    }
	} else
	    thr_act = (thread_act_t) addr;
	if (db_lookup_act(thr_act) < 0) {
	    db_printf("Bad thr_act address 0x%x\n", addr);
	    db_error(0);
	    /*NOTREACHED*/
	}
	if (db_port_iterate(thr_act, db_option(modif,'s')))
	    db_printf("\n");
}

/*
 *	Useful system state when the world has hung.
 */
void
db_sys()
{
	db_vm();
#if	NORMA_IPC
	iprintf("\n");
	db_norma_ipc();
#endif
	db_printf("current_{thread/task} 0x%x 0x%x\n",
			current_thread(),current_task());
}

void db_show_one_runq(run_queue_t runq);

void
db_show_runq(
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char *		modif)
{
	processor_set_t pset;
	processor_t proc;
	run_queue_t runq;
	boolean_t showedany = FALSE;

	queue_iterate(&all_psets, pset, processor_set_t, all_psets) {
#if NCPUS > 1	/* This code has not been tested. */
	    queue_iterate(&pset->processors, proc, processor_t, processors) {
		runq = &proc->runq;
		if (runq->count > 0) {
		    db_printf("PROCESSOR %x IN SET %x\n", proc, pset);
		    db_show_one_runq(runq);
		    showedany = TRUE;
		}
	    }
#endif	/* NCPUS > 1 */
#ifndef NCPUS
#error NCPUS undefined
#endif
	    runq = &pset->runq;
	    if (runq->count > 0) {
		db_printf("PROCESSOR SET %x\n", pset);
		db_show_one_runq(runq);
		showedany = TRUE;
	    }
	}
	if (!showedany)
	    db_printf("No runnable threads\n");
}

void
db_show_one_runq(
	run_queue_t	runq)
{
	int i, task_id, thr_act_id;
	queue_t q;
	thread_act_t thr_act;
	thread_t thread;
	task_t task;

	printf("PRI  TASK.ACTIVATION\n");
	for (i = runq->low, q = runq->runq + i; i < NRQS; i++, q++) {
	    if (!queue_empty(q)) {
		db_printf("%3d:", i);
		queue_iterate(q, thread, thread_t, links) {
		    thr_act = thread->top_act;
		    task = thr_act->task;
		    task_id = db_lookup_task(task);
		    thr_act_id = db_lookup_task_act(task, thr_act);
		    db_printf(" %d.%d", task_id, thr_act_id);
		}
		db_printf("\n");
	    }
	}
}
