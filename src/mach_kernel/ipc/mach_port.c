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
 * Revision 2.6.3.2  92/09/15  17:21:01  jeffreyh
 * 	Don't round region size to page boundary when calling
 * 	vm_map_copyin.  From jph@tamarack.cray.com
 * 	[92/09/15            dlb]
 * 
 * Revision 2.6.3.1  92/03/03  16:19:12  jeffreyh
 * 	Eliminate keep_wired argument from vm_map_copyin().
 * 	[92/02/21  10:13:34  dlb]
 * 
 * Revision 2.6  91/10/09  16:11:42  af
 * 	 Revision 2.5.2.1  91/09/16  10:16:19  rpd
 * 	 	Added <ipc/ipc_notify.h>.
 * 	 	[91/09/02            rpd]
 * 
 * Revision 2.5.2.1  91/09/16  10:16:19  rpd
 * 	Added <ipc/ipc_notify.h>.
 * 	[91/09/02            rpd]
 * 
 * Revision 2.5  91/08/28  11:14:04  jsb
 * 	Added mach_port_set_seqno and updated mach_port_get_receive_status
 * 	for mps_seqno.  Added old_mach_port_get_receive_status.
 * 	[91/08/09            rpd]
 * 	Changed port_names for new vm_map_copyout failure behavior.
 * 	[91/08/03            rpd]
 * 
 * Revision 2.4  91/05/14  16:39:20  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:25:06  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  15:53:35  mrt]
 * 
 * Revision 2.2  90/06/02  14:52:28  rpd
 * 	Modified mach_port_get_receive_status to return a mach_port_status_t.
 * 	[90/05/13            rpd]
 * 	Created for new IPC.
 * 	[90/03/26  21:06:13  rpd]
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
 *	File:	ipc/mach_port.c
 *	Author:	Rich Draves
 *	Date: 	1989
 *
 *	Exported kernel calls.  See mach/mach_port.defs.
 */

#include <mach_debug.h>

#include <mach/port.h>
#include <mach/kern_return.h>
#include <mach/notify.h>
#include <mach/mach_param.h>
#include <mach/vm_param.h>
#include <mach/vm_prot.h>
#include <kern/task.h>
#include <kern/thread_act.h>
#include <kern/thread_pool.h>
#include <mach/mach_server.h>
#include <mach/mach_port_server.h>
#include <vm/vm_map.h>
#include <vm/vm_kern.h>
#include <vm/vm_user.h>
#include <ipc/ipc_entry.h>
#include <ipc/ipc_space.h>
#include <ipc/ipc_object.h>
#include <ipc/ipc_notify.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_pset.h>
#include <ipc/ipc_right.h>
#include <kern/misc_protos.h>

/*
 * Forward declarations
 */
void mach_port_names_helper(
	ipc_port_timestamp_t	timestamp,
	ipc_entry_t		entry,
	mach_port_t		name,
	mach_port_t		*names,
	mach_port_type_t	*types,
	ipc_entry_num_t		*actualp);

void mach_port_gst_helper(
	ipc_pset_t		pset,
	ipc_port_t		port,
	ipc_entry_num_t		maxnames,
	mach_port_t		*names,
	ipc_entry_num_t		*actualp);


/* Zeroed template of qos flags */

static mach_port_qos_t	qos_template;

/*
 *	Routine:	mach_port_names_helper
 *	Purpose:
 *		A helper function for mach_port_names.
 */

void
mach_port_names_helper(
	ipc_port_timestamp_t	timestamp,
	ipc_entry_t		entry,
	mach_port_t		name,
	mach_port_t		*names,
	mach_port_type_t	*types,
	ipc_entry_num_t		*actualp)
{
	ipc_entry_bits_t bits = entry->ie_bits;
	ipc_port_request_index_t request = entry->ie_request;
	mach_port_type_t type;
	ipc_entry_num_t actual;

	if (bits & MACH_PORT_TYPE_SEND_RIGHTS) {
		ipc_port_t port;
		boolean_t died;

		port = (ipc_port_t) entry->ie_object;
		assert(port != IP_NULL);

		/*
		 *	The timestamp serializes mach_port_names
		 *	with ipc_port_destroy.  If the port died,
		 *	but after mach_port_names started, pretend
		 *	that it isn't dead.
		 */

		ip_lock(port);
		died = (!ip_active(port) &&
			IP_TIMESTAMP_ORDER(port->ip_timestamp, timestamp));
		ip_unlock(port);

		if (died) {
			/* pretend this is a dead-name entry */

			bits &= ~(IE_BITS_TYPE_MASK);
			bits |= MACH_PORT_TYPE_DEAD_NAME;
			if (request != 0)
				bits++;
			request = 0;
		}
	}

	type = IE_BITS_TYPE(bits);
	if (request != 0)
		type |= MACH_PORT_TYPE_DNREQUEST;

	actual = *actualp;
	names[actual] = name;
	types[actual] = type;
	*actualp = actual+1;
}

/*
 *	Routine:	mach_port_names [kernel call]
 *	Purpose:
 *		Retrieves a list of the rights present in the space,
 *		along with type information.  (Same as returned
 *		by mach_port_type.)  The names are returned in
 *		no particular order, but they (and the type info)
 *		are an accurate snapshot of the space.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		KERN_SUCCESS		Arrays of names and types returned.
 *		KERN_INVALID_TASK	The space is null.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_RESOURCE_SHORTAGE	Couldn't allocate memory.
 */

