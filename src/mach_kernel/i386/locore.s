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
 * Revision 2.18.5.3  92/04/30  11:50:33  bernadat
 * 	Support for SystemPro
 * 	[92/04/08            bernadat]
 * 
 * Revision 2.18.5.2  92/03/03  16:16:05  jeffreyh
 * 	Pick up changes from TRUNK
 * 	[92/02/26  11:27:25  jeffreyh]
 * 
 * Revision 2.20  92/02/19  16:29:23  elf
 * 	Add some MD debugger support.
 * 	[92/02/07            rvb]
 * 
 * Revision 2.19  92/01/03  20:07:57  dbg
 * 	Generalize retryable faults.  Add inst_fetch to fetch
 * 	instruction when segments are in use.
 * 	[91/12/06            dbg]
 * 
 * 	Remove fixed lower bound for emulated system call table.
 * 	[91/10/31            dbg]
 * 
 * 	Segment-not-present may also occur during kernel exit sequence.
 * 	Call i386_ast_taken to handle delayed floating-point exceptions.
 * 	[91/10/29            dbg]
 * 
 * Revision 2.18.5.1  92/02/18  18:47:18  jeffreyh
 * 	Removed double defined ffs() entry point for Sequent
 * 
 * 	Set CR0_NE for master cpu, otherwise fpintr() might send
 * 	exception messages under kernel interrupt mode
 * 	[91/12/06            bernadat]
 * 
 * 	Set CR0_NE for slave processors, they do not have a PIC
 * 	[91/07/09            bernadat]
 * 
 * 	Support for the Corollary MP
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.18  91/10/07  17:24:53  af
 * 	tenmicrosec() was all wrong has been expunged, since noone uses
 * 	it.
 * 	[91/09/04            rvb]
 * 
 * Revision 2.17  91/08/28  21:39:05  jsb
 * 	Add tests for V86 mode in trace and GP fault checks.  Clear
 * 	direction flag at all kernel entry points.
 * 	[91/08/20            dbg]
 * 
 * Revision 2.16  91/07/31  17:38:38  dbg
 * 	Add microsecond timing.
 * 
 * 	Save user regs directly in PCB on trap, and switch to separate
 * 	kernel stack.
 * 
 * 	Make copyin and copyout use longword move if possible.
 * 	[91/07/30  16:53:39  dbg]
 * 
 * Revision 2.15  91/06/19  11:55:16  rvb
 * 	cputypes.h->platforms.h
 * 	[91/06/12  13:44:59  rvb]
 * 
 * Revision 2.14  91/05/14  16:11:15  mrt
 * 	Correcting copyright
 * 
 * Revision 2.13  91/05/08  12:39:21  dbg
 * 	Put parentheses around substituted immediate expressions, so
 * 	that they will pass through the GNU preprocessor.
 * 
 * 	Handle multiple CPUS.
 * 	[91/04/26  14:36:46  dbg]
 * 
 * Revision 2.12  91/03/16  14:44:37  rpd
 * 	Changed call_continuation to not change spl.
 * 	[91/02/17            rpd]
 * 	Added call_continuation.
 * 	Changed the AST interface.
 * 	[91/01/18            rpd]
 * 
 * Revision 2.11  91/02/05  17:12:59  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:35:44  mrt]
 * 
 * Revision 2.10  91/01/09  22:41:32  rpd
 * 	Removed k_user_regs.
 * 	[91/01/09            rpd]
 * 
 * Revision 2.9  91/01/08  17:32:10  rpd
 * 	interrupt_returns must check for EFL_VM.
 * 	[90/12/21  14:37:44  rvb]
 * 
 * 	Add trapv86 for VM thread.
 * 	[90/12/19  17:00:56  rvb]
 * 
 * Revision 2.8  91/01/08  15:10:49  rpd
 * 	Replaced thread_bootstrap_user, thread_bootstrap_kernel
 * 	with thread_exception_return, thread_syscall_return.
 * 	Updated mach_trap_table indexing for new layout.
 * 	[90/12/17            rpd]
 * 
 * 	Renamed thread_bootstrap to thread_bootstrap_user.
 * 	Added thread_bootstrap_kernel.
 * 	[90/12/14            rpd]
 * 
 * 	Reorganized the pcb.
 * 	Added copyinmsg, copyoutmsg synonyms for copyin, copyout.
 * 	[90/12/11            rpd]
 * 
 * Revision 2.7  90/12/20  16:36:21  jeffreyh
 * 	Changes for __STDC__
 * 	[90/12/07  15:43:29  jeffreyh]
 * 
 * Revision 2.6  90/12/04  14:46:11  jsb
 * 	iPSC2 -> iPSC386.
 * 	[90/12/04  11:17:03  jsb]
 * 
 * Revision 2.5  90/09/23  17:45:16  jsb
 * 	Added support for iPSC386.
 * 	[90/09/21  16:40:55  jsb]
 * 
 * Revision 2.4  90/08/27  21:57:24  dbg
 * 	Remove interrupt/trap vectors - get from idt.s.
 * 	Fix copyout to check user address on each page boundary.
 * 	[90/07/25            dbg]
 * 
 * Revision 2.3  90/05/21  13:26:44  dbg
 * 	Add inl, outl.
 * 	[90/05/17            dbg]
 * 
 * Revision 2.2  90/05/03  15:33:45  dbg
 * 	Created.
 * 	[90/02/14            dbg]
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

#include <cpus.h>
#include <platforms.h>
#include <mach_kdb.h>
#include <mach_kgdb.h>
#include <stat_time.h>

#include <i386/asm.h>
#include <i386/eflags.h>
#include <i386/proc_reg.h>
#include <i386/trap.h>
#include <assym.s>
#include <mach/exception.h>

#if	NCPUS > 1

#ifdef	SYMMETRY
#include <sqt/asm_macros.h>
#endif

#ifdef	CBUS
#include <cbus/cbus.h>
#endif	/* CBUS */

#ifdef	MBUS
#include <mbus/mbus.h>
#endif	/* MBUS */

#define	CX(addr,reg)	addr(,reg,4)

#else
#define	CPU_NUMBER(reg)
#define	CX(addr,reg)	addr

#endif	/* NCPUS > 1 */

	.text
locore_start:

/*
 * Fault recovery.
 */
#define	RECOVER_TABLE_START	\
	.text	2		;\
	.globl	EXT(recover_table) ;\
LEXT(recover_table)		;\
	.text

#define	RECOVER(addr)		\
	.text	2		;\
	.long	9f		;\
	.long	addr		;\
	.text			;\
9:

#define	RECOVER_TABLE_END		\
	.text	2			;\
	.globl	EXT(recover_table_end)	;\
LEXT(recover_table_end)			;\
	.text

/*
 * Retry table for certain successful faults.
 */
#define	RETRY_TABLE_START	\
	.text	3		;\
	.globl	EXT(retry_table) ;\
LEXT(retry_table)		;\
	.text

#define	RETRY(addr)		\
	.text	3		;\
	.long	9f		;\
	.long	addr		;\
	.text			;\
9:

#define	RETRY_TABLE_END			\
	.text	3			;\
	.globl	EXT(retry_table_end)	;\
LEXT(retry_table_end)			;\
	.text

/*
 * Allocate recovery and retry tables.
 */
	RECOVER_TABLE_START
	RETRY_TABLE_START

/*
 * Timing routines.
 */
#if	STAT_TIME

#define	TIME_TRAP_UENTRY
#define	TIME_TRAP_UEXIT
#define	TIME_INT_ENTRY
#define	TIME_INT_EXIT

#else	/* microsecond timing */

/*
 * Microsecond timing.
 * Assumes a free-running microsecond counter.
 * no TIMER_MAX check needed.
 */

/*
 * There is only one current time-stamp per CPU, since only
 * the time-stamp in the current timer is used.
 * To save time, we allocate the current time-stamps here.
 */
	.comm	EXT(current_tstamp), 4*NCPUS

/*
 * Update time on user trap entry.
 * 11 instructions (including cli on entry)
 * Assumes CPU number in %edx.
 * Uses %ebx, %ecx.
 */
#define	TIME_TRAP_UENTRY \
	cli					/* block interrupts */	;\
	movl	VA_ETC,%ebx			/* get timer value */	;\
	movl	CX(EXT(current_tstamp),%edx),%ecx /* get old time stamp */;\
	movl	%ebx,CX(EXT(current_tstamp),%edx) /* set new time stamp */;\
	subl	%ecx,%ebx			/* elapsed = new-old */	;\
	movl	CX(EXT(current_timer),%edx),%ecx /* get current timer */;\
	addl	%ebx,LOW_BITS(%ecx)		/* add to low bits */	;\
	jns	0f				/* if overflow, */	;\
	call	timer_normalize			/* normalize timer */	;\
0:	addl	$(TH_SYS_TIMER-TH_USER_TIMER),%ecx			;\
						/* switch to sys timer */;\
	movl	%ecx,CX(EXT(current_timer),%edx) /* make it current */	;\
	sti					/* allow interrupts */

/*
 * update time on user trap exit.
 * 10 instructions.
 * Assumes CPU number in %edx.
 * Uses %ebx, %ecx.
 */
#define	TIME_TRAP_UEXIT \
	cli					/* block interrupts */	;\
	movl	VA_ETC,%ebx			/* get timer */		;\
	movl	CX(EXT(current_tstamp),%edx),%ecx /* get old time stamp */;\
	movl	%ebx,CX(EXT(current_tstamp),%edx) /* set new time stamp */;\
	subl	%ecx,%ebx			/* elapsed = new-old */	;\
	movl	CX(EXT(current_timer),%edx),%ecx /* get current timer */;\
	addl	%ebx,LOW_BITS(%ecx)		/* add to low bits */	;\
	jns	0f				/* if overflow,	*/	;\
	call	timer_normalize			/* normalize timer */	;\
0:	addl	$(TH_USER_TIMER-TH_SYS_TIMER),%ecx			;\
						/* switch to user timer	*/;\
	movl	%ecx,CX(EXT(current_timer),%edx) /* make it current */

/*
 * update time on interrupt entry.
 * 9 instructions.
 * Assumes CPU number in %edx.
 * Leaves old timer in %ebx.
 * Uses %ecx.
 */
#define	TIME_INT_ENTRY \
	movl	VA_ETC,%ecx			/* get timer */		;\
	movl	CX(EXT(current_tstamp),%edx),%ebx /* get old time stamp */;\
	movl	%ecx,CX(EXT(current_tstamp),%edx) /* set new time stamp */;\
	subl	%ebx,%ecx			/* elapsed = new-old */	;\
	movl	CX(EXT(current_timer),%edx),%ebx /* get current timer */;\
	addl	%ecx,LOW_BITS(%ebx)		/* add to low bits */	;\
	leal	CX(0,%edx),%ecx			/* timer is 16 bytes */	;\
	lea	CX(EXT(kernel_timer),%edx),%ecx	/* get interrupt timer*/;\
	movl	%ecx,CX(EXT(current_timer),%edx) /* set timer */

