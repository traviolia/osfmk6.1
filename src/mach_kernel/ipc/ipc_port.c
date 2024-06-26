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
 * Revision 2.13.2.7  92/06/24  18:00:04  jeffreyh
 * 	Add norma send once right management hook to ipc_port_release_sonce.
 * 	[92/06/08            dlb]
 * 
 * Revision 2.13.2.6  92/05/27  00:45:00  jeffreyh
 * 	Call NORMA hooks (norma_ipc_dnrequest_init) when first allocating
 * 	a dnrequest table for a port, and when encountering a NORMA fake
 * 	notification in ipc_port_destroy.
 * 	[92/05/25            dlb]
 * 
 * 	Initialize new fields of port structure, add them to debugger
 * 	show display.
 * 	[92/05/19            sjs]
 * 
 * 	Call norma_ipc_finish_receiving before destroying kmsgs in 
 * 	ipc_port_destroy because the kmsgs might still be in network format.
 * 	[92/05/12            dlb]
 * 
 * Revision 2.13.2.5.1.1  92/05/06  17:47:00  jeffreyh
 * 	Initialize new ip_norma_atrium_waiter field.  Add a norma assert.
 * 	[92/05/05            dlb]
 * 
 * Revision 2.13.2.5  92/04/08  15:44:39  jeffreyh
 * 	Set sequence number to zero before destroying port
 * 	in ipc_port_dealloc_special.
 * 	[92/04/01            dlb]
 * 
 * Revision 2.13.2.4  92/03/28  10:09:27  jeffreyh
 * 	Do norma_ipc_port_destroy call after port is deactivated.
 * 	[92/03/25            dlb]
 * 
 * Revision 2.13.2.3  92/02/21  14:35:39  jsb
 * 	In ipc_port_alloc_special, throw debugging info into spare fields.
 * 	[92/02/20  10:26:50  jsb]
 * 
 * 	Removed ip_norma_queued. Changed initialization of ip_norma_queue_next.
 * 	Added ip_norma_spare[1234].
 * 	[92/02/18  08:13:02  jsb]
 * 
 * 	Initialize and print ip_norma_xmm_object_refs field.
 * 	[92/02/16  16:00:11  jsb]
 * 
 * 	Removed ipc_port_move_special routine. Added ip_norma_xmm_object field.
 * 	[92/02/09  12:45:37  jsb]
 * 
 * Revision 2.13.2.2  92/01/21  21:50:31  jsb
 * 	Added ipc_port_move_special. (dlb@osf.org)
 * 	[92/01/17  14:29:16  jsb]
 * 
 * 	Removed norma_list_all_{ports,seqnos} hacks in ipc_print_port.
 * 	Initialize port->ip_norma_next = port (vs. IP_NULL) for new
 * 	norma/ipc_list.c implementation.
 * 	[92/01/16  21:30:02  jsb]
 * 
 * Revision 2.13.2.1  92/01/03  16:35:42  jsb
 * 	Incorporated 'show port 1' hack for listing more norma port info.
 * 	[91/12/31  17:10:21  jsb]
 * 
 * 	Added support for ip_norma_{queued,queue_next}.
 * 	Made ipc_port_print more consistent.
 * 	[91/12/27  17:16:38  jsb]
 * 
 * 	Removed ip_norma_{wanted,migrating}; added ip_norma_{sotransit,atrium}.
 * 	Incorporated 'show port 0' hack for listing all norma ports.
 * 	[91/12/26  20:00:09  jsb]
 * 
 * 	Removed references to obsolete NORMA_IPC fields in struct ipc_port.
 * 	Corrected log.
 * 	[91/12/24  14:39:54  jsb]
 * 
 * Revision 2.13  91/12/14  14:27:37  jsb
 * 	Removed ipc_fields.h hack.
 * 
 * Revision 2.12  91/11/14  16:56:14  rpd
 * 	Added ipc_fields.h support to ipc_port_{init,print}.
 *	Call norma_ipc_port_destroy instead of norma_ipc_destroy.
 * 	[91/11/00            jsb]
 * 
 * Revision 2.11  91/10/09  16:09:42  af
 * 	Removed unused variable.
 * 	[91/09/16  09:42:52  rpd]
 * 
 * Revision 2.10  91/08/28  11:13:44  jsb
 * 	Added ip_seqno and ipc_port_set_seqno.
 * 	Changed ipc_port_init to initialize ip_seqno.
 * 	Changed ipc_port_clear_receiver to zero ip_seqno.
 * 	[91/08/09            rpd]
 * 	Renamed clport fields in struct ipc_port to ip_norma fields.
 * 	[91/08/15  08:20:08  jsb]
 * 
 * Revision 2.9  91/08/03  18:18:30  jsb
 * 	Call norma_ipc_destroy when destroying port.
 * 	Added clport fields to ipc_port_print.
 * 	[91/07/24  22:14:01  jsb]
 * 
 * 	Fixed include. Changed clport field initialization.
 * 	[91/07/17  14:05:38  jsb]
 * 
 * Revision 2.8  91/06/17  15:46:21  jsb
 * 	Renamed NORMA conditionals.
 * 	[91/06/17  10:44:21  jsb]
 * 
 * Revision 2.7  91/05/14  16:35:22  mrt
 * 	Correcting copyright
 * 
 * Revision 2.6  91/03/16  14:48:27  rpd
 * 	Renamed ipc_thread_go to thread_go.
 * 	[91/02/17            rpd]
 * 
 * Revision 2.5  91/02/05  17:23:02  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  15:49:46  mrt]
 * 
 * Revision 2.4  90/11/05  14:29:30  rpd
 * 	Changed ip_release to ipc_port_release.
 * 	Use new ip_reference and ip_release.
 * 	[90/10/29            rpd]
 * 
 * Revision 2.3  90/09/28  16:55:10  jsb
 * 	Added NORMA_IPC support.
 * 	[90/09/28  14:03:45  jsb]
 * 
 * Revision 2.2  90/06/02  14:51:08  rpd
 * 	Created for new IPC.
 * 	[90/03/26  21:01:02  rpd]
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
 *	File:	ipc/ipc_port.c
 *	Author:	Rich Draves
 *	Date:	1989
 *
 *	Functions to manipulate IPC ports.
 */

#include <mach_kdb.h>
#include <zone_debug.h>

#include <mach/port.h>
#include <mach/kern_return.h>
#include <kern/lock.h>
#include <kern/ipc_sched.h>
#include <kern/ipc_kobject.h>
#include <kern/ipc_subsystem.h>
#include <kern/thread_pool.h>
#include <kern/misc_protos.h>
#include <ipc/ipc_entry.h>
#include <ipc/ipc_space.h>
#include <ipc/ipc_object.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_pset.h>
#include <ipc/ipc_thread.h>
#include <ipc/ipc_mqueue.h>
#include <ipc/ipc_notify.h>
#include <ipc/ipc_print.h>
#include <ipc/ipc_table.h>
#include <string.h>
#if	NORMA_IPC
#include <norma/ipc_node.h>
#endif	/* NORMA_IPC */

