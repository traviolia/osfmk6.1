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
 * Revision 2.7  91/05/14  16:25:55  mrt
 * 	Correcting copyright
 * 
 * Revision 2.6  91/02/05  17:18:33  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:44:42  mrt]
 * 
 * Revision 2.5  90/11/26  14:50:05  rvb
 * 	jsb bet me to XMK34, sigh ...
 * 	[90/11/26            rvb]
 * 	Synched 2.5 & 3.0 at I386q (r1.6.1.5) & XMK35 (r2.5)
 * 	[90/11/15            rvb]
 * 
 * Revision 2.4  90/11/05  14:28:26  rpd
 * 	Create 18 TBD's so that we can copy up a chain of
 * 	mbuf's.
 * 	[90/10/03            rvb]
 * 
 * Revision 1.6.1.4  90/07/27  11:26:20  rvb
 * 	Fix Intel Copyright as per B. Davies authorization.
 * 	[90/07/27            rvb]
 * 
 * Revision 1.6.1.3  90/07/10  11:43:56  rvb
 * 	Fix sram addresses to be relative to sram start.
 * 	[90/06/25            rvb]
 * 
 * Revision 1.6.1.2  90/02/28  15:49:56  rvb
 * 	Fix numerous typo's in Olivetti disclaimer.
 * 	Bump STATUS_TRIES to 15000
 * 	[90/02/28            rvb]
 * 
 * Revision 1.6.1.1  90/01/08  13:31:14  rvb
 * 	Add Intel copyright.
 * 	Add Olivetti copyright.
 * 	[90/01/08            rvb]
 * 
 * Revision 1.6  89/07/17  10:40:51  rvb
 * 	Olivetti Changes to X79 upto 5/9/89:
 * 	[89/07/11            rvb]
 * 
 * Revision 1.5  89/04/05  13:01:44  rvb
 * 	Pulled "softc" structure and moved it to .c file like any other
 * 	"vax" ether driver would do.
 * 	[89/03/07            rvb]
 * 
 * 	collapse pack_u_long_t for gcc.
 * 	[89/03/06            rvb]
 * 
 * Revision 1.4  89/03/09  20:06:15  rpd
 * 	More cleanup.
 * 
 * Revision 1.3  89/02/26  12:42:24  gm0w
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
 *	Olivetti PC586 Mach Ethernet driver v1.0
 *	Copyright Ing. C. Olivetti & C. S.p.A. 1988, 1989
 *	All rights reserved.
 *
 */ 
/*
 *   Copyright 1988, 1989 by Olivetti Advanced Technology Center, Inc.,
 * Cupertino, California.
 * 
 * 		All Rights Reserved
 * 
 *   Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appears in all
 * copies and that both the copyright notice and this permission notice
 * appear in supporting documentation, and that the name of Olivetti
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 * 
 *   OLIVETTI DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL OLIVETTI BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
 * NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUR OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Copyright 1988, 1989 by Intel Corporation, Santa Clara, California.
 * 
 * 		All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appears in all
 * copies and that both the copyright notice and this permission notice
 * appear in supporting documentation, and that the name of Intel
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
 * NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 */

#include	<i386/AT386/i82586.h>	/* chip/board specific defines	*/

#define	STATUS_TRIES	15000
#define	ETHER_ADD_SIZE	6	/* size of a MAC address 		*/
#define ETHER_PCK_SIZE	1500	/* maximum size of an ethernet packet 	*/

#define	PC586_NODE_IA1	0x00	/* 1st PC586 Vendor address byte */
#define	PC586_NODE_IA2	0xAAAA	/* 2nd PC586 Vendor address byte */
#define	PC586_NODE_IA3	0x00	/* 3rd PC586 Vendor address byte */

/*
 * 	Board Specific Defines:
 */

#define OFFSET_NORMMODE	0x3000
#define OFFSET_CHANATT	0x3002
#define OFFSET_RESET   	0x3004
#define OFFSET_INTENAB	0x3006
#define OFFSET_XFERMODE	0x3008
#define OFFSET_SYSTYPE	0x300a
#define OFFSET_INTSTAT	0x300c
#define OFFSET_PROM	0x2000 
#define OFFSET_PROM_IA1	0x2000 
#define OFFSET_PROM_IA2	0x2002 
#define OFFSET_PROM_IA3	0x2004 
#define OFFSET_PROM_IA4	0x2006 
#define OFFSET_PROM_IA5	0x2008 
#define OFFSET_PROM_IA6	0x200a 
#define OFFSET_PROM_CK1	0x203c 
#define OFFSET_PROM_CK2	0x203e 

#define EXTENDED_ADDR	0x20000
#define OFFSET_SCP      (0x7ff6 - 0x4000)
#define OFFSET_ISCP     (0x7fee - 0x4000)
#define OFFSET_SCB      (0x7fde - 0x4000)
#define OFFSET_RU       (0x4000 - 0x4000)
#define OFFSET_RBD	(0x4228 - 0x4000)
#define OFFSET_CU       (0x7814 - 0x4000)

#define OFFSET_TBD      (0x7914 - 0x4000)
#define OFFSET_TBUF     (0x79a4 - 0x4000)
#define N_FD              25  
#define N_RBD             25 
#define N_TBD		  18
#define RCVBUFSIZE       540
#define DL_DEAD         0xffff

#define CMD_0           0     
#define CMD_1           0xffff

#define PC586NULL	0xffff			/* pc586 NULL for lists	*/

#define	DSF_LOCK	1
#define DSF_RUNNING	2

#define MOD_ENAL 1
#define MOD_PROM 2

/*
 * Driver (not board) specific defines and structures:
 */

typedef	struct	{
	rbd_t	r;
	char	rbd_pad[2];
	char	rbuffer[RCVBUFSIZE];
} ru_t;

typedef struct pc_softc	pc_softc_t;

