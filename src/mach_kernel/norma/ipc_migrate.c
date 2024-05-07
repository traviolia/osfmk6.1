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
 * Revision 2.5.2.10  92/09/15  17:33:46  jeffreyh
 * 	Remove all dnrequest logic from source side of migrate. Norma
 * 	deadname requests are now managed via lazy evaluation.
 * 	[92/09/14            dlb]
 * 	norma_port_insert now handles uid generation.
 * 	[92/08/06            dlb]
 * 
 * 	Remove has_dnrequest logic.  Destination of receive right must
 * 	ALWAYS notify source when right dies so forwarding proxy can be
 * 	cleaned up.  Set ip_norma_forward in all cases.
 * 	[92/08/06            dlb]
 * 
 * 	Added port parameter for new uid scheme
 * 	[92/07/30            sjs]
 * 
 * 	Fix arguments to norma_ipc_node_dnrequest
 * 	[92/07/14            sjs]
 * 
 * Revision 2.5.2.9  92/06/24  18:05:40  jeffreyh
 * 	Change argument to r_norma_ipc_pull_receive to take the
 * 	host_paging port instead of the host_priv port.
 * 	[92/06/17            jeffreyh]
 * 
 * 	Fix transit logic to correctly cancel old proxy transits
 * 	against transits sent over from principal.
 * 	[92/06/16            dlb]
 * 
 * 	Rework destination side transit logic.
 * 	[92/06/01            dlb]
 * 
 * Revision 2.5.2.8  92/05/28  18:19:29  jeffreyh
 * 	Assert that receive right is not a special port.
 * 	[92/05/28            dlb]
 * 
 * Revision 2.5.2.7  92/05/27  00:49:17  jeffreyh
 * 	Implement dead name request logic.  Also fixed senders and port
 * 	death request logic.
 * 	[92/05/25            dlb]
 * 
 * 	Added logic to force ports to persist if they are needed as a
 * 	forwarding agent for a migrated port.
 * 	[92/05/22            sjs]
 * 
 * Revision 2.5.2.6.1.2  92/05/08  10:18:13  jeffreyh
 * 	Add logic to account for port rights that arrive in non-migrated
 * 	messages while a migrate is in progress.  atrium_msg_count -->
 * 	moved_msg_count for improved readability.
 * 	[92/05/07            dlb]
 * 
 * Revision 2.5.2.6.1.1  92/05/06  17:41:54  jeffreyh
 * 	Logic to migrate no senders, port death, and dead name requests
 * 	when a receive right migrates is badly broken.  Add panics to
 * 	prevent data structure corruption if this is attempted.
 * 	Make proxy a real port before pulling receive right from sender
 * 	(includes converting all queued messages from network format).
 * 	Wait for arrival of all messages on atrium.
 * 	Call routine to syncronize with former proxy being no longer
 * 	queued to send.  See ipc_output.c
 * 	[92/05/05            dlb]
 * 
 * 	Use ipc_port_alloc_special instead of ip_alloc in order to
 * 	get an initialized port structure for the atrium.  Add
 * 	comment justifying use of ip_free to get rid of it.
 * 	[92/04/28            dlb]
 * 
 * Revision 2.5.2.6  92/04/08  15:45:41  jeffreyh
 * 	When receiving a receive right, move messages that were queued
 * 	on the proxy to the atrium before moving the atrium's queue to
 * 	the port's queue.
 * 	[92/03/31            dlb]
 * 
 * Revision 2.5.2.5  92/03/28  10:11:38  jeffreyh
 * 	Fix stransit and sotransit accounting for migrated ports.
 * 	[92/03/25            dlb]
 * 
 * Revision 2.5.2.4  92/02/21  11:24:29  jsb
 * 	Don't convert migrated kmsgs to network format.
 * 	[92/02/21  09:06:25  jsb]
 * 
 * 	Changed norma_ipc_send_rright to return uid.
 * 	[92/02/20  17:14:09  jsb]
 * 
 * Revision 2.5.2.3  92/01/21  21:51:25  jsb
 * 	From dlb@osf.org: Fixed ip_kotype assertions and stransit logic.
 * 	[92/01/17  14:39:09  jsb]
 * 
 * 	More de-linting.
 * 	[92/01/17  11:39:52  jsb]
 * 
 * 	De-linted.
 * 	[92/01/16  22:11:26  jsb]
 * 
 * Revision 2.5.2.2  92/01/09  18:45:27  jsb
 * 	Corrected setting of ip_norma_stransit for migrated-from port.
 * 	[92/01/09  13:17:34  jsb]
 * 
 * 	Use remote_host_priv() instead of norma_get_special_port().
 * 	[92/01/04  18:24:21  jsb]
 * 
 * Revision 2.5.2.1  92/01/03  16:37:33  jsb
 * 	Use ipc_port_release instead of ip_release to allow port deallocation.
 * 	Changed ndproxy macros to nsproxy.
 * 	[91/12/31  21:35:52  jsb]
 * 
 * 	Added code to use norma_port_tabled function.
 * 	Added missing ip_reference when receiving a receive right.
 * 	[91/12/31  17:19:11  jsb]
 * 
 * 	Changed printfs to debugging printfs.
 * 	Changes for IP_NORMA_REQUEST macros being renamed to ip_ndproxy{,m,p}.
 * 	[91/12/31  12:20:36  jsb]
 * 
 * 	Fixed reference counting. No-senders now works for migrated ports.
 * 	[91/12/28  18:44:05  jsb]
 * 
 * 	Added source_node parameter to norma_ipc_receive_rright so that
 * 	we know exactly where to send norma_ipc_pull_receive request.
 * 	[91/12/27  21:32:31  jsb]
 * 
 * 	Norma_ipc_pull_receive now uses a temporary proxy for forwarded
 * 	messages, since norma_ipc_send now uses port queue.
 * 	Still need to wait for proxy to drain, and receive_rright still
 * 	needs to not clobber any proxy-queued messages when moving over
 * 	messages received on atrium.
 * 	[91/12/27  17:07:10  jsb]
 * 
 * 	A new implementation that migrates queued messages and port state
 * 	(such as qlimit) when migrating receive rights. Contains reference
 * 	counting workarounds; no-senders does not yet work on migrated ports.
 * 	This implementation does not change the migrated port's uid.
 * 	[91/12/26  18:23:25  jsb]
 * 
 * 	Corrected log. Removed uses of obsolete ipc_port fields.
 * 	Added a debugging printf in place of an assertion failure.
 * 	[91/12/24  14:24:46  jsb]
 * 
 * Revision 2.4  91/12/13  14:00:04  jsb
 * 	Changed debugging printf routines.
 * 
 * Revision 2.3  91/12/10  13:26:05  jsb
 * 	Make sure a port has a uid before migrating its receive right.
 * 	Set ip_receiver_name and ip_destination for migrated right.
 * 	[91/12/10  11:28:53  jsb]
 * 
 * Revision 2.2  91/11/14  16:45:56  rpd
 * 	Created.
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
/*
 *	File:	norma/ipc_migrate.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Functions for migration of receive rights between nodes.
 */