mutex_t			ipc_port_multiple_lock_data;
mutex_t			ipc_port_timestamp_lock_data;
ipc_port_timestamp_t	ipc_port_timestamp_data;

#if	MACH_KDB

#if	ZONE_DEBUG
/* Forwards */
void	print_type_ports(unsigned, unsigned);
void	print_ports(void);
#endif	/* ZONE_DEBUG */

int	port_count = 0;
int	port_count_warning = 0;

void port_allocated(void);
void port_deallocated(void);

void
port_allocated(void)
{
	assert(port_count >= 0);
	++port_count;
	if (port_count_warning > 0 && port_count >= port_count_warning)
		assert(port_count < port_count_warning);
}

void
port_deallocated(void)
{
	assert(port_count >= 0);
}
#endif	/* MACH_KDB */

/*
 *	Routine:	ipc_port_timestamp
 *	Purpose:
 *		Retrieve a timestamp value.
 */

ipc_port_timestamp_t
ipc_port_timestamp(void)
{
	ipc_port_timestamp_t timestamp;

	ipc_port_timestamp_lock();
	timestamp = ipc_port_timestamp_data++;
	ipc_port_timestamp_unlock();

	return timestamp;
}

/*
 *	Routine:	ipc_port_dnrequest
 *	Purpose:
 *		Try to allocate a dead-name request slot.
 *		If successful, returns the request index.
 *		Otherwise returns zero.
 *	Conditions:
 *		The port is locked and active.
 *	Returns:
 *		KERN_SUCCESS		A request index was found.
 *		KERN_NO_SPACE		No index allocated.
 */

kern_return_t
ipc_port_dnrequest(
	ipc_port_t			port,
	mach_port_t			name,
	ipc_port_t			soright,
	ipc_port_request_index_t	*indexp)
{
	ipc_port_request_t ipr, table;
	ipc_port_request_index_t index;

	assert(ip_active(port));
	assert(name != MACH_PORT_NULL);
	assert(soright != IP_NULL);

	table = port->ip_dnrequests;
	if (table == IPR_NULL)
		return KERN_NO_SPACE;

	index = table->ipr_next;
	if (index == 0)
		return KERN_NO_SPACE;

	ipr = &table[index];
	assert(ipr->ipr_name == MACH_PORT_NULL);

	table->ipr_next = ipr->ipr_next;
	ipr->ipr_name = name;
	ipr->ipr_soright = soright;

	*indexp = index;
	return KERN_SUCCESS;
}

/*
 *	Routine:	ipc_port_dngrow
 *	Purpose:
 *		Grow a port's table of dead-name requests.
 *	Conditions:
 *		The port must be locked and active.
 *		Nothing else locked; will allocate memory.
 *		Upon return the port is unlocked.
 *	Returns:
 *		KERN_SUCCESS		Grew the table.
 *		KERN_SUCCESS		Somebody else grew the table.
 *		KERN_SUCCESS		The port died.
 *		KERN_RESOURCE_SHORTAGE	Couldn't allocate new table.
 *		KERN_NO_SPACE		Couldn't grow to desired size
 */

kern_return_t
ipc_port_dngrow(
	ipc_port_t	port,
	int		target_size)
{
	ipc_table_size_t its;
	ipc_port_request_t otable, ntable;

	assert(ip_active(port));

	otable = port->ip_dnrequests;
	if (otable == IPR_NULL)
		its = &ipc_table_dnrequests[0];
	else
		its = otable->ipr_size + 1;

	if (target_size != ITS_SIZE_NONE) {
		if ((otable != IPR_NULL) &&
		    (target_size <= otable->ipr_size->its_size)) {
			ip_unlock(port);
			return KERN_SUCCESS;
	        }
		while ((its->its_size) && (its->its_size < target_size)) {
			its++;
		}
		if (its->its_size == 0) {
			ip_unlock(port);
			return KERN_NO_SPACE;
		}
	}

	ip_reference(port);
	ip_unlock(port);

	if ((its->its_size == 0) ||
	    ((ntable = it_dnrequests_alloc(its)) == IPR_NULL)) {
		ipc_port_release(port);
		return KERN_RESOURCE_SHORTAGE;
	}

	ip_lock(port);
	ip_release(port);

	/*
	 *	Check that port is still active and that nobody else
	 *	has slipped in and grown the table on us.  Note that
	 *	just checking port->ip_dnrequests == otable isn't
	 *	sufficient; must check ipr_size.
	 */

	if (ip_active(port) &&
	    (port->ip_dnrequests == otable) &&
	    ((otable == IPR_NULL) || (otable->ipr_size+1 == its))) {
		ipc_table_size_t oits;
		ipc_table_elems_t osize, nsize;
		ipc_port_request_index_t free, i;

		/* copy old table to new table */

		if (otable != IPR_NULL) {
			oits = otable->ipr_size;
			osize = oits->its_size;
			free = otable->ipr_next;

			(void) memcpy((void *)(ntable + 1),
			      (const void *)(otable + 1),
			      (osize - 1) * sizeof(struct ipc_port_request));
		} else {
			osize = 1;
			free = 0;
		}

		nsize = its->its_size;
		assert(nsize > osize);

		/* add new elements to the new table's free list */

		for (i = osize; i < nsize; i++) {
			ipc_port_request_t ipr = &ntable[i];

			ipr->ipr_name = MACH_PORT_NULL;
			ipr->ipr_next = free;
			free = i;
		}

		ntable->ipr_next = free;
		ntable->ipr_size = its;
		port->ip_dnrequests = ntable;
		ip_unlock(port);

		if (otable != IPR_NULL) {
			it_dnrequests_free(oits, otable);
	        }
#if	NORMA_IPC
	        else {
			norma_ipc_dnrequest_init(port);
	       }
#endif	/* NORMA_IPC */
	} else {
		ip_check_unlock(port);
		it_dnrequests_free(its, ntable);
	}

	return KERN_SUCCESS;
}
 
/*
 *	Routine:	ipc_port_dncancel
 *	Purpose:
 *		Cancel a dead-name request and return the send-once right.
 *	Conditions:
 *		The port must locked and active.
 */

ipc_port_t
ipc_port_dncancel(
	ipc_port_t			port,
	mach_port_t			name,
	ipc_port_request_index_t	index)
{
	ipc_port_request_t ipr, table;
	ipc_port_t dnrequest;

	assert(ip_active(port));
	assert(name != MACH_PORT_NULL);
	assert(index != 0);

	table = port->ip_dnrequests;
	assert(table != IPR_NULL);

	ipr = &table[index];
	dnrequest = ipr->ipr_soright;
	assert(ipr->ipr_name == name);

	/* return ipr to the free list inside the table */

	ipr->ipr_name = MACH_PORT_NULL;
	ipr->ipr_next = table->ipr_next;
	table->ipr_next = index;

	return dnrequest;
}

