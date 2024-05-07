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
 * Revision 2.5.2.10  92/09/15  17:34:35  jeffreyh
 * 	Add dead migrated principal hook to release forwarding proxy to
 * 	norma_ipc_yield_transits.
 * 	[92/09/15            dlb]
 * 	Verify transit count for send-once right.
 * 	[92/08/18            dlb]
 * 
 * 	norma_port_insert now handles uid generation.
 * 	[92/08/06            dlb]
 * 
 * 	Simplify norma_ipc_yield_transits -- if proxy is found, it must
 * 	be a forwarding node.  Remove PUT CODE HERE from forward case
 * 	of norma_ipc_stransit_request.  All forwarding proxies are now
 * 	covered by dead name notifications (see ipc_migrate.c).  Remove
 * 	unused declaration of norma_port_remove_locked().
 * 	[92/08/06            dlb]
 * 
 * Revision 2.5.2.9  92/06/24  18:01:50  jeffreyh
 * 	norma_ipc_stransit_wait must also recheck to see if the port is still
 * 	a proxy, and clean up if it's changed to the principal.
 * 	[92/06/16            dlb]
 * 
 * 	norma_ipc_send_sright must recheck whether the port is a proxy after
 * 	calling norma_ipc_stransit_wait.
 * 	[92/06/02            dlb]
 * 
 * 	Maintain sotransit on proxies.  Add comment explaining how
 * 	stransit and sotransit work.
 * 	[92/06/01            dlb]
 * 
 * Revision 2.5.2.8  92/05/28  18:19:48  jeffreyh
 * 	Add special port logic to norma_ipc_receive_{sright,soright}.
 * 	[92/05/28            dlb]
 * 
 * 	Split norma_ipc_send_dest() into two pieces, norma_ipc_prepare_dest(),
 * 	and norma_ipc_send_dest().  This is needed to get the bookkeeping
 * 	for send once destinations correct.
 * 	Delete norma_ipc_send_migrating_dest (empty routine).
 * 	[92/05/28            dlb]
 * 
 * Revision 2.5.2.7  92/05/27  00:50:17  jeffreyh
 * 	Deleted unnecessary asserts.  Added some retarget logic
 * 	     to norma_ipc_yeild_transits().
 * 	[92/05/19            sjs]
 * 
 * Revision 2.5.2.6.1.1  92/05/06  17:42:25  jeffreyh
 * 	Destination of migrated message must now NOT be a proxy.
 * 	Add some assertions.
 * 	[92/05/05            dlb]
 * 
 * Revision 2.5.2.6  92/04/08  15:46:18  jeffreyh
 * 	Change norma_ipc_no_local_senders to norma_ipc_yield_transits
 * 	and enhance to deal with send once rights.
 * 	[92/04/07            dlb]
 * 
 * 	Mistake in earlier change: When failing to receive destination,
 * 	all message contents are the sender's responsibility to clean up.
 * 	Don't try to clean up send rights on receiving node.
 * 	[92/04/06            dlb]
 * 
 * 	Logic to handle both null and dead ports as send rights.
 * 	[92/04/02            dlb]
 * 
 * 	Slight rearrangements to sright/stransit logic.  Try to remove port
 * 	when stransit hits zero on last send right in norma_ipc_send_sright().
 * 	[92/04/02            dlb]
 * 
 * 	Massive simplifications to norma_ipc_port_destroy.
 * 	[92/04/01            dlb]
 * 
 * Revision 2.5.2.5  92/03/28  10:12:12  jeffreyh
 * 	norma_ipc_receive_*_dest routines now must set port value in.
 * 	all cases; leave send once rights in kmsg for later cleanup.
 * 	Decrement sotransit when receiving send-once destination.
 * 	Create send-once right when receiving migrated send-once dest.
 * 	Put migrating destination rights in atrium instead of port.
 * 	[92/03/25            dlb]
 * 
 * Revision 2.5.2.4  92/02/21  11:24:59  jsb
 * 	Norma_ipc_send_{port,soright,sright,rright} now return uids.
 * 	Use new norma_ipc_remove_try routine.
 * 	[92/02/20  17:13:19  jsb]
 * 
 * 	Changed reference counting in norma_ipc_send_*_dest, now that they
 * 	are called only after successful acknowledgement of message.
 * 	Removed calls to db_show_all_uids, now that 'show all uids' exists.
 * 	Added rearm of ip_nsrequest in norma_ipc_notify_no_senders.
 * 	[92/02/18  08:58:56  jsb]
 * 
 * 	Added checks for losing all send rights without stransit becoming zero,
 * 	in which case we must generate a no-local-senders notification.
 * 	[92/02/16  15:22:29  jsb]
 * 
 * Revision 2.5.2.3  92/01/21  21:52:42  jsb
 * 	More de-linting.
 * 	[92/01/17  11:42:23  jsb]
 * 
 * 	De-linted.
 * 	[92/01/16  22:16:55  jsb]
 * 
 * Revision 2.5.2.2  92/01/09  18:45:59  jsb
 * 	Use quieter debugging printf when queue limit exceeded.
 * 	[92/01/09  16:13:18  jsb]
 * 
 * 	Use remote_host_priv() instead of norma_get_special_port().
 * 	Use new routines norma_unset_special_port and {local,remote}_special.
 * 	[92/01/04  18:20:55  jsb]
 * 
 * Revision 2.5.2.1  92/01/03  16:38:04  jsb
 * 	Use ipc_port_release instead of ip_release to allow port deallocation.
 * 	Added ip_reference to norma_ipc_send_migrating_dest so that
 * 	norma_ipc_send can always release a reference.
 * 	Changed ndproxy macros to nsproxy.
 * 	[91/12/31  21:34:24  jsb]
 * 
 * 	Added code to use norma_port_tabled function, which separates the
 * 	issues of whether a port has a uid and whether it is accessible
 * 	via norma_port_lookup.
 * 	Added a hard panic on a failed lookup in norma_ipc_no_local_senders.
 * 	Test for and ignore special uids in norma_ipc_notify_no_senders.
 * 	[91/12/31  17:14:19  jsb]
 * 
 * 	Added code to destroy active principal when no remote refs are left.
 * 	Added forwarding and absorbtion code to norma_ipc_no_local_senders.
 * 	Declared norma_ipc_no_local_senders void; changed returns accordingly.
 * 	Split norma_ipc_send_no_local_senders from norma_ipc_notify_no_senders.
 * 	Changed printfs to debugging printfs.
 * 	[91/12/31  11:57:31  jsb]
 * 
 * 	Changes for IP_NORMA_REQUEST macros being renamed to ip_ndproxy{,m,p}.
 * 	[91/12/30  10:18:08  jsb]
 * 
 * 	Test for proxies in norma_ipc_port_destroy.
 * 	[91/12/29  21:24:18  jsb]
 * 
 * 	Distinguish between dead and unfound ports in norma_ipc_receive_dest.
 * 	[91/12/29  15:58:20  jsb]
 * 
 * 	Use IP_NORMA_NSREQUEST macros. Release proxies in all cases.
 * 	Simulate copyin in receive_migrating_dest to account for
 * 	migrated messages.
 * 	[91/12/28  18:05:27  jsb]
 * 
 * 	Added source_node parameter to norma_ipc_receive_port so that it can
 * 	be passed to norma_ipc_receive_rright.
 * 	[91/12/27  21:33:34  jsb]
 * 
 * 	Norma_ipc_send_migrating_dest now expects a proxy.
 * 	[91/12/27  17:00:19  jsb]
 * 
 * 	Added norma_ipc_{send,receive}_migrating_dest routines.
 * 	Removed migrated parameter from norma_ipc_receive_dest.
 * 	Removed migration/retarget debugging code.
 * 	[91/12/26  20:45:44  jsb]
 * 
 * 	Replaced token-based implementation with stransit_request based one.
 * 	Added norma_ipc_send_dest. Removed norma_port_wire hack.
 * 	Moved norma_port_list routines to norma/ipc_list.c. Corrected log.
 * 	[91/12/24  14:23:14  jsb]
 * 
 * Revision 2.5  91/12/14  14:35:10  jsb
 * 	Removed ipc_fields.h hack.
 * 
 * Revision 2.4  91/12/13  14:09:42  jsb
 * 	Changed printfs to debugging printfs.
 * 
 * Revision 2.3  91/11/19  09:41:28  rvb
 * 	Added norma_port_wire hack, to avoid reference counting bug.
 *	Added code to test receive right migration.
 * 	[91/11/00            jsb]
 * 
 * Revision 2.2  91/11/14  16:46:18  rpd
 * 	Created.
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
 *	File:	norma/ipc_transit.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Functions for movement of rights between nodes, excluding receive
 *	rights (for which see norma/ipc_migrate.c).
 */

