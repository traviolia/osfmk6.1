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
 * Revision 2.1.3.3  92/09/15  17:15:38  jeffreyh
 * 	Do not propagate clock int to all CPUs at the same time
 * 	[92/07/24            bernadat]
 * 
 * Revision 2.1.3.2  92/06/24  17:59:32  jeffreyh
 * 	Fix to be able to compile STD+COMPAQ (Single cpu)
 * 	[92/06/09            bernadat]
 * 
 * Revision 2.1.3.1  92/04/30  11:58:19  bernadat
 * 	MP code for AT386 platforms (Corollary, SystemPro)
 * 	[92/04/08            bernadat]
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

#if NCPUS > 1

#include <sys/types.h>
#include <mach/machine.h>
#include <kern/lock.h>
#include <kern/processor.h>
#include <kern/misc_protos.h>
#include <kern/machine.h>
#include <sys/reboot.h>
#include <sys/types.h>
#include <i386/db_machdep.h>
#include <ddb/db_run.h>
#include <machine/mp/mp.h>
#include <i386/setjmp.h>
#include <i386/misc_protos.h>

int	cpu_int_word[NCPUS];

extern void cpu_interrupt(int cpu);
extern int get_ncpus(void);

void
slave_clock(void)
{
	register cpu;

	for (cpu=cpu_number()+1; cpu<NCPUS; cpu++)
		if ( machine_slot[cpu].running == TRUE) {
			i_bit_set(MP_CLOCK, &cpu_int_word[cpu]);
			cpu_interrupt(cpu);
			return;
		}
}

void
interrupt_processor(
	int		cpu)
{
	i_bit_set(MP_TLB_FLUSH, &cpu_int_word[cpu]);
	cpu_interrupt(cpu);
}

/*ARGSUSED*/
void
init_ast_check(
	processor_t	processor)
{
}

void
cause_ast_check(
	processor_t	processor)
{
	int cpu = processor->slot_num;

	i_bit_set(MP_AST, &cpu_int_word[cpu]);
	cpu_interrupt(cpu);
}

/*ARGSUSED*/
kern_return_t
cpu_start(
	int	slot_num)
{
	printf("cpu_start not implemented\n");
	return (KERN_FAILURE);
}

/*ARGSUSED*/
kern_return_t
cpu_control(
	int			slot_num,
	processor_info_t	info,
	unsigned int		count)
{
	printf("cpu_control not implemented\n");
	return (KERN_FAILURE);
}

int real_ncpus;
int wncpu = NCPUS;

/*
 * Find out how many cpus will run
 */
 
void
mp_probe_cpus(void)
{
	register i;

	/* 
	 * get real number of cpus
	 */

	real_ncpus = get_ncpus();

	/*
	 * Get number of cpus to boot, passed as an optionnal argument
	 * boot: mach [-sah#]	# from 0 to 9 is the number of cpus to boot
	 * will be overwriten by RB_ASKNAME later
	 */

	wncpu = (boothowto  >> RB_SHIFT) & 0xff;
	if (!wncpu)
		wncpu = NCPUS;
	boothowto &= ~(0xff << RB_SHIFT);

	/*
	 * Ignore real number of cpus it if number of requested cpus
	 * is smaller.
	 * Keep it if number of requested cpu is null or larger.
	 */

	if (real_ncpus < wncpu)
		wncpu = real_ncpus;
	for (i=0; i < wncpu; i++)
		machine_slot[i].is_cpu = TRUE;
}

/*
 * invoke kdb on slave processors 
 */

void
remote_kdb(void)
{
	int	my_cpu = cpu_number();
	register	i;

	for (i = 0; i < NCPUS; i++)
		if ( i != my_cpu && machine_slot[i].running == TRUE) {
			i_bit_set(MP_KDB, &cpu_int_word[i]);
			cpu_interrupt(i);
		}
}

#endif /* NCPUS > 1 */