/*
 *	Routine:	ipc_port_pdrequest
 *	Purpose:
 *		Make a port-deleted request, returning the
 *		previously registered send-once right.
 *		Just cancels the previous request if notify is IP_NULL.
 *	Conditions:
 *		The port is locked and active.  It is unlocked.
 *		Consumes a ref for notify (if non-null), and
 *		returns previous with a ref (if non-null).
 */

void
ipc_port_pdrequest(
	ipc_port_t	port,
	ipc_port_t	notify,
	ipc_port_t	*previousp)
{
	ipc_port_t previous;

	assert(ip_active(port));

	previous = port->ip_pdrequest;
	port->ip_pdrequest = notify;
	ip_unlock(port);

	*previousp = previous;
}

/*
 *	Routine:	ipc_port_nsrequest
 *	Purpose:
 *		Make a no-senders request, returning the
 *		previously registered send-once right.
 *		Just cancels the previous request if notify is IP_NULL.
 *	Conditions:
 *		The port is locked and active.  It is unlocked.
 *		Consumes a ref for notify (if non-null), and
 *		returns previous with a ref (if non-null).
 */

void
ipc_port_nsrequest(
	ipc_port_t		port,
	mach_port_mscount_t	sync,
	ipc_port_t		notify,
	ipc_port_t		*previousp)
{
	ipc_port_t previous;
	mach_port_mscount_t mscount;

	assert(ip_active(port));

	previous = port->ip_nsrequest;
	mscount = port->ip_mscount;

	if ((port->ip_srights == 0) &&
	    (sync <= mscount) &&
	    (notify != IP_NULL)) {
		port->ip_nsrequest = IP_NULL;
		ip_unlock(port);
		ipc_notify_no_senders(notify, mscount);
	} else {
		port->ip_nsrequest = notify;
		ip_unlock(port);
	}

	*previousp = previous;
}

/*
 *	Routine:	ipc_port_set_qlimit
 *	Purpose:
 *		Changes a port's queue limit; the maximum number
 *		of messages which may be queued to the port.
 *	Conditions:
 *		The port is locked and active.
 */

void
ipc_port_set_qlimit(
	ipc_port_t		port,
	mach_port_msgcount_t	qlimit)
{
	assert(ip_active(port));
	assert(qlimit <= MACH_PORT_QLIMIT_MAX);

	/* wake up senders allowed by the new qlimit */

	if (qlimit > port->ip_qlimit) {
		mach_port_msgcount_t i, wakeup;

		/* caution: wakeup, qlimit are unsigned */

		wakeup = qlimit - port->ip_qlimit;

		for (i = 0; i < wakeup; i++) {
			ipc_thread_t th;

			th = ipc_thread_dequeue(&port->ip_blocked);
			if (th == ITH_NULL)
				break;

			th->ith_state = MACH_MSG_SUCCESS;
			thread_go(th);
		}
	}

	port->ip_qlimit = qlimit;
}

/*
 *	Routine:	ipc_port_set_seqno
 *	Purpose:
 *		Changes a port's sequence number.
 *	Conditions:
 *		The port is locked and active.
 */

void
ipc_port_set_seqno(
	ipc_port_t		port,
	mach_port_seqno_t	seqno)
{
	if (port->ip_pset != IPS_NULL) {
		ipc_pset_t pset = port->ip_pset;

		ips_lock(pset);
		if (!ips_active(pset)) {
			ipc_pset_remove(pset, port);
			ips_check_unlock(pset);
			goto no_port_set;
		} else {
			imq_lock(&pset->ips_messages);
			port->ip_seqno = seqno;
			imq_unlock(&pset->ips_messages);
		}
	} else {
	    no_port_set:
		imq_lock(&port->ip_messages);
		port->ip_seqno = seqno;
		imq_unlock(&port->ip_messages);
	}
}

/*
 *	Routine:	ipc_port_clear_receiver
 *	Purpose:
 *		Prepares a receive right for transmission/destruction.
 *	Conditions:
 *		The port is locked and active.
 */

void
ipc_port_clear_receiver(
	ipc_port_t	port)
{
	ipc_pset_t pset;

	assert(ip_active(port));

	pset = port->ip_pset;
	if (pset != IPS_NULL) {
		/* No threads receiving from port, but must remove from set. */

		ips_lock(pset);
		ipc_pset_remove(pset, port);
		ips_check_unlock(pset);
	} else {
		/* Else, wake up all receivers, indicating why. */

		imq_lock(&port->ip_messages);
		ipc_mqueue_changed(&port->ip_messages, MACH_RCV_PORT_DIED);
		imq_unlock(&port->ip_messages);
	}

	ipc_port_set_mscount(port, 0);
	imq_lock(&port->ip_messages);
	port->ip_seqno = 0;
	imq_unlock(&port->ip_messages);
}

/*
 *	Routine:	ipc_port_init
 *	Purpose:
 *		Initializes a newly-allocated port.
 *		Doesn't touch the ip_object fields.
 */

void
ipc_port_init(
	ipc_port_t	port,
	ipc_space_t	space,
	mach_port_t	name)
{
	/* port->ip_kobject doesn't have to be initialized */

	port->ip_receiver = space;
	port->ip_receiver_name = name;

	port->ip_mscount = 0;
	port->ip_srights = 0;
	port->ip_sorights = 0;

	port->ip_nsrequest = IP_NULL;
	port->ip_pdrequest = IP_NULL;
	port->ip_dnrequests = IPR_NULL;

	port->ip_pset = IPS_NULL;
	port->ip_seqno = 0;
	port->ip_msgcount = 0;
	port->ip_qlimit = MACH_PORT_QLIMIT_DEFAULT;

	thread_pool_init(&port->ip_thread_pool);

	port->ip_subsystem = RPC_SUBSYSTEM_NULL;
	
	port->ip_flags = 0;

#if	NORMA_IPC
	port->ip_norma_uid = 0;
	port->ip_norma_dest_node = 0;
	port->ip_norma_stransit = 0;
	port->ip_norma_sotransit = 0;
	port->ip_norma_xmm_object_refs = 0;
	port->ip_norma_is_proxy = FALSE;
	port->ip_norma_is_special = FALSE;
	port->ip_norma_atrium_waiter = FALSE;
	port->ip_norma_forward = FALSE;
	port->ip_norma_sync = FALSE;
	port->ip_norma_is_zombie = FALSE;
	port->ip_norma_kserver_active = FALSE;
	port->ip_norma_kserver_waiting = FALSE;
	port->ip_norma_suspended = FALSE;
	port->ip_norma_atrium = IP_NULL;
	port->ip_norma_queue_next = port;
	port->ip_norma_xmm_object = IP_NULL;
	port->ip_norma_next = IP_NULL;
	port->ip_norma_tabled = FALSE;
	port->ip_norma_queued = FALSE;
#if	MACH_ASSERT
	port->ip_norma_spare1 = 0L;
	port->ip_norma_spare2 = 0L;
	port->ip_norma_spare3 = 0L;
#endif	/* MACH_ASSERT */
#endif	/* NORMA_IPC */

	ipc_mqueue_init(&port->ip_messages);
	ipc_thread_queue_init(&port->ip_blocked);
}

