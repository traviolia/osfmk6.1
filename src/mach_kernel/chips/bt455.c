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
 * Revision 2.2  91/08/24  11:50:21  af
 * 	Cleaned up a bit, avoid use of poorly documented add_clr register.
 * 	Catched various minor bloopers.
 * 	[91/08/22  11:02:18  af]
 * 
 * 	Created, from Brooktree specs:
 * 	"Graphics And Imaging Product Databook 1991"
 * 	"Bt454/Bt455 170 Mhz Monolithic CMOS 16 Color Palette RAMDAC"
 * 	Brooktree Corp. San Diego, CA, 3rd Ed.
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
 *	File: bt455.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	7/91
 *
 *	Routines for the bt454/bt455 RAMDAC
 */

#include <platforms.h>

#include <chips/bt455.h>
#include <chips/screen.h>

#ifdef	DECSTATION

typedef struct {
	volatile unsigned char	addr_cmap;
	char						pad0[3];
	volatile unsigned char	addr_cmap_data;
	char						pad1[3];
	volatile unsigned char	addr_clr;
	char						pad2[3];
	volatile unsigned char	addr_ovly;
	char						pad3[3];
} bt455_padded_regmap_t;

#else	/*DECSTATION*/

typedef bt455_regmap_t	bt455_padded_regmap_t;
#define	wbflush()

#endif	/*DECSTATION*/


/*
 * Generic register access
 */
#define bt455_select_entry(regs, regno)			\
	{						\
		(regs)->addr_cmap = (regno)&0x0f;	\
		wbflush();				\
	}


/*
 * Color map
 */
bt455_load_colormap( regs, map)
	bt455_padded_regmap_t	*regs;
	color_map_t	*map;
{
	register int    i;

	bt455_select_entry(regs, 0);

	for (i = 0; i < 16; i++, map++) {
		regs->addr_cmap_data = map->red >> 4;
		wbflush();
		regs->addr_cmap_data = map->green >> 4;
		wbflush();
		regs->addr_cmap_data = map->blue >> 4;
		wbflush();
	}
}

bt455_load_colormap_entry( regs, entry, map)
	register bt455_padded_regmap_t	*regs;
	register color_map_t	*map;
{
	bt455_select_entry(regs, entry);

	regs->addr_cmap_data = map->red >> 4;
	wbflush();
	regs->addr_cmap_data = map->green >> 4;
	wbflush();
	regs->addr_cmap_data = map->blue >> 4;
	wbflush();
}

bt455_init_colormap( regs)
	bt455_padded_regmap_t	*regs;
{
	register int    i;
	color_map_t	m[2];

	m[0].red = m[0].green = m[0].blue = 0;
	m[1].red = m[1].green = m[1].blue = 0xff;

	for (i = 0; i < 16; i++)
		bt455_load_colormap_entry(regs, i, &m[0]);

	bt455_load_colormap_entry(regs, 1, &m[1]);

	bt455_cursor_color( regs, &m[0]);
}

#if	1/*debug*/
bt455_print_colormap( regs)
	bt455_padded_regmap_t	*regs;
{
	register int    i;

	for (i = 0; i < 16; i++) {
		register unsigned char red, green, blue;

		bt455_select_entry(regs, i);
		red   = regs->addr_cmap_data;
		green = regs->addr_cmap_data;
		blue  = regs->addr_cmap_data;
		printf("%x->[x%x x%x x%x]\n", i,
			red, green, blue);

	}
}
#endif

/*
 * Video on/off
 */
bt455_video_off(regs, up)
	bt455_padded_regmap_t	*regs;
	user_info_t	*up;
{
	color_map_t		m;
	unsigned char		*save;

				/* Yes, this is awful */
	save = (unsigned char *)up->dev_dep_2.gx.colormap;

	bt455_select_entry( regs, 0);

	*save++ = regs->addr_cmap_data;		/* entry 0 */
	*save++ = regs->addr_cmap_data;
	*save++ = regs->addr_cmap_data;

	*save++ = regs->addr_cmap_data;		/* entry 1 */
	*save++ = regs->addr_cmap_data;
	*save++ = regs->addr_cmap_data;

	m.red = m.green = m.blue = 0;
	bt455_load_colormap_entry(regs, 0, &m);
	bt455_load_colormap_entry(regs, 1, &m);
}

bt455_video_on(regs, up)
	bt455_padded_regmap_t	*regs;
	user_info_t	*up;
{
	unsigned char		*save;

				/* Like I said.. */
	save = (unsigned char *)up->dev_dep_2.gx.colormap;

	bt455_select_entry( regs, 0);

	regs->addr_cmap_data = *save++;	wbflush();
	regs->addr_cmap_data = *save++;	wbflush();
	regs->addr_cmap_data = *save++;	wbflush();

	regs->addr_cmap_data = *save++;	wbflush();
	regs->addr_cmap_data = *save++;	wbflush();
	regs->addr_cmap_data = *save;

}

/*
 * Cursor 'color' [as used on DEC's board]
 */
bt455_cursor_color( regs, color)
	bt455_padded_regmap_t	*regs;
	color_map_t	*color;
{
	register int    i;

	/* Bg is the first in color */
	bt455_load_colormap_entry( regs, 8, color);
	bt455_load_colormap_entry( regs, 9, color);

	/* Fg is overlay */
	color++;
	regs->addr_ovly = color->red >> 4;
	wbflush();
	regs->addr_ovly = color->green >> 4;
	wbflush();
	regs->addr_ovly = color->blue >> 4;
	wbflush();
}

/*
 * Initialization
 */
bt455_init(regs)
	bt455_padded_regmap_t	*regs;
{
	/* Nothing really needed */
}

