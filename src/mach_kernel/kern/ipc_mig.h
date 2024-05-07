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

#ifndef	_IPC_MIG_H_
#define	_IPC_MIG_H_

#include <norma_vm.h>
#include <norma_ipc.h>

#include <mach/message.h>
#include <kern/thread.h>
#include <ipc/ipc_thread.h>

/* Send a message from the kernel */
extern mach_msg_return_t mach_msg_send_from_kernel(
	mach_msg_header_t	*msg,
	mach_msg_size_t		send_size);

#if	NORMA_IPC

/* allocate memory for out-of-stack mig structures */
extern char *mig_user_allocate(
	vm_size_t	size);

/* Deallocate memory used for out-of-stack mig structures */
extern void mig_user_deallocate(
	char		*data,
	vm_size_t	size);

#endif	/* NORMA_IPC */

/* Send a message from the kernel and receive a reply */
extern mach_msg_return_t mach_msg_rpc_from_kernel(
	mach_msg_header_t	*msg,
	mach_msg_size_t		send_size,
	mach_msg_size_t		rcv_size);

/* Destroy the thread's ith_rpc_reply port */
extern void mach_msg_abort_rpc(
	ipc_thread_t	thread);

/* In kernel version of mach_msg_overwrite_trap */
extern mach_msg_return_t mach_msg_overwrite(
	mach_msg_header_t	*msg,
	mach_msg_option_t	option,
	mach_msg_size_t		send_size,
	mach_msg_size_t		rcv_size,
	mach_port_t		rcv_name,
	mach_msg_timeout_t	timeout,
	mach_port_t		notify,
	mach_msg_header_t	*rcv_msg,
        mach_msg_size_t		rcv_msg_size);

#endif	/* _IPC_MIG_H_ */
