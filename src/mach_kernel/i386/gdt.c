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
 * Revision 2.6.3.1  92/03/03  16:15:27  jeffreyh
 * 	Pick up from TRUNK
 * 	[92/02/26  11:10:06  jeffreyh]
 * 
 * Revision 2.7  92/01/03  20:05:58  dbg
 * 	Add entries for user LDT, floating-point register
 * 	access from emulator, and floating-point emulator code.
 * 	[91/10/18            dbg]
 * 
 * Revision 2.6  91/05/14  16:08:04  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/05/08  12:31:41  dbg
 * 	Collapsed GDT again.
 * 	[91/04/26  14:34:34  dbg]
 * 
 * Revision 2.4  91/02/05  17:11:52  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:34:15  mrt]
 * 
 * Revision 2.3  90/08/27  21:56:36  dbg
 * 	Collapsed GDT.  Use new segmentation definitions.
 * 	[90/07/25            dbg]
 * 
 * Revision 2.2  90/05/03  15:27:29  dbg
 * 	Created.
 * 	[90/02/15            dbg]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
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
 * Global descriptor table.
 */
#include <i386/seg.h>
#include <i386/tss.h>
#include <mach/i386/vm_types.h>
#include <mach/i386/vm_param.h>

extern struct fake_descriptor	ldt[];
extern struct i386_tss		ktss;

struct fake_descriptor gdt[GDTSZ] = {
/* 0x000 */	{ 0, 0, 0, 0 },		/* always NULL */
/* 0x008 */	{ VM_MIN_ADDRESS,
		  (VM_MAX_KERNEL_ADDRESS-1-VM_MIN_ADDRESS)>>12,
		  SZ_32|SZ_G,
		  ACC_P|ACC_PL_K|ACC_CODE_R
		},			/* kernel code */
/* 0x010 */	{ VM_MIN_ADDRESS,
		  (VM_MAX_KERNEL_ADDRESS-1-VM_MIN_ADDRESS)>>12,
		  SZ_32|SZ_G,
		  ACC_P|ACC_PL_K|ACC_DATA_W
		},			/* kernel data */
/* 0x018 */	{ (unsigned int)ldt,
		  LDTSZ*sizeof(struct fake_descriptor)-1,
		  0,
		  ACC_P|ACC_PL_K|ACC_LDT
		},			/* local descriptor table */
/* 0x020 */	{ (unsigned int)&ktss,
		  sizeof(struct i386_tss),
		  0,
		  ACC_P|ACC_PL_K|ACC_TSS
		},			/* TSS for this processor */
/* 0x028 */	{ 0, 0, 0, 0 },		/* per-thread LDT */
/* 0x030 */	{ 0, 0, 0, 0 },		/* per-thread TSS for IO bitmap */
/* 0x038 */	{ 0, 0, 0, 0 },
/* 0x040 */	{ 0, 0, 0, 0 }
};