kern_return_t
mach_port_names(
	ipc_space_t		space,
	mach_port_t		**namesp,
	mach_msg_type_number_t	*namesCnt,
	mach_port_type_t	**typesp,
	mach_msg_type_number_t	*typesCnt)
{
	ipc_tree_entry_t tentry;
	ipc_entry_t table;
	ipc_entry_num_t tsize;
	mach_port_index_t index;
	ipc_entry_num_t actual;	/* this many names */
	ipc_port_timestamp_t timestamp;	/* logical time of this operation */
	mach_port_t *names;
	mach_port_type_t *types;
	kern_return_t kr;

	vm_size_t size;		/* size of allocated memory */
	vm_offset_t addr1;	/* allocated memory, for names */
	vm_offset_t addr2;	/* allocated memory, for types */
	vm_map_copy_t memory1;	/* copied-in memory, for names */
	vm_map_copy_t memory2;	/* copied-in memory, for types */

	/* safe simplifying assumption */
	assert_static(sizeof(mach_port_t) == sizeof(mach_port_type_t));

	if (space == IS_NULL)
		return KERN_INVALID_TASK;

	size = 0;

	for (;;) {
		ipc_entry_num_t bound;
		vm_size_t size_needed;

		is_read_lock(space);
		if (!space->is_active) {
			is_read_unlock(space);
			if (size != 0) {
				kmem_free(ipc_kernel_map, addr1, size);
				kmem_free(ipc_kernel_map, addr2, size);
			}
			return KERN_INVALID_TASK;
		}

		/* upper bound on number of names in the space */

		bound = space->is_table_size + space->is_tree_total;
		size_needed = round_page(bound * sizeof(mach_port_t));

		if (size_needed <= size)
			break;

		is_read_unlock(space);

		if (size != 0) {
			kmem_free(ipc_kernel_map, addr1, size);
			kmem_free(ipc_kernel_map, addr2, size);
		}
		size = size_needed;

		kr = vm_allocate(ipc_kernel_map, &addr1, size, TRUE);
		if (kr != KERN_SUCCESS)
			return KERN_RESOURCE_SHORTAGE;

		kr = vm_allocate(ipc_kernel_map, &addr2, size, TRUE);
		if (kr != KERN_SUCCESS) {
			kmem_free(ipc_kernel_map, addr1, size);
			return KERN_RESOURCE_SHORTAGE;
		}

		/* can't fault while we hold locks */

		kr = vm_map_wire(ipc_kernel_map, addr1, addr1 + size,
				     VM_PROT_READ|VM_PROT_WRITE, FALSE);
		assert(kr == KERN_SUCCESS);

		kr = vm_map_wire(ipc_kernel_map, addr2, addr2 + size,
				     VM_PROT_READ|VM_PROT_WRITE, FALSE);
		assert(kr == KERN_SUCCESS);
	}
	/* space is read-locked and active */

	names = (mach_port_t *) addr1;
	types = (mach_port_type_t *) addr2;
	actual = 0;

	timestamp = ipc_port_timestamp();

	table = space->is_table;
	tsize = space->is_table_size;

	for (index = 0; index < tsize; index++) {
		ipc_entry_t entry = &table[index];
		ipc_entry_bits_t bits = entry->ie_bits;

		if (IE_BITS_TYPE(bits) != MACH_PORT_TYPE_NONE) {
			mach_port_t name = MACH_PORT_MAKEB(index, bits);

			mach_port_names_helper(timestamp, entry, name,
					       names, types, &actual);
		}
	}

	for (tentry = ipc_splay_traverse_start(&space->is_tree);
	     tentry != ITE_NULL;
	     tentry = ipc_splay_traverse_next(&space->is_tree, FALSE)) {
		ipc_entry_t entry = &tentry->ite_entry;
		mach_port_t name = tentry->ite_name;

		assert(IE_BITS_TYPE(tentry->ite_bits) !=
			MACH_PORT_TYPE_NONE);

		mach_port_names_helper(timestamp, entry, name,
				       names, types, &actual);
	}
	ipc_splay_traverse_finish(&space->is_tree);
	is_read_unlock(space);

	if (actual == 0) {
		memory1 = VM_MAP_COPY_NULL;
		memory2 = VM_MAP_COPY_NULL;

		if (size != 0) {
			kmem_free(ipc_kernel_map, addr1, size);
			kmem_free(ipc_kernel_map, addr2, size);
		}
	} else {
		vm_size_t size_used;
		vm_size_t vm_size_used;

		size_used = actual * sizeof(mach_port_t);
		vm_size_used = round_page(size_used);

		/*
		 *	Make used memory pageable and get it into
		 *	copied-in form.  Free any unused memory.
		 */

		kr = vm_map_unwire(ipc_kernel_map,
				     addr1, addr1 + vm_size_used, FALSE);
		assert(kr == KERN_SUCCESS);

		kr = vm_map_unwire(ipc_kernel_map,
				     addr2, addr2 + vm_size_used, FALSE);
		assert(kr == KERN_SUCCESS);

		kr = vm_map_copyin(ipc_kernel_map, addr1, size_used,
				   TRUE, &memory1);
		assert(kr == KERN_SUCCESS);

		kr = vm_map_copyin(ipc_kernel_map, addr2, size_used,
				   TRUE, &memory2);
		assert(kr == KERN_SUCCESS);

		if (vm_size_used != size) {
			kmem_free(ipc_kernel_map,
				  addr1 + vm_size_used, size - vm_size_used);
			kmem_free(ipc_kernel_map,
				  addr2 + vm_size_used, size - vm_size_used);
		}
	}

	*namesp = (mach_port_t *) memory1;
	*namesCnt = actual;
	*typesp = (mach_port_type_t *) memory2;
	*typesCnt = actual;
	return KERN_SUCCESS;
}

/*
 *	Routine:	mach_port_type [kernel call]
 *	Purpose:
 *		Retrieves the type of a right in the space.
 *		The type is a bitwise combination of one or more
 *		of the following type bits:
 *			MACH_PORT_TYPE_SEND
 *			MACH_PORT_TYPE_RECEIVE
 *			MACH_PORT_TYPE_SEND_ONCE
 *			MACH_PORT_TYPE_PORT_SET
 *			MACH_PORT_TYPE_DEAD_NAME
 *		In addition, the following pseudo-type bits may be present:
 *			MACH_PORT_TYPE_DNREQUEST
 *				A dead-name notification is requested.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		KERN_SUCCESS		Type is returned.
 *		KERN_INVALID_TASK	The space is null.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_INVALID_NAME	The name doesn't denote a right.
 */

kern_return_t
mach_port_type(
	ipc_space_t		space,
	mach_port_t		name,
	mach_port_type_t	*typep)
{
	mach_port_urefs_t urefs;
	ipc_entry_t entry;
	kern_return_t kr;

	if (space == IS_NULL)
		return KERN_INVALID_TASK;

	kr = ipc_right_lookup_write(space, name, &entry);
	if (kr != KERN_SUCCESS)
		return kr;
	/* space is write-locked and active */

	kr = ipc_right_info(space, name, entry, typep, &urefs);
	if (kr == KERN_SUCCESS)
		is_write_unlock(space);
	/* space is unlocked */
	return kr;
}