#include <ipc/ipc_port.h>
#include <ipc/ipc_notify.h>
#include <ipc/ipc_space.h>
#include <norma/norma_internal_server.h>
#include <norma/norma_internal.h>
#include <kern/host.h>
#include <kern/misc_protos.h>
#include <norma/ipc_node.h>
#include <norma/tr.h>

/*
 * Forward.
 */
#if	MACH_ASSERT
boolean_t	reasonable_uid(
			unsigned long	uid);
#endif	/* MACH_ASSERT */

unsigned long	norma_ipc_send_sright(
			ipc_port_t	port);

unsigned long	norma_ipc_send_soright(
			ipc_port_t	port);

void		norma_ipc_stransit_wait(
			ipc_port_t	port);

/*
 * Note: no decisions should be made based on who you think the receiver
 * will be, since it might not be who you think it will be.
 * (Receive rights move.) The only exception is norma_ipc_send_dest.
 * For example, if you have a send right and you are sending it to
 * who you think the receiver is, the receiver could have moved by
 * the time it gets there.
 */

/*
 * Both send rights and send once rights in the network are tracked by
 * transit counts.  These 
 *	- are always >= 0 on principal
 *	- are always <= 0 on proxy
 *	- sum to zero for the principals and proxies that represent
 *		one port provided that there are no messages in transit.
 * A message in transit is considered to have one (negative, i.e., proxy)
 * transit for each right in the message (except if the destination is
 * a send right -- see below).  Rights can vanish without being used, so
 * if a proxy has no rights, but does have transits, then norma_ipc_yield_
 * transits is called to get rid of them.
 *
 * On a principal, each transit count represents the number of rights
 * (recorded in the port structure) that are actually in the NORMA domain.
 * On a proxy, each transit count represents the number of rights that
 * the principal thinks it has.  Send rights add a twist to this in that
 * a proxy can have additional send rights.  As long as the principal
 * thinks that the proxy has at least one, it doesn't matter how many
 * there really are, provided that the principal's view (stransit) and
 * the actual number (srights) return to zero at the same time.  This
 * allows a proxy to create additional send rights without contacting
 * the principal.  A proxy that needs more stransits contacts the principal
 * (norma_ipc_stransit_request).  An additional twist is that stransits
 * are not consumed by destination send rights in messages; this avoids
 * an stransit request in the common case where a send right is obtained
 * to represent an object and then used to send many messages.
 */

#if	MACH_ASSERT
boolean_t
reasonable_uid(
	unsigned long	uid)
{
	return (uid != 0 && (long)uid > 0);
}
#endif /* MACH_ASSERT */

