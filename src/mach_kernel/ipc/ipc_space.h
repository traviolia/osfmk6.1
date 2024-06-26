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
 * Revision 2.7  91/06/17  15:46:29  jsb
 * 	Renamed NORMA conditionals.
 * 	[91/06/17  10:45:36  jsb]
 * 
 * Revision 2.6  91/05/14  16:36:57  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/03/16  14:48:45  rpd
 * 	Added is_growing.
 * 	[91/03/04            rpd]
 * 
 * Revision 2.4  91/02/05  17:23:48  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  15:51:32  mrt]
 * 
 * Revision 2.3  90/09/28  16:55:23  jsb
 * 	Added NORMA_IPC support.
 * 	[90/09/28  14:04:12  jsb]
 * 
 * Revision 2.2  90/06/02  14:51:43  rpd
 * 	Created for new IPC.
 * 	[90/03/26  21:03:27  rpd]
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
 *	File:	ipc/ipc_space.h
 *	Author:	Rich Draves
 *	Date:	1989
 *
 *	Definitions for IPC spaces of capabilities.
 */

#ifndef	_IPC_IPC_SPACE_H_
#define _IPC_IPC_SPACE_H_

#include <norma_ipc.h>
#include <mach_kdb.h>

#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <kern/macro_help.h>
#include <kern/lock.h>
#include <kern/zalloc.h>
#include <ipc/ipc_entry.h>
#include <ipc/ipc_splay.h>
#include <ipc/ipc_types.h>

/*
 *	Every task has a space of IPC capabilities.
 *	IPC operations like send and receive use this space.
 *	IPC kernel calls manipulate the space of the target task.
 *
 *	Every space has a non-NULL is_table with is_table_size entries.
 *	A space may have a NULL is_tree.  is_tree_small records the
 *	number of entries in the tree that, if the table were to grow
 *	to the next larger size, would move from the tree to the table.
 *
 *	is_growing marks when the table is in the process of growing.
 *	When the table is growing, it can't be freed or grown by another
 *	thread, because of krealloc/kmem_realloc's requirements.
 *
 */

typedef unsigned int ipc_space_refs_t;

struct ipc_space {
	mutex_t	is_ref_lock_data;
	ipc_space_refs_t is_references;

	mutex_t	is_lock_data;
	boolean_t is_active;		/* is the space alive? */
	boolean_t is_growing;		/* is the space growing? */
	ipc_entry_t is_table;		/* an array of entries */
	ipc_entry_num_t is_table_size;	/* current size of table */
	struct ipc_table_size *is_table_next; /* info for larger table */
	struct ipc_splay_tree is_tree;	/* a splay tree of entries */
	ipc_entry_num_t is_tree_total;	/* number of entries in the tree */
	ipc_entry_num_t is_tree_small;	/* # of small entries in the tree */
	ipc_entry_num_t is_tree_hash;	/* # of hashed entries in the tree */
	boolean_t is_fast;              /* for is_fast_space() */
};

#define	IS_NULL			((ipc_space_t) 0)

extern zone_t ipc_space_zone;

#define is_alloc()		((ipc_space_t) zalloc(ipc_space_zone))
#define	is_free(is)		zfree(ipc_space_zone, (vm_offset_t) (is))

extern ipc_space_t ipc_space_kernel;
extern ipc_space_t ipc_space_reply;
#if	NORMA_IPC
extern ipc_space_t ipc_space_remote;
#endif	/* NORMA_IPC */
#if	NORMA_IPC || MACH_KDB
extern ipc_space_t default_pager_space;
#endif	/* NORMA_IPC || MACH_KDB */

#define is_fast_space(is)	((is)->is_fast)

#define	is_ref_lock_init(is)	mutex_init(&(is)->is_ref_lock_data)

#define	ipc_space_reference_macro(is)					\
MACRO_BEGIN								\
	mutex_lock(&(is)->is_ref_lock_data);				\
	assert((is)->is_references > 0);				\
	(is)->is_references++;						\
	mutex_unlock(&(is)->is_ref_lock_data);				\
MACRO_END

#define	ipc_space_release_macro(is)					\
MACRO_BEGIN								\
	ipc_space_refs_t _refs;						\
									\
	mutex_lock(&(is)->is_ref_lock_data);				\
	assert((is)->is_references > 0);				\
	_refs = --(is)->is_references;					\
	mutex_unlock(&(is)->is_ref_lock_data);				\
									\
	if (_refs == 0)							\
		is_free(is);						\
MACRO_END

#define	is_lock_init(is)	mutex_init(&(is)->is_lock_data)

#define	is_read_lock(is)	mutex_lock(&(is)->is_lock_data)
#define is_read_unlock(is)	mutex_unlock(&(is)->is_lock_data)

#define	is_write_lock(is)	mutex_lock(&(is)->is_lock_data)
#define	is_write_lock_try(is)	mutex_try(&(is)->is_lock_data)
#define is_write_unlock(is)	mutex_unlock(&(is)->is_lock_data)

#define	is_write_to_read_lock(is)

/* Take a reference on a space */
extern void ipc_space_reference(
	ipc_space_t	space);

/* Realase a reference on a space */
extern void ipc_space_release(
	ipc_space_t	space);

#define	is_reference(is)	ipc_space_reference(is)
#define	is_release(is)		ipc_space_release(is)

/* Create  new IPC space */
extern kern_return_t ipc_space_create(
	ipc_table_size_t	initial,
	ipc_space_t		*spacep);

/* Create a special IPC space */
extern kern_return_t ipc_space_create_special(
	ipc_space_t	*spacep);

/* Mark a space as dead and cleans up the entries*/
extern void ipc_space_destroy(
	ipc_space_t	space);

#endif	/* _IPC_IPC_SPACE_H_ */
