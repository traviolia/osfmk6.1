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
 * Revision 2.11.3.1  92/03/03  16:13:20  jeffreyh
 * 	Pick up changes from TRUNK
 * 	[92/02/26  10:58:37  jeffreyh]
 *
 * Revision 2.12  92/02/19  16:46:24  elf
 * 	Removed one of the many user-unfriendlinesses.
 * 	[92/02/10  17:48:25  af]
 * 
 * Revision 2.11  91/11/12  11:50:24  rvb
 * 	Fixed db_delete_cmd so that just "d" works in user space.
 * 	[91/10/31            rpd]
 * 	Fixed db_delete_thread_breakpoint for zero task_thd.
 * 	[91/10/30            rpd]
 * 
 * Revision 2.10  91/10/09  15:57:41  af
 * 	Supported thread-oriented break points.
 * 	[91/08/29            tak]
 * 
 * Revision 2.9  91/07/09  23:15:39  danner
 *	Conditionalized db_map_addr to work right on the luna. Used a
 *	ifdef luna88k. This is evil, and needs to be fixed.
 *	[91/07/08            danner]
 *
 * Revision 2.2  91/04/10  22:54:50  mbj
 * 	Grabbed 3.0 copyright/disclaimer since ddb comes from 3.0.
 * 	[91/04/09            rvb]
 * 
 * Revision 2.7  91/02/05  17:06:00  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  16:17:01  mrt]
 * 
 * Revision 2.6  91/01/08  15:09:03  rpd
 * 	Added db_map_equal, db_map_current, db_map_addr.
 * 	[90/11/10            rpd]
 * 
 * Revision 2.5  90/11/05  14:26:32  rpd
 * 	Initialize db_breakpoints_inserted to TRUE.
 * 	[90/11/04            rpd]
 * 
 * Revision 2.4  90/10/25  14:43:33  rwd
 * 	Added map field to breakpoints.
 * 	Added map argument to db_set_breakpoint, db_delete_breakpoint,
 * 	db_find_breakpoint.  Added db_find_breakpoint_here.
 * 	[90/10/18            rpd]
 * 
 * Revision 2.3  90/09/28  16:57:07  jsb
 * 	Fixed db_breakpoint_free.
 * 	[90/09/18            rpd]
 * 
 * Revision 2.2  90/08/27  21:49:53  dbg
 * 	Reflected changes in db_printsym()'s calling seq.
 * 	[90/08/20            af]
 * 	Clear breakpoints only if inserted.
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
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date:	7/90
 */

/*
 * Breakpoints.
 */
#include <mach/boolean.h>
#include <machine/db_machdep.h>
#include <ddb/db_lex.h>
#include <ddb/db_break.h>
#include <ddb/db_access.h>
#include <ddb/db_sym.h>
#include <ddb/db_variables.h>
#include <ddb/db_command.h>
#include <ddb/db_cond.h>
#include <ddb/db_expr.h>
#include <ddb/db_output.h>		/* For db_printf() */
#include <ddb/db_task_thread.h>


#define	NBREAKPOINTS	100
#define NTHREAD_LIST	(NBREAKPOINTS*3)



struct db_breakpoint	db_break_table[NBREAKPOINTS];
db_breakpoint_t		db_next_free_breakpoint = &db_break_table[0];
db_breakpoint_t		db_free_breakpoints = 0;
db_breakpoint_t		db_breakpoint_list = 0;

static struct db_thread_breakpoint	db_thread_break_list[NTHREAD_LIST];
static db_thread_breakpoint_t		db_free_thread_break_list = 0;
static boolean_t			db_thread_break_init = FALSE;
static int				db_breakpoint_number = 0;



/* Prototypes for functions local to this file.  XXX -- should be static!
 */
static int db_add_thread_breakpoint(
	register db_breakpoint_t	bkpt,
	unsigned			task_thd,
	int				count,
	boolean_t			task_bpt);

static int db_delete_thread_breakpoint(
	register db_breakpoint_t	bkpt,
	unsigned			task_thd);

