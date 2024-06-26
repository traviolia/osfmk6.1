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
 * Revision 2.9  91/08/24  11:51:31  af
 * 	Works on 3mins too.  New calling seq, declare screen sizes.
 * 	[91/08/02  01:59:46  af]
 * 
 * Revision 2.8  91/06/25  20:53:24  rpd
 * 	Tweaks to make gcc happy.
 * 
 * Revision 2.7  91/06/19  16:57:45  rvb
 * 		File moved here from mips/PMAX since it tries to be generic.
 * 	[91/06/04            rvb]
 * 
 * Revision 2.6  91/05/14  17:20:02  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/02/14  14:33:38  mrt
 * 	In interrupt routine, drop priority as now required.
 * 	[91/02/12  12:44:36  af]
 * 
 * Revision 2.4  91/02/05  17:40:04  mrt
 * 	Added author notices
 * 	[91/02/04  11:12:22  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:10:07  mrt]
 * 
 * Revision 2.3  90/12/05  23:30:35  af
 * 	Cleaned up.
 * 	[90/12/03  23:11:53  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:42:09  af
 * 	Created, from the DEC specs:
 * 	"PMAG-BA TURBOchannel Color Frame Buffer Functional Specification"
 * 	Workstation Systems Engineering, Palo Alto, CA. Aug 27, 1990
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
 *	File: cfb_hdw.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	9/90
 *
 *	Driver for the 3max Color Frame Buffer Display,
 *	hardware-level operations.
 */

#include <cfb.h>
#if	(NCFB > 0)

#include <machine/machparam.h>
#include <mach/std_types.h>
#include <sys/types.h>
#include <chips/busses.h>

#include <chips/screen_defs.h>

#include <mips/PMAX/pmag_ba.h>
#include <mips/PMAX/tc.h>

#include <chips/pm_defs.h>
#include <machine/machparam.h>

typedef pm_softc_t	cfb_softc_t;


/*
 * Definition of the driver for the auto-configuration program.
 */

int	cfb_probe(), cfb_attach(), cfb_intr();

caddr_t	cfb_std[NCFB] = { 0 };
struct	bus_device *cfb_info[NCFB];
struct	bus_driver cfb_driver = 
        { cfb_probe, 0, cfb_attach, 0, cfb_std, "cfb", cfb_info,
	  0, 0, BUS_INTR_DISABLED};

/*
 * Probe/Attach functions
 */

cfb_probe( /* reg, ui */)
{
	static probed_once = 0;

	/*
	 * Probing was really done sweeping the TC long ago
	 */
	if (tc_probe("cfb") == 0)
		return 0;
	if (probed_once++ > 1)
		printf("[mappable] ");
	return 1;
}

cfb_attach(ui)
	struct bus_device *ui;
{
	/* ... */
	printf(": color display");
}


/*
 * Interrupt routine
 */

cfb_intr(unit,spllevel)
{
	register volatile char *ack;

	/* acknowledge interrupt */
	ack = (volatile char *) cfb_info[unit]->address + CFB_OFFSET_IREQ;
	*ack = 0;

	splx(spllevel);
	lk201_led(unit);
}

cfb_vretrace(cfb, on)
	cfb_softc_t	*cfb;
{
	int i;

	for (i = 0; i < NCFB; i++)
		if (cfb_info[i]->address == (caddr_t)cfb->framebuffer)
			break;
	if (i == NCFB) return;

	(*tc_enable_interrupt)(cfb_info[i]->adaptor, on, 0);
}

/*
 * Boot time initialization: must make device
 * usable as console asap.
 */
extern int
	cfb_soft_reset(), cfb_set_status(),
	bt459_pos_cursor(), bt459_video_on(),
	bt459_video_off(), cfb_vretrace(),
	pm_get_status(), pm_char_paint(),
	pm_insert_line(), pm_remove_line(),
	pm_clear_bitmap(), pm_map_page();

static struct screen_switch cfb_sw = {
	screen_noop,		/* graphic_open */
	cfb_soft_reset,		/* graphic_close */
	cfb_set_status,		/* set_status */
	pm_get_status,		/* get_status */
	pm_char_paint,		/* char_paint */
	bt459_pos_cursor,	/* pos_cursor */
	pm_insert_line,		/* insert_line */
	pm_remove_line,		/* remove_line */
	pm_clear_bitmap,	/* clear_bitmap */
	bt459_video_on,		/* video_on */
	bt459_video_off,	/* video_off */
	cfb_vretrace,		/* intr_enable */
	pm_map_page		/* map_page */
};

cfb_cold_init(unit, up)
	user_info_t	*up;
{
	cfb_softc_t	*cfb;
	screen_softc_t	sc = screen(unit);
	int		base = tc_probe("cfb");

	bcopy(&cfb_sw, &sc->sw, sizeof(sc->sw));
	sc->flags |= COLOR_SCREEN;
	sc->frame_scanline_width = 1024;
	sc->frame_height = 1024;
	sc->frame_visible_width = 1024;
	sc->frame_visible_height = 864;

	pm_init_screen_params(sc,up);
	(void) screen_up(unit, up);

	cfb = pm_alloc(unit, base + CFB_OFFSET_BT459, base + CFB_OFFSET_VRAM, -1);

	screen_default_colors(up);

	cfb_soft_reset(sc);

	/*
	 * Clearing the screen at boot saves from scrolling
	 * much, and speeds up booting quite a bit.
	 */
	screen_blitc( unit, 'C'-'@');/* clear screen */
}

#endif	(NCFB > 0)