/*
 *	Routine:	mach_port_rename [kernel call]
 *	Purpose:
 *		Changes the name denoting a right,
 *		from oname to nname.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		KERN_SUCCESS		The right is renamed.
 *		KERN_INVALID_TASK	The space is null.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_INVALID_NAME	The oname doesn't denote a right.
 *		KERN_INVALID_VALUE	The nname isn't a legal name.
 *		KERN_NAME_EXISTS	The nname already denotes a right.
 *		KERN_RESOURCE_SHORTAGE	Couldn't allocate memory.
 */

kern_return_t
mach_port_rename(
	ipc_space_t	space,
	mach_port_t	oname,
	mach_port_t	nname)
{
	if (space == IS_NULL)
		return KERN_INVALID_TASK;

	if (!MACH_PORT_VALID(nname))
		return KERN_INVALID_VALUE;

	return ipc_object_rename(space, oname, nname);
}

/*
 *	Routine:	mach_port_allocate_name [kernel call]
 *	Purpose:
 *		Allocates a right in a space, using a specific name
 *		for the new right.  Possible rights:
 *			MACH_PORT_RIGHT_RECEIVE
 *			MACH_PORT_RIGHT_PORT_SET
 *			MACH_PORT_RIGHT_DEAD_NAME
 *
 *		A new port (allocated with MACH_PORT_RIGHT_RECEIVE)
 *		has no extant send or send-once rights and no queued
 *		messages.  Its queue limit is MACH_PORT_QLIMIT_DEFAULT
 *		and its make-send count is 0.  It is not a member of
 *		a port set.  It has no registered no-senders or
 *		port-destroyed notification requests.
 *
 *		A new port set has no members.
 *
 *		A new dead name has one user reference.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		KERN_SUCCESS		The right is allocated.
 *		KERN_INVALID_TASK	The space is null.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_INVALID_VALUE	The name isn't a legal name.
 *		KERN_INVALID_VALUE	"right" isn't a legal kind of right.
 *		KERN_NAME_EXISTS	The name already denotes a right.
 *		KERN_RESOURCE_SHORTAGE	Couldn't allocate memory.
 *
 *	Restrictions on name allocation:  NT bits are reserved by kernel,
 *	must be set on any chosen name.  Can't do this at all in kernel
 *	loaded server.
 */

kern_return_t
mach_port_allocate_name(
	ipc_space_t		space,
	mach_port_right_t	right,
	mach_port_t		name)
{
	kern_return_t		kr;
	mach_port_qos_t		qos = qos_template;

	qos.name = TRUE;

	kr = mach_port_allocate_full (space, right, SUBSYSTEM_NULL,
					&qos, &name);
	return (kr);
}

/*
 *	Routine:	mach_port_allocate [kernel call]
 *	Purpose:
 *		Allocates a right in a space.  Like mach_port_allocate_name,
 *		except that the implementation picks a name for the right.
 *		The name may be any legal name in the space that doesn't
 *		currently denote a right.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		KERN_SUCCESS		The right is allocated.
 *		KERN_INVALID_TASK	The space is null.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_INVALID_VALUE	"right" isn't a legal kind of right.
 *		KERN_RESOURCE_SHORTAGE	Couldn't allocate memory.
 *		KERN_NO_SPACE		No room in space for another right.
 */

kern_return_t
mach_port_allocate(
	ipc_space_t		space,
	mach_port_right_t	right,
	mach_port_t		*namep)
{
	kern_return_t		kr;
	mach_port_qos_t		qos = qos_template;

	kr = mach_port_allocate_full (space, right, SUBSYSTEM_NULL,
					&qos, namep);
	return (kr);
}

/*
 *	Routine:	mach_port_allocate_qos [kernel call]
 *	Purpose:
 *		Allocates a right, with qos options, in a space.  Like 
 *		mach_port_allocate_name, except that the implementation 
 *		picks a name for the right. The name may be any legal name 
 *		in the space that doesn't currently denote a right.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		KERN_SUCCESS		The right is allocated.
 *		KERN_INVALID_TASK	The space is null.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_INVALID_VALUE	"right" isn't a legal kind of right.
 *		KERN_RESOURCE_SHORTAGE	Couldn't allocate memory.
 *		KERN_NO_SPACE		No room in space for another right.
 */

kern_return_t
mach_port_allocate_qos(
	ipc_space_t		space,
	mach_port_right_t	right,
	mach_port_qos_t		*qosp,
	mach_port_t		*namep)
{
	kern_return_t		kr;

	kr = mach_port_allocate_full (space, right, SUBSYSTEM_NULL,
					qosp, namep);
	return (kr);
}

/*
 *	Routine:	mach_port_allocate_subsystem [kernel call]
 *	Purpose:
 *		Allocates a receive right in a space.  Like
 *		mach_port_allocate, except that the caller specifies an
 *		RPC subsystem that is to be used to implement RPC's to the
 *		port.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		KERN_SUCCESS		The right is allocated.
 *		KERN_INVALID_TASK	The space is null.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_RESOURCE_SHORTAGE	Couldn't allocate memory.
 *		KERN_NO_SPACE		No room in space for another right.
 *		KERN_INVALID_ARGUMENT	bogus subsystem
 */

kern_return_t
mach_port_allocate_subsystem(
	ipc_space_t		space,
	subsystem_t		subsystem,
	mach_port_t		*namep)
{
	kern_return_t		kr;
	ipc_port_t		port;
	mach_port_qos_t		qos = qos_template;

	kr = mach_port_allocate_full (space, MACH_PORT_RIGHT_RECEIVE,
					subsystem, &qos, namep);
	return (kr);
}

/*
 *	Routine:	mach_port_allocate_full [kernel call]
 *	Purpose:
 *		Allocates a right in a space.  Supports all of the
 *		special cases, such as specifying a subsystem,
 *		a specific name, a real-time port, etc.
 *		The name may be any legal name in the space that doesn't
 *		currently denote a right.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		KERN_SUCCESS		The right is allocated.
 *		KERN_INVALID_TASK	The space is null.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_INVALID_VALUE	"right" isn't a legal kind of right.
 *		KERN_RESOURCE_SHORTAGE	Couldn't allocate memory.
 *		KERN_NO_SPACE		No room in space for another right.
 */

