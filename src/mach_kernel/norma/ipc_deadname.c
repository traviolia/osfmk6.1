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
 * Revision 2.1.1.3  92/09/15  17:33:22  jeffreyh
 * 	Merge norma_ipc_node_fproxy_cancel into norma_ipc_node_dnnotify.
 * 	Add release_forward logic to norma_ipc_notify_dead_name.
 * 	[92/09/15            dlb]
 * 	Change from dlb to return kern success on
 * 	norma_ipc_node_dnrequest if node = node_self
 * 	[92/08/25            jeffreyh]
 * 
 * 	Add norma_ipc_node_fproxy_cancel.
 * 	[92/08/07            dlb]
 * 
 * 	Add source node argument to norma_ipc_node_dnnotify.
 * 	This tells us where to return outstanding transits.
 * 	[92/08/06            dlb]
 * 
 * 	Added arguments to printf()
 * 	[92/07/14            sjs]
 * 
 * Revision 2.1.1.2  92/05/28  18:20:45  jeffreyh
 * 	Provide references for norma_ipc_dead_destination() to consume.
 * 	[92/05/28            sjs]
 * 
 * Revision 2.1.1.1  92/05/27  01:00:32  jeffreyh
 * 	Created.
 * 	[92/05/25            dlb]
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

#include <ipc/ipc_port.h>
#include <ipc/ipc_table.h>
#include <mach/kern_return.h>
#include <norma/norma_internal.h>
#include <norma/norma_internal_server.h>
#include <norma/ipc_node.h>
#include <kern/assert.h>
#include <kern/host.h>
#include <kern/misc_protos.h>
#include <norma/tr.h>

/*
 *	File:	norma/ipc_dnrequest.c
 *	Author:	David L. Black
 *	Date:	1992
 *
 *	Functions for managing NORMA IPC dead name requests.
 */

/*
 *	Dead name notifications have a two-level structure: NORMA
 *	code tracks which nodes have to be notified, and the proxy
 *	structures on those nodes track the actual notifications.
 *	The existence of a dead name notification causes destruction
 *	of the corresponding proxies to be eagerly evaluated.  A special
 *	value for the notify port is used to redirect dead name
 *	notifications for remote proxies into the norma system.
 */

/*
 *	norma_ipc_dnrequest_init:
 *
 *	Called by the ipc system the first time a dead name request
 *	is queued for a port.  If the port is a proxy, ask the principal
 *	to tell us when it dies.  Caller must hold a port reference.
 */

void
norma_ipc_dnrequest_init(
	ipc_port_t	port)
{
	kern_return_t result;

	if (!IP_NORMA_IS_PROXY(port))
		return;

	result = r_norma_ipc_node_dnrequest(
		remote_host_priv(port->ip_norma_dest_node,
				 MACH_MSG_TYPE_PORT_SEND),
		port->ip_norma_uid, node_self());

	if (result != KERN_SUCCESS) {
		/*
		 *	Principal is already dead, so kill this proxy.
		 *	Caller will discover port is dead when it
		 *	rechecks after return from ipc_port_dngrow();
		 */
		ip_lock(port);
		ip_reference(port);
		norma_ipc_dead_destination(port); /* consumes lock */
	}
}

/*
 *	norma_ipc_node_dnrequest:
 *
 *	Called on principal's node by a proxy that wants to be
 *	notified when the receive right dies (so that it can process
 *	its dead name requests).  Queue a norma fake dead name notification
 *	so that we get called at that time.
 */

