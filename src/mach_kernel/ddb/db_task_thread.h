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
 * Revision 2.2  91/10/09  16:03:18  af
 * 	 Revision 2.1.3.1  91/10/05  13:08:07  jeffreyh
 * 	 	Created for task/thread handling.
 * 	 	[91/08/29            tak]
 * 
 * Revision 2.1.3.1  91/10/05  13:08:07  jeffreyh
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

#ifndef _DDB_DB_TASK_THREAD_H_
#define _DDB_DB_TASK_THREAD_H_

#include <kern/task.h>
#include <kern/thread.h>
#include <ddb/db_variables.h>		/* For db_var_aux_param_t */

/*
 * On behalf of kernel-loaded tasks, distinguish between current task
 * (=> symbol table) and current address space (=> where [e.g.]
 * breakpoints are set).  From ddb's perspective, kernel-loaded tasks
 * can retain their own symbol tables, but share the kernel's address
 * space.
 */
#define db_current_task()						\
		((current_act())? current_act()->task: TASK_NULL)
#define db_current_space()						\
		((current_act() && !current_act()->kernel_loaded)?\
			current_act()->task: TASK_NULL)
#define db_target_space(thr_act, user_space)				\
		((!(user_space) || ((thr_act) && (thr_act)->kernel_loaded))?\
			TASK_NULL:					\
			(thr_act)? 					\
				(thr_act)->task: db_current_space())
#define db_is_current_space(task) 					\
		((task) == TASK_NULL || (task) == db_current_space())

extern task_t		db_default_task;	/* default target task */
extern thread_act_t	db_default_act;		/* default target thr_act */


/* Prototypes for functions exported by this module.
 */

int db_lookup_act(thread_act_t target_act);

int db_lookup_task(task_t target_task);

int db_lookup_task_act(
	task_t		task,
	thread_act_t		target_act);

boolean_t db_check_act_address_valid(thread_act_t thr_act);

boolean_t db_get_next_act(
	thread_act_t		*actp,
	int		position);

void db_init_default_act(void);

int db_set_default_act(
	struct db_variable	*vp,
	db_expr_t		*valuep,
	int			flag,
	db_var_aux_param_t	ap);

int db_get_task_act(
	struct db_variable	*vp,
	db_expr_t		*valuep,
	int			flag,
	db_var_aux_param_t	ap);

#endif  /* !_DDB_DB_TASK_THREAD_H_ */