/*
 *	Routine:	ipc_port_alloc
 *	Purpose:
 *		Allocate a port.
 *	Conditions:
 *		Nothing locked.  If successful, the port is returned
 *		locked.  (The caller doesn't have a reference.)
 *	Returns:
 *		KERN_SUCCESS		The port is allocated.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_NO_SPACE		No room for an entry in the space.
 *		KERN_RESOURCE_SHORTAGE	Couldn't allocate memory.
 */

kern_return_t
ipc_port_alloc(
	ipc_space_t	space,
	mach_port_t	*namep,
	ipc_port_t	*portp)
{
	ipc_port_t port;
	mach_port_t name;
	kern_return_t kr;
#if	NORMA_IPC && MACH_ASSERT
	int ret = GET_RETURN_PC(&space); /* where we were called from */
	extern int input_msgh_id;
#endif	/* NORMA_IPC && MACH_ASSERT */

	kr = ipc_object_alloc(space, IOT_PORT,
			      MACH_PORT_TYPE_RECEIVE, 0,
			      &name, (ipc_object_t *) &port);
	if (kr != KERN_SUCCESS)
		return kr;
#if	MACH_KDB
	port_allocated();
#endif
	/* port is locked */

	ipc_port_init(port, space, name);

	*namep = name;
	*portp = port;

#if	NORMA_IPC && MACH_ASSERT
	port->ip_norma_spare1 = ret;
	port->ip_norma_spare2 = input_msgh_id;
#endif	/* NORMA_IPC && MACH_ASSERT */
	return KERN_SUCCESS;
}

/*
 *	Routine:	ipc_port_alloc_name
 *	Purpose:
 *		Allocate a port, with a specific name.
 *	Conditions:
 *		Nothing locked.  If successful, the port is returned
 *		locked.  (The caller doesn't have a reference.)
 *	Returns:
 *		KERN_SUCCESS		The port is allocated.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_NAME_EXISTS	The name already denotes a right.
 *		KERN_RESOURCE_SHORTAGE	Couldn't allocate memory.
 */

kern_return_t
ipc_port_alloc_name(
	ipc_space_t	space,
	mach_port_t	name,
	ipc_port_t	*portp)
{
	ipc_port_t port;
	kern_return_t kr;
#if	NORMA_IPC && MACH_ASSERT
	int ret = GET_RETURN_PC(&space); /* where we were called from */
	extern int input_msgh_id;
#endif	/* NORMA_IPC && MACH_ASSERT */

	kr = ipc_object_alloc_name(space, IOT_PORT,
				   MACH_PORT_TYPE_RECEIVE, 0,
				   name, (ipc_object_t *) &port);
	if (kr != KERN_SUCCESS)
		return kr;
#if	MACH_KDB
	port_allocated();
#endif
	/* port is locked */

	ipc_port_init(port, space, name);

	*portp = port;

#if	NORMA_IPC && MACH_ASSERT
	port->ip_norma_spare1 = ret;
	port->ip_norma_spare2 = input_msgh_id;
#endif	/* NORMA_IPC && MACH_ASSERT */
	return KERN_SUCCESS;
}

/*
 *	Routine:	ipc_port_destroy
 *	Purpose:
 *		Destroys a port.  Cleans up queued messages.
 *
 *		If the port has a backup, it doesn't get destroyed,
 *		but is sent in a port-destroyed notification to the backup.
 *	Conditions:
 *		The port is locked and alive; nothing else locked.
 *		The caller has a reference, which is consumed.
 *		Afterwards, the port is unlocked and dead.
 */