/*
 * Called when a port right of any flavor is sent to another node.
 * Port must be unlocked. Port may be a local or proxy port.
 */
unsigned long
norma_ipc_send_port(
	ipc_port_t		port,
	mach_msg_type_name_t	type_name)
{
	unsigned long uid;

	if (port == IP_NULL)
		return IP_NORMA_NULL;
	if (port == IP_DEAD)
		return IP_NORMA_DEAD;

	ip_lock(port);
	if (type_name == MACH_MSG_TYPE_PORT_SEND_ONCE) {
		uid = norma_ipc_send_soright(port);
	} else if (type_name == MACH_MSG_TYPE_PORT_SEND) {
		uid = norma_ipc_send_sright(port);
	} else if (type_name == MACH_MSG_TYPE_PORT_RECEIVE) {
		uid = norma_ipc_send_rright(port);
	} else {
		panic("norma_ipc_send_port: bad type %d\n", type_name);
	}
	assert(reasonable_uid(uid));
	return uid;
}

/*
 * Called whenever a send right is sent to another node.
 * Port must be locked. Port may be a local or proxy port.
 * Upon return, port is unlocked.
 */
unsigned long
norma_ipc_send_sright(
	ipc_port_t	port)
{
	unsigned long uid;

	/*
	 * Port must be active.
	 */
	assert(ip_active(port));
Retry:
	if (port->ip_norma_is_proxy) {
		/*
		 * A proxy must always have a nonpositive stransit,
		 * so that total stransit is no greater than the
		 * principal's stransit.
		 *
		 * Indeed, a proxy must have a negative stransit as
		 * long as it holds send rights, so that the principal
		 * can trust its stransit (in the face of proxies
		 * sending send rights to it).
		 * If we're here, then we must have send rights.
		 */
		mumble("norma_ipc_send_sright: sender sending sright\n");
		assert(port->ip_srights > 0);
		assert(port->ip_norma_stransit < 0);
		if (port->ip_srights == 1) {
			/*
			 * We are about to lose all send rights.
			 * It is therefore okay to let stransit
			 * drop to zero, as an implicit no-local-senders
			 * notification. (If stransit does not drop
			 * to zero, then we have to do an explicit
			 * no-local-senders notification.) This works
			 * since either we are sending to the receiver,
			 * who will be able to figure it out (since
			 * he will decr stransit accordingly), or
			 * we are not sending to receiver, in which
			 * case no-senders isn't true.
			 */
		} else {
			/*
			 * We will still retain some send rights,
			 * and must therefore keep stransit negative.
			 */
			assert(port->ip_srights > 1);
			assert(port->ip_norma_stransit <= -1);
			if (port->ip_norma_stransit == -1) {
				/*
				 *	Have to get more stransits.
				 *	While we're doing this, the
				 *	port could migrate here and
				 *	no longer be a proxy:  START OVER
				 */
				norma_ipc_stransit_wait(port);
				goto Retry;
			}
		}
		port->ip_norma_stransit++;
		assert(port->ip_norma_stransit <= 0);

		/*
		 * Save uid
		 */
		uid = port->ip_norma_uid;

		/*
		 * Release send right. If this is the last send right,
		 * and stransit is not zero, then we must generate
		 * an explicit no-local-senders notification.
		 */
		if (port->ip_srights == 1) {
		    if (port->ip_norma_stransit != 0) {
		        ip_unlock(port);
		        ipc_port_release_send(port);
		    }
		    else {
			/*
			 * 	Optimize by avoiding no senders notification
			 */
			port->ip_srights--;
			norma_port_remove_try(port); /* consumes lock */
			ipc_port_release(port);
		   }
		} else {
			/*
			 *	Can't be holding last reference
			 */
			port->ip_srights--;
			ip_release(port);
			ip_unlock(port);
		}
	} else {
		/*
		 * The principal can always immediately increase stransit.
		 * He does not release send right here, but rather when
		 * stransit comes back via no-local-senders notification.
		 * This allows the principal's send right count to be an
		 * upper bound on the true number of send rights in the
		 * system, which among other things keeps the a lack of
		 * senders on the principal's node from triggering a
		 * premature no-senders notification.
		 */
		assert(port->ip_norma_stransit >= 0);
		port->ip_norma_stransit++;
		
		/*
		 * If this port has never been exported, assign it a uid
		 * and place it on the norma port list.
		 */
		if (! port->ip_norma_tabled) {
			norma_port_insert(port, FALSE);
		}
		uid = port->ip_norma_uid;
		ip_unlock(port);
		assert(! port->ip_norma_is_proxy);
	}
	assert(reasonable_uid(uid));
	return uid;
}

/*
 * Called whenever a send-once right is sent to another node.
 * Port must be locked. Port must be a local port.
 * Upon return, Port is unlocked.
 */
unsigned long
norma_ipc_send_soright(
	ipc_port_t	port)
{
	unsigned long uid;

	/*
	 * Port must be active.
	 */
	assert(ip_active(port));

	if (port->ip_norma_is_proxy) {
		/*
		 * Save uid
		 */
		uid = port->ip_norma_uid;

		/*
		 * A proxy releases the send-once right.
		 * This case occurs when moving a send-once right
		 * to another node.
		 */
		port->ip_sorights--;
		port->ip_norma_sotransit++;
		norma_port_remove_try(port); /* consumes lock */
		ipc_port_release(port);
	} else {
		/*
		 * The principal does not release send-once right here, but
		 * rather when the send-once right is used as a destination
		 * (of either a reply or a send-once notification).
		 * This allows the principal's send-once right count to
		 * be an accurate count of the true number of send-once
		 * rights in the system.
		 */
		port->ip_norma_sotransit++;

		/*
		 * If this port has never been exported, assign it a uid
		 * and place it on the norma port list.
		 */
		if (! port->ip_norma_tabled) {
			norma_port_insert(port, FALSE);
		}
		uid = port->ip_norma_uid;
		assert(! port->ip_norma_is_proxy);
	}
	ip_unlock(port);
	assert(reasonable_uid(uid));
	return uid;
}