/*
 * update time on interrupt exit.
 * 11 instructions
 * Assumes CPU number in %edx, old timer in %ebx.
 * Uses %eax, %ecx.
 */
#define	TIME_INT_EXIT \
	movl	VA_ETC,%eax			/* get timer */		;\
	movl	CX(EXT(current_tstamp),%edx),%ecx /* get old time stamp */;\
	movl	%eax,CX(EXT(current_tstamp),%edx) /* set new time stamp */;\
	subl	%ecx,%eax			/* elapsed = new-old */	;\
	movl	CX(EXT(current_timer),%edx),%ecx /* get current timer */;\
	addl	%eax,LOW_BITS(%ecx)		/* add to low bits */	;\
	jns	0f				/* if overflow, */	;\
	call	timer_normalize			/* normalize timer */	;\
0:	testb	$0x80,LOW_BITS+3(%ebx)		/* old timer overflow? */;\
	jz	0f				/* if overflow, */	;\
	movl	%ebx,%ecx			/* get old timer */	;\
	call	timer_normalize			/* normalize timer */	;\
0:	movl	%ebx,CX(EXT(current_timer),%edx) /* set timer */


/*
 * Normalize timer in ecx.
 * Preserves edx; clobbers eax.
 */
	.align	ALIGN
timer_high_unit:
	.long	TIMER_HIGH_UNIT			/ div has no immediate opnd

timer_normalize:
	pushl	%edx				/ save registersz
	pushl	%eax
	xorl	%edx,%edx			/ clear divisor high
	movl	LOW_BITS(%ecx),%eax		/ get divisor low
	divl	timer_high_unit,%eax		/ quotient in eax
						/ remainder in edx
	addl	%eax,HIGH_BITS_CHECK(%ecx)	/ add high_inc to check
	movl	%edx,LOW_BITS(%ecx)		/ remainder to low_bits
	addl	%eax,HIGH_BITS(%ecx)		/ add high_inc to high bits
	popl	%eax				/ restore register
	popl	%edx
	ret

/*
 * Switch to a new timer.
 */
Entry(timer_switch)
	CPU_NUMBER(%edx)			/ get this CPU
	movl	VA_ETC,%ecx			/ get timer
	movl	CX(EXT(current_tstamp),%edx),%eax / get old time stamp
	movl	%ecx,CX(EXT(current_tstamp),%edx) / set new time stamp
	subl	%ecx,%eax			/ elapsed = new - old
	movl	CX(EXT(current_timer),%edx),%ecx / get current timer
	addl	%eax,LOW_BITS(%ecx)		/ add to low bits
	jns	0f				/ if overflow,
	call	timer_normalize			/ normalize timer
0:
	movl	S_ARG0,%ecx			/ get new timer
	movl	%ecx,CX(EXT(current_timer),%edx) / set timer
	ret

/*
 * Initialize the first timer for a CPU.
 */
Entry(start_timer)
	CPU_NUMBER(%edx)			/ get this CPU
	movl	VA_ETC,%ecx			/ get timer
	movl	%ecx,CX(EXT(current_tstamp),%edx) / set initial time stamp
	movl	S_ARG0,%ecx			/ get timer
	movl	%ecx,CX(EXT(current_timer),%edx) / set initial timer
	ret

#endif	/* accurate timing */

/*
 * Encapsulate the transfer of exception stack frames between a PCB
 * and a thread stack.  Since the whole point of these is to emulate
 * a call or exception that changes privilege level, both macros
 * assume that there is no user esp or ss stored in the source
 * frame (because there was no change of privilege to generate them).
 */

/*
 * Transfer a stack frame from a thread's user stack to its PCB.
 * We assume the thread and stack addresses have been loaded into 
 * registers (our arguments).
 *
 * The macro overwrites edi, esi, ecx and whatever registers hold the
 * thread and stack addresses (which can't be one of the above three).
 * The thread address is overwritten with the address of its saved state
 * (where the frame winds up).
 *
 * Must be called on kernel stack.
 */
#define FRAME_STACK_TO_PCB(thread, stkp)					;\
	movl	ACT_PCB(thread),thread	/* get act`s PCB */		;\
	leal	PCB_ISS(thread),%edi	/* point to PCB`s saved state */;\
	movl	%edi,thread		/* save for later */		;\
	movl	stkp,%esi		/* point to start of frame */	;\
	movl	$R_UESP,%ecx						;\
	sarl	$2,%ecx			/* word count for transfer */	;\
	cld				/* we`re incrementing */	;\
	rep								;\
	movsl				/* transfer the frame */	;\
	addl	$R_UESP,stkp		/* derive true "user" esp */	;\
	movl	stkp,R_UESP(thread)	/* store in PCB */		;\
	movl	$0,%ecx							;\
	mov	%ss,%cx			/* get current ss */		;\
	movl	%ecx,R_SS(thread)	/* store in PCB */

/*
 * Transfer a stack frame from a thread's PCB to the stack pointed
 * to by the PCB.  We assume the thread address has been loaded into
 * a register (our argument).
 *
 * The macro overwrites edi, esi, ecx and whatever register holds the
 * thread address (which can't be one of the above three).  The
 * thread address is overwritten with the address of its saved state
 * (where the frame winds up).
 *
 * Must be called on kernel stack.
 */
#define FRAME_PCB_TO_STACK(thread)					;\
	movl	ACT_PCB(thread),thread	/* get act`s PCB */		;\
	leal	PCB_ISS(thread),%esi	/* point to PCB`s saved state */;\
	movl	R_UESP(%esi),%edi	/* point to end of dest frame */;\
	movl	$R_UESP,%ecx						;\
	subl	%ecx,%edi		/* derive start of frame */	;\
	movl	%edi,thread		/* save for later */		;\
	sarl	$2,%ecx			/* word count for transfer */	;\
	cld				/* we`re incrementing */	;\
	rep								;\
	movsl				/* transfer the frame */

#undef PDEBUG

#ifdef PDEBUG

/*
 * Traditional, not ANSI.
 */
#define CAH(label) \
	.data ;\
	.globl label/**/count ;\
label/**/count: ;\
	.long	0 ;\
	.globl label/**/limit ;\
label/**/limit: ;\
	.long	0 ;\
	.text ;\
	addl	$1,%ss:label/**/count ;\
	cmpl	$0,label/**/limit ;\
	jz	label/**/exit ;\
	pushl	%eax ;\
label/**/loop: ;\
	movl	%ss:label/**/count,%eax ;\
	cmpl	%eax,%ss:label/**/limit ;\
	je	label/**/loop ;\
	popl	%eax ;\
label/**/exit:

#else	/* PDEBUG */

#define CAH(label)

#endif	/* PDEBUG */

/*
 * Trap/interrupt entry points.
 *
 * All traps must create the following save area on the PCB "stack":
 *
 *	gs
 *	fs
 *	es
 *	ds
 *	edi
 *	esi
 *	ebp
 *	cr2 if page fault - otherwise unused
 *	ebx
 *	edx
 *	ecx
 *	eax
 *	trap number
 *	error code
 *	eip
 *	cs
 *	eflags
 *	user esp - if from user
 *	user ss  - if from user
 *	es       - if from V86 thread
 *	ds       - if from V86 thread
 *	fs       - if from V86 thread
 *	gs       - if from V86 thread
 *
 */

/*
 * General protection or segment-not-present fault.
 * Check for a GP/NP fault in the kernel_return
 * sequence; if there, report it as a GP/NP fault on the user's instruction.
 *
 * esp->     0:	trap code (NP or GP)
 *	     4:	segment number in error
 *	     8	eip
 *	    12	cs
 *	    16	eflags
 *	    20	old registers (trap is from kernel)
 */
Entry(t_gen_prot)
	pushl	$(T_GENERAL_PROTECTION)	/ indicate fault type
	jmp	trap_check_kernel_exit	/ check for kernel exit sequence

Entry(t_segnp)
	pushl	$(T_SEGMENT_NOT_PRESENT)
					/ indicate fault type

trap_check_kernel_exit:
	testl	$(EFL_VM),16(%esp)	/ is trap from V86 mode?
	jnz	EXT(alltraps)		/ isn`t kernel trap if so
	testl	$3,12(%esp)		/ is trap from kernel mode?
	jne	EXT(alltraps)		/ if so:
					/ check for the kernel exit sequence
	cmpl	$EXT(kret_iret),8(%esp)	/ on IRET?
	je	fault_iret
	cmpl	$EXT(kret_popl_ds),8(%esp) / popping DS?
	je	fault_popl_ds	
	cmpl	$EXT(kret_popl_es),8(%esp) / popping ES?
	je	fault_popl_es
	cmpl	$EXT(kret_popl_fs),8(%esp) / popping FS?
	je	fault_popl_fs
	cmpl	$EXT(kret_popl_gs),8(%esp) / popping GS?
	je	fault_popl_gs
take_fault:				/ if none of the above:
	jmp	EXT(alltraps)		/ treat as normal trap.

/*
 * GP/NP fault on IRET: CS or SS is in error.
 * All registers contain the user's values.
 *
 * on SP is
 *  0	trap number
 *  4	errcode
 *  8	eip
 * 12	cs		--> trapno
 * 16	efl		--> errcode
 * 20	user eip
 * 24	user cs
 * 28	user eflags
 * 32	user esp
 * 36	user ss
 */
fault_iret:
	movl	%eax,8(%esp)		/ save eax (we don`t need saved eip)
	popl	%eax			/ get trap number
	movl	%eax,12-4(%esp)		/ put in user trap number
	popl	%eax			/ get error code
	movl	%eax,16-8(%esp)		/ put in user errcode
	popl	%eax			/ restore eax
	CAH(fltir)
	jmp	EXT(alltraps)		/ take fault

/*
 * Fault restoring a segment register.  The user's registers are still
 * saved on the stack.  The offending segment register has not been
 * popped.
 */
fault_popl_ds:
	popl	%eax			/ get trap number
	popl	%edx			/ get error code
	addl	$12,%esp		/ pop stack to user regs
	jmp	push_es			/ (DS on top of stack)
fault_popl_es:
	popl	%eax			/ get trap number
	popl	%edx			/ get error code
	addl	$12,%esp		/ pop stack to user regs
	jmp	push_fs			/ (ES on top of stack)
fault_popl_fs:
	popl	%eax			/ get trap number
	popl	%edx			/ get error code
	addl	$12,%esp		/ pop stack to user regs
	jmp	push_gs			/ (FS on top of stack)
fault_popl_gs:
	popl	%eax			/ get trap number
	popl	%edx			/ get error code
	addl	$12,%esp		/ pop stack to user regs
	jmp	push_segregs		/ (GS on top of stack)

push_es:
	pushl	%es			/ restore es,
push_fs:
	pushl	%fs			/ restore fs,
push_gs:
	pushl	%gs			/ restore gs.
push_segregs:
	movl	%eax,R_TRAPNO(%esp)	/ set trap number
	movl	%edx,R_ERR(%esp)	/ set error code
	CAH(fltpp)
	jmp	trap_set_segs		/ take trap