void
ipc_port_destroy(
	ipc_port_t	port)
{
	ipc_port_t pdrequest, nsrequest;
	ipc_mqueue_t mqueue;
	ipc_kmsg_queue_t kmqueue;
	ipc_kmsg_t kmsg;
	ipc_thread_t sender;
	ipc_port_request_t dnrequests;
	boolean_t rt;
	thread_pool_t thread_pool;

	assert(ip_active(port));
	/* port->ip_receiver_name is garbage */
	/* port->ip_receiver/port->ip_destination is garbage */
	assert(port->ip_pset == IPS_NULL);
	assert(port->ip_mscount == 0);
	assert(port->ip_seqno == 0);

	rt = IP_RT(port);

	/* first check for a backup port */

	pdrequest = port->ip_pdrequest;
	if (pdrequest != IP_NULL) {
		/* we assume the ref for pdrequest */
		port->ip_pdrequest = IP_NULL;

		/* make port be in limbo */
		port->ip_receiver_name = MACH_PORT_NULL;
		port->ip_destination = IP_NULL;
		ip_unlock(port);

		if (!ipc_port_check_circularity(port, pdrequest)) {
			/* consumes our refs for port and pdrequest */
			ipc_notify_port_destroyed(pdrequest, port);
			return;
		} else {
			/* consume pdrequest and destroy port */
			ipc_port_release_sonce(pdrequest);
		}

		ip_lock(port);
		assert(ip_active(port));
		assert(port->ip_pset == IPS_NULL);
		assert(port->ip_mscount == 0);
		assert(port->ip_seqno == 0);
		assert(port->ip_pdrequest == IP_NULL);
		assert(port->ip_receiver_name == MACH_PORT_NULL);
		assert(port->ip_destination == IP_NULL);

		/* fall through and destroy the port */
	}

	/*
	 *	rouse all blocked senders
	 *
	 *	This must be done with the port locked, because
	 *	ipc_mqueue_send can play with the ip_blocked queue
	 *	of a dead port.
	 */

	while ((sender = ipc_thread_dequeue(&port->ip_blocked)) != ITH_NULL) {
		sender->ith_state = MACH_MSG_SUCCESS;
		thread_go(sender);
	}

	/* once port is dead, we don't need to keep it locked */

	port->ip_object.io_bits &= ~IO_BITS_ACTIVE;
	port->ip_timestamp = ipc_port_timestamp();
	ip_unlock(port);
#if	NORMA_IPC
	/*
	 *	destroy any NORMA_IPC state associated with port
	 */
	norma_ipc_port_destroy(port);
	assert(!ip_nsproxyp(port->ip_nsrequest));
#endif	/* NORMA_IPC */
	
	/* wakeup any threads waiting on this pool port for an activation */
	if ((thread_pool = &port->ip_thread_pool) != THREAD_POOL_NULL)
		thread_pool_wakeup(thread_pool);

	/* throw away no-senders request */

	nsrequest = port->ip_nsrequest;
	if (nsrequest != IP_NULL)
		ipc_notify_send_once(nsrequest); /* consumes ref */

	/* destroy any queued messages */

	mqueue = &port->ip_messages;
	imq_lock(mqueue);
	assert(ipc_thread_queue_empty(&mqueue->imq_threads));
	kmqueue = &mqueue->imq_messages;

	while ((kmsg = ipc_kmsg_dequeue(kmqueue)) != IKM_NULL) {
		imq_unlock(mqueue);

		assert(kmsg->ikm_header.msgh_remote_port ==
						(mach_port_t) port);

		ipc_port_release(port);
		kmsg->ikm_header.msgh_remote_port = MACH_PORT_NULL;
#if	NORMA_IPC
		/*
		 *	This message might be in network format
		 */
		norma_ipc_finish_receiving(&kmsg);

		/*
		 *      If ikm_wait_for_send is set, then some thread is
		 *      waiting for this kmsg and will free it itself.
		 *      Mark the kmsg aborted and wake that thread.
		 *      Otherwise, free the kmsg here.
		 */
		if (kmsg->ikm_wait_for_send) {
			kmsg->ikm_send_aborted = TRUE;
			thread_wakeup((int)kmsg);
		} else {
			/* FALSE for now */
			ipc_kmsg_destroy(kmsg, FALSE);
		}
#else	/* NORMA_IPC */
		ipc_kmsg_destroy(kmsg, rt);
#endif	/* NORMA_IPC */

		imq_lock(mqueue);
	}

	imq_unlock(mqueue);

	/* generate dead-name notifications */

	dnrequests = port->ip_dnrequests;
	if (dnrequests != IPR_NULL) {
		ipc_table_size_t its = dnrequests->ipr_size;
		ipc_table_elems_t size = its->its_size;
		ipc_port_request_index_t index;

		for (index = 1; index < size; index++) {
			ipc_port_request_t ipr = &dnrequests[index];
			mach_port_t name = ipr->ipr_name;
			ipc_port_t soright;

			if (name == MACH_PORT_NULL)
				continue;

			soright = ipr->ipr_soright;
			assert(soright != IP_NULL);

#if	NORMA_IPC
			if (soright == IP_NORMA_FAKE_DNREQUEST)
				norma_ipc_notify_dead_name(port, name);
			else
#endif	/* NORMA_IPC */
			ipc_notify_dead_name(soright, name);
		}

		it_dnrequests_free(its, dnrequests);
	}

	if (ip_kotype(port) != IKOT_NONE)
		ipc_kobject_destroy(port);

	/* XXXX Perhaps should verify that ip_thread_pool is empty! */

	ipc_port_release(port); /* consume caller's ref */
}

/*
 *	Routine:	ipc_port_check_circularity
 *	Purpose:
 *		Check if queueing "port" in a message for "dest"
 *		would create a circular group of ports and messages.
 *
 *		If no circularity (FALSE returned), then "port"
 *		is changed from "in limbo" to "in transit".
 *
 *		That is, we want to set port->ip_destination == dest,
 *		but guaranteeing that this doesn't create a circle
 *		port->ip_destination->ip_destination->... == port
 *	Conditions:
 *		No ports locked.  References held for "port" and "dest".
 */

boolean_t
ipc_port_check_circularity(
	ipc_port_t	port,
	ipc_port_t	dest)
{
	ipc_port_t base;

	assert(port != IP_NULL);
	assert(dest != IP_NULL);

	if (port == dest)
		return TRUE;
	base = dest;

	/*
	 *	First try a quick check that can run in parallel.
	 *	No circularity if dest is not in transit.
	 */

	ip_lock(port);
	if (ip_lock_try(dest)) {
		if (!ip_active(dest) ||
		    (dest->ip_receiver_name != MACH_PORT_NULL) ||
		    (dest->ip_destination == IP_NULL))
			goto not_circular;

		/* dest is in transit; further checking necessary */

		ip_unlock(dest);
	}
	ip_unlock(port);

	ipc_port_multiple_lock(); /* massive serialization */

	/*
	 *	Search for the end of the chain (a port not in transit),
	 *	acquiring locks along the way.
	 */

	for (;;) {
		ip_lock(base);

		if (!ip_active(base) ||
		    (base->ip_receiver_name != MACH_PORT_NULL) ||
		    (base->ip_destination == IP_NULL))
			break;

		base = base->ip_destination;
	}

	/* all ports in chain from dest to base, inclusive, are locked */

	if (port == base) {
		/* circularity detected! */

		ipc_port_multiple_unlock();

		/* port (== base) is in limbo */

		assert(ip_active(port));
		assert(port->ip_receiver_name == MACH_PORT_NULL);
		assert(port->ip_destination == IP_NULL);

		while (dest != IP_NULL) {
			ipc_port_t next;

			/* dest is in transit or in limbo */

			assert(ip_active(dest));
			assert(dest->ip_receiver_name == MACH_PORT_NULL);

			next = dest->ip_destination;
			ip_unlock(dest);
			dest = next;
		}

		return TRUE;
	}

	/*
	 *	The guarantee:  lock port while the entire chain is locked.
	 *	Once port is locked, we can take a reference to dest,
	 *	add port to the chain, and unlock everything.
	 */

	ip_lock(port);
	ipc_port_multiple_unlock();

    not_circular:

	/* port is in limbo */

	assert(ip_active(port));
	assert(port->ip_receiver_name == MACH_PORT_NULL);
	assert(port->ip_destination == IP_NULL);

	ip_reference(dest);
	port->ip_destination = dest;

	/* now unlock chain */

	while (port != base) {
		ipc_port_t next;

		/* port is in transit */

		assert(ip_active(port));
		assert(port->ip_receiver_name == MACH_PORT_NULL);
		assert(port->ip_destination != IP_NULL);

		next = port->ip_destination;
		ip_unlock(port);
		port = next;
	}

	/* base is not in transit */

	assert(!ip_active(base) ||
	       (base->ip_receiver_name != MACH_PORT_NULL) ||
	       (base->ip_destination == IP_NULL));
	ip_unlock(base);

	return FALSE;
}

/*
 *	Routine:	ipc_port_lookup_notify
 *	Purpose:
 *		Make a send-once notify port from a receive right.
 *		Returns IP_NULL if name doesn't denote a receive right.
 *	Conditions:
 *		The space must be locked (read or write) and active.
 */

