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
 * Revision 2.3  91/05/14  16:12:24  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/05/08  12:39:51  dbg
 * 	Created.
 * 	[91/03/21            dbg]
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

#ifndef	_I386_MP_DESC_H_
#define	_I386_MP_DESC_H_

#include <cpus.h>

/*
 * Multiprocessor i386/i486 systems use a separate copy of the
 * GDT, IDT, LDT, and kernel TSS per processor.  The first three
 * are separate to avoid lock contention: the i386 uses locked
 * memory cycles to access the descriptor tables.  The TSS is
 * separate since each processor needs its own kernel stack,
 * and since using a TSS marks it busy.
 */

#include <i386/seg.h>
#include <i386/tss.h>

/*
 * The descriptor tables are together in a structure
 * allocated one per processor (except for the boot processor).
 */
struct mp_desc_table {
	struct fake_descriptor	idt[IDTSZ];	/* IDT */
	struct fake_descriptor	gdt[GDTSZ];	/* GDT */
	struct fake_descriptor	ldt[LDTSZ];	/* LDT */
	struct i386_tss		ktss;
};

/*
 * They are pointed to by a per-processor array.
 */
extern struct mp_desc_table	*mp_desc_table[NCPUS];

/*
 * The kernel TSS gets its own pointer.
 */
extern struct i386_tss		*mp_ktss[NCPUS];

/*
 * So does the GDT.
 */
extern struct fake_descriptor	*mp_gdt[NCPUS];


/*
 * Each CPU calls this routine to set up its descriptor tables.
 */

extern struct mp_desc_table *	mp_desc_init(
					int		cpu);
extern void			interrupt_stack_alloc(void);

#endif	/* _I386_MP_DESC_H_ */