static db_thread_breakpoint_t db_find_thread_breakpoint(
	db_breakpoint_t	bkpt,
	thread_act_t	thr_act);

static void db_force_delete_breakpoint(
	db_breakpoint_t	bkpt,
	unsigned	task_thd,
	boolean_t	is_task);

db_breakpoint_t db_breakpoint_alloc(void);

void db_breakpoint_free(register db_breakpoint_t bkpt);

void db_delete_breakpoint(
	task_t		task,
	db_addr_t	addr,
	unsigned	task_thd);

void db_list_breakpoints(void);



db_breakpoint_t
db_breakpoint_alloc(void)
{
	register db_breakpoint_t	bkpt;

	if ((bkpt = db_free_breakpoints) != 0) {
	    db_free_breakpoints = bkpt->link;
	    return (bkpt);
	}
	if (db_next_free_breakpoint == &db_break_table[NBREAKPOINTS]) {
	    db_printf("All breakpoints used.\n");
	    return (0);
	}
	bkpt = db_next_free_breakpoint;
	db_next_free_breakpoint++;

	return (bkpt);
}

void
db_breakpoint_free(register db_breakpoint_t bkpt)
{
	bkpt->link = db_free_breakpoints;
	db_free_breakpoints = bkpt;
}

static int
db_add_thread_breakpoint(
	register db_breakpoint_t	bkpt,
	unsigned			task_thd,
	int				count,
	boolean_t			task_bpt)
{
	register db_thread_breakpoint_t tp;

	if (db_thread_break_init == FALSE) {
	    for (tp = db_thread_break_list; 
		tp < &db_thread_break_list[NTHREAD_LIST-1]; tp++)
		tp->tb_next = tp+1;
	    tp->tb_next = 0;
	    db_free_thread_break_list = db_thread_break_list;
	    db_thread_break_init = TRUE;
	}
	if (db_free_thread_break_list == 0)
	    return (-1);
	tp = db_free_thread_break_list;
	db_free_thread_break_list = tp->tb_next;
	tp->tb_is_task = task_bpt;
	tp->tb_task_thd = task_thd;
	tp->tb_count = count;
	tp->tb_init_count = count;
	tp->tb_cond = 0;
	tp->tb_number = ++db_breakpoint_number;
	tp->tb_next = bkpt->threads;
	bkpt->threads = tp;
	return(0);
}

static int
db_delete_thread_breakpoint(
	register db_breakpoint_t	bkpt,
	unsigned			task_thd)
{
	register db_thread_breakpoint_t tp;
	register db_thread_breakpoint_t *tpp;

	if (task_thd == 0) {
	    /* delete all the thread-breakpoints */

	    for (tpp = &bkpt->threads; (tp = *tpp) != 0; tpp = &tp->tb_next)
		db_cond_free(tp);

	    *tpp = db_free_thread_break_list;
	    db_free_thread_break_list = bkpt->threads;
	    bkpt->threads = 0;
	    return 0;
	} else {
	    /* delete the specified thread-breakpoint */

	    for (tpp = &bkpt->threads; (tp = *tpp) != 0; tpp = &tp->tb_next)
		if (tp->tb_task_thd == task_thd) {
		    db_cond_free(tp);
		    *tpp = tp->tb_next;
		    tp->tb_next = db_free_thread_break_list;
		    db_free_thread_break_list = tp;
		    return 0;
		}

	    return -1;	/* not found */
	}
}

static db_thread_breakpoint_t
db_find_thread_breakpoint(
	db_breakpoint_t	bkpt,
	thread_act_t	thr_act)
{
	register db_thread_breakpoint_t tp;
	register task_t task =
			(thr_act == THR_ACT_NULL || thr_act->kernel_loaded)
					? TASK_NULL : thr_act->task;

	for (tp = bkpt->threads; tp; tp = tp->tb_next) {
	    if (tp->tb_is_task) {
		if (tp->tb_task_thd == (unsigned)task)
		    break;
		continue;
	    }
	    if (tp->tb_task_thd == (unsigned)thr_act || tp->tb_task_thd == 0)
		break;
	}
	return(tp);
}

