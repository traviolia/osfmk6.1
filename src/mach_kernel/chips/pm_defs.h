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
 * Revision 2.7  91/08/24  11:52:36  af
 * 	Dropped predefines of frame sizes.
 * 	[91/08/02  02:16:00  af]
 * 
 * Revision 2.6  91/06/19  11:54:04  rvb
 * 	mips->DECSTATION; vax->VAXSTATION
 * 	[91/06/12  14:01:57  rvb]
 * 
 * 	File moved here from mips/PMAX since it tries to be generic;
 * 	it is used on the PMAX and the Vax3100.
 * 	[91/06/04            rvb]
 * 
 * Revision 2.5  91/05/14  17:25:16  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:43:06  mrt
 * 	Added author notices
 * 	[91/02/04  11:15:49  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:14:35  mrt]
 * 
 * Revision 2.3  90/12/05  23:33:14  af
 * 	Cleanups.
 * 	[90/12/03  23:30:06  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:36:24  af
 * 	Created.
 * 	[90/09/03            af]
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
 *	File: pm_defs.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	9/90
 *
 *	Definitions specific to the "pm" simple framebuffer driver,
 *	exported across sub-modules.  Some other framebuffer drivers
 *	that share code with pm use these defs also.
 */

/* Hardware state (to be held in the screen descriptor) */

typedef struct {
	char		*cursor_registers;	/* opaque, for sharing */
	unsigned short	cursor_state;		/* some regs are W-only */
	short		unused;			/* padding, free */
	char		*vdac_registers;	/* opaque, for sharing */
	unsigned char	*framebuffer;
	unsigned char	*plane_mask;
} pm_softc_t;

extern pm_softc_t	*pm_alloc(/* unit, curs, framebuf, planem */);

/* user mapping sizes */
#define	USER_INFO_SIZE	PAGE_SIZE
#define	PMASK_SIZE	PAGE_SIZE
#define	BITMAP_SIZE(sc)						\
	((sc)->frame_height * (((sc)->flags & COLOR_SCREEN) ?	\
		sc->frame_scanline_width :			\
		sc->frame_scanline_width>>3))

#define	PM_SIZE(sc)	(USER_INFO_SIZE+PMASK_SIZE+BITMAP_SIZE(sc))
