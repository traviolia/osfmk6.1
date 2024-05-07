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
 *
 * Revision 2.1.2.9  92/09/15  17:33:17  jeffreyh
 * 	Remove forwarding proxy logic.  Now handled by dead name logic.
 * 	Add "already dead" case to norma_ipc_dead_destination.
 * 	[92/09/14            dlb]
 * 	Clear ip_norma_forward in norma_ipc_dead_destination().  Add long
 * 	XXX comment about why we can't do this on the root node (yet).
 * 	[92/08/06            dlb]
 * 
 * Revision 2.1.2.8  92/06/24  18:00:29  jeffreyh
 * 	Move test for complex kmsg to caller of ipc_kmsg_uncopyout_to_network.
 * 	[92/06/02            dlb]
 * 
 * 	Proxies now maintain sotransit.
 * 	[92/06/01            dlb]
 * 
 * Revision 2.1.2.7  92/05/28  18:20:40  jeffreyh
 * 	Add logic to unprepare a destination when uncopyout'ing a kmsg.
 * 	Convert a couple of trivial routines to macros.
 * 	[92/05/28            dlb]
 * 
 * Revision 2.1.2.6  92/05/27  00:46:52  jeffreyh
 * 	Clean mscount before destroying port in norma_ipc_dead_destination.
 * 	[92/05/25            dlb]
 * 
 * Revision 2.1.2.5  92/04/08  15:45:21  jeffreyh
 * 	Use norma_ipc_yield_transits when destroying a proxy.
 * 	Get rid of related XXX comments.
 * 	[92/04/07            dlb]
 * 
 * 	Add mousetrap to norma_ipc_error_receiving to catch use of bad
 * 	norma kmsg.
 * 	[92/04/05            dlb]
 * 
 * 	Move IP_NULL logic to norma_ipc_unsend_port from
 * 	norma_kmsg_uncopyout_to_network.  Add logic to pass IP_DEAD.
 * 	[92/04/02            dlb]
 * 
 * 		Don't norma_port_remove a dead proxy; norma_ipc_port_destroy
 * 	can cope now.
 * 	[92/04/01            dlb]
 * 
 * Revision 2.1.2.4  92/03/28  10:11:22  jeffreyh
 * 	When handling a receive error, check whether remote_port should be
 * 	removed from norma port table.
 * 	[92/03/25            dlb]
 * 	Don't unsend null ports in ipc_kmsg_uncopyout_to_network.
 * 	[92/03/20  14:12:27  dlb]
 * 
 * Revision 2.1.2.3  92/02/21  11:24:11  jsb
 * 	Moved ipc_kmsg_uncopyout_to_network here from ipc/ipc_kmsg.c.
 * 	Separated norma_ipc_uncopyout_all_kmsgs out from
 * 	norma_ipc_dead_destination (renamed from norma_ipc_destroy_proxy).
 * 	[92/02/21  10:38:49  jsb]
 * 
 * 	Eliminated norma_ipc_unsend_*_dest, since norma_ipc_send_*_dest
 * 	is no longer called until and unless message is successfully
 * 	received. Added check to ensure that destroyed proxy has queued
 * 	messages. Added call to norma_port_remove before call to
 * 	ipc_port_destroy. Removed zeroing of ip_seqno.
 * 	[92/02/18  08:44:28  jsb]
 * 
 * Revision 2.1.2.2  92/01/21  21:51:00  jsb
 * 	De-linted.
 * 	[92/01/16  21:31:56  jsb]
 * 
 * Revision 2.1.2.1  92/01/03  08:57:08  jsb
 * 	First NORMA branch checkin.
 * 
 * Revision 2.1.1.3  91/12/31  21:36:40  jsb
 * 	Changed ndproxy macros to nsproxy.
 * 
 * Revision 2.1.1.2  91/12/31  12:11:04  jsb
 * 	Changed remote_port handling in norma_ipc_destroy_proxy to account
 * 	for remote consumption of send-once right (if any) in first kmsg.
 * 	Added ipc_kmsg_uncopyout_to_network, which is given remote_port which
 * 	it in turn hands to norma_ipc_unsend{_migrating,}_dest, which now
 * 	use this port instead of trying to look it up based on uid.
 * 	Changes for IP_NORMA_REQUEST macros being renamed to ip_ndproxy{,m,p}.
 * 
 * Revision 2.1.1.1  91/12/29  21:42:51  jsb
 * 	First checkin.
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
 *	File:	norma/ipc_clean.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Routines to clean messages sent to the network.
 */

