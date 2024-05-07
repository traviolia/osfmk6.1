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
 * Revision 2.1.2.1  92/01/21  21:53:23  jsb
 * 	First checkin. Contents moved here from norma/ipc_output.c.
 * 	[92/01/10  20:51:47  jsb]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1992 Carnegie Mellon University
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
 *	File:	norma/ipc_wire.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1992
 *
 *	Functions to wire and unwire pages for norma ipc.
 */

#include <mach/kern_return.h>
#include <mach/port.h>
#include <mach/vm_attributes.h>
#include <mach/vm_param.h>
#include <kern/assert.h>
#include <kern/zalloc.h>
#include <vm/vm_fault.h>
#include <vm/vm_map.h>
#include <vm/vm_object.h>
#include <vm/vm_page.h>
#include <ipc/ipc_port.h>

/*
 * For now, we just steal pages.
 * In the future, we should remember enough information for
 * norma_ipc_thread_return to either pmap_protect appropriate pages
 * or block the thread until the pages are released.
 *
 * I think we don't need a norma_ipc_copyin_page_list_cont; we should be
 * able to use vm_map_copyin_page_list_cont.
 */
kern_return_t
norma_ipc_copyin_page_list(map, addr, len, src_destroy, copy_result, is_cont)
	vm_map_t	map;
	vm_offset_t	addr;
	vm_size_t	len;
	boolean_t	src_destroy;
	vm_map_copy_t	*copy_result;	/* OUT */
	boolean_t	is_cont;
{
	return vm_map_copyin_page_list(map, addr, len, src_destroy, TRUE,
				       copy_result, is_cont, VM_PROT_READ);
}

/*
 * Called when a thread wants to return to user space after possibly having
 * performed a norma_ipc_copyin_page_list.
 *
 * Hooks need to be added somewhere around thread_syscall_return and/or
 * returns from mach_msg_trap, etc.
 *
 * For now, this routine can do nothing (and not be called) because
 * norma_ipc_copyin_page_list simply steals pages.
 */
norma_ipc_thread_return()
{
}
