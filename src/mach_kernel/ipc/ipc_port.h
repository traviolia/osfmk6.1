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
 * Revision 2.12.2.4  92/05/28  18:14:17  jeffreyh
 * 	Change value of IP_NORMA_FAKE_DNREQUEST to 
 * 	not get confused with MACH_IPC_COMPAT
 * 
 * Revision 2.12.2.3  92/05/26  18:54:02  jeffreyh
 * 	Add dead name proxy definitions
 * 	[92/05/25            dlb]
 * 
 * 	Added ip_norma_sync to allow thread synchronization on ports.
 * 	[92/05/19            sjs]
 * 
 * 	Added norma_ip_forward to indicate a proxy cannot be thrown away
 * 	until the root node has been updated to point at the new node.
 * 	[92/05/18            sjs]
 * 
 * Revision 2.12.2.2.1.1  92/05/06  17:47:13  jeffreyh
 * 	Add ip_norma_atrium_waiter for atrium message syncronization.
 * 	[92/05/05            dlb]
 * 
 * Revision 2.12.2.2  92/02/21  11:23:37  jsb
 * 	Removed ip_norma_queued. Added ip_norma_spare[1234].
 * 	[92/02/18  07:44:11  jsb]
 * 
 * 	Added ip_norma_xmm_object_refs.
 * 	[92/02/16  16:12:50  jsb]
 * 
 * 	Added ip_norma_xmm_object.
 * 	[92/02/09  14:42:47  jsb]
 * 
 * Revision 2.12.2.1  92/01/03  16:35:50  jsb
 * 	Renamed IP_NORMA_REQUEST macros to ip_nsproxy{,m,p}.
 * 	They now look like Rich's ip_pdrequest macros.
 * 	[91/12/30  07:53:29  jsb]
 * 
 * 	Added IP_NORMA_NSREQUEST macros for no-senders notification support.
 * 	[91/12/28  17:03:42  jsb]
 * 
 * 	Added ip_norma_{queued,queue_next} for new norma_ipc_send
 * 	implementation that maintains a list of ports with unsent
 * 	remote messages. (The old implementation kept a single
 * 	list of unsent messages for all ports.)
 * 	[91/12/28  08:44:40  jsb]
 * 
 * 	Added ip_norma_atrium.
 * 	[91/12/26  20:03:40  jsb]
 * 
 * 	Added ip_norma_sotransit. Removed ip_norma_{wanted,migrating}.
 * 	Made ip_norma_dest_node unsigned.
 * 	[91/12/25  16:44:29  jsb]
 * 
 * 	NORMA_IPC: removed unused fields from struct ipc_port. Corrected log.
 * 	[91/12/24  14:16:33  jsb]
 * 
 * Revision 2.12  91/12/14  14:28:26  jsb
 * 	NORMA_IPC: replaced dummy port struct fields with real names.
 * 
 * Revision 2.11  91/11/14  16:56:20  rpd
 * 	Added ipc_fields.h hack, with fields in struct ipc_port to match.
 *	Added IP_NORMA_IS_PROXY macro.
 * 	[91/11/00            jsb]
 * 
 * Revision 2.10  91/10/09  16:10:01  af
 * 	Added (unconditional) ipc_port_print declaration.
 * 	[91/09/02            rpd]
 * 
 * Revision 2.9  91/08/28  11:13:50  jsb
 * 	Added ip_seqno and ipc_port_set_seqno.
 * 	[91/08/09            rpd]
 * 	Renamed clport (now ip_norma) fields in struct ipc_port.
 * 	[91/08/14  19:31:55  jsb]
 * 
 * Revision 2.8  91/08/03  18:18:37  jsb
 * 	Fixed include. Added clport fields directly to struct ipc_port.
 * 	[91/07/17  14:06:25  jsb]
 * 
 * Revision 2.7  91/06/17  15:46:26  jsb
 * 	Renamed NORMA conditionals.
 * 	[91/06/17  10:44:06  jsb]
 * 
 * Revision 2.6  91/05/14  16:35:34  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/02/05  17:23:10  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  15:50:04  mrt]
 * 
 * Revision 2.4  90/11/05  14:29:39  rpd
 * 	Added ipc_port_reference, ipc_port_release.
 * 	[90/10/29            rpd]
 * 
 * Revision 2.3  90/09/28  16:55:18  jsb
 * 	Added NORMA_IPC support.
 * 	[90/09/28  14:03:58  jsb]
 * 
 * Revision 2.2  90/06/02  14:51:13  rpd
 * 	Created for new IPC.
 * 	[90/03/26  21:01:25  rpd]
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
 *	File:	ipc/ipc_port.h
 *	Author:	Rich Draves
 *	Date:	1989
 *
 *	Definitions for ports.
 */

