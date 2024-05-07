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
 * Revision 2.10.4.1  92/02/18  18:38:53  jeffreyh
 * 	Added db_get_sym(). Simple interface to get symbol names
 * 	knowing the offset.
 * 	[91/12/20            bernadat]
 * 
 * 	Do not look for symbol names if address
 * 	is to small or to large, otherwise get
 * 	random names like INCLUDE_VERSION+??
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.10  91/10/09  16:02:30  af
 * 	 Revision 2.9.2.1  91/10/05  13:07:27  jeffreyh
 * 	 	Changed symbol table name qualification syntax from "xxx:yyy"
 * 	 	  to "xxx::yyy" to allow "file:func:line" in "yyy" part.
 * 		       "db_sym_parse_and_lookup" is also added for "yyy" part parsing.
 * 	 	Replaced db_search_symbol with db_search_task_symbol, and moved
 * 	 	  it to "db_sym.h" as a macro.
 * 	 	Added db_task_printsym, and changed db_printsym to call it.
 * 	 	Added include "db_task_thread.h".
 * 	 	Fixed infinite recursion of db_symbol_values.
 * 	 	[91/08/29            tak]
 * 
 * Revision 2.9.2.1  91/10/05  13:07:27  jeffreyh
 * 	Changed symbol table name qualification syntax from "xxx:yyy"
 * 	  to "xxx::yyy" to allow "file:func:line" in "yyy" part.
 * 	       "db_sym_parse_and_lookup" is also added for "yyy" part parsing.
 * 	Replaced db_search_symbol with db_search_task_symbol, and moved
 * 	  it to "db_sym.h" as a macro.
 * 	Added db_task_printsym, and changed db_printsym to call it.
 * 	Added include "db_task_thread.h".
 * 	Fixed infinite recursion of db_symbol_values.
 * 	[91/08/29            tak]
 * 
 * Revision 2.9  91/07/31  17:31:14  dbg
 * 	Add task pointer and space for string storage to symbol table
 * 	descriptor.
 * 	[91/07/31            dbg]
 * 
 * Revision 2.8  91/07/09  23:16:08  danner
 * 	Changed a printf.
 * 	[91/07/08            danner]
 * 
 * Revision 2.7  91/05/14  15:35:54  mrt
 * 	Correcting copyright
 * 
 * Revision 2.6  91/03/16  14:42:40  rpd
 * 	Changed the default db_maxoff to 4K.
 * 	[91/03/10            rpd]
 * 
 * Revision 2.5  91/02/05  17:07:07  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  16:19:17  mrt]
 * 
 * Revision 2.4  90/10/25  14:44:05  rwd
 * 	Changed db_printsym to print unsigned.
 * 	[90/10/19            rpd]
 * 
 * Revision 2.3  90/09/09  23:19:56  rpd
 * 	Avoid totally incorrect guesses of symbol names for small values.
 * 	[90/08/30  17:39:48  af]
 * 
 * Revision 2.2  90/08/27  21:52:18  dbg
 * 	Removed nlist.h.  Fixed some type declarations.
 * 	Qualifier character is ':'.
 * 	[90/08/20            dbg]
 * 	Modularized symtab info into a new db_symtab_t type.
 * 	Modified db_add_symbol_table  and others accordingly.
 * 	Defined db_sym_t, a new (opaque) type used to represent
 * 	symbols.  This should support all sort of future symtable
 * 	formats. Functions like db_qualify take a db_sym_t now.
 * 	New db_symbol_values() function to explode the content
 * 	of a db_sym_t.
 * 	db_search_symbol() replaces db_find_sym_and_offset(), which is
 * 	now a macro defined in our (new) header file.  This new
 * 	function accepts more restrictive searches, which are
 * 	entirely delegated to the symtab-specific code.
 * 	Accordingly, db_printsym() accepts a strategy parameter.
 * 	New db_line_at_pc() function.
 * 	Renamed misleading db_eqsym into db_eqname.
 * 	[90/08/20  10:47:06  af]
 * 
 * 	Created.
 * 	[90/07/25            dbg]
 * 
 * Revision 2.1  90/07/26  16:43:52  dbg
 * Created.
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

#include <machine/db_machdep.h>
#include <string.h>			/* For strcpy(), strcmp() */
#include <kern/misc_protos.h>		/* For printf() */
#include <ddb/db_aout.h>
#include <ddb/db_sym.h>
#include <ddb/db_task_thread.h>
#include <ddb/db_command.h>
#include <ddb/db_output.h>		/* For db_printf() */

#include <vm/vm_map.h>	/* vm_map_t */

