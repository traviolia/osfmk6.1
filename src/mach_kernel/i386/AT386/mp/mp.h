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
 * Revision 2.1.3.2  92/06/24  17:59:35  jeffreyh
 * 	Changes for new at386_io_lock() interface
 * 	[92/06/03            bernadat]
 * 
 * Revision 2.1.3.1  92/04/30  11:58:39  bernadat
 * 	Replaced mistyped 'l' at end of line by ; (Only build
 * 	under ODE detected it).
 * 	[92/04/30            bernadat]
 * 
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

#ifndef _I386AT_MP_H_
#define _I386AT_MP_H_

#include <cpus.h>

#if	NCPUS > 1
/*
 * Cpu number, no local memory on CPUS, this is a hack, limited to 8 cpus,
 * on the 486, bits 0, 1, 2 and 5, 6, 7, 8, 9, 10, 11 are free. We use bits
 * 0, 1 and 2 If we want to use more cpus, we will have to use bits 5-11
 * with shifts.
 */

#define	CPU_NUMBER(r) \
	movl	%cr3, r ; \
	andl	$0x7, r

#define	MP_IPL		SPL6	/* software interrupt level */

/* word describing the reason for the interrupt, one per cpu */

#ifndef	ASSEMBLER
#include <kern/lock.h>
extern	cpu_int_word[];
extern	real_ncpus;		/* real number of cpus */
extern	wncpu;			/* wanted number of cpus */
extern spinlock_t kdb_lock;	/* kdb lock		*/

extern	int	kdb_cpu;		/* current cpu running kdb	*/
extern	int	kdb_debug;
extern	int	kdb_is_slave[];
extern	int	kdb_active[];
#endif	/* ASSEMBLER */

/* Interrupt types */

#define MP_TLB_FLUSH	0x00
#define MP_CLOCK	0x01
#define	MP_KDB		0x02
#define	MP_AST		0x03
#define MP_SOFTCLOCK	0x04
#define MP_INT_AVAIL	0x05

#define i_bit(bit, word)	((long)(*(word)) & ((long)1 << (bit)))


/* 
 *	Device driver synchronization. 
 *
 *	at386_io_lock(op) and at386_io_unlock() are called
 *	by device drivers when accessing H/W. The underlying 
 *	Processing is machine dependant. But the op argument
 *	to the at386_io_lock is generic
 */

#define MP_DEV_OP_MAX	  3
#define MP_DEV_WAIT	  MP_DEV_OP_MAX	/* Wait for the lock */

/*
 * If the caller specifies an op value different than MP_DEV_WAIT, the
 * at386_io_lock function must return true if lock was successful else
 * false
 */

#define MP_DEV_OP_START 0	/* If lock busy, register a pending start op */
#define MP_DEV_OP_INTR	1	/* If lock busy, register a pending intr */
#define MP_DEV_OP_TIMEO	2	/* If lock busy, register a pending timeout */

#else	/* NCPUS > 1 */
#define at386_io_lock_state()
#define at386_io_lock(op)	(TRUE)
#define at386_io_unlock()
#endif	/* NCPUS > 1 */

#endif /* _I386AT_MP_H_ */