#ifndef	_IPC_IPC_PORT_H_
#define _IPC_IPC_PORT_H_

#include <norma_ipc.h>
#include <mach_assert.h>
#include <mach_debug.h>

#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach_debug.h>
#include <mach/port.h>
#include <kern/lock.h>
#include <kern/ipc_kobject.h>
#include <ipc/ipc_object.h>
#include <ipc/ipc_mqueue.h>
#include <ipc/ipc_table.h>
#include <ipc/ipc_thread.h>
#include <ipc/ipc_types.h>
#include <kern/thread_pool.h>
#include <ipc/ipc_entry.h>
#include <kern/rpc_common.h>

/*
 *  A receive right (port) can be in four states:
 *	1) dead (not active, ip_timestamp has death time)
 *	2) in a space (ip_receiver_name != 0, ip_receiver points
 *	to the space but doesn't hold a ref for it)
 *	3) in transit (ip_receiver_name == 0, ip_destination points
 *	to the destination port and holds a ref for it)
 *	4) in limbo (ip_receiver_name == 0, ip_destination == IP_NULL)
 *
 *  If the port is active, and ip_receiver points to some space,
 *  then ip_receiver_name != 0, and that space holds receive rights.
 *  If the port is not active, then ip_timestamp contains a timestamp
 *  taken when the port was destroyed.
 */

typedef unsigned int ipc_port_timestamp_t;

typedef unsigned int ipc_port_flags_t;

struct ipc_port {

	/*
	 * Initial sub-structure in common with ipc_pset and rpc_port
	 * First element is an ipc_object
	 */
	struct rpc_common_data port_comm;

	union {
		struct ipc_space *receiver;
		struct ipc_port *destination;
		ipc_port_timestamp_t timestamp;
	} data;

	mach_port_mscount_t ip_mscount;
	mach_port_rights_t ip_srights;
	mach_port_rights_t ip_sorights;

	struct ipc_port *ip_nsrequest;
	struct ipc_port *ip_pdrequest;
	struct ipc_port_request *ip_dnrequests;

	struct ipc_pset *ip_pset;
	mach_port_seqno_t ip_seqno;		/* locked by message queue */
	mach_port_msgcount_t ip_msgcount;
	mach_port_msgcount_t ip_qlimit;
	struct ipc_mqueue ip_messages;
	struct ipc_thread_queue ip_blocked;
	ipc_port_flags_t ip_flags;
#if	NORMA_IPC
	unsigned long ip_norma_uid;
	unsigned long ip_norma_dest_node;
	long ip_norma_stransit;
	long ip_norma_sotransit;
	long ip_norma_xmm_object_refs;
	unsigned int
	/* boolean_t */	ip_norma_is_proxy: 1,	   /* proxy for remote port */
			ip_norma_is_special: 1,	   /* norma special port */
			ip_norma_atrium_waiter: 1, /* sync for migration */
			ip_norma_forward: 1,       /* proxy is placeholder */
			ip_norma_sync: 1,          /* thread synchronize */
			ip_norma_is_zombie: 1,	   /* port to be destroyed */
			ip_norma_kserver_active: 1,/* kserver msg in process */
			ip_norma_kserver_waiting:1,/* kserver msg waiting */
			ip_norma_suspended: 1,     /* port is suspended */
			ip_norma_tabled: 1,        /* port is in norma list */
	  		ip_norma_queued: 1,        /* port is in queue list */
	  		:0;			   /* (align to boundary) */
	struct ipc_port *ip_norma_atrium;
	struct ipc_port *ip_norma_queue_next;
	struct ipc_port *ip_norma_xmm_object;
	struct ipc_port *ip_norma_next;
#if	MACH_ASSERT
	long ip_norma_spare1;
	long ip_norma_spare2;
	long ip_norma_spare3;
#endif	/* MACH_ASSERT */
#endif	/* NORMA_IPC */
};