#include <ipc/ipc_port.h>
#include <ipc/ipc_kmsg.h>
#include <norma/ipc_node.h>
#include <kern/misc_protos.h>

#include <mach/vm_param.h>

#define	norma_ipc_unsend_soright(uid)	norma_ipc_receive_soright(uid)
#define norma_ipc_unsend_sright(uid)	norma_ipc_receive_sright(uid)

ipc_kmsg_t	norma_kmsg_receive_error;

/*
 * Forward.
 */
ipc_port_t	norma_ipc_unsend_port(
			unsigned long		uid,
			mach_msg_type_name_t	type_name);

ipc_port_t	norma_ipc_unsend_rright(
			unsigned long		uid);
/*
 * These unsend routines should undo the effects of the send routines above.
 */

ipc_port_t
norma_ipc_unsend_port(
	unsigned long		uid,
	mach_msg_type_name_t	type_name)
{
	if (uid == IP_NORMA_NULL)
		return IP_NULL;
	if (uid == IP_NORMA_DEAD)
		return IP_DEAD;

	if (type_name == MACH_MSG_TYPE_PORT_SEND_ONCE) {
		return norma_ipc_unsend_soright(uid);
	} else if (type_name == MACH_MSG_TYPE_PORT_SEND) {
		return norma_ipc_unsend_sright(uid);
	} else if (type_name == MACH_MSG_TYPE_PORT_RECEIVE) {
		return norma_ipc_unsend_rright(uid);
	}
	panic("norma_ipc_unsend_port: bad type %d\n", type_name);
	return IP_NULL;
}

ipc_port_t
norma_ipc_unsend_rright(
	unsigned long	uid)
{
	ipc_port_t port;

	port = norma_port_lookup(uid, FALSE);
	if (port == IP_NULL) {
		return IP_NULL;
	}
	if (! ip_active(port)) {
		ip_unlock(port);
		return IP_DEAD;
	}
	assert(! port->ip_norma_is_proxy);
	ip_reference(port);
	ip_unlock(port);
	return port;
}

/*
 * MP notes: dest port is locked.
 */
void
ipc_kmsg_uncopyout_header_to_network(
	mach_msg_header_t	*msgh,
	ipc_port_t		dest)
{
	/*
	 * Uncopy local port
	 */
	if (msgh->msgh_local_port) {
		register mach_msg_type_name_t type;

		ip_unlock(dest);
		type = MACH_MSGH_BITS_LOCAL(msgh->msgh_bits);
		type = ipc_object_copyin_type(type);
		msgh->msgh_local_port = (mach_port_t)
		    norma_ipc_unsend_port((unsigned long)msgh->msgh_local_port,
					  type);
		ip_lock(dest);
	}

	/*
	 * Uncopy remote port, and undo effects of preparing dest.
	 */
	assert((msgh->msgh_bits & MACH_MSGH_BITS_MIGRATED) == 0);
	assert(msgh->msgh_remote_port == (mach_port_t) dest->ip_norma_uid);
	msgh->msgh_remote_port = (mach_port_t) dest;
	norma_ipc_unprepare_dest(dest, MACH_MSGH_BITS_REMOTE(msgh->msgh_bits));
}

