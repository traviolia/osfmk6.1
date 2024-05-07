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
 * 
 */
/*
 * OSF Research Institute MK6.1 (unencumbered) 1/31/1995
 */
#ifndef	_MACH_RPC_COMMON_H_
#define _MACH_RPC_COMMON_H_

/*
 * This structure defines the elements in common between
 * ports and port sets.  The ipc_common_data MUST BE FIRST here,
 * just as the ipc_object must be first within that.
 *
 * This structure must be used anywhere an ipc_object needs
 * to be locked.
 */
typedef struct rpc_common_data {
	struct ipc_common_data	rcd_comm;
	struct thread_pool	rcd_thread_pool;
	struct ipc_entry	rcd_entry;
	spinlock_t		rcd_io_lock_data;
} *rpc_common_t;

#endif	/* _MACH_RPC_COMMON_H_ */
