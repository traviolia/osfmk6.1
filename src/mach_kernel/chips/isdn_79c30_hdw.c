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
 * Copyright (c) 1992 Carnegie Mellon University
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
 * Revision 2.2  92/03/02  18:33:00  rpd
 * 	Created stub.
 * 	[92/01/19            af]
 * 
 */
/* CMU_ENDHIST */
/*
 *	File: isdn_79c30_hdw.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	1/92
 *
 *	Driver for the AMD 79c30 ISDN (Integrated Speech and
 *	Data Network) controller chip.
 */

#include <isdn.h>
#if	NISDN > 0

#include <mach/std_types.h>
#include <sys/types.h>
#include <chips/busses.h>

/*
 * Autoconf info
 */

static caddr_t isdn_std[NISDN] = { 0 };
static struct bus_device *isdn_info[NISDN];
static int isdn_probe(), isdn_attach();

struct bus_driver isdn_driver =
       { isdn_probe, 0, isdn_attach, 0, isdn_std, "isdn", isdn_info, };

/*
 * Externally visible functions
 */
int	isdn_intr();					/* kernel */

/*
 * Probe chip to see if it is there
 */
static isdn_probe (reg, ui)
	vm_offset_t	reg;
	struct bus_device *ui;
{
	return 1;
}

static isdn_attach (ui)
	struct bus_device *ui;
{
}

isdn_intr (unit, spllevel)
{
	xine_enable_interrupt(7, 0, 0);
	printf("ISDN interrupt");
}

#endif
