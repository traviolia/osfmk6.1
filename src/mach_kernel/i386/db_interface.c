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
 * Revision 2.8.5.3  92/04/30  11:50:04  bernadat
 * 	Removed RB_KDB test in kdb_trap to prevent kernel
 * 	from traping when kernel file doesnt have x mode.
 * 	[92/04/27            bernadat]
 * 
 * 	Adaptations for Corollary and Systempro
 * 	[92/04/08            bernadat]
 * 
 * Revision 2.8.5.2  92/03/03  16:14:32  jeffreyh
 * 	Pick up changes from TRUNK
 * 	[92/02/26  11:05:24  jeffreyh]
 * 
 * Revision 2.10  92/02/19  16:29:17  elf
 * 	MD debugger support.
 * 	[92/02/07            rvb]
 * 
 * Revision 2.9  92/01/03  20:05:13  dbg
 * 	Always enter debugger - ignore RB_KDB.
 * 	[91/10/30            dbg]
 * 
 * Revision 2.8.5.1  92/02/18  18:44:25  jeffreyh
 * 	Synchronize cpus for kdb when called via keyboard interrupt
 * 	[91/08/22            bernadat]
 * 
 * 	Support for the Corollary MP
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.8  91/10/09  16:06:14  af
 * 	Added user space access and check routines.
 * 	Added U*X task information print routines.
 * 	Changed db_trap to db_task_trap.
 * 	[91/08/29            tak]
 * 
 * Revision 2.7  91/07/31  17:35:23  dbg
 * 	Registers are in different locations on keyboard interrupt.
 * 	[91/07/30  16:48:52  dbg]
 * 
 * Revision 2.6  91/05/14  16:05:37  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/03/16  14:44:00  rpd
 * 	Fixed kdb_trap to skip over permanent breakpoints.
 * 	[91/03/15            rpd]
 * 
 * 	Changed kdb_trap to use db_recover.
 * 	[91/01/14            rpd]
 * 
 * 	Fixed kdb_trap to disable interrupts.
 * 	[91/01/12            rpd]
 * 
 * Revision 2.4  91/02/05  17:11:13  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:31:17  mrt]
 * 
 * Revision 2.3  90/12/04  14:45:55  jsb
 * 	Changes for merged intel/pmap.{c,h}.
 * 	[90/12/04  11:14:41  jsb]
 * 
 * Revision 2.2  90/10/25  14:44:43  rwd
 * 	Added watchpoint support.
 * 	[90/10/18            rpd]
 * 
 * 	Created.
 * 	[90/07/25            dbg]
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
 * Interface to new debugger.
 */
#include <cpus.h>
#include <platforms.h>
#include <time_stamp.h>
#include <mach_mp_debug.h>
#include <kern/spl.h>
#include <kern/cpu_number.h>
#include <kern/kern_types.h>
#include <kern/misc_protos.h>
#include <sys/reboot.h>
#include <vm/pmap.h>

#include <i386/thread.h>
#include <i386/db_machdep.h>
#include <i386/seg.h>
#include <i386/trap.h>
#include <i386/setjmp.h>
#include <i386/pmap.h>
#include <i386/misc_protos.h>

#include <mach/vm_param.h>
#include <vm/vm_map.h>
#include <kern/thread.h>
#include <kern/task.h>

#include <ddb/db_command.h>
#include <ddb/db_task_thread.h>
#include <ddb/db_run.h>
#include <ddb/db_trap.h>
#include <ddb/db_output.h>
#include <ddb/db_access.h>
#include <ddb/db_sym.h>

int	 db_active = 0;
int	 db_pass_thru[NCPUS];
struct	 i386_saved_state *i386_last_saved_statep;
struct	 i386_saved_state i386_nested_saved_state;
unsigned i386_last_kdb_sp;

extern	thread_act_t db_default_act;

#if	MACH_MP_DEBUG
extern int masked_state_cnt[];
#endif	/* MACH_MP_DEBUG */

/*
 *	Enter KDB through a keyboard trap.
 *	We show the registers as of the keyboard interrupt
 *	instead of those at its call to KDB.
 */
struct int_regs {
	int	gs;
	int	fs;
	int	edi;
	int	esi;
	int	ebp;
	int	ebx;
	struct i386_interrupt_state *is;
};

extern char *	trap_type[];
extern int	TRAP_TYPES;

/* Forward */

extern void	kdbprinttrap(
			int			type,
			int			code,
			int			pc,
			int			sp);