#include <norma_vm.h>
#include <norma_ether.h>

#include <vm/vm_kern.h>
#include <mach/vm_param.h>
#include <mach/port.h>
#include <mach/message.h>
#include <norma/norma_internal.h>
#include <norma/norma_internal_server.h>
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

/*
 * Called whenever a receive right is sent to another node.
 * Port must be locked. Port must be a local port.
 */
unsigned long
norma_ipc_send_rright(
	ipc_port_t	port)
{
	unsigned long uid;

	/*
	 * If this port has never been exported (for either srights or
	 * sorights), assign it a uid and place it on the norma port list.
	 */
	if (! port->ip_norma_tabled) {
		norma_port_insert(port, FALSE);
	}

	assert(! port->ip_norma_is_proxy);
	uid = port->ip_norma_uid;

	/*
	 * The only reference after this will be one in the norma port table.
	 */
	ip_release(port);
	assert(port->ip_references > 0);
	ip_unlock(port);

	/*
	 * For now, we continue to accept messages here.
	 * We turn into a proxy only when we have a dest_node to give senders,
	 * which will be sent by the receiver of this receive right.
	 * (We may not yet know where dest_node is, since the message
	 * carrying this receive right may be indirected...)
	 */
	return uid;
}

/*
 * When a node receives a migrated receive right, it sends this message
 * to the source of the receive right. The source then starts sending
 * queued messages until it runs out...
 * Note of course that if this port is busy enough, the migration will
 * never happen. Not easy to fix this, but it's not a correctness problem
 * and I don't think it will be a problem in practice.
 *
 * This call is executed in a kserver thread context.
 */