kern_return_t
mach_port_allocate_full(
	ipc_space_t		space,
	mach_port_right_t	right,
	subsystem_t		subsystem,
	mach_port_qos_t		*qosp,
	mach_port_t		*namep)
{
	kern_return_t		kr;

	if (space == IS_NULL)
		return (KERN_INVALID_TASK);

	if (qosp->name) {
		if (!MACH_PORT_VALID (*namep))
			return (KERN_INVALID_VALUE);
		if (is_fast_space (space))
			return (KERN_FAILURE);
	}

	if (subsystem != SUBSYSTEM_NULL) {
		if (right != MACH_PORT_RIGHT_RECEIVE)
			return (KERN_INVALID_VALUE);
	}

	if (qosp->rt) {
		if (right != MACH_PORT_RIGHT_RECEIVE)
			return (KERN_INVALID_VALUE);
	}

	switch (right) {
	    case MACH_PORT_RIGHT_RECEIVE:
	    {
		ipc_port_t	port;

		if (qosp->name)
			kr = ipc_port_alloc_name(space, *namep, &port);
		else
			kr = ipc_port_alloc(space, namep, &port);
		if (kr == KERN_SUCCESS) {
			if (qosp->rt) 
				port->ip_flags |= IPC_PORT_FLAGS_RT;

			if (subsystem != SUBSYSTEM_NULL) {
				port->ip_subsystem = &subsystem->user;
				subsystem_reference (subsystem);
			}
			ip_unlock(port);
		}
		break;
	    }

	    case MACH_PORT_RIGHT_PORT_SET:
	    {
		ipc_pset_t	pset;

		if (qosp->name)
			kr = ipc_pset_alloc_name(space, *namep, &pset);
		else
			kr = ipc_pset_alloc(space, namep, &pset);
		if (kr == KERN_SUCCESS)
			ips_unlock(pset);
		break;
	    }

	    case MACH_PORT_RIGHT_DEAD_NAME:
		kr = ipc_object_alloc_dead(space, namep);
		break;

	    default:
		kr = KERN_INVALID_VALUE;
		break;
	}

	return (kr);
}

/*
 *	Routine:	mach_port_destroy [kernel call]
 *	Purpose:
 *		Cleans up and destroys all rights denoted by a name
 *		in a space.  The destruction of a receive right
 *		destroys the port, unless a port-destroyed request
 *		has been made for it; the destruction of a port-set right
 *		destroys the port set.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		KERN_SUCCESS		The name is destroyed.
 *		KERN_INVALID_TASK	The space is null.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_INVALID_NAME	The name doesn't denote a right.
 */

kern_return_t
mach_port_destroy(
	ipc_space_t	space,
	mach_port_t	name)
{
	ipc_entry_t entry;
	kern_return_t kr;

	if (space == IS_NULL)
		return KERN_INVALID_TASK;

	kr = ipc_right_lookup_write(space, name, &entry);
	if (kr != KERN_SUCCESS)
		return kr;
	/* space is write-locked and active */

	kr = ipc_right_destroy(space, name, entry); /* unlocks space */
	return kr;
}

/*
 *	Routine:	mach_port_deallocate [kernel call]
 *	Purpose:
 *		Deallocates a user reference from a send right,
 *		send-once right, or a dead-name right.  May
 *		deallocate the right, if this is the last uref,
 *		and destroy the name, if it doesn't denote
 *		other rights.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		KERN_SUCCESS		The uref is deallocated.
 *		KERN_INVALID_TASK	The space is null.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_INVALID_NAME	The name doesn't denote a right.
 *		KERN_INVALID_RIGHT	The right isn't correct.
 */

kern_return_t
mach_port_deallocate(
	ipc_space_t	space,
	mach_port_t	name)
{
	ipc_entry_t entry;
	kern_return_t kr;

	if (space == IS_NULL)
		return KERN_INVALID_TASK;

	kr = ipc_right_lookup_write(space, name, &entry);
	if (kr != KERN_SUCCESS)
		return kr;
	/* space is write-locked */

	kr = ipc_right_dealloc(space, name, entry); /* unlocks space */
	return kr;
}

/*
 *	Routine:	mach_port_get_refs [kernel call]
 *	Purpose:
 *		Retrieves the number of user references held by a right.
 *		Receive rights, port-set rights, and send-once rights
 *		always have one user reference.  Returns zero if the
 *		name denotes a right, but not the queried right.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		KERN_SUCCESS		Number of urefs returned.
 *		KERN_INVALID_TASK	The space is null.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_INVALID_VALUE	"right" isn't a legal value.
 *		KERN_INVALID_NAME	The name doesn't denote a right.
 */

kern_return_t
mach_port_get_refs(
	ipc_space_t		space,
	mach_port_t		name,
	mach_port_right_t	right,
	mach_port_urefs_t	*urefsp)
{
	mach_port_type_t type;
	mach_port_urefs_t urefs;
	ipc_entry_t entry;
	kern_return_t kr;

	if (space == IS_NULL)
		return KERN_INVALID_TASK;

	if (right >= MACH_PORT_RIGHT_NUMBER)
		return KERN_INVALID_VALUE;

	kr = ipc_right_lookup_write(space, name, &entry);
	if (kr != KERN_SUCCESS)
		return kr;
	/* space is write-locked and active */

	kr = ipc_right_info(space, name, entry, &type, &urefs);	/* unlocks */
	if (kr != KERN_SUCCESS)
		return kr;	/* space is unlocked */
	is_write_unlock(space);

	if (type & MACH_PORT_TYPE(right))
		switch (right) {
		    case MACH_PORT_RIGHT_SEND_ONCE:
			assert(urefs == 1);
			/* fall-through */

		    case MACH_PORT_RIGHT_PORT_SET:
		    case MACH_PORT_RIGHT_RECEIVE:
			*urefsp = 1;
			break;

		    case MACH_PORT_RIGHT_DEAD_NAME:
		    case MACH_PORT_RIGHT_SEND:
			assert(urefs > 0);
			*urefsp = urefs;
			break;

		    default:
			panic("mach_port_get_refs: strange rights");
		}
	else
		*urefsp = 0;

	return kr;
}

