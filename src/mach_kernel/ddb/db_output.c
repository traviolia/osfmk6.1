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
 * Revision 2.6  91/10/09  16:01:26  af
 * 	 Revision 2.5.2.1  91/10/05  13:06:46  jeffreyh
 * 	 	Added "more" like function.
 * 	 	[91/08/29            tak]
 * 
 * Revision 2.5.2.1  91/10/05  13:06:46  jeffreyh
 * 	Added "more" like function.
 * 	[91/08/29            tak]
 * 
 * Revision 2.5  91/07/09  23:15:54  danner
 * 	Include machine/db_machdep.c.
 * 	When db_printf is invoked, call db_printing on luna. This is used
 * 	 to trigger a screen clear. Under luna88k conditional.
 * 	[91/07/08            danner]
 *
 * Revision 2.4  91/05/14  15:34:51  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:06:45  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  16:18:41  mrt]
 * 
 * Revision 2.2  90/08/27  21:51:25  dbg
 * 	Put extra features of db_doprnt in _doprnt.
 * 	[90/08/20            dbg]
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
 * 	Author: David B. Golub, Carnegie Mellon University
 *	Date:	7/90
 */

/*
 * Printf and character output for debugger.
 */

#include <mach/boolean.h>
#include <kern/misc_protos.h>
#include <stdarg.h>
#include <machine/db_machdep.h>
#include <ddb/db_command.h>
#include <ddb/db_input.h>
#include <ddb/db_output.h>
#include <ddb/db_task_thread.h>

/*
 *	Character output - tracks position in line.
 *	To do this correctly, we should know how wide
 *	the output device is - then we could zero
 *	the line position when the output device wraps
 *	around to the start of the next line.
 *
 *	Instead, we count the number of spaces printed
 *	since the last printing character so that we
 *	don't print trailing spaces.  This avoids most
 *	of the wraparounds.
 */

#ifndef	DB_MAX_LINE
#define	DB_MAX_LINE		24	/* maximum line */
#define DB_MAX_WIDTH		80	/* maximum width */
#endif	/* DB_MAX_LINE */

#define DB_MIN_MAX_WIDTH	20	/* minimum max width */
#define DB_MIN_MAX_LINE		3	/* minimum max line */
#define CTRL(c)			((c) & 0xff)

int	db_output_position = 0;		/* output column */
int	db_output_line = 0;		/* output line number */
int	db_last_non_space = 0;		/* last non-space character */
int	db_last_gen_return = 0;		/* last character generated return */
int	db_auto_wrap = 1;		/* auto wrap at end of line ? */
int	db_tab_stop_width = 8;		/* how wide are tab stops? */
#define	NEXT_TAB(i) \
	((((i) + db_tab_stop_width) / db_tab_stop_width) * db_tab_stop_width)
int	db_max_line = DB_MAX_LINE;	/* output max lines */
int	db_max_width = DB_MAX_WIDTH;	/* output line width */


/* Prototypes for functions local to this file.  XXX -- should be static!
 */
static void db_more(void);
void db_advance_output_position(int new_output_position,
				int blank);


/*
 * Force pending whitespace.
 */
void
db_force_whitespace(void)
{
	register int last_print, next_tab;

	last_print = db_last_non_space;
	while (last_print < db_output_position) {
	    next_tab = NEXT_TAB(last_print);
	    if (next_tab <= db_output_position) {
		cnputc('\t');
		last_print = next_tab;
	    }
	    else {
		cnputc(' ');
		last_print++;
	    }
	}
	db_last_non_space = db_output_position;
}

void
db_reset_more()
{
	db_output_line = 0;
}

static void
db_more(void)
{
	register  char *p;
	boolean_t quit_output = FALSE;

	for (p = "--db_more--"; *p; p++)
	    cnputc(*p);
	switch(cngetc()) {
	case ' ':
	    db_output_line = 0;
	    break;
	case 'q':
	case CTRL('c'):
	    db_output_line = 0;
	    quit_output = TRUE;
	    break;
	default:
	    db_output_line--;
	    break;
	}
	p = "\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b";
	while (*p)
	    cnputc(*p++);
	if (quit_output) {
	    db_error((char *) 1);
	    /* NOTREACHED */
	}
}

