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
 * Revision 2.2  91/10/09  16:03:04  af
 * 	 Revision 2.1.3.1  91/10/05  13:07:50  jeffreyh
 * 	 	Created for task/thread handling.
 * 	 	[91/08/29            tak]
 * 
 * Revision 2.1.3.1  91/10/05  13:07:50  jeffreyh
 * 	Created for task/thread handling.
 * 	[91/08/29            tak]
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

#include <machine/db_machdep.h>
#include <ddb/db_task_thread.h>
#include <ddb/db_variables.h>
#include <ddb/db_command.h>
#include <ddb/db_expr.h>
#include <ddb/db_lex.h>
#include <ddb/db_output.h>		/* For db_printf() */
#include <ddb/db_sym.h>

/*
 * Following constants are used to prevent infinite loop of task
 * or thread search due to the incorrect list.
 */
#define	DB_MAX_TASKID	0x10000		/* max # of tasks */
#define DB_MAX_THREADID	0x10000		/* max # of threads in a task */
#define DB_MAX_PSETS	0x10000		/* max # of processor sets */

task_t		db_default_task;	/* default target task */
thread_act_t	db_default_act;		/* default target thr_act */



/* Prototypes for functions local to this file.
 */
static task_t db_lookup_task_id(register task_id);

static thread_act_t db_lookup_act_id(
	task_t	 task,
	register thread_id);



/*
 * search valid task queue, and return the queue position as the task id
 */
int
db_lookup_task(task_t target_task)
{
	register task_t task;
	register task_id;
	register processor_set_t pset;
	register npset = 0;

	task_id = 0;
	if (queue_first(&all_psets) == 0)
	    return(-1);
	queue_iterate(&all_psets, pset, processor_set_t, all_psets) {
	    if (npset++ >= DB_MAX_PSETS)
		return(-1);
	    if (queue_first(&pset->tasks) == 0)
		continue;
	    queue_iterate(&pset->tasks, task, task_t, pset_tasks) {
		if (target_task == task)
		    return(task_id);
		if (task_id++ >= DB_MAX_TASKID)
		    return(-1);
	    }
	}
	return(-1);
}

/*
 * search thread queue of the task, and return the queue position
 */
int
db_lookup_task_act(
	task_t		task,
	thread_act_t	target_act)
{
	register thread_act_t thr_act;
	register act_id;

	act_id = 0;
	if (queue_first(&task->thr_acts) == 0)
	    return(-1);
	queue_iterate(&task->thr_acts, thr_act, thread_act_t, thr_acts) {
	    if (target_act == thr_act)
		return(act_id);
	    if (act_id++ >= DB_MAX_THREADID)
		return(-1);
	}
	return(-1);
}

/*
 * search thr_act queue of every valid task, and return the queue position
 * as the thread id.
 */
int
db_lookup_act(thread_act_t target_act)
{
	register act_id;
	register task_t task;
	register processor_set_t pset;
	register ntask = 0;
	register npset = 0;

	if (queue_first(&all_psets) == 0)
	    return(-1);
	queue_iterate(&all_psets, pset, processor_set_t, all_psets) {
	    if (npset++ >= DB_MAX_PSETS)
		return(-1);
	    if (queue_first(&pset->tasks) == 0)
		continue;
	    queue_iterate(&pset->tasks, task, task_t, pset_tasks) {
		if (ntask++ > DB_MAX_TASKID)
		    return(-1);
		if (task->thr_act_count == 0)
		    continue;
	        act_id = db_lookup_task_act(task, target_act);
		if (act_id >= 0)
		    return(act_id);
	    }
	}
	return(-1);
}

/*
 * check the address is a valid thread address
 */
int force_act_lookup = 0;
boolean_t
db_check_act_address_valid(thread_act_t thr_act)
{
	if (!force_act_lookup && db_lookup_act(thr_act) < 0) {
	    db_printf("Bad thr_act address 0x%x\n", thr_act);
	    db_flush_lex();
	    return(FALSE);
	} else
	    return(TRUE);
}

/*
 * convert task_id(queue postion) to task address
 */
static task_t
db_lookup_task_id(register task_id)
{
	register task_t task;
	register processor_set_t pset;
	register npset = 0;

	if (task_id > DB_MAX_TASKID)
	    return(TASK_NULL);
	if (queue_first(&all_psets) == 0)
	    return(TASK_NULL);
	queue_iterate(&all_psets, pset, processor_set_t, all_psets) {
	    if (npset++ >= DB_MAX_PSETS)
		return(TASK_NULL);
	    if (queue_first(&pset->tasks) == 0)
		continue;
	    queue_iterate(&pset->tasks, task, task_t, pset_tasks) {
		if (task_id-- <= 0)
			return(task);
	    }
	}
	return(TASK_NULL);
}

