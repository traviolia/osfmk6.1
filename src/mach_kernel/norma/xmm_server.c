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
 * Revision 2.5.1.6  92/06/24  18:03:11  jeffreyh
 * 	Create ksvm object for local XMM stacks.
 * 	[92/06/24            dlb]
 * 
 * 	Changed routines to meet new interface.
 * 	[92/06/18            sjs]
 * 
 * 	Change boolean change_in_progress to count of pending_replies
 * 	and use in lock_request/lock_completed.  Add release logic
 * 	to lock_completed.
 * 	[92/06/09            dlb]
 * 
 * 	Handle null mobj in memory_object_data_* routines; return
 * 	KERN_FAILURE and caller will clean up.
 * 	Call xmm_object_internal_mobj_create for internal objects
 * 	instead of creating port xmm_object's for them.
 * 	[92/06/08            dlb]
 * 
 * 	Change use_old_pageout boolean to meaningful values.  Implement
 * 	memory_object_data_initialize.
 * 	[92/06/04            dlb]
 * 
 * Revision 2.5.1.5  92/03/28  10:13:20  jeffreyh
 * 	Changed data_write to data_write_return and deleted data_return
 * 	 method.  Added state information to mobj to prevent terminate
 * 	 from destroying an object that is in process by
 * 	 change_completed; support for m_o_change_attributes.
 * 	[92/03/20            sjs]
 * 
 * Revision 2.5.1.4  92/03/03  16:24:06  jeffreyh
 * 	Pick up fix from dlb to add missing vm_object_dealocate to the
 * 	 object->internal case of k_server_set_ready().
 * 	[92/02/29            jeffreyh]
 * 
 * Revision 2.5.1.3  92/02/21  11:28:01  jsb
 * 	Release send right to memory object in memory_object_terminate, now
 * 	that the xmm_user layer keeps a separate send right.
 * 	[92/02/20  14:02:57  jsb]
 * 
 * 	Explicitly provide name parameter to xmm_decl macro.
 * 	Changed termination for new reference counting implementation.
 * 	[92/02/16  15:53:26  jsb]
 * 
 * 	In memory_object_terminate, don't release_send memory_object_name
 * 	if it is null. Do call xmm_object_by_memory_object_release.
 * 	[92/02/11  18:23:15  jsb]
 * 
 * 	Changed xmm_memory_object_init to use xmm_object_by_memory_object
 * 	instead of xmm_lookup. Removed xmm_lookup.
 * 	[92/02/10  17:02:39  jsb]
 * 
 * 	Instead of holding a vm_object pointer, always do a vm_object_lookup
 * 	on pager to obtain vm_object. This allows us to notice when vm_object.c
 * 	has removed (as in vm_object_terminate) or changed (vm_object_collapse)
 * 	the port to object associations. Removed now unneeded xmm_object_set.
 * 	Declare second parameter of memory_object_* calls as xmm_obj_t, thanks
 * 	to new pager_request_t declaration of object->pager_request.
 * 	[92/02/10  09:47:16  jsb]
 * 
 * 	Use new xmm_decl, and new memory_object_name and deallocation protocol.
 * 	Removed svm exceptions; this is now handled by xmm_vm_object_lookup.
 * 	Changed xmm_lookup to not use memory_object kobject to hold
 * 	both mobj and vm_object; we now use memory_object->ip_norma_xmm_object
 * 	which is migrated upon memory_object port migration.
 * 	Don't defined memory_object_{init,create}; instead, vm/vm_object.c
 * 	calls new routine xmm_memory_object_init routine which passes
 * 	internal and size parameters down the xmm layers.
 * 	[92/02/09  13:54:41  jsb]
 * 
 * Revision 2.5.1.2  92/01/21  21:54:46  jsb
 * 	De-linted. Supports new (dlb) memory object routines.
 * 	Supports arbitrary reply ports to lock_request, etc.
 * 	Converted mach_port_t (and port_t) to ipc_port_t.
 * 	[92/01/20  17:28:58  jsb]
 * 
 * Revision 2.5.1.1  92/01/03  17:13:19  jsb
 * 	MACH_PORT_NULL -> IP_NULL.
 * 
 * Revision 2.5  91/08/28  11:16:24  jsb
 * 	Added temporary definition for memory_object_change_completed.
 * 	[91/08/16  14:21:20  jsb]
 * 
 * 	Added comment to xmm_lookup about read-only pagers.
 * 	[91/08/15  10:12:12  jsb]
 * 
 * Revision 2.4  91/08/03  18:19:40  jsb
 * 	Changed mach_port_t to ipc_port_t whereever appropriate.
 * 	[91/07/17  14:07:08  jsb]
 * 
 * Revision 2.3  91/07/01  08:26:29  jsb
 * 	Added support for memory_object_create.
 * 	Now export normal memory_object_init with standard arguments.
 * 	Improved object initialization logic.
 * 	Added garbage collection.
 * 	[91/06/29  15:39:01  jsb]
 * 
 * Revision 2.2  91/06/17  15:48:33  jsb
 * 	First checkin.
 * 	[91/06/17  11:05:10  jsb]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991, 1992 Carnegie Mellon University
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
/*
 *	File:	norma/xmm_server.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Interface between kernel and xmm system.
 */