/*
 * Multiple symbol tables
 *
 * mach, bootstrap, name_server, default_pager, unix, 1 spare
 */
#define	MAXNOSYMTABS	6

db_symtab_t	db_symtabs[MAXNOSYMTABS] = {{0,},};
int db_nsymtab = 0;

db_symtab_t	*db_last_symtab;

unsigned int	db_maxoff = 0x4000;
extern		char end;
unsigned int	db_maxval = (unsigned int)&end;
unsigned int	db_minval = 0x1000;

/* Prototypes for functions local to this file.  XXX -- should be static!
 */
static char *db_qualify(
	db_sym_t	esym,
	register char	*symtabname);

boolean_t db_eqname(
	char		*src,
	char		*dst,
	unsigned	c);

boolean_t db_symbol_is_ambiguous(db_sym_t sym);

void db_shorten_filename(char **filenamep);

void qsort_swap(
	register int	*a,
	register int	*b,
	register int	size);

void qsort_rotate(
	register int	*a,
	register int	*b,
	register int	*c,
	register int	size);

void qsort_recur(
	char	*left,
	char	*right,
	int	eltsize,
	int	(*compfun)(char *, char *));

void qsort_checker(
	char	*table,
	int	nbelts,
	int	eltsize,
	int	(*compfun)(char *, char *));

void bubble_sort(
	char	*table,
	int	nbelts,
	int	eltsize,
	int	(*compfun)(char *, char *));


/*
 * Initialization routine for ddb.
 */
void
ddb_init(void)
{
	X_db_init();
	db_machdep_init();
}

/*
 * Add symbol table, with given name, to list of symbol tables.
 */
boolean_t
db_add_symbol_table(
	char		*start,
	char		*end,
	char		*name,
	char		*ref,
	char		*map_pointer,
	unsigned long	minsym,
	unsigned long	maxsym,
	boolean_t	sorted)
{
	register db_symtab_t *st;
	extern vm_map_t kernel_map;

	if (db_nsymtab >= MAXNOSYMTABS)
	    return (FALSE);

	st = &db_symtabs[db_nsymtab];
	st->start = start;
	st->end = end;
	st->private = ref;
	st->map_pointer = (map_pointer == (char *)kernel_map)? 0: map_pointer;
	strcpy(st->name, name);
	st->minsym = minsym;
	st->maxsym = maxsym;
	if (maxsym == 0)
		st->sorted = FALSE;
	else
		st->sorted = sorted;

	db_nsymtab++;

	if (sorted && db_maxval < maxsym)
		db_maxval = maxsym + db_maxoff;

#if	!mips
	/* db_printf fails on the mips on first invocation of
	 * db_add_symbol_table
	 */
	if (sorted) {
		db_printf(
			"[ symbol table range for %s: from 0x%x to 0x%x (sorted) ]\n",
			  st->name, st->minsym, st->maxsym);
	}
	else {
		db_printf(
			"[ symbol table for %s unsorted ]\n",
			st->name);
	}
#endif	/* mips */

	return (TRUE);
}

/*
 *  db_qualify("vm_map", "ux") returns "ux::vm_map".
 *
 *  Note: return value points to static data whose content is
 *  overwritten by each call... but in practice this seems okay.
 */
static char *
db_qualify(
	db_sym_t	sym,
	register char	*symtabname)
{
	char		*symname;
	static char     tmp[256];
	register char	*s;

	X_db_symbol_values(sym, &symname, 0);
	s = tmp;
	while (*s++ = *symtabname++) {
	}
	s[-1] = ':';
	*s++ = ':';
	while (*s++ = *symname++) {
	}
	return tmp;
}


boolean_t
db_eqname(
	char		*src,
	char		*dst,
	unsigned	c)
{
	if (!strcmp(src, dst))
	    return (TRUE);
	if (src[0] == c)
	    return (!strcmp(src+1,dst));
	return (FALSE);
}

boolean_t
db_value_of_name(
	char		*name,
	db_expr_t	*valuep)
{
	db_sym_t	sym;

	sym = db_lookup(name);
	if (sym == DB_SYM_NULL)
	    return (FALSE);
	db_symbol_values(sym, &name, valuep);
	return (TRUE);
}

/*
 * Display list of possible completions for a symbol.
 */
