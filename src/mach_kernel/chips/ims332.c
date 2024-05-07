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
/* CMU_HIST */
/*
 * Revision 2.5  92/05/22  15:48:03  jfriedl
 * 	Some fields in user_info_t got renamed.
 * 	[92/05/13  20:40:39  af]
 * 
 * Revision 2.4  92/05/05  10:04:55  danner
 * 	Unconditionally do the reset at init time.
 * 	Fixed blooper in loading cursor colors.
 * 	[92/04/14  11:53:29  af]
 * 
 * Revision 2.3  92/03/05  11:36:47  rpd
 * 	Got real specs ( thanks Jukki!! ):
 * 	"IMS G332 colour video controller"
 * 	1990 Databook, pp 139-163, Inmos Ltd.
 * 	[92/03/03            af]
 * 
 * Revision 2.2  92/03/02  18:32:50  rpd
 * 	Created stub copy to make things compile.
 * 	[92/03/02            af]
 * 
 */
/* CMU_ENDHIST */
/*
 *	File: ims332.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	1/92
 *
 *	Routines for the Inmos IMS-G332 Colour video controller
 */

#include <platforms.h>

#include <chips/ims332.h>
#include <chips/screen.h>

/*
 * Generic register access
 */
typedef volatile unsigned char *ims332_padded_regmap_t;

#ifdef	MAXINE

unsigned int
ims332_read_register(regs, regno)
	unsigned char	*regs;
{
	unsigned char		*rptr;
	register unsigned int	val, v1;

	/* spec sez: */
	rptr = regs + 0x80000 + (regno << 4);
	val = * ((volatile unsigned short *) rptr );
	v1  = * ((volatile unsigned short *) regs );

	return (val & 0xffff) | ((v1 & 0xff00) << 8);
}

ims332_write_register(regs, regno, val)
	unsigned char		*regs;
	register unsigned int	val;
{
	unsigned char		*wptr;

	/* spec sez: */
	wptr = regs + 0xa0000 + (regno << 4);
	* ((volatile unsigned int *)(regs))   = (val >> 8) & 0xff00;
	* ((volatile unsigned short *)(wptr)) = val;
}

#define	assert_ims332_reset_bit(r)	*r &= ~0x40
#define	deassert_ims332_reset_bit(r)	*r |=  0x40

#else	/*MAXINE*/

#define	ims332_read_register(p,r)			\
		((unsigned int *)(p)) [ (r) ]
#define	ims332_write_register(p,r,v)			\
		((unsigned int *)(p)) [ (r) ] = (v)

#endif	/*MAXINE*/


/*
 * Color map
 */
ims332_load_colormap( regs, map)
	ims332_padded_regmap_t	*regs;
	color_map_t	*map;
{
	register int    i;

	for (i = 0; i < 256; i++, map++)
		ims332_load_colormap_entry(regs, i, map);
}

ims332_load_colormap_entry( regs, entry, map)
	ims332_padded_regmap_t	*regs;
	color_map_t	*map;
{
	/* ?? stop VTG */
	ims332_write_register(regs, IMS332_REG_LUT_BASE + (entry & 0xff),
			      (map->red << 16) |
			      (map->green << 8) |
			      (map->blue));
}

ims332_init_colormap( regs)
	ims332_padded_regmap_t	*regs;
{
	color_map_t		m;

	m.red = m.green = m.blue = 0;
	ims332_load_colormap_entry( regs, 0, &m);

	m.red = m.green = m.blue = 0xff;
	ims332_load_colormap_entry( regs, 1, &m);
	ims332_load_colormap_entry( regs, 255, &m);

	/* since we are at it, also fix cursor LUT */
	ims332_load_colormap_entry( regs, IMS332_REG_CURSOR_LUT_0, &m);
	ims332_load_colormap_entry( regs, IMS332_REG_CURSOR_LUT_1, &m);
	/* *we* do not use this, but the prom does */
	ims332_load_colormap_entry( regs, IMS332_REG_CURSOR_LUT_2, &m);
}

#if	1/*debug*/
ims332_print_colormap( regs)
	ims332_padded_regmap_t	*regs;
{
	register int    i;

	for (i = 0; i < 256; i++) {
		register unsigned int	color;

		color = ims332_read_register( regs, IMS332_REG_LUT_BASE + i);
		printf("%x->[x%x x%x x%x]\n", i,
			(color >> 16) & 0xff,
			(color >> 8) & 0xff,
			color & 0xff);
	}
}
#endif

/*
 * Video on/off
 *
 * It is unfortunate that X11 goes backward with white@0
 * and black@1.  So we must stash away the zero-th entry
 * and fix it while screen is off.  Also must remember
 * it, sigh.
 */
struct vstate {
	ims332_padded_regmap_t	*regs;
	unsigned short	off;
};

