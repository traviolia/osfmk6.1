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
 * Revision 2.6.3.1  92/03/03  16:20:36  jeffreyh
 * 	Changes from TRUNK
 * 	[92/02/26  11:56:28  jeffreyh]
 * 
 * Revision 2.7  92/01/03  20:17:50  dbg
 * 	Remove fixed lower bound.  Fix type declaration for
 * 	eml_routine_t.  Remove syscall_val structure.
 * 	[91/10/31            dbg]
 * 
 * Revision 2.6  91/06/25  10:29:22  rpd
 * 	Fixed includes to avoid circularities.
 * 	[91/06/24            rpd]
 * 
 * Revision 2.5  91/06/06  17:07:37  jsb
 * 	Added emulation_vector_t for new get/set emulation vector calls.
 * 	[91/05/24  17:47:38  jsb]
 * 
 * Revision 2.4  91/05/14  16:47:16  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:29:30  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:18:04  mrt]
 * 
 * Revision 2.2  90/09/09  14:32:52  rpd
 * 	Use decl_simple_lock_data.
 * 	[90/08/30            rpd]
 * 
 * 	Allow emulation of syscalls with negative numbers.  Clobber MACH
 * 	system calls at your own risk!
 * 	[89/04/19            dbg]
 * 
 * Revision 2.1  89/08/03  15:54:14  rwd
 * Created.
 * 
 * Revision 2.2  88/08/03  15:34:07  dorr
 * Get rid of errno and eosys fields.  Use return value and
 * special return code of ERESTART to represent same information.
 * 
 * 15-Jul-88  David Golub (dbg) at Carnegie-Mellon University
 *	Added lock for reference count.
 *
 * 26-Jan-88  Douglas Orr (dorr) at Carnegie-Mellon University
 *	Added maxsyscall variable and typedefs for user space emulation library
 *
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

#ifndef	_KERN_SYSCALL_EMULATION_H_
#define	_KERN_SYSCALL_EMULATION_H_

#ifndef	ASSEMBLER
#include <mach/machine/vm_types.h>
#include <kern/lock.h>
#include <kern/task.h>

typedef	vm_offset_t	eml_routine_t;

typedef struct eml_dispatch {
	mutex_t		lock;		/* lock for reference count */
	int		ref_count;	/* reference count */
	int 		disp_count; 	/* count of entries in vector */
	int		disp_min;	/* index of lowest entry in vector */
	eml_routine_t	disp_vector[1];	/* first entry in array of dispatch */
					/* routines (array has disp_count
					   elements) */
} *eml_dispatch_t;

typedef vm_offset_t	*emulation_vector_t; /* Variable-length array */

#define EML_ROUTINE_NULL	(eml_routine_t)0
#define EML_DISPATCH_NULL	(eml_dispatch_t)0

#define	EML_SUCCESS		(0)

#define	EML_MOD			(err_kern|err_sub(2))
#define	EML_BAD_TASK		(EML_MOD|0x0001)
#define	EML_BAD_CNT		(EML_MOD|0x0002)

/* Per-task initialization */
extern void	eml_init(void);

/* Take reference on common task emulation vector */
extern void	eml_task_reference(
			task_t	new_task,
			task_t	parent_task);

/* Deallocate reference on common task emulation vector */
extern void	eml_task_deallocate(
			task_t	task);

#endif	/* ASSEMBLER */

#endif	/* _KERN_SYSCALL_EMULATION_H_ */
