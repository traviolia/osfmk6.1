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
 * Revision 2.5  91/05/14  16:47:55  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:29:50  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:18:48  mrt]
 * 
 * Revision 2.3  91/01/08  15:17:29  rpd
 * 	Added mach_trap_stack, MACH_TRAP_STACK.
 * 	[90/12/18            rpd]
 * 
 * Revision 2.2  90/06/02  14:56:34  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  22:21:08  rpd]
 * 
 * Revision 2.1  89/08/03  15:46:55  rwd
 * Created.
 * 
 *  7-Mar-89  David Golub (dbg) at Carnegie-Mellon University
 *	Moved user-visible definitions to mach/syscall_sw.h.
 *
 * 28-Oct-88  David Golub (dbg) at Carnegie-Mellon University
 *	Removed all non-MACH calls and options.  Changed
 *	argument-list-size field in mach_trap_table to be the number of
 *	arguments.
 *
 * Revision 2.4  88/10/27  10:48:53  rpd
 * 	Changed msg_{send,receive,rpc}_trap to 20, 21, 22.
 * 	[88/10/26  14:45:13  rpd]
 * 
 * Revision 2.3  88/10/11  10:21:04  rpd
 * 	Changed traps msg_send, msg_receive_, msg_rpc_ to
 * 	msg_send_old, msg_receive_old, msg_rpc_old.
 * 	Added msg_send_trap, msg_receive_trap, msg_rpc_trap.
 * 	[88/10/06  12:24:33  rpd]
 * 
 * 20-Apr-88  David Black (dlb) at Carnegie-Mellon University
 *	Removed thread_times().
 *
 * 18-Jan-88  David Golub (dbg) at Carnegie-Mellon University
 *	Replaced task_data with thread_reply; change is invisible to
 *	users.  Carried over Mary Thompson's change below:
 *
 * Mary Thompson (originally made 19-May-87 in /usr/mach/include)
 *	Went back to old msg_receive_ and msg_rpc_ traps
 *	so that the library would work with the standard kernels
 *
 * 01-Mar-88  Douglas Orr (dorr) at Carnegie-Mellon University
 *	Add htg_unix_syscall
 *
 * 15-May-87  Avadis Tevanian (avie) at Carnegie-Mellon University
 *	LOCORE -> ASSEMBLER.  Removed unnecessary conditional
 *	compilation (assumes MACH to the full extent).
 *
 *  8-Apr-87  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Change name for "msg_receive" to "msg_receive_", to enable user
 *	library to loop looking for software interrupt condition.
 *	Same for msg_rpc.
 *
 * 30-Mar-87  David Black (dlb) at Carnegie-Mellon University
 *	Added kern_timestamp()
 *
 * 27-Mar-87  David Black (dlb) at Carnegie-Mellon University
 *	Added thread_times for MACH_TIME_NEW.  Flushed MACH_TIME.
 *
 * 25-Mar-87  Avadis Tevanian (avie) at Carnegie-Mellon University
 *	Added map_fd, deleted DEFUNCT code.
 *
 * 18-Feb-87  Robert Baron (rvb) at Carnegie-Mellon University
 * 	Added # args to the kernel_trap() macro.
 *	Balance needs to know # of args passed to trap routine
 *
 *  4-Feb-87  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Added ctimes().
 *
 *  7-Nov-86  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Added "init_process".
 *
 *  4-Nov-86  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Removed some old Accent calls; they should be supplanted by
 *	library conversion routines to Mach calls.
 *
 * 29-Oct-86  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Put swtch under MACH_MP; added inode_swap_preference.
 *
 * 12-Oct-86  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Added traps for msg_send, msg_receive, msg_rpc, under
 *	either MACH_ACC or MACH_IPC.  Removed unused calls.
 *	Restructured and renamed trap table.
 *
 *  1-Sep-86  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Moved stuff from user source directory back here, and into
 *	<machine/syscall_sw.h>.  It's only natural that the kernel
 *	sources describe what traps exist, and how they're made.
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
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

#ifndef	_KERN_SYSCALL_SW_H_
#define	_KERN_SYSCALL_SW_H_

/*
 *	mach_trap_stack indicates the trap may discard
 *	its kernel stack.  Some architectures may need
 *	to save more state in the pcb for these traps.
 */

typedef struct {
	int		mach_trap_arg_count;
	int		(*mach_trap_function)(void);
	boolean_t	mach_trap_stack;
	int		mach_trap_unused;
} mach_trap_t;

extern mach_trap_t	mach_trap_table[];
extern int		mach_trap_count;
extern kern_return_t	kern_invalid(void);

#define	MACH_TRAP(name, arg_count)		\
		{ (arg_count), (int (*)(void)) (name), FALSE, 0 }

#define	MACH_TRAP_STACK(name, arg_count)	MACH_TRAP(name, arg_count)

#endif	/* _KERN_SYSCALL_SW_H_ */