db_thread_breakpoint_t
db_find_thread_breakpoint_here(
	task_t		task,
	db_addr_t	addr)
{
	db_breakpoint_t bkpt;

	bkpt = db_find_breakpoint(task, (db_addr_t)addr);
	if (bkpt == 0)
	    return(0);
	return(db_find_thread_breakpoint(bkpt, current_act()));
}

db_thread_breakpoint_t
db_find_breakpoint_number(
	int		num,
	db_breakpoint_t *bkptp)
{
	register db_thread_breakpoint_t tp;
	register db_breakpoint_t bkpt;

	for (bkpt = db_breakpoint_list; bkpt != 0; bkpt = bkpt->link) {
	    for (tp = bkpt->threads; tp; tp = tp->tb_next) {
		if (tp->tb_number == num) {
		    if (bkptp)
			*bkptp = bkpt;
		    return(tp);
		}
	    }
	}
	return(0);
}

static void
db_force_delete_breakpoint(
	db_breakpoint_t	bkpt,
	unsigned	task_thd,
	boolean_t	is_task)
{
	db_printf("deleted a stale breakpoint at ");
	if (bkpt->task == TASK_NULL || db_lookup_task(bkpt->task) >= 0)
	   db_task_printsym(bkpt->address, DB_STGY_PROC, bkpt->task);
	else
	   db_printf("0x%x", bkpt->address);
	if (bkpt->task)
	   db_printf(" in task %x", bkpt->task);
	if (task_thd)
	   db_printf(" for %s %x", (is_task)? "task": "thr_act", task_thd);
	db_printf("\n");
	db_delete_thread_breakpoint(bkpt, task_thd);
}

void
db_check_breakpoint_valid(void)
{
	register db_thread_breakpoint_t tbp, tbp_next;
	register db_breakpoint_t bkpt, *bkptp;

	bkptp = &db_breakpoint_list;
	for (bkpt = *bkptp; bkpt; bkpt = *bkptp) {
	    if (bkpt->task != TASK_NULL) {
		if (db_lookup_task(bkpt->task) < 0) {
		    db_force_delete_breakpoint(bkpt, 0, FALSE);
		    *bkptp = bkpt->link;
		    db_breakpoint_free(bkpt);
		    continue;
		}
	    } else {
		for (tbp = bkpt->threads; tbp; tbp = tbp_next) {
		    tbp_next = tbp->tb_next;
		    if (tbp->tb_task_thd == 0)
			continue;
		    if ((tbp->tb_is_task && 
			 db_lookup_task((task_t)(tbp->tb_task_thd)) < 0) ||
			(!tbp->tb_is_task && 
			 db_lookup_act((thread_act_t)(tbp->tb_task_thd)) < 0)) {
			db_force_delete_breakpoint(bkpt, 
					tbp->tb_task_thd, tbp->tb_is_task);
		    }
		}
		if (bkpt->threads == 0) {
		    db_put_task_value(bkpt->address, BKPT_SIZE,
				 bkpt->bkpt_inst, bkpt->task);
		    *bkptp = bkpt->link;
		    db_breakpoint_free(bkpt);
		    continue;
		}
	    }
	    bkptp = &bkpt->link;
	}
}