kern_return_t
norma_ipc_pull_receive(
	host_t		host,
	unsigned	uid,
	unsigned	dest_node,
	int		*stransit,
	int		*sotransit,
	ipc_port_t	*nsrequest,
	ipc_port_t	*pdrequest,
	unsigned	*seqno,
	unsigned	*qlimit,
	int		*moved_msg_count,
	ipc_port_t	*xmm_object,
	int		*xmm_object_refs)
{
	ipc_port_t port, proxy, dest;
	ipc_mqueue_t mqueue;
	ipc_kmsg_queue_t kmqueue;
	ipc_kmsg_t kmsg;
	int proxy_stransit;
	int msg_count;

	printf1("norma_ipc_pull_receive(uid=%x dest=%d)\n", uid, dest_node);
	/*
	 * Guard against random bozos calling this routine.
	 */
	if (host == HOST_NULL) {
		return KERN_INVALID_ARGUMENT;
	}

	assert(dest_node != node_self());

	/*
	 * Create a private proxy for forwarding messages via norma_ipc_send.
	 */
	proxy = ipc_port_alloc_special(ipc_space_remote);
	if (proxy == IP_NULL) {
		panic("norma_ipc_pull_receive: ipc_port_alloc_special");
	}
	proxy->ip_norma_uid = uid;
	proxy->ip_norma_dest_node = dest_node;
	proxy->ip_norma_is_proxy = TRUE;
	proxy->ip_references++; /* to be released before returning */

	/*
	 * Find the port.
	 */
	port = norma_port_lookup(uid, FALSE);
	assert(port != IP_NULL);
	assert(! port->ip_norma_is_proxy);

	/*
	 * Migrate messages to destination.
	 * New messages may show up. We could turn
	 * them off, perhaps, by adjusting queue
	 * limit, but it's probably not worth it,
	 * and what do we do about SEND_ALWAYS?
	 */
	printf1("norma_ipc_pull_receive: moving %d msgs\n", port->ip_msgcount);
	mqueue = &port->ip_messages;
	imq_lock(mqueue);
	assert(ipc_thread_queue_empty(&mqueue->imq_threads));
	kmqueue = &mqueue->imq_messages;
	msg_count = 0;
	while ((kmsg = ipc_kmsg_dequeue(kmqueue)) != IKM_NULL) {
		imq_unlock(mqueue);
		assert(kmsg->ikm_header.msgh_remote_port == (mach_port_t)port);

		/*
		 * Pull destination right out of port.  It gets recreated
		 * by recipient on atrium when the message arrives.
		 */
		if (MACH_MSGH_BITS_REMOTE(kmsg->ikm_header.msgh_bits)
		    == MACH_MSG_TYPE_PORT_SEND)
			port->ip_srights--;
		else
			port->ip_sorights--;
		ip_release(port);
		assert(port->ip_references > 0);

		/*
		 * Send the message via fake proxy port.
		 */
		kmsg->ikm_header.msgh_remote_port = (mach_port_t) proxy;
		kmsg->ikm_header.msgh_bits |= MACH_MSGH_BITS_MIGRATED;
		kmsg->ikm_forward_proxy = proxy;

		ip_unlock(port);
		ip_lock(proxy);
		ip_reference(proxy); /* dp_finish will release this reference */

#define	NORMA_PROXY_PORT_QUEUE_LIMIT	1
		if (norma_ipc_send(kmsg
#if	NORMA_PROXY_PORT_QUEUE_LIMIT
				   ,MACH_SEND_ALWAYS, 0
#endif
				   ) == KERN_NOT_RECEIVER) {
			panic("norma_ipc_pull_receive() failed to forward\n");
		}
		msg_count++;
		printf1("sent kmsg 0x%x\n", kmsg);
		ip_lock(port);
		imq_lock(mqueue);
	}
	imq_unlock(mqueue);

	/*
	 * Copy out port state.
	 * Some assertions came from ipc_port_clear_receiver.
	 */
	assert(port->ip_mscount == 0);
	assert(port->ip_seqno == 0);
	assert((ip_kotype(port) == IKOT_NONE) ||
	       (ip_kotype(port) == IKOT_PAGER));
	assert(port->ip_pset == IPS_NULL);

	/*
	 *	The no senders and port death requests are easy to deal with
	 *	(just move them).  The regular dead name requests are also
	 *	easy (just leave them).  Norma (node) dead name requests
	 *	will move when the proxy is discarded.
	 */

	*nsrequest = port->ip_nsrequest;
	*pdrequest = port->ip_pdrequest;
	*xmm_object = port->ip_norma_xmm_object;
	*xmm_object_refs = port->ip_norma_xmm_object_refs;
	*seqno = port->ip_seqno;
	*qlimit = port->ip_qlimit;
	*moved_msg_count = msg_count;

	/*
	 * Tell the recipient what its new stransit count should be.
	 * If there are any local send rights, leave 1 stransit here.
	 *
	 * Note that any send rights associated with messages
	 * (as destinations) have already been accounted for above.
	 *
	 */
	assert(port->ip_srights >= port->ip_norma_stransit);
	if (port->ip_srights - port->ip_norma_stransit > 0) {
		proxy_stransit = 1;
	} else {
		proxy_stransit = 0;
	}
	*stransit = port->ip_norma_stransit + proxy_stransit;

	/*
	 * Send a count of send_once rights.  This includes three
	 * different cases:
	 *	1) Local send once rights.  These are remote to
	 *	    the recipient and need an sotransit there.  We
	 *	    leave an sotransit per right here.
	 *	2) Send once rights on recipient's node.  These
	 *	    will be local there, and will be adjusted there.
	 *	3) Send once rights on some other node.  These will
	 *	    also be remote to the recipient and need an sotransit.
	 * Note that send once rights as the destination of messages
	 * were taken out above when the messages were moved.
	 */

	*sotransit = port->ip_sorights;

	/*
	 * Change port into a proxy.
	 */
	dest = port->ip_destination;
	port->ip_destination = IP_NULL;
	assert((ip_kotype(port) == IKOT_NONE) ||
	       (ip_kotype(port) == IKOT_PAGER));
	port->ip_receiver = ipc_space_remote;
	port->ip_receiver_name = 1; /* name used by ipc_port_alloc_special */

	port->ip_mscount = 0;
	port->ip_norma_xmm_object_refs = 0;
	port->ip_norma_xmm_object = IP_NULL;
	port->ip_srights -= port->ip_norma_stransit;
	port->ip_sorights -= port->ip_norma_sotransit;
	port->ip_references -= (port->ip_norma_stransit +
				port->ip_norma_sotransit);
	assert(port->ip_references > 0);

	port->ip_nsrequest = ip_nsproxym(port);
	port->ip_pdrequest = IP_NULL;
	/* port->ip_dnrequests unchanged */

	port->ip_pset = IPS_NULL;
	port->ip_seqno = 0;
	port->ip_msgcount = 0;
	port->ip_qlimit = MACH_PORT_QLIMIT_DEFAULT;

	port->ip_norma_stransit = -proxy_stransit;
	port->ip_norma_sotransit = -port->ip_sorights;
	port->ip_norma_dest_node = dest_node;
	port->ip_norma_is_proxy = TRUE;
	port->ip_norma_is_special = FALSE;

	/*
	 * By setting the forward field, we make sure this port cannot be
	 * thrown away by normal operations, but will persist as a place
	 * holder to the new dest.
	 */
	port->ip_norma_forward = TRUE;

	/*
	 * XXX
	 * What do we do with:
	 *	ip_blocked?
	 *
	 */
	ip_unlock(port);

	/*
	 *	Release destination reference.
	 */
	if (dest != IP_NULL)
		ipc_port_release(dest);

	/*
	 * Finally, deallocate proxy port if all messages have been sent.
	 * Otherwise, it will be deallocated in dp_finish().
	 */
	ip_lock(proxy);
	if (--proxy->ip_references == 1) {
		ip_unlock(proxy);
		ipc_port_dealloc_special(proxy, ipc_space_remote);
	} else
		ip_unlock(proxy);

	return KERN_SUCCESS;
}