extern void	kdb_kentry(
			struct int_regs		*int_regs);
extern int	db_user_to_kernel_address(
			task_t			task,
			vm_offset_t		addr,
			unsigned		*kaddr,
			int			flag);
extern void	db_write_bytes_user_space(
			vm_offset_t		addr,
			int			size,
			char			*data,
			task_t			task);
extern int	db_search_null(
			task_t			task,
			unsigned		*svaddr,
			unsigned		evaddr,
			unsigned		*skaddr,
			int			flag);
extern int	kdb_enter(void);
extern void	kdb_leave(void);
extern void	lock_kdb(void);
extern void	unlock_kdb(void);

/*
 *  kdb_trap - field a TRACE or BPT trap
 */


extern jmp_buf_t *db_recover;
spl_t	saved_ipl[NCPUS];	/* just to know what was IPL before trap */
struct i386_saved_state *saved_state[NCPUS];

int
kdb_trap(
	int			type,
	int			code,
	struct i386_saved_state *regs)
{
	extern char 		etext;
	boolean_t		trap_from_user;
	spl_t			s = db_splhigh();

	switch (type) {
	    case T_DEBUG:	/* single_step */
	    {
	    	extern int dr_addr[];
		int addr;
	    	int status = dr6();

		if (status & 0xf) {	/* hmm hdw break */
			addr =	status & 0x8 ? dr_addr[3] :
				status & 0x4 ? dr_addr[2] :
				status & 0x2 ? dr_addr[1] :
					       dr_addr[0];
			regs->efl |= EFL_RF;
			db_single_step_cmd(addr, 0, 1, "p");
		}
	    }
	    case T_INT3:	/* breakpoint */
	    case T_WATCHPOINT:	/* watchpoint */
	    case -1:	/* keyboard interrupt */
		break;

	    default:
		if (db_recover) {
		    i386_nested_saved_state = *regs;
		    db_printf("Caught ");
		    if (type > TRAP_TYPES)
			db_printf("type %d", type);
		    else
			db_printf("%s", trap_type[type]);
		    db_printf(" trap, code = %x, pc = %x\n",
			      code, regs->eip);
			(void)db_splx(s);
		    db_error("");
		    /*NOTREACHED*/
		}
		kdbprinttrap(type, code, regs->eip, (int)&regs->uesp);
	}

	saved_ipl[cpu_number()] = s;
	saved_state[cpu_number()] = regs;

	i386_last_saved_statep = regs;
	i386_last_kdb_sp = (unsigned) &type;

#if	NCPUS > 1
	if (!kdb_enter())
		goto kdb_exit;
#endif	/* NCPUS > 1 */

		
	/*  Should switch to kdb's own stack here. */

	ddb_regs = *regs;
	trap_from_user = IS_USER_TRAP(&ddb_regs, &etext);
	if (!trap_from_user) {
	    /*
	     * Kernel mode - esp and ss not saved
	     */
	    ddb_regs.uesp = (int)&regs->uesp;	/* kernel stack pointer */
	    ddb_regs.ss   = KERNEL_DS;
	}

	db_active++;
	cnpollc(TRUE);
	db_task_trap(type, code, trap_from_user);
	cnpollc(FALSE);
	db_active--;

	regs->eip    = ddb_regs.eip;
	regs->efl    = ddb_regs.efl;
	regs->eax    = ddb_regs.eax;
	regs->ecx    = ddb_regs.ecx;
	regs->edx    = ddb_regs.edx;
	regs->ebx    = ddb_regs.ebx;
	if (trap_from_user) {
	    /*
	     * user mode - saved esp and ss valid
	     */
	    regs->uesp = ddb_regs.uesp;		/* user stack pointer */
	    regs->ss   = ddb_regs.ss & 0xffff;	/* user stack segment */
	}
	regs->ebp    = ddb_regs.ebp;
	regs->esi    = ddb_regs.esi;
	regs->edi    = ddb_regs.edi;
	regs->es     = ddb_regs.es & 0xffff;
	regs->cs     = ddb_regs.cs & 0xffff;
	regs->ds     = ddb_regs.ds & 0xffff;
	regs->fs     = ddb_regs.fs & 0xffff;
	regs->gs     = ddb_regs.gs & 0xffff;

	if ((type == T_INT3) &&
	    (db_get_task_value(regs->eip, BKPT_SIZE, FALSE, TASK_NULL)
								 == BKPT_INST))
	    regs->eip += BKPT_SIZE;

#if	NCPUS > 1
kdb_exit:
	kdb_leave();
#endif	/* NCPUS > 1 */