void
db_set_breakpoint(
	task_t		task,
	db_addr_t	addr,
	int		count,
	thread_act_t	thr_act,
	boolean_t	task_bpt)
{
	register db_breakpoint_t bkpt;
	db_breakpoint_t alloc_bkpt = 0;
	unsigned task_thd;

	bkpt = db_find_breakpoint(task, addr);
	if (bkpt) {
	    if (thr_act == THR_ACT_NULL
		|| db_find_thread_breakpoint(bkpt, thr_act)) {
		db_printf("Already set.\n");
		return;
	    }
	} else {
	    if (!DB_CHECK_ACCESS(addr, BKPT_SIZE, task)) {
		db_printf("Cannot set break point at %x\n", addr);
		return;
	    }
	    alloc_bkpt = bkpt = db_breakpoint_alloc();
	    if (bkpt == 0) {
		db_printf("Too many breakpoints.\n");
		return;
	    }
	    bkpt->task = task;
	    bkpt->flags = (task && thr_act == THR_ACT_NULL)?
				(BKPT_USR_GLOBAL|BKPT_1ST_SET): 0;
	    bkpt->address = addr;
	    bkpt->threads = 0;
	}
	if (db_breakpoint_list == 0)
	    db_breakpoint_number = 0;
	task_thd = (task_bpt)? (unsigned)(thr_act->task): (unsigned)thr_act;
	if (db_add_thread_breakpoint(bkpt, task_thd, count, task_bpt) < 0) {
	    if (alloc_bkpt)
		db_breakpoint_free(alloc_bkpt);
	    db_printf("Too many thread_breakpoints.\n");
	} else {
	    db_printf("set breakpoint #%x\n", db_breakpoint_number);
	    if (alloc_bkpt) {
		bkpt->link = db_breakpoint_list;
		db_breakpoint_list = bkpt;
	    }
	}
}

void
db_delete_breakpoint(
	task_t		task,
	db_addr_t	addr,
	unsigned	task_thd)
{
	register db_breakpoint_t	bkpt;
	register db_breakpoint_t	*prev;

	for (prev = &db_breakpoint_list; (bkpt = *prev) != 0;
					     prev = &bkpt->link) {
	    if ((bkpt->task == task
		   || (task != TASK_NULL && (bkpt->flags & BKPT_USR_GLOBAL)))
		&& bkpt->address == addr)
		break;
	}
	if (bkpt && (bkpt->flags & BKPT_SET_IN_MEM)) {
	    db_printf("cannot delete it now.\n");
	    return;
	}
	if (bkpt == 0
	    || db_delete_thread_breakpoint(bkpt, task_thd) < 0) {
	    db_printf("Not set.\n");
	    return;
	}
	if (bkpt->threads == 0) {
	    *prev = bkpt->link;
	    db_breakpoint_free(bkpt);
	}
}

db_breakpoint_t
db_find_breakpoint(
	task_t		task,
	db_addr_t	addr)
{
	register db_breakpoint_t	bkpt;

	for (bkpt = db_breakpoint_list; bkpt != 0; bkpt = bkpt->link) {
	    if ((bkpt->task == task
		  || (task != TASK_NULL && (bkpt->flags & BKPT_USR_GLOBAL)))
		&& bkpt->address == addr)
		return (bkpt);
	}
	return (0);
}

boolean_t
db_find_breakpoint_here(
	task_t		task,
	db_addr_t	addr)
{
	register db_breakpoint_t	bkpt;

	for (bkpt = db_breakpoint_list; bkpt != 0; bkpt = bkpt->link) {
	    if ((bkpt->task == task
		   || (task != TASK_NULL && (bkpt->flags & BKPT_USR_GLOBAL)))
                && bkpt->address == addr)
		return(TRUE);
	    if ((bkpt->flags & BKPT_USR_GLOBAL) == 0 &&
		  DB_PHYS_EQ(task, addr, bkpt->task, bkpt->address))
		return (TRUE);
	}
	return(FALSE);
}

boolean_t	db_breakpoints_inserted = TRUE;

