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

#ifndef	_DDB_DB_PRINT_H_
#define	_DDB_DB_PRINT_H_

#include <mach/boolean.h>
#include <machine/db_machdep.h>

/* Prototypes for functions exported by this module.
 */
void db_show_regs(
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char		*modif);

void db_show_all_acts(
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char *		modif);

void db_show_one_act(
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char *		modif);

void db_show_one_task(
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char *		modif);

void db_show_shuttle(
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char *		modif);

void db_show_port_id(
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char *		modif);

void db_show_port_id(
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char *		modif);

void db_show_one_space(
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char *		modif);

void db_show_all_spaces(
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char *		modif);

void db_sys(void);

int db_port_kmsg_count(
	ipc_port_t	port);

db_addr_t db_task_from_space(
	ipc_space_t	space,
	int		*task_id);

void db_show_one_spinlock(
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char *		modif);

void db_show_one_mutex(
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char *		modif);

void db_show_subsystem(
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char *		modif);

void db_show_runq(
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char *		modif);

#endif	/* !_DDB_DB_PRINT_H_ */