	saved_state[cpu_number()] = 0;

#if	MACH_MP_DEBUG
	masked_state_cnt[cpu_number()] = 0;
#endif	/* MACH_MP_DEBUG */

	(void) db_splx(s);
	return (1);
}

/*
 *	Enter KDB through a keyboard trap.
 *	We show the registers as of the keyboard interrupt
 *	instead of those at its call to KDB.
 */

spl_t kdb_oldspl;

void
kdb_kentry(
	struct int_regs	*int_regs)
{
	extern char etext;
	boolean_t trap_from_user;
	struct i386_interrupt_state *is = int_regs->is;
	struct i386_saved_state regs;
	spl_t s;

	s = db_splhigh();
	kdb_oldspl = s;

	if (IS_USER_TRAP(is, &etext))
	{
	    regs.uesp = ((int *)(is+1))[0];
	    regs.ss   = ((int *)(is+1))[1];
	}
	else {
	    regs.ss  = KERNEL_DS;
	    regs.uesp= (int)(is+1);
	}
	regs.efl = is->efl;
	regs.cs  = is->cs;
	regs.eip = is->eip;
	regs.eax = is->eax;
	regs.ecx = is->ecx;
	regs.edx = is->edx;
	regs.ebx = int_regs->ebx;
	regs.ebp = int_regs->ebp;
	regs.esi = int_regs->esi;
	regs.edi = int_regs->edi;
	regs.ds  = is->ds;
	regs.es  = is->es;
	regs.fs  = int_regs->fs;
	regs.gs  = int_regs->gs;

	saved_state[cpu_number()] = &regs;

#if	NCPUS > 1
	if (!kdb_enter())
		goto kdb_exit;
#endif	/* NCPUS > 1 */

	bcopy((char *)&regs, (char *)&ddb_regs, sizeof (ddb_regs));
	trap_from_user = IS_USER_TRAP(&ddb_regs, &etext);

	db_active++;
	cnpollc(TRUE);
	db_task_trap(-1, 0, trap_from_user);
	cnpollc(FALSE);
	db_active--;

	if (trap_from_user) {
	    ((int *)(is+1))[0] = ddb_regs.uesp;
	    ((int *)(is+1))[1] = ddb_regs.ss & 0xffff;
	}
	is->efl = ddb_regs.efl;
	is->cs  = ddb_regs.cs & 0xffff;
	is->eip = ddb_regs.eip;
	is->eax = ddb_regs.eax;
	is->ecx = ddb_regs.ecx;
	is->edx = ddb_regs.edx;
	int_regs->ebx = ddb_regs.ebx;
	int_regs->ebp = ddb_regs.ebp;
	int_regs->esi = ddb_regs.esi;
	int_regs->edi = ddb_regs.edi;
	is->ds  = ddb_regs.ds & 0xffff;
	is->es  = ddb_regs.es & 0xffff;
	int_regs->fs = ddb_regs.fs & 0xffff;
	int_regs->gs = ddb_regs.gs & 0xffff;

#if	NCPUS > 1
kdb_exit:
	kdb_leave();
#endif	/* NCPUS > 1 */
	saved_state[cpu_number()] = 0;

	(void) db_splx(s);
}

/*
 * Print trap reason.
 */

void
kdbprinttrap(
	int	type,
	int	code,
	int	pc,
	int	sp)
{
	printf("kernel: ");
	if (type > TRAP_TYPES)
	    printf("type %d", type);
	else
	    printf("%s", trap_type[type]);
	printf(" trap, code=%x eip=%x esp=%x\n", code, pc, sp);
}

int
db_user_to_kernel_address(
	task_t		task,
	vm_offset_t	addr,
	unsigned	*kaddr,
	int		flag)
{
	register pt_entry_t *ptp;
	
	ptp = pmap_pte(task->map->pmap, addr);
	if (ptp == PT_ENTRY_NULL || (*ptp & INTEL_PTE_VALID) == 0) {
	    if (flag) {
		db_printf("\nno memory is assigned to address %08x\n", addr);
		db_error(0);
		/* NOTREACHED */
	    }
	    return(-1);
	}
	*kaddr = (unsigned)ptetokv(*ptp) + (addr & (INTEL_PGBYTES-1));
	return(0);
}
	
/*
 * Read bytes from kernel address space for debugger.
 */

