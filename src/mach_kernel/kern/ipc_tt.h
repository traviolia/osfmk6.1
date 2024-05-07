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
 * Revision 2.6  91/06/25  10:29:05  rpd
 * 	Changed the convert_foo_to_bar functions
 * 	to use ipc_port_t instead of mach_port_t.
 * 	[91/05/27            rpd]
 * 
 * Revision 2.5  91/05/14  16:43:09  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:27:19  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:14:05  mrt]
 * 
 * Revision 2.3  91/01/08  15:16:15  rpd
 * 	Added retrieve_task_self_fast, retrieve_thread_self_fast.
 * 	[90/12/27            rpd]
 * 
 * Revision 2.2  90/06/02  14:54:42  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  22:05:32  rpd]
 * 
 * Revision 2.1  89/08/03  15:57:16  rwd
 * Created.
 * 
 * Revision 2.3  88/09/25  22:14:32  rpd
 * 	Changed includes to the new style.
 * 	[88/09/19  16:25:46  rpd]
 * 
 * Revision 2.2  88/08/06  18:21:33  rpd
 * Created.
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

#ifndef	_KERN_IPC_TT_H_
#define _KERN_IPC_TT_H_

#include <mach/boolean.h>
#include <mach/port.h>
#include <vm/vm_kern.h>
#include <kern/task.h>
#include <kern/thread.h>
#include <kern/ipc_kobject.h>
#include <ipc/ipc_space.h>
#include <ipc/ipc_table.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_right.h>
#include <ipc/ipc_entry.h>
#include <ipc/ipc_object.h>
#include <kern/host.h>


/* Initialize a task's IPC state */
extern void ipc_task_init(
	task_t		task,
	task_t		parent);

/* Enable a task for IPC access */
extern void ipc_task_enable(
	task_t		task);

/* Disable IPC access to a task */
extern void ipc_task_disable(
	task_t		task);

/* Clean up and destroy a task's IPC state */
extern void ipc_task_terminate(
	task_t		task);

/* Initialize a thread's IPC state */
extern void ipc_thread_init(
	thread_t	thread);

/* Clean up and destroy a thread's IPC state */
extern void ipc_thread_terminate(
	thread_t	thread);

/* Return a send right for the task's user-visible self port */
extern ipc_port_t retrieve_task_self_fast(
	task_t		task);

/* Return a send right for the thread's user-visible self port */
extern ipc_port_t retrieve_act_self_fast( thread_act_t );

/* Convert from a port to a task */
extern task_t convert_port_to_task(
	ipc_port_t	port);

/* Convert from a port to a space */
extern ipc_space_t convert_port_to_space(
	ipc_port_t	port);

/* Convert from a port to a map */
extern vm_map_t convert_port_to_map(
	ipc_port_t	port);

/* Convert from a port to a thread */
extern thread_act_t convert_port_to_act( ipc_port_t );

/* Convert from a task to a port */
extern ipc_port_t convert_task_to_port(
	task_t		task);

/* Convert from a thread to a port */
extern ipc_port_t convert_act_to_port( thread_act_t );

/* Deallocate a space ref produced by convert_port_to_space */
extern void space_deallocate(
	ipc_space_t	space);

/* Allocate a reply port */
extern mach_port_t mach_reply_port(void);

/* Initialize a thread_act's ipc mechanism */
extern void ipc_thr_act_init(task_t, thread_act_t);

/* Disable IPC access to a thread_act */
extern void ipc_thr_act_disable(thread_act_t);

/* Clean up and destroy a thread_act's IPC state */
extern void ipc_thr_act_terminate(thread_act_t);

#endif	/* _KERN_IPC_TT_H_ */
