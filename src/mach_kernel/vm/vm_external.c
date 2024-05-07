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
 * Revision 2.7  91/05/14  17:48:23  mrt
 * 	Correcting copyright
 * 
 * Revision 2.6  91/02/05  17:57:44  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:31:19  mrt]
 * 
 * Revision 2.5  91/01/08  16:44:33  rpd
 * 	Changed zchange calls to make the zones non-collectable.
 * 	[90/12/29            rpd]
 * 
 * Revision 2.4  90/12/20  17:05:13  jeffreyh
 * 	Change zchange to accept new argument. Made zones collectable.
 * 	[90/12/11            jeffreyh]
 * 
 * Revision 2.3  90/05/29  18:38:33  rwd
 * 	Picked up rfr changes.
 * 	[90/04/12  13:46:29  rwd]
 * 
 * Revision 2.2  90/01/11  11:47:26  dbg
 * 	Add changes from mainline:
 * 		Fixed off-by-one error in vm_external_create.
 * 		[89/12/18  23:40:56  rpd]
 * 
 * 		Keep the bitmap size info around, as it may travel across
 * 		objects.  Reflect this in vm_external_destroy().
 * 		[89/10/16  15:32:06  af]
 * 
 * 		Removed lint.
 * 		[89/08/07            mwyoung]
 * 
 * Revision 2.1  89/08/03  16:44:41  rwd
 * Created.
 * 
 * Revision 2.3  89/04/18  21:24:49  mwyoung
 * 	Created.
 * 	[89/04/18            mwyoung]
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
 *	This module maintains information about the presence of
 *	pages not in memory.  Since an external memory object
 *	must maintain a complete knowledge of its contents, this
 *	information takes the form of hints.
 */
#include <string.h>	/* for memcpy()/memset() */

#include <mach/boolean.h>
#include <kern/kalloc.h>
#include <vm/vm_external.h>
#include <mach/vm_param.h>
#include <kern/assert.h>

/*
 *	This is the largest object size that we will
 *	create existence info for. Assuming a 4K pagesize,
 *	a vm_external_max_size of 4MB results in a max
 *	existence map size of 1K bits or 128 bytes,
 *	which happens to be the old VM_EXTERNAL_SMALL_SIZE.
 */
int vm_external_max_size = 4*1024*1024;

/*
 *	The implementation uses bit arrays to record whether
 *	a page has been written to external storage.
 *
 *	XXXO
 *	Should consider using existence_map to hold bits directly
 *	when existence_size <= 4 bytes (i.e., 32 pages).
 */

void
vm_external_create(
	vm_external_t	result,
	vm_offset_t	size)
{
	vm_size_t	bytes;
	
	if (size > vm_external_max_size) {
		vm_external_nullify(result);
		return;
	}
	bytes = (atop(size) + 07) >> 3;
	if (bytes < KALLOC_MINSIZE) {
		bytes = KALLOC_MINSIZE;
	}
	result->existence_map = (char *) kalloc(bytes);
	result->existence_size = bytes;
	memset(result->existence_map, 0, bytes);
}

void
vm_external_destroy(
	vm_external_t	e)
{
	if (e->existence_map != VM_EXISTENCE_MAP_NULL) {
		kfree((vm_offset_t) e->existence_map,
		      (vm_size_t) e->existence_size);
		vm_external_nullify(e);
	}
}

void
vm_external_copy(
	vm_external_t	e1,
	vm_external_t	e2)
{
	assert(vm_external_null(e2));
	vm_external_create(e2, ptoa(e1->existence_size << 3));
	if (vm_external_null(e2)) {
		return;
	}
	assert(e1->existence_size == e2->existence_size);
	memcpy(e2->existence_map, e1->existence_map, e1->existence_size);
}

vm_external_state_t
_vm_external_state_get(
	vm_external_t	e,
	vm_offset_t	offset)
{
	unsigned
	int		bit, byte;

	assert(e->existence_map != VM_EXISTENCE_MAP_NULL);
	bit = atop(offset);
	byte = bit >> 3;
	if (byte >= e->existence_size) {
		return VM_EXTERNAL_STATE_ABSENT;
	} else if (e->existence_map[byte] & (1 << (bit & 07))) {
		return VM_EXTERNAL_STATE_EXISTS;
	} else {
		return VM_EXTERNAL_STATE_ABSENT;
	}
}

void
vm_external_state_set(
	vm_external_t	e,
	vm_offset_t	offset)
{
	unsigned
	int		bit, byte;

	if (e->existence_map == VM_EXISTENCE_MAP_NULL)
		return;

	bit = atop(offset);
	byte = bit >> 3;
	assert(byte < e->existence_size);
	e->existence_map[byte] |= (1 << (bit & 07));
}

void
vm_external_module_initialize(void)
{
}

