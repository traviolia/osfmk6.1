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
 * Revision 2.7  91/08/24  11:51:38  af
 * 	Moved padding of regmap here.  Rid of "struct pcc" hack, now
 * 	we understand a pm_softc, and compatibles.
 * 	[91/08/02  02:27:53  af]
 * 
 * Revision 2.6  91/06/19  11:47:22  rvb
 * 	mips->DECSTATION; vax->VAXSTATION
 * 	[91/06/12  14:01:22  rvb]
 * 
 * 	File moved here from mips/PMAX since it tries to be generic;
 * 	it is used on the PMAX and the Vax3100.
 * 	[91/06/04            rvb]
 * 
 * Revision 2.5  91/05/14  17:20:33  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:40:14  mrt
 * 	Added author notices
 * 	[91/02/04  11:12:34  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:10:24  mrt]
 * 
 * Revision 2.3  90/12/05  23:30:42  af
 * 	Works now.
 * 	[90/12/03  23:12:48  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:43:21  af
 * 	Created, from the DEC specs:
 * 	"DECstation 3100 Desktop Workstation Functional Specification"
 * 	Workstation Systems Engineering, Palo Alto, CA. Aug 28, 1990.
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
 *	File: dc503.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	9/90
 *
 *	Routines for the DEC DC503 Programmable Cursor Chip
 */
#include <platforms.h>

#include <chips/pm_defs.h>
#include <chips/dc503.h>


#if	defined(DECSTATION) || defined(VAXSTATION)

typedef struct {
	volatile unsigned short	pcc_cmdr;	/* all regs are wo */
	short						pad0;
	volatile unsigned short	pcc_xpos;
	short						pad1;
	volatile unsigned short	pcc_ypos;
	short						pad2;
	volatile unsigned short	pcc_xmin1;
	short						pad3;
	volatile unsigned short	pcc_xmax1;
	short						pad4;
	volatile unsigned short	pcc_ymin1;
	short						pad5;
	volatile unsigned short	pcc_ymax1;
	short						pad6[9];
	volatile unsigned short	pcc_xmin2;
	short						pad7;
	volatile unsigned short	pcc_xmax2;
	short						pad8;
	volatile unsigned short	pcc_ymin2;
	short						pad9;
	volatile unsigned short	pcc_ymax2;
	short						pad10;
	volatile unsigned short	pcc_memory;
	short						pad11;
} dc503_padded_regmap_t;

#else

typedef dc503_regmap_t	dc503_padded_regmap_t;

#endif

#ifdef	DECSTATION
#define X_CSHIFT 212
#define Y_CSHIFT 34
#define PCC_STATE (DC503_CMD_ENPA | DC503_CMD_ENPB)
#endif	/*DECSTATION*/

#ifdef	VAXSTATION
#define X_CSHIFT 216
#define Y_CSHIFT 34
#define wbflush()
#define PCC_STATE (DC503_CMD_ENPA | DC503_CMD_ENPB | DC503_CMD_HSHI)
#endif	/*VAXSTATION*/

/*
 * Cursor
 */
dc503_pos_cursor( regs, x, y)
	dc503_padded_regmap_t	*regs;
{
	regs->pcc_xpos = x + X_CSHIFT;
	regs->pcc_ypos = y + Y_CSHIFT;
	wbflush();
}

dc503_load_cursor( pm, cursor)
	pm_softc_t	*pm;
	unsigned short	*cursor;
{
	dc503_padded_regmap_t	*regs;
	register int    i;

	regs = (dc503_padded_regmap_t*)pm->cursor_registers;

	pm->cursor_state |= DC503_CMD_LODSA;
	regs->pcc_cmdr = pm->cursor_state;
	wbflush();
	for (i = 0; i < 32; i++) {
		regs->pcc_memory = *cursor++;
		wbflush();
	}
	pm->cursor_state &= ~DC503_CMD_LODSA;
	regs->pcc_cmdr = pm->cursor_state;
}


unsigned short dc503_default_cursor[16+16] = {
/* Plane A */
	0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
	0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
/* Plane B */
	0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
	0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff
};

/*
 * Vert retrace interrupt
 */
dc503_vretrace( pm, on)
	pm_softc_t	*pm;
{
	if (on)
		pm->cursor_state |= DC503_CMD_ENRG2;
	else
		pm->cursor_state &= ~DC503_CMD_ENRG2;
	((dc503_padded_regmap_t*)pm->cursor_registers)->pcc_cmdr = pm->cursor_state;
}

/*
 * Video on/off
 */
dc503_video_on( pm, up)
	pm_softc_t	*pm;
{
	pm->cursor_state = DC503_CMD_ENPA | (pm->cursor_state & ~DC503_CMD_FOPB);
	((dc503_padded_regmap_t*)pm->cursor_registers)->pcc_cmdr = pm->cursor_state;
}

dc503_video_off( pm, up)
	pm_softc_t	*pm;
{
	pm->cursor_state = DC503_CMD_FOPB | (pm->cursor_state & ~DC503_CMD_ENPA);
	((dc503_padded_regmap_t*)pm->cursor_registers)->pcc_cmdr = pm->cursor_state;
}


/*
 * Initialization
 */
dc503_init( pm )
	pm_softc_t	*pm;
{
	dc503_padded_regmap_t	*regs;

	regs = (dc503_padded_regmap_t*)pm->cursor_registers;

	dc503_load_cursor( pm, dc503_default_cursor);
	dc503_pos_cursor( regs, 0, 0);	/* XXX off screen */

	regs->pcc_xmin1 = 0;	/* test only */
	regs->pcc_xmax1 = 0;
	regs->pcc_ymin1 = 0;
	regs->pcc_ymax1 = 0;

	regs->pcc_xmin2 = 212;	/* vert retrace detector */
	regs->pcc_xmax2 = 212+1023;
	regs->pcc_ymin2 = 34+863;
	regs->pcc_ymax2 = 34+863;

#if 0
	regs->pcc_cmdr = DC503_CMD_FOPB | DC503_CMD_VBHI;/* reset */
#endif
	pm->cursor_state = PCC_STATE;
	regs->pcc_cmdr = pm->cursor_state;
}