void
db_read_bytes(
	vm_offset_t	addr,
	int		size,
	char		*data,
	task_t		task)
{
	register char	*src;
	register int	n;
	unsigned	kern_addr;

	src = (char *)addr;
	if (addr >= VM_MIN_KERNEL_ADDRESS || task == TASK_NULL) {
	    if (task == TASK_NULL)
	        task = db_current_space();
	    while (--size >= 0) {
		if (addr++ < VM_MIN_KERNEL_ADDRESS && task == TASK_NULL) {
		    db_printf("\nbad address %x\n", addr);
		    db_error(0);
		    /* NOTREACHED */
		}
		*data++ = *src++;
	    }
	    return;
	}
	while (size > 0) {
	    if (db_user_to_kernel_address(task, addr, &kern_addr, 1) < 0)
		return;
	    src = (char *)kern_addr;
	    n = intel_trunc_page(addr+INTEL_PGBYTES) - addr;
	    if (n > size)
		n = size;
	    size -= n;
	    addr += n;
	    while (--n >= 0)
		*data++ = *src++;
	}
}

/*
 * Write bytes to kernel address space for debugger.
 */

void
db_write_bytes(
	vm_offset_t	addr,
	int		size,
	char		*data,
	task_t		task)
{
	register char	*dst;

	register pt_entry_t *ptep0 = 0;
	pt_entry_t	oldmap0 = 0;
	vm_offset_t	addr1;
	register pt_entry_t *ptep1 = 0;
	pt_entry_t	oldmap1 = 0;
	extern char	etext;

	if ((addr < VM_MIN_KERNEL_ADDRESS) ^ 
	    ((addr + size) <= VM_MIN_KERNEL_ADDRESS)) {
	    db_error("\ncannot write data into mixed space\n");
	    /* NOTREACHED */
	}
	if (addr < VM_MIN_KERNEL_ADDRESS) {
	    if (task) {
		db_write_bytes_user_space(addr, size, data, task);
		return;
	    } else if (db_current_space() == TASK_NULL) {
		db_printf("\nbad address %x\n", addr);
		db_error(0);
		/* NOTREACHED */
	    }
	}
	    
	if (addr >= VM_MIN_KERNEL_ADDRESS &&
	    addr <= (vm_offset_t)&etext)
	{
	    ptep0 = pmap_pte(kernel_pmap, addr);
	    oldmap0 = *ptep0;
	    *ptep0 |= INTEL_PTE_WRITE;

	    addr1 = i386_trunc_page(addr + size - 1);
	    if (i386_trunc_page(addr) != addr1) {
		/* data crosses a page boundary */

		ptep1 = pmap_pte(kernel_pmap, addr1);
		oldmap1 = *ptep1;
		*ptep1 |= INTEL_PTE_WRITE;
	    }
	    flush_tlb();
	}

	dst = (char *)addr;

	while (--size >= 0)
	    *dst++ = *data++;

	if (ptep0) {
	    *ptep0 = oldmap0;
	    if (ptep1) {
		*ptep1 = oldmap1;
	    }
	    flush_tlb();
	}
}
	
void
db_write_bytes_user_space(
	vm_offset_t	addr,
	int		size,
	char		*data,
	task_t		task)
{
	register char	*dst;
	register int	n;
	unsigned	kern_addr;

	while (size > 0) {
	    if (db_user_to_kernel_address(task, addr, &kern_addr, 1) < 0)
		return;
	    dst = (char *)kern_addr;
	    n = intel_trunc_page(addr+INTEL_PGBYTES) - addr;
	    if (n > size)
		n = size;
	    size -= n;
	    addr += n;
	    while (--n >= 0)
		*dst++ = *data++;
	}
}

boolean_t
db_check_access(
	vm_offset_t	addr,
	int		size,
	task_t		task)
{
	register	n;
	unsigned	kern_addr;

	if (addr >= VM_MIN_KERNEL_ADDRESS) {
	    if (kernel_task == TASK_NULL)
	        return(TRUE);
	    task = kernel_task;
	} else if (task == TASK_NULL) {
	    if (current_act() == THR_ACT_NULL)
		return(FALSE);
	    task = current_act()->task;
	}
	while (size > 0) {
	    if (db_user_to_kernel_address(task, addr, &kern_addr, 0) < 0)
		return(FALSE);
	    n = intel_trunc_page(addr+INTEL_PGBYTES) - addr;
	    if (n > size)
		n = size;
	    size -= n;
	    addr += n;
	}
	return(TRUE);
}