/*
 *	Sending a destination is a two step process.  At the time
 *	the message moves into the norma ipc system, we send the
 *	destination, but keep a reference on the port.  When the
 *	send completes successfully, we release that reference.  At
 *	least, that's how it works for send-once rights.  For send
 *	rights, we have to hold them until the message is successfully
 *	sent in order to avoid premature activation of the no senders
 *	logic.  norma_ipc_prepare_dest() is called at the time of the
 *	initial send, and norma_ipc_send_dest() is called when the
 *	send is known to be successful.  Messages sent as part of
 *	a receive right migration ignore this logic; their destination
 *	rights are managed by code in ipc_migrate.c.
 */

/*
 *	norma_ipc_prepare_dest:
 *
 *	Called at time of initial send.  Does nothing if destination
 *	is a send right.  Consumes a send once right (but not its
 *	reference).  Returns FALSE if it consumed the dest right
 *	(send once case), TRUE if not (send case); the return is
 *	an indication of whethere the right still needs to be consumed.
 *	Port must be locked.
 */

boolean_t
norma_ipc_prepare_dest(
	ipc_port_t		port,
	mach_msg_type_name_t	type_name)
{
	/*
	 *	Port must be a proxy!
	 */
	assert(IP_NORMA_IS_PROXY(port));
	assert(port->ip_norma_uid != 0);

	/*
	 *	Nothing happens now for send rights.
	 */
	if (type_name == MACH_MSG_TYPE_PORT_SEND) {
		return TRUE;
	}

	/*
	 *	Consume send once right, but NOT reference.
	 */
	assert(type_name == MACH_MSG_TYPE_PORT_SEND_ONCE);
	port->ip_sorights--;
	port->ip_norma_sotransit++;
	norma_port_remove_try(port); /* consumes lock */
	assert(port->ip_references > 0);
	ip_lock(port);
	return FALSE;
}

/*
 *	Undo effects of norma_ipc_prepare_dest
 */
void
norma_ipc_unprepare_dest(
	ipc_port_t		port,
	mach_msg_type_name_t	type_name)
{
	if (type_name == MACH_MSG_TYPE_PORT_SEND_ONCE) {
		port->ip_sorights++;
		port->ip_norma_sotransit--;
	}
}

/*
 *	Final step in sending destination.  Message has been
 * 	sent successfully.
 */
void
norma_ipc_send_dest(
	ipc_port_t		port,
	mach_msg_type_name_t	type_name)
{
	/*
	 * Port was a proxy when it was first sent.  Might have
	 * migrated here since then.
	 */
	
	ip_lock(port);
	assert(port->ip_norma_uid != 0);

	/*
	 * This is different from norma_ipc_send_port, because we know
	 * we are sending to receiver. We just need to undo refcount
	 * changes performed by copyin; we don't need to modify
	 * stransit, and neither does the receiver. However, if we lose
	 * all send rights here (e.g., from a move_send destination),
	 * then we must generate an explicit no-local-senders notification.
	 *
	 * In send once case, prepare dest consumed the right, but not
	 * the reference, and we don't get called.
	 */
	assert(type_name == MACH_MSG_TYPE_PORT_SEND);

	if (port->ip_srights == 1) {
		ip_unlock(port);
		printf1("norma_ipc_send_dest.s: release %x\n", port);
		ipc_port_release_send(port);
		printf1("norma_ipc_send_dest.s: released %x\n", port);
	} else {
		port->ip_srights--;
		ip_release(port);
		ip_unlock(port);
	}
}

/*
 * Called when a port right of any flavor is received from another node.
 */
ipc_port_t
norma_ipc_receive_port(
	unsigned long		uid,
	mach_msg_type_name_t	type_name,
	unsigned long		source_node)
{
	/*
	 * Catch null and dead ports.
	 */
	if (uid == IP_NORMA_NULL)
		return IP_NULL;
	if (uid == IP_NORMA_DEAD)
		return IP_DEAD;

	if (type_name == MACH_MSG_TYPE_PORT_SEND_ONCE) {
		return norma_ipc_receive_soright(uid);
	} else if (type_name == MACH_MSG_TYPE_PORT_SEND) {
		return norma_ipc_receive_sright(uid);
	} else if (type_name == MACH_MSG_TYPE_PORT_RECEIVE) {
		return norma_ipc_receive_rright(uid, source_node);
	} else {
		panic("norma_ipc_receive_port: bad type %d\n", type_name);
		return IP_NULL;
	}
}

/*
 * Find or create proxy for given uid, and add a send right reference.
 */