ipc_port_t
ipc_port_lookup_notify(
	ipc_space_t	space,
	mach_port_t	name)
{
	ipc_port_t port;
	ipc_entry_t entry;

	assert(space->is_active);

	entry = ipc_entry_lookup(space, name);
	if (entry == IE_NULL)
		return IP_NULL;

	if ((entry->ie_bits & MACH_PORT_TYPE_RECEIVE) == 0)
		return IP_NULL;

	port = (ipc_port_t) entry->ie_object;
	assert(port != IP_NULL);

	ip_lock(port);
	assert(ip_active(port));
	assert(port->ip_receiver_name == name);
	assert(port->ip_receiver == space);

	ip_reference(port);
	port->ip_sorights++;
	ip_unlock(port);

	return port;
}

/*
 *	Routine:	ipc_port_make_send
 *	Purpose:
 *		Make a naked send right from a receive right.
 *	Conditions:
 *		The port is not locked but it is active.
 */

ipc_port_t
ipc_port_make_send(
	ipc_port_t	port)
{
	assert(IP_VALID(port));

	ip_lock(port);
	assert(ip_active(port));
	port->ip_mscount++;
	port->ip_srights++;
	ip_reference(port);
	ip_unlock(port);

	return port;
}

/*
 *	Routine:	ipc_port_copy_send
 *	Purpose:
 *		Make a naked send right from another naked send right.
 *			IP_NULL		-> IP_NULL
 *			IP_DEAD		-> IP_DEAD
 *			dead port	-> IP_DEAD
 *			live port	-> port + ref
 *	Conditions:
 *		Nothing locked except possibly a space.
 */

ipc_port_t
ipc_port_copy_send(
	ipc_port_t	port)
{
	ipc_port_t sright;

	if (!IP_VALID(port))
		return port;

	ip_lock(port);
	if (ip_active(port)) {
		assert(port->ip_srights > 0);

		ip_reference(port);
		port->ip_srights++;
		sright = port;
	} else
		sright = IP_DEAD;
	ip_unlock(port);

	return sright;
}

/*
 *	Routine:	ipc_port_copyout_send
 *	Purpose:
 *		Copyout a naked send right (possibly null/dead),
 *		or if that fails, destroy the right.
 *	Conditions:
 *		Nothing locked.
 */

mach_port_t
ipc_port_copyout_send(
	ipc_port_t	sright,
	ipc_space_t	space)
{
	mach_port_t name;

	if (IP_VALID(sright)) {
		kern_return_t kr;

		kr = ipc_object_copyout(space, (ipc_object_t) sright,
					MACH_MSG_TYPE_PORT_SEND, TRUE, &name);
		if (kr != KERN_SUCCESS) {
			ipc_port_release_send(sright);

			if (kr == KERN_INVALID_CAPABILITY)
				name = MACH_PORT_DEAD;
			else
				name = MACH_PORT_NULL;
		}
	} else
		name = (mach_port_t) sright;

	return name;
}

/*
 *	Routine:	ipc_port_release_send
 *	Purpose:
 *		Release a (valid) naked send right.
 *		Consumes a ref for the port.
 *	Conditions:
 *		Nothing locked.
 */

void
ipc_port_release_send(
	ipc_port_t	port)
{
	ipc_port_t nsrequest = IP_NULL;
	mach_port_mscount_t mscount;

	assert(IP_VALID(port));

	ip_lock(port);
	ip_release(port);

	if (!ip_active(port)) {
		ip_check_unlock(port);
		return;
	}

	assert(port->ip_srights > 0);

	if (--port->ip_srights == 0) {
#if	NORMA_IPC
		assert((port->ip_norma_stransit == 0) ||
			(IP_NORMA_IS_PROXY(port)));
		if (ip_nsproxyp(port->ip_nsrequest)) {
			assert(port->ip_mscount == 0);
			assert(ip_nsproxy(port->ip_nsrequest) == port);
			port->ip_nsrequest = IP_NULL;
			/* consumes ip_lock(port) */
			norma_ipc_notify_no_senders(port);
		} else
#endif	/* NORMA_IPC */
		{
			nsrequest = port->ip_nsrequest;
			if (nsrequest != IP_NULL) {
				port->ip_nsrequest = IP_NULL;
				mscount = port->ip_mscount;
				ip_unlock(port);
				ipc_notify_no_senders(nsrequest, mscount);
			} else
				ip_unlock(port);
		}

	} else
		ip_unlock(port);
}

/*
 *	Routine:	ipc_port_make_sonce
 *	Purpose:
 *		Make a naked send-once right from a receive right.
 *	Conditions:
 *		The port is not locked but it is active.
 */

ipc_port_t
ipc_port_make_sonce(
	ipc_port_t	port)
{
	assert(IP_VALID(port));

	ip_lock(port);
	assert(ip_active(port));
	port->ip_sorights++;
	ip_reference(port);
	ip_unlock(port);

	return port;
}

/*
 *	Routine:	ipc_port_release_sonce
 *	Purpose:
 *		Release a naked send-once right.
 *		Consumes a ref for the port.
 *
 *		In normal situations, this is never used.
 *		Send-once rights are only consumed when
 *		a message (possibly a send-once notification)
 *		is sent to them.
 *	Conditions:
 *		Nothing locked except possibly a space.
 */

void
ipc_port_release_sonce(
	ipc_port_t	port)
{
	assert(IP_VALID(port));

	ip_lock(port);

	assert(port->ip_sorights > 0);

	port->ip_sorights--;

#if	NORMA_IPC
	if (IP_NORMA_IS_PROXY(port)) {
		port->ip_norma_sotransit--;
		norma_port_remove_try(port); /* consumes lock */
		ip_lock(port);
	}
#endif	/* NORMA_IPC */

	ip_release(port);

	if (!ip_active(port)) {
		ip_check_unlock(port);
		return;
	}

	ip_unlock(port);
}

/*
 *	Routine:	ipc_port_release_receive
 *	Purpose:
 *		Release a naked (in limbo or in transit) receive right.
 *		Consumes a ref for the port; destroys the port.
 *	Conditions:
 *		Nothing locked.
 */

void
ipc_port_release_receive(
	ipc_port_t	port)
{
	ipc_port_t dest;

	assert(IP_VALID(port));

	ip_lock(port);
	assert(ip_active(port));
	assert(port->ip_receiver_name == MACH_PORT_NULL);
	dest = port->ip_destination;

	ipc_port_destroy(port); /* consumes ref, unlocks */

	if (dest != IP_NULL)
		ipc_port_release(dest);
}

/*
 *	Routine:	ipc_port_alloc_special
 *	Purpose:
 *		Allocate a port in a special space.
 *		The new port is returned with one ref.
 *		If unsuccessful, IP_NULL is returned.
 *	Conditions:
 *		Nothing locked.
 */