#define ip_object		port_comm.rcd_comm.icd_object
#define ip_references		ip_object.io_references
#define ip_bits			ip_object.io_bits
#define ip_kobject		port_comm.rcd_comm.icd_kobject
#define ip_subsystem		port_comm.rcd_comm.icd_subsystem
#define ip_sobject		port_comm.rcd_comm.icd_sobject
#define ip_sbits		port_comm.rcd_comm.icd_sbits
#define ip_receiver_name	port_comm.rcd_comm.icd_receiver_name

#define ip_thread_pool		port_comm.rcd_thread_pool
#define ip_rsentry		port_comm.rcd_rsentry
#define ip_soentry		port_comm.rcd_soentry
#if	MACH_DEBUG
#define ip_rslinks		port_comm.rcd_ip_rslinks
#define ip_solinks		port_comm.rcd_ip_solinks
#endif

#define	ip_receiver		data.receiver
#define	ip_destination		data.destination
#define	ip_timestamp		data.timestamp

#define	IP_NULL			((ipc_port_t) IO_NULL)
#define	IP_DEAD			((ipc_port_t) IO_DEAD)

#define	IP_VALID(port)		IO_VALID(&(port)->ip_object)

#define	ip_active(port)		io_active(&(port)->ip_object)
#define	ip_lock_init(port)	io_lock_init(&(port)->ip_object)
#define	ip_lock(port)		io_lock(&(port)->ip_object)
#define	ip_lock_try(port)	io_lock_try(&(port)->ip_object)
#define	ip_unlock(port)		io_unlock(&(port)->ip_object)
#define	ip_check_unlock(port)	io_check_unlock(&(port)->ip_object)
#define	ip_reference(port)	io_reference(&(port)->ip_object)
#define	ip_release(port)	io_release(&(port)->ip_object)

#define	ip_kotype(port)		io_kotype(&(port)->ip_object)

#define	IPC_PORT_FLAGS_RT ((ipc_port_flags_t)(1 << 0))
#define IP_RT(port) (((port)->ip_flags & IPC_PORT_FLAGS_RT) != 0)

typedef ipc_table_index_t ipc_port_request_index_t;

typedef struct ipc_port_request {
	union {
		struct ipc_port *port;
		ipc_port_request_index_t index;
	} notify;

	union {
		mach_port_t name;
		struct ipc_table_size *size;
	} name;
} *ipc_port_request_t;

#define	ipr_next		notify.index
#define	ipr_size		name.size

#define	ipr_soright		notify.port
#define	ipr_name		name.name

#define	IPR_NULL		((ipc_port_request_t) 0)

/*
 *	Taking the ipc_port_multiple lock grants the privilege
 *	to lock multiple ports at once.  No ports must locked
 *	when it is taken.
 */

extern mutex_t	ipc_port_multiple_lock_data;

#define	ipc_port_multiple_lock_init()					\
		mutex_init(&ipc_port_multiple_lock_data)

#define	ipc_port_multiple_lock()					\
		mutex_lock(&ipc_port_multiple_lock_data)

#define	ipc_port_multiple_unlock()					\
		mutex_unlock(&ipc_port_multiple_lock_data)

/*
 *	The port timestamp facility provides timestamps
 *	for port destruction.  It is used to serialize
 *	mach_port_names with port death.
 */

extern mutex_t	ipc_port_timestamp_lock_data;
extern ipc_port_timestamp_t ipc_port_timestamp_data;

#define	ipc_port_timestamp_lock_init()					\
		mutex_init(&ipc_port_timestamp_lock_data)

