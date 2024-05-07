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
 * Revision 2.5  91/10/09  15:56:44  af
 * 	 Revision 2.4.3.1  91/10/05  13:03:41  jeffreyh
 * 	 	Added db_{get,put}_task_value, and changed db_{get,put}_value
 * 	 	  to call them.  db_{get,put}_value are left for compatibility
 * 	 	  reason.
 * 	 	Added "task" parameter to specifiy target task space.
 * 	 	Added db_access_level to indicate implementation dependent
 * 	 	  access capability.
 * 	 	[91/08/29            tak]
 * 
 * Revision 2.4.3.1  91/10/05  13:03:41  jeffreyh
 * 	Added db_{get,put}_task_value, and changed db_{get,put}_value
 * 	  to call them.  db_{get,put}_value are left for compatibility
 * 	  reason.
 * 	Added "task" parameter to specifiy target task space.
 * 	Added db_access_level to indicate implementation dependent
 * 	  access capability.
 * 	[91/08/29            tak]
 * 
 * Revision 2.4  91/05/14  15:31:33  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:05:44  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  16:16:22  mrt]
 * 
 * Revision 2.2  90/08/27  21:48:20  dbg
 * 	Fix type declarations.
 * 	[90/08/07            dbg]
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
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date:	7/90
 */
#include <mach/boolean.h>
#include <machine/db_machdep.h>		/* type definitions */
#include <machine/setjmp.h>
#include <kern/task.h>
#include <ddb/db_access.h>



/*
 * Access unaligned data items on aligned (longword)
 * boundaries.
 */

int db_access_level = DB_ACCESS_LEVEL;

static unsigned db_extend[] = {	/* table for sign-extending */
	0,
	0xFFFFFF80U,
	0xFFFF8000U,
	0xFF800000U
};

db_expr_t
db_get_task_value(
	db_addr_t	addr,
	register int	size,
	boolean_t	is_signed,
	task_t		task)
{
	char		data[sizeof(int)];
	register db_expr_t value;
	register int	i;

	db_read_bytes((vm_offset_t)addr, size, data, task);

	value = 0;
#if	BYTE_MSF
	for (i = 0; i < size; i++)
#else	/* BYTE_LSF */
	for (i = size - 1; i >= 0; i--)
#endif
	{
	    value = (value << 8) + (data[i] & 0xFF);
	}
	    
	if (size < 4) {
	    if (is_signed && (value & db_extend[size]) != 0)
		value |= db_extend[size];
	}
	return (value);
}

void
db_put_task_value(
	db_addr_t	addr,
	register int	size,
	register db_expr_t value,
	task_t		task)
{
	char		data[sizeof(int)];
	register int	i;

#if	BYTE_MSF
	for (i = size - 1; i >= 0; i--)
#else	/* BYTE_LSF */
	for (i = 0; i < size; i++)
#endif
	{
	    data[i] = value & 0xFF;
	    value >>= 8;
	}

	db_write_bytes((vm_offset_t)addr, size, data, task);
}

db_expr_t
db_get_value(
	db_addr_t	addr,
	int		size,
	boolean_t	is_signed)
{
	return(db_get_task_value(addr, size, is_signed, TASK_NULL));
}

void
db_put_value(
	db_addr_t	addr,
	int		size,
	db_expr_t	value)
{
	db_put_task_value(addr, size, value, TASK_NULL);
}
