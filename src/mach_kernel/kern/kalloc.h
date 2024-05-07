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
 * Revision 2.7  91/05/18  14:31:59  rpd
 * 	Added kalloc_init.
 * 	[91/03/22            rpd]
 * 
 * Revision 2.6  91/05/14  16:43:32  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/02/05  17:27:26  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:14:21  mrt]
 * 
 * Revision 2.4  90/06/02  14:54:51  rpd
 * 	Changed types to vm_offset_t.
 * 	[90/03/26  22:07:07  rpd]
 * 
 * Revision 2.3  89/09/08  11:25:56  dbg
 * 	MACH_KERNEL: remove non-MACH data types.
 * 	[89/07/11            dbg]
 * 
 * Revision 2.2  89/08/31  16:19:04  rwd
 * 	First Checkin
 * 	[89/08/23  15:41:50  rwd]
 * 
 * Revision 2.10  89/03/09  20:13:03  rpd
 * 	More cleanup.
 * 
 * Revision 2.9  89/02/25  18:04:45  gm0w
 * 	Kernel code cleanup.	
 * 	Put entire file under #ifdef KERNEL
 * 	[89/02/15            mrt]
 * 
 * Revision 2.8  89/02/07  01:01:53  mwyoung
 * Relocated from sys/kalloc.h
 * 
 * Revision 2.7  89/01/18  02:10:51  jsb
 * 	Fixed log.
 * 	[88/01/18            rpd]
 * 
 * Revision 2.2  89/01/18  01:16:25  jsb
 * 	Use MINSIZE of 16 instead of 64 (mostly for afs);
 * 	eliminate NQUEUES (see kalloc.c).
 * 	[89/01/13            jsb]
 *
 * 26-Oct-87 Peter King (king) at NeXT, Inc.
 *	Created.
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

#ifndef	_KERN_KALLOC_H_
#define _KERN_KALLOC_H_

#include <mach/machine/vm_types.h>
#include <kern/lock.h>

#define KALLOC_MINSIZE		16

extern vm_offset_t	kalloc(
				vm_size_t	size);

extern void		krealloc(
				vm_offset_t	*addrp,
				vm_size_t	old_size,
				vm_size_t	new_size,
				spinlock_t	*lock);

extern vm_offset_t	kget(
				vm_size_t	size);

extern void		kfree(
				vm_offset_t	data,
				vm_size_t	size);

extern void		kalloc_init(
				void);

#endif	/* _KERN_KALLOC_H_ */