/*
 * Debug trap.  Check for single-stepping across system call into
 * kernel.  If this is the case, taking the debug trap has turned
 * off single-stepping - save the flags register with the trace
 * bit set.
 */
Entry(t_debug)
	testl	$(EFL_VM),8(%esp)	/ is trap from V86 mode?
	jnz	0f			/ isn`t kernel trap if so
	testl	$3,4(%esp)		/ is trap from kernel mode?
	jnz	0f			/ if so:
	cmpl	$syscall_entry,(%esp)	/ system call entry?
	jne	0f			/ if so:
					/ flags are sitting where syscall
					/ wants them
	addl	$8,%esp			/ remove eip/cs
	jmp	syscall_entry_2		/ continue system call entry

0:	pushl	$0			/ otherwise:
	pushl	$(T_DEBUG)		/ handle as normal
	jmp	EXT(alltraps)		/ debug fault

/*
 * Page fault traps save cr2.
 */
Entry(t_page_fault)
	pushl	$(T_PAGE_FAULT)		/ mark a page fault trap
	pusha				/ save the general registers
	movl	%cr2,%eax		/ get the faulting address
	movl	%eax,12(%esp)		/ save in esp save slot
	jmp	trap_push_segs		/ continue fault

/*
 * All 'exceptions' enter here with:
 *	esp->   trap number
 *		error code
 *		old eip
 *		old cs
 *		old eflags
 *		old esp		if trapped from user
 *		old ss		if trapped from user
 *
 * NB: below use of CPU_NUMBER assumes that macro will use correct
 * segment register for any kernel data accesses.
 */
Entry(alltraps)
	pusha				/ save the general registers
trap_push_segs:
	pushl	%ds			/ save the segment registers
	pushl	%es
	pushl	%fs
	pushl	%gs

trap_set_segs:
	cld				/ clear direction flag
	testl	$(EFL_VM),R_EFLAGS(%esp) / in V86 mode?
	jnz	trap_from_user		/ user mode trap if so
	testb	$3,R_CS(%esp)		/ user mode trap?
	jnz	trap_from_user
	CPU_NUMBER(%edx)
	cmpl	$0,CX(EXT(active_kloaded),%edx)
	je	trap_from_kernel	/ if clear, truly in kernel
	cmpl	$EXT(etext),R_EIP(%esp)	/ pc within kernel?
	jb	trap_from_kernel
trap_from_kloaded:
	/*
	 * We didn't enter here "through" PCB (i.e., using ring 0 stack),
	 * so transfer the stack frame into the PCB explicitly, then
	 * start running on resulting "PCB stack".  We have to set
	 * up a simulated "uesp" manually, since there's none in the
	 * frame.
	 */
	CAH(atstart)
	CPU_NUMBER(%edx)
	movl	CX(EXT(active_kloaded),%edx),%ebx
	movl	CX(EXT(kernel_stack),%edx),%eax
	xchgl	%esp,%eax
	FRAME_STACK_TO_PCB(%ebx,%eax)
	CAH(atend)
	jmp	EXT(take_trap)

trap_from_user:
	mov	%ss,%ax			/ switch to kernel data segment
	mov	%ax,%ds			/ (same as kernel stack segment)
	mov	%ax,%es

	CPU_NUMBER(%edx)
	TIME_TRAP_UENTRY

	movl	CX(EXT(kernel_stack),%edx),%ebx
	xchgl	%ebx,%esp		/ switch to kernel stack
					/ user regs pointer already set
LEXT(take_trap)
	pushl	%ebx			/ record register save area
	pushl	%ebx			/ pass register save area to trap
	call	EXT(user_trap)		/ call user trap routine
	movl	4(%esp),%esp		/ switch back to PCB stack

/*
 * Return from trap or system call, checking for ASTs.
 * On PCB stack.
 */

LEXT(return_from_trap)
	CPU_NUMBER(%edx)
	cmpl	$0,CX(EXT(need_ast),%edx)
	je	EXT(return_to_user)	/ if we need an AST:

	movl	CX(EXT(kernel_stack),%edx),%esp
					/ switch to kernel stack
	pushl	$0			/ push preemption flag
	call	EXT(i386_astintr)	/ take the AST
	addl	$4,%esp			/ pop preemption flag
	popl	%esp			/ switch back to PCB stack (w/exc link)
	jmp	EXT(return_from_trap)	/ and check again (rare)
					/ ASTs after this point will
					/ have to wait

/*
 * Arrange the checks needed for kernel-loaded (or kernel-loading)
 * threads so that branch is taken in kernel-loaded case.
 */
LEXT(return_to_user)
	TIME_TRAP_UEXIT
	CPU_NUMBER(%eax)
	cmpl	$0,CX(EXT(active_kloaded),%eax)
	jnz	EXT(return_xfer_stack)
	movl	CX(EXT(active_threads),%eax),%ebx / get active thread
	movl	TH_TOP_ACT(%ebx),%ebx		/ get thread->top_act
	cmpl	$0,ACT_KLOADING(%ebx)		/ check if kernel-loading
	jnz	EXT(return_kernel_loading)
	
/*
 * Return from kernel mode to interrupted thread.
 */

LEXT(return_from_kernel)
LEXT(kret_popl_gs)
	popl	%gs			/ restore segment registers
LEXT(kret_popl_fs)
	popl	%fs
LEXT(kret_popl_es)
	popl	%es
LEXT(kret_popl_ds)
	popl	%ds
	popa				/ restore general registers
	addl	$8,%esp			/ discard trap number and error code

LEXT(kret_iret)
	iret				/ return from interrupt


LEXT(return_xfer_stack)
	/*
	 * If we're on PCB stack in a kernel-loaded task, we have
	 * to transfer saved state back to thread stack and swap
	 * stack pointers here, because the hardware's not going
	 * to do so for us.
	 */
	CAH(rxsstart)
	CPU_NUMBER(%eax)
	movl	CX(EXT(kernel_stack),%eax),%esp
	movl	CX(EXT(active_kloaded),%eax),%eax
	FRAME_PCB_TO_STACK(%eax)
	movl	%eax,%esp
	CAH(rxsend)
	jmp	EXT(return_from_kernel)

/*
 * Hate to put this here, but setting up a separate swap_func for
 * kernel-loaded threads no longer works, since thread executes
 * "for a while" (i.e., until it reaches glue code) when first
 * created, even if it's nominally suspended.  Hence we can't
 * transfer the PCB when the thread first resumes, because we
 * haven't initialized it yet. 
 */
/*
 * Have to force transfer to new stack "manually".  Use a string
 * move to transfer all of our saved state to the stack pointed
 * to by iss.uesp, then install a pointer to it as our current
 * stack pointer.
 */
LEXT(return_kernel_loading)
	CPU_NUMBER(%eax)
	movl	CX(EXT(kernel_stack),%eax),%esp
	movl	CX(EXT(active_threads),%eax),%ebx / get active thread
	movl	TH_TOP_ACT(%ebx),%ebx		/ get thread->top_act
	movl	%ebx,%edx			/ save for later
	movl	$0,ACT_KLOADING(%edx)		/ clear kernel-loading bit
	FRAME_PCB_TO_STACK(%ebx)
	movl	%ebx,%esp			/ start running on new stack
	movl	$1,ACT_KLOADED(%edx)		/ set kernel-loaded bit
	movl	%edx,CX(EXT(active_kloaded),%eax) / set cached indicator
	jmp	EXT(return_from_kernel)

/*
 * Trap from kernel mode.  No need to switch stacks or load segment registers.
 */
trap_from_kernel:
#if	MACH_KDB || MACH_KGDB
	movl	%ss,%ax
	movl	%ax,%ds
	movl	%ax,%es			# switch to kernel data seg
	movl	%esp,%ebx		# save current stack
	
	cmpl	EXT(int_stack_high),%esp # on an interrupt stack?
	jb	2f			# OK if so

	CPU_NUMBER(%edx)		# get CPU number
	cmpl	CX(EXT(kernel_stack),%edx),%esp
					# if not already on kernel stack,
	ja	0f			#   check some more
	cmpl	CX(EXT(active_stacks),%edx),%esp
	ja	2f			# on kernel stack: no switch
0:
	movl	CX(EXT(kernel_stack),%edx),%esp
2:
	pushl	%ebx			# save old stack
	pushl	%ebx			# pass as parameter
	call	EXT(kernel_trap)	# to kernel trap routine
	addl	$4,%esp			# pop parameter
	popl	%esp			# return to old stack
#else	/* MACH_KDB || MACH_KGDB */
	pushl	%esp			# pass parameter
	call	EXT(kernel_trap)	# to kernel trap routine
	addl	$4,%esp			# pop parameter
#endif	/* MACH_KDB || MACH_KGDB */

#if	MACH_RT
	CPU_NUMBER(%edx)

	movl	CX(EXT(need_ast),%edx),%eax / get pending asts
	testl	$AST_URGENT,%eax	/ any urgent preemption?
	je	EXT(return_from_kernel)	/ no, nothing to do
	cmpl	$0,EXT(preemptable)	/ kernel-mode, preemption enabled?
	je	EXT(return_from_kernel)	/ no, skip it
	cmpl	$T_PREEMPT,48(%esp)	/ preempt request?
	jne	EXT(return_from_kernel)	/ no, nothing to do
	movl	CX(EXT(kernel_stack),%edx),%eax
	movl	%esp,%ecx
	xorl	%eax,%ecx
	andl	$(-KERNEL_STACK_SIZE),%ecx
	testl	%ecx,%ecx		/ are we on the kernel stack?
	jne	EXT(return_from_kernel)	/ no, skip it

	pushl	$1			/ push preemption flag
	call	EXT(i386_astintr)	/ take the AST
	addl	$4,%esp			/ pop preemption flag
#endif	/* MACH_RT */

	jmp	EXT(return_from_kernel)

/*
 *	Called as a function, makes the current thread
 *	return from the kernel as if from an exception.
 */

	.globl	EXT(thread_exception_return)
	.globl	EXT(thread_bootstrap_return)
LEXT(thread_exception_return)
LEXT(thread_bootstrap_return)
	movl	%esp,%ecx			/ get kernel stack
	or	$(KERNEL_STACK_SIZE-1),%ecx
	movl	-3-IKS_SIZE(%ecx),%esp		/ switch back to PCB stack
	jmp	EXT(return_from_trap)

Entry(call_continuation)
	movl	S_ARG0,%eax			/ get continuation
	movl	%esp,%ecx			/ get kernel stack
	or	$(KERNEL_STACK_SIZE-1),%ecx
	addl	$(-3-IKS_SIZE),%ecx
	movl	%ecx,%esp			/ pop the stack
	xorl	%ebp,%ebp			/ zero frame pointer
	jmp	*%eax				/ goto continuation

#if 0
#define LOG_INTERRUPT(info,msg)			\
        pushal				;	\
        pushl	msg			;	\
        pushl	info			;	\
        call	EXT(log_thread_action)	;	\
        add	$8,%esp			;	\
        popal
#define CHECK_INTERRUPT_TIME(n)                 \
	pushal				;	\
	pushl	$n			;	\
	call	EXT(check_thread_time)	;	\
	add	$4,%esp			;	\
	popal
#else
#define LOG_INTERRUPT(info,msg)
#define CHECK_INTERRUPT_TIME(n)
#endif

imsg_start:
	String	"interrupt start"
imsg_end:
	String	"interrupt end"

/*
 * All interrupts enter here.
 * old %eax on stack; interrupt number in %eax.
 */
Entry(all_intrs)
	pushl	%ecx			/ save registers
	pushl	%edx
	cld				/ clear direction flag

	cmpl	%ss:EXT(int_stack_high),%esp / on an interrupt stack?
	jb	int_from_intstack	/ if not:

	pushl	%ds			/ save segment registers
	pushl	%es
	mov	%ss,%dx			/ switch to kernel segments
	mov	%dx,%ds
	mov	%dx,%es

	CPU_NUMBER(%edx)

	movl	CX(EXT(int_stack_top),%edx),%ecx
	xchgl	%ecx,%esp		/ switch to interrupt stack

#if	STAT_TIME
	pushl	%ecx			/ save pointer to old stack
#else
	pushl	%ebx			/ save %ebx - out of the way
					/ so stack looks the same
	pushl	%ecx			/ save pointer to old stack
	TIME_INT_ENTRY			/ do timing
#endif

	LOG_INTERRUPT(%eax, $imsg_start)

	call	EXT(interrupt)		/ call generic interrupt routine

	LOG_INTERRUPT($0, $imsg_end)

	CHECK_INTERRUPT_TIME(1500)

	.globl	EXT(return_to_iret)
LEXT(return_to_iret)			/ (label for kdb_kintr and hardclock)

	CPU_NUMBER(%edx)
#if	STAT_TIME
#else
	TIME_INT_EXIT			/ do timing
	movl	4(%esp),%ebx		/ restore the extra reg we saved
#endif

	popl	%esp			/ switch back to old stack

	movl	CX(EXT(need_ast),%edx),%eax
	testl	%eax,%eax		/ any pending asts?
	je	1f			/ no, nothing to do
	testl	$(EFL_VM),I_EFL(%esp)	/ if in V86
	jnz	ast_from_interrupt	/ take it
	testb	$3,I_CS(%esp)		/ user mode,
	jnz	ast_from_interrupt	/ take it
	cmpl	$EXT(etext),I_EIP(%esp) / if within kernel-loaded task,
	jnb	ast_from_interrupt	/ take it

#if	MACH_RT
	cmpl	$0,EXT(preemptable)	/ kernel-mode, preemption enabled?
	je	1f			/ no, skip it
	cmpl	$0,CX(EXT(preemption_level),%edx) / preemption masked?
	jne	1f			/ yes, skip it
	testl	$AST_URGENT,%eax	/ any urgent requests?
	je	1f			/ no, skip it
	cmpl	$LEXT(locore_end),I_EIP(%esp)	/ are we in locore code?
	jb	1f			/ yes, skip it
	movl	CX(EXT(kernel_stack),%edx),%eax
	movl	%esp,%ecx
	xorl	%eax,%ecx
	andl	$(-KERNEL_STACK_SIZE),%ecx
	testl	%ecx,%ecx		/ are we on the kernel stack?
	jne	1f			/ no, skip it

/*
 * Take an AST from kernel space.  We don't need (and don't want)
 * to do as much as the case where the interrupt came from user
 * space.
 */

	sti
	pushl	$1			/ push preemption flag
	call	EXT(i386_astintr)	/ take the AST
	addl	$4,%esp			/ pop preemption flag
#endif	/* MACH_RT */

1:
	pop	%es			/ restore segment regs
	pop	%ds
	pop	%edx
	pop	%ecx
	pop	%eax
	iret				/ return to caller

int_from_intstack:
	call	EXT(interrupt)		/ call interrupt routine
LEXT(return_to_iret_i)			/ ( label for kdb_kintr)
	pop	%edx			/ must have been on kernel segs
	pop	%ecx
	pop	%eax			/ no ASTs
	iret

/*
 *	Take an AST from an interrupt.
 *	On PCB stack.
 * sp->	es	-> edx
 *	ds	-> ecx
 *	edx	-> eax
 *	ecx	-> trapno
 *	eax	-> code
 *	eip
 *	cs
 *	efl
 *	esp
 *	ss
 */
ast_from_interrupt:
	pop	%es			/ restore all registers ...
	pop	%ds
	popl	%edx
	popl	%ecx
	popl	%eax
	sti				/ Reenable interrupts
	pushl	$0			/ zero code
	pushl	$0			/ zero trap number
	pusha				/ save general registers
	push	%ds			/ save segment registers
	push	%es
	push	%fs
	push	%gs
	mov	%ss,%dx			/ switch to kernel segments
	mov	%dx,%ds
	mov	%dx,%es

	/*
	 * See if we interrupted a kernel-loaded thread executing
	 * in its own task.
	 */
	CPU_NUMBER(%edx)
	cmpl	$EXT(etext),R_EIP(%esp)
	jb	0f			/ not kernel-loaded, back to normal

	/*
	 * Transfer the current stack frame by hand into the PCB.
	 */
	CAH(afistart)
	movl	CX(EXT(active_kloaded),%edx),%eax
	movl	CX(EXT(kernel_stack),%edx),%ebx
	xchgl	%ebx,%esp
	FRAME_STACK_TO_PCB(%eax,%ebx)
	CAH(afiend)
	TIME_TRAP_UENTRY
	jmp	3f
0:
	TIME_TRAP_UENTRY

	movl	CX(EXT(kernel_stack),%edx),%eax
					/ switch to kernel stack
	xchgl	%eax,%esp
3:
	pushl	%eax
	pushl	$0			/ push preemption flag
	call	EXT(i386_astintr)	/ take the AST
	addl	$4,%esp			/ pop preemption flag
	popl	%esp			/ back to PCB stack
	jmp	EXT(return_from_trap)	/ return

#if	MACH_KDB || MACH_KGDB
/*
 * kdb_kintr:	enter kdb from keyboard interrupt.
 * Chase down the stack frames until we find one whose return
 * address is the interrupt handler.   At that point, we have:
 *
 * frame->	saved %ebp
 *		return address in interrupt handler
 *		ivect
 *		saved SPL
 *		return address == return_to_iret_i
 *		saved %edx
 *		saved %ecx
 *		saved %eax
 *		saved %eip
 *		saved %cs
 *		saved %efl
 *
 * OR:
 * frame->	saved %ebp
 *		return address in interrupt handler
 *		ivect
 *		saved SPL
 *		return address == return_to_iret
 *		pointer to save area on old stack
 *	      [ saved %ebx, if accurate timing ]
 *
 * old stack:	saved %es
 *		saved %ds
 *		saved %edx
 *		saved %ecx
 *		saved %eax
 *		saved %eip
 *		saved %cs
 *		saved %efl
 *
 * Call kdb, passing it that register save area.
 */

#if MACH_KGDB
Entry(kgdb_kintr)
#endif /* MACH_KGDB */
#if MACH_KDB
Entry(kdb_kintr)
#endif /* MACH_KDB */
	movl	%ebp,%eax		/ save caller`s frame pointer
	movl	$EXT(return_to_iret),%ecx / interrupt return address 1
	movl	$EXT(return_to_iret_i),%edx / interrupt return address 2

