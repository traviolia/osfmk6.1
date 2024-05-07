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
 * Revision 2.5  91/05/14  16:09:37  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/03/16  14:44:30  rpd
 * 	Updated for new kmem_alloc interface.
 * 	[91/03/03            rpd]
 * 
 * Revision 2.3  91/02/05  17:12:31  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:35:11  mrt]
 * 
 * Revision 2.2  90/05/03  15:27:57  dbg
 * 	Created.
 * 	[90/02/21            dbg]
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

#include <mach/vm_param.h>
#include <vm/vm_kern.h>
#include <vm/vm_map.h>
#include <i386/pmap.h>
#include <i386/io_map_entries.h>

extern vm_offset_t	virtual_avail;

/*
 * Allocate and map memory for devices that may need to be mapped before
 * Mach VM is running.
 */
vm_offset_t
io_map(phys_addr, size)
	vm_offset_t	phys_addr;
	vm_size_t	size;
{
	vm_offset_t	start;

	if (kernel_map == VM_MAP_NULL) {
	    /*
	     * VM is not initialized.  Grab memory.
	     */
	    start = virtual_avail;
	    virtual_avail += round_page(size);
	}
	else {
	    (void) kmem_alloc_pageable(kernel_map, &start, round_page(size));
	}
	(void) pmap_map_bd(start, phys_addr, phys_addr + round_page(size),
			VM_PROT_READ|VM_PROT_WRITE);
	return (start);
}