void
db_advance_output_position(int new_output_position,
			   int blank)
{
	if (db_max_width >= DB_MIN_MAX_WIDTH 
	    && new_output_position >= db_max_width) {
		/* auto new line */
		if (!db_auto_wrap || blank)
		    cnputc('\n');
		db_output_position = 0;
		db_last_non_space = 0;
		db_last_gen_return = 1;
		db_output_line++;
	} else {
		db_output_position = new_output_position;
	}
}

boolean_t
db_reserve_output_position(int increment)
{
	if (db_max_width >= DB_MIN_MAX_WIDTH
	    && db_output_position + increment >= db_max_width) {
		/* auto new line */
		if (!db_auto_wrap || db_last_non_space != db_output_position)
		    cnputc('\n');
		db_output_position = 0;
		db_last_non_space = 0;
		db_last_gen_return = 1;
		db_output_line++;
		return TRUE;
	}
	return FALSE;
}

/*
 * Output character.  Buffer whitespace.
 */
void
db_putchar(char c)
{
	if (db_max_line >= DB_MIN_MAX_LINE && db_output_line >= db_max_line-1)
	    db_more();
	if (c > ' ' && c <= '~') {
	    /*
	     * Printing character.
	     * If we have spaces to print, print them first.
	     * Use tabs if possible.
	     */
	    db_force_whitespace();
	    cnputc(c);
	    db_last_gen_return = 0;
	    db_advance_output_position(db_output_position+1, 0);
	    db_last_non_space = db_output_position;
	}
	else if (c == '\n') {
	    /* Return */
	    if (db_last_gen_return) {
		db_last_gen_return = 0;
	    } else {
		cnputc(c);
		db_output_position = 0;
		db_last_non_space = 0;
		db_output_line++;
		db_check_interrupt();
	    }
	}
	else if (c == '\t') {
	    /* assume tabs every 8 positions */
	    db_advance_output_position(NEXT_TAB(db_output_position), 1);
	}
	else if (c == ' ') {
	    /* space */
	    db_advance_output_position(db_output_position+1, 1);
	}
	else if (c == '\007') {
	    /* bell */
	    cnputc(c);
	}
	/* other characters are assumed non-printing */
}

/*
 * Return output position
 */
int
db_print_position(void)
{
	return (db_output_position);
}

/*
 * End line if too long.
 */
void
db_end_line(void)
{
	if (db_output_position >= db_max_width-1) {
	    /* auto new line */
	    if (!db_auto_wrap)
		cnputc('\n');
	    db_output_position = 0;
	    db_last_non_space = 0;
	    db_last_gen_return = 1;
	    db_output_line++;
	}
}

/*
 * Printing
 */
extern int	db_radix;

void
db_printf(char *fmt, ...)
{
	va_list	listp;

#ifdef	luna88k
	db_printing();
#endif
	va_start(listp, fmt);
	_doprnt(fmt, &listp, db_putchar, db_radix);
	va_end(listp);
}

/* alternate name */

void
kdbprintf(char *fmt, ...)
{
	va_list	listp;

	va_start(listp, fmt);
	_doprnt(fmt, &listp, db_putchar, db_radix);
	va_end(listp);
}

int	indent = 0;

/*
 * Printing (to console) with indentation.
 */
void
iprintf(char *fmt, ...)
{
	va_list listp;
	register int i;

	for (i = indent; i > 0; ){
	    if (i >= 8) {
		kdbprintf("\t");
		i -= 8;
	    }
	    else {
		kdbprintf(" ");
		i--;
	    }
	}

	va_start(listp, fmt);
	_doprnt(fmt, &listp, db_putchar, db_radix);
	va_end(listp);
}

void
db_output_prompt(void)
{
#if	NORMA_IPC
	extern int _node_self;	/* node_self() may not be callable yet */
#endif	/* NORMA_IPC */

	db_printf("db%s", (db_default_act) ? "t": "");
#if	NORMA_IPC
	db_printf("%d", _node_self);
#endif
#if	NCPUS > 1
	db_printf("{%d}", cpu_number());
#endif
	db_printf("> ");
}