boolean_t
db_phys_eq(
	task_t		task1,
	vm_offset_t	addr1,
	task_t		task2,
	vm_offset_t	addr2)
{
	unsigned	kern_addr1, kern_addr2;

	if (addr1 >= VM_MIN_KERNEL_ADDRESS || addr2 >= VM_MIN_KERNEL_ADDRESS)
	    return(addr1 == addr2);
	if ((addr1 & (INTEL_PGBYTES-1)) != (addr2 & (INTEL_PGBYTES-1)))
	    return(FALSE);
	if (task1 == TASK_NULL) {
	    if (current_act() == THR_ACT_NULL)
		return(FALSE);
	    task1 = current_act()->task;
	}
	if (db_user_to_kernel_address(task1, addr1, &kern_addr1, 0) < 0 ||
		db_user_to_kernel_address(task2, addr2, &kern_addr2, 0) < 0)
	    return(FALSE);
	return(kern_addr1 == kern_addr2);
}

#define DB_USER_STACK_ADDR		(VM_MIN_KERNEL_ADDRESS)
#define DB_NAME_SEARCH_LIMIT		(DB_USER_STACK_ADDR-(INTEL_PGBYTES*3))

int
db_search_null(
	task_t		task,
	unsigned	*svaddr,
	unsigned	evaddr,
	unsigned	*skaddr,
	int		flag)
{
	register unsigned vaddr;
	register unsigned *kaddr;

	kaddr = (unsigned *)*skaddr;
	for (vaddr = *svaddr; vaddr > evaddr; vaddr -= sizeof(unsigned)) {
	    if (vaddr % INTEL_PGBYTES == 0) {
		vaddr -= sizeof(unsigned);
		if (db_user_to_kernel_address(task, vaddr, skaddr, 0) < 0)
		    return(-1);
		kaddr = (unsigned *)*skaddr;
	    } else {
		vaddr -= sizeof(unsigned);
		kaddr--;
	    }
	    if ((*kaddr == 0) ^ (flag  == 0)) {
		*svaddr = vaddr;
		*skaddr = (unsigned)kaddr;
		return(0);
	    }
	}
	return(-1);
}

void
db_task_name(
	task_t		task)
{
	register char *p;
	register n;
	unsigned vaddr, kaddr;

	vaddr = DB_USER_STACK_ADDR;
	kaddr = 0;

	/*
	 * skip nulls at the end
	 */
	if (db_search_null(task, &vaddr, DB_NAME_SEARCH_LIMIT, &kaddr, 0) < 0) {
	    db_printf(DB_NULL_TASK_NAME);
	    return;
	}
	/*
	 * search start of args
	 */
	if (db_search_null(task, &vaddr, DB_NAME_SEARCH_LIMIT, &kaddr, 1) < 0) {
	    db_printf(DB_NULL_TASK_NAME);
	    return;
	}

	n = DB_TASK_NAME_LEN-1;
	p = (char *)kaddr + sizeof(unsigned);
	for (vaddr += sizeof(int); vaddr < DB_USER_STACK_ADDR && n > 0; 
							vaddr++, p++, n--) {
	    if (vaddr % INTEL_PGBYTES == 0) {
		(void)db_user_to_kernel_address(task, vaddr, &kaddr, 0);
		p = (char*)kaddr;
	    }
	    db_printf("%c", (*p < ' ' || *p > '~')? ' ': *p);
	}
	while (n-- >= 0)	/* compare with >= 0 for one more space */
	    db_printf(" ");
}

#if NCPUS == 1

void
db_machdep_init(void)
{
	/* nothing */
}

#else /* NCPUS > 1 */

/*
 * Code used to synchronize kdb among all cpus, one active at a time, switch
 * from on to another using kdb_on! #cpu or cpu #cpu
 */

spinlock_t	kdb_lock;		/* kdb lock			*/

void
db_machdep_init(void)
{
	spinlock_init(&kdb_lock);
}

int	kdb_cpu = -1;			/* current cpu running kdb	*/
int	kdb_debug = 0;
int	kdb_is_slave[NCPUS];
int	kdb_active[NCPUS];

/*
 * Called when entering kdb:
 * Takes kdb lock. If if we were called remotly (slave state) we just
 * wait for kdb_cpu to be equal to cpu_number(). Otherwise enter kdb if
 * not active on another cpu.
 * If db_pass_thru[cpu_number()] > 0, then kdb can't stop now.
 */