#include <mor.h>

#include <ipc/ipc_space.h>
#include <ipc/ipc_port.h>
#include <vm/memory_object.h>
#include <vm/vm_fault.h>
#include <vm/vm_map.h>
#include <vm/vm_object.h>
#include <vm/vm_page.h>
#include <vm/vm_pageout.h>
#include <kern/queue.h>
#include <kern/host.h>
#include <kern/ipc_kobject.h>
#include <kern/lock.h>
#include <kern/misc_protos.h>
#include <machine/machparam.h>
#include <norma/xmm_obj.h>
#include <norma/xmm_methods.h>
#include <norma/svm_copy.h> /* XXX only for COPY_CALL_PUSH_PAGE_TO_KERNEL */
#include <norma/xmm_server_rename.h>

#define	XMM_OBJECT_CHECKS	MACH_ASSERT

/*
 * Forward.
 */
#if	XMM_OBJECT_CHECKS
void		object_check(
			xmm_obj_t	mobj);

void		xmm_server_object_check(
			vm_object_t	object);
#else	/* XMM_OBJECT_CHECKS */
#define	object_check(obj)
#define	xmm_server_object_check(obj)
#endif	/* XMM_OBJECT_CHECKS */

typedef struct mobj *mobj_t;

#define	SYNCHRONOUS_TERMINATION	0

struct mobj {
	struct xmm_obj	obj;
	ipc_port_t	pager;
	int		pending_replies;/* Replies expected to operations
					   in progress. */
	unsigned
	/* boolean_t */	lock_request:1,	/* indicate waiting lock_request */
 	/* boolean_t */	terminating:1,	/* indicate mobj is terminating */
 	/* boolean_t */	caching:1,	/* pending caching msg to svm */
	/* boolean_t */	modwc:1,	/* write_completions indicator */
	/* small int */	caching_age:2;	/* ticks since cached */
	queue_chain_t	caching_list;	/* list of all cached mobjs */
	int		p_count;	/* XXX count of pending WCs */
	int		p_sync;		/* XXX count of pending SCs */
#if	PENDING_TRACKING
	int		p_seen;		/* data_writes on the way out */
	int		p_matched;	/* completeds on the way in */
	int		p_terminated;	/* lossage due to termination */
#endif
};

/*
 * set ready stores the reply in the server object, calls set
 * attributes_common - if the reply is still in the mobj,
 * then set ready may have to do the completion
 */

#undef  KOBJ
#define KOBJ    ((struct mobj *) kobj)

#define	m_server_deallocate		m_interpose_deallocate
#define	m_server_init			m_invalid_init
#define	m_server_terminate		m_invalid_terminate
#define	m_server_copy			m_invalid_copy
#define	m_server_data_request		m_invalid_data_request
#define	m_server_data_unlock		m_invalid_data_unlock
#define	m_server_data_return		m_invalid_data_return
#define	m_server_lock_completed		m_invalid_lock_completed
#define	m_server_supply_completed	m_invalid_supply_completed
#define	m_server_change_completed	m_invalid_change_completed
#define	m_server_synchronize		m_invalid_synchronize
#define	m_server_freeze			m_invalid_freeze
#define	m_server_share			m_invalid_share
#define	m_server_declare_page		m_invalid_declare_page
#define	m_server_declare_pages		m_invalid_declare_pages
#define	m_server_caching		m_invalid_caching
#define	m_server_uncaching		m_invalid_uncaching
#define	m_server_debug			m_invalid_debug

xmm_decl(server, "server", sizeof(struct mobj));

queue_head_t xmm_server_caching_list;
spinlock_t xmm_server_caching_slock;

#define xmm_queue_lock_init()	spinlock_init(&xmm_server_caching_slock)
#define xmm_queue_lock()	spinlock_lock(&xmm_server_caching_slock)
#define xmm_queue_unlock()	spinlock_unlock(&xmm_server_caching_slock)

int xmm_server_lazy_enable = 1;
int xmm_server_queue_version = 0;
int xmm_server_timeout_count = 0;
int xmm_server_timeout_multiple = 5;

kern_return_t
k_server_data_unavailable(
	xmm_obj_t	kobj,
	vm_offset_t	offset,
	vm_size_t	length)
{
#ifdef	lint
	(void) K_DATA_UNAVAILABLE(kobj, offset, length);
#endif	/* lint */

#if	MACH_ASSERT
	xmm_obj_lock(kobj);
	assert(!KOBJ->terminating);
	xmm_obj_unlock(kobj);
#endif	/* MACH_ASSERT */

	return memory_object_data_unavailable(vm_object_lookup(KOBJ->pager),
					      offset, length);
}

kern_return_t
k_server_get_attributes(
	xmm_obj_t			kobj,
	memory_object_flavor_t		flavor,
	memory_object_info_t		attributes,
	unsigned			*count)
{
#ifdef	lint
	(void) K_GET_ATTRIBUTES(kobj, flavor, attributes, count);
#endif	/* lint */

	xmm_obj_lock(kobj);
	if (KOBJ->terminating) {
		xmm_obj_unlock(kobj);
		return KERN_INVALID_ARGUMENT;
	}
	xmm_obj_unlock(kobj);

	return memory_object_get_attributes(vm_object_lookup(KOBJ->pager),
					    flavor, attributes, count);
}