void
db_set_breakpoints(void)
{
	register db_breakpoint_t bkpt;
	register task_t	task;
	int		inst;
	thread_act_t	cur_act = current_act();
	task_t		cur_task =
				(cur_act && !cur_act->kernel_loaded) ?
					cur_act->task : TASK_NULL;

	if (!db_breakpoints_inserted) {
	    for (bkpt = db_breakpoint_list; bkpt != 0; bkpt = bkpt->link) {
		if (bkpt->flags & BKPT_SET_IN_MEM)
		    continue;
		task = bkpt->task;
		if (bkpt->flags & BKPT_USR_GLOBAL) {
		    if ((bkpt->flags & BKPT_1ST_SET) == 0) {
		        if (cur_task == TASK_NULL)
			    continue;
		        task = cur_task;
		    } else
			bkpt->flags &= ~BKPT_1ST_SET;
		}
		if (DB_CHECK_ACCESS(bkpt->address, BKPT_SIZE, task)) {
		    inst = db_get_task_value(bkpt->address, BKPT_SIZE, FALSE,
								task);
		    if (inst == BKPT_SET(inst))
			continue;
		    bkpt->bkpt_inst = inst;
		    db_put_task_value(bkpt->address,
				BKPT_SIZE,
				BKPT_SET(bkpt->bkpt_inst), task);
		    bkpt->flags |= BKPT_SET_IN_MEM;
		} else {
		    db_printf("Warning: cannot set breakpoint at %x ", 
				bkpt->address);
		    if (task)
			db_printf("in task %x\n", task);
		    else
			db_printf("in kernel space\n");
		}
	    }
	    db_breakpoints_inserted = TRUE;
	}
}

void
db_clear_breakpoints(void)
{
	register db_breakpoint_t bkpt, *bkptp;
	register task_t	 task;
	int      inst;
	thread_act_t	 cur_act = current_act();
	task_t	 cur_task = (cur_act && !cur_act->kernel_loaded) ?
			cur_act->task: TASK_NULL;

	if (db_breakpoints_inserted) {
	    bkptp = &db_breakpoint_list;
	    for (bkpt = *bkptp; bkpt; bkpt = *bkptp) {
		task = bkpt->task;
		if (bkpt->flags & BKPT_USR_GLOBAL) {
		    if (cur_task == TASK_NULL) {
			bkptp = &bkpt->link;
			continue;
		    }
		    task = cur_task;
		}
		if ((bkpt->flags & BKPT_SET_IN_MEM)
		    && DB_CHECK_ACCESS(bkpt->address, BKPT_SIZE, task)) {
		    inst = db_get_task_value(bkpt->address, BKPT_SIZE, FALSE, 
								task);
		    if (inst != BKPT_SET(inst)) {
			if (bkpt->flags & BKPT_USR_GLOBAL) {
			    bkptp = &bkpt->link;
			    continue;
			}
			db_force_delete_breakpoint(bkpt, 0, FALSE);
			*bkptp = bkpt->link;
		        db_breakpoint_free(bkpt);
			continue;
		    }
		    db_put_task_value(bkpt->address, BKPT_SIZE,
				 bkpt->bkpt_inst, task);
		    bkpt->flags &= ~BKPT_SET_IN_MEM;
		}
		bkptp = &bkpt->link;
	    }
	    db_breakpoints_inserted = FALSE;
	}
}

/*
 * Set a temporary breakpoint.
 * The instruction is changed immediately,
 * so the breakpoint does not have to be on the breakpoint list.
 */
db_breakpoint_t
db_set_temp_breakpoint(
	task_t		task,
	db_addr_t	addr)
{
	register db_breakpoint_t	bkpt;

	bkpt = db_breakpoint_alloc();
	if (bkpt == 0) {
	    db_printf("Too many breakpoints.\n");
	    return 0;
	}
	bkpt->task = task;
	bkpt->address = addr;
	bkpt->flags = BKPT_TEMP;
	bkpt->threads = 0;
	if (db_add_thread_breakpoint(bkpt, 0, 1, FALSE) < 0) {
	    if (bkpt)
		db_breakpoint_free(bkpt);
	    db_printf("Too many thread_breakpoints.\n");
	    return 0;
	}
	bkpt->bkpt_inst = db_get_task_value(bkpt->address, BKPT_SIZE, 
						FALSE, task);
	db_put_task_value(bkpt->address, BKPT_SIZE, 
				BKPT_SET(bkpt->bkpt_inst), task);
	return bkpt;
}

