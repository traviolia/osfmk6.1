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
 * Revision 2.3.2.10  92/09/15  17:34:30  jeffreyh
 * 	Moved node_is_valid macro to ipc_node.h
 * 	[92/07/23            jeffreyh]
 * 
 * Revision 2.3.2.9  92/06/24  18:05:56  jeffreyh
 * 	Initialize NORMA_HOST_PAGING_PORT.
 * 	[92/06/17            jeffreyh]
 * 
 * 	Also init sotransit for special proxy.
 * 	[92/06/01            dlb]
 * 
 * Revision 2.3.2.8  92/05/28  18:19:41  jeffreyh
 * 	Add logic to handle send once rights for special ports.
 * 	[92/05/28            dlb]
 * 
 * 	remote_special has logic to increment the reference count when it
 * 	receives sorights.
 * 	[92/05/28            sjs]
 * 
 * Revision 2.3.2.7  92/04/09  14:25:40  jeffreyh
 * 	Use NORMA_ETHER instead of iPSC checks to decide
 * 	 what node_is_valid define to use. 
 * 
 * Revision 2.3.2.6  92/04/08  16:17:07  jeffreyh
 * 	Fix ifdef
 * 
 * Revision 2.3.2.5  92/04/08  15:46:15  jeffreyh
 * 	Added bounds check for node number in norma_get_special_port
 * 	[92/04/08            sjs]
 * 
 * 	Add validity check to node argument of norma_get_special_port.
 * 	[92/04/07            dlb]
 * 
 * 	Add consume on success logic and fix error check in
 * 	norma_port_location_hint.  Remove ipc_port_node.
 * 	[92/04/03            dlb]
 * 
 * Revision 2.3.2.4  92/02/18  19:16:34  jeffreyh
 * 	Use IP_NORMA_IS_PROXY macro instead of checking for norma_uid
 * 	in norma_port_location_hint.  The latter gives the wrong
 * 	answer if the port has been exported and we're not node 0.
 * 	[92/02/14            dlb]
 * 
 * Revision 2.3.2.3  92/01/21  21:52:33  jsb
 * 	More de-linting.
 * 	[92/01/17  11:41:55  jsb]
 * 
 * 	De-linted.
 * 	[92/01/16  22:14:38  jsb]
 * 
 * Revision 2.3.2.2  92/01/09  18:45:51  jsb
 * 	Removed placeholder logic.
 * 	Added remote_{special,device,host,host_priv} routines.
 * 	[92/01/04  18:35:52  jsb]
 * 
 * Revision 2.3.2.1  92/01/03  16:37:58  jsb
 * 	Corrected log. Removed norma_token_startup call. Fixed type clash.
 * 	[91/12/24  14:26:15  jsb]
 * 
 * Revision 2.2  91/11/14  16:48:12  rpd
 * 	Moved from norma/ipc_xxx.c.
 * 
 * Revision 2.5  91/08/28  11:16:15  jsb
 * 	Renamed clport things to norma things.
 * 	Eliminated NORMA_ETHER special case node/lid uid division.
 * 	[91/08/15  09:14:06  jsb]
 * 
 * 	Renamed ipc_clport_foo things to norma_ipc_foo.
 * 	Added host_port_at_node().
 * 	[91/08/14  19:21:16  jsb]
 * 
 * Revision 2.4  91/08/03  18:19:34  jsb
 * 	Conditionalized printfs.
 * 	[91/08/01  22:55:17  jsb]
 * 
 * 	Eliminated dynamically allocated clport structures; this information
 * 	is now stored in ports directly. This simplifies issues of allocation
 * 	at interrupt time.
 * 
 * 	Added definitions for IP_CLPORT_{NODE,LID} appropriate for using the
 * 	low two bytes of an internet address as a node number.
 * 
 * 	Revised norma_special_port mechanism to better handle port death
 * 	and port replacement. Eliminated redundant special port calls.
 * 	Cleaned up associated startup code.
 * 	[91/07/25  19:09:16  jsb]
 * 
 * Revision 2.3  91/07/01  08:25:52  jsb
 * 	Some locking changes.
 * 	[91/06/29  15:10:46  jsb]
 * 
 * Revision 2.2  91/06/17  15:48:05  jsb
 * 	Moved here from ipc/ipc_clport.c.
 * 	[91/06/17  11:07:25  jsb]
 * 
 * Revision 2.7  91/06/06  17:05:35  jsb
 * 	Added norma_get_special_port, norma_set_special_port.
 * 	[91/05/25  10:28:14  jsb]
 * 
 * 	Much code moved to other norma/ files.
 * 	[91/05/13  17:16:24  jsb]
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
/*
 *	File:	norma/ipc_special.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1990
 *
 *	Functions to support norma special ports.
 */

#include <norma_vm.h>
#include <norma_ipc.h>
#include <norma_ether.h>
#include <dynamic_num_nodes.h>