/*
 * convert (task_id, act_id) pair to thr_act address
 */
static thread_act_t
db_lookup_act_id(
	task_t	 task,
	register act_id)
{
	register thread_act_t thr_act;

	
	if (act_id > DB_MAX_THREADID)
	    return(THR_ACT_NULL);
	if (queue_first(&task->thr_acts) == 0)
	    return(THR_ACT_NULL);
	queue_iterate(&task->thr_acts, thr_act, thread_act_t, thr_acts) {
	    if (act_id-- <= 0)
		return(thr_act);
	}
	return(THR_ACT_NULL);
}

/*
 * get next parameter from a command line, and check it as a valid
 * thread address
 */
boolean_t
db_get_next_act(
	thread_act_t	*actp,
	int		position)
{
	db_expr_t	value;
	thread_act_t	thr_act;

	*actp = THR_ACT_NULL;
	if (db_expression(&value)) {
	    thr_act = (thread_act_t) value;
	    if (!db_check_act_address_valid(thr_act)) {
		db_flush_lex();
		return(FALSE);
	    }
	} else if (position <= 0) {
	    thr_act = db_default_act;
	} else
	    return(FALSE);
	*actp = thr_act;
	return(TRUE);
}

/*
 * check the default thread is still valid
 *	( it is called in entering DDB session )
 */
void
db_init_default_act(void)
{
	if (db_lookup_act(db_default_act) < 0) {
	    db_default_act = THR_ACT_NULL;
	    db_default_task = TASK_NULL;
	} else
	    db_default_task = db_default_act->task;
}

/*
 * set or get default thread which is used when /t or :t option is specified
 * in the command line
 */
int
db_set_default_act(
	struct db_variable	*vp,
	db_expr_t		*valuep,
	int			flag,
	db_var_aux_param_t	ap)			/* unused */
{
	thread_act_t	thr_act;
	int		task_id;
	int		act_id;

	if (flag == DB_VAR_SHOW) {
	    db_printf("%#n", db_default_act);
	    task_id = db_lookup_task(db_default_task);
	    if (task_id != -1) {
		act_id = db_lookup_act(db_default_act);
		if (act_id != -1) {
		    db_printf(" (task%d.%d)", task_id, act_id);
		}
	    }
	    return(0);
	}

	if (flag != DB_VAR_SET) {
	    *valuep = (db_expr_t) db_default_act;
	    return(0);
	}
	thr_act = (thread_act_t) *valuep;
	if (thr_act != THR_ACT_NULL && !db_check_act_address_valid(thr_act))
	    db_error(0);
	    /* NOTREACHED */
	db_default_act = thr_act;
	if (thr_act)
		db_default_task = thr_act->task;
	return(0);
}

/*
 * convert $taskXXX[.YYY] type DDB variable to task or thread address
 */
int
db_get_task_act(
	struct db_variable	*vp,
	db_expr_t		*valuep,
	int			flag,
	db_var_aux_param_t	ap)
{
	task_t	 		task;
	thread_act_t		thr_act;
	int	 		task_id;

	if (flag == DB_VAR_SHOW) {
	    db_printf("%#n", db_default_task);
	    task_id = db_lookup_task(db_default_task);
	    if (task_id != -1)
		db_printf(" (task%d)", task_id);
	    return(0);
	}

	if (flag != DB_VAR_GET) {
	    db_error("Cannot set to $task variable\n");
	    /* NOTREACHED */
	}
	if ((task = db_lookup_task_id(ap->suffix[0])) == TASK_NULL) {
	    db_printf("no such task($task%d)\n", ap->suffix[0]);
	    db_error(0);
	    /* NOTREACHED */
	}
	if (ap->level <= 1) {
	    *valuep = (db_expr_t) task;
	    return(0);
	}
	if ((thr_act = db_lookup_act_id(task, ap->suffix[1])) == THR_ACT_NULL){
	    db_printf("no such thr_act($task%d.%d)\n", 
					ap->suffix[0], ap->suffix[1]);
	    db_error(0);
	    /* NOTREACHED */
	}
	*valuep = (db_expr_t) thr_act;
	return(0);
}
