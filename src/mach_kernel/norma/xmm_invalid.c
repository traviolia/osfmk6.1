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
 * Revision 2.4.2.4  92/06/24  18:02:47  jeffreyh
 * 	Changed routines to for new interfaces.
 * 	[92/06/18            sjs]
 * 
 * 	Add release arguments to lock_completed.
 * 	[92/06/09            dlb]
 * 
 * 	use_old_pageout --> use_routine
 * 	[92/06/04            dlb]
 * 
 * Revision 2.4.2.3  92/03/28  10:13:02  jeffreyh
 * 	Changed data_write to data_write_return, removed data_return
 * 	 method.  Change TERMINATE and CHANGE_COMPLETED methods to
 * 	 support m_o_change_attributes()
 * 	[92/03/20            sjs]
 * 
 * Revision 2.4.2.2  92/02/21  11:26:03  jsb
 * 	Explicitly provide name parameter to xmm_decl macro.
 * 	[92/02/16  14:21:46  jsb]
 * 
 * 	Use new xmm_decl, and new memory_object_name and deallocation protocol.
 * 	[92/02/09  13:56:31  jsb]
 * 
 * Revision 2.4.2.1  92/01/21  21:54:23  jsb
 * 	De-linted. Supports new (dlb) memory object routines.
 * 	Supports arbitrary reply ports to lock_request, etc.
 * 	Converted mach_port_t (and port_t) to ipc_port_t.
 * 	[92/01/20  17:23:35  jsb]
 * 
 * Revision 2.4  91/12/10  13:26:26  jsb
 * 	Added better debugging support to xmm_invalid_complain.
 * 	[91/12/10  12:46:26  jsb]
 * 
 * Revision 2.3  91/07/01  08:26:20  jsb
 * 	Added declaration of invalid_mclass, for debugging use.
 * 	[91/06/29  15:32:54  jsb]
 * 
 * Revision 2.2  91/06/17  15:48:26  jsb
 * 	First checkin.
 * 	[91/06/17  11:03:08  jsb]
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
 *	File:	norma/xmm_invalid.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Definitions for invalid instances of xmm functions.
 */

#ifdef	MACH_KERNEL
#include <norma/xmm_obj.h>
#else	/* MACH_KERNEL */
#include <xmm_obj.h>
#endif	/* MACH_KERNEL */

xmm_decl(invalid, "invalid", 0);

xmm_invalid_complain(name, mobj)
	char *name;
	xmm_obj_t mobj;
{
	panic("xmm_invalid_complain(%s,0x%x)", name, mobj);
#if 000
	printf("m_invalid_%s from xmm_%s\n",
	       name, mobj->k_kobj->class->c_name);
	if ((int) mobj->refcount < -1000 || (int) mobj->refcount > 1000) {
		printf("deallocated! 0x%x/0x%x=%s\n",
		       mobj->k_kobj,
		       mobj,
		       (char *) mobj->refcount);
	}
#endif
	return KERN_FAILURE;
}

xmm_invalid_complain_reply(name, reply)
	char *name;
	xmm_reply_t reply;
{
	panic("xmm_invalid_complain(%s,0x%x)", name, reply);
	/* XXX could print more info here */
	return KERN_FAILURE;
}

m_invalid_init(mobj, pagesize, internal, size)
	xmm_obj_t mobj;
	vm_size_t pagesize;
	boolean_t internal;
	vm_size_t size;
{
#ifdef	lint
	M_INIT(mobj, pagesize, internal, size);
#endif	lint
	printf("m_invalid_init from xmm_%s\n", mobj->class->c_name);
	return KERN_FAILURE;
}

m_invalid_terminate(mobj, release)
	xmm_obj_t mobj;
	boolean_t release;
{
#ifdef	lint
	M_TERMINATE(mobj, release);
#endif	lint
	return xmm_invalid_complain("terminate", mobj);
}

void
m_invalid_deallocate(mobj)
	xmm_obj_t mobj;
{
	panic("m_invalid_deallocate");
}

void
k_invalid_deallocate(kobj)
	xmm_obj_t kobj;
{
	panic("k_invalid_deallocate");
}

m_invalid_copy(mobj, offset, length, new_mobj)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	xmm_obj_t new_mobj;
{
#ifdef	lint
	M_COPY(mobj, offset, length, new_mobj);
#endif	lint
	return xmm_invalid_complain("copy", mobj);
}

m_invalid_data_request(mobj, offset, length, desired_access)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{
#ifdef	lint
	M_DATA_REQUEST(mobj, offset, length, desired_access);
#endif	lint
	return xmm_invalid_complain("data_request", mobj);
}

