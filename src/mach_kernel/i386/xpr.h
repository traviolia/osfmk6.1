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
 * Revision 2.4.9.2  92/04/30  11:51:27  bernadat
 * 	Adaptations for Corollary and Systempro
 * 	[92/04/08            bernadat]
 * 
 * Revision 2.4.9.1  92/02/18  18:50:37  jeffreyh
 * 	Changed time_stamp type to unsigned
 * 	[91/12/06            bernadat]
 * 
 * 	Support for the Corollary MP,
 * 	time_stamp can be implemented by a slave cpu
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.4  91/05/14  16:19:01  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:15:45  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:39:08  mrt]
 * 
 * Revision 2.2  90/05/03  15:38:37  dbg
 * 	First checkin.
 * 
 * Revision 1.3  89/02/26  12:35:32  gm0w
 * 	Changes for cleanup.
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
 
/*
 *	File:	xpr.h
 *
 *	Machine dependent module for the XPR tracing facility.
 */

#include <platforms.h>
#if	AT386 && NCPUS > 1
extern	unsigned time_stamp;
#define	XPR_TIMESTAMP	(int) time_stamp
#else	/* AT386 && NCPUS > 1 */
#define XPR_TIMESTAMP	(0)
#endif	/* AT386 && NCPUS > 1 */