#define	ipc_port_timestamp_lock()					\
		mutex_lock(&ipc_port_timestamp_lock_data)

#define	ipc_port_timestamp_unlock()					\
		mutex_unlock(&ipc_port_timestamp_lock_data)

/* Retrieve a port timestamp value */
extern ipc_port_timestamp_t ipc_port_timestamp(void);

/*
 *	Compares two timestamps, and returns TRUE if one
 *	happened before two.  Note that this formulation
 *	works when the timestamp wraps around at 2^32,
 *	as long as one and two aren't too far apart.
 */

#define	IP_TIMESTAMP_ORDER(one, two)	((int) ((one) - (two)) < 0)

#define	ipc_port_translate_receive(space, name, portp)			\
		ipc_object_translate((space), (name),			\
				     MACH_PORT_RIGHT_RECEIVE,		\
				     (ipc_object_t *) (portp))

#define	ipc_port_translate_send(space, name, portp)			\
		ipc_object_translate((space), (name),			\
				     MACH_PORT_RIGHT_SEND,		\
				     (ipc_object_t *) (portp))

/* Allocate a dead-name request slot */
extern kern_return_t
ipc_port_dnrequest(
	ipc_port_t			port,
	mach_port_t			name,
	ipc_port_t			soright,
	ipc_port_request_index_t	*indexp);

/* Grow a port's table of dead-name requests */
extern kern_return_t ipc_port_dngrow(
	ipc_port_t	port,
	int		target_size);

/* Cancel a dead-name request and return the send-once right */
extern ipc_port_t ipc_port_dncancel(
	ipc_port_t			port,
	mach_port_t			name,
	ipc_port_request_index_t	index);

#define	ipc_port_dnrename(port, index, oname, nname)			\
MACRO_BEGIN								\
	ipc_port_request_t ipr, table;					\
									\
	assert(ip_active(port));					\
									\
	table = port->ip_dnrequests;					\
	assert(table != IPR_NULL);					\
									\
	ipr = &table[index];						\
	assert(ipr->ipr_name == oname);					\
									\
	ipr->ipr_name = nname;						\
MACRO_END

/* Make a port-deleted request */
extern void ipc_port_pdrequest(
	ipc_port_t	port,
	ipc_port_t	notify,
	ipc_port_t	*previousp);

/* Make a no-senders request */
extern void ipc_port_nsrequest(
	ipc_port_t		port,
	mach_port_mscount_t	sync,
	ipc_port_t		notify,
	ipc_port_t		*previousp);

/* Change a port's queue limit */
extern void ipc_port_set_qlimit(
	ipc_port_t		port,
	mach_port_msgcount_t	qlimit);

#define	ipc_port_set_mscount(port, mscount)				\
MACRO_BEGIN								\
	assert(ip_active(port));					\
									\
	(port)->ip_mscount = (mscount);					\
MACRO_END

/* Change a port's sequence number */
extern void ipc_port_set_seqno(
	ipc_port_t		port, 
	mach_port_seqno_t 	seqno);

/* Prepare a receive right for transmission/destruction */
extern void ipc_port_clear_receiver(
	ipc_port_t	port);

/* Initialize a newly-allocated port */
extern void ipc_port_init(
	ipc_port_t	port,
	ipc_space_t	space,
	mach_port_t	name);

/* Allocate a port */
extern kern_return_t ipc_port_alloc(
	ipc_space_t	space,
	mach_port_t	*namep,
	ipc_port_t	*portp);

/* Allocate a port, with a specific name */
extern kern_return_t ipc_port_alloc_name(
	ipc_space_t	space,
	mach_port_t	name,
	ipc_port_t	*portp);

/* Destroy a port */
extern void ipc_port_destroy(
	ipc_port_t	port);

/* Check if queueing "port" in a message for "dest" would create a circular 
   group of ports and messages */
extern boolean_t
ipc_port_check_circularity(
	ipc_port_t	port,
	ipc_port_t	dest);

/* Make a send-once notify port from a receive right */
extern ipc_port_t ipc_port_lookup_notify(
	ipc_space_t	space, 
	mach_port_t 	name);