0:	cmpl	16(%eax),%ecx		/ does this frame return to
					/ interrupt handler (1)?
	je	1f
	cmpl	$kdb_from_iret,16(%eax)
	je	1f
	cmpl	16(%eax),%edx		/ interrupt handler (2)?
	je	2f			/ if not:
	cmpl	$kdb_from_iret_i,16(%eax)
	je	2f
	movl	(%eax),%eax		/ try next frame
	jmp	0b

1:	movl	$kdb_from_iret,16(%eax)	/ returns to kernel/user stack
	ret

2:	movl	$kdb_from_iret_i,16(%eax)
					/ returns to interrupt stack
	ret

/*
 * On return from keyboard interrupt, we will execute
 * kdb_from_iret_i
 *	if returning to an interrupt on the interrupt stack
 * kdb_from_iret
 *	if returning to an interrupt on the user or kernel stack
 */
kdb_from_iret:
					/ save regs in known locations
#if	STAT_TIME
	pushl	%ebx			/ caller`s %ebx is in reg
#else
	movl	4(%esp),%eax		/ get caller`s %ebx
	pushl	%eax			/ push on stack
#endif
	pushl	%ebp
	pushl	%esi
	pushl	%edi
	push	%fs
	push	%gs
	pushl	%esp			/ pass regs
#if MACH_KGDB
	call    EXT(kgdb_kentry)	/ to kgdb
#endif /* MACH_KGDB */
#if MACH_KDB
        call    EXT(kdb_kentry)		/ to kdb
#endif /* MACH_KDB */
	addl	$4,%esp			/ pop parameters
	pop	%gs			/ restore registers
	pop	%fs
	popl	%edi
	popl	%esi
	popl	%ebp
#if	STAT_TIME
	popl	%ebx
#else
	popl	%eax
	movl	%eax,4(%esp)
#endif
	jmp	EXT(return_to_iret)	/ normal interrupt return

kdb_from_iret_i:			/ on interrupt stack
	pop	%edx			/ restore saved registers
	pop	%ecx
	pop	%eax
	pushl	$0			/ zero error code
	pushl	$0			/ zero trap number
	pusha				/ save general registers
	push	%ds			/ save segment registers
	push	%es
	push	%fs
	push	%gs
	pushl	%esp			/ pass regs,
	pushl	$0			/ code,
	pushl	$-1			/ type to kdb
#if MACH_KGDB
	call    EXT(kgdb_trap)
#endif /* MACH_KGDB */
#if MACH_KDB
        call    EXT(kdb_trap)
#endif /* MACH_KDB */
	addl	$12,%esp		/ remove parameters
	pop	%gs			/ restore segment registers
	pop	%fs
	pop	%es
	pop	%ds
	popa				/ restore general registers
	addl	$8,%esp
	iret

#endif	/* MACH_KDB || MACH_KGDB */


/*
 * Mach RPC enters through a call gate, like a system call.
 *
 * blah, blah, blah ...
 *
 */

Entry(mach_rpc)
	pushf				/ save flags as soon as possible
	pushl	%eax			/ save system call number
	pushl	$0			/ clear trap number slot

	pusha				/ save the general registers
	pushl	%ds			/ and the segment registers
	pushl	%es
	pushl	%fs
	pushl	%gs

	mov	%ss,%dx			/ switch to kernel data segment
	mov	%dx,%ds
	mov	%dx,%es

