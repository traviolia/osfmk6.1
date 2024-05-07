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
 * Revision 2.5.3.3  92/06/24  17:58:17  jeffreyh
 * 	Fixed MCOUNT macro for __STDC__ [condict]
 * 
 * Revision 2.5.3.2  92/03/28  10:05:11  jeffreyh
 * 	Changes from MK71
 * 	[92/03/20  12:11:56  jeffreyh]
 * 
 * Revision 2.7  92/02/29  15:33:41  rpd
 * 	Added ENTRY2.
 * 	[92/02/28            rpd]
 * 
 * Revision 2.6  92/02/19  15:07:52  elf
 * 	Changed #if __STDC__ to #ifdef __STDC__
 * 	[92/01/16            jvh]
 * 
 * Revision 2.5  91/05/14  16:02:45  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:10:42  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:30:29  mrt]
 * 
 * Revision 2.3  90/12/20  16:35:27  jeffreyh
 * 	changes for __STDC__
 * 	[90/12/06            jeffreyh]
 * 
 * Revision 2.2  90/05/03  15:24:12  dbg
 * 	First checkin.
 * 
 *
 * 	Typo on ENTRY if gprof
 * 	[90/03/29            rvb]
 * 
 * 	fix SVC for "ifdef wheeze" [kupfer]
 * 	Fix the GPROF definitions.
 * 	ENTRY(x) gets profiled iffdef GPROF.
 * 	Entry(x) (and DATA(x)) is NEVER profiled.
 * 	MCOUNT can be used by asm that intends to build a frame,
 * 	after the frame is built.
 *	[90/02/26            rvb]
 *
 * 	Add #define addr16 .byte 0x67
 * 	[90/02/09            rvb]
 * 	Added LBi, SVC and ENTRY
 * 	[89/11/10  09:51:33  rvb]
 * 
 * 	New a.out and coff compatible .s files.
 * 	[89/10/16            rvb]
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

#ifndef	_I386_ASM_H_
#define	_I386_ASM_H_

#ifdef	MACH_KERNEL
#include <mach_kdb.h>
#include <gprof.h>
#else	/* MACH_KERNEL */
#define	MACH_KDB 0
#endif	/* MACH_KERNEL */


#define S_ARG0	 4(%esp)
#define S_ARG1	 8(%esp)
#define S_ARG2	12(%esp)
#define S_ARG3	16(%esp)

#define FRAME	pushl %ebp; movl %esp, %ebp
#define EMARF	leave

#define B_ARG0	 8(%ebp)
#define B_ARG1	12(%ebp)
#define B_ARG2	16(%ebp)
#define B_ARG3	20(%ebp)

/* There is another definition of ALIGN for .c sources */
#ifdef __LANGUAGE_ASSEMBLY
#define ALIGN 2
#endif /* __LANGUAGE_ASSEMBLY */

#define LB(x,n) n
#if	__STDC__
#ifndef __NO_UNDERSCORES__
#define	LCL(x)	x
#define EXT(x) _ ## x
#define LEXT(x) _ ## x ## :
#else
#define	LCL(x)	_L_ ## x
#define EXT(x) x
#define LEXT(x) x ## :
#endif
#define LBc(x,n) n ## :
#define LBb(x,n) n ## b
#define LBf(x,n) n ## f
#else /* __STDC__ */
#ifndef __NO_UNDERSCORES__
#define LCL(x) x
#define EXT(x) _/**/x
#define LEXT(x) _/**/x/**/:
#else /* __NO_UNDERSCORES__ */
#define	LCL(x)	_L_/**/x
#define EXT(x) x
#define LEXT(x) x/**/:
#endif /* __NO_UNDERSCORES__ */
#define LBc(x,n) n/**/:
#define LBb(x,n) n/**/b
#define LBf(x,n) n/**/f
#endif /* __STDC__ */

#define SVC .byte 0x9a; .long 0; .word 0x7

#define RPC_SVC .byte 0x9a; .long 0; .word 0xf

#define String	.asciz
#define Value	.word
#define Times(a,b) (a*b)
#define Divide(a,b) (a/b)

#define INB	inb	%dx, %al
#define OUTB	outb	%al, %dx
#define INL	inl	%dx, %eax
#define OUTL	outl	%eax, %dx

#define data16	.byte 0x66
#define addr16	.byte 0x67

#if	GPROF
#define MCOUNT		.data; LBc(x, 9) .long 0; .text; lea LBb(x, 9),%edx; call EXT(mcount)
#if	__STDC__

#define	ENTRY(x)	.globl EXT(x); .align ALIGN; LEXT(x) ; \
			pushl %ebp; movl %esp, %ebp; MCOUNT; popl %ebp;
#define	ENTRY2(x,y)	.globl EXT(x); .globl EXT(y); \
			.align ALIGN; LEXT(x) LEXT(y) ; \
			pushl %ebp; movl %esp, %ebp; MCOUNT; popl %ebp;
#define	ASENTRY(x) 	.globl x; .align ALIGN; x ## : ; \
			pushl %ebp; movl %esp, %ebp; MCOUNT; popl %ebp;

#else   /* __STDC__ */

#define	ENTRY(x)	.globl EXT(x); .align ALIGN; LEXT(x) ; \
			pushl %ebp; movl %esp, %ebp; MCOUNT; popl %ebp;
#define	ENTRY2(x,y)	.globl EXT(x); .globl EXT(y); \
			.align ALIGN; LEXT(x) LEXT(y)
#define	ASENTRY(x) 	.globl x; .align ALIGN; x: ; \
  			pushl %ebp; movl %esp, %ebp; MCOUNT; popl %ebp;

#endif	/* __STDC__ */
#else	/* GPROF */
#if	__STDC__
#define MCOUNT
#define	ENTRY(x)	.globl EXT(x); .align ALIGN; LEXT(x)
#define	ENTRY2(x,y)	.globl EXT(x); .globl EXT(y); \
			.align ALIGN; LEXT(x) LEXT(y)
#define	ASENTRY(x)	.globl x; .align ALIGN; x ## :

#else 	/* __STDC__ */

#define MCOUNT
#define	ENTRY(x)	.globl EXT(x); .align ALIGN; LEXT(x)
#define	ENTRY2(x,y)	.globl EXT(x); .globl EXT(y); \
			.align ALIGN; LEXT(x) LEXT(y)
#define	ASENTRY(x)	.globl x; .align ALIGN; x:

#endif	/* __STDC__ */
#endif	/* GPROF */

#define	Entry(x)	.globl EXT(x); .align ALIGN; LEXT(x)
#define	DATA(x)		.globl EXT(x); .align ALIGN; LEXT(x)

#ifdef __LANGUAGE_ASSEMBLY
#if	MACH_KDB
#include <ddb/stab.h>
/*
 * This pseudo-assembler line is added so that there will be at least
 *	one N_SO entry in the symbol stable to define the current file name.
 */
.stabs ,N_SO,0,0,Ltext0
Ltext0:
#endif	/* MACH_KDB */

#else /* NO __LANGUAGE_ASSEMBLY */

/* These defines are here for .c files that wish to reference global symbols
 * within __asm__ statements. 
 */
#ifndef __NO_UNDERSCORES__
#define CC_SYM_PREFIX "_"
#else
#define CC_SYM_PREFIX ""
#endif /* __NO_UNDERSCORES__ */
#endif /* __LANGUAGE_ASSEMBLY */

#endif /* _I386_ASM_H_ */

