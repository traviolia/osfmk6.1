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
 * Revision 2.1.2.1  92/09/15  17:35:07  jeffreyh
 * 	Created.
 * 	[92/09/15  15:31:28  jeffreyh]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1992 Carnegie Mellon University
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
 *	File:		norma/tr.c
 *	Authors:	Alan Langerman, Jeffrey Heller
 *	Date:		1992
 *
 *	Internal trace routines.  Like old-style XPRs but
 *	less formatting.
 */

#include <norma/tr.h>

#if	TRACE_BUFFER
#include <mach_kdb.h>
#include <kern/lock.h>
#include <kern/posixtime.h>
#include <kern/spl.h>

/*
 *	Primitive event tracing facility for kernel debugging.  Yes,
 *	this has some resemblance to XPRs.  However, it is primarily
 *	intended for post-mortem analysis through ddb.
 */

#define	TRACE_MAX	(4 * 1024)
#define	TRACE_WINDOW	40

typedef struct trace_event {
	char		*funcname;
	char		*file;
	unsigned int	lineno;
	char		*fmt;
	unsigned int	tag1;
	unsigned int	tag2;
	unsigned int	tag3;
	unsigned int	tag4;
	int		timestamp[2]; /* largest needed by any clock */
} trace_event;

trace_event	trace_buffer[TRACE_MAX];
unsigned long	trace_index;
unsigned long	trace_wraps;
spinlock_t	trace_lock;

void
tr_init(void)
{
	spinlock_init(&trace_lock);
}

void
tr(
	char		*funcname,
	char		*file,
	unsigned int	lineno,
	char		*fmt,
	unsigned int	tag1,
	unsigned int	tag2,
	unsigned int	tag3,
	unsigned int	tag4)
{
	int	s;

	s = splimp();
	spinlock_lock(&trace_lock);
	trace_buffer[trace_index].funcname = funcname;
	trace_buffer[trace_index].file = file;
	trace_buffer[trace_index].lineno = lineno;
	trace_buffer[trace_index].fmt = fmt;
	trace_buffer[trace_index].tag1 = tag1;
	trace_buffer[trace_index].tag2 = tag2;
	trace_buffer[trace_index].tag3 = tag3;
	trace_buffer[trace_index].tag4 = tag4;
	get_uniq_timestamp(trace_buffer[trace_index].timestamp);
	if (trace_index == TRACE_MAX-1) {
		trace_index = 0;
		++trace_wraps;
	} else
		trace_index++;
	spinlock_unlock(&trace_lock);
	splx(s);
}

#if	MACH_KDB
#include <ddb/db_output.h>

/*
 * Forward.
 */
void	show_tr(
		unsigned long	index,
		unsigned long	range,
		unsigned long	show_extra);

int	matches(
		char	*pattern,
		char	*target);

void	parse_tr(
		unsigned long	index,
		unsigned long	range);
void
show_tr(
	unsigned long	index,
	unsigned long	range,
	unsigned long	show_extra)
{
	int		i;
	char		*filename, *cp;

	if (index == 0) {
		index = trace_index - (TRACE_WINDOW-4);
		range = TRACE_WINDOW;
		show_extra = 0;
	}
	if (index + range > TRACE_MAX)
		range = TRACE_MAX - index;
	for (i = index; i < index + range; ++i) {
		if (trace_buffer[i].file == (char *) 0 ||
		    trace_buffer[i].funcname == (char *) 0 ||
		    trace_buffer[i].lineno == 0 ||
		    trace_buffer[i].fmt == 0) {
			db_printf("[%04x%s]\n", i,
				  i >= trace_index ? "*" : "");
			continue;
		}
		for (cp = trace_buffer[i].file; *cp; ++cp)
			if (*cp == '/')
				filename = cp + 1;
		db_printf("[%04x%s](%x/%8x) %s", i,
			  i >= trace_index ? "*" : "",
			  trace_buffer[i].timestamp[1],
			  trace_buffer[i].timestamp[0],
			  trace_buffer[i].funcname);
		if (show_extra) {
			db_printf("(%s:%05d):\n\t", 
				  filename, trace_buffer[i].lineno);
		} else
			db_printf(":  ");
		db_printf(trace_buffer[i].fmt, trace_buffer[i].tag1,
			  trace_buffer[i].tag2, trace_buffer[i].tag3,
			  trace_buffer[i].tag4);
		db_printf("\n");
	}
}


int
matches(
	char	*pattern,
	char	*target)
{
	char	*cp, *cp1, *cp2;

	for (cp = target; *cp; ++cp) {
		for (cp2 = pattern, cp1 = cp; *cp2 && *cp1; ++cp2, ++cp1)
			if (*cp2 != *cp1)
				break;
		if (!*cp2)
			return 1;
	}
	return 0;
}


char	parse_tr_buffer[100] = "KMSG";

void
parse_tr(
	unsigned long	index,
	unsigned long	range)
{
	int		i;
	char		*filename, *cp;
	char		*string = parse_tr_buffer;

	if (index == 0) {
		index = trace_index - (TRACE_WINDOW-4);
		range = TRACE_WINDOW;
	}
	if (index + range > TRACE_MAX)
		range = TRACE_MAX - index;
	for (i = index; i < index + range; ++i) {
		if (trace_buffer[i].file == (char *) 0 ||
		    trace_buffer[i].funcname == (char *) 0 ||
		    trace_buffer[i].lineno == 0 ||
		    trace_buffer[i].fmt == 0) {
			db_printf("[%04x%s]\n", i,
				  i >= trace_index ? "*" : "");
			continue;
		}
		if (!matches(string, trace_buffer[i].fmt))
			continue;
		for (cp = trace_buffer[i].file; *cp; ++cp)
			if (*cp == '/')
				filename = cp + 1;
		db_printf("[%04x%s] %s", i, i >= trace_index ? "*" : "",
		       trace_buffer[i].funcname);
		db_printf(":  ");
		db_printf(trace_buffer[i].fmt, trace_buffer[i].tag1,
			  trace_buffer[i].tag2, trace_buffer[i].tag3,
			  trace_buffer[i].tag4);
		db_printf("\n");
	}
}

#endif	/* MACH_KDB */

#endif	/* TRACE_BUFFER */