/*
 * Shuffle eflags,eip,cs into proper places
 */

	movl	R_EIP(%esp),%ebx	/ eflags are in EIP slot
	movl	R_CS(%esp),%ecx		/ eip is in CS slot
	movl	R_EFLAGS(%esp),%edx	/ cs is in EFLAGS slot
	movl	%ecx,R_EIP(%esp)	/ fix eip
	movl	%edx,R_CS(%esp)		/ fix cs
	movl	%ebx,R_EFLAGS(%esp)	/ fix eflags

	CPU_NUMBER(%edx)
        TIME_TRAP_UENTRY

        negl    %eax                    / get system call number
        shll    $4,%eax                 / manual indexing

/*
 * For mach_rpc_return_trap, switch to client (lower) activation
 * stack now. But leave space for client buffers on the stack.
 *
 * XXX When rpc buffers move from the kernel stack to the activation,
 *     the "skip client buffers on stack" instruction will no longer
 *     be necessary.
 */

        cmpl    $0x240, %eax            / XXX mach_rpc_return_trap number
        jne     1f

        movl    CX(EXT(active_threads),%edx), %ebx
        movl    TH_TOP_ACT(%ebx), %ebx
        movl    ACT_LOWER(%ebx), %ebx

        cmpl    $0, %ebx
        je      1f

        movl    ACT_KERNEL_STACK(%ebx), %ebx
        movl    %ebx, CX(EXT(active_stacks),%edx)
        lea     KERNEL_STACK_SIZE-IKS_SIZE-IEL_SIZE(%ebx), %ebx
        movl    %ebx, CX(EXT(kernel_stack),%edx)

        xchgl   %ebx, %esp
        subl    $(RPC_CLIENT_BUF_SIZE), %esp	/ XXX skip client buffers on stk
        jmp     3f

1:
/*
 * Check here for mach_rpc from kernel-loaded task --
 *  - Note that kernel-loaded task returns via real return. 
 * We didn't enter here "through" PCB (i.e., using ring 0 stack),
 * so transfer the stack frame into the PCB explicitly, then
 * start running on resulting "PCB stack".  We have to set
 * up a simulated "uesp" manually, since there's none in the
 * frame.
 */
        cmpl    $0,CX(EXT(active_kloaded),%edx)
        jz      2f
        CAH(mrstart)
        movl    CX(EXT(active_kloaded),%edx),%ebx
        movl    CX(EXT(kernel_stack),%edx),%edx
        xchgl   %edx,%esp

        FRAME_STACK_TO_PCB(%ebx,%edx)
        CAH(mrend)

        CPU_NUMBER(%edx)
        jmp     3f

2:
	CPU_NUMBER(%edx)
	movl	CX(EXT(kernel_stack),%edx),%ebx
					/ get current kernel stack
	xchgl	%ebx,%esp		/ switch stacks - %ebx points to
					/ user registers.

3:

/*
 * Register use on entry:
 *   eax contains syscall number
 *   ebx contains user regs pointer
 */
#undef 	RPC_TRAP_REGISTERS
#ifdef	RPC_TRAP_REGISTERS
	pushl	R_ESI(%ebx)
	pushl	R_EDI(%ebx)
	pushl	R_ECX(%ebx)
	pushl	R_EDX(%ebx)
#else
	movl	EXT(mach_trap_table)(%eax),%ecx
					/ get number of arguments
	jecxz	2f			/ skip argument copy if none
	movl	R_UESP(%ebx),%esi	/ get user stack pointer
	lea	4(%esi,%ecx,4),%esi	/ skip user return address,
					/ and point past last argument
	cmpl	$(VM_MAX_ADDRESS),%esi	/ in user space?
	ja	mach_call_addr		/ address error if not
	movl	%esp,%edx		/ save kernel ESP for error recovery
1:
	subl	$4,%esi
	RECOVER(mach_call_addr_push)
	pushl	(%esi)			/ push argument on stack
	loop	1b			/ loop for all arguments
#endif

/*
 * Register use on entry:
 *   eax contains syscall number
 *   ebx contains user regs pointer
 */
2:
	CAH(call_call)
	call	*EXT(mach_trap_table)+4(%eax)
					/ call procedure
	movl	%esp,%ecx		/ get kernel stack
	or	$(KERNEL_STACK_SIZE-1),%ecx
	movl	-3-IKS_SIZE(%ecx),%esp	/ switch back to PCB stack
	movl	%eax,R_EAX(%esp)	/ save return value
	jmp	EXT(return_from_trap)	/ return to user

/*
 * Wrapper to make an RPC return from a collocated target "look
 * like" the trap used to return from a user-space target.  Record
 * both the return code (%ecx) and a "frame pointer" used to
 * locate arguments, where mach_rpc_return_trap() expects to
 * find them.
 */
Entry(mach_rpc_return_wrapper)
	movl	CX(EXT(active_threads),%eax),%eax	/ get active thread
	movl	TH_TOP_ACT(%eax),%eax			/ get thread->top_act
	movl	ACT_PCB(%eax),%eax			/ get act's PCB
	leal	PCB_ISS(%eax),%eax			/ get PCB's saved state
	movl	%ecx,R_ECX(%eax)			/ save return code
	leal	-8(%esp),%ecx				/ save frame pointer
	movl	%ecx,R_EBP(%eax)

/*
 * For mach_rpc_return_trap, switch to client (lower) activation
 * stack now. But leave space for client buffers on the stack.
 */
        CPU_NUMBER(%edx)

        movl    CX(EXT(active_threads),%edx), %ebx
        movl    TH_TOP_ACT(%ebx), %ebx
        movl    ACT_LOWER(%ebx), %ebx

        cmpl    $0, %ebx
        je      1f

        movl    ACT_KERNEL_STACK(%ebx), %ebx
        movl    %ebx, CX(EXT(active_stacks),%edx)
        lea     KERNEL_STACK_SIZE-IKS_SIZE-IEL_SIZE(%ebx), %ebx
        movl    %ebx, CX(EXT(kernel_stack),%edx)

        xchgl   %ebx, %esp
        subl    $RPC_CLIENT_BUF_SIZE, %esp	/ XXX skip client buffers on stk
        subl    $2048, %esp             / XXX For debugger only
1:
	jmp	EXT(mach_rpc_return_trap)

/*
 * System call enters through a call gate.  Flags are not saved -
 * we must shuffle stack to look like trap save area.
 *
 * esp->	old eip
 *		old cs
 *		old esp
 *		old ss
 *
 * eax contains system call number.
 *
 * NB: below use of CPU_NUMBER assumes that macro will use correct
 * correct segment register for any kernel data accesses.
 */
Entry(syscall)
syscall_entry:
	pushf				/ save flags as soon as possible
syscall_entry_2:
	pushl	%eax			/ save system call number
	pushl	$0			/ clear trap number slot

	pusha				/ save the general registers
	pushl	%ds			/ and the segment registers
	pushl	%es
	pushl	%fs
	pushl	%gs

	mov	%ss,%dx			/ switch to kernel data segment
	mov	%dx,%ds
	mov	%dx,%es

/*
 * Shuffle eflags,eip,cs into proper places
 */

	movl	R_EIP(%esp),%ebx	/ eflags are in EIP slot
	movl	R_CS(%esp),%ecx		/ eip is in CS slot
	movl	R_EFLAGS(%esp),%edx	/ cs is in EFLAGS slot
	movl	%ecx,R_EIP(%esp)	/ fix eip
	movl	%edx,R_CS(%esp)		/ fix cs
	movl	%ebx,R_EFLAGS(%esp)	/ fix eflags

	CPU_NUMBER(%edx)
/*
 * Check here for syscall from kernel-loaded task --
 * We didn't enter here "through" PCB (i.e., using ring 0 stack),
 * so transfer the stack frame into the PCB explicitly, then
 * start running on resulting "PCB stack".  We have to set
 * up a simulated "uesp" manually, since there's none in the
 * frame.
 */
	cmpl	$0,CX(EXT(active_kloaded),%edx)
	jz	0f
	CAH(scstart)
	movl	CX(EXT(active_kloaded),%edx),%ebx
	movl	CX(EXT(kernel_stack),%edx),%edx
	xchgl	%edx,%esp
	FRAME_STACK_TO_PCB(%ebx,%edx)
	CAH(scend)
	TIME_TRAP_UENTRY
	CPU_NUMBER(%edx)
	jmp	1f

0:
	TIME_TRAP_UENTRY

	CPU_NUMBER(%edx)
	movl	CX(EXT(kernel_stack),%edx),%ebx
					/ get current kernel stack
	xchgl	%ebx,%esp		/ switch stacks - %ebx points to
					/ user registers.
					/ user regs pointer already set

/*
 * Check for MACH or emulated system call
 * Register use (from here till we begin processing call):
 *   eax contains system call number
 *   ebx points to user regs
 */
1:
	movl	CX(EXT(active_threads),%edx),%edx
					/ point to current thread
	movl	TH_TOP_ACT(%edx),%edx	/ get thread->top_act
	movl	ACT_TASK(%edx),%edx	/ point to task
	movl	TASK_EMUL(%edx),%edx	/ get emulation vector
	orl	%edx,%edx		/ if none,
	je	syscall_native		/    do native system call
	movl	%eax,%ecx		/ copy system call number
	subl	DISP_MIN(%edx),%ecx	/ get displacement into syscall
					/ vector table
	jl	syscall_native		/ too low - native system call
	cmpl	DISP_COUNT(%edx),%ecx	/ check range
	jnl	syscall_native		/ too high - native system call
	movl	DISP_VECTOR(%edx,%ecx,4),%edx
					/ get the emulation vector
	orl	%edx,%edx		/ emulated system call if not zero
	jnz	syscall_emul

/*
 * Native system call.
 * Register use on entry:
 *   eax contains syscall number
 *   ebx points to user regs
 */
syscall_native:
	negl	%eax			/ get system call number
	jl	mach_call_range		/ out of range if it was positive

	cmpl	EXT(mach_trap_count),%eax / check system call table bounds
	jg	mach_call_range		/ error if out of range
	shll	$4,%eax			/ manual indexing

	movl	EXT(mach_trap_table)+4(%eax),%edx
					/ get procedure
	cmpl	$EXT(kern_invalid),%edx	/ if not "kern_invalid" 
	jne	mach_syscall_native	/ 	go on with Mach syscall

	CPU_NUMBER(%edx)
	movl	CX(EXT(active_threads),%edx),%edx
					/ point to current thread
	movl	TH_TOP_ACT(%edx),%edx	/ get thread->top_act
	movl	ACT_TASK(%edx),%edx	/ point to task	
	movl	TASK_EMUL(%edx),%edx	/ get emulation vector
	orl	%edx,%edx		/ if it exists,
	jne	mach_syscall_native	/    do native system call
	shrl	$4,%eax			/ restore syscall number
	jmp	mach_call_range		/ try it as a "server" syscall
		