/* Make a naked send right from a receive right */
extern ipc_port_t ipc_port_make_send(
	ipc_port_t	port);

/* Make a naked send right from another naked send right */
extern ipc_port_t ipc_port_copy_send(
	ipc_port_t	port);

/* Copyout a naked send right */
extern mach_port_t ipc_port_copyout_send(
	ipc_port_t	sright,
	ipc_space_t	space);

/* Release a (valid) naked send right */
extern void ipc_port_release_send(
	ipc_port_t	port);

/* Make a naked send-once right from a receive right */
extern ipc_port_t ipc_port_make_sonce(
	ipc_port_t	port);

/* Release a naked send-once right */
extern void ipc_port_release_sonce(
	ipc_port_t	port);

/* Release a naked (in limbo or in transit) receive right */
extern void ipc_port_release_receive(
	ipc_port_t	port);

/* Allocate a port in a special space */
extern ipc_port_t ipc_port_alloc_special(
	ipc_space_t	space);

/* Deallocate a port in a special space */
extern void ipc_port_dealloc_special(
	ipc_port_t	port,
	ipc_space_t	space);

#define	ipc_port_alloc_kernel()		\
		ipc_port_alloc_special(ipc_space_kernel)
#define	ipc_port_dealloc_kernel(port)	\
		ipc_port_dealloc_special((port), ipc_space_kernel)

#define	ipc_port_alloc_reply()		\
		ipc_port_alloc_special(ipc_space_reply)
#define	ipc_port_dealloc_reply(port)	\
		ipc_port_dealloc_special((port), ipc_space_reply)

#define	ipc_port_reference(port)	\
		ipc_object_reference(&(port)->ip_object)

#define	ipc_port_release(port)		\
		ipc_object_release(&(port)->ip_object)

#if	NORMA_IPC

#define	IP_NORMA_IS_PROXY(port)	((port)->ip_norma_is_proxy)

/*
 *	A proxy never has a real nsrequest, but is always has a fake
 *	nsrequest so that the norma ipc system is notified when there
 *	are no send rights for a proxy. A fake nsrequest is indicated by
 *	the low bit of the pointer.  This works because the zone package
 *	guarantees that the two low bits of port pointers are zero.
 */

#define	ip_nsproxyp(nsrequest)	((unsigned int)(nsrequest) & 1)
#define ip_nsproxy(nsrequest)	((ipc_port_t)((unsigned int)(nsrequest) &~ 1))
#define	ip_nsproxym(proxy)	((ipc_port_t)((unsigned int)(proxy) | 1))

/*
 *	Dead name requests have to stay on the node that they are originally
 * 	made on (the right entry all but contains a pointer to the request
 *	structure).  If they are attached to a proxy port, the proxy port
 *	attaches a fake dnrequest to the real port.  When the real receive
 *	right is destroyed, the fake dnrequest invokes the norma system.
 *	The norma system notifies the remote node to destroy the proxy,
 *	setting off the real dead name notifications.  Fake dn requests
 *	are indicated by the magic constant (note that it's not aligned,
 *	hence can't be a port).  The use of 2 as the least significant
 *	digit prevents this from getting confused with a real dead name
 *	request (ports are 4-byte aligned).
 *
 *	Due to our use of existing dnrequest data structures, 0 can't
 *	be used as the name of a node (it looks like a cancelled dnrequest).
 *	The remapping here is somewhat arbitrary.
 */

#define	IP_NORMA_FAKE_DNREQUEST	(ipc_port_t)0x22222222

#define NODE_TO_DNREQUEST_NAME(node)	((node) + 0x10000)
#define DNREQUEST_NAME_TO_NODE(node)	((node) - 0x10000)


#define	port_wellness_check(p)	(assert ((p->ip_refcount - p->ip_srights - \
				 p->ip_sorights - p->ip_msgcount -	\
				 (norma_port_tabled(p) ? 1 : 0))) >= 0)
#endif	/* NORMA_IPC */

#endif	/* _IPC_IPC_PORT_H_ */
