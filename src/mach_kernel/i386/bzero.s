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
 * Revision 2.4  91/05/14  16:04:14  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:10:53  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:30:45  mrt]
 * 
 * Revision 2.2  90/05/03  15:24:56  dbg
 * 	From Bob Baron.
 * 	[90/04/30            dbg]
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

#include <i386/asm.h>

/*
 * void *memset(void * addr, int pattern, size_t length)
 */

ENTRY(memset)
	pushl	%edi
	movl	4+ 4(%esp),%edi		/* addr */
	movb	4+ 8(%esp),%al		/* pattern */
	movl	4+ 12(%esp),%edx	/* length */
	movb	%al,%ah
	movw	%ax,%cx
	shll	$16,%eax
	movw	%cx,%ax	
	cld
/ zero longs
	movl	%edx,%ecx
	shrl	$2,%ecx
	rep
	stosl
/ zero bytes
	movl	%edx,%ecx
	andl	$3,%ecx
	rep
	stosb
	movl	4+ 4(%esp),%eax		/* returns its first argument */
	popl	%edi
	ret

/*
 * void bzero(char * addr, unsigned int length)
 */
Entry(blkclr)
ENTRY(bzero)
	pushl	%edi
	movl	4+ 4(%esp),%edi		/* addr */
	movl	4+ 8(%esp),%edx		/* length */
	xorl	%eax,%eax
	cld
/ zero longs
	movl	%edx,%ecx
	shrl	$2,%ecx
	rep
	stosl
/ zero bytes
	movl	%edx,%ecx
	andl	$3,%ecx
	rep
	stosb
	popl	%edi
	ret