mach_syscall_native:
	CPU_NUMBER(%edx)
	movl	CX(EXT(active_threads),%edx),%edx
					/ point to current thread
	movl	TH_TOP_ACT(%edx),%edx	/ get thread->top_act
	movl	ACT_MACH_EXC_PORT(%edx),%edx
	movl	$EXT(realhost),%ecx
	movl	HOST_NAME(%ecx),%ecx
	cmpl	%edx,%ecx		/ act->mach_exc_port = host_name ?
	je	do_native_call		/ -> send to kernel, do not collect $200
	cmpl	$0,%edx			/ thread->mach_exc_port = null ?
	je	try_task		/ try task
	jmp	mach_syscall_exception
        /* NOT REACHED */

try_task:
	CPU_NUMBER(%edx)
	movl	CX(EXT(active_threads),%edx),%edx
					/ point to current thread
	movl	TH_TOP_ACT(%edx),%edx	/ get thread->top_act
	movl	ACT_TASK(%edx),%edx	/ point to task
	movl	TASK_MACH_EXC_PORT(%edx),%edx
	movl	$EXT(realhost),%ecx
	movl	HOST_NAME(%ecx),%ecx
	cmpl	%edx,%ecx		/ thread->mach_exc_port = host_name ?
	je	do_native_call		/ -> send to kernel
	cmpl	$0,%edx			/ thread->mach_exc_port = null ?
	je	EXT(syscall_failed)	/ try task
	jmp	mach_syscall_exception
	/* NOT REACHED */

/*
 * Register use on entry:
 *   eax contains syscall number
 *   ebx contains user regs pointer
 */
do_native_call:
	movl	EXT(mach_trap_table)(%eax),%ecx
					/ get number of arguments
	jecxz	mach_call_call		/ skip argument copy if none
	movl	R_UESP(%ebx),%esi	/ get user stack pointer
	lea	4(%esi,%ecx,4),%esi	/ skip user return address,
					/ and point past last argument
	CPU_NUMBER(%edx)
	movl	CX(EXT(active_kloaded),%edx),%edx
					/ point to current thread
	orl	%edx,%edx		/ if kernel-loaded, skip addr check
	jnz	0f
	cmpl	$(VM_MAX_ADDRESS),%esi	/ in user space?
	ja	mach_call_addr		/ address error if not
0:
	movl	%esp,%edx		/ save kernel ESP for error recovery
1:
	subl	$4,%esi
	RECOVER(mach_call_addr_push)
	pushl	(%esi)			/ push argument on stack
	loop	1b			/ loop for all arguments

/*
 * Register use on entry:
 *   eax contains syscall number
 *   ebx contains user regs pointer
 */
mach_call_call:
	CAH(call_call)
	call	*EXT(mach_trap_table)+4(%eax)
					/ call procedure
	movl	%esp,%ecx		/ get kernel stack
	or	$(KERNEL_STACK_SIZE-1),%ecx
	movl	-3-IKS_SIZE(%ecx),%esp	/ switch back to PCB stack
	movl	%eax,R_EAX(%esp)	/ save return value
	jmp	EXT(return_from_trap)	/ return to user

/*
 * Address out of range.  Change to page fault.
 * %esi holds failing address.
 * Register use on entry:
 *   ebx contains user regs pointer
 */
mach_call_addr_push:
	movl	%edx,%esp		/ clean parameters from stack
mach_call_addr:
	movl	%esi,R_CR2(%ebx)	/ set fault address
	movl	$(T_PAGE_FAULT),R_TRAPNO(%ebx)
					/ set page-fault trap
	movl	$(T_PF_USER),R_ERR(%ebx)
					/ set error code - read user space
	CAH(call_addr)
	jmp	EXT(take_trap)		/ treat as a trap

/*
 * try sending mach system call exception to server
 * Register use on entry:
 *   eax contains syscall number
 */
mach_syscall_exception:
	push	%eax			/ code (syscall no.)
	movl	%esp,%edx
	push	$1			/ code_cnt = 1
	push	%edx			/ exception_type_t (see i/f docky)
	push	$EXC_MACH_SYSCALL	/ exception
	CAH(exception)
	call	EXT(exception)
	/* no return */

/*
 * System call out of range.  Treat as invalid-instruction trap.
 * (? general protection?)
 * Register use on entry:
 *   eax contains syscall number
 */
mach_call_range:
	CPU_NUMBER(%edx)
	movl	CX(EXT(active_threads),%edx),%edx
					/ point to current thread
	movl	TH_TOP_ACT(%edx),%edx	/ get thread->top_act
	movl	ACT_TASK(%edx),%edx	/ point to task
	movl	TASK_EMUL(%edx),%edx	/ get emulation vector
	orl	%edx,%edx		/ if emulator,
	jne	EXT(syscall_failed)	/    handle as illegal instruction
					/ else generate syscall exception:
	push 	%eax
	movl	%esp,%edx
	push	$1			/ code_cnt = 1
	push	%edx			/ exception_type_t (see i/f docky)
	push	$EXC_SYSCALL
	CAH(call_range)
	call	EXT(exception)
	/* no return */

/*
 * Restart a mach system call after raising an exception.
 */
	.globl  EXT(restart_mach_syscall)
LEXT(restart_mach_syscall)
	movl	%esp,%ecx		/ get kernel stack
	or	$(KERNEL_STACK_SIZE-1),%ecx
	movl	-3-IKS_SIZE(%ecx),%ebx	/ get user PCB
	movl	R_EAX(%ebx),%eax	/ syscall number,
	negl	%eax			/ negate it and
	shll	$4,%eax			/ change to syscall table index
	CAH(restart)
	jmp	do_native_call		/ try native system call

	.globl	EXT(syscall_failed)
LEXT(syscall_failed)
	movl	%esp,%ecx		/ get kernel stack
	or	$(KERNEL_STACK_SIZE-1),%ecx
	movl	-3-IKS_SIZE(%ecx),%esp	/ switch back to PCB stack
	CPU_NUMBER(%edx)
	movl	CX(EXT(kernel_stack),%edx),%ebx
					/ get current kernel stack
	xchgl	%ebx,%esp		/ switch stacks - %ebx points to
					/ user registers.
					/ user regs pointer already set

	movl	$(T_INVALID_OPCODE),R_TRAPNO(%ebx)
					/ set invalid-operation trap
	movl	$0,R_ERR(%ebx)		/ clear error code
	CAH(failed)
	jmp	EXT(take_trap)		/ treat as a trap

/*
 * User space emulation of system calls.
 * edx - user address to handle syscall
 *
 * User stack will become:
 * uesp->	eflags
 *		eip
 * Register use on entry:
 *   ebx contains user regs pointer
 *   edx contains emulator vector address
 */
syscall_emul:
	movl	R_UESP(%ebx),%edi	/ get user stack pointer
	CPU_NUMBER(%eax)
	movl	CX(EXT(active_kloaded),%eax),%eax
	orl	%eax,%eax		/ if thread not kernel-loaded,
	jz	0f			/   do address checks
	subl	$8,%edi
	jmp	1f			/ otherwise, skip them
0:
	cmpl	$(VM_MAX_ADDRESS),%edi	/ in user space?
	ja	syscall_addr		/ address error if not
	subl	$8,%edi			/ push space for new arguments
	cmpl	$(VM_MIN_ADDRESS),%edi	/ still in user space?
	jb	syscall_addr		/ error if not
1:
	movl	R_EFLAGS(%ebx),%eax	/ move flags
	RECOVER(syscall_addr)
	movl	%eax,0(%edi)		/ to user stack
	movl	R_EIP(%ebx),%eax	/ move eip
	RECOVER(syscall_addr)
	movl	%eax,4(%edi)		/ to user stack
	movl	%edi,R_UESP(%ebx)	/ set new user stack pointer
	movl	%edx,R_EIP(%ebx)	/ change return address to trap
	movl	%ebx,%esp		/ back to PCB stack
	CAH(emul)
	jmp	EXT(return_from_trap)	/ return to user


/*
 * Address error - address is in %edi.
 * Register use on entry:
 *   ebx contains user regs pointer
 */
syscall_addr:
	movl	%edi,R_CR2(%ebx)	/ set fault address
	movl	$(T_PAGE_FAULT),R_TRAPNO(%ebx)
					/ set page-fault trap
	movl	$(T_PF_USER),R_ERR(%ebx)
					/ set error code - read user space
	CAH(addr)
	jmp	EXT(take_trap)		/ treat as a trap

/**/
/*
 * Utility routines.
 */

/*
 * RPC upcall to a server.
 *
 * arg0:	pcb stack pointer
 * arg1:	user stack pointer
 * arg2:	server function eip
 * arg3: 	return code
 */
ENTRY(rpc_upcall)
        movl    S_ARG0, %esi          	/ get pcb stack pointer
	movl	S_ARG1, %edi		/ get user stack pointer
	movl	%edi, R_UESP(%esi)	/ set user stack pointer from trap
	movl	S_ARG2, %edi		/ get server function eip
	movl	%edi, R_EIP(%esi)	/ set return address from trap
	movl	S_ARG3, %edi		/ get return code
	movl	%edi, R_EAX(%esi)	/ set return code from trap 
	movl 	%esi, %esp		/ switch to pcb stack
	jmp	EXT(return_from_trap)	/ return to user	


/*
 * Copy from user address space.
 * arg0:	user address
 * arg1:	kernel address
 * arg2:	byte count
 */
Entry(copyinmsg)
ENTRY(copyin)
	pushl	%esi
	pushl	%edi			/ save registers

	movl	8+S_ARG0,%esi		/ get user start address
	movl	8+S_ARG1,%edi		/ get kernel destination address
	movl	8+S_ARG2,%edx		/ get count

	lea	0(%esi,%edx),%eax	/ get user end address + 1

	CPU_NUMBER(%ecx)
	movl	CX(EXT(active_kloaded),%ecx),%ecx
	orl	%ecx,%ecx
	jnz	0f
#if	(VM_MIN_ADDRESS != 0)
	cmpl	$(VM_MIN_ADDRESS),%esi	/ is start within user space?
	jb	copyin_fail
#endif
	cmpl	$(VM_MAX_ADDRESS),%eax
	ja	copyin_fail		/ or above user range
0:
	cmpl	%esi,%eax
	jb	copyin_fail		/ fail if wrap-around
	cld				/ count up
	movl	%edx,%ecx		/ move by longwords first
	shrl	$2,%ecx
	RECOVER(copyin_fail)
	rep
	movsl				/ move longwords
	movl	%edx,%ecx		/ now move remaining bytes
	andl	$3,%ecx
	RECOVER(copyin_fail)
	rep
	movsb
	xorl	%eax,%eax		/ return 0 for success
copy_ret:
	popl	%edi			/ restore registers
	popl	%esi
	ret				/ and return

copyin_fail:
	movl	$1,%eax			/ return 1 for failure
	jmp	copy_ret		/ pop frame and return

/*
 * Copy string from user address space.
 * arg0:	user address
 * arg1:	kernel address
 * arg2:	max byte count
 * arg3:	actual byte count (OUT)
 */