ipc_port_t
norma_ipc_receive_sright(
	unsigned long	uid)
{
	ipc_port_t port;


	/*
	 * Handle the special port case.
	 */
	if (IP_NORMA_SPECIAL(uid)) {
		if (IP_NORMA_NODE(uid) == node_self())
			return(local_special(IP_NORMA_LID(uid),
					     MACH_MSG_TYPE_PORT_SEND));
	        else
			return(remote_special(IP_NORMA_NODE(uid),
					      IP_NORMA_LID(uid),
					      MACH_MSG_TYPE_PORT_SEND));
	}

	/*
	 * Try to find the port.
	 */
	port = norma_port_lookup(uid, TRUE);

	/*
	 * If we don't have a port, then we must create a proxy.
	 */
	if (port == IP_NULL) {
		assert(IP_NORMA_NODE(uid) != node_self());
		port = ipc_port_alloc_special(ipc_space_remote);
		if (port == IP_NULL) {
			panic("receive_sright: ipc_port_alloc_special");
		}
		port->ip_nsrequest = ip_nsproxym(port);
		port->ip_norma_stransit = -1;
		port->ip_srights = 1;
		port->ip_norma_uid = uid;
		port->ip_norma_dest_node = IP_NORMA_NODE(uid);
		assert (port->ip_norma_dest_node != node_self());
		port->ip_norma_is_proxy = TRUE;
		ip_lock(port);
		norma_port_insert(port, TRUE);
		ip_unlock(port);
		return port;
	}

	/*
	 * Is it a proxy?
	 */
	if (port->ip_norma_is_proxy) {
		/*
		 * Just adjust srights and stransit.
		 */
		assert(port->ip_nsrequest != IP_NULL);
		assert(ip_nsproxyp(port->ip_nsrequest));
		assert(ip_active(port)); /* XXX How could it be otherwise? */
		port->ip_srights++;
		port->ip_norma_stransit--;
		ip_reference(port);
		ip_unlock(port);
		return port;
	}

	/*
	 * It is a principal. Is it dead?
	 */
	if (! ip_active(port)) {
		printf1("norma_ipc_receive_sright: dead port %x\n", uid);
		/*
		 * Adjust stransit, since sender used up an stransit in
		 * sending to us. This may enable us to free the port.
		 * Consume immediately the sright associated with stransit.
		 */
		assert(port->ip_norma_stransit > 0);
		assert(port->ip_srights > 0);
		port->ip_norma_stransit--;
		port->ip_srights--;
		norma_port_remove_try(port); /* consumes lock */
		return IP_DEAD;
	}

	/*
	 * It is a living principal.
	 * Decrement stransit, since sender incremented stransit,
	 * since it could not be sure that we were still the principal.
	 * Consume sright that was associated with stransit.
	 */
	assert(port->ip_srights > 0);
	assert(port->ip_norma_stransit > 0);
	port->ip_norma_stransit--;
	ip_unlock(port);
	return port;
}

/*
 * Find or create proxy for given uid, and add a send-once right reference.
 */
ipc_port_t
norma_ipc_receive_soright(
	unsigned long	uid)
{
	ipc_port_t port;

	/*
	 * Handle the special port case.
	 */
	if (IP_NORMA_SPECIAL(uid)) {
		if (IP_NORMA_NODE(uid) == node_self())
			return(local_special(IP_NORMA_LID(uid),
					     MACH_MSG_TYPE_PORT_SEND_ONCE));
	        else
			return(remote_special(IP_NORMA_NODE(uid),
					      IP_NORMA_LID(uid),
					      MACH_MSG_TYPE_PORT_SEND_ONCE));
	}
	/*
	 * Try to find the port.
	 */
	port = norma_port_lookup(uid, TRUE);

	/*
	 * If we don't have a port, then we must create a proxy.
	 */
	if (port == IP_NULL) {
		assert(IP_NORMA_NODE(uid) != node_self());
		port = ipc_port_alloc_special(ipc_space_remote);
		if (port == IP_NULL) {
			panic("receive_soright: ipc_port_alloc_special");
		}
		port->ip_nsrequest = ip_nsproxym(port);
		port->ip_norma_sotransit = -1;
		port->ip_sorights = 1;
		port->ip_norma_uid = uid;
		port->ip_norma_dest_node = IP_NORMA_NODE(uid);
		assert (port->ip_norma_dest_node != node_self());
		port->ip_norma_is_proxy = TRUE;
		ip_lock(port);
		norma_port_insert(port, TRUE);
		ip_unlock(port);
		return port;
	}

	/*
	 * Is it a proxy?
	 */
	if (port->ip_norma_is_proxy) {
		/*
		 * Just adjust sorights.
		 * We increment, because we are inheriting a remote count.
		 */
		assert(port->ip_nsrequest != IP_NULL);
		assert(ip_nsproxyp(port->ip_nsrequest));
		assert(ip_active(port)); /* XXX How could it be otherwise? */
		port->ip_sorights++;
		port->ip_norma_sotransit--;
		ip_reference(port);
		ip_unlock(port);
		return port;
	}

	/*
	 * It is a principal. Is it dead?
	 */
	if (! ip_active(port)) {
		/*
		 * We decrement sorights, because it has come home.
		 */
		printf1("norma_ipc_receive_soright: dead port %x\n", uid);
		port->ip_sorights--;
		port->ip_norma_sotransit--;
		assert((long) port->ip_sorights >= 0);
		assert(port->ip_norma_sotransit >= 0);
		norma_port_remove_try(port); /* consumes lock */
		return IP_DEAD;
	}

	/*
	 * It is a living principal.
	 * Leave sorights alone; we consume the one we left for remote node.
	 */
	port->ip_norma_sotransit--;
	assert((long) port->ip_sorights >= 0);
	assert(port->ip_norma_sotransit >= 0);
	ip_reference(port);
	ip_unlock(port);
	return port;
}

/*
 * Find destination port for given uid.  ack_error_data is the data to send
 * back with the ack if the receive fails.
 */
