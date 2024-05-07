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
 * Revision 2.4  91/05/14  17:51:11  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  18:00:18  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:34:35  mrt]
 * 
 * Revision 2.2  90/06/02  15:12:02  rpd
 * 	Added declaration of vm_pageout_setup.
 * 	[90/05/31            rpd]
 * 
 * 	Changes for MACH_KERNEL:
 * 	. Remove non-XP declarations.
 * 	[89/04/28            dbg]
 * 
 * Revision 2.1  89/08/03  16:45:59  rwd
 * Created.
 * 
 * Revision 2.9  89/04/18  21:28:02  mwyoung
 * 	No relevant history.
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
/*
 *	File:	vm/vm_pageout.h
 *	Author:	Avadis Tevanian, Jr.
 *	Date:	1986
 *
 *	Declarations for the pageout daemon interface.
 */

#ifndef	_VM_VM_PAGEOUT_H_
#define _VM_VM_PAGEOUT_H_

#include <norma_vm.h>
#include <norma_ipc.h>

#include <mach/boolean.h>
#include <mach/machine/vm_types.h>
#include <vm/vm_object.h>
#include <vm/vm_page.h>

/*
 *	The following ifdef only exists because XMM must (currently)
 *	be given a page at a time.  This should be removed
 *	in the future.
 */
#if	NORMA_VM
#define	DATA_WRITE_MAX	1
#define	POINTER_T(copy)	(copy)
#else	/* NORMA_VM */
#define	DATA_WRITE_MAX	32
#define	POINTER_T(copy)	(pointer_t)(copy)
#endif	/* NORMA_VM */

#define PAGEOUT_PAGES(object, new_object, new_offset, action, po)	\
MACRO_BEGIN								\
									\
	vm_map_copy_t		copy;					\
	register int		i;                                      \
	register vm_page_t	hp;					\
									\
	vm_object_unlock(object);					\
									\
	assert(new_offset > 0);						\
        assert(new_offset % page_size == 0);				\
	(void) vm_map_copyin_object(new_object, 0, new_offset, &copy);	\
									\
	(void) memory_object_data_return(				\
			object->pager,					\
			object->pager_request,				\
			po,						\
			POINTER_T(copy),				\
			new_offset,					\
		(action == MEMORY_OBJECT_LOCK_RESULT_MUST_CLEAN),	\
			!should_flush);                                 \
									\
	vm_object_lock(object);						\
									\
	for (i = 0; i < atop(new_offset); i++) {			\
		hp = holding_pages[i];					\
		if (hp != VM_PAGE_NULL)					\
			VM_PAGE_FREE(hp);				\
	}								\
									\
        new_object = VM_OBJECT_NULL;					\
MACRO_END

/*
 *	Exported routines.
 */
extern void		vm_pageout(void);

extern vm_page_t	vm_pageout_setup(
					vm_page_t	m,
					vm_offset_t	paging_offset,
					vm_object_t	new_object,
					vm_offset_t	new_offset,
					boolean_t	flush);

extern void		vm_pageout_page(
					vm_page_t	m,
					boolean_t	initial,
					boolean_t	flush);

#if	NORMA_VM
/*
 *	In NORMA systems, it is necessary to throttle
 *	pageouts between nodes.  Moreover, lock_request, synchronize
 *	and initialize activities also falls in this category.
 */
#define	PAGEOUT_RESERVE_INTERNAL	(0)
#define	PAGEOUT_RESERVE_EXTERNAL	(1)
#define	PAGEOUT_RESERVE_LOCAL		(2)
#define	PAGEOUT_RESERVE_LOCK_REQUEST	(3)
#define	PAGEOUT_RESERVE_SYNCHRONIZE	(4)
#define	PAGEOUT_RESERVE_MAX		(5)

extern boolean_t	vm_pageout_reserve(
					vm_object_t	object,
					vm_offset_t	offset,
					vm_size_t	size,
					int		type);

extern void		vm_pageout_completed(
					vm_object_t	object,
					vm_offset_t	offset,
					vm_size_t	size);

extern void		vm_pageout_await_completed(
					int		type);

extern void		vm_pageout_reserve_terminate(
					vm_object_t	object);

#endif	/* NORMA_VM */

#if	NORMA_IPC
extern boolean_t	vm_netipc_able_continue_recv(void);
#endif	/* NORMA_IPC */

#endif	/* _VM_VM_PAGEOUT_H_ */