/*
 *	Routine:	mach_port_mod_refs
 *	Purpose:
 *		Modifies the number of user references held by a right.
 *		The resulting number of user references must be non-negative.
 *		If it is zero, the right is deallocated.  If the name
 *		doesn't denote other rights, it is destroyed.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		KERN_SUCCESS		Modified number of urefs.
 *		KERN_INVALID_TASK	The space is null.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_INVALID_VALUE	"right" isn't a legal value.
 *		KERN_INVALID_NAME	The name doesn't denote a right.
 *		KERN_INVALID_RIGHT	Name doesn't denote specified right.
 *		KERN_INVALID_VALUE	Impossible modification to urefs.
 *		KERN_UREFS_OVERFLOW	Urefs would overflow.
 */

kern_return_t
mach_port_mod_refs(
	ipc_space_t		space,
	mach_port_t		name,
	mach_port_right_t	right,
	mach_port_delta_t	delta)
{
	ipc_entry_t entry;
	kern_return_t kr;

	if (space == IS_NULL)
		return KERN_INVALID_TASK;

	if (right >= MACH_PORT_RIGHT_NUMBER)
		return KERN_INVALID_VALUE;

	kr = ipc_right_lookup_write(space, name, &entry);
	if (kr != KERN_SUCCESS)
		return kr;
	/* space is write-locked and active */

	kr = ipc_right_delta(space, name, entry, right, delta);	/* unlocks */
	return kr;
}


/*
 *	Routine:	mach_port_set_mscount [kernel call]
 *	Purpose:
 *		Changes a receive right's make-send count.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		KERN_SUCCESS		Set make-send count.
 *		KERN_INVALID_TASK	The space is null.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_INVALID_NAME	The name doesn't denote a right.
 *		KERN_INVALID_RIGHT	Name doesn't denote receive rights.
 */

kern_return_t
mach_port_set_mscount(
	ipc_space_t		space,
	mach_port_t		name,
	mach_port_mscount_t	mscount)
{
	ipc_port_t port;
	kern_return_t kr;

	if (space == IS_NULL)
		return KERN_INVALID_TASK;

	kr = ipc_port_translate_receive(space, name, &port);
	if (kr != KERN_SUCCESS)
		return kr;
	/* port is locked and active */

	ipc_port_set_mscount(port, mscount);

	ip_unlock(port);
	return KERN_SUCCESS;
}

/*
 *	Routine:	mach_port_set_seqno [kernel call]
 *	Purpose:
 *		Changes a receive right's sequence number.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		KERN_SUCCESS		Set sequence number.
 *		KERN_INVALID_TASK	The space is null.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_INVALID_NAME	The name doesn't denote a right.
 *		KERN_INVALID_RIGHT	Name doesn't denote receive rights.
 */

kern_return_t
mach_port_set_seqno(
	ipc_space_t		space,
	mach_port_t		name,
	mach_port_seqno_t	seqno)
{
	ipc_port_t port;
	kern_return_t kr;

	if (space == IS_NULL)
		return KERN_INVALID_TASK;

	kr = ipc_port_translate_receive(space, name, &port);
	if (kr != KERN_SUCCESS)
		return kr;
	/* port is locked and active */

	ipc_port_set_seqno(port, seqno);

	ip_unlock(port);
	return KERN_SUCCESS;
}

/*
 *	Routine:	mach_port_gst_helper
 *	Purpose:
 *		A helper function for mach_port_get_set_status.
 */

void
mach_port_gst_helper(
	ipc_pset_t		pset,
	ipc_port_t		port,
	ipc_entry_num_t		maxnames,
	mach_port_t		*names,
	ipc_entry_num_t		*actualp)
{
	ipc_pset_t ip_pset;
	mach_port_t name;

	assert(port != IP_NULL);

	ip_lock(port);
	assert(ip_active(port));

	name = port->ip_receiver_name;
	assert(name != MACH_PORT_NULL);
	ip_pset = port->ip_pset;

	ip_unlock(port);

	if (pset == ip_pset) {
		ipc_entry_num_t actual = *actualp;

		if (actual < maxnames)
			names[actual] = name;

		*actualp = actual+1;
	}
}

/*
 *	Routine:	mach_port_get_set_status [kernel call]
 *	Purpose:
 *		Retrieves a list of members in a port set.
 *		Returns the space's name for each receive right member.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		KERN_SUCCESS		Retrieved list of members.
 *		KERN_INVALID_TASK	The space is null.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_INVALID_NAME	The name doesn't denote a right.
 *		KERN_INVALID_RIGHT	Name doesn't denote a port set.
 *		KERN_RESOURCE_SHORTAGE	Couldn't allocate memory.
 */