kern_return_t
k_server_lock_request(
	xmm_obj_t	kobj,
	vm_offset_t	offset,
	vm_size_t	length,
	boolean_t	should_clean,
	boolean_t	should_flush,
	vm_prot_t	lock_value,
	xmm_reply_t	reply)
{
	vm_object_t object;

#ifdef	lint
	(void) K_LOCK_REQUEST(kobj, offset, length, should_clean, should_flush,
			      lock_value, reply);
#endif	/* lint */

	xmm_obj_lock(kobj);
	if (KOBJ->terminating) {
		xmm_obj_unlock(kobj);
		if (reply != XMM_REPLY_NULL)
			M_LOCK_COMPLETED(kobj, offset, length, reply);
		return KERN_SUCCESS;
	}
	if ((object = vm_object_lookup(KOBJ->pager)) == VM_OBJECT_NULL) {
		/*
		 * There is a conflict between a m_o_lock_request issued by
		 * the pager and a m_o_data_return issued by a terminating
		 * vm_object. The terminating vm_object flushes all its
		 * resident pages to the pager. If the pager has in parallel
		 * issued a m_o_lock_request, then the m_o_lock_completed must
		 * be issued by the SERVER layer *ONLY* when the corresponding
		 * resident page (if any) has completed its m_o_data_return.
		 * This race condition has been solved by using a flag in the
		 * mobj structure in order to mark that a thread is waiting for
		 * sending a m_o_lock_completed. It will be delivered when the
		 * vm_object has returned all its pages, i.e. in m_o_terminate.
		 */
		if (reply != XMM_REPLY_NULL) {
			KOBJ->lock_request = TRUE;
			assert_wait((int) kobj, FALSE);
			xmm_obj_unlock(kobj);
			thread_block((void (*)(void)) 0);
			assert(KOBJ->terminating);
			M_LOCK_COMPLETED(kobj, offset, length, reply);
		} else
			xmm_obj_unlock(kobj);
		return KERN_SUCCESS;
	}

	if (reply != XMM_REPLY_NULL) {
		KOBJ->pending_replies++;
		assert(!xmm_reply_empty(reply));
	}
	xmm_obj_unlock(kobj);

	return memory_object_lock_request(object,
					  offset, length, should_clean,
					  should_flush, lock_value,
					  (ipc_port_t) reply,
					  MACH_MSG_TYPE_PORT_SEND_ONCE);
}

kern_return_t
k_server_data_error(
	xmm_obj_t	kobj,
	vm_offset_t	offset,
	vm_size_t	length,
	kern_return_t	error_value)
{
#ifdef	lint
	(void) K_DATA_ERROR(kobj, offset, length, error_value);
#endif	/* lint */

	xmm_obj_lock(kobj);
	if (KOBJ->terminating) {
		xmm_obj_unlock(kobj);
		return KERN_SUCCESS;
	}
	xmm_obj_unlock(kobj);

	return	memory_object_data_error(vm_object_lookup(KOBJ->pager),
					 offset, length, error_value);
}

kern_return_t
k_server_set_ready(
	xmm_obj_t			kobj,
	boolean_t			object_ready,
	boolean_t			may_cache,
        boolean_t			write_completions,
	memory_object_copy_strategy_t	copy_strategy,
        vm_size_t			cluster_size,
	ipc_port_t			memory_object_name,
	xmm_reply_t			reply,
	boolean_t			temporary,
	char				*existence_map,
	unsigned			existence_size)
{
	vm_object_t object;
	kern_return_t kr;
	boolean_t norequest;

#ifdef	lint
	(void) K_SET_READY(kobj, object_ready, may_cache, write_completions,
			   copy_strategy, cluster_size,
			   memory_object_name, reply, temporary, existence_map,
			   existence_size);
#endif	/* lint */

	xmm_obj_lock(kobj);
	KOBJ->modwc = write_completions;

	/*
	 * If the object no longer exists, or already has a send right
	 * for memory_object_name, then discard the send right;
	 * otherwise, store the send right in the object.
	 */
	if (KOBJ->terminating ||
	    (object = vm_object_lookup(KOBJ->pager)) == VM_OBJECT_NULL) {
		xmm_obj_unlock(kobj);
		ipc_port_release_send(memory_object_name);

		/* nothing more to do; send reply if requested and return. */
		if (reply != XMM_REPLY_NULL)
			(void) M_CHANGE_COMPLETED(kobj, may_cache,
						copy_strategy, reply);
		return KERN_SUCCESS;
	}
	xmm_obj_unlock(kobj);

	vm_object_lock(object);
	if (object->pager_name == IP_NULL) {
		object->pager_name = memory_object_name;
	} else {
		assert(object->pager_name == memory_object_name);
		ipc_port_release_send(memory_object_name);
	}

	/*
	 * Take a reference to protect object from being deleted.
	 */
	if (!may_cache)
		object->ref_count++;
	vm_object_unlock(object);
		
	/*
	 * Hold a pseudo-reference for this reply.
	 */
	if (reply != XMM_REPLY_NULL) {
		xmm_obj_lock(kobj);
		KOBJ->pending_replies++;
		assert(!xmm_reply_empty(reply));
		xmm_obj_unlock(kobj);
	}

	/*
	 * Call set_attributes_common.
	 */
	kr = memory_object_set_attributes_common(object, may_cache,
						 object_ready,
						 write_completions,
						 cluster_size, copy_strategy,
						 temporary, existence_map,
						 existence_size);

	/*
	 * If we were trying to uncache the object, but the object
	 * is no longer cachable, then destroy it. We cannot rely
	 * on vm_object_deallocate to handle the destruction because
	 * the thread waiting for the uncache response is holding
	 * a reference to the object.
	 */
	if (!may_cache) {
		vm_object_lock(object);
		assert(!object->can_persist);
		if (object->temporary_uncaching) {
			vm_object_unlock(object);
			memory_object_destroy(object, KERN_FAILURE);
		} else {
			vm_object_unlock(object);
			vm_object_deallocate(object);
		}
	}

	/*
	 * Send a reply if one was requested.
	 */
	if (reply != XMM_REPLY_NULL) {
		xmm_obj_lock(kobj);
                KOBJ->pending_replies--;
		xmm_obj_unlock(kobj);
		(void) M_CHANGE_COMPLETED(kobj, may_cache, copy_strategy,
					  reply);
	}
	return kr;
}