void
db_print_completion(
	char *symstr)
{
	register int i;
	int symtab_start = 0;
	int symtab_end = db_nsymtab;
	register char *cp;
	int nsym = 0;
	char *name = (char *)0;
	int len;
	int toadd;

	/*
	 * Look for, remove, and remember any symbol table specifier.
	 */
	for (cp = symstr; *cp; cp++) {
		if (*cp == ':' && cp[1] == ':') {
			*cp = '\0';
			for (i = 0; i < db_nsymtab; i++) {
				if (! strcmp(symstr, db_symtabs[i].name)) {
					symtab_start = i;
					symtab_end = i + 1;
					break;
				}
			}
			*cp = ':';
			if (i == db_nsymtab)
				return;
			symstr = cp+2;
		}
	}

	/*
	 * Look in the specified set of symbol tables.
	 * Return on first match.
	 */
	for (i = symtab_start; i < symtab_end; i++) {
		if (X_db_print_completion(&db_symtabs[i], symstr))
			break;
	}
}

/*
 * Lookup a (perhaps incomplete) symbol.
 * If the symbol has a qualifier (e.g., ux::vm_map),
 * then only the specified symbol table will be searched;
 * otherwise, all symbol tables will be searched.
 */
int
db_lookup_incomplete(
	char *symstr,
	int symlen)
{
	register int i;
	int symtab_start = 0;
	int symtab_end = db_nsymtab;
	register char *cp;
	int nsym = 0;
	char *name = (char *)0;
	int len;
	int toadd;

	/*
	 * Look for, remove, and remember any symbol table specifier.
	 */
	for (cp = symstr; *cp; cp++) {
		if (*cp == ':' && cp[1] == ':') {
			*cp = '\0';
			for (i = 0; i < db_nsymtab; i++) {
				if (! strcmp(symstr, db_symtabs[i].name)) {
					symtab_start = i;
					symtab_end = i + 1;
					break;
				}
			}
			*cp = ':';
			if (i == db_nsymtab)
				return 0;
			symstr = cp+2;
		}
	}

	/*
	 * Look in the specified set of symbol tables.
	 * Return on first match.
	 */
	for (i = symtab_start; i < symtab_end; i++) {
		nsym = X_db_lookup_incomplete(&db_symtabs[i], symstr,
					      &name, &len, &toadd);
		if (nsym > 0) {
			if (toadd > 0) {
				len = strlen(symstr);
				if (len + toadd >= symlen)
					return 0;
				bcopy(&name[len], &symstr[len], toadd);
				symstr[len + toadd] = '\0';
			}
			break;
		}
	}
	return nsym;
}

/*
 * Lookup a symbol.
 * If the symbol has a qualifier (e.g., ux::vm_map),
 * then only the specified symbol table will be searched;
 * otherwise, all symbol tables will be searched.
 */
db_sym_t
db_lookup(char *symstr)
{
	db_sym_t sp;
	register int i;
	int symtab_start = 0;
	int symtab_end = db_nsymtab;
	register char *cp;

	/*
	 * Look for, remove, and remember any symbol table specifier.
	 */
	for (cp = symstr; *cp; cp++) {
		if (*cp == ':' && cp[1] == ':') {
			*cp = '\0';
			for (i = 0; i < db_nsymtab; i++) {
				if (! strcmp(symstr, db_symtabs[i].name)) {
					symtab_start = i;
					symtab_end = i + 1;
					break;
				}
			}
			*cp = ':';
			if (i == db_nsymtab)
				db_error("Invalid symbol table name\n");
			symstr = cp+2;
		}
	}

	/*
	 * Look in the specified set of symbol tables.
	 * Return on first match.
	 */
	for (i = symtab_start; i < symtab_end; i++) {
		if (sp = X_db_lookup(&db_symtabs[i], symstr)) {
			db_last_symtab = &db_symtabs[i];
			return sp;
		}
	}
	return 0;
}

/*
 * Print a symbol completion
 */
void
db_sym_print_completion(
	db_symtab_t *stab,
	char *name,
	int function,
	char *fname,
	int line)
{
	if (stab != db_symtabs)
		db_printf("%s::", stab->name);
	db_printf(name);
	if (function) {
	    db_putchar('(');
	    db_putchar(')');
	}
	if (fname) {
	    db_printf(" [static from %s", fname);
	    if (line > 0)
		db_printf(":%d", line);
	    db_putchar(']');
	}
	db_putchar('\n');
}

/*
 * Common utility routine to parse a symbol string into a file
 * name, a (possibly incomplete) symbol name without line number.
 * This routine is called from X_db_print_completion if the object
 * dependent handler supports qualified search with a file name.
 * It parses the symbol string, and call an object dependent routine
 * with parsed file name and symbol name.
 */ 