kern_return_t
mach_port_get_set_status(
	ipc_space_t			space,
	mach_port_t			name,
	mach_port_t			**members,
	mach_msg_type_number_t		*membersCnt)
{
	ipc_entry_num_t actual;		/* this many members */
	ipc_entry_num_t maxnames;	/* space for this many members */
	kern_return_t kr;

	vm_size_t size;		/* size of allocated memory */
	vm_offset_t addr;	/* allocated memory */
	vm_map_copy_t memory;	/* copied-in memory */

	if (space == IS_NULL)
		return KERN_INVALID_TASK;

	size = PAGE_SIZE;	/* initial guess */

	for (;;) {
		ipc_tree_entry_t tentry;
		ipc_entry_t entry, table;
		ipc_entry_num_t tsize;
		mach_port_index_t index;
		mach_port_t *names;
		ipc_pset_t pset;

		kr = vm_allocate(ipc_kernel_map, &addr, size, TRUE);
		if (kr != KERN_SUCCESS)
			return KERN_RESOURCE_SHORTAGE;

		/* can't fault while we hold locks */

		kr = vm_map_wire(ipc_kernel_map, addr, addr + size,
				     VM_PROT_READ|VM_PROT_WRITE, FALSE);
		assert(kr == KERN_SUCCESS);

		kr = ipc_right_lookup_read(space, name, &entry);
		if (kr != KERN_SUCCESS) {
			kmem_free(ipc_kernel_map, addr, size);
			return kr;
		}
		/* space is read-locked and active */

		if (IE_BITS_TYPE(entry->ie_bits) != MACH_PORT_TYPE_PORT_SET) {
			is_read_unlock(space);
			kmem_free(ipc_kernel_map, addr, size);
			return KERN_INVALID_RIGHT;
		}

		pset = (ipc_pset_t) entry->ie_object;
		assert(pset != IPS_NULL);
		/* the port set must be active */

		names = (mach_port_t *) addr;
		maxnames = size / sizeof(mach_port_t);
		actual = 0;

		table = space->is_table;
		tsize = space->is_table_size;

		for (index = 0; index < tsize; index++) {
			ipc_entry_t ientry = &table[index];
			ipc_entry_bits_t bits = ientry->ie_bits;

			if (bits & MACH_PORT_TYPE_RECEIVE) {
				ipc_port_t port =
					(ipc_port_t) ientry->ie_object;

				mach_port_gst_helper(pset, port,
						     maxnames,
						     names, &actual);
			}
		}

		for (tentry = ipc_splay_traverse_start(&space->is_tree);
		     tentry != ITE_NULL;
		     tentry = ipc_splay_traverse_next(&space->is_tree,
			FALSE)) {
			ipc_entry_bits_t bits = tentry->ite_bits;

			assert(IE_BITS_TYPE(bits) !=
				MACH_PORT_TYPE_NONE);

			if (bits & MACH_PORT_TYPE_RECEIVE) {
				ipc_port_t port =
					(ipc_port_t) tentry->ite_object;

				mach_port_gst_helper(pset, port,
						     maxnames,
						     names, &actual);
			}
		}
		ipc_splay_traverse_finish(&space->is_tree);
		is_read_unlock(space);

		if (actual <= maxnames)
			break;

		/* didn't have enough memory; allocate more */

		kmem_free(ipc_kernel_map, addr, size);
		size = round_page(actual * sizeof(mach_port_t)) + PAGE_SIZE;
	}

	if (actual == 0) {
		memory = VM_MAP_COPY_NULL;

		kmem_free(ipc_kernel_map, addr, size);
	} else {
		vm_size_t size_used;
		vm_size_t vm_size_used;

		size_used = actual * sizeof(mach_port_t);
		vm_size_used = round_page(size_used);

		/*
		 *	Make used memory pageable and get it into
		 *	copied-in form.  Free any unused memory.
		 */

		kr = vm_map_unwire(ipc_kernel_map,
				     addr, addr + vm_size_used, FALSE);
		assert(kr == KERN_SUCCESS);

		kr = vm_map_copyin(ipc_kernel_map, addr, size_used,
				   TRUE, &memory);
		assert(kr == KERN_SUCCESS);

		if (vm_size_used != size)
			kmem_free(ipc_kernel_map,
				  addr + vm_size_used, size - vm_size_used);
	}

	*members = (mach_port_t *) memory;
	*membersCnt = actual;
	return KERN_SUCCESS;
}

/*
 *	Routine:	mach_port_move_member [kernel call]
 *	Purpose:
 *		If after is MACH_PORT_NULL, removes member
 *		from the port set it is in.  Otherwise, adds
 *		member to after, removing it from any set
 *		it might already be in.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		KERN_SUCCESS		Moved the port.
 *		KERN_INVALID_TASK	The space is null.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_INVALID_NAME	Member didn't denote a right.
 *		KERN_INVALID_RIGHT	Member didn't denote a receive right.
 *		KERN_INVALID_NAME	After didn't denote a right.
 *		KERN_INVALID_RIGHT	After didn't denote a port set right.
 *		KERN_NOT_IN_SET
 *			After is MACH_PORT_NULL and Member isn't in a port set.
 */

kern_return_t
mach_port_move_member(
	ipc_space_t	space,
	mach_port_t	member,
	mach_port_t	after)
{
	ipc_entry_t entry;
	ipc_port_t port;
	ipc_pset_t nset;
	kern_return_t kr;

	if (space == IS_NULL)
		return KERN_INVALID_TASK;

	kr = ipc_right_lookup_read(space, member, &entry);
	if (kr != KERN_SUCCESS)
		return kr;
	/* space is read-locked and active */

	if ((entry->ie_bits & MACH_PORT_TYPE_RECEIVE) == 0) {
		is_read_unlock(space);
		return KERN_INVALID_RIGHT;
	}

	port = (ipc_port_t) entry->ie_object;
	assert(port != IP_NULL);

	if (after == MACH_PORT_NULL)
		nset = IPS_NULL;
	else {
		entry = ipc_entry_lookup(space, after);
		if (entry == IE_NULL) {
			is_read_unlock(space);
			return KERN_INVALID_NAME;
		}

		if ((entry->ie_bits & MACH_PORT_TYPE_PORT_SET) == 0) {
			is_read_unlock(space);
			return KERN_INVALID_RIGHT;
		}

		nset = (ipc_pset_t) entry->ie_object;
		assert(nset != IPS_NULL);
	}

	kr = ipc_pset_move(space, port, nset);
	/* space is unlocked */
	return kr;
}

/*
 *	Routine:	mach_port_request_notification [kernel call]
 *	Purpose:
 *		Requests a notification.  The caller supplies
 *		a send-once right for the notification to use,
 *		and the call returns the previously registered
 *		send-once right, if any.  Possible types:
 *
 *		MACH_NOTIFY_PORT_DESTROYED
 *			Requests a port-destroyed notification
 *			for a receive right.  Sync should be zero.
 *		MACH_NOTIFY_NO_SENDERS
 *			Requests a no-senders notification for a
 *			receive right.  If there are currently no
 *			senders, sync is less than or equal to the
 *			current make-send count, and a send-once right
 *			is supplied, then an immediate no-senders
 *			notification is generated.
 *		MACH_NOTIFY_DEAD_NAME
 *			Requests a dead-name notification for a send
 *			or receive right.  If the name is already a
 *			dead name, sync is non-zero, and a send-once
 *			right is supplied, then an immediate dead-name
 *			notification is generated.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		KERN_SUCCESS		Requested a notification.
 *		KERN_INVALID_TASK	The space is null.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_INVALID_VALUE	Bad id value.
 *		KERN_INVALID_NAME	Name doesn't denote a right.
 *		KERN_INVALID_RIGHT	Name doesn't denote appropriate right.
 *		KERN_INVALID_CAPABILITY	The notify port is dead.
 *	MACH_NOTIFY_PORT_DESTROYED:
 *		KERN_INVALID_VALUE	Sync isn't zero.
 *	MACH_NOTIFY_DEAD_NAME:
 *		KERN_RESOURCE_SHORTAGE	Couldn't allocate memory.
 *		KERN_INVALID_ARGUMENT	Name denotes dead name, but
 *			sync is zero or notify is IP_NULL.
 *		KERN_UREFS_OVERFLOW	Name denotes dead name, but
 *			generating immediate notif. would overflow urefs.
 */