void
db_delete_temp_breakpoint(
	task_t		task,
	db_breakpoint_t	bkpt)
{
	db_put_task_value(bkpt->address, BKPT_SIZE, bkpt->bkpt_inst, task);
	db_delete_thread_breakpoint(bkpt, 0);
	db_breakpoint_free(bkpt);
}

/*
 * List breakpoints.
 */
void
db_list_breakpoints(void)
{
	register db_breakpoint_t	bkpt;

	if (db_breakpoint_list == 0) {
	    db_printf("No breakpoints set\n");
	    return;
	}

	db_printf(" No  Space ThrAct***      Cnt  Address(Cond)\n");
	for (bkpt = db_breakpoint_list;
	     bkpt != 0;
	     bkpt = bkpt->link)
	{
	    register 	db_thread_breakpoint_t tp;
	    int		task_id;
	    int		act_id;

	    if (bkpt->threads) {
		for (tp = bkpt->threads; tp; tp = tp->tb_next) {
		    db_printf("%3d  ", tp->tb_number);
		    if (bkpt->flags & BKPT_USR_GLOBAL)
			db_printf("user     ");
		    else if (bkpt->task == TASK_NULL)
			db_printf("kernel   ");
		    else if ((task_id = db_lookup_task(bkpt->task)) < 0)
			db_printf("%08x ", bkpt->task);
		    else
			db_printf("task%-3d  ", task_id);
		    if (tp->tb_task_thd == 0) {
			db_printf("all         ");
		    } else {
			if (tp->tb_is_task) {
			    task_id = db_lookup_task((task_t)(tp->tb_task_thd));
			    if (task_id < 0)
				db_printf("%08x    ", tp->tb_task_thd);
			    else
				db_printf("task%03d     ", task_id);
			} else {
			    thread_act_t thd = (thread_act_t)(tp->tb_task_thd);
			    task_id = db_lookup_task(thd->task);
			    act_id = db_lookup_task_act(thd->task, thd);
			    if (task_id < 0 || act_id < 0)
				db_printf("%08x    ", tp->tb_task_thd);
			    else	
				db_printf("task%03d.%-3d ", task_id, act_id);
			}
		    }
	    	    db_printf("%3d  ", tp->tb_init_count);
		    db_task_printsym(bkpt->address, DB_STGY_PROC, bkpt->task);
		    if (tp->tb_cond > 0) {
			db_printf("(");
			db_cond_print(tp);
			db_printf(")");
		    }
		    db_printf("\n");
		}
	    } else {
		if (bkpt->task == TASK_NULL)
		    db_printf("  ?  kernel   ");
		else
		    db_printf("%8x ", bkpt->task);
		db_printf("(?)              ");
		db_task_printsym(bkpt->address, DB_STGY_PROC, bkpt->task);
		db_printf("\n");
	    }
	}
}

