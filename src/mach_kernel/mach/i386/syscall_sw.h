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
 * Revision 2.5  91/05/14  16:52:22  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:32:17  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:10:01  mrt]
 * 
 * Revision 2.3  90/12/05  23:46:16  af
 * 	Made GNU preproc happy.
 * 
 * Revision 2.2  90/05/03  15:48:01  dbg
 * 	Created.
 * 	[90/04/30  16:36:25  dbg]
 * 
 * Revision 1.3.1.1  89/12/22  22:22:03  rvb
 * 	Use asm.h
 * 	[89/12/22            rvb]
 * 
 * Revision 1.3  89/03/09  20:19:53  rpd
 * 	More cleanup.
 * 
 * Revision 1.2  89/02/26  13:01:00  gm0w
 * 	Changes for cleanup.
 * 
 * 31-Dec-88  Robert Baron (rvb) at Carnegie-Mellon University
 *	Derived from MACH2.0 vax release.
 *
 *  1-Sep-86  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Created from mach_syscalls.h in the user library sources.
 *
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988 Carnegie Mellon University
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

#ifndef	_MACH_I386_SYSCALL_SW_H_
#define _MACH_I386_SYSCALL_SW_H_

#include <machine/asm.h>

#define kernel_trap(trap_name,trap_number,number_args) \
ENTRY(trap_name) \
	movl	$ trap_number,%eax; \
	SVC; \
	ret; 

#define rpc_trap(trap_name,trap_number,number_args) \
ENTRY(trap_name) \
        movl    $ trap_number,%eax; \
        RPC_SVC; \
        ret; 

#endif	/* _MACH_I386_SYSCALL_SW_H_ */
