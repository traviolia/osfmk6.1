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
 * Revision 2.5  91/10/09  15:57:01  af
 * 	 Revision 2.4.3.1  91/10/05  13:03:49  jeffreyh
 * 	 	Added declarations of db_{get,put}_task_value.
 * 	 	Added definitions of implementation dependent access capability.
 * 	 	Added default defines of space access check functions.
 * 	 	[91/08/29            tak]
 * 
 * Revision 2.4.3.1  91/10/05  13:03:49  jeffreyh
 * 	Added declarations of db_{get,put}_task_value.
 * 	Added definitions of implementation dependent access capability.
 * 	Added default defines of space access check functions.
 * 	[91/08/29            tak]
 * 
 * Revision 2.4  91/05/14  15:31:48  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:05:49  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  16:16:37  mrt]
 * 
 * Revision 2.2  90/08/27  21:48:27  dbg
 * 	Created.
 * 	[90/08/07            dbg]
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
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date:	7/90
 */
/*
 * Data access functions for debugger.
 */

#ifndef	_DDB_DB_ACCESS_H_
#define	_DDB_DB_ACCESS_H_

#include <mach/boolean.h>
#include <machine/db_machdep.h>
#include <ddb/db_task_thread.h>

/* implementation dependent access capability */
#define	DB_ACCESS_KERNEL	0	/* only kernel space */
#define DB_ACCESS_CURRENT	1	/* kernel or current task space */
#define DB_ACCESS_ANY		2	/* any space */

#ifndef	DB_ACCESS_LEVEL
#define DB_ACCESS_LEVEL		DB_ACCESS_KERNEL
#endif	/* DB_ACCESS_LEVEL */

#ifndef DB_VALID_KERN_ADDR
#define DB_VALID_KERN_ADDR(addr)	((addr) >= VM_MIN_KERNEL_ADDRESS \
					  && (addr) < VM_MAX_KERNEL_ADDRESS)
#define DB_VALID_ADDRESS(addr,user)	((user != 0) ^ DB_VALID_KERN_ADDR(addr))
#define DB_PHYS_EQ(task1,addr1,task2,addr2)	0
#define DB_CHECK_ACCESS(addr,size,task)	db_is_current_space(task)
#endif	/* DB_VALID_KERN_ADDR */

extern int db_access_level;



/* Prototypes for functions exported by ddb/db_access.c.
 */
db_expr_t db_get_task_value(
	db_addr_t	addr,
	register int	size,
	boolean_t	is_signed,
	task_t		task);

void db_put_task_value(
	db_addr_t	addr,
	register int	size,
	register db_expr_t value,
	task_t		task);

db_expr_t db_get_value(
	db_addr_t	addr,
	int		size,
	boolean_t	is_signed);

void db_put_value(
	db_addr_t	addr,
	int		size,
	db_expr_t	value);

#endif	/* !_DDB_DB_ACCESS_H_ */
