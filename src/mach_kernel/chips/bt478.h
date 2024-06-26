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
 * Revision 2.7  91/08/24  11:50:37  af
 * 	Moved padding of regmap to impl file.
 * 	[91/08/02  02:06:41  af]
 * 
 * Revision 2.6  91/06/19  11:46:15  rvb
 * 	File moved here from mips/PMAX since it tries to be generic.
 * 	[91/06/04            rvb]
 * 
 * Revision 2.5  91/05/14  17:19:43  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:39:55  mrt
 * 	Added author notices
 * 	[91/02/04  11:12:11  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:09:53  mrt]
 * 
 * Revision 2.3  90/12/05  23:30:30  af
 * 	Works on pmaxen.
 * 	[90/12/03  23:10:15  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:37:05  af
 * 	Created, from DEC specs and Brooktree data sheets:
 * 	"Product Databook 1989"
 * 	"Bt478 80 MHz 256 Color Palette RAMDAC"
 * 	Brooktree Corp. San Diego, CA
 * 	LA78001 Rev. M
 * 	[90/10/10            af]
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
 *	File: bt478.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/90
 *
 *	Defines for the bt478 Cursor/RAMDAC chip
 */

typedef struct {
	volatile unsigned char	addr_mapwa;
	volatile unsigned char	addr_map;
	volatile unsigned char	addr_mask;
	volatile unsigned char	addr_mapra;
	volatile unsigned char	addr_overwa;
	volatile unsigned char	addr_over;
	volatile unsigned char	addr_xxxx;
	volatile unsigned char	addr_overra;
} bt478_regmap_t;

