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
 * Revision 2.5.4.2  92/04/30  11:50:45  bernadat
 * 	Adaptations for Corollary and Systempro
 * 	[92/04/08            bernadat]
 * 
 * Revision 2.5.4.1  92/02/18  18:48:03  jeffreyh
 * 	Adapt int_stack_high value for Corollary
 * 	[91/09/26  04:40:18  bernadat]
 * 
 * Revision 2.5  91/11/12  11:50:51  rvb
 * 	Added simple_lock_pause.
 * 	[91/11/12            rpd]
 * 
 * Revision 2.4  91/07/31  17:39:10  dbg
 * 	Move interrupt-stack allocation for multiprocessors here.
 * 	[91/07/30  16:54:10  dbg]
 * 
 * Revision 2.3  91/05/14  16:12:08  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/05/08  12:39:33  dbg
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

#include <cpus.h>

#if	NCPUS > 1

#include <kern/cpu_number.h>
#include <mach/machine.h>
#include <vm/vm_kern.h>

#include <i386/mp_desc.h>
#include <i386/lock.h>
#include <i386/misc_protos.h>

#include <kern/misc_protos.h>

/*
 * The i386 needs an interrupt stack to keep the PCB stack from being
 * overrun by interrupts.  All interrupt stacks MUST lie at lower addresses
 * than any thread`s kernel stack.
 */

/*
 * Addresses of bottom and top of interrupt stacks.
 */
vm_offset_t	interrupt_stack[NCPUS];
vm_offset_t	int_stack_top[NCPUS];

/*
 * Barrier address.
 */
vm_offset_t	int_stack_high;

/*
 * First cpu`s interrupt stack.
 */
char		intstack[];	/* bottom */
char		eintstack[];	/* top */

/*
 * We allocate interrupt stacks from physical memory.
 */
extern
vm_offset_t	avail_start;

/*
 * Multiprocessor i386/i486 systems use a separate copy of the
 * GDT, IDT, LDT, and kernel TSS per processor.  The first three
 * are separate to avoid lock contention: the i386 uses locked
 * memory cycles to access the descriptor tables.  The TSS is
 * separate since each processor needs its own kernel stack,
 * and since using a TSS marks it busy.
 */

/*
 * Allocated descriptor tables.
 */
struct mp_desc_table	*mp_desc_table[NCPUS] = { 0 };

/*
 * Pointer to TSS for access in load_context.
 */
struct i386_tss		*mp_ktss[NCPUS] = { 0 };

/*
 * Pointer to GDT to reset the KTSS busy bit.
 */
struct fake_descriptor	*mp_gdt[NCPUS] = { 0 };

/*
 * Boot-time tables, for initialization and master processor.
 */
extern struct fake_descriptor	idt[IDTSZ];
extern struct fake_descriptor	gdt[GDTSZ];
extern struct fake_descriptor	ldt[LDTSZ];
extern struct i386_tss		ktss;

/*
 * Allocate and initialize the per-processor descriptor tables.
 */

struct fake_descriptor ldt_desc_pattern = {
	(unsigned int) 0,
	LDTSZ * sizeof(struct fake_descriptor) - 1,
	0,
	ACC_P|ACC_PL_K|ACC_LDT
};
struct fake_descriptor tss_desc_pattern = {
	(unsigned int) 0,
	sizeof(struct i386_tss),
	0,
	ACC_P|ACC_PL_K|ACC_TSS
};

struct mp_desc_table *
mp_desc_init(
	int	mycpu)
{
	register struct mp_desc_table *mpt;

	if (mycpu == master_cpu) {
	    /*
	     * Master CPU uses the tables built at boot time.
	     * Just set the TSS and GDT pointers.
	     */
	    mp_ktss[mycpu] = &ktss;
	    mp_gdt[mycpu] = gdt;
	    return 0;
	}
	else {
	    /*
	     * Other CPUs allocate the table from the bottom of
	     * the interrupt stack.
	     */
	    mpt = (struct mp_desc_table *) interrupt_stack[mycpu];

	    mp_desc_table[mycpu] = mpt;
	    mp_ktss[mycpu] = &mpt->ktss;
	    mp_gdt[mycpu] = mpt->gdt;

	    /*
	     * Copy the tables
	     */
	    bcopy((char *)idt,
		  (char *)mpt->idt,
		  sizeof(idt));
	    bcopy((char *)gdt,
		  (char *)mpt->gdt,
		  sizeof(gdt));
	    bcopy((char *)ldt,
		  (char *)mpt->ldt,
		  sizeof(ldt));
	    bzero((char *)&mpt->ktss,
		  sizeof(struct i386_tss));

	    /*
	     * Fix up the entries in the GDT to point to
	     * this LDT and this TSS.
	     */
	    mpt->gdt[sel_idx(KERNEL_LDT)] = ldt_desc_pattern;
	    mpt->gdt[sel_idx(KERNEL_LDT)].offset =
			(unsigned int) mpt->ldt;
	    fix_desc(&mpt->gdt[sel_idx(KERNEL_LDT)], 1);

	    mpt->gdt[sel_idx(KERNEL_TSS)] = tss_desc_pattern;
	    mpt->gdt[sel_idx(KERNEL_TSS)].offset =
			(unsigned int) &mpt->ktss;
	    fix_desc(&mpt->gdt[sel_idx(KERNEL_TSS)], 1);

	    mpt->ktss.ss0 = KERNEL_DS;
	    mpt->ktss.io_bit_map_offset = 0x0FFF;	/* no IO bitmap */

	    return mpt;
	}
}


/*
 * Called after all CPUs have been found, but before the VM system
 * is running.  The machine array must show which CPUs exist.
 */
void
interrupt_stack_alloc(void)
{
	register int	i;
	int		cpu_count;
	vm_offset_t	stack_start;

	/*
	 * Count the number of CPUs.
	 */
	cpu_count = 0;
	for (i = 0; i < NCPUS; i++)
	    if (machine_slot[i].is_cpu)
		cpu_count++;

	/*
	 * Allocate an interrupt stack for each CPU except for
	 * the master CPU (which uses the bootstrap stack)
	 */
	stack_start = phystokv(avail_start);
	avail_start = round_page(avail_start + INTSTACK_SIZE*(cpu_count-1));
	bzero((char *)stack_start, INTSTACK_SIZE*(cpu_count-1));

	/*
	 * Set up pointers to the top of the interrupt stack.
	 */
	for (i = 0; i < NCPUS; i++) {
	    if (i == master_cpu) {
		interrupt_stack[i] = (vm_offset_t) intstack;
		int_stack_top[i]   = (vm_offset_t) eintstack;
	    }
	    else if (machine_slot[i].is_cpu) {
		interrupt_stack[i] = stack_start;
		int_stack_top[i]   = stack_start + INTSTACK_SIZE;

		stack_start += INTSTACK_SIZE;
	    }
	}

	/*
	 * Set up the barrier address.  All thread stacks MUST
	 * be above this address.
	 */
#if	AT386
	/*
	 * intstack is at higher addess than stack_start for AT mps
	 * so int_stack_high must point at eintstack.
	 * XXX
	 * But what happens if a kernel stack gets allocated below
	 * 1 Meg ? Probably never happens, there is only 640 K available
	 * There.
	 */
	int_stack_high = (vm_offset_t) eintstack;
#else	/* AT386 */
	int_stack_high = stack_start;
#endif	/* AT386 */
}

#endif /* NCPUS > 1 */