/*
 * terminate will have released our reference, so it is only necessary
 * to do an unlink to dispose of the kobj.
 */
kern_return_t
k_server_release_all(
	xmm_obj_t	kobj)
{
#ifdef	lint
	K_RELEASE(kobj);
#endif	/* lint */
	xmm_obj_unlink(kobj);

	return(KERN_SUCCESS);
}

kern_return_t
k_server_destroy(
	xmm_obj_t	kobj,
	kern_return_t	reason)
{
#ifdef	lint
	(void) K_DESTROY(kobj, reason);
#endif	/* lint */
	xmm_obj_lock(kobj);
	if (KOBJ->terminating) {
		xmm_obj_unlock(kobj);
		return KERN_SUCCESS;
	}
	xmm_obj_unlock(kobj);
	return memory_object_destroy(vm_object_lookup(KOBJ->pager), reason);
}

kern_return_t
k_server_data_supply(
	xmm_obj_t	kobj,
	vm_offset_t	offset,
	vm_map_copy_t	data,
	vm_size_t	length,
	vm_prot_t	lock_value,
	boolean_t	precious,
	xmm_reply_t	reply)
{
#ifdef	lint
	(void) K_DATA_SUPPLY(kobj, offset, data, length, lock_value, precious,
			     reply);
#endif	/* lint */
	xmm_obj_lock(kobj);
	if (KOBJ->terminating) {
		xmm_obj_unlock(kobj);
		vm_map_copy_discard(data);
		return KERN_SUCCESS;
	}
	xmm_obj_unlock(kobj);
	return	memory_object_data_supply(vm_object_lookup(KOBJ->pager),
					  offset, (vm_offset_t) data,
					  length, lock_value, precious,
					  (ipc_port_t) reply,
					  MACH_MSG_TYPE_PORT_SEND_ONCE);
}

unsigned int	c_kserver_data_write_completed = 0;
unsigned int	c_ksdwc_terminate = 0;
unsigned int	c_ksdwc_null_obj = 0;
unsigned int	c_ksdwc_fading_obj = 0;
unsigned int	c_ksdwc_found_obj = 0;

#if	MACH_ASSERT
#define	KSDWC_RECENT_HISTORY	2048
vm_object_t	ksdwc_recent_obj[KSDWC_RECENT_HISTORY];
int		ksdwc_recent_obj_index = 0;
#endif /*MACH_ASSERT*/

kern_return_t
k_server_data_write_completed(
	xmm_obj_t	kobj,
	vm_offset_t	offset,
	vm_size_t	size)
{
	vm_object_t object;

#ifdef	lint
	K_DATA_WRITE_COMPLETED(kobj, offset, size);
#endif	/* lint */
	++c_kserver_data_write_completed;

	xmm_obj_lock(kobj);
	assert(KOBJ->modwc == TRUE);
	if (KOBJ->terminating) {
		assert(KOBJ->p_count == 0);
		xmm_obj_unlock(kobj);
		++c_ksdwc_terminate;
		return KERN_SUCCESS;
	}
	assert(KOBJ->p_count > 0);
	KOBJ->p_count--;
#if	PENDING_TRACKING
	KOBJ->p_matched++;
#endif
	xmm_obj_unlock(kobj);

	if ((object = vm_object_lookup(KOBJ->pager)) == VM_OBJECT_NULL) {
		if ((object = vm_object_lookup_terminating(KOBJ->pager))
		    == VM_OBJECT_NULL) {
			++c_ksdwc_null_obj;
			printf("xmm_server warning:  reservation loss?\n");
			return KERN_SUCCESS;
		} else {
			/*
			 *	Special case:  the object is not available
			 *	through the normal paths.  At this time,
			 *	the object is NOT in the cache.
			 */
			assert(!object->alive);
			++c_ksdwc_fading_obj;
		}
	} else {
		assert(object->ref_count > 0);
		++c_ksdwc_found_obj;
	}
#if	MACH_ASSERT
	ksdwc_recent_obj[ksdwc_recent_obj_index++] = object;
	if (ksdwc_recent_obj_index >= KSDWC_RECENT_HISTORY)
		ksdwc_recent_obj_index = 0;
#endif
	return memory_object_data_write_completed(object, offset, size);
}