void
ipc_kmsg_uncopyout_to_network(
	ipc_kmsg_t	kmsg)
{
    	mach_msg_descriptor_t	*saddr, *eaddr;
    	mach_msg_body_t		*body;
	
	kmsg->ikm_header.msgh_bits &= ~ (MACH_MSGH_BITS_COMPLEX_DATA |
					 MACH_MSGH_BITS_COMPLEX_PORTS |
					 MACH_MSGH_BITS_MIGRATED);

    	body = (mach_msg_body_t *) (&kmsg->ikm_header + 1);
    	saddr = (mach_msg_descriptor_t *) (body + 1);
    	eaddr = (mach_msg_descriptor_t *) saddr + body->msgh_descriptor_count;

    	for ( ; saddr <  eaddr; saddr++) {

	    switch (saddr->type.type) {
	    
	        case MACH_MSG_PORT_DESCRIPTOR: {
		    mach_msg_type_name_t 	name;
		    ipc_port_t 			*port;
		    mach_msg_port_descriptor_t 	*dsc;
		
		    dsc = &saddr->port;
		
		    /* this is really the type SEND, SEND_ONCE, etc. */
		    name = dsc->disposition;
		    port = (ipc_port_t *)&dsc->name;
		    *port = norma_ipc_unsend_port(
			    	(unsigned long) *port, name);

		    break;
	        }
	        case MACH_MSG_OOL_DESCRIPTOR: {
		  /*
		   * This is a copy object.
		   * It is okay to leave copy objects in page-list form.
		   * The netipc module is responsible for leaving them
		   * in a sane state.
		   */
		    break;
	        }
		case MACH_MSG_OOL_PORTS_DESCRIPTOR: {
		    ipc_port_t            		*ports;
		    int					j;
		    mach_msg_type_name_t    		name;
		    mach_msg_ool_ports_descriptor_t 	*dsc;
		
		    dsc = &saddr->ool_ports;

		    /* this is really the type SEND, SEND_ONCE, etc. */
		    name = dsc->disposition;
	    	
		    ports = (ipc_port_t *) dsc->address;
	    	
		    /*
		     * Convert uids into ports and undo refcount action.
		     */

		    for ( j = 0; j < dsc->count; j++) {
			    ports[j] = norma_ipc_unsend_port(
			    	(unsigned long) ports[j], name);
		    }
		    break;
	        }
	        default: {
		    panic("ipc_kmsg_uncopyout_to_network:  bad descriptor");
		}
	    }
	}
}

/*
 * Convert all kmsgs queued on port from network to internal format,
 * and move them to mqueue. Since from_kmsgs and to_kmsgs are isolated queues,
 * they doesn't need to be locked, but their associated port is locked.
 */
void
norma_ipc_uncopyout_all_kmsgs(
	ipc_port_t		port,
	ipc_kmsg_queue_t	from_kmsgs,
	ipc_kmsg_queue_t	to_kmsgs)
{
	register ipc_kmsg_t kmsg;

	while ((kmsg = ipc_kmsg_dequeue(from_kmsgs)) != IKM_NULL) {
		printf1("uncopyout ksmg=0x%x msgh_id=%d; was s=%d so=%d\n",
			kmsg, kmsg->ikm_header.msgh_id,
			port->ip_srights, port->ip_sorights);

		/*
		 * Uncopy header
		 */
		ipc_kmsg_uncopyout_header_to_network(&kmsg->ikm_header, port);

		/*
		 * Uncopy body
		 */
		if (kmsg->ikm_header.msgh_bits & MACH_MSGH_BITS_COMPLEX) {
			ip_unlock(port);
			ipc_kmsg_uncopyout_to_network(kmsg);
			ip_lock(port);
		}

		/*
		 * Move kmsg to the uncopyout queue.
		 */
		ipc_kmsg_enqueue_macro(to_kmsgs, kmsg);
	}
}

/*
 * Called when a send to a remote port returns a notification that
 * the port is dead.  Caller must donate a port reference.
 * MP notes: The port is locked. Upon return the port is unlocked.
 */
