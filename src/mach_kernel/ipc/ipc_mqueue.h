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
 * Revision 2.6  91/08/28  11:13:37  jsb
 * 	Added seqno argument to ipc_mqueue_receive.
 * 	[91/08/10            rpd]
 * 
 * Revision 2.5  91/05/14  16:34:12  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:22:29  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  15:46:46  mrt]
 * 
 * Revision 2.3  91/01/08  15:14:42  rpd
 * 	Changed continuation argument and IMQ_NULL_CONTINUE to (void (*)()).
 * 	[90/12/18            rpd]
 * 	Reorganized ipc_mqueue_receive.
 * 	[90/11/22            rpd]
 * 
 * Revision 2.2  90/06/02  14:50:44  rpd
 * 	Created for new IPC.
 * 	[90/03/26  20:57:40  rpd]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 *	File:	ipc/ipc_mqueue.h
 *	Author:	Rich Draves
 *	Date:	1989
 *
 *	Definitions for message queues.
 */

#ifndef	_IPC_IPC_MQUEUE_H_
#define _IPC_IPC_MQUEUE_H_

#include <mach_assert.h>

#include <mach/message.h>
#include <kern/assert.h>
#include <kern/lock.h>
#include <kern/macro_help.h>
#include <ipc/ipc_kmsg.h>
#include <ipc/ipc_thread.h>
#include <ipc/ipc_object.h>
#include <ipc/ipc_types.h>

typedef struct ipc_mqueue {
	mutex_t	imq_lock_data;
	struct ipc_kmsg_queue imq_messages;
	struct ipc_thread_queue imq_threads;
} *ipc_mqueue_t;

#define	IMQ_NULL		((ipc_mqueue_t) 0)

#define	imq_lock_init(mq)	mutex_init(&(mq)->imq_lock_data)
#define	imq_lock(mq)		mutex_lock(&(mq)->imq_lock_data)
#define	imq_lock_try(mq)	mutex_try(&(mq)->imq_lock_data)
#define	imq_unlock(mq)		mutex_unlock(&(mq)->imq_lock_data)
#define	imq_lock_addr(mq)	(&(mq)->imq_lock_data)

#define	IMQ_NULL_CONTINUE	((void (*)(void)) 0)

/*
 * Exported interfaces
 */
/* Initialize a newly-allocated message queue */
extern void ipc_mqueue_init(
	ipc_mqueue_t	mqueue);

/* Move messages from one queue to another */
extern void ipc_mqueue_move(
	ipc_mqueue_t	dest,
	ipc_mqueue_t	source,
	ipc_port_t	port);

/* Wake up receivers waiting in a message queue */
extern void ipc_mqueue_changed(
	ipc_mqueue_t		mqueue,
	mach_msg_return_t	mr);

/* Send a message to a port */
extern mach_msg_return_t ipc_mqueue_send(
	ipc_kmsg_t		kmsg,
	mach_msg_option_t	option,
	mach_msg_timeout_t	timeout);

/* Convert a name in a space to a message queue */
extern mach_msg_return_t ipc_mqueue_copyin(
	ipc_space_t	space,
	mach_port_t	name,
	ipc_mqueue_t	*mqueuep,
	ipc_object_t	*objectp);

/* Receive a message from a message queue */
extern mach_msg_return_t ipc_mqueue_receive(
	ipc_mqueue_t		mqueue,
	mach_msg_option_t	option,
	mach_msg_size_t		max_size,
	mach_msg_timeout_t	timeout,
	boolean_t		resume,
	void			(*continuation)(void),
	ipc_kmsg_t		*kmsgp,
	mach_port_seqno_t	*seqnop);

/*
 *	extern void
 *	ipc_mqueue_send_always(ipc_kmsg_t);
 *
 *	Unfortunately, to avoid warnings/lint about unused variables
 *	when assertions are turned off, we need two versions of this.
 */

#if	MACH_ASSERT

#define	ipc_mqueue_send_always(kmsg)					\
MACRO_BEGIN								\
	mach_msg_return_t mr;						\
									\
	mr = ipc_mqueue_send((kmsg), MACH_SEND_ALWAYS,			\
			     MACH_MSG_TIMEOUT_NONE);			\
	assert(mr == MACH_MSG_SUCCESS);					\
MACRO_END

#else	/* MACH_ASSERT */

#define	ipc_mqueue_send_always(kmsg)					\
MACRO_BEGIN								\
	(void) ipc_mqueue_send((kmsg), MACH_SEND_ALWAYS,		\
			       MACH_MSG_TIMEOUT_NONE);			\
MACRO_END

#endif	/* MACH_ASSERT */

#endif	/* _IPC_IPC_MQUEUE_H_ */