kern_return_t
k_server_create_copy(
	xmm_obj_t	kobj,
	ipc_port_t	new_copy_pager,
	kern_return_t	*result)
{
#ifdef	lint
	(void) K_CREATE_COPY(kobj, new_copy_pager, result);
#endif	/* lint */
	xmm_obj_lock(kobj);
	if (KOBJ->terminating) {
		xmm_obj_unlock(kobj);
		if (IP_VALID(new_copy_pager))
			ipc_port_release_send(new_copy_pager);
		*result = KERN_INVALID_ARGUMENT;
		return KERN_SUCCESS;
	}
	xmm_obj_unlock(kobj);
	return memory_object_create_copy(vm_object_lookup(KOBJ->pager),
					 new_copy_pager, result);
}

kern_return_t
k_server_uncaching_permitted(
	xmm_obj_t	kobj)
{
#ifdef	lint
	(void) K_UNCACHING_PERMITTED(kobj);
#endif	/* lint */

#if	MACH_ASSERT
	xmm_obj_lock(kobj);
	assert(!KOBJ->terminating);
	xmm_obj_unlock(kobj);
#endif	/* MACH_ASSERT */

	return memory_object_uncaching_permitted(vm_object_lookup(KOBJ->pager));
}

kern_return_t
k_server_synchronize_completed(
	xmm_obj_t	kobj,
	vm_offset_t	offset,
	vm_size_t	size)
{
	kern_return_t kr;

#ifdef	lint
	K_SYNCHRONIZE_COMPLETED(kobj, offset, size);
#endif	/* lint */

	xmm_obj_lock(kobj);
	assert(!KOBJ->terminating);
	assert(KOBJ->p_sync > 0);
	KOBJ->p_sync--;
	xmm_obj_unlock(kobj);

	kr = memory_object_synchronize_completed(vm_object_lookup(KOBJ->pager),
						 offset, size);
	assert(kr == KERN_SUCCESS);
	return kr;
}

void
xmm_memory_object_init(
	vm_object_t	object)
{
	ipc_port_t xmm_object;
	xmm_obj_t mobj;

	/*
	 * Find or create xmm_object corresponding to memory_object.
	 * Once created, the xmm_object for a memory_object remains
	 * the same until the object is terminated by all kernels.
	 */
	xmm_object = xmm_object_by_memory_object(object->pager);
	assert(xmm_object != IP_NULL);	/* XXX */

	/*
	 * If xmm_object is local, then so is the svm stack, which will
	 * be stored as xmm_object's kobject, and we need a local
	 * ksvm mobj. Otherwise, we need to create an import mobj.  
	 */
	if (IP_NORMA_IS_PROXY(xmm_object)) {
		xmm_import_create(xmm_object, &mobj);
	} else {
		assert(ip_kotype(xmm_object) == IKOT_XMM_OBJECT);
		mobj = (xmm_obj_t) xmm_object->ip_kobject;
		ipc_port_release_send(xmm_object);
#if	COPY_CALL_PUSH_PAGE_TO_KERNEL
		xmm_ksvm_create(mobj, &mobj, TRUE);
#else	/* COPY_CALL_PUSH_PAGE_TO_KERNEL */
		xmm_ksvm_create(mobj, &mobj);
#endif	/* COPY_CALL_PUSH_PAGE_TO_KERNEL */
	}

	/*
	 * Create a server layer on top.
	 */
	xmm_obj_allocate(&server_class, mobj, &mobj);

	/*
	 * Associate server mobj with vm object, and pager with mobj.
	 *
	 * The reason that we don't store the vm object directly in the
	 * mobj is that the vm object can change, for example as a result
	 * of vm_object_collapse.
	 */
	MOBJ->pager = object->pager;
	MOBJ->pending_replies = 0;
	MOBJ->lock_request = FALSE;
	MOBJ->terminating = FALSE;
	MOBJ->caching = FALSE;
	MOBJ->caching_age = 0;
	object->pager_request = mobj;
	MOBJ->modwc = object->pager_modwc;
	MOBJ->p_count = 0;
#if	PENDING_TRACKING
	MOBJ->p_seen = 0;
	MOBJ->p_matched = 0;
	MOBJ->p_terminated = 0;
#endif

	/*
	 * Intialize the mobj for this kernel.
	 */
	(void) M_INIT(mobj, PAGE_SIZE, (boolean_t) object->internal,
		      object->size);
}

