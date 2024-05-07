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
 * Revision 2.3  91/07/11  11:00:35  danner
 * 	Copyright Fixes
 * 
 * Revision 2.2  91/07/09  23:16:05  danner
 * 	Broken out of db_run.c because I needed access to the step mode in the 
 * 	machine dependent luna88k debugger code.
 * 	[91/07/09  00:47:31  danner]
 * 
 * Revision 2.2  91/04/11  00:13:39  mbj
 * 	Added to support luna debugger interface. Broken out of db_run.c
 * 	[91/03/18            danner]
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

#ifndef	_DDB_DB_RUN_H_
#define	_DDB_DB_RUN_H_

#include <mach/boolean.h>
#include <machine/db_machdep.h>
#include <kern/task.h>

extern int db_run_mode;

/* modes the system may be running in */

#define	STEP_NONE	0
#define	STEP_ONCE	1
#define	STEP_RETURN	2
#define	STEP_CALLT	3
#define	STEP_CONTINUE	4
#define STEP_INVISIBLE	5
#define	STEP_COUNT	6


/* Prototypes for functions exported by this module.
 */

boolean_t db_stop_at_pc(
	boolean_t	*is_breakpoint,
	task_t		task,
	task_t		space);

void db_restart_at_pc(
	boolean_t	watchpt,
	task_t	  	task);

void db_single_step(
	db_regs_t	*regs,
	task_t	  	task);

void db_single_step_cmd(
	db_expr_t	addr,
	int		have_addr,
	db_expr_t	count,
	char *		modif);

void db_trace_until_call_cmd(
	db_expr_t	addr,
	int		have_addr,
	db_expr_t	count,
	char *		modif);

void db_trace_until_matching_cmd(
	db_expr_t	addr,
	int		have_addr,
	db_expr_t	count,
	char *		modif);

void db_continue_cmd(
	db_expr_t	addr,
	int		have_addr,
	db_expr_t	count,
	char *		modif);

boolean_t db_in_single_step(void);

#endif	/* !_DDB_DB_RUN_H_ */