m_invalid_data_unlock(mobj, offset, length, desired_access)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{
#ifdef	lint
	M_DATA_UNLOCK(mobj, offset, length, desired_access);
#endif	lint
	return xmm_invalid_complain("data_unlock", mobj);
}

m_invalid_data_write_return(mobj, offset, data, length,
			    dirty, kernel_copy, use_routine)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_offset_t data;
	vm_size_t length;
	boolean_t dirty;
	boolean_t kernel_copy;
	int use_routine;
{
#ifdef	lint
	M_DATA_WRT_RTN(mobj, offset, data, length,
		       dirty, kernel_copy, use_routine);
#endif	lint
	return xmm_invalid_complain("data_write_return", mobj);
}

m_invalid_lock_completed(mobj, offset, length, reply, release)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	xmm_reply_t reply;
	boolean_t release;
{
#ifdef	lint
	M_LOCK_COMPLETED(mobj, offset, length, reply, release);
#endif	lint
	return xmm_invalid_complain_reply("lock_completed", reply);
}

m_invalid_supply_completed(mobj, offset, length, result, error_offset, reply,
			   release)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	kern_return_t result;
	vm_offset_t error_offset;
	xmm_reply_t reply;
	boolean_t release;
{
#ifdef	lint
	M_SUPPLY_COMPLETED(mobj, offset, length, result, error_offset, reply);
#endif	lint
	return xmm_invalid_complain_reply("supply_completed", reply);
}

m_invalid_change_completed(mobj, may_cache, copy_strategy, reply, release)
	xmm_obj_t mobj;
	boolean_t may_cache;
	memory_object_copy_strategy_t copy_strategy;
	xmm_reply_t reply;
	boolean_t release;
{
#ifdef	lint
	M_CHANGE_COMPLETED(mobj, may_cache, copy_strategy, reply, release);
#endif	lint
	return xmm_invalid_complain_reply("change_completed", reply);
}

k_invalid_release(kobj)
	xmm_obj_t kobj;
{
#ifdef	lint
	K_RELEASE(kobj);
#endif
	panic("k_invalid_release");
	return(KERN_FAILURE);
}

k_invalid_data_unavailable(kobj, offset, length)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
{
#ifdef	lint
	K_DATA_UNAVAILABLE(kobj, offset, length);
#endif	lint
	return xmm_invalid_complain("data_unavailable", kobj);
}

k_invalid_get_attributes(kobj, object_ready, may_cache, copy_strategy)
	xmm_obj_t kobj;
	boolean_t *object_ready;
	boolean_t *may_cache;
	memory_object_copy_strategy_t *copy_strategy;
{
#ifdef	lint
	K_GET_ATTRIBUTES(kobj, object_ready, may_cache, copy_strategy);
#endif	lint
	return xmm_invalid_complain("get_attributes", kobj);
}

k_invalid_lock_request(kobj, offset, length, should_clean, should_flush,
		       lock_value, reply)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
	boolean_t should_clean;
	boolean_t should_flush;
	vm_prot_t lock_value;
	xmm_reply_t reply;
{
#ifdef	lint
	K_LOCK_REQUEST(kobj, offset, length, should_clean, should_flush,
		       lock_value, reply);
#endif	lint
	return xmm_invalid_complain("lock_request", kobj);
}

k_invalid_data_error(kobj, offset, length, error_value)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
	kern_return_t error_value;
{
#ifdef	lint
	K_DATA_ERROR(kobj, offset, length, error_value);
#endif	lint
	return xmm_invalid_complain("data_error", kobj);
}

k_invalid_set_ready(kobj, object_ready, may_cache, copy_strategy,
		    use_routine, memory_object_name, reply)
	xmm_obj_t kobj;
	boolean_t object_ready;
	boolean_t may_cache;
	memory_object_copy_strategy_t copy_strategy;
	int use_routine;
	ipc_port_t memory_object_name;
	xmm_reply_t reply;
{
#ifdef	lint
	K_SET_READY(kobj, object_ready, may_cache, copy_strategy,
		    use_routine, memory_object_name, reply);
#endif	lint
	return xmm_invalid_complain("set_ready", kobj);
}

k_invalid_destroy(kobj, reason)
	xmm_obj_t kobj;
	kern_return_t reason;
{
#ifdef	lint
	K_DESTROY(kobj, reason);
#endif	lint
	return xmm_invalid_complain("destroy", kobj);
}

k_invalid_data_supply(kobj, offset, data, length, lock_value, precious, reply)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_offset_t data;
	vm_size_t length;
	vm_prot_t lock_value;
	boolean_t precious;
	xmm_reply_t reply;
{
#ifdef	lint
	K_DATA_SUPPLY(kobj, offset, data, length, lock_value, precious, reply);
#endif	lint
	return xmm_invalid_complain("data_supply", kobj);
}
