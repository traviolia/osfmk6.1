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
 *	Joseph S. Barrera III 1991
 *	Global definitions for svm module.
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1992 Carnegie Mellon University
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

#include <mach_pagemap.h>
#include <mach_assert.h>

#include <norma/xmm_obj.h>
#include <mach/vm_param.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_space.h>
#include <kern/macro_help.h>
#include <kern/lock.h>

#include <norma/svm_state.h>
#include <norma/svm_temporary.h>
#include <norma/svm_request.h>
#include <norma/svm_copy.h>
#include <norma/svm_change.h>
#include <norma/svm_pagecopy.h>

#define	MOBJ_STATE_UNCALLED		0
#define	MOBJ_STATE_CALLED		1
#define	MOBJ_STATE_READY		2
#define MOBJ_STATE_SHOULD_TERMINATE	3
#define	MOBJ_STATE_TERMINATED		4

#define	MM(xobj)		((struct mobj *) xobj)
#define	KK(xobj)		((struct kobj *) xobj)

#define	K			KK(k)
#define	READER			KK(reader)
#define	WRITER			KK(writer)
#define	COPY			MM(copy)
#define	NEW_COPY		MM(new_copy)
#define	OLD_COPY		MM(old_copy)
#define	SHADOW			MM(shadow)

extern struct xmm_class		ksvm_class;
extern struct xmm_class		msvm_class;

/*
 * lock is set when pager gives us a message.
 * prot is set when we send message to kernels;
 * it should simply reflect max of all corresponding kobj prot.
 */
struct mobj {
	struct xmm_obj	obj;
	xmm_obj_t	shadow;
	xmm_obj_t	copy;
	xmm_obj_t	kobj_list;
	xmm_obj_t	terminate_mobj;
	unsigned int	num_pages;
	queue_head_t	request_list;
	unsigned int	request_count;
	request_t	last_found;		/* lookup optimization */
	struct svm_state bits;			/* kernel pages access state */
	ipc_port_t	memory_object;		/* for xmm_object_release */
	ipc_port_t	memory_object_name;	/* at most one send right */
	change_t	change;
#if	MACH_PAGEMAP
	struct vm_external pagemap;		/* non-zero implies frozen */
#endif	/* MACH_PAGEMAP */
	vm_size_t	cluster_size;
	int		lock_batching;		/* lock coalescing counter */
	short		kobj_count;
	short		k_count;		/* outstanding kernel reqs. */
	short		wc_pending;		/* pending write completeds */
	unsigned int
	/* enum */	state:3,
	/* boolean_t */	may_cache:1,
	/* enum */	copy_strategy:3,
	/* boolean_t */	temporary:1,
	/* boolean_t */	dirty_copy:1,
	/* boolean_t */	destroyed:1,
	/* boolean_t */	copy_in_progress:1,
	/* boolean_t */	copy_wanted:1,
	/* boolean_t */ ready_wanted:1,
	/* boolean_t */ disable_in_progress:1,
	/* boolean_t */	modwc:1,		/* expect write_completeds */
	/* boolean_t */	temporary_disabled:1,	/* only valid when obj is ready
						 * and object is temporary */
	/* boolean_t */ term_sent:1,		/* sent m_o_terminate */
	/* boolean_t */ destroy_needed:1,
	/* boolean_t */	extend_in_progress:1,
	/* boolean_t */	extend_wanted:1;
	queue_head_t	sync_requests;	/* memory_object_sync reqs */
	spinlock_t	mp_lock;	/* request list simple lock */
};

#define	svm_copy_event(mobj)	(((int) mobj) + 1)
#define	svm_disable_event(mobj)	(((int) mobj) + 2)
#define	svm_ready_event(mobj)	(((int) mobj) + 2)
#define	svm_extend_event(mobj)	(((int) mobj) + 3)

#define	xmm_list_lock_init(mobj)	spinlock_init(&MM(mobj)->mp_lock)
#define	xmm_list_lock(mobj)		spinlock_lock(&MM(mobj)->mp_lock)
#define	xmm_list_unlock(mobj)		spinlock_unlock(&MM(mobj)->mp_lock)

struct kobj {
	struct xmm_obj	obj;
	struct svm_state bits;			/* kernel pages status */
	xmm_obj_t	mobj;			/* back pointer */
	xmm_obj_t	next;
	gather_t	lock_gather;		/* lock_req coalesce struct */
	short		k_count;
	short		wc_pending;		/* pending write completeds */
	unsigned
	/* boolean_t */	initialized:1,		/* Ok to launch ops? */
	/* boolean_t */	active:1,		/* mobj ready, kobj !cached */
#if	MACH_PAGEMAP
	/* boolean_t */	has_pagemap:1,
#endif	/* MACH_PAGEMAP */
#if	COPY_CALL_PUSH_PAGE_TO_KERNEL
	/* boolean_t */	local:1,		/* local kernel object */
#endif	/* COPY_CALL_PUSH_PAGE_TO_KERNEL */
	/* boolean_t */	readied:1,		/* has been sent set_ready */
	/* boolean_t */	needs_terminate:1,	/* terminate pending */
	/* boolean_t */	terminated:1;		/* has been terminated */
};