kern_return_t
memory_object_terminate(
	ipc_port_t	memory_object,
	xmm_obj_t	mobj,
	ipc_port_t	memory_object_name)
{
	/*
	 * Check that no simple lock is held.
	 */
	/* check_simple_locks(); */

	xmm_obj_lock(mobj);
	if (MOBJ->modwc) {
#if	PENDING_TRACKING
	    MOBJ->p_terminated += MOBJ->p_count;
#endif
	    MOBJ->p_count = 0;
	}
	MOBJ->terminating = TRUE;
	assert(MOBJ->p_sync == 0);

#if	!SYNCHRONOUS_TERMINATION
	/*
	 *	When synchronous termination is enabled, the
	 *	object either shouldn't have paging flow
	 *	control enabled on it or all pending pageouts
	 *	should have completed.
	 *
	 *	When synchronous termination is disabled,
	 *	the object termination process is semi-synchronous.
	 *	All but the last few pageouts will be handled
	 *	by paging flow control.  The last few pageouts
	 *	have their reservations cancelled, and so are
	 *	not guaranteed to complete by this time.
	 */
#else
	assert(MOBJ->modwc == FALSE || MOBJ->p_count == 0);
	if (MOBJ->p_count > 0)
		panic("memory_object_terminate:  outstanding pageouts");
#endif
	/*
	 * Wakeup any pending m_o_lock_completed.
	 */
	if (MOBJ->lock_request)
		thread_wakeup((int) mobj);

	/*
	 * Hold onto our reference; let k_server_release dispose of it.
	 */
	xmm_obj_release(mobj);	/* consumes lock */

	if (memory_object != IP_NULL) {
		ipc_port_release_send(memory_object);
	}
	if (memory_object_name != IP_NULL) {
		ipc_port_release_send(memory_object_name);
	}
	xmm_queue_lock();
	if (MOBJ->caching) {
		queue_remove(&xmm_server_caching_list, MOBJ, mobj_t,
			     caching_list);
		MOBJ->caching = FALSE;
		xmm_server_queue_version++;
	}
	xmm_queue_unlock();

	return M_TERMINATE(mobj, FALSE);
}

kern_return_t
memory_object_copy(
	ipc_port_t	memory_object,
	xmm_obj_t	mobj)
{
#ifdef	lint
	memory_object++;
#endif	/* lint */

	/*
	 * Check that no simple lock is held.
	 */
	/* check_simple_locks(); */

	if (mobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}
	return M_COPY(mobj);
}

kern_return_t
memory_object_data_request(
	ipc_port_t	memory_object,
	xmm_obj_t	mobj,
	vm_offset_t	offset,
	vm_size_t	length,
	vm_prot_t	desired_access)
{
#ifdef	lint
	memory_object++;
#endif	/* lint */

	/*
	 * Check that no simple lock is held.
	 */
	/* check_simple_locks(); */

	if (mobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}
	return M_DATA_REQUEST(mobj, offset, length, desired_access);
}

kern_return_t
memory_object_data_unlock(
	ipc_port_t	memory_object,
	xmm_obj_t	mobj,
	vm_offset_t	offset,
	vm_size_t	length,
	vm_prot_t	desired_access)
{
#ifdef	lint
	memory_object++;
#endif	/* lint */

	/*
	 * Check that no simple lock is held.
	 */
	/* check_simple_locks(); */

	if (mobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}
	return M_DATA_UNLOCK(mobj, offset, length, desired_access);
}

kern_return_t
memory_object_data_write(
	ipc_port_t	memory_object,
	xmm_obj_t	mobj,
	vm_offset_t	offset,
	vm_map_copy_t	data,
	vm_size_t	length)
{
#ifdef	lint
	memory_object++;
#endif	/* lint */
	/*
	 * Since use_old_pageout is always false, this should never be called.
	 */
	panic("memory_object_data_write");
	return KERN_SUCCESS;
}

kern_return_t
memory_object_data_initialize(
	ipc_port_t	memory_object,
	xmm_obj_t	mobj,
	vm_offset_t	offset,
	vm_map_copy_t	data,
	vm_size_t	length)
{
#ifdef	lint
	memory_object++;
#endif	/* lint */
	/*
	 * Since copy_delay is always mapped by svm layer to copy_call,
	 * this should never be called.
	 */
	panic("memory_object_data_initialize");
	return KERN_SUCCESS;
}

kern_return_t
memory_object_lock_completed(
	ipc_port_t		reply_to,
	mach_msg_type_name_t	reply_to_type,
	xmm_obj_t		mobj,
	vm_offset_t		offset,
	vm_size_t		length)
{
	xmm_reply_t reply = (xmm_reply_t) reply_to;

	/*
	 * Check that no simple lock is held.
	 */
	/* check_simple_locks(); */

	assert(reply != XMM_REPLY_NULL);
	assert(!xmm_reply_empty(reply));
	assert(reply_to_type == MACH_MSG_TYPE_PORT_SEND_ONCE);
	MOBJ->pending_replies--;

	return M_LOCK_COMPLETED(mobj, offset, length, reply);
}

kern_return_t
memory_object_supply_completed(
	ipc_port_t		reply_to,
	mach_msg_type_name_t	reply_to_type,
	xmm_obj_t		mobj,
	vm_offset_t		offset,
	vm_size_t		length,
	kern_return_t		result,
	vm_offset_t		error_offset)
{
	xmm_reply_t reply = (xmm_reply_t) reply_to;

	/*
	 * Check that no simple lock is held.
	 */
	/* check_simple_locks(); */

	assert(reply != XMM_REPLY_NULL);
	assert(!xmm_reply_empty(reply));
	assert(reply_to_type == MACH_MSG_TYPE_PORT_SEND_ONCE);
	return M_SUPPLY_COMPLETED(mobj, offset, length, result, error_offset,
				  reply);
}