/* Delete breakpoint */
void
db_delete_cmd(void)
{
	register n;
	thread_act_t 	 thr_act;
	unsigned task_thd;
	boolean_t user_global = FALSE;
	boolean_t task_bpt = FALSE;
	boolean_t user_space = FALSE;
	boolean_t thd_bpt = FALSE;
	db_expr_t addr;
	int t;
	
	t = db_read_token();
	if (t == tSLASH) {
	    t = db_read_token();
	    if (t != tIDENT) {
		db_printf("Bad modifier \"%s\"\n", db_tok_string);
		db_error(0);
	    }
	    user_global = db_option(db_tok_string, 'U');
	    user_space = (user_global)? TRUE: db_option(db_tok_string, 'u');
	    task_bpt = db_option(db_tok_string, 'T');
	    thd_bpt = db_option(db_tok_string, 't');
	    if (task_bpt && user_global)
		db_error("Cannot specify both 'T' and 'U' option\n");
	    t = db_read_token();
	}
	if (t == tHASH) {
	    db_thread_breakpoint_t tbp;
	    db_breakpoint_t bkpt;

	    if (db_read_token() != tNUMBER) {
		db_printf("Bad break point number #%s\n", db_tok_string);
		db_error(0);
	    }
	    if ((tbp = db_find_breakpoint_number(db_tok_number, &bkpt)) == 0) {
	        db_printf("No such break point #%d\n", db_tok_number);
	        db_error(0);
	    }
	    db_delete_breakpoint(bkpt->task, bkpt->address, tbp->tb_task_thd);
	    return;
	}
	db_unread_token(t);
	if (!db_expression(&addr)) {
	    /*
	     *	We attempt to pick up the user_space indication from db_dot,
	     *	so that a plain "d" always works.
	     */
	    addr = (db_expr_t)db_dot;
	    if (!user_space && !DB_VALID_ADDRESS(addr, FALSE))
		user_space = TRUE;
	}
	if (!DB_VALID_ADDRESS(addr, user_space)) {
	    db_printf("Address 0x%x is not in %s space\n", addr, 
			(user_space)? "user": "kernel");
	    db_error(0);
	}
	if (thd_bpt || task_bpt) {
	    for (n = 0; db_get_next_act(&thr_act, n); n++) {
		if (thr_act == THR_ACT_NULL)
		    db_error("No active thr_act\n");
		if (task_bpt) {
		    if (thr_act->task == TASK_NULL)
			db_error("No task\n");
		    task_thd = (unsigned) (thr_act->task);
		} else
		    task_thd = (user_global)? 0: (unsigned) thr_act;
		db_delete_breakpoint(db_target_space(thr_act, user_space),
					(db_addr_t)addr, task_thd);
	    }
	} else {
	    db_delete_breakpoint(db_target_space(THR_ACT_NULL, user_space),
					 (db_addr_t)addr, 0);
	}
}

/* Set breakpoint with skip count */
void
db_breakpoint_cmd(
	db_expr_t	addr,
	int		have_addr,
	db_expr_t	count,
	char *		modif)
{
	register n;
	thread_act_t thr_act;
	boolean_t user_global = db_option(modif, 'U');
	boolean_t task_bpt = db_option(modif, 'T');
	boolean_t user_space;

	if (count == -1)
	    count = 1;

	if (!task_bpt && db_option(modif,'t'))
	  task_bpt = TRUE;

	if (task_bpt && user_global)
	    db_error("Cannot specify both 'T' and 'U'\n");
	user_space = (user_global)? TRUE: db_option(modif, 'u');
	if (user_space && db_access_level < DB_ACCESS_CURRENT)
	    db_error("User space break point is not supported\n");
	if ((!task_bpt || !user_space) &&
	    !DB_VALID_ADDRESS(addr, user_space)) {
	    /* if the user has explicitly specified user space,
	       do not insert a breakpoint into the kernel */
	    if (user_space)
	      db_error("Invalid user space address\n");
	    user_space = TRUE;
	    db_printf("0x%x is in user space\n", addr);
	}
	if (db_option(modif, 't') || task_bpt) {
	    for (n = 0; db_get_next_act(&thr_act, n); n++) {
		if (thr_act == THR_ACT_NULL)
		    db_error("No active thr_act\n");
		if (task_bpt && thr_act->task == TASK_NULL)
		    db_error("No task\n");
		if (db_access_level <= DB_ACCESS_CURRENT && user_space
			 && thr_act->task != db_current_space())
		    db_error("Cannot set break point in inactive user space\n");
		db_set_breakpoint(db_target_space(thr_act, user_space), 
					(db_addr_t)addr, count,
					(user_global)? THR_ACT_NULL: thr_act,
					task_bpt);
	    }
	} else {
	    db_set_breakpoint(db_target_space(THR_ACT_NULL, user_space),
				 (db_addr_t)addr,
				 count, THR_ACT_NULL, FALSE);
	}
}

/* list breakpoints */
void
db_listbreak_cmd(void)
{
	db_list_breakpoints();
}