ipc_port_t
ipc_port_alloc_special(
	ipc_space_t	space)
{
	ipc_object_type_t otype;
	ipc_port_t port;
#if	NORMA_IPC && MACH_ASSERT
	int ret = GET_RETURN_PC(&space); /* where we were called from */
	extern int input_msgh_id;
#endif	/* NORMA_IPC && MACH_ASSERT */

	port = (ipc_port_t) io_alloc(IOT_PORT);
	if (port == IP_NULL)
		return IP_NULL;
#if	MACH_KDB
	port_allocated();
#endif

	bzero((char *)port, sizeof(*port));
	io_lock_init(&port->ip_object);
	port->ip_references = 1;
	port->ip_object.io_bits = io_makebits(TRUE, IOT_PORT, 0);



	ipc_port_init(port, space, 1);


#if	NORMA_IPC && MACH_ASSERT
	port->ip_norma_spare1 = ret;
	port->ip_norma_spare2 = input_msgh_id;
#endif	/* NORMA_IPC && MACH_ASSERT */
	return port;
}

/*
 *	Routine:	ipc_port_dealloc_special
 *	Purpose:
 *		Deallocate a port in a special space.
 *		Consumes one ref for the port.
 *	Conditions:
 *		Nothing locked.
 */

void
ipc_port_dealloc_special(
	ipc_port_t	port,
	ipc_space_t	space)
{
	ip_lock(port);
	assert(ip_active(port));
	assert(port->ip_receiver_name != MACH_PORT_NULL);
	assert(port->ip_receiver == space);

	/*
	 *	We clear ip_receiver_name and ip_receiver to simplify
	 *	the ipc_space_kernel check in ipc_mqueue_send.
	 */

	port->ip_receiver_name = MACH_PORT_NULL;
	port->ip_receiver = IS_NULL;

	/* relevant part of ipc_port_clear_receiver */
	ipc_port_set_mscount(port, 0);
	port->ip_seqno = 0;

	ipc_port_destroy(port);
}

#if	MACH_KDB

#if	ZONE_DEBUG
#include <kern/misc_protos.h> /* must be included before printf redefinition */
#endif	/* ZONE_DEBUG */

#include <ddb/db_output.h>
#include <ddb/db_print.h>

#define	printf	kdbprintf
extern int indent;

/*
 * ipc_entry_print - pretty-print an ipc_entry
 */
static void ipc_entry_print(struct ipc_entry *, char *); /* forward */

static void ipc_entry_print(struct ipc_entry *iep, char *tag)
{

	iprintf("%s @", tag);
	printf(" 0x%x, bits=%x object=%x\n",
		iep, iep->ie_bits, iep->ie_object);
	indent += 2;
	iprintf("urefs=%x ", IE_BITS_UREFS(iep->ie_bits));
	printf("type=%x gen=%x\n",
		IE_BITS_TYPE(iep->ie_bits), IE_BITS_GEN(iep->ie_bits));
	indent -= 2;
}


/*
 *	Routine:	ipc_port_print
 *	Purpose:
 *		Pretty-print a port for kdb.
 */
int	ipc_port_print_long = 0;	/* set for more detail */

void
ipc_port_print(
	ipc_port_t	port)
{
	extern int indent;
	db_addr_t task;
	int task_id;
	printf("port 0x%x\n", port);

	indent += 2;

	ipc_object_print(&port->ip_object);

	if (ipc_port_print_long) {
		ipc_entry_print(&port->ip_rsentry, "rsentry");
		ipc_entry_print(&port->ip_soentry, "soentry");
		iprintf("pool=0x%x", port->ip_thread_pool);
#if MACH_DEBUG
		printf(" ip_rslinks(%x,%x) solinks(%x,%x)",
			port->ip_rslinks.next, port->ip_rslinks.prev,
			port->ip_solinks.next, port->ip_solinks.prev);
#endif
		printf("\n");
	}

	if (!ip_active(port)) {
		iprintf("timestamp=0x%x", port->ip_timestamp);
	} else if (port->ip_receiver_name == MACH_PORT_NULL) {
		iprintf("destination=0x%x (", port->ip_destination);
		if (port->ip_destination != MACH_PORT_NULL &&
		    (task = db_task_from_space(port->ip_destination->
					       ip_receiver, &task_id)))
			printf("task%d at 0x%x", task_id, task);
		else
			printf("unknown");
		printf(")");
	} else {
		iprintf("receiver=0x%x (", port->ip_receiver);
		if (port->ip_receiver == ipc_space_kernel)
			printf("kernel");
		else if (port->ip_receiver == ipc_space_reply)
			printf("reply");

#if	NORMA_IPC
		else if (port->ip_receiver == ipc_space_remote)
			printf("remote");
#endif	/* NORMA_IPC */
		else if (port->ip_receiver == default_pager_space)
			printf("default_pager");
		else if (task = db_task_from_space(port->ip_receiver, &task_id))
			printf("task%d at 0x%x", task_id, task);
		else
			printf("unknown");
		printf(")");
	}
	printf(", receiver_name=0x%x\n", port->ip_receiver_name);

	iprintf("mscount=%d", port->ip_mscount);
	printf(", srights=%d", port->ip_srights);
	printf(", sorights=%d\n", port->ip_sorights);

	iprintf("nsrequest=0x%x", port->ip_nsrequest);
	printf(", pdrequest=0x%x", port->ip_pdrequest);
	printf(", dnrequests=0x%x\n", port->ip_dnrequests);

	iprintf("pset=0x%x", port->ip_pset);
	printf(", seqno=%d", port->ip_seqno);
	printf(", msgcount=%d", port->ip_msgcount);
	printf(", qlimit=%d\n", port->ip_qlimit);

	iprintf("kmsgs=0x%x", port->ip_messages.imq_messages.ikmq_base);
	printf(", rcvrs=0x%x", port->ip_messages.imq_threads.ithq_base);
	printf(", sndrs=0x%x", port->ip_blocked.ithq_base);
	printf(", kobj=0x%x\n", port->ip_kobject);

	iprintf("flags=0x%x", port->ip_flags);
	
#if	NORMA_IPC
#define	BOOL(b)		((b) ? "" : "!")
	iprintf("norma_uid=%x", port->ip_norma_uid);
	printf(", dest_node=%d", port->ip_norma_dest_node);
	printf(", stransit=%d", port->ip_norma_stransit);
	printf(", xorefs=%d", port->ip_norma_xmm_object_refs);
	printf(", sotransit=%d\n", port->ip_norma_sotransit);

	iprintf("%snorma_is_proxy", BOOL(port->ip_norma_is_proxy));
	printf(", %snorma_forward, %snorma_sync, %sis_special, %squeued\n", 
	       BOOL(port->ip_norma_forward),
	       BOOL(port->ip_norma_sync),
	       BOOL(port->ip_norma_is_special),
	       BOOL(port->ip_norma_queued));

	iprintf("%sis_zombie, %skserver_active",
		BOOL(port->ip_norma_is_zombie),
		BOOL(port->ip_norma_kserver_active));
	printf(", %skserver_waiting, %ssuspended, %stabled\n", 
	       BOOL(port->ip_norma_kserver_waiting),
	       BOOL(port->ip_norma_suspended),
	       BOOL(port->ip_norma_tabled));

	iprintf("norma_atrium=0x%x", port->ip_norma_atrium);
	printf(", queue_next=0x%x", port->ip_norma_queue_next);
	printf(", xmm_object=0x%x", port->ip_norma_xmm_object);
	printf(", next=0x%x\n", port->ip_norma_next);

#if	MACH_ASSERT
	iprintf("norma_spare1=0x%x", port->ip_norma_spare1);
	printf(", norma_spare2=0x%x", port->ip_norma_spare2);
	printf(", norma_spare3=0x%x\n", port->ip_norma_spare3);
#endif	/* MACH_ASSERT */
#endif	/* NORMA_IPC */

	indent -=2;
}

