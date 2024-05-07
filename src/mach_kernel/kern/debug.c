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
 * Revision 2.18.2.2  92/03/28  10:09:54  jeffreyh
 * 	Pick up changes from MK71
 * 	[92/03/20  13:15:42  jeffreyh]
 * 
 * Revision 2.19  92/02/19  11:19:26  elf
 * 	Added cpu number print, locking to print in Assert for
 * 	 multiprocessor case.
 * 	[92/01/12            danner]
 * 
 * Revision 2.18.2.1  92/01/21  21:50:38  jsb
 * 	Added panic_on_assertion_failure flag, set true by default.
 * 	[92/01/16  21:31:13  jsb]
 * 
 * Revision 2.18  91/12/10  16:32:45  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:51:54  jsb]
 * 
 * Revision 2.17  91/08/03  18:18:47  jsb
 * 	Replaced obsolete NORMA tag with NORMA_IPC.
 * 	[91/07/27  18:13:01  jsb]
 * 
 * Revision 2.16  91/07/31  17:44:28  dbg
 * 	Minor SUN changes.
 * 	[91/07/12            dbg]
 * 
 * Revision 2.15  91/07/09  23:16:17  danner
 * 	Luna88k support.
 * 	[91/06/26            danner]
 * 
 * Revision 2.14  91/06/17  15:46:57  jsb
 * 	Renamed NORMA conditionals.
 * 	[91/06/17  10:49:18  jsb]
 * 
 * Revision 2.13  91/05/14  16:40:46  mrt
 * 	Correcting copyright
 * 
 * Revision 2.12  91/03/16  14:49:39  rpd
 * 	In panic, only call halt_cpu when not calling Debugger.
 * 	[91/03/12            rpd]
 * 
 * Revision 2.11  91/02/05  17:25:52  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:11:43  mrt]
 * 
 * Revision 2.10  90/12/14  11:02:02  jsb
 * 	NORMA_IPC support: print node number as well as cpu in panic.
 * 	[90/12/13  21:40:37  jsb]
 * 
 * Revision 2.9  90/12/04  14:51:00  jsb
 * 	Added i860 support for Debugger function.
 * 	[90/12/04  11:01:25  jsb]
 * 
 * Revision 2.8  90/11/05  14:30:49  rpd
 * 	Added Assert.
 * 	[90/11/04            rpd]
 * 
 * Revision 2.7  90/10/25  14:45:10  rwd
 * 	Change sun3 debugger invocation.
 * 	[90/10/17            rwd]
 * 
 * Revision 2.6  90/09/09  23:20:09  rpd
 * 	Fixed panic and log to supply cnputc to _doprnt.
 * 	[90/09/09            rpd]
 * 
 * Revision 2.5  90/09/09  14:32:03  rpd
 * 	Use decl_simple_lock_data.
 * 	[90/08/30            rpd]
 * 
 * Revision 2.4  90/08/27  22:02:14  dbg
 * 	Pass extra argument to _doprnt.
 * 	[90/08/21            dbg]
 * 
 * Revision 2.3  90/05/03  15:46:53  dbg
 * 	Added i386 case.
 * 	[90/02/21            dbg]
 * 
 * Revision 2.2  89/11/29  14:09:04  af
 * 	Added mips case, RCS-ed.
 * 	[89/10/28            af]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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

#include <mach_assert.h>
#include <mach_kdb.h>
#include <norma_ipc.h>
#include <cpus.h>

#include <kern/cpu_number.h>
#include <kern/lock.h>
#include <kern/spl.h>
#include <kern/thread.h>
#include <kern/assert.h>
#include <kern/misc_protos.h>
#include <stdarg.h>

int panic_on_assertion_failure = 1;

#if	MACH_KDB
#include <ddb/db_command.h>	/* for db_error() prototype */
extern int db_active;
#endif	/* MACH_KDB */

#if	MACH_ASSERT

int mach_assert = 1;