ims332_video_off(vstate, up)
	struct vstate	*vstate;
	user_info_t	*up;
{
	register ims332_padded_regmap_t	*regs = vstate->regs;
	register unsigned		*save, csr;

	if (vstate->off)
		return;

	/* Yes, this is awful */
	save = (unsigned *)up->dev_dep_2.gx.colormap;

	*save = ims332_read_register(regs, IMS332_REG_LUT_BASE);

	ims332_write_register(regs, IMS332_REG_LUT_BASE, 0);

	ims332_write_register( regs, IMS332_REG_COLOR_MASK, 0);

	/* cursor now */
	csr = ims332_read_register(regs, IMS332_REG_CSR_A);
	csr |= IMS332_CSR_A_DISABLE_CURSOR;
	ims332_write_register(regs, IMS332_REG_CSR_A, csr);

	vstate->off = 1;
}

ims332_video_on(vstate, up)
	struct vstate	*vstate;
	user_info_t	*up;
{
	register ims332_padded_regmap_t	*regs = vstate->regs;
	register unsigned		*save, csr;

	if (!vstate->off)
		return;

	/* Like I said.. */
	save = (unsigned *)up->dev_dep_2.gx.colormap;

	ims332_write_register(regs, IMS332_REG_LUT_BASE, *save);

	ims332_write_register( regs, IMS332_REG_COLOR_MASK, 0xffffffff);

	/* cursor now */
	csr = ims332_read_register(regs, IMS332_REG_CSR_A);
	csr &= ~IMS332_CSR_A_DISABLE_CURSOR;
	ims332_write_register(regs, IMS332_REG_CSR_A, csr);

	vstate->off = 0;
}

/*
 * Cursor
 */
ims332_pos_cursor(regs,x,y)
	ims332_padded_regmap_t	*regs;
	register int	x,y;
{
	ims332_write_register( regs, IMS332_REG_CURSOR_LOC,
		((x & 0xfff) << 12) | (y & 0xfff) );
}


ims332_cursor_color( regs, color)
	ims332_padded_regmap_t	*regs;
	color_map_t	*color;
{
	/* Bg is color[0], Fg is color[1] */
	ims332_write_register(regs, IMS332_REG_CURSOR_LUT_0,
			      (color->red << 16) |
			      (color->green << 8) |
			      (color->blue));
	color++;
	ims332_write_register(regs, IMS332_REG_CURSOR_LUT_1,
			      (color->red << 16) |
			      (color->green << 8) |
			      (color->blue));
}

ims332_cursor_sprite( regs, cursor)
	ims332_padded_regmap_t	*regs;
	unsigned short		*cursor;
{
	register int i;

	/* We *could* cut this down a lot... */
	for (i = 0; i < 512; i++, cursor++)
		ims332_write_register( regs,
			IMS332_REG_CURSOR_RAM+i, *cursor);
}

/*
 * Initialization
 */
ims332_init(regs, reset)
	ims332_padded_regmap_t	*regs;
	unsigned int		*reset;
{

	assert_ims332_reset_bit(reset);
	delay(1);	/* specs sez 50ns.. */
	deassert_ims332_reset_bit(reset);

	/* CLOCKIN appears to receive a 6.25 Mhz clock --> PLL 12 for 75Mhz monitor */
	ims332_write_register(regs, IMS332_REG_BOOT, 12 | IMS332_BOOT_CLOCK_PLL);

	/* initialize VTG */
	ims332_write_register(regs, IMS332_REG_CSR_A,
				IMS332_BPP_8 | IMS332_CSR_A_DISABLE_CURSOR);
	delay(50);	/* spec does not say */

	/* datapath registers (values taken from prom's settings) */

	ims332_write_register( regs, IMS332_REG_HALF_SYNCH,		0x10);
	ims332_write_register( regs, IMS332_REG_BACK_PORCH,		0x21);
	ims332_write_register( regs, IMS332_REG_DISPLAY,		0x100);
	ims332_write_register( regs, IMS332_REG_SHORT_DIS,		0x5d);
	ims332_write_register( regs, IMS332_REG_BROAD_PULSE,		0x9f);
	ims332_write_register( regs, IMS332_REG_V_SYNC,			0xc);
	ims332_write_register( regs, IMS332_REG_V_PRE_EQUALIZE,		2);
	ims332_write_register( regs, IMS332_REG_V_POST_EQUALIZE,	2);
	ims332_write_register( regs, IMS332_REG_V_BLANK,		0x2a);
	ims332_write_register( regs, IMS332_REG_V_DISPLAY,		0x600);
	ims332_write_register( regs, IMS332_REG_LINE_TIME,		0x146);
	ims332_write_register( regs, IMS332_REG_LINE_START,		0x10);
	ims332_write_register( regs, IMS332_REG_MEM_INIT, 		0xa);
	ims332_write_register( regs, IMS332_REG_XFER_DELAY,		0xa);

	ims332_write_register( regs, IMS332_REG_COLOR_MASK, 0xffffff);

	ims332_init_colormap( regs );

	ims332_write_register(regs, IMS332_REG_CSR_A,
		IMS332_BPP_8 | IMS332_CSR_A_DMA_DISABLE | IMS332_CSR_A_VTG_ENABLE);

}