kern_return_t
norma_ipc_receive_dest(
	unsigned long		uid,
	mach_msg_type_name_t	type_name,
	ipc_port_t		*remote_port,
        unsigned long		*ack_error_data)
{
	ipc_port_t port;

	/*
	 * This can only be some flavor of send or send-once right.
	 */
	assert(type_name == MACH_MSG_TYPE_PORT_SEND ||
	       type_name == MACH_MSG_TYPE_PORT_SEND_ONCE);

	/*
	 * Handle the special port case.
	 */
	if (IP_NORMA_SPECIAL(uid)) {
		if (IP_NORMA_NODE(uid) == node_self()) {
			*remote_port = local_special(IP_NORMA_LID(uid),
						     type_name);
		} else {
			*remote_port = remote_special(IP_NORMA_NODE(uid),
						      IP_NORMA_LID(uid),
						      type_name);
		}
		return KERN_SUCCESS;
	}

	/*
	 * Find associated port.
	 */
	port = norma_port_lookup(uid, FALSE);
	if (port == IP_NULL) {
		/*
		 * Must be an invalid uid; otherwise, we would have
		 * found something with norma_port_lookup.
		 */
		panic("norma_ipc_receive_dest: invalid uid %x!\n", uid);
		*remote_port = IP_NULL;
		*ack_error_data = uid;
		return KERN_INVALID_NAME;
	}

	/*
	 * Check to see whether we are the correct receiver for this message.
	 * If not, try to say where the correct receiver would be.
	 */
	if (port->ip_norma_is_proxy) {
		/*
		 * Tell sender to use new node.
		 */
		printf1("norma_ipc_receive_dest: migrated dest %d -> %d\n",
		       node_self(), port->ip_norma_dest_node);
		*remote_port = IP_NULL;
		*ack_error_data = port->ip_norma_dest_node;
		ip_unlock(port);
		return KERN_NOT_RECEIVER;
	}

	/*
	 * We have a local principal. Make sure it is active.
	 */
	if (! ip_active(port)) {
		/*
		 * If it is not active, it is a dead port, kept alive
		 * by remote send and send-once references.
		 */
		printf1("norma_ipc_receive_dest: dead port %x\n", uid);

		/*
		 *	Sender is responsible for all cleanup.
		 */

		*remote_port = IP_NULL;
		*ack_error_data = uid;
		ip_unlock(port);
		return KERN_INVALID_RIGHT;
	}
	
	/*
	 * Check queue limit.
	 */
#if 1
	/*
	 * There are locking issues here that need to be adressed.
	 * In the meantime, don't even bother looking at ip_msgcount.
	 */
#else
	if (port->ip_msgcount >= port->ip_qlimit) {
		mumble("norma_ipc_receive_dest: queue=%d >= limit=%d uid=%x\n",
		       port->ip_msgcount, port->ip_qlimit, uid);
		/*
		 * XXX
		 * Should tell sender to block, and remember to wake him up.
		 */
	}
#endif
	
	/*
	 * Return port. Simulate copyin.
	 */
	if (type_name == MACH_MSG_TYPE_PORT_SEND) {
		/*
		 * Create a send right reference.
		 */
		ip_reference(port);
		port->ip_srights++;
	} else {
		/*
		 * Consume a preexisting send-once reference,
		 * created when send-once right was sent to current sender.
		 */
		assert(type_name == MACH_MSG_TYPE_PORT_SEND_ONCE);
		assert(port->ip_norma_sotransit > 0);
		port->ip_norma_sotransit--;
	}
	*remote_port = port;
	ip_unlock(port);
	return KERN_SUCCESS;
}

kern_return_t
norma_ipc_receive_migrating_dest(
	unsigned long		uid,
	mach_msg_type_name_t	type_name,
	ipc_port_t		*remote_port,
        unsigned long		*ack_error_data)
{
	ipc_port_t port, atrium;

	assert(! IP_NORMA_SPECIAL(uid));

	/*
	 * Find associated port.
	 * It must be a proxy with an atrium.
	 */
	port = norma_port_lookup(uid, FALSE);
	if (port == IP_NULL) {
		panic("norma_ipc_receive_migrating_dest: invalid uid %x!\n",
		      uid);
		*remote_port = IP_NULL;
		*ack_error_data = uid;
		return KERN_INVALID_RIGHT;
	}
	if (port->ip_norma_is_proxy) {
		panic("norma_ipc_receive_migrating_dest: %x is proxy!\n",
		      uid);
		*remote_port = IP_NULL;
		*ack_error_data = uid;
		ip_unlock(port);
		return KERN_INVALID_RIGHT;
	}
	if ((atrium = port->ip_norma_atrium) == IP_NULL) {
		panic("norma_ipc_receive_migrating_dest: %x not migrating!\n",
		      uid);
		*remote_port = IP_NULL;	
		*ack_error_data = uid;
		ip_unlock(port);
		return KERN_INVALID_RIGHT;
	}
	assert(ip_active(port));
	ip_unlock(port);

	/*
	 * Return port. Put destination right in atrium.
	 * It'll be moved later.  Don't need references for atrium.
	 */
	ip_lock(atrium);
	if (type_name == MACH_MSG_TYPE_PORT_SEND) {
		atrium->ip_srights++;
	} else {
		assert(type_name == MACH_MSG_TYPE_PORT_SEND_ONCE);
		atrium->ip_sorights++;
	}
	ip_unlock(atrium);
	*remote_port = port;
	return KERN_SUCCESS;
}

/*
 * Called from ipc_port_destroy.
 */
