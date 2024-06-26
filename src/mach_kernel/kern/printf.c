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
 * Revision 2.9.8.2  92/09/15  17:21:39  jeffreyh
 * 	Use print_lock for MBUS as well, unused only for SYMMETRY.
 * 	[92/07/17            bernadat]
 * 
 * Revision 2.9.8.1  92/02/18  19:09:25  jeffreyh
 * 	Use printf_lock only for CBUS. Does not work with Sequent
 * 	[91/12/11            bernadat]
 * 
 * Revision 2.9.3.1  91/09/26  04:47:46  bernadat
 * 	Support for Corollary MP
 * 	Use a printf_lock and print cpu number
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.9  91/06/06  17:07:22  jsb
 * 	Added gets (derived from boot_gets).
 * 	[91/05/14  09:18:50  jsb]
 * 
 * Revision 2.8  91/05/14  16:44:59  mrt
 * 	Correcting copyright
 * 
 * Revision 2.7  91/02/05  17:28:14  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:15:41  mrt]
 * 
 * Revision 2.6  90/10/25  14:45:18  rwd
 * 	Purged uprintf.
 * 	[90/10/21            rpd]
 * 
 * Revision 2.5  90/08/27  22:03:08  dbg
 * 	Add extra formats to printf: '#- ' prefixes, %z (signed hex),
 * 	%r and %n (signed and unsigned, current radix).
 * 	[90/08/20            dbg]
 * 
 * Revision 2.4  90/01/11  11:43:44  dbg
 * 	De-linted.
 * 	[90/01/03            dbg]
 * 
 * Revision 2.3  89/11/29  14:09:06  af
 * 	Ooops, a typo.
 * 	[89/10/29  09:34:26  af]
 * 
 * 	Changed the case for %c to load ints and not chars. Or
 * 	else it is byte-order dependent since C passes char as ints.
 * 	[89/10/13            af]
 * 
 * 	Turned the unused 'file descriptor' field for _doprnt and putchar
 * 	into a more useful pointer to an (optional) specialized putchar
 * 	routine.  This can be used, for instance, to divert debugging
 * 	printouts to some specialized interface or IOP.
 * 	[89/10/09            af]
 * 
 * Revision 2.2  89/09/25  11:00:58  rwd
 * 	Added case 'X' same as 'x' for now.
 * 	[89/09/20            rwd]
 * 
 * Revision 2.1  89/08/03  15:51:14  rwd
 * Created.
 * 
 *  8-Aug-88  David Golub (dbg) at Carnegie-Mellon University
 *	Converted for MACH kernel use.  Removed %r, %R, %b; added %b
 *	from Berkeley's kernel to print bit fields in device registers;
 *	changed to use varargs.
 *
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988 Carnegie Mellon University
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
 *  Common code for printf et al.
 *
 *  The calling routine typically takes a variable number of arguments,
 *  and passes the address of the first one.  This implementation
 *  assumes a straightforward, stack implementation, aligned to the
 *  machine's wordsize.  Increasing addresses are assumed to point to
 *  successive arguments (left-to-right), as is the case for a machine
 *  with a downward-growing stack with arguments pushed right-to-left.
 *
 *  To write, for example, fprintf() using this routine, the code
 *
 *	fprintf(fd, format, args)
 *	FILE *fd;
 *	char *format;
 *	{
 *	_doprnt(format, &args, fd);
 *	}
 *
 *  would suffice.  (This example does not handle the fprintf's "return
 *  value" correctly, but who looks at the return value of fprintf
 *  anyway?)
 *
 *  This version implements the following printf features:
 *
 *	%d	decimal conversion
 *	%u	unsigned conversion
 *	%x	hexadecimal conversion
 *	%X	hexadecimal conversion with capital letters
 *	%o	octal conversion
 *	%c	character
 *	%s	string
 *	%m.n	field width, precision
 *	%-m.n	left adjustment
 *	%0m.n	zero-padding
 *	%*.*	width and precision taken from arguments
 *
 *  This version does not implement %f, %e, or %g.  It accepts, but
 *  ignores, an `l' as in %ld, %lo, %lx, and %lu, and therefore will not
 *  work correctly on machines for which sizeof(long) != sizeof(int).
 *  It does not even parse %D, %O, or %U; you should be using %ld, %o and
 *  %lu if you mean long conversion.
 *
 *  As mentioned, this version does not return any reasonable value.
 *
 *  Permission is granted to use, modify, or propagate this code as
 *  long as this notice is incorporated.
 *
 *  Steve Summit 3/25/87
 */

/*
 * Added formats for decoding device registers:
 *
 * printf("reg = %b", regval, "<base><arg>*")
 *
 * where <base> is the output base expressed as a control character:
 * i.e. '\10' gives octal, '\20' gives hex.  Each <arg> is a sequence of
 * characters, the first of which gives the bit number to be inspected
 * (origin 1), and the rest (up to a control character (<= 32)) give the
 * name of the register.  Thus
 *	printf("reg = %b\n", 3, "\10\2BITTWO\1BITONE")
 * would produce
 *	reg = 3<BITTWO,BITONE>
 *
 * If the second character in <arg> is also a control character, it
 * indicates the last bit of a bit field.  In this case, printf will extract
 * bits <1> to <2> and print it.  Characters following the second control
 * character are printed before the bit field.
 *	printf("reg = %b\n", 0xb, "\10\4\3FIELD1=\2BITTWO\1BITONE")
 * would produce
 *	reg = b<FIELD1=2,BITONE>
 */
/*
 * Added for general use:
 *	#	prefix for alternate format:
 *		0x (0X) for hex
 *		leading 0 for octal
 *	+	print '+' if positive
 *	blank	print ' ' if positive
 *
 *	z	signed hexadecimal
 *	r	signed, 'radix'
 *	n	unsigned, 'radix'
 *
 *	D,U,O,Z	same as corresponding lower-case versions
 *		(compatibility)
 */

#include <platforms.h>
#include <mach/boolean.h>
#include <cpus.h>
#include <kern/cpu_number.h>
#include <kern/lock.h>
#include <kern/thread.h>
#include <kern/misc_protos.h>
#include <stdarg.h>
#include <string.h>

/*
 * Forward declarations
 */
void printnum(
	register unsigned int	u,
	register int		base,
	void			(*putc)(char));

#if	NCPUS > 1
static void printn(
		unsigned l,
		unsigned n);
#endif	/* NCPUS > 1 */

#define isdigit(d) ((d) >= '0' && (d) <= '9')
#define Ctod(c) ((c) - '0')

#define MAXBUF (sizeof(long int) * 8)		 /* enough for binary */

void
printnum(
	register unsigned int	u,		/* number to print */
	register int		base,
	void			(*putc)(char))
{
	char	buf[MAXBUF];	/* build number here */
	register char *	p = &buf[MAXBUF-1];
	static char digs[] = "0123456789abcdef";

	do {
	    *p-- = digs[u % base];
	    u /= base;
	} while (u != 0);

	while (++p != &buf[MAXBUF])
	    (*putc)(*p);

}

void 
_doprnt(
	register const char	*fmt,
	va_list			*argp,
 	void			(*putc)(char),	/* character output */
	int			radix)		/* default radix - for '%r' */
{
	int		length;
	int		prec;
	boolean_t	ladjust;
	char		padc;
	int		n;
	unsigned int	u;
	int		plus_sign;
	int		sign_char;
	boolean_t	altfmt;
	int		base;
	char		c;

	while (*fmt != '\0') {
	    if (*fmt != '%') {
		(*putc)(*fmt++);
		continue;
	    }

	    fmt++;

	    length = 0;
	    prec = -1;
	    ladjust = FALSE;
	    padc = ' ';
	    plus_sign = 0;
	    sign_char = 0;
	    altfmt = FALSE;

	    while (TRUE) {
		if (*fmt == '#') {
		    altfmt = TRUE;
		    fmt++;
		}
		else if (*fmt == '-') {
		    ladjust = TRUE;
		    fmt++;
		}
		else if (*fmt == '+') {
		    plus_sign = '+';
		    fmt++;
		}
		else if (*fmt == ' ') {
		    if (plus_sign == 0)
			plus_sign = ' ';
		    fmt++;
		}
		else
		    break;
	    }

	    if (*fmt == '0') {
		padc = '0';
		fmt++;
	    }

	    if (isdigit(*fmt)) {
		while(isdigit(*fmt))
		    length = 10 * length + Ctod(*fmt++);
	    }
	    else if (*fmt == '*') {
		length = va_arg(*argp, int);
		fmt++;
		if (length < 0) {
		    ladjust = !ladjust;
		    length = -length;
		}
	    }

	    if (*fmt == '.') {
		fmt++;
		if (isdigit(*fmt)) {
		    prec = 0;
		    while(isdigit(*fmt))
			prec = 10 * prec + Ctod(*fmt++);
		}
		else if (*fmt == '*') {
		    prec = va_arg(*argp, int);
		    fmt++;
		}
	    }

	    if (*fmt == 'l')
		fmt++;	/* need it if sizeof(int) < sizeof(long) */

	    switch(*fmt) {
		case 'b':
		case 'B':
		{
		    register char *p;
		    boolean_t	  any;
		    register int  i;

		    u = va_arg(*argp, unsigned int);
		    p = va_arg(*argp, char *);
		    base = *p++;
		    printnum(u, base, putc);

		    if (u == 0)
			break;

		    any = FALSE;
		    while (i = *p++) {
			if (*p <= 32) {
			    /*
			     * Bit field
			     */
			    register int j;
			    if (any)
				(*putc)(',');
			    else {
				(*putc)('<');
				any = TRUE;
			    }
			    j = *p++;
			    for (; (c = *p) > 32; p++)
				(*putc)(c);
			    printnum((unsigned)( (u>>(j-1)) & ((2<<(i-j))-1)),
					base, putc);
			}
			else if (u & (1<<(i-1))) {
			    if (any)
				(*putc)(',');
			    else {
				(*putc)('<');
				any = TRUE;
			    }
			    for (; (c = *p) > 32; p++)
				(*putc)(c);
			}
			else {
			    for (; *p > 32; p++)
				continue;
			}
		    }
		    if (any)
			(*putc)('>');
		    break;
		}

		case 'c':
		    c = va_arg(*argp, int);
		    (*putc)(c);
		    break;

		case 's':
		{
		    register char *p;
		    register char *p2;

		    if (prec == -1)
			prec = 0x7fffffff;	/* MAXINT */

		    p = va_arg(*argp, char *);

		    if (p == (char *)0)
			p = "";

		    if (length > 0 && !ladjust) {
			n = 0;
			p2 = p;

			for (; *p != '\0' && n < prec; p++)
			    n++;

			p = p2;

			while (n < length) {
			    (*putc)(' ');
			    n++;
			}
		    }

		    n = 0;

		    while (*p != '\0') {
			if (++n > prec)
			    break;

			(*putc)(*p++);
		    }

		    if (n < length && ladjust) {
			while (n < length) {
			    (*putc)(' ');
			    n++;
			}
		    }

		    break;
		}

		case 'o':
		case 'O':
		    base = 8;
		    goto print_unsigned;

		case 'd':
		case 'D':
		    base = 10;
		    goto print_signed;

		case 'u':
		case 'U':
		    base = 10;
		    goto print_unsigned;

		case 'x':
		case 'X':
		    base = 16;
		    goto print_unsigned;

		case 'z':
		case 'Z':
		    base = 16;
		    goto print_signed;

		case 'r':
		case 'R':
		    base = radix;
		    goto print_signed;

		case 'n':
		    base = radix;
		    goto print_unsigned;

		print_signed:
		    n = va_arg(*argp, int);
		    if (n >= 0) {
			u = n;
			sign_char = plus_sign;
		    }
		    else {
			u = -n;
			sign_char = '-';
		    }
		    goto print_num;

		print_unsigned:
		    u = va_arg(*argp, unsigned int);
		    goto print_num;

		print_num:
		{
		    char	buf[MAXBUF];	/* build number here */
		    register char *	p = &buf[MAXBUF-1];
		    static char digits[] = "0123456789abcdef";
		    char *prefix = 0;

		    if (u != 0 && altfmt) {
			if (base == 8)
			    prefix = "0";
			else if (base == 16)
			    prefix = "0x";
		    }

		    do {
			*p-- = digits[u % base];
			u /= base;
		    } while (u != 0);

		    length -= (&buf[MAXBUF-1] - p);
		    if (sign_char)
			length--;
		    if (prefix)
			length -= strlen((const char *) prefix);

		    if (padc == ' ' && !ladjust) {
			/* blank padding goes before prefix */
			while (--length >= 0)
			    (*putc)(' ');
		    }
		    if (sign_char)
			(*putc)(sign_char);
		    if (prefix)
			while (*prefix)
			    (*putc)(*prefix++);
		    if (padc == '0') {
			/* zero padding goes after sign and prefix */
			while (--length >= 0)
			    (*putc)('0');
		    }
		    while (++p != &buf[MAXBUF])
			(*putc)(*p);

		    if (ladjust) {
			while (--length >= 0)
			    (*putc)(' ');
		    }
		    break;
		}

		case '\0':
		    fmt--;
		    break;

		default:
		    (*putc)(*fmt);
	    }
	fmt++;
	}
}

#if	NCPUS > 1
boolean_t	new_printf_cpu_number = FALSE;

static void
printn(
	unsigned l,
	unsigned n)
{
	if (n > 9)
		printn(l - 1, n / 10);
	else
		while (--l > 0)
			cnputc('0');
	cnputc('0' + n % 10);
}
#endif	/* NCPUS > 1 */

spinlock_t	printf_lock;

void
printf_init(void)
{
	/*
	 * Lock is only really needed after the first thread is created.
	 */
	spinlock_init(&printf_lock);
}

/*VARARGS1*/
void
printf(const char *fmt, ...)
{
	va_list	listp;
#if	NCPUS > 1
	unsigned i, j, n;
#endif	/* NCPUS > 1 */

	va_start(listp, fmt);
#if	NCPUS > 1
	if (active_threads[master_cpu]) {
		spinlock_lock(&printf_lock);
		if (cpu_number() != master_cpu)
			new_printf_cpu_number = TRUE;
		if (new_printf_cpu_number) {
			cnputc('{');
			for (i = 1, j = NCPUS-1; j > 0xF; j >>= 4)
				i++;
			while (i-- > 0) {
				j = (n >> (i * 4)) & 0xF;
				if (j <= 9)
					cnputc('0' + j);
				else
					cnputc('a' + (j - 10));
			}
			cnputc('}');
			cnputc(' ');
		}
		_doprnt(fmt, &listp, cnputc, 16);
		spinlock_unlock(&printf_lock);
      } else 
#endif	/* NCPUS > 1 */
	_doprnt(fmt, &listp, cnputc, 16);
	va_end(listp);
}

/* derived from boot_gets */
void
safe_gets(
	char	*str,
	int	maxlen)
{
	register char *lp;
	register int c;
	char *strmax = str + maxlen - 1; /* allow space for trailing 0 */

	lp = str;
	for (;;) {
		c = cngetc();
		switch (c) {
		case '\n':
		case '\r':
			printf("\n");
			*lp++ = 0;
			return;
			
		case '\b':
		case '#':
		case '\177':
			if (lp > str) {
				printf("\b \b");
				lp--;
			}
			continue;

		case '@':
		case 'u'&037:
			lp = str;
			printf("\n\r");
			continue;

		default:
			if (c >= ' ' && c < '\177') {
				if (lp < strmax) {
					*lp++ = c;
					printf("%c", c);
				}
				else {
					printf("%c", '\007'); /* beep */
				}
			}
		}
	}
}