Entry(copyinstr)
	pushl	%esi
	pushl	%edi			/ save registers

	movl	8+S_ARG0,%esi		/ get user start address
	movl	8+S_ARG1,%edi		/ get kernel destination address
	movl	8+S_ARG2,%edx		/ get count

	lea	0(%esi,%edx),%eax	/ get user end address + 1

	CPU_NUMBER(%ecx)
	movl	CX(EXT(active_kloaded),%ecx),%ecx
	orl	%ecx,%ecx
	jnz	0f
#if	(VM_MIN_ADDRESS != 0)
	cmpl	$(VM_MIN_ADDRESS),%esi	/ is start within user space?
	jb	copystr_fail
#endif
	cmpl	$(VM_MAX_ADDRESS),%eax
	ja	copystr_fail		/ or above user range
0:
	cmpl	%esi,%eax
	jb	copystr_fail		/ fail if wrap-around
	xorl	%eax,%eax
	cmpl	$0,%edx
	je	2f
1:
	RECOVER(copystr_fail)		/ copy bytes...
	movb	(%esi),%eax
	incl	%esi
	movb	%eax,(%edi)
	incl	%edi
	decl	%edx
	je	2f			/ ...till max reached
	cmpl	$0,%eax
	jne	1b			/ ...or till a NUL found
2:
	xorl	%eax,%eax		/ return zero for success
	movl	8+S_ARG3,%esi		/ get OUT len ptr
	cmpl	$0,%esi
	jz	copystr_ret		/ if null, just return
	subl	8+S_ARG1,%edi
	movl	%edi,(%esi)		/ else set OUT arg to xfer len
copystr_ret:
	popl	%edi			/ restore registers
	popl	%esi
	ret				/ and return

copystr_fail:
	movl	$1,%eax		/ return 1 for failure
	jmp	copy_ret		/ pop frame and return

/*
 * Copy to user address space.
 * arg0:	kernel address
 * arg1:	user address
 * arg2:	byte count
 */
Entry(copyoutmsg)
ENTRY(copyout)
	pushl	%esi
	pushl	%edi			/ save registers
	pushl	%ebx

	movl	12+S_ARG0,%esi		/ get kernel start address
	movl	12+S_ARG1,%edi		/ get user start address
	movl	12+S_ARG2,%edx		/ get count

	leal	0(%edi,%edx),%eax	/ get user end address + 1

	CPU_NUMBER(%ecx)
	cmpl	$0,CX(EXT(active_kloaded),%ecx)
	jnz	0f

#if	(VM_MIN_ADDRESS != 0)
	cmpl	$(VM_MIN_ADDRESS),%edi	/ within user space?
	jb	copyout_fail
#endif
	cmpl	$(VM_MAX_ADDRESS),%eax
	ja	copyout_fail		/ or above user range
0:
	cmpl	%edi,%eax
	jb	copyout_fail		/ fail if wrap-around
	
/*
 * Check whether user address space is writable
 * before writing to it - hardware is broken.
 *
 * Skip check if "user" address is really in
 * kernel space (i.e., if it's in a kernel-loaded
 * task).
 *
 * Register usage:
 *	esi/edi	source/dest pointers for rep/mov
 *	ecx	counter for rep/mov
 *	edx	counts down from 3rd arg
 *	eax	count of bytes for each (partial) page copy
 *	ebx	shadows edi, used to adjust edx
 */
	movl	%edi,%ebx		/ copy edi for syncing up
copyout_retry:
	/ if restarting after a partial copy, put edx back in sync,
	addl	%ebx,%edx		/ edx -= (edi - ebx);
	subl	%edi,%edx		/
	movl	%edi,%ebx		/ ebx = edi;

	cmpl	$(VM_MAX_ADDRESS),%edi
	ja	0f
	movl	%cr3,%ecx		/ point to page directory
#if	NCPUS > 1 && AT386
	andl	$(~0x7), %ecx		/ remove cpu number
#endif	/* NCPUS > 1 && AT386 */
	movl	%edi,%eax		/ get page directory bits
	shrl	$(PDESHIFT),%eax	/ from user address
	movl	KERNELBASE(%ecx,%eax,4),%ecx
					/ get page directory pointer
	testl	$(PTE_V),%ecx		/ present?
	jz	0f			/ if not, fault is OK
	andl	$(PTE_PFN),%ecx		/ isolate page frame address
	movl	%edi,%eax		/ get page table bits
	shrl	$(PTESHIFT),%eax
	andl	$(PTEMASK),%eax		/ from user address
	leal	KERNELBASE(%ecx,%eax,4),%ecx
					/ point to page table entry
	movl	(%ecx),%eax		/ get it
	testl	$(PTE_V),%eax		/ present?
	jz	0f			/ if not, fault is OK
	testl	$(PTE_W),%eax		/ writable?
	jnz	0f			/ OK if so
/*
 * Not writable - must fake a fault.  Turn off access to the page.
 */
	andl	$(PTE_INVALID),(%ecx)	/ turn off valid bit
	movl	%cr3,%eax		/ invalidate TLB
	movl	%eax,%cr3
0:
/*
 * Copy only what fits on the current destination page.
 * Check for write-fault again on the next page.
 */
	leal	NBPG(%edi),%eax		/ point to
	andl	$(-NBPG),%eax		/ start of next page
	subl	%edi,%eax		/ get number of bytes to that point
	cmpl	%edx,%eax		/ bigger than count?
	jle	1f			/ if so,
	movl	%edx,%eax		/ use count
1:
	cld				/ count up
	movl	%eax,%ecx		/ move by longwords first
	shrl	$2,%ecx
	RECOVER(copyout_fail)
	RETRY(copyout_retry)
	rep
	movsl
	movl	%eax,%ecx		/ now move remaining bytes
	andl	$3,%ecx
	RECOVER(copyout_fail)
	RETRY(copyout_retry)
	rep
	movsb				/ move
	movl	%edi,%ebx		/ copy edi for syncing up
	subl	%eax,%edx		/ and decrement count
	jg	copyout_retry		/ restart on next page if not done
	xorl	%eax,%eax		/ return 0 for success
copyout_ret:
	popl	%ebx
	popl	%edi			/ restore registers
	popl	%esi
	ret				/ and return

copyout_fail:
	movl	$1,%eax			/ return 1 for failure
	jmp	copyout_ret		/ pop frame and return

/*
 * FPU routines.
 */
	
/*
 * Initialize FPU.
 */
ENTRY(_fninit)
	fninit
	ret

/*
 * Read control word
 */
ENTRY(_fstcw)
	pushl	%eax		/ get stack space
	fstcw	(%esp)
	popl	%eax
	ret

/*
 * Set control word
 */
ENTRY(_fldcw)
	fldcw	4(%esp)
	ret

/*
 * Read status word
 */
ENTRY(_fnstsw)
	xor	%eax,%eax		/ clear high 16 bits of eax
	fnstsw	%ax			/ read FP status
	ret

/*
 * Clear FPU exceptions
 */
ENTRY(_fnclex)
	fnclex
	ret

/*
 * Clear task-switched flag.
 */
ENTRY(_clts)
	clts
	ret

/*
 * Save complete FPU state.  Save error for later.
 */
ENTRY(_fpsave)
	movl	4(%esp),%eax		/ get save area pointer
	fnsave	(%eax)			/ save complete state, including
					/ errors
	ret

/*
 * Restore FPU state.
 */
ENTRY(_fprestore)
	movl	4(%esp),%eax		/ get save area pointer
	frstor	(%eax)			/ restore complete state
	ret

/*
 * Set cr3
 */
ENTRY(set_cr3)
#if	NCPUS > 1 && AT386
	CPU_NUMBER(%eax)
	orl	4(%esp), %eax
#else	/* NCPUS > 1 && AT386 */
	movl	4(%esp),%eax		/ get new cr3 value
#endif	/* NCPUS > 1 && AT386 */
	/*
	 * Don't set PDBR to a new value (hence invalidating the
	 * "paging cache") if the new value matches the current one.
	 */
	movl	%cr3,%edx		/ get current cr3 value
	cmpl	%eax,%edx
	je	0f			/ if two are equal, don't set
	movl	%eax,%cr3		/ load it (and flush cache)
0:
	ret

/*
 * Read cr3
 */
ENTRY(get_cr3)
	movl	%cr3,%eax
#if	NCPUS > 1 && AT386
	andl	$(~0x7), %eax		/* remove cpu number */
#endif	/* NCPUS > 1 && AT386 */
	ret

/*
 * Flush TLB
 */
ENTRY(flush_tlb)
	movl	%cr3,%eax		/ flush tlb by reloading CR3
	movl	%eax,%cr3		/ with itself
	ret

/*
 * Read cr2
 */
ENTRY(get_cr2)
	movl	%cr2,%eax
	ret

/*
 * Read ldtr
 */
Entry(get_ldt)
	xorl	%eax,%eax
	sldt	%ax
	ret

/*
 * Set ldtr
 */
Entry(set_ldt)
	lldt	4(%esp)
	ret

/*
 * Read task register.
 */
ENTRY(get_tr)
	xorl	%eax,%eax
	str	%ax
	ret

/*
 * Set task register.  Also clears busy bit of task descriptor.
 */
ENTRY(set_tr)
	movl	S_ARG0,%eax		/ get task segment number
	subl	$8,%esp			/ push space for SGDT
	sgdt	2(%esp)			/ store GDT limit and base (linear)
	movl	4(%esp),%edx		/ address GDT
	movb	$(K_TSS),5(%edx,%eax)	/ fix access byte in task descriptor
	ltr	%ax			/ load task register
	addl	$8,%esp			/ clear stack
	ret				/ and return

/*
 * Set task-switched flag.
 */
ENTRY(_setts)
	movl	%cr0,%eax		/ get cr0
	orl	$(CR0_TS),%eax		/ or in TS bit
	movl	%eax,%cr0		/ set cr0
	ret

/*
 * io register must not be used on slaves (no AT bus)
 */
#if	NCPUS > 1 && defined(CBUS)
#define	ILL_ON_SLAVE			\
	CPU_NUMBER(%eax)	;	\
	cmpl	$0, %eax	;	\
	je	0f		;	\
	call	EXT(cbus_ill_io);	\
0:
#else	/* NCPUS > 1 && defined(Z1000) */
#define	ILL_ON_SLAVE
#endif	/* NCPUS > 1 && defined(Z1000) */


/*
 * void outb(unsigned char *io_port,
 *	     unsigned char byte)
 *
 * Output a byte to an IO port.
 */
ENTRY(outb)
	ILL_ON_SLAVE
	movl	S_ARG0,%edx		/ IO port address
	movl	S_ARG1,%eax		/ data to output
	outb	%al,%dx			/ send it out
#ifdef	iPSC386
	mull	%ecx			/ Delay a little to make H/W happy
#endif	/* iPSC386 */
	ret

/*
 * unsigned char inb(unsigned char *io_port)
 *
 * Input a byte from an IO port.
 */
