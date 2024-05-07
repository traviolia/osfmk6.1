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
 * Revision 2.3  91/05/14  16:51:06  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/02/05  17:31:38  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:16:36  mrt]
 * 
 * Revision 2.1  89/08/03  15:59:35  rwd
 * Created.
 * 
 * Revision 2.4  89/02/25  18:12:08  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 2.3  89/02/07  00:51:34  mwyoung
 * Relocated from sys/boolean.h
 * 
 * Revision 2.2  88/08/24  02:23:06  mwyoung
 * 	Adjusted include file references.
 * 	[88/08/17  02:09:46  mwyoung]
 * 
 *
 * 18-Nov-87  Avadis Tevanian (avie) at Carnegie-Mellon University
 *	Header file fixup, purge history.
 *
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
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
 *	File:	mach/boolean.h
 *
 *	Boolean data type.
 *
 */

#ifndef	BOOLEAN_H_
#define	BOOLEAN_H_

/*
 *	Pick up "boolean_t" type definition
 */

#ifndef	ASSEMBLER
#include <mach/machine/boolean.h>
#endif	/* ASSEMBLER */

/*
 *	Define TRUE and FALSE, only if they haven't been before,
 *	and not if they're explicitly refused.
 */

#ifndef	NOBOOL

#ifndef	TRUE
#define TRUE	1
#endif	/* TRUE */

#ifndef	FALSE
#define FALSE	0
#endif	/* FALSE */

#endif	/* !defined(NOBOOL) */

#endif	/* BOOLEAN_H_ */
