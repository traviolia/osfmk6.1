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
 * Revision 2.3  92/05/05  10:53:00  danner
 * 	Added frc_set_address(), to customize it.
 * 	Makes NSC's boards happy (any TC box).
 * 	[92/04/13            jcb]
 * 
 * Revision 2.2  92/04/01  15:14:23  rpd
 * 	Created, based on maxine's counter.
 * 	[92/03/10            af]
 * 
 */
/* CMU_ENDHIST */
/*
 *	File: frc.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	3/92
 *
 *	Generic, mappable free running counter driver.
 */

#include <frc.h>
#if	NFRC > 0

#include <mach/std_types.h>
#include <sys/types.h>
#include <chips/busses.h>
#include <device/device_types.h>

/*
 * Machine defines
 * All you need to do to get this working on a
 * random box is to define one macro and provide
 * the correct virtual address.
 */
#include	<platforms.h>
#ifdef	DECSTATION
#define	btop(x)		mips_btop(x)
#endif	/* DECSTATION */

/*
 * Autoconf info
 */

static vm_offset_t frc_std[NFRC] = { 0 };
static struct bus_device *frc_info[NFRC];
static int frc_probe(), frc_attach();

struct bus_driver frc_driver =
       { frc_probe, 0, frc_attach, 0, frc_std, "frc", frc_info, };

/*
 * Externally visible functions
 */
io_return_t	frc_openclose();				/* user */
vm_offset_t	frc_mmap();
void		frc_set_address();

/* machine-specific setups */
void
frc_set_address(unit, offset)
	vm_offset_t	offset;
{
	if (unit < NFRC) {
		frc_std[unit] = offset;
	}
}


/*
 * Probe chip to see if it is there
 */
static frc_probe (reg, ui)
	vm_offset_t	reg;
	struct bus_device *ui;
{
	/* see if something present at the given address */
	if (check_memory(reg, 0))
		return 0;
	frc_std[ui->unit] += reg;
	printf("[mappable] ");
	return 1;
}

static frc_attach (ui)
	struct bus_device *ui;
{
	printf(": free running counter");
}

int frc_intr()
{
	/* we do not expect interrupts */
	panic("frc_intr");
}

io_return_t
frc_openclose()
{
	return D_SUCCESS;
}

vm_offset_t
frc_mmap(dev, off, prot)
	dev_t		dev;
	vm_offset_t	off;
	vm_prot_t	prot;
{
	if ((prot & VM_PROT_WRITE) || (off >= PAGE_SIZE) )
		return (-1);
	return (btop(pmap_extract(pmap_kernel(), frc_std[minor(dev)])));
}

#endif