/* Break into debugger.
 * If debugger is not present, system will panic.
 */

void
Assert(
	const char	*file,
	int		line,
	const char	*expression)
{
	if (! mach_assert) {
		return;
	}
#if NCPUS > 1
	printf("{%d} Assertion failed: file \"%s\", line %d: %s\n", 
	       cpu_number(), file, line, expression);
#else
	printf("Assertion failed: file \"%s\", line %d: %s\n", file, line,
		expression);
#endif
	if (panic_on_assertion_failure) {
#if	MACH_KDB
		if (!db_active)
			Debugger("assertion failure");
		else
			db_error("assertion failure");
#else	/* MACH_KDB */
		Debugger("assertion failure");
#endif	/* MACH_KDB */
	}
}

#endif	/* MACH_ASSERT */

void
Debugger(
	const char	*message)
{
#ifdef	lint
	message++;
#endif	/* lint */

#ifdef	vax
	__asm__("bpt");
#endif	/* vax */
#ifdef	sun
	current_thread()->pcb->flag |= TRACE_KDB;
	__asm__("orw  #0x00008000,sr");
#endif	/* sun */
#ifdef	mips 
	gimmeabreak();
#endif	/* mips */
#ifdef  luna88k
	gimmeabreak();
#endif
#ifdef	i386
	__asm__("int3");
#endif	/* i386 */
#ifdef	i860
	gimmeabreak();
#endif	/* i860 */
#ifdef  PC532
	__asm__("bpt");
#endif  /* PC532 */
}

char			*panicstr;
spinlock_t		panic_lock;
int			paniccpu;
volatile int		panicwait;

void
panic_init(void)
{
	spinlock_init(&panic_lock);
}

/*VARARGS1*/
void
panic(char *str, ...)
{
	va_list	listp;
	spl_t	s = splhigh();	/* do not let other CPUs interrupt */

#if	NORMA_IPC
	extern int _node_self;	/* node_self() may not be callable yet */
#endif	/* NORMA_IPC */

 restart:
	spinlock_lock(&panic_lock);
	if (panicstr) {
	    if (cpu_number() != paniccpu) {
		spinlock_unlock(&panic_lock);
#if	MACH_KDB
		/*
		 * Wait until message has been printed to identify correct
		 * cpu that made the first panic.
		 */
		while (panicwait)
		    continue;
		Debugger("panic");
		goto restart;
	    } else {
		Debugger("Double panic");
		splx(s);
		return;
#else	/* MACH_KDB */
		halt_cpu();
		/* NOTREACHED */
#endif	/* MACH_KDB */
	    }
	}
	panicstr = str;
	paniccpu = cpu_number();
	panicwait = 1;

	spinlock_unlock(&panic_lock);
	printf("panic");
#if	NORMA_IPC
	printf("(node %d)", _node_self);
#endif
#if	NCPUS > 1
	printf("(cpu %d)", (unsigned) paniccpu);
#endif
	printf(": ");
	va_start(listp, str);
	_doprnt((const char *) str, &listp, cnputc, 0);
	va_end(listp);
	printf("\n");

	/*
	 * Release panicwait indicator so that other cpus may call Debugger().
	 */
	panicwait = 0;
#if	MACH_KDB
	Debugger("panic");

	/*
	 * Release panicstr so that we can handle normally other panics.
	 */
	spinlock_lock(&panic_lock);
	panicstr = (char *)0;
	spinlock_unlock(&panic_lock);
	splx(s);
#else	/* MACH_KDB */
	halt_cpu();
	/* NOTREACHED */
#endif	/* MACH_KDB */
}

/*
 * We'd like to use BSD's log routines here...
 */
/*VARARGS2*/
void
log(int level, char *fmt, ...)
{
	va_list	listp;

#ifdef lint
	level++;
#endif /* lint */
	va_start(listp, fmt);
	_doprnt(fmt, &listp, cnputc, 0);
	va_end(listp);
}
