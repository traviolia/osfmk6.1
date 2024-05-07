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

#ifndef	_DDB_DB_EXAMINE_H_
#define	_DDB_DB_EXAMINE_H_

#include <machine/db_machdep.h>
#include <kern/task.h>

/* Prototypes for functions exported by this module.
 */

void db_examine_cmd(
	db_expr_t	addr,
	int		have_addr,
	db_expr_t	count,
	char *		modif);

void db_examine_forward(
	db_expr_t	addr,
	int		have_addr,
	db_expr_t	count,
	char *		modif);

void db_examine_backward(
	db_expr_t	addr,
	int		have_addr,
	db_expr_t	count,
	char *		modif);

void db_examine(
	db_addr_t	addr,
	char *		fmt,	/* format string */
	int		count,	/* repeat count */
	task_t		task);

void db_print_cmd(void);

void db_print_loc_and_inst(
	db_addr_t	loc,
	task_t		task);

void db_search_cmd(void);

void db_search(
	db_addr_t	addr,
	int		size,
	db_expr_t	value,
	db_expr_t	mask,
	unsigned int	count,
	task_t		task);

#endif	/* !_DDB_DB_EXAMINE_H_ */
