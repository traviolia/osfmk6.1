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
 * Revision 2.6  91/10/09  16:02:45  af
 * 	 Revision 2.5.1.1  91/10/05  13:07:39  jeffreyh
 * 	 	Added macro definitions of db_find_task_sym_and_offset(),
 * 	 	  db_find_xtrn_task_sym_and_offset(), db_search_symbol().
 * 	 	[91/08/29            tak]
 * 
 * Revision 2.5.1.1  91/10/05  13:07:39  jeffreyh
 * 	Added macro definitions of db_find_task_sym_and_offset(),
 * 	  db_find_xtrn_task_sym_and_offset(), db_search_symbol().
 * 	[91/08/29            tak]
 * 
 * Revision 2.5  91/07/31  17:31:49  dbg
 * 	Add map pointer and storage for name to db_symtab_t.
 * 	[91/07/30  16:45:08  dbg]
 * 
 * Revision 2.4  91/05/14  15:36:08  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:07:12  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  16:19:27  mrt]
 * 
 * Revision 2.2  90/08/27  21:52:39  dbg
 * 	Changed type of db_sym_t to char * - it's a better type for an
 * 	opaque pointer.
 * 	[90/08/22            dbg]
 * 
 * 	Created.
 * 	[90/08/19            af]
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
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	8/90
 */

#ifndef	_DDB_DB_SYM_H_
#define	_DDB_DB_SYM_H_

#include <mach/boolean.h>
#include <mach/machine/vm_types.h>
#include <machine/db_machdep.h>
#include <kern/task.h>

/*
 * This module can handle multiple symbol tables
 */
#define	SYMTAB_NAME_LEN	32

typedef struct {
	char		*start;		/* symtab location */
	char		*end;
	char		*private;	/* optional machdep pointer */
	char		*map_pointer;	/* symbols are for this map only,
					   if not null */
	char		name[SYMTAB_NAME_LEN];
					/* symtab name */
	unsigned long	minsym;		/* lowest symbol value */
	unsigned long	maxsym;		/* highest symbol value */
	boolean_t	sorted;		/* is this table sorted ? */
} db_symtab_t;

extern db_symtab_t	*db_last_symtab; /* where last symbol was found */

/*
 * Symbol representation is specific to the symtab style:
 * BSD compilers use dbx' nlist, other compilers might use
 * a different one
 */
typedef	char *		db_sym_t;	/* opaque handle on symbols */
#define	DB_SYM_NULL	((db_sym_t)0)

/*
 * Non-stripped symbol tables will have duplicates, for instance
 * the same string could match a parameter name, a local var, a
 * global var, etc.
 * We are most concern with the following matches.
 */
typedef int		db_strategy_t;	/* search strategy */

#define	DB_STGY_ANY	0			/* anything goes */
#define DB_STGY_XTRN	1			/* only external symbols */
#define DB_STGY_PROC	2			/* only procedures */

extern boolean_t	db_qualify_ambiguous_names;
					/* if TRUE, check across symbol tables
					 * for multiple occurrences of a name.
					 * Might slow down quite a bit */

extern unsigned int	db_maxoff;

/* Prototypes for functions exported by this module.
 */
boolean_t db_add_symbol_table(
	char		*start,
	char		*end,
	char		*name,
	char		*ref,
	char		*map_pointer,
	unsigned long	minsym,
	unsigned long	maxsym,
	boolean_t	sorted);

boolean_t db_value_of_name(
	char		*name,
	db_expr_t	*valuep);

db_sym_t db_lookup(char *symstr);

char * db_get_sym(
	db_expr_t	* off);

db_sym_t db_sym_parse_and_lookup(
	int	(*func)(db_symtab_t *,
			char *,
			char *,
			int,
			db_sym_t*,
			char **,
			int *),
	db_symtab_t	*symtab,
	char		*symstr);

int db_sym_parse_and_lookup_incomplete(
	int	(*func)(db_symtab_t *,
			char *,
			char *,
			int,
			db_sym_t*,
			char **,
			int *),
	db_symtab_t	*symtab,
	char		*symstr,
	char		**name,
	int		*len,
	int		*toadd);

int db_sym_parse_and_print_completion(
	int	(*func)(db_symtab_t *,
			char *),
	db_symtab_t	*symtab,
	char		*symstr);

db_sym_t db_search_task_symbol(
	register db_addr_t	val,
	db_strategy_t		strategy,
	db_expr_t		*offp,
	task_t			task);

db_sym_t db_search_task_symbol_and_line(
	register db_addr_t	val,
	db_strategy_t		strategy,
	db_expr_t		*offp,
	char			**filenamep,
	int			*linenump,
	task_t			task,
	int			*argsp);

void db_symbol_values(
	db_sym_t	sym,
	char		**namep,
	db_expr_t	*valuep);

void db_task_printsym(
	db_expr_t	off,
	db_strategy_t	strategy,
	task_t		task);

void db_printsym(
	db_expr_t	off,
	db_strategy_t	strategy);

boolean_t db_line_at_pc(
	db_sym_t	sym,
	char		**filename,
	int		*linenum,
	db_expr_t	pc);

void qsort(
	char	*table,
	int	nbelts,
	int	eltsize,
	int	(*compfun)(char *, char *));

void qsort_limit_search(
	char	*target,
	char	**start,
	char	**end,
	int	eltsize,
	int	(*compfun)(char *, char *));

void db_sym_print_completion(
	db_symtab_t *stab,
	char *name,
	int function,
	char *fname,
	int line);

void db_print_completion(
	char *symstr);

int db_lookup_incomplete(
	char *symstr,
	int symlen);

void ddb_init(void);

void db_machdep_init(void);

/* Some convenience macros.
 */
#define db_find_sym_and_offset(val,namep,offp)	\
	db_symbol_values(db_search_symbol(val,DB_STGY_ANY,offp),namep,0)
					/* find name&value given approx val */

#define db_find_xtrn_sym_and_offset(val,namep,offp)	\
	db_symbol_values(db_search_symbol(val,DB_STGY_XTRN,offp),namep,0)
					/* ditto, but no locals */

#define db_find_task_sym_and_offset(val,namep,offp,task)	\
	db_symbol_values(db_search_task_symbol(val,DB_STGY_ANY,offp,task),  \
			 namep, 0)	/* find name&value given approx val */

#define db_find_xtrn_task_sym_and_offset(val,namep,offp,task)	\
	db_symbol_values(db_search_task_symbol(val,DB_STGY_XTRN,offp,task), \
			 namep,0)	/* ditto, but no locals */

#define db_search_symbol(val,strgy,offp)	\
	db_search_task_symbol(val,strgy,offp,0)
					/* find symbol in current task */

#endif	/* !_DDB_DB_SYM_H_ */
