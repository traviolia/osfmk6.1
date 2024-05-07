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
 * 
 */
/*
 * OSF Research Institute MK6.1 (unencumbered) 1/31/1995
 */

#include <kgdb/kgdb_defs.h>
#include <sys/types.h>
#include <kern/misc_protos.h>

int kgdb_radix = 16;

extern void             switch_to_kd(void);
extern void             switch_to_com1(void);


/*
 * kgdb_printf()
 *
 *	Do a printf() to the system console.  Used for debug printf's
 *	within the kernel remote gdb support code.  Output forced to
 *	the system console. 
 */
void
kgdb_printf(const char *fmt, ...)
{
	va_list	listp;

	/*
	 * Switch output to the system console
	 */
	switch_to_kd();

	va_start(listp, fmt);
	_doprnt(fmt, &listp, cnputc, kgdb_radix);
	va_end(listp);

	/*
	 * Switch output to serial line
	 */
	switch_to_com1();
}