void
norma_ipc_port_destroy(
	ipc_port_t	port)
{
	ipc_port_t	root;
	kern_return_t	kr;

	/*
	 * This routine cannot be called on a migrating port, since
	 * the receive right is held by the kernel.
	 * However, it's hard for us to tell if a port is migrating,
	 * and thus hard to assert anything here.
	 */

	/*
	 * If the port migrated here, update the port's root node's
	 * notion of the port's location before destroying it.
	 * Otherwise, message loops can result as follows.
	 *	- root node sends or forwards a message to the
	 *	  node last thought to have the port
	 *	- the node thought to have the port has destroyed
	 *	  its proxy, so it forwards the message back to
	 *	  the port's root node
	 * By updating the root node, messages will ultimately
	 * be forwarded to this node, which knows what to do.
	 *
	 * If there are no remote send rights nor remote send once rights,
	 * it is not worth to update the port's root node.
	 */
	if (!IP_NORMA_IS_PROXY(port) &&
	    port->ip_norma_uid != 0 &&
	    (port->ip_norma_stransit > 0 || port->ip_norma_sotransit > 0) &&
	    IP_NORMA_NODE(port->ip_norma_uid) != node_self()) {
		root = remote_host_priv(IP_NORMA_NODE(port->ip_norma_uid),
					MACH_MSG_TYPE_PORT_SEND);
		kr = r_norma_ipc_node_set(root,
					  port->ip_norma_uid,
					  node_self());
		assert(kr == KERN_SUCCESS);
	}

	/*
	 * If this is a special port (principal), remove it from the
	 * special port table. Note that it may be in more than one slot.
	 * Release send right reference for each slot.
	 *
	 * No-senders is never true as long as a port is in the list,
	 * since the list holds a send right for each occurance of the
	 * port in the list.
	 */
	if (port->ip_norma_is_special) {
		norma_unset_special_port(port);
		assert(port->ip_norma_is_special == FALSE);
	}

	/*
	 * See if port can be removed from norma system.  
	 * norma_port_remove_try knows how to cope with proxies.
	 */
	ip_lock(port);
	norma_port_remove_try(port); /* consumes lock */
}

/*
 * Upon call, port is locked and unlocked upon return.
 */
void
norma_ipc_notify_no_senders(
	ipc_port_t	port)
{
	unsigned long	dest_node;
	unsigned long	uid;
	int		stransit;

	assert(port->ip_nsrequest == IP_NULL);
	assert(port->ip_norma_is_proxy);
	assert(port->ip_norma_stransit <= 0);
	uid = port->ip_norma_uid;
	dest_node = port->ip_norma_dest_node;
	stransit = -port->ip_norma_stransit;
	port->ip_norma_stransit = 0;
	printf1("notify no_senders(0x%x:%x) s=0 so=%d\n",
	       port, uid, port->ip_sorights);
	assert(port->ip_srights == 0);

	/*
	 * There are no local send rights...
	 * if there are also no local send-once rights, then destroy
	 * the proxy; otherwise, rearm nsrequest in case this proxy
	 * acquires send rights in the future.
	 */
	if (port->ip_sorights == 0) {
		assert(IP_NORMA_SPECIAL(uid) || port->ip_norma_sotransit == 0);
		norma_port_remove(port); /* consumes lock */
	} else {
		port->ip_nsrequest = ip_nsproxym(port);
		ip_unlock(port);
	}
	if (IP_NORMA_SPECIAL(uid)) {
		/*
		 * Don't generate no-senders notifications for special uids.
		 * Remote node wouldn't even know what to do with one.
		 */
		printf1("norma_ipc_notify_no_senders: special port %x\n", uid);
		return;
	}
	if (stransit > 0) 
		norma_ipc_send_yield_transits(dest_node, uid, stransit, 0);
}

void
norma_ipc_yield_transits(
	host_t		host_priv,
	unsigned long	uid,
	int		stransit,
	int		sotransit)
{
	ipc_port_t port;
	unsigned long node;
	ipc_port_t nsrequest = IP_NULL;
	mach_port_mscount_t mscount;
	TR_DECL("norma_ipc_yield_transits");

	assert (stransit > 0 || sotransit > 0);

	if (host_priv == HOST_NULL) {
		fret("norma_ipc_yield_transits: invalid host\n");
		return;
	}
	port = norma_port_lookup(uid, FALSE);
	if (port == IP_NULL) {
		if (IP_NORMA_NODE(uid) == node_self()) {
			/*
			 * XXX
			 * This can happen if this node rebooted.
			 * Otherwise, it is supposed to have saved
			 * a port for forwarding purposes.
			 */
			printf("norma_ipc_yield_transits: failed lookup!\n");
			panic("uid %x stransit %d\n", uid, stransit);
		} else {
			tr2("trying node %d", IP_NORMA_NODE(uid));
			norma_ipc_send_yield_transits(IP_NORMA_NODE(uid),
						uid, stransit, sotransit);
		}
		return;
	}
	printf1("norma_ipc_yield_transits(uid=%x port=0x%x stransit=%d)\n",
	     uid, port, stransit);
	if (port->ip_norma_is_proxy) {
	        /*
	         * Port must be a forwarding pointer.  Pass request along.
	         */
	        assert(port->ip_norma_forward);
		node = port->ip_norma_dest_node;
		ip_unlock(port);
		norma_ipc_send_yield_transits(node, uid, stransit, sotransit);
		return;
	}
	if (sotransit > 0) {
		assert(!ip_active(port));
		assert(port->ip_norma_sotransit >= sotransit);
		assert(port->ip_sorights >= sotransit);
		port->ip_norma_sotransit -= sotransit;
		port->ip_sorights -= sotransit;
		port->ip_references -= sotransit;
	}
	if (stransit > 0) {
		assert(port->ip_norma_stransit >= stransit);
		assert(port->ip_srights >= stransit);
		port->ip_norma_stransit -= stransit;
		port->ip_srights -= stransit;
		port->ip_references -= stransit;
		if (ip_active(port) && port->ip_srights == 0) {
			assert((port->ip_norma_stransit == 0));
			assert(!ip_nsproxyp(port->ip_nsrequest));
			nsrequest = port->ip_nsrequest;
			port->ip_nsrequest = IP_NULL;
			mscount = port->ip_mscount;
		}
	}

	assert(port->ip_references > 0);

	if (ip_active(port)) {
		ip_unlock(port);
		if (nsrequest != IP_NULL)
			ipc_notify_no_senders(nsrequest, mscount);
		return;
	}

	/*
	 *	If all the norma rights are now in, and this is
	 *	a dead migrated principal, then we can release the
	 *	forwarding proxy on the origin node.  This is done
	 *	via a dead name notification with release_forward
	 *	set to TRUE.
	 *
	 *	If needed, the port may be removed from the NORMA port list
	 *	(and perhaps destroyed) before the dead name notification,
	 *	since the dead name notification won't send us back anything.
	 */
	if (port->ip_norma_stransit == 0 && port->ip_norma_sotransit == 0) {
		node = IP_NORMA_NODE(uid);
		if (port->ip_norma_tabled)
			norma_port_remove(port); /* consumes lock */
		else
			ip_check_unlock(port);
		if (node != node_self())
			r_norma_ipc_node_dnnotify(
				remote_host_priv(node,
						 MACH_MSG_TYPE_PORT_SEND),
				uid,
				node_self(),
				TRUE);
	} else
		ip_check_unlock(port);
}

