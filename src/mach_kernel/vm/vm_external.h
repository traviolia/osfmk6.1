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
 * Revision 2.5  91/05/14  17:48:31  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:57:53  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:31:29  mrt]
 * 
 * Revision 2.3  90/05/29  18:38:36  rwd
 * 	Picked up rfr changes.
 * 	[90/04/12  13:46:56  rwd]
 * 
 * Revision 2.2  90/01/11  11:47:31  dbg
 * 	Added changes from mainline:
 * 		Remember bitmap size, for safer deallocation.
 * 		[89/10/16  15:32:58  af]
 * 		Declare vm_external_destroy().
 * 		[89/08/08            mwyoung]
 * 
 * Revision 2.1  89/08/03  16:44:46  rwd
 * Created.
 * 
 * Revision 2.3  89/04/18  21:24:53  mwyoung
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

#ifndef	_VM_VM_EXTERNAL_H_
#define _VM_VM_EXTERNAL_H_

#include <mach_pagemap.h>

#if	MACH_PAGEMAP

/*
 *	External page management hint technology
 *
 *	The data structure exported by this module maintains
 *	a (potentially incomplete) map of the pages written
 *	to external storage for a range of virtual memory.
 */

/*
 *	The data structure representing the state of pages
 *	on external storage.
 */

typedef struct vm_external {
    	vm_size_t	existence_size;	/* Size of the following bitmap */
	char		*existence_map;	/* A bitmap of pages that have been
					 * written to backing storage.
					 */
} *vm_external_t;

#define	VM_EXISTENCE_MAP_NULL	((char *) 0)

#define	vm_external_nullify(e)	((e)->existence_map = VM_EXISTENCE_MAP_NULL)
#define	vm_external_null(e)	((e)->existence_map == VM_EXISTENCE_MAP_NULL)

/*
 *	The states that may be recorded for a page of external storage.
 */

typedef int	vm_external_state_t;
#define	VM_EXTERNAL_STATE_EXISTS		1
#define	VM_EXTERNAL_STATE_UNKNOWN		2
#define	VM_EXTERNAL_STATE_ABSENT		3


/*
 *	Routines exported by this module.
 */

extern void			vm_external_module_initialize(void);
						/* Initialize the module */

extern void			vm_external_create(
						/* Create a vm_external_t */
						vm_external_t	result,
						vm_offset_t	size);

extern void			vm_external_destroy(
						/* Destroy one */
						vm_external_t	e);

extern void			vm_external_copy(
						/* Copy one into another */
						vm_external_t	e1,
						vm_external_t	e2);

extern void			vm_external_state_set(	
						/* Set state of a page to
						 * VM_EXTERNAL_STATE_EXISTS. */
						vm_external_t	e,
						vm_offset_t	offset);

#define	vm_external_state_get(e,offset)	((! vm_external_null(e)) ? \
					  _vm_external_state_get(e, offset) : \
					  VM_EXTERNAL_STATE_UNKNOWN)
						/* Retrieve the state for a
						 * given page, if known.  */

extern vm_external_state_t	_vm_external_state_get(
						/* HIDDEN routine */
						vm_external_t	e,
						vm_offset_t	offset);

#endif	/* MACH_PAGEMAP */
#endif	/* _VM_VM_EXTERNAL_H_ */