#include <vm/vm_kern.h>
#include <mach/vm_param.h>
#include <mach/port.h>
#include <mach/message.h>
#include <mach/mach_server.h>	/* for vm_set_default_memory_manager */
#include <mach/norma_special_ports.h>
#include <mach/mach_norma_server.h>
#include <mach/mach_norma.h>
#include <kern/assert.h>
#include <kern/host.h>
#include <kern/sched_prim.h>
#include <kern/ipc_sched.h>
#include <kern/ipc_kobject.h>
#include <kern/zalloc.h>
#include <kern/misc_protos.h>
#include <device/device_port.h>
#include <ipc/ipc_mqueue.h>
#include <ipc/ipc_thread.h>
#include <ipc/ipc_kmsg.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_pset.h>
#include <ipc/ipc_space.h>
#include <norma/ipc_node.h>
#include <norma/tr.h>

ipc_port_t host_special_port_array[MAX_SPECIAL_ID];
#define host_special_port(id) (host_special_port_array[id])
spinlock_t host_special_port_lock;

/*
 * The first three special ports are kernel owned.
 * They are DEVICE, HOST, and HOST_PRIV.
 * The kernel has receive rights to these ports.
 * Users are not allowed to change these ports.
 * No-more senders notifications are not requested for these ports.
 */

void
norma_ipc_init(void)
{
#if     DYNAMIC_NUM_NODES
	/*
	 * dynamicaic allocation of NORMA Protocol Control Structures
	 */
	alloc_norma_control_structures();
#endif
	/*
	 * Register master device, host, and host_priv ports.
	 */
	assert(master_device_port != IP_NULL);
	master_device_port->ip_norma_is_special = TRUE;
	host_special_port(NORMA_DEVICE_PORT) = master_device_port;

	assert(realhost.host_self != IP_NULL);
	realhost.host_self->ip_norma_is_special = TRUE;
	host_special_port(NORMA_HOST_PORT) = realhost.host_self;

	assert(realhost.host_priv_self != IP_NULL);
	realhost.host_priv_self->ip_norma_is_special = TRUE;
	host_special_port(NORMA_HOST_PRIV_PORT) = realhost.host_priv_self;

	spinlock_init(&host_special_port_lock);

	/*
	 * Initialize trace module
	 */
	TR_INIT();

	/*
	 * Initialize network subsystem
	 */
	netipc_init();

	/*
	 * initialize the norma uid pool
	 */
	norma_uid_array_init();

	/*
	 * Initialize transit module.
	 */
	norma_transit_yield_init();

	/*
	 * Start up netipc thread, kserver module, and token module.
	 * XXX netipc_init should do the former...
	 */
	(void) kernel_thread(kernel_task, netipc_thread, (char *) 0);
	norma_kserver_startup();
}

/*
 *	Return likely location to find port's receiver.
 */

kern_return_t
norma_port_location_hint(
	task_t		task,
	ipc_port_t	port,
	int		*node)
{
	ip_lock(port);
	if (!IP_VALID(port)) {
		ip_unlock(port);
		return KERN_INVALID_ARGUMENT;
	}
	if (IP_NORMA_IS_PROXY(port)) {
		*node = port->ip_norma_dest_node;
	} else {
		*node = node_self();
	}

	ip_unlock(port);
	ipc_port_release_send(port);

	return KERN_SUCCESS;
}

kern_return_t
norma_set_special_port(
	host_t		host,
	int		id,
	ipc_port_t	port)
{
	ipc_port_t old;

	if (host == HOST_NULL) {
		return KERN_INVALID_HOST;
	}
	if (id <= 0 || id > MAX_SPECIAL_ID) {
		return KERN_INVALID_VALUE;
	}
	if (id == NORMA_HOST_PAGING_PORT) {
	  	kern_return_t ret;
		old = port;
		ret = vm_set_default_memory_manager(host, &old);
		if (ret != KERN_SUCCESS)
			return ret;
	} else 	if (id <= MAX_SPECIAL_KERNEL_ID) {
		/* these never change */
		return KERN_INVALID_VALUE;
	} else {
		ip_lock(port);
		if (!IP_VALID(port)) {
			ip_unlock(port);
			return KERN_INVALID_ARGUMENT;
		}
		spinlock_lock(&host_special_port_lock);
		old = host_special_port(id);
		host_special_port(id) = port;
		port->ip_norma_is_special = TRUE;
		spinlock_unlock(&host_special_port_lock);
		ip_unlock(port);
	}
	if (old != IP_NULL) {
		ip_lock(old);
		if (IP_VALID(old)) {
			ip_unlock(old);
			fret("special id #%d replaced\n", id);
			ipc_port_release_send(old);
		} else
			ip_unlock(old);
	}
	return KERN_SUCCESS;
}

/*
 * Internally called when port is being destroyed.
 * XXX
 * Is it also called when proxy is destroyed???
 */