/*
 * An obvious improvement here would be to somehow, in the initial
 * sending of send rights from receiver to sender, pass along some
 * initial negative stransit. The current scheme is a lose for send
 * rights that are used as capabilities, i.e., passed in the body
 * of the message instead of being used as a destination.
 */
void
norma_ipc_stransit_wait(
	ipc_port_t	port)
{
	kern_return_t kr;
	int stransit;
	unsigned retarget_node = port->ip_norma_dest_node;

	/*
	 * This loop acts as a retarget mechanism until it succeeds
	 * or fails altogether.
	 */
	do {
	        ip_unlock(port);
	        kr = norma_ipc_send_stransit_request(retarget_node,
						     port->ip_norma_uid,
						     &stransit,
						     &retarget_node);

		ip_lock(port);
	        if (kr != KERN_SUCCESS) {
		    panic("norma_ipc_stransit_wait: stransit_request: %d/%x\n",
			  kr, kr);
		    return;
		}
		if (retarget_node != -1) {

		    /*
		     * update our notion of where the principal now is
		     */
		    if (retarget_node == node_self()) {

		    	/*
			 *	It's here, hence we don't need any
			 *	stransits.  Caller will retry and
			 *	do the right thing.
			 */
		        return;
		    }
		    else {
			port->ip_norma_dest_node = retarget_node;
		    }
		}
	} while (retarget_node != -1);

	/*
	 *	While we were getting the stransits, the node
	 *	could have migrated here.
	 */
	if (IP_NORMA_IS_PROXY(port)) {

		/*
		 * Still a proxy, common case.
		 */
		mumble("ip_stransit_wait: stransit += %d\n", stransit);
		port->ip_norma_stransit -= stransit;
	}
	else {

		/*
		 *	Now we're the principal.  Undo effects of getting
		 *	the stransits (that we don't need any more).
		 */
		port->ip_norma_stransit -= stransit;
		port->ip_srights -= stransit;
		port->ip_references -= stransit;
		mumble("ipc_stransit_wait: changed to principal\n");
	}
}

kern_return_t
norma_ipc_stransit_request(
	host_t		host_priv,
	unsigned	uid,
	int		*stransitp,
        unsigned	*retarget_node)
{
	ipc_port_t port;

	mumble("norma_ipc_stransit_request: called\n");
	*retarget_node = 0xFFFFFFFF;
	if (host_priv == HOST_NULL) {
		fret("norma_ipc_stransit_request: invalid host\n");
		return KERN_INVALID_HOST;
	}
	port = norma_port_lookup(uid, FALSE);
	if (port == IP_NULL) {
		fret("norma_ipc_stransit_request: failed lookup %x\n",
		     uid);
		return KERN_INVALID_NAME;
	}
	if (port->ip_norma_is_proxy) {
	        if (port->ip_norma_forward) {
			/*
			 * Just retarget.  Don't think about trying to
			 * dispose of this forwarding proxy; the norma
			 * dead name request that was left at migrate time
			 * will take care of it.
			 */
			*retarget_node = port->ip_norma_dest_node;
			ip_unlock(port);
			return KERN_SUCCESS;
		 } else {
		/* XXX should probably forward to current principal */
		        ip_unlock(port);
			fret("norma_ipc_stransit_request: is proxy\n");
		        return KERN_INVALID_RIGHT;
		 }
	}
	if (! ip_active(port)) {
		ip_unlock(port);
		fret("norma_ipc_stransit_request: is not active\n");
		return KERN_FAILURE;
	}
	*stransitp = 500;
	port->ip_norma_stransit += *stransitp;
	port->ip_srights += *stransitp;
	port->ip_references += *stransitp;
	ip_unlock(port);
	mumble("norma_ipc_stransit_request: success\n");
	return KERN_SUCCESS;
}


/*
 *	Update root node's notion of port location.
 */
kern_return_t
norma_ipc_node_set(
	host_t		host_priv,
	unsigned	uid,
	unsigned	node)
{
	ipc_port_t	port;
	TR_DECL("norma_ipc_node_set");

	tr4("host_priv 0x%x uid 0x%x node 0x%x", host_priv, uid, node);
	assert(IP_NORMA_NODE(uid) == node_self());
	assert(host_priv != HOST_NULL);

	port = norma_port_lookup(uid, FALSE);
	if (port != IP_NULL) {
		assert(port->ip_norma_forward == TRUE);
		assert(IP_NORMA_IS_PROXY(port));

		port->ip_norma_dest_node = node;
		ip_unlock(port);
	}

	return KERN_SUCCESS;
}
