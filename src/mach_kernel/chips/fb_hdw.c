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
 * Revision 2.2  91/08/24  11:51:55  af
 * 	Created, from .. NO SPECS AT ALL.  A little experimentation
 * 	with ddb, and a lot of past experience did the magic.
 * 	I guess this screen driver really is portable.
 * 	BTW, the cursor is still mirrored, ahem.
 * 	[91/01/25            af]
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
 *	File: fb_hdw.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	7/91
 *
 *	Driver for the 3max Monochrome Frame Buffer Display,
 *	hardware-level operations.
 */

#include <mfb.h>
#if	(NMFB > 0)

#include <machine/machparam.h>
#include <mach/std_types.h>
#include <sys/types.h>
#include <chips/busses.h>

#include <chips/screen_defs.h>

#include <mips/PMAX/pmag_aa.h>
#include <mips/PMAX/tc.h>

#include <chips/pm_defs.h>
typedef pm_softc_t	fb_softc_t;


/*
 * Definition of the driver for the auto-configuration program.
 */

int	fb_probe(), fb_attach(), fb_intr();

caddr_t	fb_std[NMFB] = { 0 };
struct	bus_device *fb_info[NMFB];
struct	bus_driver fb_driver = 
        { fb_probe, 0, fb_attach, 0, fb_std, "fb", fb_info,
	  0, 0, BUS_INTR_DISABLED};

/*
 * Probe/Attach functions
 */

fb_probe( /* reg, ui */)
{
	static probed_once = 0;

	/*
	 * Probing was really done sweeping the TC long ago
	 */
	if (tc_probe("fb") == 0)
		return 0;
	if (probed_once++ > 1)
		printf("[mappable] ");
	return 1;
}

fb_attach(ui)
	struct bus_device *ui;
{
	/* ... */
	printf(": monochrome display");
}


/*
 * Interrupt routine
 */

fb_intr(unit,spllevel)
{
	register volatile char *ack;

	/* acknowledge interrupt */
	ack = (volatile char *) fb_info[unit]->address + FB_OFFSET_IREQ;
	*ack = 0;

	splx(spllevel);
	lk201_led(unit);
}

fb_vretrace(fb, on)
	fb_softc_t	*fb;
{
	int i;

	for (i = 0; i < NMFB; i++)
		if (fb_info[i]->address == (caddr_t)fb->framebuffer)
			break;
	if (i == NMFB) return;

	(*tc_enable_interrupt)(fb_info[i]->adaptor, on, 0);
}

/*
 * Video on/off
 */
fb_video_on(fb, up)
	fb_softc_t	*fb;
	user_info_t	*up;
{
	if (!fb->cursor_state)	/* video is "on" at boot */
		return;
	bt455_video_on(fb->vdac_registers, up);
	bt431_cursor_on(fb->cursor_registers);
	fb->cursor_state = 0;
}

fb_video_off(fb, up)
	fb_softc_t	*fb;
	user_info_t	*up;
{
	if (fb->cursor_state)
		return;
	bt455_video_off(fb->vdac_registers, up);
	bt431_cursor_off(fb->cursor_registers);
	fb->cursor_state = 1;
}

/*
 * Boot time initialization: must make device
 * usable as console asap.
 */
extern int
	fb_soft_reset(), fb_set_status(),
	bt431_pos_cursor(), fb_video_on(),
	fb_video_off(), fb_vretrace(),
	pm_get_status(), pm_char_paint(),
	pm_insert_line(), pm_remove_line(),
	pm_clear_bitmap(), pm_map_page();

static struct screen_switch fb_sw = {
	screen_noop,		/* graphic_open */
	fb_soft_reset,		/* graphic_close */
	fb_set_status,		/* set_status */
	pm_get_status,		/* get_status */
	pm_char_paint,		/* char_paint */
	bt431_pos_cursor,	/* pos_cursor */
	pm_insert_line,		/* insert_line */
	pm_remove_line,		/* remove_line */
	pm_clear_bitmap,	/* clear_bitmap */
	fb_video_on,		/* video_on */
	fb_video_off,	/* video_off */
	fb_vretrace,		/* intr_enable */
	pm_map_page		/* map_page */
};

fb_cold_init(unit, up)
	user_info_t	*up;
{
	fb_softc_t	*fb;
	screen_softc_t	sc = screen(unit);
	int		base = tc_probe("fb");

	bcopy(&fb_sw, &sc->sw, sizeof(sc->sw));
#if 0
	sc->flags |= MONO_SCREEN;
#else
	sc->flags |= COLOR_SCREEN;
#endif
	sc->frame_scanline_width = 2048;
	sc->frame_height = 1024;
	sc->frame_visible_width = 1280;
	sc->frame_visible_height = 1024;

	pm_init_screen_params(sc, up);
	(void) screen_up(unit, up);

	fb = pm_alloc(unit, base+FB_OFFSET_BT431, base+FB_OFFSET_VRAM, -1);
	fb->vdac_registers = (char*) base + FB_OFFSET_BT455;

	screen_default_colors(up);

	fb_soft_reset(sc);

	/*
	 * Clearing the screen at boot saves from scrolling
	 * much, and speeds up booting quite a bit.
	 */
	screen_blitc( unit, 'C'-'@');/* clear screen */
}

#endif	(NMFB > 0)