void
norma_ipc_dead_destination(
	ipc_port_t	port)
{
	ipc_kmsg_t kmsg;
	struct ipc_mqueue mqueue;
	struct ipc_kmsg_queue kmsgs;
	long stransit;
	long sotransit;

	printf1("norma_ipc_dead_destination: 0x%x:%x refs %d\n",
		port, port->ip_norma_uid, port->ip_references);

	assert(port->ip_norma_is_proxy);
	assert(port->ip_references > 0);

	/*
	 *	If the port is already dead, then the work below
	 *	has been done for us.  Just try to remove it from
	 *	the norma system.  Consume caller's reference.
	 */
	port->ip_norma_is_zombie = FALSE;
	if (!ip_active(port)) {
		norma_port_remove_try(port); /* consumes lock */
		ipc_port_release(port);
		return;
	}

	/*
	 * Insure that *all* messages have been managed exactly once.
	 * Initialize a local for uncopyout messages. Tranfert all
	 * uncopyout messages from the local temporary queue to the
	 * local uncopyout queue. Since norma_ipc_send_yield_transits()
	 * should be called without any lock held, the call occurs
	 * in the same loop.
	 */
	ipc_mqueue_init(&mqueue);
	for (;;) {
		imq_lock(&port->ip_messages);
		if (!ipc_kmsg_queue_empty(&port->ip_messages.imq_messages)) {
			kmsgs = port->ip_messages.imq_messages;
			ipc_kmsg_queue_init(&port->ip_messages.imq_messages);
			imq_unlock(&port->ip_messages);
			/*
			 * Convert all kmsgs from network to internal format.
			 */
			norma_ipc_uncopyout_all_kmsgs(port, &kmsgs,
						      &mqueue.imq_messages);
		} else if (port->ip_norma_stransit != 0 ||
			   port->ip_norma_sotransit != 0) {
			/*
			 * Yield transits to principal if there are any.
			 */
			imq_unlock(&port->ip_messages);
			stransit = -port->ip_norma_stransit;
			port->ip_norma_stransit = 0;
			sotransit = -port->ip_norma_sotransit;
			port->ip_norma_sotransit = 0;
			ip_unlock(port);
			norma_ipc_send_yield_transits(port->ip_norma_dest_node,
						      port->ip_norma_uid,
						      stransit, sotransit);
			ip_lock(port);
		} else
			break;
	}
	port->ip_messages.imq_messages = mqueue.imq_messages;
	imq_unlock(&port->ip_messages);

	/*
	 *	Destroy the port.  We clear the nsrequest because
	 *	yield_transits took care of this (above).  Clearing
	 *	mscount and checking seqno are required by ipc_port_destroy().
	 */
	if (port->ip_nsrequest) {
		assert(ip_nsproxyp(port->ip_nsrequest));
		port->ip_nsrequest = IP_NULL;
	} else
		assert(port->ip_srights == 0 && port->ip_sorights == 0);

	ipc_port_set_mscount(port, 0);
	assert(port->ip_seqno == 0);
	printf1("dead_destination: 0x%x:%x: about to destroy port\n",
		port, port->ip_norma_uid);
	ipc_port_destroy(port);
	printf1("dead_destination: 0x%x:%x: destroyed port\n",
		port, port->ip_norma_uid);
}

/*
 *	Error routine called when a message is received from the
 *	network and is undeliverable.
 */
void
norma_ipc_error_receiving(
	ipc_kmsg_t	*kmsgp)
{
	/*
	 *	Common case: it's not a norma message
	 */
	if ((*kmsgp)->ikm_size != IKM_SIZE_NORMA)
		return;

	/*
	 * XXX	Debugging check.  In addition to being sized
	 * XXX	as above, norma kmsgs are always page aligned.
	 */
	if ((vm_offset_t)*kmsgp != trunc_page((vm_offset_t) *kmsgp))
		panic("norma_ipc_error_receiving: bogus norma kmsg");

	/*
	 *	Drop this on a queue of kmsgs that need
	 *	to be destroyed due to receive errors.  The
	 *	destination (remote) port right is assumed to be in
	 *	local form (may be PORT_NULL), others in network form
	 *
	 * 	This queue is emptied by the netipc thread.
	 */
	assert(netipc_thread_lock_held());
	(*kmsgp)->ikm_next = norma_kmsg_receive_error;
	(*kmsgp)->ikm_prev = IKM_BOGUS;  /* XXX Temporary */
	norma_kmsg_receive_error = *kmsgp;
	*kmsgp = IKM_NULL;

	netipc_thread_wakeup();
}

/*
 *	Routine called by the netipc thread to clean up receive errors.
 */

void
norma_ipc_clean_receive_errors(void)
{
	ipc_kmsg_t	kmsg;
	ipc_port_t	remote_port;

	netipc_thread_lock();
	while ((kmsg = norma_kmsg_receive_error) != IKM_NULL) {

		/*
		 *	Advance the queue and destroy this kmsg.
		 *	Call norma_ipc_finish_receiving to convert
		 *	all ports to internal form (from network form).
		 *	If the destination port is not null, check
		 *	to see if it should be untabled; this is because
		 *	the transit ref goes away in receive_*_dest, but
		 *	we don't want to throw the port away there.
		 */
		norma_kmsg_receive_error = kmsg->ikm_next;
		kmsg->ikm_next = IKM_BOGUS;  /* Temporary for debug */

		netipc_thread_unlock();
		norma_ipc_finish_receiving(&kmsg);
		remote_port = (ipc_port_t) kmsg->ikm_header.msgh_remote_port;
		if (remote_port != MACH_PORT_NULL) {
			ip_lock(remote_port);
			norma_port_remove_try(remote_port); /* consumes lock */
		}
		/* ### FALSE for now */
		ipc_kmsg_destroy(kmsg, FALSE);
		netipc_thread_lock();
	}
	netipc_thread_unlock();
}
