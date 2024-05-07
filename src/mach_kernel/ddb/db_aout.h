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

/*
 * Symbol table routines for a.out format files.
 */

#ifndef	_DDB_DB_AOUT_H_
#define	_DDB_DB_AOUT_H_

#include <machine/db_machdep.h>		/* data types */
#include <ddb/db_sym.h>			/* db_symtab_t */

boolean_t X_db_sym_init(
	char *	symtab,
	char *	esymtab,
	char *	name,
	char *	task_addr);

db_sym_t X_db_lookup(
	db_symtab_t	*stab,
	char *		symstr);

int X_db_lookup_incomplete(
	db_symtab_t	*stab,
	char *		symstr,
	char **		name,
	int		*len,
	int		*toadd);

int X_db_print_completion(
	db_symtab_t	*stab,
	char *		symstr);

db_sym_t X_db_search_symbol(
	db_symtab_t	*symtab,
	db_addr_t	off,
	db_strategy_t	strategy,
	db_expr_t	*diffp);		/* in/out */

void X_db_symbol_values(
	db_sym_t	sym,
	char		**namep,
	db_expr_t	*valuep);

db_sym_t X_db_search_by_addr(
	db_symtab_t	*stab,
	unsigned	addr,
	char		**file,
	char		**func,
	int 	 	*line,
	unsigned	*diff,
	int 		*args);

boolean_t X_db_line_at_pc(
	db_symtab_t	*stab,
	db_sym_t	sym,
	char		**file,
	int		*line,
	db_expr_t	pc);

void X_db_init(void);

#endif	/* !_DDB_DB_AOUT_H_ */
