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
 * Revision 2.2  91/08/24  11:50:17  af
 * 	Created, from Brooktree specs:
 * 	"Product Databook 1989"
 * 	"Bt431 Monolithic CMOS 64x64 Pixel Cursor Generator"
 * 	Brooktree Corp. San Diego, CA
 * 	LA59001 Rev. J
 * 	[91/07/25            af]
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
 *	File: bt431.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	8/91
 *
 *	Defines for the bt431 Cursor chip
 */


typedef struct {
	volatile unsigned char	addr_lo;
	volatile unsigned char	addr_hi;
	volatile unsigned char	addr_cmap;
	volatile unsigned char	addr_reg;
} bt431_regmap_t;

/*
 * Additional registers addressed indirectly
 */

#define	BT431_REG_CMD		0x0000
#define	BT431_REG_CXLO		0x0001
#define	BT431_REG_CXHI		0x0002
#define	BT431_REG_CYLO		0x0003
#define	BT431_REG_CYHI		0x0004
#define	BT431_REG_WXLO		0x0005
#define	BT431_REG_WXHI		0x0006
#define	BT431_REG_WYLO		0x0007
#define	BT431_REG_WYHI		0x0008
#define	BT431_REG_WWLO		0x0009
#define	BT431_REG_WWHI		0x000a
#define	BT431_REG_WHLO		0x000b
#define	BT431_REG_WHHI		0x000c

#define BT431_REG_CRAM_BASE	0x0000
#define BT431_REG_CRAM_END	0x01ff

/*
 * Command register
 */

#define BT431_CMD_CURS_ENABLE	0x40
#define BT431_CMD_XHAIR_ENABLE	0x20
#define BT431_CMD_OR_CURSORS	0x10
#define BT431_CMD_AND_CURSORS	0x00
#define BT431_CMD_1_1_MUX	0x00
#define BT431_CMD_4_1_MUX	0x04
#define BT431_CMD_5_1_MUX	0x08
#define BT431_CMD_xxx_MUX	0x0c
#define BT431_CMD_THICK_1	0x00
#define BT431_CMD_THICK_3	0x01
#define BT431_CMD_THICK_5	0x02
#define BT431_CMD_THICK_7	0x03
