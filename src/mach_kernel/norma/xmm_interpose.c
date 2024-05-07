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
 * Revision 2.3.2.4  92/06/24  18:02:44  jeffreyh
 * 	Calling sequence changes for XMM Framework Cleanup.  No more
 * 	xmm_kobj_link().
 * 	[92/06/24            sjs]
 * 
 * 	Pass release arguments in lock_completed.
 * 	[92/06/09            dlb]
 * 
 * 	use_old_pageout --> use_routine
 * 	[92/06/04            dlb]
 * 
 * Revision 2.3.2.3  92/03/28  10:12:54  jeffreyh
 * 	Changed data_write to data_write_return, deleted data_return method.
 * 	[92/03/20            sjs]
 * 
 * Revision 2.3.2.2  92/02/21  11:26:00  jsb
 * 	Now xmm_kobj_link handles checks for multiple inits.
 * 	[92/02/18  08:00:07  jsb]
 * 
 * 	Explicitly provide name parameter to xmm_decl macro.
 * 	Changed m_interpose_terminate to simply call M_TERMINATE instead of
 * 	explicitly deallocating obj, since we now do real reference counting.
 * 	[92/02/16  14:21:04  jsb]
 * 
 * 	Use new xmm_decl, and new memory_object_name and deallocation protocol.
 * 	[92/02/09  12:54:10  jsb]
 * 
 * Revision 2.3.2.1  92/01/21  21:54:19  jsb
 * 	De-linted. Supports new (dlb) memory object routines.
 * 	Supports arbitrary reply ports to lock_request, etc.
 * 	Converted mach_port_t (and port_t) to ipc_port_t.
 * 	[92/01/20  17:23:15  jsb]
 * 
 * Revision 2.3  91/07/01  08:26:16  jsb
 * 	Collect garbage. Return valid return values.
 * 	[91/06/29  15:31:20  jsb]
 * 
 * Revision 2.2  91/06/17  15:48:21  jsb
 * 	First checkin.
 * 	[91/06/17  11:08:05  jsb]
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
/*
 *	File:	norma/xmm_interpose.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Definitions for null instances of xmm functions.
 */

#ifdef	MACH_KERNEL
#include <norma/xmm_obj.h>
#else	/* MACH_KERNEL */
#include <xmm_obj.h>
#endif	/* MACH_KERNEL */

xmm_decl(interpose, "interpose", sizeof(struct xmm_obj));

m_interpose_init(mobj, pagesize, internal, size)
	xmm_obj_t mobj;
	vm_size_t pagesize;
	boolean_t internal;
	vm_size_t size;
{
	return M_INIT(mobj, pagesize, internal, size);
}

m_interpose_terminate(mobj, release)
	xmm_obj_t mobj;
	boolean_t release;
{
	return M_TERMINATE(mobj, release);
}

void
m_interpose_deallocate(mobj)
	xmm_obj_t mobj;
{
}

m_interpose_copy(mobj, offset, length, new_mobj)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	xmm_obj_t new_mobj;
{
	return M_COPY(mobj, offset, length, new_mobj);
}

m_interpose_data_request(mobj, offset, length, desired_access)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{
	return M_DATA_REQUEST(mobj, offset, length, desired_access);
}

m_interpose_data_unlock(mobj, offset, length, desired_access)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{
	return M_DATA_UNLOCK(mobj, offset, length, desired_access);
}

m_interpose_data_write_return(mobj, offset, data, length,
		dirty, kernel_copy, use_routine)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_offset_t data;
	vm_size_t length;
	boolean_t dirty;
	boolean_t kernel_copy;
	int use_routine;
{
	return M_DATA_WRT_RTN(mobj, offset, data, length,
			      dirty, kernel_copy, use_routine);
}

m_interpose_lock_completed(mobj, offset, length, reply, release)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	xmm_reply_t reply;
	boolean_t release;
{
	return M_LOCK_COMPLETED(mobj, offset, length, reply, release);
}

m_interpose_supply_completed(mobj, offset, length, result, error_offset, reply,
			     release)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	kern_return_t result;
	vm_offset_t error_offset;
	xmm_reply_t reply;
	boolean_t release;
{
	return M_SUPPLY_COMPLETED(reply, offset, length, result, error_offset);
}

m_interpose_change_completed(mobj, may_cache, copy_strategy, reply, release)
	xmm_obj_t mobj;
	boolean_t may_cache;
	memory_object_copy_strategy_t copy_strategy;
	xmm_reply_t reply;
	boolean_t release;
{
	return M_CHANGE_COMPLETED(mobj, may_cache, copy_strategy, reply,
				  release);
}


k_interpose_data_unavailable(kobj, offset, length)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
{
	return K_DATA_UNAVAILABLE(kobj, offset, length);
}

k_interpose_get_attributes(kobj, object_ready, may_cache, copy_strategy)
	xmm_obj_t kobj;
	boolean_t *object_ready;
	boolean_t *may_cache;
	memory_object_copy_strategy_t *copy_strategy;
{
	return K_GET_ATTRIBUTES(kobj, object_ready, may_cache, copy_strategy);
}

k_interpose_lock_request(kobj, offset, length, should_clean, should_flush,
			 lock_value, reply)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
	boolean_t should_clean;
	boolean_t should_flush;
	vm_prot_t lock_value;
	xmm_reply_t reply;
{
	return K_LOCK_REQUEST(kobj, offset, length, should_clean, should_flush,
			      lock_value, reply);
}

k_interpose_data_error(kobj, offset, length, error_value)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
	kern_return_t error_value;
{
	return K_DATA_ERROR(kobj, offset, length, error_value);
}

k_interpose_set_ready(kobj, object_ready, may_cache, copy_strategy,
		      use_routine, memory_object_name, reply)
	xmm_obj_t kobj;
	boolean_t object_ready;
	boolean_t may_cache;
	memory_object_copy_strategy_t copy_strategy;
	int use_routine;
	ipc_port_t memory_object_name;
	xmm_reply_t reply;
{
	return K_SET_READY(kobj, object_ready, may_cache, copy_strategy,
			   use_routine, memory_object_name, reply);
}

k_interpose_destroy(kobj, reason)
	xmm_obj_t kobj;
	kern_return_t reason;
{
	return K_DESTROY(kobj, reason);
}

k_interpose_data_supply(kobj, offset, data, length, lock_value, precious,
			reply)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_offset_t data;
	vm_size_t length;
	vm_prot_t lock_value;
	boolean_t precious;
	xmm_reply_t reply;
{
	return K_DATA_SUPPLY(kobj, offset, data, length, lock_value, precious,
			     reply);
}

k_interpose_release(kobj)
	xmm_obj_t kobj;
{
	return K_RELEASE(kobj);
}