kern_return_t
memory_object_freeze(
	ipc_port_t	memory_object,
	xmm_obj_t	mobj,
	char		*existence_map,
	unsigned	existence_size)
{
#ifdef	lint
	memory_object++;
#endif	/* lint */

	/*
	 * Check that no simple lock is held.
	 */
	/* check_simple_locks(); */

	if (mobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}
	return M_FREEZE(mobj, existence_map, existence_size);
}

kern_return_t
memory_object_share(
	ipc_port_t	memory_object,
	xmm_obj_t	mobj)
{
#ifdef	lint
	memory_object++;
#endif	/* lint */

	/*
	 * Check that no simple lock is held.
	 */
	/* check_simple_locks(); */

	if (mobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}
	return M_SHARE(mobj);
}

kern_return_t
memory_object_declare_page(
	ipc_port_t	memory_object,
	xmm_obj_t	mobj,
	vm_offset_t	offset,
	vm_size_t	size)
{
#ifdef	lint
	memory_object++;
#endif	/* lint */

	/*
	 * Check that no simple lock is held.
	 */
	/* check_simple_locks(); */

	if (mobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}
	return M_DECLARE_PAGE(mobj, offset, size);
}

kern_return_t
memory_object_declare_pages(
	ipc_port_t	memory_object,
	xmm_obj_t	mobj,
	char		*existence_map,
	unsigned	existence_size,
	boolean_t	frozen)
{
#ifdef	lint
	memory_object++;
#endif	/* lint */

	/*
	 * Check that no simple lock is held.
	 */
	/* check_simple_locks(); */

	if (mobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}
	return M_DECLARE_PAGES(mobj, existence_map, existence_size, frozen);
}

kern_return_t
memory_object_data_return(
	ipc_port_t	memory_object,
	xmm_obj_t	mobj,
	vm_offset_t	offset,
	vm_map_copy_t	data,
	vm_size_t	length,
	boolean_t	dirty,
	boolean_t	kernel_copy)
{
#ifdef	lint
	memory_object++;
#endif	/* lint */

	/*
	 * Check that no simple lock is held.
	 */
	/* check_simple_locks(); */

	if (mobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}

	xmm_obj_lock(mobj);
	if (MOBJ->modwc) {
		assert(MOBJ->p_count >= 0);
		MOBJ->p_count++;
#if	PENDING_TRACKING
		MOBJ->p_seen++;
#endif
	}
	xmm_obj_unlock(mobj);

	return M_DATA_RETURN(mobj, offset, data, length, dirty, kernel_copy);
}

kern_return_t
memory_object_caching(
	ipc_port_t	memory_object,
	xmm_obj_t	mobj)
{
#ifdef	lint
	memory_object++;
#endif	/* lint */

	/*
	 * Check that no simple lock is held.
	 */
	/* check_simple_locks(); */

	if (mobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}
	if (! xmm_server_lazy_enable) {
		return M_CACHING(mobj);
	}
	assert(! MOBJ->terminating);
	xmm_queue_lock();
	assert(! MOBJ->caching);
	MOBJ->caching = TRUE;
	MOBJ->caching_age = 0;
	queue_enter(&xmm_server_caching_list, MOBJ, mobj_t, caching_list);
	xmm_server_queue_version++;
	xmm_queue_unlock();
	xmm_server_caching_threadcall();
	return KERN_SUCCESS;
}

kern_return_t
memory_object_uncaching(
	ipc_port_t	memory_object,
	xmm_obj_t	mobj)
{
#ifdef	lint
	memory_object++;
#endif	/* lint */

	/*
	 * Check that no simple lock is held.
	 */
	/* check_simple_locks(); */

	if (mobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}
	assert(! MOBJ->terminating);
	xmm_queue_lock();
	if (MOBJ->caching) {
		assert(xmm_server_lazy_enable);
		MOBJ->caching = FALSE;
		queue_remove(&xmm_server_caching_list, MOBJ, mobj_t,
			     caching_list);
		xmm_server_queue_version++;
		xmm_queue_unlock();
		return k_server_uncaching_permitted(mobj);
	} else {
		xmm_queue_unlock();
		return M_UNCACHING(mobj);
	}
}

kern_return_t
memory_object_synchronize(
	ipc_port_t	memory_object,
	xmm_obj_t 	mobj,
	vm_offset_t	offset,
	vm_size_t	length,
	vm_sync_t	sync_flags)
{
#ifdef	lint
	memory_object++;
#endif	/* lint */

	/*
	 * Check that no simple lock is held.
	 */
	/* check_simple_locks(); */

	if (mobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}

	xmm_obj_lock(mobj);
	MOBJ->p_sync++;
	xmm_obj_unlock(mobj);

	return M_SYNCHRONIZE(mobj, offset, length, sync_flags);
}

#if	MOR
/*ARGSUSED*/
void
xmm_memory_object_rep_init(
	vm_object_rep_t	repo)
{
	panic("xmm_memory_object_rep_init is not implemented\n");
}

