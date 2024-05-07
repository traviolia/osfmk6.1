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
/*
 *	Definitions for RPC subsystems.
 */

#ifndef	_IPC_IPC_SUBSYSTEM_H_
#define _IPC_IPC_SUBSYSTEM_H_

#include <mach/kern_return.h>
#include <mach/std_types.h>
#include <mach/port.h>
#include <mach/rpc.h>
#include <mach/mach_types.h>
#include <kern/kern_types.h>
#include <kern/lock.h>

#define subsystem_lock_init(subsys)	spinlock_init(&(subsys)->lock)
#define subsystem_lock(subsys)		spinlock_lock(&(subsys)->lock)
#define subsystem_unlock(subsys)	spinlock_unlock(&(subsys)->lock)

/*
 *	A subsystem describes a set of server routines that can be invoked by
 *	mach_rpc() on the ports that are registered with the subsystem.
 *	See struct rpc_subsystem in mach/rpc.h, for more details.
 */
struct subsystem {
	/* Synchronization/destruction information */
	spinlock_t	lock;		/* Subsystem lock		     */
	int		ref_count;	/* Number of references to me	     */
	vm_size_t	size;		/* Number of bytes in this structure */
					/* including the variable length     */
					/* user_susbystem description	     */
        /* Task information */
        task_t          task;           /* Task to which I belong	     */
        queue_chain_t   subsystem_list; /* list of subsystems in task	     */

	/* IPC stuff: */
	struct ipc_port *ipc_self;	/* Port naming this subsystem	     */

	struct rpc_subsystem user;	/* MIG-generated subsystem descr     */
};

#define SUBSYSTEM_NULL	((subsystem_t) 0)

extern void		subsystem_init(void);

/* Subsystem create, with 1 reference. */
extern kern_return_t	mach_subsystem_create(
				task_t			parent_task,
				user_subsystem_t	user_subsys,
				mach_msg_type_number_t	user_subsysCount,
				subsystem_t		*subsystem);

/* Take additional reference on subsystem (make sure it doesn't go away) */
extern void		subsystem_reference(
				subsystem_t	subsystem);

/* Remove one reference on subsystem (it is destroyed if 0 refs remain) */
extern void		subsystem_deallocate(
				subsystem_t	subsystem);

#if	MACH_KDB
extern void		subsystem_print(
				subsystem_t	subsystem);
#endif	/* MACH_KDB */

#endif	/* _IPC_IPC_SUBSYSTEM_H_ */
