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
 * Revision 2.7  91/08/24  11:53:14  af
 * 	Added new hires, monochrome screen driver "fb", dressed as a color one.
 * 	[91/08/02  01:56:50  af]
 * 
 * Revision 2.6  91/06/19  11:54:24  rvb
 * 	mips->DECSTATION; vax->VAXSTATION
 * 	[91/06/12  14:02:13  rvb]
 * 
 * 	File moved here from mips/PMAX since it tries to be generic;
 * 	it is used on the PMAX and the Vax3100.
 * 	[91/06/04            rvb]
 * 
 * Revision 2.5  91/05/14  17:27:41  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:44:22  mrt
 * 	Added author notices
 * 	[91/02/04  11:17:47  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:16:23  mrt]
 * 
 * Revision 2.3  90/12/05  23:34:24  af
 * 	Do not use a single patchable switch, but a pointer
 * 	to a per-device one.
 * 	[90/12/03  23:37:08  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:41:38  af
 * 	Re-Created from pm_switch.c for consistent file naming.
 * 	[90/09/03            af]
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
 *	File: screen_switch.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	9/90
 *
 *	Autoconfiguration code for the Generic Screen Driver.
 */

#include <platforms.h>

#ifdef	DECSTATION
#include <gx.h>
#include <cfb.h>
#include <mfb.h>
#include <xcfb.h>
#endif	/*DECSTATION*/

#ifdef	VAXSTATION
#define NGX 0
#define NCFB 0
#define NXCFB 0
#endif	/*VAXSTATION*/

#include <chips/screen_switch.h>

/* When nothing needed */
int screen_noop()
{}

/*
 * Vector of graphic interface drivers to probe.
 * Zero terminate this list.
 */


#if	NGX > 0
extern int gq_probe(), gq_cold_init();
extern unsigned int gq_mem_need();

extern int ga_probe(), ga_cold_init();
extern unsigned int ga_mem_need();
#endif	/* NGX > 0 */

#if	NCFB > 0
extern int cfb_probe(), cfb_cold_init();
extern unsigned int pm_mem_need();
#endif	/* NCFB > 0 */

#if	NMFB > 0
extern int fb_probe(), fb_cold_init();
extern unsigned int pm_mem_need();
#endif	/* NMFB > 0 */

#if	NXCFB > 0
extern int xcfb_probe(), xcfb_cold_init();
extern unsigned int pm_mem_need();
#endif	/* NXCFB > 0 */

extern int pm_probe(), pm_cold_init();
extern unsigned int pm_mem_need();

struct screen_probe_vector screen_probe_vector[] = {

#if	NGX > 0
	gq_probe, gq_mem_need, gq_cold_init, /* 3max 3D color option */
	ga_probe, ga_mem_need, ga_cold_init, /* 3max 2D color option */
#endif	/* NGX > 0 */

#if	NMFB > 0
	fb_probe, pm_mem_need, fb_cold_init, /* 3max/3min 1D(?) mono option */
#endif	/* NMFB > 0 */

#if	NCFB > 0
	cfb_probe, pm_mem_need, cfb_cold_init, /* 3max 1D(?) color option */
#endif	/* NCFB > 0 */

#if	NXCFB > 0
	xcfb_probe, pm_mem_need, xcfb_cold_init,/* MAXine frame buffer */
#endif	/* NXCFB > 0 */

	pm_probe, pm_mem_need, pm_cold_init, /* "pm" mono/color (pmax) */
	0,
};

char	*screen_data;	/* opaque */

int screen_find()
{
	struct screen_probe_vector *p = screen_probe_vector;
	for (;p->probe; p++)
		if ((*p->probe)()) {
			(*p->setup)(0/*XXX*/, screen_data);
			return 1;
		}
	return 0;
}

unsigned int
screen_memory_alloc(avail)
	char *avail;
{
	struct screen_probe_vector *p = screen_probe_vector;
	int             size;
	for (; p->probe; p++)
		if ((*p->probe) ()) {
			screen_data = avail;
			size = (*p->alloc) ();
			bzero(screen_data, size);
			return size;
		}
	return 0;

}