/*ARGSUSED*/
kern_return_t
memory_object_rejected(
	ipc_port_t		memory_object,
	xmm_obj_t		mobj,
	kern_return_t		reason)
{
	panic("memory_object_rejected is not implemented\n");
	return KERN_FAILURE;	
}
#endif	/* MOR */

void
xmm_server_caching_threadcall(void)
{
	xmm_obj_t mobj;
	queue_entry_t q, q_next;
	int version;

	/*
	 * Cache everything on the aged caching list.
	 */
	xmm_queue_lock();
restart:
	for (q = queue_first(&xmm_server_caching_list);
	     ! queue_end(&xmm_server_caching_list, q);
	     q = q_next) {
		assert(xmm_server_lazy_enable);
		mobj = (xmm_obj_t) q;
		q_next = queue_next(&MOBJ->caching_list);
		if (MOBJ->caching_age > 1) {
			MOBJ->caching = FALSE;
			queue_remove(&xmm_server_caching_list, MOBJ, mobj_t,
				     caching_list);
			version = ++xmm_server_queue_version;
			xmm_queue_unlock();
			(void) M_CACHING(mobj);
			xmm_queue_lock();
			if (version != xmm_server_queue_version) {
				/* not safe to continue; restart at begining */
				goto restart;
			}
		}
	}
	xmm_queue_unlock();
}

boolean_t
xmm_server_caching_timeout(void)
{
	mobj_t obj;
	boolean_t timeout_wanted = FALSE;

	xmm_queue_lock();
	if (++xmm_server_timeout_count < xmm_server_timeout_multiple) {
		xmm_queue_unlock();
		return FALSE;
	} else {
		xmm_server_timeout_count = 0;
	}

	/*
	 * Merge contents of new caching list into old caching list.
	 *
	 * XXX
	 * Should have a queue_merge function that just moves a couple
	 * of pointers, instead of having to interate over the whole list.
	 */
	queue_iterate(&xmm_server_caching_list, obj, mobj_t, caching_list) {
		assert(xmm_server_lazy_enable);
		assert(obj->caching);
		if (obj->caching_age > 1) {
			timeout_wanted = TRUE;
		} else {
			obj->caching_age++;
		}
	}
	xmm_server_queue_version++;
	xmm_queue_unlock();

	return timeout_wanted;
}

void
xmm_server_init(void)
{
	queue_init(&xmm_server_caching_list);
	xmm_queue_lock_init();
}


#if	XMM_OBJECT_CHECKS
/*
 *	Verify that object state matches MOBJ state.
 *	However, when terminating an object, the pager's
 *	type might be IKOT_PAGER_TERMINATING rather
 *	than IKOT_PAGER and the pager's object might not exist.
 */
void
object_check(
	xmm_obj_t	mobj)
{
	vm_object_t obj;

	assert(ip_kotype(MOBJ->pager) == IKOT_PAGER ||
	       ip_kotype(MOBJ->pager) == IKOT_PAGER_TERMINATING);
	obj = (vm_object_t)MOBJ->pager->ip_kobject;
	/*
	 *	The number of completions pending on the object
	 *	must equal or exceed the number of completions
	 *	in the mobj.  While normally these counts should
	 *	be equal, in practice it is possible for reservations
	 *	to be acquired without being used for some time.
	 *	(The thread acquiring the reservation(s) can block.)
	 *	It is conceivable that a thread could block after
	 *	making a reservation, on its way through the XMM stack,
	 *	but again the count in the XMM stack can't exceed
	 *	that on the object itself.  A data_write_completed
	 *	travelling through the stack on the way to the
	 *	object could decrement the stack's count and then
	 *	block -- again, the stack's count couldn't exceed
	 *	that of the object.  This assertion should be valid
	 *	even in an SMP environment.
	 *
	 *	However, pending counts should be ignored for pagers
	 *	that don't participate in paging flow control.
	 */
	assert(obj == VM_OBJECT_NULL || !MOBJ->modwc ||
	       obj->completions_pending >= MOBJ->p_count);
}


void
xmm_server_object_check(
	vm_object_t	object)
{
	xmm_obj_t mobj;

	mobj = (xmm_obj_t)object->pager_request;
	assert(!MOBJ->modwc || MOBJ->p_count <= object->completions_pending);
}
#endif	/* XMM_OBJECT_CHECKS */


#if	MACH_KDB
#include <ddb/db_output.h>

#define printf	kdbprintf

void
xmm_object_stack_print(
	xmm_obj_t 	mobj)
{
	if (mobj == XMM_OBJ_NULL)
		return;
	xmm_obj_print((db_addr_t)mobj);
	indent += 4;
	iprintf("pager=0x%x pending_replies=%d wc_pending=%d sc_pending=%d\n",
		MOBJ->pager,
		MOBJ->pending_replies,
		MOBJ->p_count,
		MOBJ->p_sync);
	iprintf("%slock_request, %sterminating, %scaching, %smodwc",
		BOOL(MOBJ->lock_request),
		BOOL(MOBJ->terminating),		
		BOOL(MOBJ->caching),
		BOOL(MOBJ->modwc));
	printf(", caching_age=%d\n", MOBJ->caching_age);
	indent -= 4;
	M_DEBUG(mobj, 0);
}

#endif	/* MACH_KDB */