ipc_port_t
ipc_name_to_data(
	task_t		task,
	mach_port_t	name)
{
	ipc_space_t	space;
	ipc_entry_t	entry;

	if (task == TASK_NULL) {
		db_printf("port_name_to_data: task is null\n");
		return (0);
	}
	if ((space = task->itk_space) == 0) {
		db_printf("port_name_to_data: task->itk_space is null\n");
		return (0);
	}
	if (!space->is_active) {
		db_printf("port_name_to_data: task->itk_space not active\n");
		return (0);
	}
	if ((entry = ipc_entry_lookup(space, name)) == 0) {
		db_printf("port_name_to_data: lookup yields zero\n");
		return (0);
	}
	return ((ipc_port_t)entry->ie_object);
}

#if	ZONE_DEBUG
void
print_type_ports(type, dead)
	unsigned type;
	unsigned dead;
{
	ipc_port_t port;
	int n;

	n = 0;
	for (port = (ipc_port_t)first_element(ipc_object_zones[IOT_PORT]);
	     port;
	     port = (ipc_port_t)next_element(ipc_object_zones[IOT_PORT], 
					     (vm_offset_t)port))
		if (ip_kotype(port) == type &&
		    (!dead || !ip_active(port))) {
			if (++n % 5)
				printf("0x%x\t", port);
			else
				printf("0x%x\n", port);
		}
	if (n % 5)
		printf("\n");
}

void
print_ports(void)
{
	ipc_port_t port;
	int total_port_count;
	int space_null_count;
	int space_kernel_count;
	int space_reply_count;
	int space_pager_count;
	int space_other_count;
#if	NORMA_IPC
	int space_remote_count;
#endif	/* NORMA_IPC */
	struct {
		int total_count;
		int dead_count;
	} port_types[IKOT_MAX_TYPE];

	total_port_count = 0;

	bzero((char *)&port_types[0], sizeof(port_types));
	space_null_count = 0;
	space_kernel_count = 0;
	space_reply_count = 0;
	space_pager_count = 0;
	space_other_count = 0;
#if	NORMA_IPC
	space_remote_count = 0;
#endif	/* NORMA_IPC */

	for (port = (ipc_port_t)first_element(ipc_object_zones[IOT_PORT]);
	     port;
	     port = (ipc_port_t)next_element(ipc_object_zones[IOT_PORT], 
					     (vm_offset_t)port)) {
		total_port_count++;
		if (ip_kotype(port) >= IKOT_MAX_TYPE) {
			port_types[IKOT_UNKNOWN].total_count++;
			if (!io_active(&port->ip_object))
				port_types[IKOT_UNKNOWN].dead_count++;
		} else {
			port_types[ip_kotype(port)].total_count++;
			if (!io_active(&port->ip_object))
				port_types[ip_kotype(port)].dead_count++;
		}

		if (!port->ip_receiver)
			space_null_count++;
		else if (port->ip_receiver == ipc_space_kernel)
		  	space_kernel_count++;
		else if (port->ip_receiver == ipc_space_reply)
		  	space_reply_count++;
#if	NORMA_IPC
		else if (port->ip_receiver == ipc_space_remote)
		  	space_remote_count++;
#endif	/* NORMA_IPC */
		else if (port->ip_receiver == default_pager_space)
		  	space_pager_count++;
		else
			space_other_count++;
	}
	printf("\n%7d	total ports\n\n", total_port_count);

#define PRINT_ONE_PORT_TYPE(name) \
	printf("%7d	%s", port_types[IKOT_##name].total_count, # name); \
	if (port_types[IKOT_##name].dead_count) \
	     printf(" (%d dead ports)", port_types[IKOT_##name].dead_count);\
	printf("\n");

	PRINT_ONE_PORT_TYPE(NONE);
	PRINT_ONE_PORT_TYPE(THREAD);
	PRINT_ONE_PORT_TYPE(TASK);
	PRINT_ONE_PORT_TYPE(HOST);
	PRINT_ONE_PORT_TYPE(HOST_PRIV);
	PRINT_ONE_PORT_TYPE(PROCESSOR);
	PRINT_ONE_PORT_TYPE(PSET);
	PRINT_ONE_PORT_TYPE(PSET_NAME);
	PRINT_ONE_PORT_TYPE(PAGER);
	PRINT_ONE_PORT_TYPE(PAGING_REQUEST);
	PRINT_ONE_PORT_TYPE(XMM_OBJECT);
	PRINT_ONE_PORT_TYPE(DEVICE);
	PRINT_ONE_PORT_TYPE(XMM_PAGER);
	PRINT_ONE_PORT_TYPE(XMM_KERNEL);
	PRINT_ONE_PORT_TYPE(XMM_REPLY);
	PRINT_ONE_PORT_TYPE(PAGER_TERMINATING);
	PRINT_ONE_PORT_TYPE(PAGING_NAME);
	PRINT_ONE_PORT_TYPE(CLOCK);
	PRINT_ONE_PORT_TYPE(CLOCK_CTRL);
	PRINT_ONE_PORT_TYPE(PAGER_REP);
	PRINT_ONE_PORT_TYPE(MASTER_DEVICE);
	PRINT_ONE_PORT_TYPE(UNKNOWN);
	printf("\nipc_space:\n\n");
#if	NORMA_IPC
	printf("NULL	KERNEL	REPLY	REMOTE	PAGER	OTHER\n");
	printf("%d	%d	%d	%d	%d	%d\n",
	       space_null_count,
	       space_kernel_count,
	       space_reply_count,
	       space_remote_count,
	       space_pager_count,
	       space_other_count
	);
#else	/* NORMA_IPC */
	printf("NULL	KERNEL	REPLY	PAGER	OTHER\n");
	printf("%d	%d	%d	%d	%d\n",
	       space_null_count,
	       space_kernel_count,
	       space_reply_count,
	       space_pager_count,
	       space_other_count
	);
#endif	/* NORMA_IPC */
}

#endif	/* ZONE_DEBUG */

#endif	/* MACH_KDB */
