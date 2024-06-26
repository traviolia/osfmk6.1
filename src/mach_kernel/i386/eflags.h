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
 * Revision 2.4  91/05/14  16:06:35  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:11:26  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:31:41  mrt]
 * 
 * Revision 2.2  90/05/03  15:25:03  dbg
 * 	Created.
 * 	[90/02/08            dbg]
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

#ifndef	_I386_EFLAGS_H_
#define	_I386_EFLAGS_H_

/*
 *	i386 flags register
 */
#define	EFL_CF		0x00000001		/* carry */
#define	EFL_PF		0x00000004		/* parity of low 8 bits */
#define	EFL_AF		0x00000010		/* carry out of bit 3 */
#define	EFL_ZF		0x00000040		/* zero */
#define	EFL_SF		0x00000080		/* sign */
#define	EFL_TF		0x00000100		/* trace trap */
#define	EFL_IF		0x00000200		/* interrupt enable */
#define	EFL_DF		0x00000400		/* direction */
#define	EFL_OF		0x00000800		/* overflow */
#define	EFL_IOPL	0x00003000		/* IO privilege level: */
#define	EFL_IOPL_KERNEL	0x00000000			/* kernel */
#define	EFL_IOPL_USER	0x00003000			/* user */
#define	EFL_NT		0x00004000		/* nested task */
#define	EFL_RF		0x00010000		/* resume without tracing */
#define	EFL_VM		0x00020000		/* virtual 8086 mode */
#define EFL_AC		0x00040000		/* alignment check */
#define EFL_ID		0x00200000		/* cpuID instruction */

#define	EFL_USER_SET	(EFL_IF)
#define	EFL_USER_CLEAR	(EFL_IOPL|EFL_NT|EFL_RF)

#endif	/* _I386_EFLAGS_H_ */