kern_return_t
mach_port_request_notification(
	ipc_space_t		space,
	mach_port_t		name,
	mach_msg_id_t		id,
	mach_port_mscount_t	sync,
	ipc_port_t		notify,
	ipc_port_t		*previousp)
{
	kern_return_t kr;

	if (space == IS_NULL)
		return KERN_INVALID_TASK;

	if (notify == IP_DEAD)
		return KERN_INVALID_CAPABILITY;

	switch (id) {
	    case MACH_NOTIFY_PORT_DESTROYED: {
		ipc_port_t port, previous;

		if (sync != 0)
			return KERN_INVALID_VALUE;

		kr = ipc_port_translate_receive(space, name, &port);
		if (kr != KERN_SUCCESS)
			return kr;
		/* port is locked and active */

		ipc_port_pdrequest(port, notify, &previous);
		/* port is unlocked */

		*previousp = previous;
		break;
	    }

	    case MACH_NOTIFY_NO_SENDERS: {
		ipc_port_t port;

		kr = ipc_port_translate_receive(space, name, &port);
		if (kr != KERN_SUCCESS)
			return kr;
		/* port is locked and active */

		ipc_port_nsrequest(port, sync, notify, previousp);
		/* port is unlocked */
		break;
	    }

	    case MACH_NOTIFY_DEAD_NAME:
		kr = ipc_right_dnrequest(space, name, sync != 0,
					 notify, previousp);
		if (kr != KERN_SUCCESS)
			return kr;
		break;

	    default:
		return KERN_INVALID_VALUE;
	}

	return KERN_SUCCESS;
}

/*
 *	Routine:	mach_port_insert_right [kernel call]
 *	Purpose:
 *		Inserts a right into a space, as if the space
 *		voluntarily received the right in a message,
 *		except that the right gets the specified name.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		KERN_SUCCESS		Inserted the right.
 *		KERN_INVALID_TASK	The space is null.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_INVALID_VALUE	The name isn't a legal name.
 *		KERN_NAME_EXISTS	The name already denotes a right.
 *		KERN_INVALID_VALUE	Message doesn't carry a port right.
 *		KERN_INVALID_CAPABILITY	Port is null or dead.
 *		KERN_UREFS_OVERFLOW	Urefs limit would be exceeded.
 *		KERN_RIGHT_EXISTS	Space has rights under another name.
 *		KERN_RESOURCE_SHORTAGE	Couldn't allocate memory.
 */

kern_return_t
mach_port_insert_right(
	ipc_space_t		space,
	mach_port_t		name,
	ipc_port_t		poly,
	mach_msg_type_name_t	polyPoly)
{
	if (space == IS_NULL)
		return KERN_INVALID_TASK;

	if (!MACH_PORT_VALID(name) ||
	    !MACH_MSG_TYPE_PORT_ANY_RIGHT(polyPoly))
		return KERN_INVALID_VALUE;

	if (!IO_VALID((ipc_object_t) poly))
		return KERN_INVALID_CAPABILITY;

	return ipc_object_copyout_name(space, (ipc_object_t) poly, 
				       polyPoly, FALSE, name);
}

/*
 *	Routine:	mach_port_extract_right [kernel call]
 *	Purpose:
 *		Extracts a right from a space, as if the space
 *		voluntarily sent the right to the caller.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		KERN_SUCCESS		Extracted the right.
 *		KERN_INVALID_TASK	The space is null.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_INVALID_VALUE	Requested type isn't a port right.
 *		KERN_INVALID_NAME	Name doesn't denote a right.
 *		KERN_INVALID_RIGHT	Name doesn't denote appropriate right.
 */

kern_return_t
mach_port_extract_right(
	ipc_space_t		space,
	mach_port_t		name,
	mach_msg_type_name_t	msgt_name,
	ipc_port_t		*poly,
	mach_msg_type_name_t	*polyPoly)
{
	kern_return_t kr;

	if (space == IS_NULL)
		return KERN_INVALID_TASK;

	if (!MACH_MSG_TYPE_PORT_ANY(msgt_name))
		return KERN_INVALID_VALUE;

	kr = ipc_object_copyin(space, name, msgt_name, (ipc_object_t *) poly);

	if (kr == KERN_SUCCESS)
		*polyPoly = ipc_object_copyin_type(msgt_name);
	return kr;
}


kern_return_t
mach_port_get_attributes(
	ipc_space_t		space,
	mach_port_t		name,
	int			flavor,
        mach_port_info_t	info,
        mach_msg_type_number_t	*count)
{
	ipc_port_t port;
	kern_return_t kr;

	if (space == IS_NULL)
		return KERN_INVALID_TASK;

        switch (flavor) {
        case MACH_PORT_LIMITS_INFO: {
                mach_port_limits_t *lp = (mach_port_limits_t *)info;

                if (*count < MACH_PORT_LIMITS_INFO_COUNT)
                        return KERN_FAILURE;
                
                kr = ipc_port_translate_receive(space, name, &port);
                if (kr != KERN_SUCCESS)
                        return kr;
                /* port is locked and active */

                lp->mpl_qlimit = port->ip_qlimit;
                *count = MACH_PORT_LIMITS_INFO_COUNT;
                ip_unlock(port);
                break;
        }

        case MACH_PORT_RECEIVE_STATUS: {
                mach_port_status_t *statusp = (mach_port_status_t *)info;
                
                if (*count < MACH_PORT_RECEIVE_STATUS_COUNT)
                        return KERN_FAILURE;
                
                kr = ipc_port_translate_receive(space, name, &port);
                if (kr != KERN_SUCCESS)
                        return kr;
                /* port is locked and active */

                if (port->ip_pset != IPS_NULL) {
                        ipc_pset_t pset = port->ip_pset;

                        ips_lock(pset);
                        if (!ips_active(pset)) {
                                ipc_pset_remove(pset, port);
                                ips_check_unlock(pset);
                                goto no_port_set;
                        } else {
                                statusp->mps_pset = pset->ips_local_name;
                                imq_lock(&pset->ips_messages);
                                statusp->mps_seqno = port->ip_seqno;
                                imq_unlock(&pset->ips_messages);
                                ips_unlock(pset);
                                assert(MACH_PORT_VALID(statusp->mps_pset));
                        }
                } else {
                no_port_set:
                        statusp->mps_pset = MACH_PORT_NULL;
                        imq_lock(&port->ip_messages);
                        statusp->mps_seqno = port->ip_seqno;
                        imq_unlock(&port->ip_messages);
                }
                statusp->mps_mscount = port->ip_mscount;
                statusp->mps_qlimit = port->ip_qlimit;
                statusp->mps_msgcount = port->ip_msgcount;
                statusp->mps_sorights = port->ip_sorights;
                statusp->mps_srights = port->ip_srights > 0;
                statusp->mps_pdrequest = port->ip_pdrequest != IP_NULL;
                statusp->mps_nsrequest = port->ip_nsrequest != IP_NULL;
		statusp->mps_rt = IP_RT(port);
                *count = MACH_PORT_RECEIVE_STATUS_COUNT;
                ip_unlock(port);
                break;
        }
	
	case MACH_PORT_DNREQUESTS_SIZE: {
		ipc_port_request_t	table;
		
                if (*count < MACH_PORT_DNREQUESTS_SIZE_COUNT)
                        return KERN_FAILURE;
		
                kr = ipc_port_translate_receive(space, name, &port);
                if (kr != KERN_SUCCESS)
                        return kr;
                /* port is locked and active */
		
		table = port->ip_dnrequests;
		if (table == IPR_NULL)
			*(int *)info = 0;
		else
			*(int *)info = table->ipr_size->its_size;
                *count = MACH_PORT_DNREQUESTS_SIZE_COUNT;
                ip_unlock(port);
		break;
	}

        default:
		return KERN_INVALID_ARGUMENT;
                /*NOTREACHED*/
        }                

	return KERN_SUCCESS;
}