/*
 * Receive a receive right.
 * We create a port and export it, giving the uid that we pick
 * to the node that sent us the receive right. We enter a forwarding
 * from its uid to ours in our forwarding table. We will first receive
 * all the messages queued at its port, then it will stop queueing
 * messages and instead tell nodes to send messages to us.
 *
 * What happens if we had a proxy for this uid?
 *
 * This routine is always called from a thread context.
 */

#define NORMA_INITIAL_TRANSIT 	(10000*1000)

ipc_port_t
norma_ipc_receive_rright(
	unsigned long	uid,
	unsigned long	source_node)
{
	kern_return_t kr;
	ipc_port_t port, atrium, xmm_object;
	int stransit, sotransit, refs;
	ipc_kmsg_t kmsg;
	ipc_mqueue_t mqueue;
	int moved_msg_count;
	int old_stransit, old_sotransit;
	boolean_t wanted;
	struct ipc_kmsg_queue kmsgs;
	struct ipc_mqueue local_mqueue;

	mumble("receive_rright %x\n", uid);
	assert(source_node != node_self());

	/*
	 * Allocate an atrium port to hold forwarded messages.
	 * This gives us one queue for forwarded messages and another
	 * for proxy usage.
	 */
	atrium = ipc_port_alloc_special(ipc_space_remote);
	printf1("ipc_port_alloc_special:atrium=0x%x\n", atrium);
	if (atrium == IP_NULL) {
		panic("receive_rright: atrium alloc");
	}

	/*
	 * Get a real port for this uid.  If there's an
	 * existing proxy, we change that, otherwise we
	 * allocate a new port.  We want a real port because
	 * the sender is going to turn his into a proxy soon,
	 * so messages that arrive in the interim should stay
	 * here.  The atrium is used to make sure that the
	 * messages on the sender's node stay first on the
	 * queue.
	 *
	 * XXX There is probably a really subtle message ordering
	 * XXX bug in here if neither the from nor to nodes are the
	 * XXX `home' node (in the NORMA uid) and the port has been
	 * XXX on both of them at some point in the past.
	 * 
	 * XXX
	 * What keeps this port from being deallocated?
	 * Perhaps we should hack up another sright/reference.
	 */
	assert(!IP_NORMA_SPECIAL(uid));
	port = norma_port_lookup(uid, TRUE);
	if (port == IP_NULL) {
		mumble("receive_rright: new proxy\n");
		port = ipc_port_alloc_special(ipc_space_remote);
		if (port == IP_NULL) {
			panic("receive_rright: ipc_port_alloc_special");
		}
		port->ip_norma_uid = uid;
		port->ip_norma_dest_node = source_node;
		assert(source_node != node_self());
		ip_lock(port);
		norma_port_insert(port, TRUE);
	} else {
		/*
		 * Found existing proxy.  Convert all kmsgs to internal
		 * format before converting proxy to port.
		 *
		 * Dequeue the port if it's queued.  Clearing
		 * ip_norma_is_proxy below makes it unqueueable
		 */
		mumble("receive_rright: old proxy\n");
		assert(ip_active(port));
		assert(port->ip_norma_is_proxy);
		port->ip_nsrequest = IP_NULL;
		netipc_port_dequeue_sync(port);
		/*
		 * Insure that *all* messages have been managed exactly once.
		 * Initialize a local for uncopyout messages. Tranfert all
		 * uncopyout messages from the local temporary queue to the
		 * local uncopyout queue.
		 */
		ipc_mqueue_init(&local_mqueue);
		imq_lock(&port->ip_messages);
		while (!ipc_kmsg_queue_empty(&port->ip_messages.imq_messages)) {
			kmsgs = port->ip_messages.imq_messages;
			ipc_kmsg_queue_init(&port->ip_messages.imq_messages);
			imq_unlock(&port->ip_messages);
			norma_ipc_uncopyout_all_kmsgs(port, &kmsgs,
						      &local_mqueue.imq_messages);
			imq_lock(&port->ip_messages);
		}
		port->ip_messages.imq_messages = local_mqueue.imq_messages;
		imq_unlock(&port->ip_messages);
	}
	/*
	 * Setting the port to !proxy allows xmm_object_by_memory_object
	 * to set up an xmm_pager port for us; we must prevent that in
	 * case the pull_receive brings us back a valid port.
	 */
	assert(port->ip_norma_xmm_object == IP_NULL);
	port->ip_norma_xmm_object = port;

	port->ip_norma_is_proxy = FALSE;
	port->ip_norma_forward = FALSE;

	/*
	 * Since this is supposed to be a receive right, it needs some
	 * stransits and sotransits so it can receive send and send
	 * once rights.  Drop in some initial values; these will be
	 * replaced when the real values are pulled over.  Save the
	 * old (proxy) transit counts for accounting code below.
	 */
	old_stransit = port->ip_norma_stransit;
	old_sotransit = port->ip_norma_sotransit;
	port->ip_norma_stransit = NORMA_INITIAL_TRANSIT;
	port->ip_norma_sotransit = NORMA_INITIAL_TRANSIT;
	assert(old_stransit <= 0);
	assert(old_sotransit <= 0);

	port->ip_srights += NORMA_INITIAL_TRANSIT;
	port->ip_sorights += NORMA_INITIAL_TRANSIT;
	port->ip_references += 2*NORMA_INITIAL_TRANSIT;

	/*
	 * I believe there is one port reference associated with
	 * the receive right itself. This ip_reference matches the
	 * ipc_port_release in norma_ipc_send_rright.
	 */
	port->ip_norma_atrium = atrium;
	ip_reference(port);
	ip_unlock(port);

	/*
	 * Pull over the receive right (and all of its messages).
	 */
	printf1("about to call r_norma_ipc_pull_receive\n");
	kr = r_norma_ipc_pull_receive(remote_host_priv(source_node,
						   MACH_MSG_TYPE_PORT_SEND),
				      uid,
				      node_self(),
				      &stransit,
				      &sotransit,
				      &port->ip_nsrequest,
				      &port->ip_pdrequest,
				      &port->ip_seqno,
				      &port->ip_qlimit,
				      &moved_msg_count,
				      &xmm_object,
				      &refs);
	printf1("r_norma_ipc_pull_receive returns %d/%x\n", kr, kr);
	assert(kr == KERN_SUCCESS);

	ip_lock(port);
	port->ip_norma_xmm_object_refs = refs;
	wanted = ((int)port->ip_norma_xmm_object & 1);
	port->ip_norma_xmm_object = xmm_object;
	ip_unlock(port);
	if (wanted)
		thread_wakeup((int) &port->ip_norma_xmm_object);

	/*
	 *	The reply message to norma_ipc_pull_receive is NOT
	 *	ordered with respect to messages being queued on the
	 *	atrium.  Wait until all the messages we are expecting
	 *	have arrived.
	 */
	ip_lock(atrium);
	while (atrium->ip_msgcount != moved_msg_count) {
		atrium->ip_norma_atrium_waiter = TRUE;
		assert_wait((int)&atrium->ip_norma_atrium, FALSE);
		ip_unlock(atrium);
		thread_block((void (*)(void)) 0);
		ip_lock(atrium);
	}
	ip_unlock(atrium);

	/*
	 *	Always request dead name notification for source node
	 *	so that forwarding proxy will get cleaned up.
	 */
	norma_ipc_node_dnrequest(&realhost, uid, source_node);
		
	netipc_thread_lock();

	/*
	 * Now that we have the real (pulled over) values of stransit, use
	 * them to replace the dummy values we inserted above.  First adjust
	 * the pulled over values by cancelling any local (negative) values.
	 * The resulting numbers are the real transit counts that affect
	 * the rights and references counts.
	 */
	ip_lock(port);
	stransit += old_stransit;
	sotransit += old_sotransit;
	port->ip_norma_stransit += stransit - NORMA_INITIAL_TRANSIT;
	port->ip_norma_sotransit += sotransit - NORMA_INITIAL_TRANSIT;
	port->ip_srights += stransit - NORMA_INITIAL_TRANSIT;
	port->ip_sorights += sotransit - NORMA_INITIAL_TRANSIT;
	port->ip_references += (stransit - NORMA_INITIAL_TRANSIT +
				sotransit - NORMA_INITIAL_TRANSIT);
	assert(port->ip_references > 0);
 
	/*
	 * Set any remaining fields.
	 * XXX Does ip_receiver's value matter here?
	 */
	port->ip_receiver_name = MACH_PORT_NULL;
	port->ip_destination = IP_NULL;

	/*
	 * XXX Resulting values for srights and sorights
	 * XXX should be positive and "reasonable".
	 */
	assert ((port->ip_srights >= 0) &&
			(port->ip_srights < NORMA_INITIAL_TRANSIT));
	assert ((port->ip_sorights >= 0) &&
			(port->ip_sorights <NORMA_INITIAL_TRANSIT));

	/*
	 * Move all messages queued on the port to the atrium.  The
	 * destination rights for these messages are in the port already.
	 */
	mqueue = &port->ip_messages;
	imq_lock(mqueue);
	ip_lock(atrium);
	while ((kmsg = ipc_kmsg_dequeue(&mqueue->imq_messages))
	       != IKM_NULL) {
		register mach_msg_header_t *msgh;
		unsigned long ack_error_data;

		port->ip_msgcount--;

		/*
		 * 	There are two possible reasons for a message
		 *	to be here:
		 *		(1) It was queued outbound on the proxy.
		 *		(2) It arrived inbound while we were
		 *			moving the receive right.
		 *	In case (1) the message is not in a NORMA kmsg,
		 *	and hence will be ignored by norma_ipc_finish_
		 *	receiving.  These messages were converted to
		 *	internal format by norma_ipc_uncopyout_all_kmsgs.
		 *	Case (2) messages will just came off the network
		 *	and will be handled by norma_ipc_finish_receiving.
		 *	Hence all we have to do is requeue anything we find
		 *	here on the atrium.
		 */
		atrium->ip_msgcount++;
		ipc_kmsg_enqueue(&atrium->ip_messages.imq_messages, kmsg);
	}
	ip_unlock(atrium);

	assert(port->ip_msgcount == 0);

	/*
	 * Move messages and rights from atrium to new principal.
	 * The rights on the atrium are the destination rights
	 * for the messages queued there from remote nodes.
	 */
	printf1("port=0x%x, ->atrium=0x%x atrium=0x%x count=%d\n",
	       port,
	       port->ip_norma_atrium,
	       atrium,
	       atrium->ip_msgcount);
	printf1("--- old count=%d\n", port->ip_msgcount);
	port->ip_msgcount = atrium->ip_msgcount;
	port->ip_messages.imq_messages = atrium->ip_messages.imq_messages;
	port->ip_srights += atrium->ip_srights;
	port->ip_sorights += atrium->ip_sorights;
	port->ip_references += (atrium->ip_srights + atrium->ip_sorights);
	/*
	 * XXX Resulting values for srights and sorights
	 * XXX should be positive and "reasonable".
	 */
	assert ((port->ip_srights >= 0) &&
			(port->ip_srights < NORMA_INITIAL_TRANSIT));
	assert ((port->ip_sorights >= 0) &&
			(port->ip_sorights < NORMA_INITIAL_TRANSIT));
	assert(port->ip_references > 0);

	/*
	 *	Clean out atrium state transferred to port.
	 */
	atrium->ip_msgcount = 0;
	ipc_mqueue_init(&atrium->ip_messages);
	atrium->ip_references -= (atrium->ip_srights + atrium->ip_sorights);
	atrium->ip_srights = 0;
	atrium->ip_sorights = 0;

	assert(atrium->ip_references == 1);

	port->ip_norma_atrium = IP_NULL;

	imq_unlock(mqueue);
	ip_unlock(port);
	netipc_thread_unlock();

	ipc_port_dealloc_special(atrium, ipc_space_remote);

	/*
	 * Return port.
	 */
	mumble("receive_rright: returning port 0x%x\n", port);
	return port;
}