kern_return_t
norma_ipc_node_dnrequest(
	host_t		host_priv,
	unsigned	uid,
	unsigned	node)
{
	ipc_port_t	port;
	ipc_port_request_index_t	junk_request;
	kern_return_t			kr;
	unsigned long			dest;
	TR_DECL("norma_ipc_node_dnrequest");

	if (host_priv == HOST_NULL)
		return(KERN_INVALID_ARGUMENT);

	port = norma_port_lookup(uid, FALSE);

	if (port != IP_NULL && !ip_active(port)) {
		ip_unlock(port);
		port = IP_NULL;
	}

        if (port == IP_NULL) {
		/*
		 *	The receive right is already dead.
		 *	However, we don't necessarily know
		 *	the receive right's root node, so
		 *	notify the caller but pass back a token
		 *	indicating the caller should use its
		 *	current idea of the root node.
		 */
		if (node != node_self()) {
	        	tr3("Stale dn request node 0x%x uid 0x%x",
			       node, uid);
			r_norma_ipc_node_dnnotify(
				remote_host_priv(node,
						 MACH_MSG_TYPE_PORT_SEND),
				uid, IP_NORMA_FAKE_NODE, FALSE);
		}
		return(KERN_SUCCESS);
	}

        if (port->ip_norma_is_proxy) {
                /*
                 * Send this along to the principal.  This is an
		 * asynchronous message.
                 */
		dest = port->ip_norma_dest_node;
		ip_unlock(port);
		r_norma_ipc_node_dnrequest(
				remote_host_priv(dest,
						 MACH_MSG_TYPE_PORT_SEND),
                                uid, node);
		return(KERN_SUCCESS);
	}

	/*
	 *	Place norma dnrequest.
	 */

	if (node == node_self()) {
		ip_unlock(port);
		return KERN_SUCCESS;
	}

        kr = ipc_port_dnrequest(port, NODE_TO_DNREQUEST_NAME(node),
				IP_NORMA_FAKE_DNREQUEST,
				&junk_request);
	while (kr != KERN_SUCCESS) {
		kr = ipc_port_dngrow(port, ITS_SIZE_NONE);
                if (kr != KERN_SUCCESS)
		    panic("norma_ipc_node_dnrequest: ipc_port_dngrow failure");

	        ip_lock(port);
		kr = ipc_port_dnrequest(port, NODE_TO_DNREQUEST_NAME(node),
					IP_NORMA_FAKE_DNREQUEST,
					&junk_request);
	}

	ip_unlock(port);
	return(KERN_SUCCESS);
}

/*
 *	norma_ipc_notify_dead_name:
 *
 *	A port is about to be destroyed, and has a norma dead name
 *	notification attached to it.  If it's a proxy, forward the
 *	deadname notification to the principal.  If it's a principal,
 *	tell the proxy that requested the notification to kill itself.
 *
 *	NOTE: This routine depends on norma_ipc_port_destroy NOT clobbering
 *		the port's uid.
 */

void
norma_ipc_notify_dead_name(
	ipc_port_t	port,
	unsigned long	name)
{
	register unsigned long	node = DNREQUEST_NAME_TO_NODE(name);

	if(IP_NORMA_IS_PROXY(port)) {
		r_norma_ipc_node_dnrequest(
			remote_host_priv(port->ip_norma_dest_node,
				MACH_MSG_TYPE_PORT_SEND),
			port->ip_norma_uid,
			node);
	}
	else {
		register boolean_t release_forward;

		/*
		 *	If the proxy on the node is a forwarding proxy,
		 *	the forwarding can be released if it's not the
		 *	root node for the port OR there are no send or
		 *	send once rights for the port out in the network.
		 */
		release_forward =
			((IP_NORMA_NODE(port->ip_norma_uid) != node) ||
			 ((port->ip_norma_stransit == 0) &&
			  (port->ip_norma_sotransit == 0)));

		r_norma_ipc_node_dnnotify(
			remote_host_priv(node, MACH_MSG_TYPE_PORT_SEND),
			port->ip_norma_uid,
			node_self(),
			release_forward);
	}
}

/*
 *	norma_ipc_node_dnnotify:
 *
 *	Called on proxy node when principal has died.  Kill proxy to
 *	trigger its dead name notifications.  Proxy may already be
 *	gone for other reasons, so just ignore this if we can't find one.
 */
kern_return_t
norma_ipc_node_dnnotify(
	host_t		host_priv,
	unsigned	uid,
	unsigned	source_node,
	boolean_t	release_forward)
{
	ipc_port_t port;

	port = norma_port_lookup(uid, FALSE);

	/*
	 *	Ignore this if we can't find the proxy.  If the
	 *	port is queued, the send will fail and destroy the
	 *	proxy anyway; destroying it prematurely will just
	 *	confuse the send logic.  Reset the dest node so that
	 *	yield transit calls go to the right place.
	 */
	if (port == IP_NULL)
		return KERN_SUCCESS;

	if (!IP_NORMA_IS_PROXY(port)) {
		assert(!ip_active(port));
		ip_unlock(port);
		return KERN_SUCCESS;
	}

	if (release_forward) {
		assert((IP_NORMA_NODE(port->ip_norma_uid) != node_self()) ||
			 ((port->ip_norma_stransit == 0) &&
			  (port->ip_norma_sotransit == 0)));
		port->ip_norma_forward = FALSE;
	}

	if (!port->ip_norma_queued) {
		ip_reference(port);
		if (source_node != IP_NORMA_FAKE_NODE)
			port->ip_norma_dest_node = source_node;
		norma_ipc_dead_destination(port); /* consumes lock */
	} else
		ip_unlock(port);

	return KERN_SUCCESS;
}