int
db_sym_parse_and_print_completion(
	int		(*func)(db_symtab_t *,
				 char *),
	db_symtab_t	*symtab,
	char		*symstr)
{
	register 	char *p;
	register 	n;
	char	 	*sym_name;
	char		*component[2];
	int		nsym;

	/*
	 * disassemble the symbol into components: [file_name:]symbol
	 */
	component[0] = symstr;
	component[1] = 0;
	for (p = symstr, n = 1; *p; p++) {
		if (*p == ':') {
			if (n == 2)
				break;
			*p = 0;
			component[n++] = p+1;
		}
	}
	if (*p == 0) {
		if (n == 1) {
			sym_name = component[0];
		} else {
			sym_name = component[1];
		}
		nsym = func(symtab, sym_name);
	} else
		nsym = 0;
	if (n == 2)
		component[1][-1] = ':';
	return nsym;
}

/*
 * Common utility routine to parse a symbol string into a file
 * name, a (possibly incomplete) symbol name without line number.
 * This routine is called from X_db_lookup_incomplete if the object
 * dependent handler supports qualified search with a file name.
 * It parses the symbol string, and call an object dependent routine
 * with parsed file name and symbol name.
 */ 
int
db_sym_parse_and_lookup_incomplete(
	int		(*func)(db_symtab_t *,
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
	int		*toadd)
{
	register 	char *p;
	register 	n;
	char	 	*file_name = 0;
	char	 	*sym_name = 0;
	char		*component[2];
	int		nsym = 0;

	/*
	 * disassemble the symbol into components: [file_name:]symbol
	 */
	component[0] = symstr;
	component[1] = 0;
	for (p = symstr, n = 1; *p; p++) {
		if (*p == ':') {
			if (n == 2)
				break;
			*p = 0;
			component[n++] = p+1;
		}
	}
	if (*p == 0) {
		if (n == 1) {
			file_name = 0;
			sym_name = component[0];
		} else {
			file_name = component[0];
			sym_name = component[1];
		}
		nsym = func(symtab, file_name, sym_name, 0, (db_sym_t *)0,
			    name, len);
		if (nsym > 0)
			*toadd = *len - strlen(sym_name);
	}
	if (n == 2)
		component[1][-1] = ':';
	return(nsym);
}

/*
 * Common utility routine to parse a symbol string into a file
 * name, a symbol name and line number.
 * This routine is called from X_db_lookup if the object dependent
 * handler supports qualified search with a file name or a line number.
 * It parses the symbol string, and call an object dependent routine
 * with parsed file name, symbol name and line number.
 */ 
db_sym_t
db_sym_parse_and_lookup(
	int		(*func)(db_symtab_t *, char *, char *, int,
				db_sym_t*, char **, int *),
	db_symtab_t	*symtab,
	char		*symstr)
{
	register 	char *p;
	register 	n;
	int	 	n_name;
	int	 	line_number;
	char	 	*file_name = 0;
	char	 	*sym_name = 0;
	char		*component[3];
	db_sym_t 	found = DB_SYM_NULL;

	/*
	 * disassemble the symbol into components:
	 *	[file_name:]symbol[:line_nubmer]
	 */
	component[0] = symstr;
	component[1] = component[2] = 0;
	for (p = symstr, n = 1; *p; p++) {
		if (*p == ':') {
			if (n >= 3)
				break;
			*p = 0;
			component[n++] = p+1;
		}
	}
	if (*p != 0)
		goto out;
	line_number = 0;
	n_name = n;
	p = component[n-1];
	if (*p >= '0' && *p <= '9') {
		if (n == 1)
			goto out;
		for (line_number = 0; *p; p++) {
			if (*p < '0' || *p > '9')
				goto out;
			line_number = line_number*10 + *p - '0';
		}
		n_name--;
	} else if (n >= 3)
		goto out;
	if (n_name == 1) {
		for (p = component[0]; *p && *p != '.'; p++);
		if (*p == '.') {
			file_name = component[0];
			sym_name = 0;
		} else {
			file_name = 0;
			sym_name = component[0];
		}
	} else {
		file_name = component[0];
		sym_name = component[1];
	}
	(void) func(symtab, file_name, sym_name, line_number, &found,
		   (char **)0, (int *)0);
	
out:
	while (--n >= 1)
		component[n][-1] = ':';
	return(found);
}

/*
 * Does this symbol name appear in more than one symbol table?
 * Used by db_symbol_values to decide whether to qualify a symbol.
 */
boolean_t db_qualify_ambiguous_names = FALSE;

boolean_t
db_symbol_is_ambiguous(db_sym_t sym)
{
	char		*sym_name;
	register int	i;
	register
	boolean_t	found_once = FALSE;

	if (!db_qualify_ambiguous_names)
		return FALSE;

	X_db_symbol_values(sym, &sym_name, 0);
	for (i = 0; i < db_nsymtab; i++) {
		if (X_db_lookup(&db_symtabs[i], sym_name)) {
			if (found_once)
				return TRUE;
			found_once = TRUE;
		}
	}
	return FALSE;
}

/*
 * Find the closest symbol to val, and return its name
 * and the difference between val and the symbol found.
 */
unsigned int db_search_maxoff = 0x4000;
db_sym_t
db_search_task_symbol(
	register db_addr_t	val,
	db_strategy_t		strategy,
	db_expr_t		*offp,
	task_t			task)
{
	register
	unsigned int	diff;
	unsigned int	newdiff;
	register int	i;
	db_symtab_t	*sp;
	db_sym_t	ret = DB_SYM_NULL, sym;
	vm_map_t	map_for_val;

	if (task == TASK_NULL)
	    task = db_current_task();
	map_for_val = (task == TASK_NULL)? VM_MAP_NULL: task->map;
again:
	newdiff = diff = ~0;
	db_last_symtab = 0;
	for (sp = &db_symtabs[0], i = 0;
	     i < db_nsymtab;
	     sp++, i++) {
	    if (((vm_map_t)sp->map_pointer == VM_MAP_NULL ||
		 (vm_map_t)sp->map_pointer == map_for_val) &&
		(sp->maxsym == 0 ||
		 ((unsigned long) val >= sp->minsym &&
		  (unsigned long) val <= sp->maxsym))) {
		sym = X_db_search_symbol(sp, val, strategy,
					 (db_expr_t *) &newdiff);
		if (newdiff < diff) {
		    db_last_symtab = sp;
		    diff = newdiff;
		    ret = sym;
		    if (diff <= db_search_maxoff)
			break;
		}
	    }
	}
	if (ret == DB_SYM_NULL && map_for_val != VM_MAP_NULL) {
		map_for_val = VM_MAP_NULL;
		goto again;
	}
	*offp = diff;
	return ret;
}

/*
 * Find the closest symbol to val, and return its name
 * and the difference between val and the symbol found.
 * Also return the filename and linenumber if available.
 */
db_sym_t
db_search_task_symbol_and_line(
	register db_addr_t	val,
	db_strategy_t		strategy,
	db_expr_t		*offp,
	char			**filenamep,
	int			*linenump,
	task_t			task,
	int			*argsp)
{
	register
	unsigned int	diff;
	unsigned int	newdiff;
	register int	i;
	db_symtab_t	*sp;
	db_sym_t	ret = DB_SYM_NULL, sym;
	vm_map_t	map_for_val;
	char 		*func;
	char		*filename;
	int		linenum;
	int		args;

	if (task == TASK_NULL)
	    task = db_current_task();
	map_for_val = (task == TASK_NULL)? VM_MAP_NULL: task->map;
	*filenamep = (char *) 0;
	*linenump = 0;
	*argsp = -1;
    again:
	filename = (char *) 0;
	linenum = 0;
	newdiff = diff = ~0;
	db_last_symtab = 0;
	for (sp = &db_symtabs[0], i = 0;
	     i < db_nsymtab;
	     sp++, i++) {
	    if (((vm_map_t)sp->map_pointer == VM_MAP_NULL ||
		 (vm_map_t)sp->map_pointer == map_for_val) &&
		(sp->maxsym == 0 ||
		 ((unsigned long) val >= sp->minsym &&
		  (unsigned long) val <= sp->maxsym))) {
		sym = X_db_search_by_addr(sp, val, &filename, &func,
					  &linenum, &newdiff, &args);
		if (sym && newdiff < diff) {
		    db_last_symtab = sp;
		    diff = newdiff;
		    ret = sym;
		    *filenamep = filename;
		    *linenump = linenum;
		    *argsp = args;
		    if (diff <= db_search_maxoff)
			break;
		}
	    }
	}
	if (ret == DB_SYM_NULL && map_for_val != VM_MAP_NULL) {
		map_for_val = VM_MAP_NULL;
		goto again;
	}
	*offp = diff;
	if (*filenamep)
		db_shorten_filename(filenamep);
	return ret;
}

/*
 * Return name and value of a symbol
 */
void
db_symbol_values(
	db_sym_t	sym,
	char		**namep,
	db_expr_t	*valuep)
{
	db_expr_t	value;

	if (sym == DB_SYM_NULL) {
		*namep = 0;
		return;
	}

	X_db_symbol_values(sym, namep, &value);

	if (db_symbol_is_ambiguous(sym))
		*namep = db_qualify(sym, db_last_symtab->name);
	if (valuep)
		*valuep = value;
}


/*
 * Print a the closest symbol to value
 *
 * After matching the symbol according to the given strategy
 * we print it in the name+offset format, provided the symbol's
 * value is close enough (eg smaller than db_maxoff).
 * We also attempt to print [filename:linenum] when applicable
 * (eg for procedure names).
 *
 * If we could not find a reasonable name+offset representation,
 * then we just print the value in hex.  Small values might get
 * bogus symbol associations, e.g. 3 might get some absolute
 * value like _INCLUDE_VERSION or something, therefore we do
 * not accept symbols whose value is zero (and use plain hex).
 */

void
db_task_printsym(
	db_expr_t	off,
	db_strategy_t	strategy,
	task_t		task)
{
	db_expr_t	d;
	char 		*filename;
	char		*name;
	db_expr_t	value;
	int 		linenum;
	db_sym_t	cursym;

	if (off >= db_maxval || off < db_minval) {
		db_printf("%#n", off);
		return;
	}
	cursym = db_search_task_symbol(off, strategy, &d, task);

	db_symbol_values(cursym, &name, &value);
	if (name == 0 || d >= db_maxoff || value == 0) {
		db_printf("%#n", off);
		return;
	}
	db_printf("%s", name);
	if (d)
		db_printf("+0x%x", d);
	if (strategy == DB_STGY_PROC) {
		if (db_line_at_pc(cursym, &filename, &linenum, off)) {
			db_printf(" [%s", filename);
			if (linenum > 0)
				db_printf(":%d", linenum);
			db_printf("]");
		}
	}
}

/*
 * Return symbol name for a given offset and
 * change the offset to be relative to this symbol.
 * Very usefull for xpr, when you want to log offsets
 * in a user friendly way.
 */

char null_sym[] = "";

char *
db_get_sym(db_expr_t *off)
{
	db_sym_t	cursym;
	db_expr_t	value;
	char		*name;
	db_expr_t	d;

	cursym = db_search_symbol(*off, DB_STGY_ANY, &d);
	db_symbol_values(cursym, &name, &value);
	if (name) 
		*off = d;
	else
		name = null_sym;
	return(name);
}

void
db_printsym(
	db_expr_t	off,
	db_strategy_t	strategy)
{
	db_task_printsym(off, strategy, TASK_NULL);
}

int db_short_filename = 1;

void
db_shorten_filename(char **filenamep)
{
	char *cp, *cp_slash;

	if (! *filenamep)
		return;
	for (cp = cp_slash = *filenamep; *cp; cp++) {
		if (*cp == '/')
			cp_slash = cp;
	}
	if (*cp_slash == '/')
		*filenamep = cp_slash+1;
}

boolean_t
db_line_at_pc(
	db_sym_t	sym,
	char		**filename,
	int		*linenum,
	db_expr_t	pc)
{
	boolean_t result;

	if (db_last_symtab == 0)
		return FALSE;
	if (X_db_line_at_pc( db_last_symtab, sym, filename, linenum, pc)) {
		if (db_short_filename)
			db_shorten_filename(filename);
		result = TRUE;
	} else 
		result = FALSE;
	return(result);
}

int qsort_check = 0;

void
qsort(
	char	*table,
	int	nbelts,
	int	eltsize,
	int	(*compfun)(char *, char *))
{
	if (nbelts <= 0 || eltsize <= 0 || compfun == 0) {
		printf("qsort: invalid parameters\n");
		return;
	}
	qsort_recur(table, table + nbelts * eltsize, eltsize, compfun);

	if (qsort_check)
		qsort_checker(table, nbelts, eltsize, compfun);
}

void
qsort_swap(
	register int	*a,
	register int	*b,
	register int	size)
{
	register int temp;
	char *aa, *bb;
	char ctemp;

	for (; size >= sizeof (int); size -= sizeof (int), a++, b++) {
		temp = *a;
		*a = *b;
		*b = temp;
	}
	aa = (char *)a;
	bb = (char *)b;
	for (; size > 0; size--, aa++, bb++) {
		ctemp = *aa;
		*aa = *bb;
		*bb = ctemp;
	}
}

/* rotate the three elements to the left */
void
qsort_rotate(
	register int	*a,
	register int	*b,
	register int	*c,
	register int	size)
{
	register int temp;
	char *aa, *bb, *cc;
	char ctemp;

	for (; size >= sizeof (int); size -= sizeof (int), a++, b++, c++) {
		temp = *a;
		*a = *c;
		*c = *b;
		*b = temp;
	}
	aa = (char *)a;
	bb = (char *)b;
	cc = (char *)c;
	for (; size > 0; size--, aa++, bb++, cc++) {
		ctemp = *aa;
		*aa = *cc;
		*cc = *bb;
		*bb = ctemp;
	}
}

void
qsort_recur(
	char	*left,
	char	*right,
	int	eltsize,
	int	(*compfun)(char *, char *))
{
	char *i, *j;
	char *sameleft, *sameright;

    top:
	if (left + eltsize - 1 >= right) {
		return;
	}

	/* partition element (reference for "same"ness */
	sameleft = left + (((right - left) / eltsize) / 2) * eltsize;
	sameright = sameleft;

	i = left;
	j = right - eltsize;

    again:
    	while (i < sameleft) {
		int comp;

		comp = (*compfun)(i, sameleft);
		if (comp == 0) {
			/*
			 * Move to the "same" partition.
			 */
			/*
			 * Shift the left part of the "same" partition to
			 * the left, so that "same" elements stay in their
			 * original order.
			 */
			sameleft -= eltsize;
			qsort_swap((int *) i, (int *) sameleft, eltsize);
		} else if (comp < 0) {
			/*
			 * Stay in the "left" partition.
			 */
			i += eltsize;
		} else {
			/*
			 * Should be moved to the "right" partition.
			 * Wait until the next loop finds an appropriate
			 * place to store this element.
			 */
			break;
		}
	}

	while (j > sameright) {
		int comp;

		comp = (*compfun)(sameright, j);
		if (comp == 0) {
			/*
			 * Move to the right of the "same" partition.
			 */
			sameright += eltsize;
			qsort_swap((int *) sameright, (int *) j, eltsize);
		} else if (comp > 0) {
			/*
			 * Move to the "left" partition.
			 */
			if (i == sameleft) {
				/*
				 * Unfortunately, the "left" partition
				 * has already been fully processed, so
				 * we have to shift the "same" partition
				 * to the right to free a "left" element.
				 * This is done by moving the leftest same
				 * to the right of the "same" partition.
				 */
				sameright += eltsize;
				qsort_rotate((int *) sameleft, (int*) sameright,
					     (int *) j, eltsize);
				sameleft += eltsize;
				i = sameleft;
			} else {
				/*
				 * Swap with the "left" partition element
				 * waiting to be moved to the "right"
				 * partition.
				 */
				qsort_swap((int *) i, (int *) j, eltsize);
				j -= eltsize;
				/*
				 * Go back to the 1st loop.
				 */
				i += eltsize;
				goto again;
			}
		} else {
			/*
			 * Stay in the "right" partition.
			 */
			j -= eltsize;
		}
	}
			
	if (i != sameleft) {
		/*
		 * The second loop completed (the"right" partition is ok),
		 * but we have to go back to the first loop, and deal with
		 * the element waiting for a place in the "right" partition.
		 * Let's shift the "same" zone to the left.
		 */
		sameleft -= eltsize;
		qsort_rotate((int *) sameright, (int *) sameleft, (int *) i,
			     eltsize);
		sameright -= eltsize;
		j = sameright;
		/*
		 * Go back to 1st loop.
		 */
		goto again;
	}

	/*
	 * The partitions are correct now. Recur on the smallest side only.
	 */
	if (sameleft - left >= right - (sameright + eltsize)) {
		qsort_recur(sameright + eltsize, right, eltsize, compfun);
		/*
		 * The "right" partition is now completely sorted.
		 * The "same" partition is OK, so...
		 * Ignore them, and start the loops again on the
		 * "left" partition.
		 */
		right = sameleft;
		goto top;
	} else {
		qsort_recur(left, sameleft, eltsize, compfun);
		/*
		 * The "left" partition is now completely sorted.
		 * The "same" partition is OK, so ...
		 * Ignore them, and start the loops again on the
		 * "right" partition.
		 */
		left = sameright + eltsize;
		goto top;
	}
}

void
qsort_checker(
	char	*table,
	int	nbelts,
	int	eltsize,
	int	(*compfun)(char *, char *))
{
	char *curr, *prev, *last;

	prev = table;
	curr = prev + eltsize;
	last = table + (nbelts * eltsize);

	while (prev < last) {
		if ((*compfun)(prev, curr) > 0) {
			printf("**** qsort_checker: error between 0x%x and 0x%x!!!\n", prev, curr);
			break;
		}
		prev = curr;
		curr += eltsize;
	}
	printf("qsort_checker: OK\n");
}

int qsort_search_debug = 0;

void
qsort_limit_search(
	char	*target,
	char	**start,
	char	**end,
	int	eltsize,
	int	(*compfun)(char *, char *))
{
	register char *left, *right;
	char *oleft, *oright, *part;
	int nbiter = 0;
	int comp;

	oleft = left = *start;
	oright = right = *end;
	part = (char *) 0;

	while (left < right) {
		nbiter++;
		part = left + (((right - left) / eltsize) / 2) * eltsize;
		comp = (*compfun)(target, part);
		if (comp > 0) {
			oleft = left;
			oright = right;
			left = part;
			if (left == oleft)
				break;
			if (qsort_search_debug > 1)
				printf(" [ Moved left from 0x%x to 0x%x]\n",
				       oleft, left);
		} else if (comp < 0) {
			oright = right;
			oleft = left;
			right = part;
			if (qsort_search_debug > 1)
				printf(" [ Moved right from 0x%x to 0x%x]\n",
				       oright, right);
		} else {
			if (qsort_search_debug > 1)
				printf(" [ FOUND! left=0x%x right=0x%x]\n",
				       left, right);
			for (left = part;
			     left > *start && (*compfun)(left, part) == 0;
			     left -= eltsize);
			for (right = part + eltsize;
			     right < *end && (*compfun)(right, part) == 0;
			     right += eltsize);
			oright = right;
			oleft = left;
			break;
		}
	}
	
	if (qsort_search_debug)
		printf("[ Limited from %x-%x to %x-%x in %d iters ]\n",
			  *start, *end, oleft, oright, nbiter);
	*start = oleft;
	*end = oright;
}

void
bubble_sort(
	char	*table,
	int	nbelts,
	int	eltsize,
	int	(*compfun)(char *, char *))
{
	boolean_t sorted;
	char *end;
	register char *p;

	end = table + ((nbelts-1) * eltsize);
	do {
		sorted = TRUE;
		for (p = table; p < end; p += eltsize) {
			if ((*compfun)(p, p + eltsize) > 0) {
				qsort_swap((int *) p, (int *) (p + eltsize),
					   eltsize);
				sorted = FALSE;
			}
		}
	} while (sorted == FALSE);

	if (qsort_check)
		qsort_checker(table, nbelts, eltsize, compfun);
}


/*
 * The following is a KLUDGE to clone the server symbol table
 * using offset addresses for the in-kernel server.
 */
void db_clone_symtabXXX(char *, char *,vm_offset_t, char *);
vm_offset_t	vm_min_inks_addr = VM_MAX_KERNEL_ADDRESS;

void
db_clone_symtabXXX(
	char *clonee,			/* which symtab to clone	*/
	char *cloner,			/* in-kernel-server name	*/
	vm_offset_t base,		/* base address of cloner	*/
	char * map)			/* task->map of cloner		*/
{
	db_symtab_t	*st, *st_src;
	char *		memp;
	vm_size_t	size;
	long		offset;
	extern vm_offset_t kalloc(vm_size_t);
	extern void db_clone_offsetXXX(char *, long);

	if (db_nsymtab >= MAXNOSYMTABS) {
	    db_printf("db_clone_symtab: Too Many Symbol Tables\n");
	    return;
	}

	/* save addr to demarcate kernel/inks boundary (1st time only)	*/
	if (vm_min_inks_addr == VM_MAX_KERNEL_ADDRESS) {
		vm_min_inks_addr = base;
		db_qualify_ambiguous_names = TRUE;
	}

	st = &db_symtabs[db_nsymtab];	/* destination symtab		*/
					/* find source symtab		*/
	for (st_src = &db_symtabs[0]; st_src < st; st_src++)
		if (!strcmp(clonee, st_src->name))
			break;
	if (st_src >= st) {
	    db_printf("db_clone_symtab: clonee (%s) not found\n", clonee);
	    return;
	}
					/* alloc new symbols		*/
	size = (vm_size_t)(st_src->end - st_src->private);
	memp = (char *)kalloc( round_page(size) );
	if (!memp) {
	    db_printf("db_clone_symtab: no memory for symtab\n");
	    return;
	}

	*st = *st_src;			/* bulk copy src -> dest	*/
	strcpy(st->name, cloner);	/* new name			*/
	st->private = memp;		/* copy symbols			*/
	bcopy((const char *)st_src->private, st->private, size);
	st->start = memp + sizeof(int);	/* fixup pointers to symtab	*/
	st->end   = memp + *(int *)memp;
	st->map_pointer = map;		/* set map (if any)		*/

	/* Offset symbols, leaving strings pointing into st_src		*/
	offset	    = base - st_src->minsym;
	st->minsym  += offset;
	st->maxsym  += offset;
	db_clone_offsetXXX(memp, offset);
	db_nsymtab++;

	db_printf( "[ cloned symbol table for %s: range 0x%x to 0x%x %s]\n",
		  st->name, st->minsym, st->maxsym,
		  st->sorted ? "(sorted) " : "");
	db_maxval = (unsigned int)st->maxsym + db_maxoff;
}