void
norma_unset_special_port(
	ipc_port_t	port)
{
	unsigned long id;

	ip_lock(port);
	port->ip_norma_is_special = FALSE;
	ip_unlock(port);

	spinlock_lock(&host_special_port_lock);
	for (id = 1; id <= MAX_SPECIAL_ID; id++) {
		if (host_special_port(id) == port) {
			assert(id >= MAX_SPECIAL_KERNEL_ID);
			fret("special id #%d died\n", id);
			host_special_port(id) = IP_NULL;
			spinlock_unlock(&host_special_port_lock);
			ipc_port_release_send(port);
			spinlock_lock(&host_special_port_lock);
		}
	}
	spinlock_unlock(&host_special_port_lock);
}

ipc_port_t
local_special(
	unsigned long		id,
	mach_msg_type_name_t	type)
{
	ipc_port_t port;

	assert(IP_NORMA_SPECIAL(id));
	assert(id > 0 && id <= MAX_SPECIAL_KERNEL_ID);
	spinlock_lock(&host_special_port_lock);
	if ((port = host_special_port(id)) == IP_NULL) {
		spinlock_unlock(&host_special_port_lock);
		return(IP_NULL);
	}
	spinlock_unlock(&host_special_port_lock);
	if (type == MACH_MSG_TYPE_PORT_SEND)
		return ipc_port_make_send(port);

	assert(type == MACH_MSG_TYPE_PORT_SEND_ONCE);
	return ipc_port_make_sonce(port);
}

ipc_port_t
remote_special(
	unsigned long		node,
	unsigned long		id,
	mach_msg_type_name_t	type)
{
	ipc_port_t port;
	unsigned long uid = IP_NORMA_UID(node, id);

	assert(node != node_self());
	assert(id > 0 && id <= MAX_SPECIAL_KERNEL_ID);
	port = norma_port_lookup(uid, TRUE);
	if (port == IP_NULL) {
		port = ipc_port_alloc_special(ipc_space_remote);
		if (port == IP_NULL) {
			panic("remote_special: ipc_port_alloc_special");
		}
		port->ip_nsrequest = ip_nsproxym(port);
		if (type == MACH_MSG_TYPE_PORT_SEND) {
			port->ip_srights = 1;
		}
		else {
			assert(type == MACH_MSG_TYPE_PORT_SEND_ONCE);
			port->ip_sorights = 1;
		}	
		port->ip_norma_uid = uid;
		port->ip_norma_dest_node = node;
		port->ip_norma_is_proxy = TRUE;
		ip_lock(port);
		norma_port_insert(port, TRUE);
	} else {
		if (type == MACH_MSG_TYPE_PORT_SEND) {
			port->ip_srights++;
		}
		else {
			assert(type == MACH_MSG_TYPE_PORT_SEND_ONCE);
			port->ip_sorights++;
		}
		port->ip_references++;
	}
	port->ip_norma_stransit = -0x70000000;	/* XXX */
	port->ip_norma_sotransit = -0x70000000;	/* XXX */
	ip_unlock(port);
	return port;
}

ipc_port_t
remote_device(
	unsigned long		node,
	mach_msg_type_name_t	type)
{
	return remote_special(node, (unsigned long) NORMA_DEVICE_PORT, type);
}

ipc_port_t
remote_host(
	unsigned long		node,
	mach_msg_type_name_t	type)
{
	return remote_special(node, (unsigned long) NORMA_HOST_PORT, type);
}

ipc_port_t
remote_host_priv(
	unsigned long		node,
	mach_msg_type_name_t	type)
{
	return remote_special(node, (unsigned long) NORMA_HOST_PRIV_PORT,
					type);
}

/*
 * XXX
 * The blocking behavior of this call is now somewhat funny.
 * A remote call will block until the remote node comes up.
 * However, in either local or remote case, a null port may be returned;
 * that is, we do not block for a valid port to show up.
 */
kern_return_t
norma_get_special_port(
	host_t		host,
	int		node,
	int		id,
	ipc_port_t	*portp)
{
	if (host == HOST_NULL) {
		return KERN_INVALID_HOST;
	}
	if (id <= 0 || id > MAX_SPECIAL_ID || !node_is_valid (node)) {
		return KERN_INVALID_ARGUMENT;
	}
	if (id == NORMA_HOST_PAGING_PORT && node == node_self()) {
		*portp = MACH_PORT_NULL;
		return vm_set_default_memory_manager(host, portp);
	} else if (id <= MAX_SPECIAL_KERNEL_ID
		   && id != NORMA_HOST_PAGING_PORT) {
		if (node == node_self()) {
			*portp = local_special(id, MACH_MSG_TYPE_PORT_SEND);
		} else {
			*portp = remote_special(node, id,
					        MACH_MSG_TYPE_PORT_SEND);
		}
	} else {
		if (node == node_self()) {
			spinlock_lock(&host_special_port_lock);
			*portp = host_special_port(id);
			spinlock_unlock(&host_special_port_lock);
			*portp = ipc_port_copy_send(*portp);
		} else {
			return r_norma_get_special_port(
			        remote_host_priv(node,MACH_MSG_TYPE_PORT_SEND),
			        node, id, portp);
		}
	}
	return KERN_SUCCESS;
}