kern_return_t
mach_port_set_attributes(
	ipc_space_t		space,
	mach_port_t		name,
	int			flavor,
        mach_port_info_t	info,
        mach_msg_type_number_t	count)
{
	ipc_port_t port;
	kern_return_t kr;
        
	if (space == IS_NULL)
		return KERN_INVALID_TASK;

        switch (flavor) {
                
        case MACH_PORT_LIMITS_INFO: {
                mach_port_limits_t *mplp = (mach_port_limits_t *)info;
                
                if (count < MACH_PORT_LIMITS_INFO_COUNT)
                        return KERN_FAILURE;
                
                if (mplp->mpl_qlimit > MACH_PORT_QLIMIT_MAX)
                        return KERN_INVALID_VALUE;

                kr = ipc_port_translate_receive(space, name, &port);
                if (kr != KERN_SUCCESS)
                        return kr;
                /* port is locked and active */

                ipc_port_set_qlimit(port, mplp->mpl_qlimit);
                ip_unlock(port);
                break;
        }
	case MACH_PORT_DNREQUESTS_SIZE: {
                if (count < MACH_PORT_DNREQUESTS_SIZE_COUNT)
                        return KERN_FAILURE;
                
                kr = ipc_port_translate_receive(space, name, &port);
                if (kr != KERN_SUCCESS)
                        return kr;
                /* port is locked and active */
		
		kr = ipc_port_dngrow(port, *(int *)info);
		if (kr != KERN_SUCCESS)
			return kr;
		break;
	}
        default:
		return KERN_INVALID_ARGUMENT;
                /*NOTREACHED*/
        }
	return KERN_SUCCESS;
}


/*
 * Create an empty thread_activation (sans thread_shuttle) attached to
 * a port or port set in a given task to receive incoming threads.
 */
/*
 * NOTE: the following calls targeted at a thread_act port may be
 * called on an empty thread_act:
 *
 * thread_get_exception_ports  thread_set_exception_ports
 * thread_get_special_port     thread_set_special_port
 * thread_get_state            thread_suspend
 * thread_resume               thread_swap_exception_ports
 * thread_sample               thread_terminate
 *
 * The following calls targeted at a thread_act port may _not_ be
 * called on an empty thread_act (and will return KERN_INVALID_ARGUMENT
 * if they are called with one):
 *
 * thread_abort                thread_info
 * thread_abort_safely         thread_wire
 * thread_depress_abort
 *
 * Also, if thread_switch() is called with an empty thread_act as
 * its first argument, the argument will be ignored (i.e., the
 * function will behave as if a zero-valued argument had been
 * given).
 */
kern_return_t
thread_activation_create(task_t task, mach_port_t name,
                         vm_offset_t user_stack, vm_size_t stack_size, 
			 thread_act_t *new_act)
{
	ipc_space_t space;
	ipc_object_t object;
	kern_return_t kr;
	thread_act_t thr_act;
	int is_port = 1;

	if (task == TASK_NULL)
		return KERN_INVALID_TASK;

	/* First create the new activation.  */
	/*
	 * We'll need this stack later -- make sure it's present.
	 */
	assert(user_stack != 0);
	kr = act_create(task, user_stack, stack_size, &thr_act);
	if (kr != KERN_SUCCESS)
		return kr;

	space = task->itk_space;

	kr = ipc_object_translate(space, name,
				  MACH_PORT_RIGHT_PORT_SET, &object);
	if (kr != KERN_SUCCESS) {
		kr = ipc_object_translate(space, name,
					  MACH_PORT_RIGHT_RECEIVE, &object);
		if (kr != KERN_SUCCESS) {
			thread_terminate(thr_act);
			act_deallocate(thr_act);
			return kr;
		}
		is_port = 0;
	}
	/* port/pset is locked and active */

#if	MACH_ASSERT
	if (watchacts & WA_PORT)
		printf("thr_act on %s=%x stack=%x thr_act=%x\n",
		       (is_port ? "port" : "pset"),
		       object, user_stack, thr_act);
#endif	/* MACH_ASSERT */

	/* Assign the activation to the thread_pool.  */
	kr = act_set_thread_pool(thr_act, (ipc_port_t)object);
	if (kr != KERN_SUCCESS) {
		io_unlock(object);
		thread_terminate(thr_act);
		act_deallocate(thr_act);
		return kr;
	}
	io_unlock(object);

#if	MACH_ASSERT
	if (watchacts & WA_PORT)
		printf("\tpool_port=%x thr_act=%x\n", (ipc_port_t)object, thr_act);
#endif	/* MACH_ASSERT */

	/* Pass our reference to the activation back to the user.  */
	*new_act = thr_act;

	return KERN_SUCCESS;
}

