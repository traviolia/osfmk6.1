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
 * Revision 2.3  91/06/25  10:33:21  rpd
 * 	Changed memory_object_t to ipc_port_t where appropriate.
 * 	[91/05/28            rpd]
 * 
 * Revision 2.2  91/05/18  14:39:45  rpd
 * 	Created.
 * 	[91/03/22            rpd]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
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
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 */

#ifndef	_VM_MEMORY_OBJECT_H_
#define	_VM_MEMORY_OBJECT_H_

#include <norma_vm.h>
#include <mach/boolean.h>
#include <ipc/ipc_types.h>

extern kern_return_t	memory_object_set_attributes_common(
				vm_object_t	object,
				boolean_t	may_cache,
#if	NORMA_VM
				boolean_t	object_ready,
				boolean_t	write_completions,
				vm_size_t	cluster_size,
#endif	/* NORMA_VM */
				memory_object_copy_strategy_t copy_strategy,
				boolean_t	temporary,
				char		*existence_map,
				vm_size_t	existence_size);

extern boolean_t	memory_object_sync (
				vm_object_t	object,
				vm_offset_t	offset,
				vm_size_t	size,
				boolean_t	should_flush,
				boolean_t	should_return);

extern ipc_port_t	memory_manager_default_reference(void);

extern boolean_t	memory_manager_default_port(
				ipc_port_t	port);

extern void		memory_manager_default_init(void);

#endif	/* _VM_MEMORY_OBJECT_H_ */