ENTRY(inb)
	ILL_ON_SLAVE
	movl	S_ARG0,%edx		/ IO port address
	xor	%eax,%eax		/ clear high bits of register
	inb	%dx,%al			/ get the byte
#ifdef	iPSC386
/ Do a long multiply to delay a little to make H/W happy.  Must
/ save and restore EAX which is used to hold result of multiply
	pushl	%eax
	mull	%ecx
	popl	%eax
#endif	/* iPSC386 */
	ret

/*
 * void outw(unsigned short *io_port,
 *	     unsigned short word)
 *
 * Output a word to an IO port.
 */
ENTRY(outw)
	ILL_ON_SLAVE
	movl	S_ARG0,%edx		/ IO port address
	movl	S_ARG1,%eax		/ data to output
	outw	%ax,%dx			/ send it out
	ret

/*
 * unsigned short inw(unsigned short *io_port)
 *
 * Input a word from an IO port.
 */
ENTRY(inw)
	ILL_ON_SLAVE
	movl	S_ARG0,%edx		/ IO port address
	xor	%eax,%eax		/ clear high bits of register
	inw	%dx,%ax			/ get the word
	ret

/*
 * void outl(unsigned int *io_port,
 *	     unsigned int byte)
 *
 * Output an int to an IO port.
 */
ENTRY(outl)
	ILL_ON_SLAVE
	movl	S_ARG0,%edx		/ IO port address
	movl	S_ARG1,%eax		/ data to output
	outl	%eax,%dx		/ send it out
	ret

/*
 * unsigned int inl(unsigned int *io_port)
 *
 * Input an int from an IO port.
 */
ENTRY(inl)
	ILL_ON_SLAVE
	movl	S_ARG0,%edx		/ IO port address
	inl	%dx,%eax		/ get the int
	ret

/*
 * void loutb(unsigned byte *io_port,
 *	      unsigned byte *data,
 *	      unsigned int count)
 *
 * Output an array of bytes to an IO port.
 */
ENTRY(loutb)
	ILL_ON_SLAVE
	movl	%esi,%eax		/ save register
	movl	S_ARG0,%edx		/ get io port number
	movl	S_ARG1,%esi		/ get data address
	movl	S_ARG2,%ecx		/ get count

	cld				/ count up

	rep
	outsb				/ output

	movl	%eax,%esi		/ restore register
	ret				/ exit


/*
 * void loutw(unsigned short *io_port,
 *	      unsigned short *data,
 *	      unsigned int count)
 *
 * Output an array of shorts to an IO port.
 */
ENTRY(loutw)
	ILL_ON_SLAVE
	movl	%esi,%eax		/ save register
	movl	S_ARG0,%edx		/ get io port number
	movl	S_ARG1,%esi		/ get data address
	movl	S_ARG2,%ecx		/ get count

	cld				/ count up

	rep
	outsw				/ output

	movl	%eax,%esi		/ restore register
	ret				/ exit


/*
 * void linb(unsigned char *io_port,
 *	     unsigned char *data,
 *	     unsigned int count)
 *
 * Input an array of bytes from an IO port.
 */
ENTRY(linb)
	ILL_ON_SLAVE
	movl	%edi,%eax		/ save register
	movl	S_ARG0,%edx		/ get io port number
	movl	S_ARG1,%edi		/ get data address
	movl	S_ARG2,%ecx		/ get count

	cld				/ count up

	rep
	insb				/ input

	movl	%eax,%edi		/ restore register
	ret				/ exit


/*
 * void linw(unsigned short *io_port,
 *	     unsigned short *data,
 *	     unsigned int count)
 *
 * Input an array of shorts from an IO port.
 */
ENTRY(linw)
	ILL_ON_SLAVE
	movl	%edi,%eax		/ save register
	movl	S_ARG0,%edx		/ get io port number
	movl	S_ARG1,%edi		/ get data address
	movl	S_ARG2,%ecx		/ get count

	cld				/ count up

	rep
	insw				/ input

	movl	%eax,%edi		/ restore register
	ret				/ exit


/*
 * int inst_fetch(int eip, int cs);
 *
 * Fetch instruction byte.  Return -1 if invalid address.
 */
	.globl	EXT(inst_fetch)
LEXT(inst_fetch)
	movl	S_ARG1, %eax		/ get segment
	movw	%ax,%fs			/ into FS
	movl	S_ARG0, %eax		/ get offset
	RETRY(EXT(inst_fetch))		/ re-load FS on retry
	RECOVER(EXT(inst_fetch_fault))
	movzbl	%fs:(%eax),%eax		/ load instruction byte
	ret

LEXT(inst_fetch_fault)
	movl	$-1,%eax		/ return -1 if error
	ret


/*
 * Done with recovery and retry tables.
 */
	RECOVER_TABLE_END
	RETRY_TABLE_END



ENTRY(dr6)
	movl	%db6, %eax
	ret

/*	dr<i>(address, type, len, persistence)
 */
ENTRY(dr0)
	movl	S_ARG0, %eax
	movl	%eax,EXT(dr_addr)
	movl	%eax, %db0
	movl	$0, %ecx
	jmp	0f
ENTRY(dr1)
	movl	S_ARG0, %eax
	movl	%eax,EXT(dr_addr)+1*4
	movl	%eax, %db1
	movl	$2, %ecx
	jmp	0f
ENTRY(dr2)
	movl	S_ARG0, %eax
	movl	%eax,EXT(dr_addr)+2*4
	movl	%eax, %db2
	movl	$4, %ecx
	jmp	0f

ENTRY(dr3)
	movl	S_ARG0, %eax
	movl	%eax,EXT(dr_addr)+3*4
	movl	%eax, %db3
	movl	$6, %ecx

0:
	pushl	%ebp
	movl	%esp, %ebp

	movl	%db7, %edx
	movl	%edx,EXT(dr_addr)+4*4
	andl	dr_msk(,%ecx,2),%edx	/* clear out new entry */
	movl	%edx,EXT(dr_addr)+5*4
	movzbl	B_ARG3, %eax
	andb	$3, %al
	shll	%cl, %eax
	orl	%eax, %edx

	movzbl	B_ARG1, %eax
	andb	$3, %al
	addb	$0x10, %ecx
	shll	%cl, %eax
	orl	%eax, %edx

	movzbl	B_ARG2, %eax
	andb	$3, %al
	addb	$0x2, %ecx
	shll	%cl, %eax
	orl	%eax, %edx

	movl	%edx, %db7
	movl	%edx,EXT(dr_addr)+7*4
	movl	%edx, %eax
	leave
	ret

	.data
	.comm	EXT(spinlock_count),4*NCPUS
	.comm	EXT(preemption_level),4*NCPUS
	.comm	EXT(interrupt_level),4*NCPUS

DATA(preemptable)	/ Not on an MP (makes cpu_number() usage unsafe)
#if	MACH_RT && (NCPUS == 1)
	.long	1
#else
	.long	0
#endif	/* MACH_RT && (NCPUS == 1) */

dr_msk:
	.long	~0x000f0003
	.long	~0x00f0000c
	.long	~0x0f000030
	.long	~0xf00000c0
ENTRY(dr_addr)
	.long	0,0,0,0
	.long	0,0,0,0
	.text

/*
 * Determine cpu model and set global cpu_x86_model
 *
 * Relies on 386 eflags bit 18 (AC) always being zero & 486 preserving it.
 * Relies on 486 eflags bit 21 (ID) always being zero & 586 preserving it.
 * (assumes cpu_x86_model-homogenous MPs; else convert to per-cpu array)
 */

ENTRY(set_cpu_model)
	movl	%esp,%edx	/ save stack pointer
	and	$~0x3,%esp	/ Align stack to avoid AC fault
	pushl	%ebx		/ save ebx
	pushfl			/ push EFLAGS
	popl	%eax		/ pop into eax
	movl	%eax,%ecx	/ Save original EFLAGS in ecx
	xorl	$(EFL_AC+EFL_ID),%eax	/ toggle ID,AC bits
	pushl	%eax		/ push new value
	popfl			/ through the EFLAGS register
	pushfl			/ and back
	popl	%eax		/ into eax
	movl	$EXT(cpu_x86_model),%ebx	/ ebx = &cpu_x86_model
	movl	$386, (%ebx)			/ *ebx = 386; by default
	xorl	%ecx,%eax	/ see what changed
	testl	$EFL_AC,%eax	/ test AC bit
	jz	0f		/ if AC toggled (486 or higher)
	movl	$486, (%ebx)	/	*ebx = 486;
	testl	$EFL_ID,%eax	/	test ID bit
	jz	0f		/	if ID toggled (586 or higher)
	movl	$586, (%ebx)	/		*ebx = 586;
0:	pushl	%ecx		/ restore EFLAGS
	popfl			/
	popl	%ebx		/ restore ebx
	movl	%edx,%esp	/ restore stack pointer
	ret			/ return;

ENTRY(get_cr0)
	movl	%cr0, %eax
	ret

ENTRY(set_cr0)
	movl	4(%esp), %eax
	movl	%eax, %cr0
	ret

#ifndef	SYMMETRY

/*
 * ffs(mask)
 */
ENTRY(ffs)
	bsfl	S_ARG0, %eax
	jz	0f
	incl	%eax
	ret
0:	xorl	%eax, %eax
	ret

/*
 * cpu_shutdown()
 * Force reboot
 */

null_idtr:
	.word	0
	.long	0

Entry(cpu_shutdown)
        lidt    null_idtr       / disable the interrupt handler
        xor     %ecx,%ecx       / generate a divide by zero
        div     %ecx,%eax       / reboot now
        ret                     / this will "never" be executed

#endif	/* SYMMETRY */


/*
 * setbit(int bitno, int *s) - set bit in bit string
 */
ENTRY(setbit)
	movl	S_ARG0, %ecx		/ bit number
	movl	S_ARG1, %eax		/ address
	btsl	%ecx, (%eax)		/ set bit
	ret

/*
 * clrbit(int bitno, int *s) - clear bit in bit string
 */
ENTRY(clrbit)
	movl	S_ARG0, %ecx		/ bit number
	movl	S_ARG1, %eax		/ address
	btrl	%ecx, (%eax)		/ clear bit
	ret

/*
 * ffsbit(int *s) - find first set bit in bit string
 */
ENTRY(ffsbit)
	movl	S_ARG0, %ecx		/ address
	movl	$0, %edx		/ base offset
0:
	bsfl	(%ecx), %eax		/ check argument bits
	jnz	1f			/ found bit, return
	addl	$4, %ecx		/ increment address
	addl	$32, %edx		/ increment offset
	jmp	0b			/ try again
1:
	addl	%edx, %eax		/ return offset
	ret

#if	NCPUS > 1

ENTRY(minsecurity)
	pushl	%ebp
	movl	%esp,%ebp
/*
 * jail: set the EIP to "jail" to block a kernel thread.
 * Useful to debug synchronization problems on MPs.
 */
ENTRY(jail)
	jmp	EXT(jail)

#endif	/* NCPUS > 1 */

	.text
LEXT(locore_end)