int
kdb_enter(void)
{
	int my_cpu = cpu_number();

	if (db_pass_thru[my_cpu])
		return(0);

	kdb_active[my_cpu]++;
	lock_kdb();

	if (kdb_debug)
		db_printf("kdb_enter: cpu %d, is_slave %d, kdb_cpu %d, run mode %d\n",
			  my_cpu, kdb_is_slave[my_cpu], kdb_cpu, db_run_mode);
	if (kdb_cpu == -1 && !kdb_is_slave[my_cpu]) {
		remote_kdb();	/* stop other cpus */
		kdb_cpu = my_cpu;
		return(1);
	} else if (kdb_cpu == my_cpu)
		return(1);
	else
		return(0);
}

void
kdb_leave(void)
{
	int my_cpu = cpu_number();

	if (db_run_mode == STEP_CONTINUE)
		kdb_cpu = -1;
	if (kdb_is_slave[my_cpu])
		kdb_is_slave[my_cpu]--;
	if (kdb_debug)
		db_printf("kdb_leave: cpu %d, kdb_cpu %d, run_mode %d\n",
			  my_cpu, kdb_cpu, db_run_mode);
	unlock_kdb();
	kdb_active[my_cpu]--;
}

void
lock_kdb(void)
{
	int	my_cpu = cpu_number();
	register	i;
	extern void	kdb_console(void);

	for(;;) {
		kdb_console();
		if (kdb_cpu != -1 && kdb_cpu != my_cpu) {
			continue;
		}
		spinlock_lock(&kdb_lock);
		if (kdb_cpu == -1 || kdb_cpu == my_cpu)
			break;
		spinlock_unlock(&kdb_lock);
	} 
}

#if	TIME_STAMP
extern unsigned old_time_stamp;
#endif	/* TIME_STAMP */

void
unlock_kdb(void)
{
	spinlock_unlock(&kdb_lock);
#if	TIME_STAMP
	old_time_stamp = 0;
#endif	/* TIME_STAMP */
}


#ifdef	__STDC__
#define KDB_SAVE(type, name) extern type name; type name##_save = name
#define KDB_RESTORE(name) name = name##_save
#else	/* __STDC__ */
#define KDB_SAVE(type, name) extern type name; type name/**/_save = name
#define KDB_RESTORE(name) name = name/**/_save
#endif	/* __STDC__ */

#define KDB_SAVE_CTXT() \
	KDB_SAVE(int, db_run_mode); \
	KDB_SAVE(boolean_t, db_sstep_print); \
	KDB_SAVE(int, db_loop_count); \
	KDB_SAVE(int, db_call_depth); \
	KDB_SAVE(int, db_inst_count); \
	KDB_SAVE(int, db_last_inst_count); \
	KDB_SAVE(int, db_load_count); \
	KDB_SAVE(int, db_store_count); \
	KDB_SAVE(boolean_t, db_cmd_loop_done); \
	KDB_SAVE(jmp_buf_t *, db_recover); \
	KDB_SAVE(db_addr_t, db_dot); \
	KDB_SAVE(db_addr_t, db_last_addr); \
	KDB_SAVE(db_addr_t, db_prev); \
	KDB_SAVE(db_addr_t, db_next); \
	KDB_SAVE(db_regs_t, ddb_regs); 

#define KDB_RESTORE_CTXT() \
	KDB_RESTORE(db_run_mode); \
	KDB_RESTORE(db_sstep_print); \
	KDB_RESTORE(db_loop_count); \
	KDB_RESTORE(db_call_depth); \
	KDB_RESTORE(db_inst_count); \
	KDB_RESTORE(db_last_inst_count); \
	KDB_RESTORE(db_load_count); \
	KDB_RESTORE(db_store_count); \
	KDB_RESTORE(db_cmd_loop_done); \
	KDB_RESTORE(db_recover); \
	KDB_RESTORE(db_dot); \
	KDB_RESTORE(db_last_addr); \
	KDB_RESTORE(db_prev); \
	KDB_RESTORE(db_next); \
	KDB_RESTORE(ddb_regs); 

/*
 * switch to another cpu
 */

void
kdb_on(
	int		cpu)
{
	KDB_SAVE_CTXT();
	if (cpu < 0 || cpu >= NCPUS || !kdb_active[cpu])
		return;
	kdb_cpu = cpu;
	unlock_kdb();
	lock_kdb();
	KDB_RESTORE_CTXT();
	if (kdb_cpu == -1) /* someone continued */
		db_continue_cmd(0, 0, 0, "");
}

#endif	/* NCPUS > 1 */
