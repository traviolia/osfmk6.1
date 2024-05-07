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
 * Revision 2.22.1.4  92/04/08  15:44:20  jeffreyh
 * 	Temporary debugging logic.
 * 	[92/04/06            dlb]
 * 
 * Revision 2.22.1.3  92/03/03  16:18:34  jeffreyh
 * 	Picked up changes from Joe's branch
 * 	[92/03/03  10:08:49  jeffreyh]
 * 
 * 	Eliminate keep_wired argument from vm_map_copyin().
 * 	[92/02/21  10:12:26  dlb]
 * 	Changes from TRUNK
 * 	[92/02/26  11:41:33  jeffreyh]
 * 
 * Revision 2.22.1.2.2.1  92/03/01  22:24:38  jsb
 * 	Added use_page_lists logic to ipc_kmsg_copyin_compat.
 * 
 * Revision 2.23  92/01/14  16:44:23  rpd
 * 	Fixed ipc_kmsg_copyin, ipc_kmsg_copyout, etc
 * 	to use copyinmap and copyoutmap for out-of-line ports.
 * 	[91/12/16            rpd]
 * 
 * Revision 2.22.1.2  92/02/21  11:23:22  jsb
 * 	Moved ipc_kmsg_copyout_to_network to norma/ipc_output.c.
 * 	Moved ipc_kmsg_uncopyout_to_network to norma/ipc_clean.c.
 * 	[92/02/21  10:34:46  jsb]
 * 
 * 	We no longer convert to network format directly from user format;
 * 	this greatly simplifies kmsg cleaning issues. Added code to detect
 * 	and recover from vm_map_convert_to_page_list failure.
 * 	Streamlined and fixed ipc_kmsg_copyout_to_network.
 * 	[92/02/21  09:01:52  jsb]
 * 
 * 	Modified for new form of norma_ipc_send_port which returns uid.
 * 	[92/02/20  17:11:17  jsb]
 * 
 * Revision 2.22.1.1  92/01/03  16:34:59  jsb
 * 	Mark out-of-line ports as COMPLEX_DATA.
 * 	[92/01/02  13:53:15  jsb]
 * 
 * 	In ipc_kmsg_uncopyout_to_network: don't process local or remote port.
 * 	Do clear the migrate bit as well as the complex_{data,ports} bits.
 * 	[91/12/31  11:42:07  jsb]
 * 
 * 	Added ipc_kmsg_uncopyout_to_network().
 * 	[91/12/29  21:05:29  jsb]
 * 
 * 	Added support in ipc_kmsg_print for MACH_MSGH_BITS_MIGRATED.
 * 	[91/12/26  19:49:16  jsb]
 * 
 * 	Made clean_kmsg routines aware of norma uids.
 * 	Cleaned up ipc_{msg,kmsg}_print. Corrected log.
 * 	[91/12/24  13:59:49  jsb]
 * 
 * Revision 2.22  91/12/15  10:37:53  jsb
 * 	Improved ddb 'show kmsg' support.
 * 
 * Revision 2.21  91/12/14  14:26:03  jsb
 * 	Removed ipc_fields.h hack.
 * 	Made ipc_kmsg_clean_{body,partial} aware of remote ports.
 * 	They don't yet clean up remote ports, but at least they
 * 	no longer pass port uids to ipc_object_destroy.
 * 
 * Revision 2.20  91/12/13  13:51:58  jsb
 * 	Use norma_ipc_copyin_page_list when sending to remote port.
 * 
 * Revision 2.19  91/12/10  13:25:46  jsb
 * 	Added ipc_kmsg_copyout_to_network, as required by ipc_kserver.c.
 * 	Picked up vm_map_convert_to_page_list call changes from dlb.
 * 	Changed NORMA_VM conditional for ipc_kmsg_copyout_to_kernel
 * 	to NORMA_IPC.
 * 	[91/12/10  11:20:36  jsb]
 * 
 * Revision 2.18  91/11/14  16:55:57  rpd
 * 	Picked up mysterious norma changes.
 * 	[91/11/14            rpd]
 * 
 * Revision 2.17  91/10/09  16:09:08  af
 * 	Changed msgh_kind to msgh_seqno in ipc_msg_print.
 * 	[91/10/05            rpd]
 * 
 * Revision 2.16  91/08/28  11:13:20  jsb
 * 	Changed msgh_kind to msgh_seqno.
 * 	[91/08/09            rpd]
 * 	Changed for new vm_map_copyout failure behavior.
 * 	[91/08/03            rpd]
 * 	Update page list discriminant logic to allow use of page list for
 * 	kernel objects that do not require page stealing (devices).
 * 	[91/07/31  15:00:55  dlb]b
 * 
 * 	Add arg to vm_map_copyin_page_list.
 * 	[91/07/30  14:10:38  dlb]
 * 
 * 	Turn page lists on by default.
 * 	[91/07/03  14:01:00  dlb]
 * 	Renamed clport fields in struct ipc_port to ip_norma fields.
 * 	Added checks for sending receive rights remotely.
 * 	[91/08/15  08:22:20  jsb]
 * 
 * Revision 2.15  91/08/03  18:18:16  jsb
 * 	Added support for ddb commands ``show msg'' and ``show kmsg''.
 * 	Made changes for elimination of intermediate clport structure.
 * 	[91/07/27  22:25:06  jsb]
 * 
 * 	Moved MACH_MSGH_BITS_COMPLEX_{PORTS,DATA} to mach/message.h.
 * 	Removed complex_data_hint_xxx[] garbage.
 * 	Adopted new vm_map_copy_t page_list technology.
 * 	[91/07/04  13:09:45  jsb]
 * 
 * Revision 2.14  91/07/01  08:24:34  jsb
 * 	From David Black at OSF: generalized page list support.
 * 	[91/06/29  16:29:29  jsb]
 * 
 * Revision 2.13  91/06/17  15:46:04  jsb
 * 	Renamed NORMA conditionals.
 * 	[91/06/17  10:45:05  jsb]
 * 
 * Revision 2.12  91/06/06  17:05:52  jsb
 * 	More NORMA_IPC stuff. Cleanup will follow.
 * 	[91/06/06  16:00:08  jsb]
 * 
 * Revision 2.11  91/05/14  16:33:01  mrt
 * 	Correcting copyright
 * 
 * Revision 2.10  91/03/16  14:47:57  rpd
 * 	Replaced ith_saved with ipc_kmsg_cache.
 * 	[91/02/16            rpd]
 * 
 * Revision 2.9  91/02/05  17:21:52  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  15:45:30  mrt]
 * 
 * Revision 2.8  91/01/08  15:13:49  rpd
 * 	Added ipc_kmsg_free.
 * 	[91/01/05            rpd]
 * 	Optimized ipc_kmsg_copyout_object for send rights.
 * 	[90/12/21            rpd]
 * 	Changed to use new copyinmsg/copyoutmsg operations.
 * 	Changed ipc_kmsg_get to check that the size is multiple of four.
 * 	[90/12/05            rpd]
 * 	Removed MACH_IPC_GENNOS.
 * 	[90/11/08            rpd]
 * 
 * Revision 2.7  90/11/05  14:28:36  rpd
 * 	Changed ip_reference to ipc_port_reference.
 * 	Changed ip_release to ipc_port_release.
 * 	Use new io_reference and io_release.
 * 	Use new ip_reference and ip_release.
 * 	[90/10/29            rpd]
 * 
 * Revision 2.6  90/09/09  14:31:50  rpd
 * 	Fixed ipc_kmsg_copyin_compat to clear unused bits instead
 * 	of returning an error when they are non-zero.
 * 	[90/09/08            rpd]
 * 
 * Revision 2.5  90/08/06  17:05:53  rpd
 * 	Fixed ipc_kmsg_copyout_body to turn off msgt_deallocate
 * 	for in-line data.  It might be on if the compatibility mode
 * 	generated the message.
 * 
 * 	Fixed ipc_kmsg_copyin, ipc_kmsg_copyin_compat to check
 * 	that msgt_name, msgt_size, msgt_number are zero
 * 	in long-form type descriptors.
 * 	[90/08/04            rpd]
 * 
 * 	Fixed atomicity bug in ipc_kmsg_copyout_header,
 * 	when the destination and reply ports are the same.
 * 	[90/08/02            rpd]
 * 
 * Revision 2.4  90/08/06  15:07:31  rwd
 * 	Fixed ipc_kmsg_clean_partial to deallocate correctly
 * 	the OOL memory in the last type spec.
 * 	Removed debugging panic in ipc_kmsg_put.
 * 	[90/06/21            rpd]
 * 
 * Revision 2.3  90/06/19  22:58:03  rpd
 * 	For debugging: added panic to ipc_kmsg_put.
 * 	[90/06/04            rpd]
 * 
 * Revision 2.2  90/06/02  14:50:05  rpd
 * 	Changed ocurrences of inline; it is a gcc keyword.
 * 	[90/06/02            rpd]
 * 
 * 	For out-of-line memory, if length is zero allow any address.
 * 	This is more compatible with old IPC.
 * 	[90/04/23            rpd]
 * 	Created for new IPC.
 * 	[90/03/26  20:55:45  rpd]
 * 
 * Revision 2.16.2.1  91/09/16  10:15:35  rpd
 * 	Removed unused variables.  Added <ipc/ipc_notify.h>.
 * 	[91/09/02            rpd]
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
 *	File:	ipc/ipc_kmsg.c
 *	Author:	Rich Draves
 *	Date:	1989
 *
 *	Operations on kernel messages.
 */

#include <cpus.h>
#include <norma_ipc.h>
#include <norma_vm.h>

#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/message.h>
#include <mach/port.h>
#include <mach/mach_server.h>
#include <kern/assert.h>
#include <kern/kalloc.h>
#include <kern/rtalloc.h>
#include <kern/spl.h>
#include <kern/misc_protos.h>
#include <vm/vm_map.h>
#include <vm/vm_object.h>
#include <vm/vm_kern.h>
#include <ipc/port.h>
#include <ipc/ipc_entry.h>
#include <ipc/ipc_kmsg.h>
#include <ipc/ipc_thread.h>
#include <ipc/ipc_notify.h>
#include <ipc/ipc_object.h>
#include <ipc/ipc_space.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_right.h>
#include <ipc/ipc_hash.h>
#include <ipc/ipc_table.h>
#include <device/net_io.h>
#include <string.h>

extern vm_map_t		ipc_kernel_copy_map;
extern vm_size_t	ipc_kmsg_max_vm_space;
extern vm_size_t	msg_ool_size_small;
extern vm_size_t	msg_ool_size_small_rt;

#define MSG_OOL_SIZE_SMALL(rt)	((rt) ? msg_ool_size_small_rt : \
				        msg_ool_size_small)
 /*
  *    We keep a per-processor cache of kernel message buffers.
  *    The cache saves the overhead/locking of using kalloc/kfree.
  *    The per-processor cache seems to miss less than a per-thread cache,
  *    and it also uses less memory.
  */

ipc_kmsg_t ipc_kmsg_cache[NCPUS];

/*
 * Forward declarations
 */

void ipc_kmsg_clean(
	ipc_kmsg_t	kmsg,
	boolean_t	rt);

void ipc_kmsg_clean_body(
    	ipc_kmsg_t	kmsg,
    	int		number,
	boolean_t	rt);

void ipc_kmsg_clean_partial(
	ipc_kmsg_t		kmsg,
	mach_msg_type_number_t	number,
	vm_offset_t		paddr,
	vm_size_t		length,
	boolean_t		rt);

mach_msg_return_t ipc_kmsg_copyout_body(
    	ipc_kmsg_t		kmsg,
	ipc_space_t		space,
	vm_map_t		map,
	mach_msg_body_t		*slist,
	boolean_t		rt);

mach_msg_return_t ipc_kmsg_copyin_body(
	ipc_kmsg_t		kmsg,
	ipc_space_t		space,
	vm_map_t		map,
	boolean_t		rt);

/*
 *	Routine:	ipc_kmsg_enqueue
 *	Purpose:
 *		Enqueue a kmsg.
 */

void
ipc_kmsg_enqueue(
	ipc_kmsg_queue_t	queue,
	ipc_kmsg_t		kmsg)
{
	ipc_kmsg_enqueue_macro(queue, kmsg);
}

/*
 *	Routine:	ipc_kmsg_dequeue
 *	Purpose:
 *		Dequeue and return a kmsg.
 */

ipc_kmsg_t
ipc_kmsg_dequeue(
	ipc_kmsg_queue_t	queue)
{
	ipc_kmsg_t first;

	first = ipc_kmsg_queue_first(queue);

	if (first != IKM_NULL)
		ipc_kmsg_rmqueue_first_macro(queue, first);

	return first;
}

/*
 *	Routine:	ipc_kmsg_rmqueue
 *	Purpose:
 *		Pull a kmsg out of a queue.
 */

void
ipc_kmsg_rmqueue(
	ipc_kmsg_queue_t	queue,
	ipc_kmsg_t		kmsg)
{
	ipc_kmsg_t next, prev;

	assert(queue->ikmq_base != IKM_NULL);

	next = kmsg->ikm_next;
	prev = kmsg->ikm_prev;

	if (next == kmsg) {
		assert(prev == kmsg);
		assert(queue->ikmq_base == kmsg);

		queue->ikmq_base = IKM_NULL;
	} else {
		if (queue->ikmq_base == kmsg)
			queue->ikmq_base = next;

		next->ikm_prev = prev;
		prev->ikm_next = next;
	}
	/* XXX Temporary debug logic */
	kmsg->ikm_next = IKM_BOGUS;
	kmsg->ikm_prev = IKM_BOGUS;
}

/*
 *	Routine:	ipc_kmsg_queue_next
 *	Purpose:
 *		Return the kmsg following the given kmsg.
 *		(Or IKM_NULL if it is the last one in the queue.)
 */

ipc_kmsg_t
ipc_kmsg_queue_next(
	ipc_kmsg_queue_t	queue,
	ipc_kmsg_t		kmsg)
{
	ipc_kmsg_t next;

	assert(queue->ikmq_base != IKM_NULL);

	next = kmsg->ikm_next;
	if (queue->ikmq_base == next)
		next = IKM_NULL;

	return next;
}

/*
 *	Routine:	ipc_kmsg_destroy
 *	Purpose:
 *		Destroys a kernel message.  Releases all rights,
 *		references, and memory held by the message.
 *		Frees the message.
 *	Conditions:
 *		No locks held.
 */

void
ipc_kmsg_destroy(
	ipc_kmsg_t	kmsg,
	boolean_t	rt)
{
	ipc_kmsg_queue_t queue;
	boolean_t empty;

	/*
	 *	ipc_kmsg_clean can cause more messages to be destroyed.
	 *	Curtail recursion by queueing messages.  If a message
	 *	is already queued, then this is a recursive call.
	 */

	queue = &current_thread()->ith_messages;
	empty = ipc_kmsg_queue_empty(queue);
	ipc_kmsg_enqueue(queue, kmsg);

	if (empty) {
		/* must leave kmsg in queue while cleaning it */

		while ((kmsg = ipc_kmsg_queue_first(queue)) != IKM_NULL) {
			ipc_kmsg_clean(kmsg, rt);
			ipc_kmsg_rmqueue(queue, kmsg);
			ikm_free(kmsg, rt);
		}
	}
}

/*
 *	Routine:	ipc_kmsg_clean_body
 *	Purpose:
 *		Cleans the body of a kernel message.
 *		Releases all rights, references, and memory.
 *
 *	Conditions:
 *		No locks held.
 */

void
ipc_kmsg_clean_body(
	ipc_kmsg_t	kmsg,
	int		number,
	boolean_t	rt)
{
    int				i;
    ipc_object_t    		object;
    mach_msg_descriptor_t 	*saddr, *eaddr;
    mach_msg_bits_t 		mbits = kmsg->ikm_header.msgh_bits;

    if ( number == 0 )
	return;

    saddr = (mach_msg_descriptor_t *) 
			((mach_msg_base_t *) &kmsg->ikm_header + 1);
    eaddr = saddr + number;

    for ( ; saddr < eaddr; saddr++ ) {
	
	switch (saddr->type.type) {
	    
	    case MACH_MSG_PORT_DESCRIPTOR: {
		mach_msg_port_descriptor_t *dsc;

		dsc = &saddr->port;

		/* 
		 * Destroy port rights carried in the message 
		 */
		if (!IO_VALID((ipc_object_t) dsc->name))
		    continue;
		ipc_object_destroy((ipc_object_t) dsc->name, dsc->disposition);
		break;
	    }
	    case MACH_MSG_OOL_DESCRIPTOR : {
		mach_msg_ool_descriptor_t *dsc;

		dsc = &saddr->out_of_line;
		
		/* 
		 * Destroy memory carried in the message 
		 */
		if (dsc->size == 0) {
		    assert(dsc->address == (void *) 0);
		} else {
		    if (dsc->copy == MACH_MSG_PHYSICAL_COPY &&
			    dsc->size < MSG_OOL_SIZE_SMALL(rt)) {
		        FREE((vm_offset_t)dsc->address,
			     (vm_size_t)dsc->size,
			     rt);
		    } else {
		    	vm_map_copy_discard((vm_map_copy_t) dsc->address);
		    }
		}
		break;
	    }
	    case MACH_MSG_OOL_PORTS_DESCRIPTOR : {
		ipc_object_t             	*objects;
		mach_msg_type_number_t   	j;
		vm_offset_t              	data;
		mach_msg_ool_ports_descriptor_t	*dsc;

		dsc = &saddr->ool_ports;
		objects = (ipc_object_t *) dsc->address;

		if (dsc->count == 0) {
			break;
		}

		assert(objects != (ipc_object_t *) 0);
		
		/* destroy port rights carried in the message */
		
		for (j = 0; j < dsc->count; j++) {
		    ipc_object_t object = objects[j];
		    
		    if (!IO_VALID(object))
			continue;
		    
		    ipc_object_destroy(object, dsc->disposition);
		}

		/* destroy memory carried in the message */

		assert(dsc->count != 0);

		FREE((vm_offset_t) dsc->address, 
		     (vm_size_t) dsc->count * sizeof(mach_port_t),
		     rt);
		break;
	    }
	    default : {
		printf("cleanup: don't understand this type of descriptor\n");
	    }
	}
    }
}

/*
 *	Routine:	ipc_kmsg_clean_partial
 *	Purpose:
 *		Cleans a partially-acquired kernel message.
 *		number is the index of the type descriptor
 *		in the body of the message that contained the error.
 *		If dolast, the memory and port rights in this last
 *		type spec are also cleaned.  In that case, number
 *		specifies the number of port rights to clean.
 *	Conditions:
 *		Nothing locked.
 */

void
ipc_kmsg_clean_partial(
	ipc_kmsg_t		kmsg,
	mach_msg_type_number_t	number,
	vm_offset_t		paddr,
	vm_size_t		length,
	boolean_t		rt)
{
	ipc_object_t object;
	mach_msg_bits_t mbits = kmsg->ikm_header.msgh_bits;
	mach_msg_body_t saddr;

	object = (ipc_object_t) kmsg->ikm_header.msgh_remote_port;
	assert(IO_VALID(object));
	ipc_object_destroy(object, MACH_MSGH_BITS_REMOTE(mbits));

	object = (ipc_object_t) kmsg->ikm_header.msgh_local_port;
	if (IO_VALID(object))
		ipc_object_destroy(object, MACH_MSGH_BITS_LOCAL(mbits));

	if (paddr) {
		(void) vm_deallocate(ipc_kernel_copy_map, paddr, length);
	}

	ipc_kmsg_clean_body(kmsg, number, rt);
}

/*
 *	Routine:	ipc_kmsg_clean
 *	Purpose:
 *		Cleans a kernel message.  Releases all rights,
 *		references, and memory held by the message.
 *	Conditions:
 *		No locks held.
 */

void
ipc_kmsg_clean(
	ipc_kmsg_t	kmsg,
	boolean_t	rt)
{
	ipc_object_t object;
	mach_msg_bits_t mbits = kmsg->ikm_header.msgh_bits;

	object = (ipc_object_t) kmsg->ikm_header.msgh_remote_port;
	if (IO_VALID(object))
		ipc_object_destroy(object, MACH_MSGH_BITS_REMOTE(mbits));

	object = (ipc_object_t) kmsg->ikm_header.msgh_local_port;
	if (IO_VALID(object))
		ipc_object_destroy(object, MACH_MSGH_BITS_LOCAL(mbits));

	if (mbits & MACH_MSGH_BITS_COMPLEX) {
		mach_msg_body_t *body;

		body = (mach_msg_body_t *) (&kmsg->ikm_header + 1);
		ipc_kmsg_clean_body(kmsg, body->msgh_descriptor_count, rt);
	}
}

/*
 *	Routine:	ipc_kmsg_free
 *	Purpose:
 *		Free a kernel message buffer.
 *	Conditions:
 *		Nothing locked.
 */

void
ipc_kmsg_free(
	ipc_kmsg_t	kmsg,
	boolean_t	rt)
{
	vm_size_t size = kmsg->ikm_size;

	switch (size) {
#if	NORMA_IPC
	    case IKM_SIZE_NORMA:
		/* return it to the norma ipc code */
		norma_kmsg_put(kmsg);
		break;
#endif	/* NORMA_IPC */

	    case IKM_SIZE_NETWORK:
		/* return it to the network code */
		net_kmsg_put(kmsg);
		break;

	    default:
		FREE((vm_offset_t) kmsg, size, rt);
		break;
	}
}

/*
 *	Routine:	ipc_kmsg_get
 *	Purpose:
 *		Allocates a kernel message buffer.
 *		Copies a user message to the message buffer.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		MACH_MSG_SUCCESS	Acquired a message buffer.
 *		MACH_SEND_MSG_TOO_SMALL	Message smaller than a header.
 *		MACH_SEND_MSG_TOO_SMALL	Message size not long-word multiple.
 *		MACH_SEND_NO_BUFFER	Couldn't allocate a message buffer.
 *		MACH_SEND_INVALID_DATA	Couldn't copy message data.
 */

mach_msg_return_t
ipc_kmsg_get(
	mach_msg_header_t	*msg,
	mach_msg_size_t		size,
	ipc_kmsg_t		*kmsgp,
	ipc_space_t		space,
	boolean_t		*rtp)
{
	mach_msg_size_t			msg_and_trailer_size;
	ipc_kmsg_t 			kmsg;
	mach_msg_format_0_trailer_t 	*trailer;
	mach_port_t			dest_name;
	ipc_entry_t			dest_entry;
	ipc_port_t			dest_port;

	if ((size < sizeof(mach_msg_header_t)) || (size & 3))
		return MACH_SEND_MSG_TOO_SMALL;

	/*
	 * Copyin just the destination mach_port_t
	 */
	if (copyinmsg((char *) &msg->msgh_remote_port,
		      (char *) &dest_name,
		      sizeof(mach_port_t))) {
		return MACH_SEND_INVALID_DATA;
	}
	
	/*
	 * Validate the space
	 */
	is_write_lock(space);
	if (!space->is_active) {
	  is_write_unlock(space);
	  return MACH_SEND_INVALID_DEST;
	}
	
	/*
	 * Lookup and validate the entry
	 */
	dest_entry = ipc_entry_lookup(space, dest_name);
	if (dest_entry == IE_NULL) {
	  is_write_unlock(space);
	  return MACH_SEND_INVALID_DEST;
        }

	/*
	 * Extract the port and check whether it is a RT port
	 */
	dest_port = (ipc_port_t) dest_entry->ie_object;
	if (dest_port == IP_NULL) {
	  is_write_unlock(space);
	  return MACH_SEND_INVALID_DEST;
        }
	  
	*rtp = IP_RT(dest_port);
	is_write_unlock(space);

	msg_and_trailer_size = size + MAX_TRAILER_SIZE;

	if (msg_and_trailer_size <= IKM_SAVED_MSG_SIZE) {
		if ((!*rtp) && ikm_cache_get(&kmsg)) {
			ikm_check_initialized(kmsg, IKM_SAVED_KMSG_SIZE);
		} else {
			kmsg = ikm_alloc(IKM_SAVED_MSG_SIZE, *rtp);
			if (kmsg == IKM_NULL)
				return MACH_SEND_NO_BUFFER;
			ikm_init(kmsg, IKM_SAVED_MSG_SIZE);
		}
	} else {
		kmsg = ikm_alloc(msg_and_trailer_size, *rtp);

		if (kmsg == IKM_NULL)
			return MACH_SEND_NO_BUFFER;
		ikm_init(kmsg, msg_and_trailer_size);
	}

	if (copyinmsg((char *) msg, (char *) &kmsg->ikm_header, size)) {
		ikm_free(kmsg, *rtp);
		return MACH_SEND_INVALID_DATA;
	}

	kmsg->ikm_header.msgh_size = size;

	/* 
	 * I reserve for the trailer the largest space (MAX_TRAILER_SIZE)
	 * However, the internal size field of the trailer (msgh_trailer_size)
	 * is initialized to the minimum (sizeof(mach_msg_trailer_t)), to optimize
	 * the cases where no implicit data is requested.
	 */
	trailer = (mach_msg_format_0_trailer_t *) ((vm_offset_t)&kmsg->ikm_header + size);
	trailer->msgh_sender = KERNEL_SECURITY_TOKEN;
	trailer->msgh_trailer_type = MACH_MSG_TRAILER_FORMAT_0;
	trailer->msgh_trailer_size = MACH_MSG_TRAILER_MINIMUM_SIZE;

	*kmsgp = kmsg;
	return MACH_MSG_SUCCESS;
}

/*
 *	Routine:	ipc_kmsg_get_from_kernel
 *	Purpose:
 *		Allocates a kernel message buffer.
 *		Copies a kernel message to the message buffer.
 *		Only resource errors are allowed.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		MACH_MSG_SUCCESS	Acquired a message buffer.
 *		MACH_SEND_NO_BUFFER	Couldn't allocate a message buffer.
 */

extern mach_msg_return_t
ipc_kmsg_get_from_kernel(
	mach_msg_header_t	*msg,
	mach_msg_size_t		size,
	ipc_kmsg_t		*kmsgp,
	boolean_t		*rtp)
{
	ipc_kmsg_t 	kmsg;
	mach_msg_size_t	msg_and_trailer_size;
	mach_msg_format_0_trailer_t *trailer;
	ipc_port_t	dest_port;

	assert(size >= sizeof(mach_msg_header_t));
	assert((size & 3) == 0);

	/* naturally align the message before tacking on the trailer */
	msg_and_trailer_size = size + MAX_TRAILER_SIZE;

	assert(IP_VALID((ipc_port_t) msg->msgh_remote_port));
	*rtp = IP_RT((ipc_port_t) msg->msgh_remote_port);

	kmsg = ikm_alloc(msg_and_trailer_size, *rtp);

	if (kmsg == IKM_NULL)
		return MACH_SEND_NO_BUFFER;
	ikm_init(kmsg, msg_and_trailer_size);

	(void) memcpy((void *) &kmsg->ikm_header, (const void *) msg, size);

	kmsg->ikm_header.msgh_size = size;

	/* 
	 * I reserve for the trailer the largest space (MAX_TRAILER_SIZE)
	 * However, the internal size field of the trailer (msgh_trailer_size)
	 * is initialized to the minimum (sizeof(mach_msg_trailer_t)), to optimize
	 * the cases where no implicit data is requested.
	 */
	trailer = (mach_msg_format_0_trailer_t *) ((vm_offset_t)&kmsg->ikm_header + size);
	trailer->msgh_sender = KERNEL_SECURITY_TOKEN;
	trailer->msgh_trailer_type = MACH_MSG_TRAILER_FORMAT_0;
	trailer->msgh_trailer_size = MACH_MSG_TRAILER_MINIMUM_SIZE;

	*kmsgp = kmsg;
	return MACH_MSG_SUCCESS;
}

/*
 *	Routine:	ipc_kmsg_put
 *	Purpose:
 *		Copies a message buffer to a user message.
 *		Copies only the specified number of bytes.
 *		Frees the message buffer.
 *	Conditions:
 *		Nothing locked.  The message buffer must have clean
 *		header fields.
 *	Returns:
 *		MACH_MSG_SUCCESS	Copied data out of message buffer.
 *		MACH_RCV_INVALID_DATA	Couldn't copy to user message.
 */

mach_msg_return_t
ipc_kmsg_put(
	mach_msg_header_t	*msg,
	ipc_kmsg_t		kmsg,
	mach_msg_size_t		size,
	boolean_t		rt)
{
	mach_msg_return_t mr;

	ikm_check_initialized(kmsg, kmsg->ikm_size);

	if (copyoutmsg((const char *) &kmsg->ikm_header, (char *) msg, size))
		mr = MACH_RCV_INVALID_DATA;
	else
		mr = MACH_MSG_SUCCESS;

	if (kmsg->ikm_size != IKM_SAVED_KMSG_SIZE || rt ||
	    !ikm_cache_put(kmsg))
		ikm_free(kmsg, rt);

	return mr;
}

/*
 *	Routine:	ipc_kmsg_put_to_kernel
 *	Purpose:
 *		Copies a message buffer to a kernel message.
 *		Frees the message buffer.
 *		No errors allowed.
 *	Conditions:
 *		Nothing locked.
 */

void
ipc_kmsg_put_to_kernel(
	mach_msg_header_t	*msg,
	ipc_kmsg_t		kmsg,
	mach_msg_size_t		size)
{
	(void) memcpy((void *) msg, (const void *) &kmsg->ikm_header, size);

	ikm_free(kmsg, FALSE);
}

/*
 *	Routine:	ipc_kmsg_copyin_header
 *	Purpose:
 *		"Copy-in" port rights in the header of a message.
 *		Operates atomically; if it doesn't succeed the
 *		message header and the space are left untouched.
 *		If it does succeed the remote/local port fields
 *		contain object pointers instead of port names,
 *		and the bits field is updated.  The destination port
 *		will be a valid port pointer.
 *
 *		The notify argument implements the MACH_SEND_CANCEL option.
 *		If it is not MACH_PORT_NULL, it should name a receive right.
 *		If the processing of the destination port would generate
 *		a port-deleted notification (because the right for the
 *		destination port is destroyed and it had a request for
 *		a dead-name notification registered), and the port-deleted
 *		notification would be sent to the named receive right,
 *		then it isn't sent and the send-once right for the notify
 *		port is quietly destroyed.
 *
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		MACH_MSG_SUCCESS	Successful copyin.
 *		MACH_SEND_INVALID_HEADER
 *			Illegal value in the message header bits.
 *		MACH_SEND_INVALID_DEST	The space is dead.
 *		MACH_SEND_INVALID_NOTIFY
 *			Notify is non-null and doesn't name a receive right.
 *			(Either KERN_INVALID_NAME or KERN_INVALID_RIGHT.)
 *		MACH_SEND_INVALID_DEST	Can't copyin destination port.
 *			(Either KERN_INVALID_NAME or KERN_INVALID_RIGHT.)
 *		MACH_SEND_INVALID_REPLY	Can't copyin reply port.
 *			(Either KERN_INVALID_NAME or KERN_INVALID_RIGHT.)
 */

mach_msg_return_t
ipc_kmsg_copyin_header(
	mach_msg_header_t	*msg,
	ipc_space_t		space,
	mach_port_t		notify)
{
	mach_msg_bits_t mbits = msg->msgh_bits &~ MACH_MSGH_BITS_CIRCULAR;
	mach_port_t dest_name = msg->msgh_remote_port;
	mach_port_t reply_name = msg->msgh_local_port;
	kern_return_t kr;

    {
	mach_msg_type_name_t dest_type = MACH_MSGH_BITS_REMOTE(mbits);
	mach_msg_type_name_t reply_type = MACH_MSGH_BITS_LOCAL(mbits);
	ipc_object_t dest_port, reply_port;
	ipc_port_t dest_soright, reply_soright;
	ipc_port_t notify_port;

	if (!MACH_MSG_TYPE_PORT_ANY_SEND(dest_type))
		return MACH_SEND_INVALID_HEADER;

	if ((reply_type == 0) ?
	    (reply_name != MACH_PORT_NULL) :
	    !MACH_MSG_TYPE_PORT_ANY_SEND(reply_type))
		return MACH_SEND_INVALID_HEADER;

	is_write_lock(space);
	if (!space->is_active)
		goto invalid_dest;

	if (notify != MACH_PORT_NULL) {
		ipc_entry_t entry;

		if (((entry = ipc_entry_lookup(space, notify)) == IE_NULL) ||
		    ((entry->ie_bits & MACH_PORT_TYPE_RECEIVE) == 0)) {
			is_write_unlock(space);
			return MACH_SEND_INVALID_NOTIFY;
		}

		notify_port = (ipc_port_t) entry->ie_object;
	}

	if (dest_name == reply_name) {
		ipc_entry_t entry;
		mach_port_t name = dest_name;

		/*
		 *	Destination and reply ports are the same!
		 *	This is a little tedious to make atomic, because
		 *	there are 25 combinations of dest_type/reply_type.
		 *	However, most are easy.  If either is move-sonce,
		 *	then there must be an error.  If either are
		 *	make-send or make-sonce, then we must be looking
		 *	at a receive right so the port can't die.
		 *	The hard cases are the combinations of
		 *	copy-send and make-send.
		 */

		entry = ipc_entry_lookup(space, name);
		if (entry == IE_NULL)
			goto invalid_dest;

		assert(reply_type != 0); /* because name not null */

		if (!ipc_right_copyin_check(space, name, entry, reply_type))
			goto invalid_reply;

		if ((dest_type == MACH_MSG_TYPE_MOVE_SEND_ONCE) ||
		    (reply_type == MACH_MSG_TYPE_MOVE_SEND_ONCE)) {
			/*
			 *	Why must there be an error?  To get a valid
			 *	destination, this entry must name a live
			 *	port (not a dead name or dead port).  However
			 *	a successful move-sonce will destroy a
			 *	live entry.  Therefore the other copyin,
			 *	whatever it is, would fail.  We've already
			 *	checked for reply port errors above,
			 *	so report a destination error.
			 */

			goto invalid_dest;
		} else if ((dest_type == MACH_MSG_TYPE_MAKE_SEND) ||
			   (dest_type == MACH_MSG_TYPE_MAKE_SEND_ONCE) ||
			   (reply_type == MACH_MSG_TYPE_MAKE_SEND) ||
			   (reply_type == MACH_MSG_TYPE_MAKE_SEND_ONCE)) {
			kr = ipc_right_copyin(space, name, entry,
					      dest_type, FALSE,
					      &dest_port, &dest_soright);
			if (kr != KERN_SUCCESS)
				goto invalid_dest;

			/*
			 *	Either dest or reply needs a receive right.
			 *	We know the receive right is there, because
			 *	of the copyin_check and copyin calls.  Hence
			 *	the port is not in danger of dying.  If dest
			 *	used the receive right, then the right needed
			 *	by reply (and verified by copyin_check) will
			 *	still be there.
			 */

			assert(IO_VALID(dest_port));
			assert(entry->ie_bits & MACH_PORT_TYPE_RECEIVE);
			assert(dest_soright == IP_NULL);

			kr = ipc_right_copyin(space, name, entry,
					      reply_type, TRUE,
					      &reply_port, &reply_soright);

			assert(kr == KERN_SUCCESS);
			assert(reply_port == dest_port);
			assert(entry->ie_bits & MACH_PORT_TYPE_RECEIVE);
			assert(reply_soright == IP_NULL);
		} else if ((dest_type == MACH_MSG_TYPE_COPY_SEND) &&
			   (reply_type == MACH_MSG_TYPE_COPY_SEND)) {
			/*
			 *	To make this atomic, just do one copy-send,
			 *	and dup the send right we get out.
			 */

			kr = ipc_right_copyin(space, name, entry,
					      dest_type, FALSE,
					      &dest_port, &dest_soright);
			if (kr != KERN_SUCCESS)
				goto invalid_dest;

			assert(entry->ie_bits & MACH_PORT_TYPE_SEND);
			assert(dest_soright == IP_NULL);

			/*
			 *	It's OK if the port we got is dead now,
			 *	so reply_port is IP_DEAD, because the msg
			 *	won't go anywhere anyway.
			 */

			reply_port = (ipc_object_t)
				ipc_port_copy_send((ipc_port_t) dest_port);
			reply_soright = IP_NULL;
		} else if ((dest_type == MACH_MSG_TYPE_MOVE_SEND) &&
			   (reply_type == MACH_MSG_TYPE_MOVE_SEND)) {
			/*
			 *	This is an easy case.  Just use our
			 *	handy-dandy special-purpose copyin call
			 *	to get two send rights for the price of one.
			 */

			kr = ipc_right_copyin_two(space, name, entry,
						  &dest_port, &dest_soright);
			if (kr != KERN_SUCCESS)
				goto invalid_dest;

			/* the entry might need to be deallocated */

			if (IE_BITS_TYPE(entry->ie_bits)
						== MACH_PORT_TYPE_NONE)
				ipc_entry_dealloc(space, name, entry);

			reply_port = dest_port;
			reply_soright = IP_NULL;
		} else {
			ipc_port_t soright;

			assert(((dest_type == MACH_MSG_TYPE_COPY_SEND) &&
				(reply_type == MACH_MSG_TYPE_MOVE_SEND)) ||
			       ((dest_type == MACH_MSG_TYPE_MOVE_SEND) &&
				(reply_type == MACH_MSG_TYPE_COPY_SEND)));

			/*
			 *	To make this atomic, just do a move-send,
			 *	and dup the send right we get out.
			 */

			kr = ipc_right_copyin(space, name, entry,
					      MACH_MSG_TYPE_MOVE_SEND, FALSE,
					      &dest_port, &soright);
			if (kr != KERN_SUCCESS)
				goto invalid_dest;

			/* the entry might need to be deallocated */

			if (IE_BITS_TYPE(entry->ie_bits)
						== MACH_PORT_TYPE_NONE)
				ipc_entry_dealloc(space, name, entry);

			/*
			 *	It's OK if the port we got is dead now,
			 *	so reply_port is IP_DEAD, because the msg
			 *	won't go anywhere anyway.
			 */

			reply_port = (ipc_object_t)
				ipc_port_copy_send((ipc_port_t) dest_port);

			if (dest_type == MACH_MSG_TYPE_MOVE_SEND) {
				dest_soright = soright;
				reply_soright = IP_NULL;
			} else {
				dest_soright = IP_NULL;
				reply_soright = soright;
			}
		}
	} else if (!MACH_PORT_VALID(reply_name)) {
		ipc_entry_t entry;

		/*
		 *	No reply port!  This is an easy case
		 *	to make atomic.  Just copyin the destination.
		 */

		entry = ipc_entry_lookup(space, dest_name);
		if (entry == IE_NULL)
			goto invalid_dest;

		kr = ipc_right_copyin(space, dest_name, entry,
				      dest_type, FALSE,
				      &dest_port, &dest_soright);
		if (kr != KERN_SUCCESS)
			goto invalid_dest;

		/* the entry might need to be deallocated */

		if (IE_BITS_TYPE(entry->ie_bits) == MACH_PORT_TYPE_NONE)
			ipc_entry_dealloc(space, dest_name, entry);

		reply_port = (ipc_object_t) reply_name;
		reply_soright = IP_NULL;
	} else {
		ipc_entry_t dest_entry, reply_entry;
		ipc_port_t saved_reply;

		/*
		 *	This is the tough case to make atomic.
		 *	The difficult problem is serializing with port death.
		 *	At the time we copyin dest_port, it must be alive.
		 *	If reply_port is alive when we copyin it, then
		 *	we are OK, because we serialize before the death
		 *	of both ports.  Assume reply_port is dead at copyin.
		 *	Then if dest_port dies/died after reply_port died,
		 *	we are OK, because we serialize between the death
		 *	of the two ports.  So the bad case is when dest_port
		 *	dies after its copyin, reply_port dies before its
		 *	copyin, and dest_port dies before reply_port.  Then
		 *	the copyins operated as if dest_port was alive
		 *	and reply_port was dead, which shouldn't have happened
		 *	because they died in the other order.
		 *
		 *	We handle the bad case by undoing the copyins
		 *	(which is only possible because the ports are dead)
		 *	and failing with MACH_SEND_INVALID_DEST, serializing
		 *	after the death of the ports.
		 *
		 *	Note that it is easy for a user task to tell if
		 *	a copyin happened before or after a port died.
		 *	For example, suppose both dest and reply are
		 *	send-once rights (types are both move-sonce) and
		 *	both rights have dead-name requests registered.
		 *	If a port dies before copyin, a dead-name notification
		 *	is generated and the dead name's urefs are incremented,
		 *	and if the copyin happens first, a port-deleted
		 *	notification is generated.
		 *
		 *	Note that although the entries are different,
		 *	dest_port and reply_port might still be the same.
		 */

		dest_entry = ipc_entry_lookup(space, dest_name);
		if (dest_entry == IE_NULL)
			goto invalid_dest;

		reply_entry = ipc_entry_lookup(space, reply_name);
		if (reply_entry == IE_NULL)
			goto invalid_reply;

		assert(dest_entry != reply_entry); /* names are not equal */
		assert(reply_type != 0); /* because reply_name not null */

		if (!ipc_right_copyin_check(space, reply_name, reply_entry,
					    reply_type))
			goto invalid_reply;

		kr = ipc_right_copyin(space, dest_name, dest_entry,
				      dest_type, FALSE,
				      &dest_port, &dest_soright);
		if (kr != KERN_SUCCESS)
			goto invalid_dest;

		assert(IO_VALID(dest_port));

		saved_reply = (ipc_port_t) reply_entry->ie_object;
		/* might be IP_NULL, if this is a dead name */
		if (saved_reply != IP_NULL)
			ipc_port_reference(saved_reply);

		kr = ipc_right_copyin(space, reply_name, reply_entry,
				      reply_type, TRUE,
				      &reply_port, &reply_soright);
		assert(kr == KERN_SUCCESS);

		if ((saved_reply != IP_NULL) && (reply_port == IO_DEAD)) {
			ipc_port_t dest = (ipc_port_t) dest_port;
			ipc_port_timestamp_t timestamp;
			boolean_t must_undo;

			/*
			 *	The reply port died before copyin.
			 *	Check if dest port died before reply.
			 */

			ip_lock(saved_reply);
			assert(!ip_active(saved_reply));
			timestamp = saved_reply->ip_timestamp;
			ip_unlock(saved_reply);

			ip_lock(dest);
			must_undo = (!ip_active(dest) &&
				     IP_TIMESTAMP_ORDER(dest->ip_timestamp,
							timestamp));
			ip_unlock(dest);

			if (must_undo) {
				/*
				 *	Our worst nightmares are realized.
				 *	Both destination and reply ports
				 *	are dead, but in the wrong order,
				 *	so we must undo the copyins and
				 *	possibly generate a dead-name notif.
				 */

				ipc_right_copyin_undo(
						space, dest_name, dest_entry,
						dest_type, dest_port,
						dest_soright);
				/* dest_entry may be deallocated now */

				ipc_right_copyin_undo(
						space, reply_name, reply_entry,
						reply_type, reply_port,
						reply_soright);
				/* reply_entry may be deallocated now */

				is_write_unlock(space);

				if (dest_soright != IP_NULL)
					ipc_notify_dead_name(dest_soright,
							     dest_name);
				assert(reply_soright == IP_NULL);

				ipc_port_release(saved_reply);
				return MACH_SEND_INVALID_DEST;
			}
		}

		/* the entries might need to be deallocated */

		if (IE_BITS_TYPE(reply_entry->ie_bits) == MACH_PORT_TYPE_NONE)
			ipc_entry_dealloc(space, reply_name, reply_entry);

		if (IE_BITS_TYPE(dest_entry->ie_bits) == MACH_PORT_TYPE_NONE)
			ipc_entry_dealloc(space, dest_name, dest_entry);

		if (saved_reply != IP_NULL)
			ipc_port_release(saved_reply);
	}

	/*
	 *	At this point, dest_port, reply_port,
	 *	dest_soright, reply_soright are all initialized.
	 *	Any defunct entries have been deallocated.
	 *	The space is still write-locked, and we need to
	 *	make the MACH_SEND_CANCEL check.  The notify_port pointer
	 *	is still usable, because the copyin code above won't ever
	 *	deallocate a receive right, so its entry still exists
	 *	and holds a ref.  Note notify_port might even equal
	 *	dest_port or reply_port.
	 */

	if ((notify != MACH_PORT_NULL) &&
	    (dest_soright == notify_port)) {
		ipc_port_release_sonce(dest_soright);
		dest_soright = IP_NULL;
	}

	is_write_unlock(space);

	if (dest_soright != IP_NULL)
		ipc_notify_port_deleted(dest_soright, dest_name);

	if (reply_soright != IP_NULL)
		ipc_notify_port_deleted(reply_soright, reply_name);

	dest_type = ipc_object_copyin_type(dest_type);
	reply_type = ipc_object_copyin_type(reply_type);

	msg->msgh_bits = (MACH_MSGH_BITS_OTHER(mbits) |
			  MACH_MSGH_BITS(dest_type, reply_type));
	msg->msgh_remote_port = (mach_port_t) dest_port;
	msg->msgh_local_port = (mach_port_t) reply_port;
    }

	return MACH_MSG_SUCCESS;

    invalid_dest:
	is_write_unlock(space);
	return MACH_SEND_INVALID_DEST;

    invalid_reply:
	is_write_unlock(space);
	return MACH_SEND_INVALID_REPLY;
}

#if	NORMA_IPC
#define MEMORY_OBJECT_DATA_PROVIDED_MSG  ((mach_msg_id_t)2038)
#define	MUST_STEAL_CROCK(kmsg)						\
	(kmsg->ikm_header.msgh_id ==  MEMORY_OBJECT_DATA_PROVIDED_MSG)
#endif	/* NORMA_IPC */

/*
 *	Routine:	ipc_kmsg_copyin_body
 *	Purpose:
 *		"Copy-in" port rights and out-of-line memory
 *		in the message body.
 *
 *		In all failure cases, the message is left holding
 *		no rights or memory.  However, the message buffer
 *		is not deallocated.  If successful, the message
 *		contains a valid destination port.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		MACH_MSG_SUCCESS	Successful copyin.
 *		MACH_SEND_INVALID_MEMORY	Can't grab out-of-line memory.
 *		MACH_SEND_INVALID_RIGHT	Can't copyin port right in body.
 *		MACH_SEND_INVALID_TYPE	Bad type specification.
 *		MACH_SEND_MSG_TOO_SMALL	Body is too small for types/data.
 */

mach_msg_return_t
ipc_kmsg_copyin_body(
	ipc_kmsg_t	kmsg,
	ipc_space_t	space,
	vm_map_t	map,
	boolean_t	rt)
{
    ipc_object_t       		dest;
    mach_msg_body_t		*body;
    mach_msg_descriptor_t	*saddr, *eaddr;
    boolean_t 			complex;
    mach_msg_return_t 		mr;
    boolean_t 			use_page_lists, steal_pages;
    int				i;
    kern_return_t		kr;
    vm_size_t			space_needed = 0;
    vm_offset_t			paddr = 0;
    mach_msg_descriptor_t	*sstart;
    vm_map_copy_t		copy = VM_MAP_COPY_NULL;
    
    /*
     * Determine if the target is a kernel port.
     */
    dest = (ipc_object_t) kmsg->ikm_header.msgh_remote_port;
    complex = FALSE;
    use_page_lists = ipc_kobject_vm_page_list(ip_kotype((ipc_port_t)dest));
    steal_pages = ipc_kobject_vm_page_steal(ip_kotype((ipc_port_t)dest));

#if	NORMA_IPC
    if (IP_NORMA_IS_PROXY((ipc_port_t) dest)) {
	use_page_lists = TRUE;
	steal_pages = MUST_STEAL_CROCK(kmsg); /* FALSE unless CROCK req'd */
    }
#endif	/* NORMA_IPC */
    
    body = (mach_msg_body_t *) (&kmsg->ikm_header + 1);
    saddr = (mach_msg_descriptor_t *) (body + 1);
    eaddr = saddr + body->msgh_descriptor_count;
    
    /* make sure the message does not ask for more msg descriptors
     * than the message can hold.
     */
    
    if (eaddr <= saddr ||
	eaddr > (mach_msg_descriptor_t *) (&kmsg->ikm_header + 
			    kmsg->ikm_header.msgh_size)) {
	ipc_kmsg_clean_partial(kmsg,0,0,0,rt);
	return MACH_SEND_MSG_TOO_SMALL;
    }
    
    /*
     * Make an initial pass to determine kernal VM space requirements for
     * physical copies.
     */
    for (sstart = saddr; sstart <  eaddr; sstart++) {

	if (sstart->type.type == MACH_MSG_OOL_DESCRIPTOR) {

		assert(!(sstart->out_of_line.copy == MACH_MSG_PHYSICAL_COPY &&
		       (use_page_lists || steal_pages)));

		if (sstart->out_of_line.copy != MACH_MSG_PHYSICAL_COPY &&
		    sstart->out_of_line.copy != MACH_MSG_VIRTUAL_COPY) {
		    /*
		     * Invalid copy option
		     */
		    ipc_kmsg_clean_partial(kmsg,0,0,0,rt);
		    return MACH_SEND_INVALID_TYPE;
		}
		
		if (sstart->out_of_line.copy == MACH_MSG_PHYSICAL_COPY && 
		    sstart->out_of_line.size >= MSG_OOL_SIZE_SMALL(rt) &&
		    !sstart->out_of_line.deallocate) {

		     	/*
		      	 * Out-of-line memory descriptor, accumulate kernel
		      	 * memory requirements
		      	 */
		    	space_needed += round_page(sstart->out_of_line.size);
		    	if (space_needed > ipc_kmsg_max_vm_space) {

			    	/*
			     	 * Per message kernel memory limit exceeded
			     	 */
			    	ipc_kmsg_clean_partial(kmsg,0,0,0,rt);
		            	return MACH_MSG_VM_KERNEL;
		    	}
	        }
	}
    }

    /*
     * Allocate space in the pageable kernel ipc copy map for all the
     * ool data that is to be physically copied.  Map is marked wait for
     * space.
     */
    if (space_needed) {
	if (rt) {
		printf("ipc_kmsg_copyin: using vm_allocate for RT OOL data\n");
	}
	if (vm_allocate(ipc_kernel_copy_map, &paddr, space_needed, 
				TRUE) != KERN_SUCCESS) {
		ipc_kmsg_clean_partial(kmsg,0,0,0,rt);
		return MACH_MSG_VM_KERNEL;
	}
    }

    /* 
     * handle the OOL regions and port descriptors.
     * the check for complex messages was done earlier.
     */
    
    for (i = 0, sstart = saddr; sstart <  eaddr; sstart++) {
	
	switch (sstart->type.type) {
	    
	    case MACH_MSG_PORT_DESCRIPTOR: {
		mach_msg_type_name_t 		name;
		ipc_object_t 			object;
		mach_msg_port_descriptor_t 	*dsc;
		
		dsc = &sstart->port;
		
		/* this is really the type SEND, SEND_ONCE, etc. */
		name = dsc->disposition;
		dsc->disposition = ipc_object_copyin_type(name);
		
		if (!MACH_PORT_VALID(dsc->name)) {
		    complex = TRUE;
		    break;
		}
		kr = ipc_object_copyin(space, dsc->name, name, &object);
		if (kr != KERN_SUCCESS) {
		    ipc_kmsg_clean_partial(kmsg, i, paddr, space_needed, rt);
		    return MACH_SEND_INVALID_RIGHT;
		}
		if ((dsc->disposition == MACH_MSG_TYPE_PORT_RECEIVE) &&
		    ipc_port_check_circularity((ipc_port_t) object,
					       (ipc_port_t) dest)) {
		    kmsg->ikm_header.msgh_bits |= MACH_MSGH_BITS_CIRCULAR;
		}
		dsc->name = (mach_port_t) object;
		complex = TRUE;
		break;
	    }
	    case MACH_MSG_OOL_DESCRIPTOR: {
		vm_size_t            		length;
		boolean_t            		dealloc;
		vm_offset_t          		addr;
		vm_offset_t          		kaddr;
		mach_msg_ool_descriptor_t 	*dsc;
		
		dsc = &sstart->out_of_line;
		dealloc = dsc->deallocate;
		addr = (vm_offset_t) dsc->address;
		
		length = dsc->size;
		
		if (length == 0) {
		    dsc->address = 0;
		} else if (use_page_lists) {
		    int	options;

		    assert(rt == FALSE);
		    /*
		     * Use page list copy mechanism if specified. Since the
		     * destination is a kernel port, no RT handling is
		     * necessary. 
		     */
		    if (steal_pages == FALSE) {
			/*
			 * XXX Temporary Hackaround.
			 * XXX Because the same page
			 * XXX might be in more than one
			 * XXX out of line region, steal
			 * XXX (busy) pages from previous
			 * XXX region so that this copyin
			 * XXX won't block (permanently).
			 */
			if (copy != VM_MAP_COPY_NULL)
			    vm_map_copy_steal_pages(copy);
		    }

		    /*
		     *	Set up options for copying in page list.
		     *  If deallocating, steal pages to prevent
		     *  vm code from lazy evaluating deallocation.
		     */
		    options = VM_PROT_READ;
#if	NORMA_IPC
		    if (dealloc) {
			options |= VM_MAP_COPYIN_OPT_SRC_DESTROY |
		    			VM_MAP_COPYIN_OPT_STEAL_PAGES;
			kmsg->ikm_must_wait = FALSE;
		    }
		    else if (steal_pages) {
		    	options |= VM_MAP_COPYIN_OPT_STEAL_PAGES;
			kmsg->ikm_must_wait = FALSE;
		    }
		    else {
		    	kmsg->ikm_must_wait = TRUE;
		    }
#else	/* NORMA_IPC */
		    if (dealloc) {
			options |= VM_MAP_COPYIN_OPT_SRC_DESTROY |
		    			VM_MAP_COPYIN_OPT_STEAL_PAGES;
		    }
		    else if (steal_pages) {
		    	options |= VM_MAP_COPYIN_OPT_STEAL_PAGES;
		    }
#endif	/* NORMA_IPC */

		    if (vm_map_copyin_page_list(map, addr, length, options,
						&copy, FALSE)
			!= KERN_SUCCESS) {

			ipc_kmsg_clean_partial(kmsg, i, paddr, 
						       space_needed, rt);
			return MACH_SEND_INVALID_MEMORY;
		    }

		    dsc->address = (void *) copy;
		    dsc->copy = MACH_MSG_PAGE_LIST_COPY_T;
		} else if (length < MSG_OOL_SIZE_SMALL(rt) &&
		    	   dsc->copy == MACH_MSG_PHYSICAL_COPY) {

		    /*
		     * If the data is 'small' enough, always kalloc space for
		     * it and copy it in.  The data will be copied out
		     * on the  message receive.  This is a performance
		     * optimization that assumes the cost of VM operations
		     * dominates the copyin/copyout overhead for 'small'
		     * regions.
		     * If the kernel is the message target, a consistent data
		     * repesentation is needed for ool data since kernel 
		     * functions may deallocate the ool data.  In this case
		     * a vm_map_copy_t is allocated along with the space for
		     * the data as an optimization. No RT handling is needed.
		     */ 
		    if (is_ipc_kobject(ip_kotype((ipc_port_t)dest))) {
			vm_map_copy_t copy;

			assert(rt == FALSE);
		        copy = (vm_map_copy_t) kalloc(
					sizeof(struct vm_map_copy) + length);
			if (copy == VM_MAP_COPY_NULL) {
			    ipc_kmsg_clean_partial(kmsg, i, paddr, 
							space_needed, rt);
			    return MACH_MSG_VM_KERNEL;
			}
			copy->type = VM_MAP_COPY_KERNEL_BUFFER;
		        if (copyin((const char *) addr, (char *) (copy + 1), 
								length)) {
			    kfree((vm_offset_t) copy,
				  sizeof(struct vm_map_copy) + length);
			    ipc_kmsg_clean_partial(kmsg, i, paddr, 
						 	space_needed, rt);
			    return MACH_SEND_INVALID_MEMORY;
		        }	
		        dsc->address = (void *) copy;
			dsc->copy = MACH_MSG_KALLOC_COPY_T;
			copy->size = length;
			copy->offset = 0;
			copy->cpy_data = (vm_offset_t) (copy + 1);
		    } else {
		        if ((kaddr = ALLOC(length, rt)) == (vm_offset_t) 0) {
			    ipc_kmsg_clean_partial(kmsg, i, paddr, 
							space_needed, rt);
			    return MACH_MSG_VM_KERNEL;
			}

		        if (copyin((const char *) addr, (char *) kaddr, 
								length)) {
			    FREE(kaddr, length, rt);
			    ipc_kmsg_clean_partial(kmsg, i, paddr, 
							space_needed, rt);
			    return MACH_SEND_INVALID_MEMORY;
		        }	
		        dsc->address = (void *) kaddr;
		    }
		    if (dealloc) {
	    	        (void) vm_map_remove(map, trunc_page(addr), 
					     round_page(addr + length),
 					     VM_MAP_REMOVE_WAIT_FOR_KWIRE|
 					     VM_MAP_REMOVE_INTERRUPTIBLE);
		    }
		} else {
		    if ((dsc->copy == MACH_MSG_PHYSICAL_COPY) && !dealloc) {

		        /*
		         * If the request is a physical copy and the source
		         * is not being deallocated, then allocate space
		         * in the kernel's pageable ipc copy map and copy
		         * the data in.  The semantics guarantee that the
			 * data will have been physically copied before
			 * the send operation terminates.  Thus if the data
			 * is not being deallocated, we must be prepared
			 * to page if the region is sufficiently large.
		         */
		     	if (copyin((const char *) addr, (char *) paddr, 
								length)) {
			        ipc_kmsg_clean_partial(kmsg, i, paddr, 
							   space_needed, rt);
			        return MACH_SEND_INVALID_MEMORY;
		        }	

			/*
			 * The kernel ipc copy map is marked no_zero_fill.
			 * If the transfer is not a page multiple, we need
			 * to zero fill the balance.
			 */
			if (!page_aligned(length)) {
				(void) memset((void *) (paddr + length), 0,
					round_page(length) - length);
			}
			if (vm_map_copyin(ipc_kernel_copy_map, paddr, length,
					   TRUE, &copy) != KERN_SUCCESS) {
			    ipc_kmsg_clean_partial(kmsg, i, paddr, 
						       space_needed, rt);
			    return MACH_MSG_VM_KERNEL;
		        }
			paddr += round_page(length);
			space_needed -= round_page(length);
		    } else {

			/*
			 * Make a virtual copy of the of the data if requested
			 * or if a physical copy was requested but the source
			 * is being deallocated.
			 */
			if (vm_map_copyin(map, addr, length,
					   dealloc, &copy) != KERN_SUCCESS) {
			    ipc_kmsg_clean_partial(kmsg, i, paddr, 
						       space_needed, rt);
			    return MACH_SEND_INVALID_MEMORY;
		        }
		    }
		    dsc->address = (void *) copy;
		}
		complex = TRUE;
		break;
	    }
	    case MACH_MSG_OOL_PORTS_DESCRIPTOR: {
		vm_size_t 				length;
		vm_offset_t             		data;
		vm_offset_t             		addr;
		ipc_object_t            		*objects;
		int					j;
		mach_msg_type_name_t    		name;
		mach_msg_ool_ports_descriptor_t 	*dsc;
		
		dsc = &sstart->ool_ports;
		addr = (vm_offset_t) dsc->address;

		/* calculate length of data in bytes, rounding up */
		length = dsc->count * sizeof(mach_port_t);
		
		if (length == 0) {
		    complex = TRUE;
		    dsc->address = (void *) 0;
		    break;
		}

		data = ALLOC(length, rt);

		if (data == 0) {
		    ipc_kmsg_clean_partial(kmsg, i, paddr, space_needed, rt);
		    return MACH_SEND_NO_BUFFER;
		}
		
		if (copyinmap(map, addr, data, length)) {
		    FREE(data, length, rt);
		    ipc_kmsg_clean_partial(kmsg, i, paddr, space_needed, rt);
		    return MACH_SEND_INVALID_MEMORY;
		}

		if (dsc->deallocate) {
			(void) vm_deallocate(map, addr, length);
		}
		
		dsc->address = (void *) data;
		
		/* this is really the type SEND, SEND_ONCE, etc. */
		name = dsc->disposition;
		dsc->disposition = ipc_object_copyin_type(name);
		
		objects = (ipc_object_t *) data;
		
		for ( j = 0; j < dsc->count; j++) {
		    mach_port_t port = (mach_port_t) objects[j];
		    ipc_object_t object;
		    
		    if (!MACH_PORT_VALID(port))
			continue;
		    
		    kr = ipc_object_copyin(space, port, name, &object);

		    if (kr != KERN_SUCCESS) {
			int k;

			for(k = 0; k < j; k++) {
			    object = objects[k];
		    	    if (!MACH_PORT_VALID(port))
			 	continue;
		    	    ipc_object_destroy(object, dsc->disposition);
			}
			FREE(data, length, rt);
			ipc_kmsg_clean_partial(kmsg, i, paddr, 
						   space_needed, rt);
			return MACH_SEND_INVALID_RIGHT;
		    }
		    
		    if ((dsc->disposition == MACH_MSG_TYPE_PORT_RECEIVE) &&
			ipc_port_check_circularity(
						   (ipc_port_t) object,
						   (ipc_port_t) dest))
			kmsg->ikm_header.msgh_bits |= MACH_MSGH_BITS_CIRCULAR;
		    
		    objects[j] = object;
		}
		
		complex = TRUE;
		break;
	    }
	    default: {
		/*
		 * Invalid descriptor
		 */
		ipc_kmsg_clean_partial(kmsg, i, paddr, space_needed, rt);
		return MACH_SEND_INVALID_TYPE;
	    }
	}
	i++ ;
    }
    
    if (!complex)
	kmsg->ikm_header.msgh_bits &= ~MACH_MSGH_BITS_COMPLEX;
    
    return MACH_MSG_SUCCESS;
}


/*
 *	Routine:	ipc_kmsg_copyin
 *	Purpose:
 *		"Copy-in" port rights and out-of-line memory
 *		in the message.
 *
 *		In all failure cases, the message is left holding
 *		no rights or memory.  However, the message buffer
 *		is not deallocated.  If successful, the message
 *		contains a valid destination port.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		MACH_MSG_SUCCESS	Successful copyin.
 *		MACH_SEND_INVALID_HEADER
 *			Illegal value in the message header bits.
 *		MACH_SEND_INVALID_NOTIFY	Bad notify port.
 *		MACH_SEND_INVALID_DEST	Can't copyin destination port.
 *		MACH_SEND_INVALID_REPLY	Can't copyin reply port.
 *		MACH_SEND_INVALID_MEMORY	Can't grab out-of-line memory.
 *		MACH_SEND_INVALID_RIGHT	Can't copyin port right in body.
 *		MACH_SEND_INVALID_TYPE	Bad type specification.
 *		MACH_SEND_MSG_TOO_SMALL	Body is too small for types/data.
 */

mach_msg_return_t
ipc_kmsg_copyin(
	ipc_kmsg_t	kmsg,
	ipc_space_t	space,
	vm_map_t	map,
	mach_port_t	notify,
	boolean_t	rt)
{
    mach_msg_return_t 		mr;
    
    mr = ipc_kmsg_copyin_header(&kmsg->ikm_header, space, notify);
    if (mr != MACH_MSG_SUCCESS)
	return mr;
    
    if ((kmsg->ikm_header.msgh_bits & MACH_MSGH_BITS_COMPLEX) == 0)
	return MACH_MSG_SUCCESS;
    
    return( ipc_kmsg_copyin_body( kmsg, space, map, rt) );
}

/*
 *	Routine:	ipc_kmsg_copyin_from_kernel
 *	Purpose:
 *		"Copy-in" port rights and out-of-line memory
 *		in a message sent from the kernel.
 *
 *		Because the message comes from the kernel,
 *		the implementation assumes there are no errors
 *		or peculiarities in the message.
 *
 *		Returns TRUE if queueing the message
 *		would result in a circularity.
 *	Conditions:
 *		Nothing locked.
 */

void
ipc_kmsg_copyin_from_kernel(
	ipc_kmsg_t	kmsg)
{
	mach_msg_bits_t bits = kmsg->ikm_header.msgh_bits;
	mach_msg_type_name_t rname = MACH_MSGH_BITS_REMOTE(bits);
	mach_msg_type_name_t lname = MACH_MSGH_BITS_LOCAL(bits);
	ipc_object_t remote = (ipc_object_t) kmsg->ikm_header.msgh_remote_port;
	ipc_object_t local = (ipc_object_t) kmsg->ikm_header.msgh_local_port;

	/* translate the destination and reply ports */

	ipc_object_copyin_from_kernel(remote, rname);
	if (IO_VALID(local))
		ipc_object_copyin_from_kernel(local, lname);

	/*
	 *	The common case is a complex message with no reply port,
	 *	because that is what the memory_object interface uses.
	 */

	if (bits == (MACH_MSGH_BITS_COMPLEX |
		     MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0))) {
		bits = (MACH_MSGH_BITS_COMPLEX |
			MACH_MSGH_BITS(MACH_MSG_TYPE_PORT_SEND, 0));

		kmsg->ikm_header.msgh_bits = bits;
	} else {
		bits = (MACH_MSGH_BITS_OTHER(bits) |
			MACH_MSGH_BITS(ipc_object_copyin_type(rname),
				       ipc_object_copyin_type(lname)));

		kmsg->ikm_header.msgh_bits = bits;
		if ((bits & MACH_MSGH_BITS_COMPLEX) == 0)
			return;
	}
    {
    	mach_msg_descriptor_t	*saddr, *eaddr;
    	mach_msg_body_t		*body;

    	body = (mach_msg_body_t *) (&kmsg->ikm_header + 1);
    	saddr = (mach_msg_descriptor_t *) (body + 1);
    	eaddr = (mach_msg_descriptor_t *) saddr + body->msgh_descriptor_count;

    	for ( ; saddr <  eaddr; saddr++) {

	    switch (saddr->type.type) {
	    
	        case MACH_MSG_PORT_DESCRIPTOR: {
		    mach_msg_type_name_t 	name;
		    ipc_object_t 		object;
		    mach_msg_port_descriptor_t 	*dsc;
		
		    dsc = &saddr->port;
		
		    /* this is really the type SEND, SEND_ONCE, etc. */
		    name = dsc->disposition;
		    object = (ipc_object_t) dsc->name;
		    dsc->disposition = ipc_object_copyin_type(name);
		
		    if (!IO_VALID(object)) {
		        break;
		    }

		    ipc_object_copyin_from_kernel(object, name);
		    
		    if ((dsc->disposition == MACH_MSG_TYPE_PORT_RECEIVE) &&
		        ipc_port_check_circularity((ipc_port_t) object, 
						(ipc_port_t) remote)) {
		        kmsg->ikm_header.msgh_bits |= MACH_MSGH_BITS_CIRCULAR;
		    }
		    break;
	        }
	        case MACH_MSG_OOL_DESCRIPTOR: {
		    /*
		     * The sender should supply ready-made memory, i.e.
		     * a vm_map_copy_t, so we don't need to do anything.
		     */
		    break;
	        }
	        case MACH_MSG_OOL_PORTS_DESCRIPTOR: {
		    ipc_object_t            		*objects;
		    int					j;
		    mach_msg_type_name_t    		name;
		    mach_msg_ool_ports_descriptor_t 	*dsc;
		
		    dsc = &saddr->ool_ports;

		    /* this is really the type SEND, SEND_ONCE, etc. */
		    name = dsc->disposition;
		    dsc->disposition = ipc_object_copyin_type(name);
	    	
		    objects = (ipc_object_t *) dsc->address;
	    	
		    for ( j = 0; j < dsc->count; j++) {
		        ipc_object_t object = objects[j];
		        
		        if (!IO_VALID(object))
			    continue;
		        
		        ipc_object_copyin_from_kernel(object, name);
    
		        if ((dsc->disposition == MACH_MSG_TYPE_PORT_RECEIVE) &&
			    ipc_port_check_circularity(
						       (ipc_port_t) object,
						       (ipc_port_t) remote))
			    kmsg->ikm_header.msgh_bits |= MACH_MSGH_BITS_CIRCULAR;
		    }
		    break;
	        }
	        default: {
#if	MACH_ASSERT
		    panic("ipc_kmsg_copyin_from_kernel:  bad descriptor");
#endif	/* MACH_ASSERT */
		}
	    }
	}
    }
}

/*
 *	Routine:	ipc_kmsg_copyout_header
 *	Purpose:
 *		"Copy-out" port rights in the header of a message.
 *		Operates atomically; if it doesn't succeed the
 *		message header and the space are left untouched.
 *		If it does succeed the remote/local port fields
 *		contain port names instead of object pointers,
 *		and the bits field is updated.
 *
 *		The notify argument implements the MACH_RCV_NOTIFY option.
 *		If it is not MACH_PORT_NULL, it should name a receive right.
 *		If the process of receiving the reply port creates a
 *		new right in the receiving task, then the new right is
 *		automatically registered for a dead-name notification,
 *		with the notify port supplying the send-once right.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		MACH_MSG_SUCCESS	Copied out port rights.
 *		MACH_RCV_INVALID_NOTIFY	
 *			Notify is non-null and doesn't name a receive right.
 *			(Either KERN_INVALID_NAME or KERN_INVALID_RIGHT.)
 *		MACH_RCV_HEADER_ERROR|MACH_MSG_IPC_SPACE
 *			The space is dead.
 *		MACH_RCV_HEADER_ERROR|MACH_MSG_IPC_SPACE
 *			No room in space for another name.
 *		MACH_RCV_HEADER_ERROR|MACH_MSG_IPC_KERNEL
 *			Couldn't allocate memory for the reply port.
 *		MACH_RCV_HEADER_ERROR|MACH_MSG_IPC_KERNEL
 *			Couldn't allocate memory for the dead-name request.
 */

mach_msg_return_t
ipc_kmsg_copyout_header(
	mach_msg_header_t	*msg,
	ipc_space_t		space,
	mach_port_t		notify)
{
	mach_msg_bits_t mbits = msg->msgh_bits;
	ipc_port_t dest = (ipc_port_t) msg->msgh_remote_port;

	assert(IP_VALID(dest));

    {
	mach_msg_type_name_t dest_type = MACH_MSGH_BITS_REMOTE(mbits);
	mach_msg_type_name_t reply_type = MACH_MSGH_BITS_LOCAL(mbits);
	ipc_port_t reply = (ipc_port_t) msg->msgh_local_port;
	mach_port_t dest_name, reply_name;

	if (IP_VALID(reply)) {
		ipc_port_t notify_port;
		ipc_entry_t entry;
		kern_return_t kr;

		/*
		 *	Handling notify (for MACH_RCV_NOTIFY) is tricky.
		 *	The problem is atomically making a send-once right
		 *	from the notify port and installing it for a
		 *	dead-name request in the new entry, because this
		 *	requires two port locks (on the notify port and
		 *	the reply port).  However, we can safely make
		 *	and consume send-once rights for the notify port
		 *	as long as we hold the space locked.  This isn't
		 *	an atomicity problem, because the only way
		 *	to detect that a send-once right has been created
		 *	and then consumed if it wasn't needed is by getting
		 *	at the receive right to look at ip_sorights, and
		 *	because the space is write-locked status calls can't
		 *	lookup the notify port receive right.  When we make
		 *	the send-once right, we lock the notify port,
		 *	so any status calls in progress will be done.
		 */

		is_write_lock(space);

		for (;;) {
			ipc_port_request_index_t request;

			if (!space->is_active) {
				is_write_unlock(space);
				return (MACH_RCV_HEADER_ERROR|
					MACH_MSG_IPC_SPACE);
			}

			if (notify != MACH_PORT_NULL) {
				notify_port = ipc_port_lookup_notify(space,
								     notify);
				if (notify_port == IP_NULL) {
					is_write_unlock(space);
					return MACH_RCV_INVALID_NOTIFY;
				}
			} else
				notify_port = IP_NULL;

			if ((reply_type != MACH_MSG_TYPE_PORT_SEND_ONCE) &&
			    ipc_right_reverse(space, (ipc_object_t) reply,
					      &reply_name, &entry)) {
				/* reply port is locked and active */

				/*
				 *	We don't need the notify_port
				 *	send-once right, but we can't release
				 *	it here because reply port is locked.
				 *	Wait until after the copyout to
				 *	release the notify port right.
				 */

				assert(entry->ie_bits &
						MACH_PORT_TYPE_SEND_RECEIVE);
				break;
			}

			ip_lock(reply);
			if (!ip_active(reply)) {
				ip_release(reply);
				ip_check_unlock(reply);

				if (notify_port != IP_NULL)
					ipc_port_release_sonce(notify_port);

				ip_lock(dest);
				is_write_unlock(space);

				reply = IP_DEAD;
				reply_name = MACH_PORT_DEAD;
				goto copyout_dest;
			}

			reply_name = (mach_port_t)reply;
			kr = ipc_entry_get(space,
				reply_type == MACH_MSG_TYPE_PORT_SEND_ONCE,
				&reply_name, &entry);
			if (kr != KERN_SUCCESS) {
				ip_unlock(reply);

				if (notify_port != IP_NULL)
					ipc_port_release_sonce(notify_port);

				/* space is locked */
				kr = ipc_entry_grow_table(space,
							  ITS_SIZE_NONE);
				if (kr != KERN_SUCCESS) {
					/* space is unlocked */

					if (kr == KERN_RESOURCE_SHORTAGE)
						return (MACH_RCV_HEADER_ERROR|
							MACH_MSG_IPC_KERNEL);
					else
						return (MACH_RCV_HEADER_ERROR|
							MACH_MSG_IPC_SPACE);
				}
				/* space is locked again; start over */

				continue;
			}

			assert(IE_BITS_TYPE(entry->ie_bits)
						== MACH_PORT_TYPE_NONE);
			assert(entry->ie_object == IO_NULL);

			if (notify_port == IP_NULL) {
				/* not making a dead-name request */

				entry->ie_object = (ipc_object_t) reply;
				break;
			}

			kr = ipc_port_dnrequest(reply, reply_name,
						notify_port, &request);
			if (kr != KERN_SUCCESS) {
				ip_unlock(reply);

				ipc_port_release_sonce(notify_port);

				ipc_entry_dealloc(space, reply_name, entry);
				is_write_unlock(space);

				ip_lock(reply);
				if (!ip_active(reply)) {
					/* will fail next time around loop */

					ip_unlock(reply);
					is_write_lock(space);
					continue;
				}

				kr = ipc_port_dngrow(reply, ITS_SIZE_NONE);
				/* port is unlocked */
				if (kr != KERN_SUCCESS)
					return (MACH_RCV_HEADER_ERROR|
						MACH_MSG_IPC_KERNEL);

				is_write_lock(space);
				continue;
			}

			notify_port = IP_NULL; /* don't release right below */

			entry->ie_object = (ipc_object_t) reply;
			entry->ie_request = request;
			break;
		}

		/* space and reply port are locked and active */

		ip_reference(reply);	/* hold onto the reply port */

		kr = ipc_right_copyout(space, reply_name, entry,
				       reply_type, TRUE, (ipc_object_t) reply);
		/* reply port is unlocked */
		assert(kr == KERN_SUCCESS);

		if (notify_port != IP_NULL)
			ipc_port_release_sonce(notify_port);

		ip_lock(dest);
		is_write_unlock(space);
	} else {
		/*
		 *	No reply port!  This is an easy case.
		 *	We only need to have the space locked
		 *	when checking notify and when locking
		 *	the destination (to ensure atomicity).
		 */

		is_read_lock(space);
		if (!space->is_active) {
			is_read_unlock(space);
			return MACH_RCV_HEADER_ERROR|MACH_MSG_IPC_SPACE;
		}

		if (notify != MACH_PORT_NULL) {
			ipc_entry_t entry;

			/* must check notify even though it won't be used */

			if (((entry = ipc_entry_lookup(space, notify))
								== IE_NULL) ||
			    ((entry->ie_bits & MACH_PORT_TYPE_RECEIVE) == 0)) {
				is_read_unlock(space);
				return MACH_RCV_INVALID_NOTIFY;
			}
		}

		ip_lock(dest);
		is_read_unlock(space);

		reply_name = (mach_port_t) reply;
	}

	/*
	 *	At this point, the space is unlocked and the destination
	 *	port is locked.  (Lock taken while space was locked.)
	 *	reply_name is taken care of; we still need dest_name.
	 *	We still hold a ref for reply (if it is valid).
	 *
	 *	If the space holds receive rights for the destination,
	 *	we return its name for the right.  Otherwise the task
	 *	managed to destroy or give away the receive right between
	 *	receiving the message and this copyout.  If the destination
	 *	is dead, return MACH_PORT_DEAD, and if the receive right
	 *	exists somewhere else (another space, in transit)
	 *	return MACH_PORT_NULL.
	 *
	 *	Making this copyout operation atomic with the previous
	 *	copyout of the reply port is a bit tricky.  If there was
	 *	no real reply port (it wasn't IP_VALID) then this isn't
	 *	an issue.  If the reply port was dead at copyout time,
	 *	then we are OK, because if dest is dead we serialize
	 *	after the death of both ports and if dest is alive
	 *	we serialize after reply died but before dest's (later) death.
	 *	So assume reply was alive when we copied it out.  If dest
	 *	is alive, then we are OK because we serialize before
	 *	the ports' deaths.  So assume dest is dead when we look at it.
	 *	If reply dies/died after dest, then we are OK because
	 *	we serialize after dest died but before reply dies.
	 *	So the hard case is when reply is alive at copyout,
	 *	dest is dead at copyout, and reply died before dest died.
	 *	In this case pretend that dest is still alive, so
	 *	we serialize while both ports are alive.
	 *
	 *	Because the space lock is held across the copyout of reply
	 *	and locking dest, the receive right for dest can't move
	 *	in or out of the space while the copyouts happen, so
	 *	that isn't an atomicity problem.  In the last hard case
	 *	above, this implies that when dest is dead that the
	 *	space couldn't have had receive rights for dest at
	 *	the time reply was copied-out, so when we pretend
	 *	that dest is still alive, we can return MACH_PORT_NULL.
	 *
	 *	If dest == reply, then we have to make it look like
	 *	either both copyouts happened before the port died,
	 *	or both happened after the port died.  This special
	 *	case works naturally if the timestamp comparison
	 *	is done correctly.
	 */

    copyout_dest:

	if (ip_active(dest)) {
		ipc_object_copyout_dest(space, (ipc_object_t) dest,
					dest_type, &dest_name);
		/* dest is unlocked */
	} else {
		ipc_port_timestamp_t timestamp;

		timestamp = dest->ip_timestamp;
		ip_release(dest);
		ip_check_unlock(dest);

		if (IP_VALID(reply)) {
			ip_lock(reply);
			if (ip_active(reply) ||
			    IP_TIMESTAMP_ORDER(timestamp,
					       reply->ip_timestamp))
				dest_name = MACH_PORT_DEAD;
			else
				dest_name = MACH_PORT_NULL;
			ip_unlock(reply);
		} else
			dest_name = MACH_PORT_DEAD;
	}

	if (IP_VALID(reply))
		ipc_port_release(reply);

	msg->msgh_bits = (MACH_MSGH_BITS_OTHER(mbits) |
			  MACH_MSGH_BITS(reply_type, dest_type));
	msg->msgh_local_port = dest_name;
	msg->msgh_remote_port = reply_name;
    }

	return MACH_MSG_SUCCESS;
}

/*
 *	Routine:	ipc_kmsg_copyout_object
 *	Purpose:
 *		Copy-out a port right.  Always returns a name,
 *		even for unsuccessful return codes.  Always
 *		consumes the supplied object.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		MACH_MSG_SUCCESS	The space acquired the right
 *			(name is valid) or the object is dead (MACH_PORT_DEAD).
 *		MACH_MSG_IPC_SPACE	No room in space for the right,
 *			or the space is dead.  (Name is MACH_PORT_NULL.)
 *		MACH_MSG_IPC_KERNEL	Kernel resource shortage.
 *			(Name is MACH_PORT_NULL.)
 */

mach_msg_return_t
ipc_kmsg_copyout_object(
	ipc_space_t		space,
	ipc_object_t		object,
	mach_msg_type_name_t	msgt_name,
	mach_port_t		*namep)
{
	kern_return_t kr;

	if (!IO_VALID(object)) {
		*namep = (mach_port_t) object;
		return MACH_MSG_SUCCESS;
	}

	kr = ipc_object_copyout(space, object, msgt_name, TRUE, namep);
	if (kr != KERN_SUCCESS) {
		ipc_object_destroy(object, msgt_name);

		if (kr == KERN_INVALID_CAPABILITY)
			*namep = MACH_PORT_DEAD;
		else {
			*namep = MACH_PORT_NULL;

			if (kr == KERN_RESOURCE_SHORTAGE)
				return MACH_MSG_IPC_KERNEL;
			else
				return MACH_MSG_IPC_SPACE;
		}
	}

	return MACH_MSG_SUCCESS;
}

#define SKIP_PORT_DESCRIPTORS(s, e)					\
MACRO_BEGIN								\
	if ((s) != MACH_MSG_DESCRIPTOR_NULL) {				\
		while ((s) < (e)) {					\
			if ((s)->type.type != MACH_MSG_PORT_DESCRIPTOR)	\
				break;					\
			(s)++;						\
		}							\
		if ((s) >= (e))						\
			(s) = MACH_MSG_DESCRIPTOR_NULL;			\
	}								\
MACRO_END

#define INCREMENT_SCATTER(s)						\
MACRO_BEGIN								\
	if ((s) != MACH_MSG_DESCRIPTOR_NULL) {				\
		(s)++;							\
	}								\
MACRO_END

/*
 *	Routine:	ipc_kmsg_copyout_body
 *	Purpose:
 *		"Copy-out" port rights and out-of-line memory
 *		in the body of a message.
 *
 *		The error codes are a combination of special bits.
 *		The copyout proceeds despite errors.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		MACH_MSG_SUCCESS	Successfull copyout.
 *		MACH_MSG_IPC_SPACE	No room for port right in name space.
 *		MACH_MSG_VM_SPACE	No room for memory in address space.
 *		MACH_MSG_IPC_KERNEL	Resource shortage handling port right.
 *		MACH_MSG_VM_KERNEL	Resource shortage handling memory.
 */

mach_msg_return_t
ipc_kmsg_copyout_body(
    	ipc_kmsg_t		kmsg,
	ipc_space_t		space,
	vm_map_t		map,
	mach_msg_body_t		*slist,
	boolean_t		rt)
{
    mach_msg_body_t 		*body;
    mach_msg_descriptor_t 	*saddr, *eaddr;
    mach_msg_return_t 		mr = MACH_MSG_SUCCESS;
    kern_return_t 		kr;
    int				i;
    int				j;
    vm_offset_t         	addr;
    vm_offset_t         	data;
    mach_msg_ool_descriptor_t	*ddsc;
    mach_msg_descriptor_t 	*sstart, *send;

    body = (mach_msg_body_t *) (&kmsg->ikm_header + 1);
    saddr = (mach_msg_descriptor_t *) (body + 1);
    eaddr = saddr + body->msgh_descriptor_count;

    /*
     * Do scatter list setup
     */
    if (slist != MACH_MSG_BODY_NULL) {
	sstart = (mach_msg_descriptor_t *) (slist + 1);
	send = sstart + slist->msgh_descriptor_count;
    }
    else {
	sstart = MACH_MSG_DESCRIPTOR_NULL;
    }

    for ( ; saddr < eaddr; saddr++ ) {
	
	switch (saddr->type.type) {
	    
	    case MACH_MSG_PORT_DESCRIPTOR: {
		mach_msg_port_descriptor_t *dsc;
		
		/* 
		 * Copyout port right carried in the message 
		 */
		dsc = &saddr->port;
		mr |= ipc_kmsg_copyout_object(space, 
					      (ipc_object_t) dsc->name, 
					      dsc->disposition, 
					      (mach_port_t *) &dsc->name);

		break;
	    }
	    case MACH_MSG_OOL_DESCRIPTOR : {
		vm_offset_t			rcv_addr;
		vm_offset_t			snd_addr;
		mach_msg_ool_descriptor_t	*dsc;
		mach_msg_copy_options_t		copy_option;

		SKIP_PORT_DESCRIPTORS(sstart, send);

		dsc = &saddr->out_of_line;

		assert(dsc->copy != MACH_MSG_KALLOC_COPY_T);
#if	!NORMA_IPC
		assert(dsc->copy != MACH_MSG_PAGE_LIST_COPY_T);
#endif	/* !NORMA_IPC */

		copy_option = dsc->copy;

		if ((snd_addr = (vm_offset_t) dsc->address) != 0) {
		    if (sstart != MACH_MSG_DESCRIPTOR_NULL &&
			sstart->out_of_line.copy == MACH_MSG_OVERWRITE) {

			/*
			 * There is an overwrite descriptor specified in the
			 * scatter list for this ool data.  The descriptor
			 * has already been verified
			 */
			rcv_addr = (vm_offset_t) sstart->out_of_line.address;
			dsc->copy = MACH_MSG_OVERWRITE;
		    } else {
			dsc->copy = MACH_MSG_ALLOCATE;
		    }
			
		    if (copy_option == MACH_MSG_PHYSICAL_COPY &&
			    dsc->size < MSG_OOL_SIZE_SMALL(rt)) {

			/* 
			 * Sufficiently 'small' data was copied into a kalloc'ed
			 * buffer copy was requested.  Just copy it out and 
			 * free the buffer.
			 */
			if (dsc->copy == MACH_MSG_ALLOCATE) {

			    /*
			     * If there is no overwrite region, allocate
			     * space in receiver's address space for the
			     * data
			     */
			    if ((kr = vm_allocate(map, &rcv_addr, dsc->size, 
					TRUE)) != KERN_SUCCESS) {
		    	        if (kr == KERN_RESOURCE_SHORTAGE)
				        mr |= MACH_MSG_VM_KERNEL;
		    	        else
				        mr |= MACH_MSG_VM_SPACE;
			        FREE(snd_addr, dsc->size, rt);
			        dsc->address = (void *) 0;
				INCREMENT_SCATTER(sstart);
			        break;
			    }
			}
			(void) copyoutmap(map, snd_addr, rcv_addr, dsc->size);
			FREE(snd_addr, dsc->size, rt);
		    } else {

			/*
			 * Whether the data was virtually or physically
			 * copied we have a vm_map_copy_t for it.
			 * If there's an overwrite region specified
			 * overwrite it, otherwise do a virtual copy out.
			 */
			if (dsc->copy == MACH_MSG_OVERWRITE) {
			    kr = vm_map_copy_overwrite(map, rcv_addr,
					(vm_map_copy_t) dsc->address, TRUE);
			} else {
			    kr = vm_map_copyout(map, &rcv_addr, 
						(vm_map_copy_t) dsc->address);
			}	
			if (kr != KERN_SUCCESS) {
		    	    if (kr == KERN_RESOURCE_SHORTAGE)
				mr |= MACH_MSG_VM_KERNEL;
		    	    else
				mr |= MACH_MSG_VM_SPACE;
		    	    vm_map_copy_discard((vm_map_copy_t) dsc->address);
			    dsc->address = 0;
			    INCREMENT_SCATTER(sstart);
			    break;
			}
		    }
		    dsc->address = (void *) rcv_addr;
		}
		INCREMENT_SCATTER(sstart);
		break;
	    }
	    case MACH_MSG_OOL_PORTS_DESCRIPTOR : {
		vm_map_copy_t         		copy;
		vm_offset_t            		addr;
		mach_port_t            		*objects;
		mach_msg_type_number_t 		j;
    		vm_size_t           		length;
		mach_msg_ool_ports_descriptor_t	*dsc;

		SKIP_PORT_DESCRIPTORS(sstart, send);

		dsc = &saddr->ool_ports;

		length = dsc->count * sizeof(mach_port_t);

		if (length != 0) {
		    if (sstart != MACH_MSG_DESCRIPTOR_NULL &&
			sstart->ool_ports.copy == MACH_MSG_OVERWRITE) {

			/*
			 * There is an overwrite descriptor specified in the
			 * scatter list for this ool data.  The descriptor
			 * has already been verified
			 */
			addr = (vm_offset_t) sstart->out_of_line.address;
			dsc->copy = MACH_MSG_OVERWRITE;
		    } 
		    else {

		   	/*
			 * Dynamically allocate the region
			 */
			dsc->copy = MACH_MSG_ALLOCATE;
		    	if ((kr = vm_allocate(map, &addr, length, TRUE)) !=
		    						KERN_SUCCESS) {
			    ipc_kmsg_clean_body(kmsg,
						body->msgh_descriptor_count,
						rt);
			    dsc->address = 0;

			    if (kr == KERN_RESOURCE_SHORTAGE){
			    	mr |= MACH_MSG_VM_KERNEL;
			    } else {
			    	mr |= MACH_MSG_VM_SPACE;
			    }
			    INCREMENT_SCATTER(sstart);
			    break;
		    	}
		    }
		} else {
		    assert(dsc->address == 0);
		    INCREMENT_SCATTER(sstart);
		    break;
		}

		
		objects = (mach_port_t *) dsc->address ;
		
		/* copyout port rights carried in the message */
		
		for ( j = 0; j < dsc->count ; j++) {
		    ipc_object_t object =
			(ipc_object_t) objects[j];
		    
		    mr |= ipc_kmsg_copyout_object(space, object,
					dsc->disposition, &objects[j]);
		}

		/* copyout to memory allocated above */
		
		data = (vm_offset_t) dsc->address;
		(void) copyoutmap(map, data, addr, length);
		FREE(data, length, rt);
		
		dsc->address = (void *) addr;
		INCREMENT_SCATTER(sstart);
		break;
	    }
	    default : {
		panic("untyped IPC copyout body: invalid message descriptor");
	    }
	}
    }
    return mr;
}

/*
 *	Routine:	ipc_kmsg_copyout
 *	Purpose:
 *		"Copy-out" port rights and out-of-line memory
 *		in the message.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		MACH_MSG_SUCCESS	Copied out all rights and memory.
 *		MACH_RCV_INVALID_NOTIFY	Bad notify port.
 *			Rights and memory in the message are intact.
 *		MACH_RCV_HEADER_ERROR + special bits
 *			Rights and memory in the message are intact.
 *		MACH_RCV_BODY_ERROR + special bits
 *			The message header was successfully copied out.
 *			As much of the body was handled as possible.
 */

mach_msg_return_t
ipc_kmsg_copyout(
	ipc_kmsg_t		kmsg,
	ipc_space_t		space,
	vm_map_t		map,
	mach_port_t		notify,
	mach_msg_body_t		*slist,
	boolean_t		rt)
{
	mach_msg_bits_t mbits = kmsg->ikm_header.msgh_bits;
	mach_msg_return_t mr;

	mr = ipc_kmsg_copyout_header(&kmsg->ikm_header, space, notify);
	if (mr != MACH_MSG_SUCCESS)
		return mr;

	if (mbits & MACH_MSGH_BITS_COMPLEX) {
		mr = ipc_kmsg_copyout_body(kmsg, space, map, slist, rt);

		if (mr != MACH_MSG_SUCCESS)
			mr |= MACH_RCV_BODY_ERROR;
	}

	return mr;
}

/*
 *	Routine:	ipc_kmsg_copyout_pseudo
 *	Purpose:
 *		Does a pseudo-copyout of the message.
 *		This is like a regular copyout, except
 *		that the ports in the header are handled
 *		as if they are in the body.  They aren't reversed.
 *
 *		The error codes are a combination of special bits.
 *		The copyout proceeds despite errors.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		MACH_MSG_SUCCESS	Successful copyout.
 *		MACH_MSG_IPC_SPACE	No room for port right in name space.
 *		MACH_MSG_VM_SPACE	No room for memory in address space.
 *		MACH_MSG_IPC_KERNEL	Resource shortage handling port right.
 *		MACH_MSG_VM_KERNEL	Resource shortage handling memory.
 */

mach_msg_return_t
ipc_kmsg_copyout_pseudo(
	ipc_kmsg_t		kmsg,
	ipc_space_t		space,
	vm_map_t		map,
	mach_msg_body_t		*slist,
	boolean_t		rt)
{
	mach_msg_bits_t mbits = kmsg->ikm_header.msgh_bits;
	ipc_object_t dest = (ipc_object_t) kmsg->ikm_header.msgh_remote_port;
	ipc_object_t reply = (ipc_object_t) kmsg->ikm_header.msgh_local_port;
	mach_msg_type_name_t dest_type = MACH_MSGH_BITS_REMOTE(mbits);
	mach_msg_type_name_t reply_type = MACH_MSGH_BITS_LOCAL(mbits);
	mach_port_t dest_name, reply_name;
	mach_msg_return_t mr;

	assert(IO_VALID(dest));

	mr = (ipc_kmsg_copyout_object(space, dest, dest_type, &dest_name) |
	      ipc_kmsg_copyout_object(space, reply, reply_type, &reply_name));

	kmsg->ikm_header.msgh_bits = mbits &~ MACH_MSGH_BITS_CIRCULAR;
	kmsg->ikm_header.msgh_remote_port = dest_name;
	kmsg->ikm_header.msgh_local_port = reply_name;

	if (mbits & MACH_MSGH_BITS_COMPLEX) {
		mr |= ipc_kmsg_copyout_body(kmsg, space, map, slist, rt);
	}

	return mr;
}

/*
 *	Routine:	ipc_kmsg_copyout_dest
 *	Purpose:
 *		Copies out the destination port in the message.
 *		Destroys all other rights and memory in the message.
 *	Conditions:
 *		Nothing locked.
 */

void
ipc_kmsg_copyout_dest(
	ipc_kmsg_t	kmsg,
	ipc_space_t	space,
	boolean_t	rt)
{
	mach_msg_bits_t mbits = kmsg->ikm_header.msgh_bits;
	ipc_object_t dest = (ipc_object_t) kmsg->ikm_header.msgh_remote_port;
	ipc_object_t reply = (ipc_object_t) kmsg->ikm_header.msgh_local_port;
	mach_msg_type_name_t dest_type = MACH_MSGH_BITS_REMOTE(mbits);
	mach_msg_type_name_t reply_type = MACH_MSGH_BITS_LOCAL(mbits);
	mach_port_t dest_name, reply_name;

	assert(IO_VALID(dest));

	io_lock(dest);
	if (io_active(dest)) {
		ipc_object_copyout_dest(space, dest, dest_type, &dest_name);
		/* dest is unlocked */
	} else {
		io_release(dest);
		io_check_unlock(dest);
		dest_name = MACH_PORT_DEAD;
	}

	if (IO_VALID(reply)) {
		ipc_object_destroy(reply, reply_type);
		reply_name = MACH_PORT_NULL;
	} else
		reply_name = (mach_port_t) reply;

	kmsg->ikm_header.msgh_bits = (MACH_MSGH_BITS_OTHER(mbits) |
				      MACH_MSGH_BITS(reply_type, dest_type));
	kmsg->ikm_header.msgh_local_port = dest_name;
	kmsg->ikm_header.msgh_remote_port = reply_name;

	if (mbits & MACH_MSGH_BITS_COMPLEX) {
		mach_msg_body_t *body;

		body = (mach_msg_body_t *) (&kmsg->ikm_header + 1);
		ipc_kmsg_clean_body(kmsg, body->msgh_descriptor_count, rt);
	}
}

/*
 *	Routine:	ipc_kmsg_check_scatter
 *	Purpose:
 *		Checks scatter and gather lists for consistency.
 *		
 *	Algorithm:
 *		The gather is assumed valid since it has been copied in.
 *		The scatter list has only been range checked.
 *		Gather list descriptors are sequentially paired with scatter 
 *		list descriptors, with port descriptors in either list ignored.
 *		Descriptors are consistent if the type fileds match and size
 *		of the scatter descriptor is less than or equal to the
 *		size of the gather descriptor.  A MACH_MSG_ALLOCATE copy 
 *		strategy in a scatter descriptor matches any size in the 
 *		corresponding gather descriptor assuming they are the same type.
 *		Either list may be larger than the other.  During the
 *		subsequent copy out, excess scatter descriptors are ignored
 *		and excess gather descriptors default to dynamic allocation.
 *		
 *		In the case of a size error, a new scatter list is formed
 *		from the gather list copying only the size and type fields.
 *		
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		MACH_MSG_SUCCESS		Lists are consistent
 *		MACH_RCV_INVALID_TYPE		Scatter type does not match
 *						gather type
 *		MACH_RCV_SCATTER_SMALL		Scatter size less than gather
 *						size
 */

mach_msg_return_t
ipc_kmsg_check_scatter(
	ipc_kmsg_t		kmsg,
	mach_msg_option_t	option,
	mach_msg_body_t		**slistp,
	mach_msg_size_t		*sizep)
{
    	mach_msg_body_t		*body;
	mach_msg_descriptor_t 	*gstart, *gend;
	mach_msg_descriptor_t 	*sstart, *send;
	boolean_t 		size_error = FALSE;
	mach_msg_return_t 	mr = MACH_MSG_SUCCESS;

	assert(*slistp != MACH_MSG_BODY_NULL);
	assert(*sizep != 0);

    	body = (mach_msg_body_t *) (&kmsg->ikm_header + 1);
    	gstart = (mach_msg_descriptor_t *) (body + 1);
    	gend = gstart + body->msgh_descriptor_count;

	sstart = (mach_msg_descriptor_t *) (*slistp + 1);
	send = sstart + (*slistp)->msgh_descriptor_count;

	while (gstart < gend) {

	    /*
	     * Skip port descriptors in gather list. 
	     */
	    if (gstart->type.type != MACH_MSG_PORT_DESCRIPTOR) {

		/*
	 	 * A scatter list with a 0 descriptor count is treated as an
	 	 * automatic size mismatch.
	 	 */
		 if ((*slistp)->msgh_descriptor_count == 0) {
		    if ((option & MACH_RCV_LARGE) == 0)
			return(MACH_RCV_SCATTER_SMALL);
	    	    size_error = TRUE;
		    break;
		 }

		/*
		 * Skip port descriptors in  scatter list.
		 */
		while (sstart < send) {
		    if (sstart->type.type != MACH_MSG_PORT_DESCRIPTOR)
			break;
		    sstart++;
		}

		/*
		 * No more scatter descriptors, we're done
		 */
		if (sstart >= send) {
		    break;
		}

		/*
		 * Check type, copy and size fields
		 */
		if (gstart->type.type == MACH_MSG_OOL_DESCRIPTOR) {
		    if (sstart->type.type != MACH_MSG_OOL_DESCRIPTOR) {
			return(MACH_RCV_INVALID_TYPE);
		    }
		    if (sstart->out_of_line.copy == MACH_MSG_OVERWRITE && 
			gstart->out_of_line.size > sstart->out_of_line.size) {
			if ((option & MACH_RCV_LARGE) == 0)
				return(MACH_RCV_SCATTER_SMALL);
			size_error = TRUE;
		    }
		}
		else {
		    if (sstart->type.type != MACH_MSG_OOL_PORTS_DESCRIPTOR) {
			return(MACH_RCV_INVALID_TYPE);
		    }
		    if (sstart->ool_ports.copy == MACH_MSG_OVERWRITE &&
			gstart->ool_ports.count > sstart->ool_ports.count) {
			if ((option & MACH_RCV_LARGE) == 0)
				return(MACH_RCV_SCATTER_SMALL);
			size_error = TRUE;
		    }
		}
	        sstart++;
	    }
	    gstart++;
	}

	/*
   	 * If there is a size error, form a new scatter list from the
	 * gather list.
	 */
	if (size_error) {

	    /*
	     * First toss the old list and allocate a new one
	     */
	    kfree((vm_offset_t)(*slistp), *sizep);
	    *sizep = (unsigned) gend - (unsigned) body;
	    *slistp = (mach_msg_body_t *) kalloc(*sizep);
	    if (*slistp == MACH_MSG_BODY_NULL) {
		 return(MACH_MSG_VM_KERNEL|MACH_RCV_BODY_ERROR);
	    }
	    (void) memset((void *) *slistp, 0, *sizep);

	    /*
	     * Now fill in the  descriptor count and the type and size 
	     * fields for each descriptor
	     */
	    (*slistp)->msgh_descriptor_count = body->msgh_descriptor_count;
    	    gstart = (mach_msg_descriptor_t *) (body + 1);
    	    gend = gstart + body->msgh_descriptor_count;
	    sstart = (mach_msg_descriptor_t *) (*slistp + 1);

	    for (; gstart < gend; gstart++, sstart++) {
		sstart->type.type = gstart->type.type;
		switch (gstart->type.type) {
		    case MACH_MSG_PORT_DESCRIPTOR:
			break;
		    case MACH_MSG_OOL_DESCRIPTOR:
			sstart->out_of_line.size = gstart->out_of_line.size;
			break;
		    case MACH_MSG_OOL_PORTS_DESCRIPTOR:
			sstart->ool_ports.count = gstart->ool_ports.count;
			break;
		}
	    }
	    mr = MACH_RCV_SCATTER_SMALL;
	}

	return(mr);
}

/*
 *	Routine:	ikm_cache_get
 *	Purpose:	Attempt to allocate from the per-cpu IKM cache.
 *	Conditions:	Nothing locked.
 *
 *	If the IKM cache for the current cpu is not empty, this routine
 *	will return the address of the block, nulling out the cache.
 *	TRUE is returned for success, FALSE for failure.
 *	Preemption must be disabled while in here.
 */

boolean_t
ikm_cache_get(
	ipc_kmsg_t	* kmsg)
{
	int		cpu = cpu_number ();
	boolean_t	result = FALSE;

	disable_preemption();

	*kmsg = ipc_kmsg_cache[cpu];
	if (*kmsg) {
		result = TRUE;
		ipc_kmsg_cache[cpu] = NULL;
	}

	enable_preemption();

	return (result);
}

/*
 *	Routine:	ikm_cache_put
 *	Purpose:	Attempt to free a block to the per-cpu IKM cache.
 *	Conditions:	Nothing locked.
 *
 *	If the IKM cache for the current cpu is empty, this routine
 *	will store its argument into the cache.
 *	TRUE is returned for success, FALSE for failure.
 *	Preemption must be disabled while in here.
 */

boolean_t
ikm_cache_put(
	ipc_kmsg_t	kmsg)
{
	int		cpu = cpu_number ();
	boolean_t	result = FALSE;

	disable_preemption();

	if (ipc_kmsg_cache[cpu] == NULL) {
		result = TRUE;
		ipc_kmsg_cache[cpu] = kmsg;
	}

	enable_preemption();

	return (result);
}

/*
 *	Routine:	ipc_kmsg_copyout_to_kernel
 *	Purpose:
 *		Copies out the destination and reply ports in the message.
 *		Leaves all other rights and memory in the message alone.
 *	Conditions:
 *		Nothing locked.
 *
 *	Derived from ipc_kmsg_copyout_dest.
 *	Use by mach_msg_rpc_from_kernel (which used to use copyout_dest).
 *	We really do want to save rights and memory.
 */

void
ipc_kmsg_copyout_to_kernel(
	ipc_kmsg_t	kmsg,
	ipc_space_t	space)
{
	mach_msg_bits_t mbits = kmsg->ikm_header.msgh_bits;
	ipc_object_t dest = (ipc_object_t) kmsg->ikm_header.msgh_remote_port;
	ipc_object_t reply = (ipc_object_t) kmsg->ikm_header.msgh_local_port;
	mach_msg_type_name_t dest_type = MACH_MSGH_BITS_REMOTE(mbits);
	mach_msg_type_name_t reply_type = MACH_MSGH_BITS_LOCAL(mbits);
	mach_port_t dest_name, reply_name;

	assert(IO_VALID(dest));

	io_lock(dest);
	if (io_active(dest)) {
		ipc_object_copyout_dest(space, dest, dest_type, &dest_name);
		/* dest is unlocked */
	} else {
		io_release(dest);
		io_check_unlock(dest);
		dest_name = MACH_PORT_DEAD;
	}

	reply_name = (mach_port_t) reply;

	kmsg->ikm_header.msgh_bits = (MACH_MSGH_BITS_OTHER(mbits) |
				      MACH_MSGH_BITS(reply_type, dest_type));
	kmsg->ikm_header.msgh_local_port = dest_name;
	kmsg->ikm_header.msgh_remote_port = reply_name;
}

#include <mach_kdb.h>
#if	MACH_KDB

#include <ddb/db_output.h>
#include <ipc/ipc_print.h>
/*
 * Forward declarations
 */
void ipc_msg_print_untyped(
	mach_msg_body_t		*body);

char * ipc_type_name(
	int		type_name,
	boolean_t	received);

void ipc_print_type_name(
	int	type_name);

char *
ipc_type_name(
	int		type_name,
	boolean_t	received)
{
	switch (type_name) {
		case MACH_MSG_TYPE_PORT_NAME:
		return "port_name";
		
		case MACH_MSG_TYPE_MOVE_RECEIVE:
		if (received) {
			return "port_receive";
		} else {
			return "move_receive";
		}
		
		case MACH_MSG_TYPE_MOVE_SEND:
		if (received) {
			return "port_send";
		} else {
			return "move_send";
		}
		
		case MACH_MSG_TYPE_MOVE_SEND_ONCE:
		if (received) {
			return "port_send_once";
		} else {
			return "move_send_once";
		}
		
		case MACH_MSG_TYPE_COPY_SEND:
		return "copy_send";
		
		case MACH_MSG_TYPE_MAKE_SEND:
		return "make_send";
		
		case MACH_MSG_TYPE_MAKE_SEND_ONCE:
		return "make_send_once";
		
		default:
		return (char *) 0;
	}
}
		
void
ipc_print_type_name(
	int	type_name)
{
	char *name = ipc_type_name(type_name, TRUE);
	if (name) {
		db_printf("%s", name);
	} else {
		db_printf("type%d", type_name);
	}
}

/*
 * ipc_kmsg_print	[ debug ]
 */
void
ipc_kmsg_print(
	ipc_kmsg_t	kmsg)
{
	db_printf("kmsg=0x%x\n", kmsg);
	db_printf("ikm_next=0x%x, prev=0x%x, size=%d\n",
		  kmsg->ikm_next,
		  kmsg->ikm_prev,
		  kmsg->ikm_size);
#if	NORMA_IPC
	db_printf("page=0x%x, copy=0x%x, source_node=%d, ool_bytes=0x%x\n",
		  kmsg->ikm_page,
		  kmsg->ikm_copy,
		  kmsg->ikm_source_node,
		  kmsg->ikm_ool_bytes);
	db_printf("%snorma_acked, %shas_dest_right, %swait_for_send, ",
		  kmsg->ikm_norma_acked ? "" : "!",
		  kmsg->ikm_has_dest_right ? "" : "!",
		  kmsg->ikm_wait_for_send ? "" : "!");
	db_printf("%ssend_completed, %ssend_aborted\n",
		  kmsg->ikm_send_completed ? "" : "!",
		  kmsg->ikm_send_aborted ? "" : "!");
	db_printf("%sno_wait, %smust_wait, %scritical, ",
		  kmsg->ikm_no_wait ? "" : "!",
		  kmsg->ikm_must_wait ? "" : "!",
		  kmsg->ikm_critical ? "" : "!");
	db_printf("forward_proxy=0x%x, netarray=0x%x\n",
		  kmsg->ikm_forward_proxy,
		  kmsg->ikm_netarray);
#endif	/* NORMA_IPC */
	ipc_msg_print(&kmsg->ikm_header);
}

/*
 * ipc_msg_print	[ debug ]
 */
void
ipc_msg_print(
	mach_msg_header_t	*msgh)
{
	db_printf("msgh_bits=0x%x: ", msgh->msgh_bits);
	if (msgh->msgh_bits & MACH_MSGH_BITS_COMPLEX) {
		db_printf("complex,");
	}
	if (msgh->msgh_bits & MACH_MSGH_BITS_CIRCULAR) {
		db_printf("circular,");
	}
	if (msgh->msgh_bits & MACH_MSGH_BITS_COMPLEX_PORTS) {
		db_printf("complex_ports,");
	}
	if (msgh->msgh_bits & MACH_MSGH_BITS_COMPLEX_DATA) {
		db_printf("complex_data,");
	}
	if (msgh->msgh_bits & MACH_MSGH_BITS_MIGRATED) {
		db_printf("migrated,");
	}
	if (msgh->msgh_bits & MACH_MSGH_BITS_UNUSED) {
		db_printf("unused=0x%x,",
			  msgh->msgh_bits & MACH_MSGH_BITS_UNUSED);
	}
	db_printf("l=0x%x,r=0x%x\n",
		  MACH_MSGH_BITS_LOCAL(msgh->msgh_bits),
		  MACH_MSGH_BITS_REMOTE(msgh->msgh_bits));

	db_printf("msgh_id=%d,size=%d",
		  msgh->msgh_id,
		  msgh->msgh_size);

	if (msgh->msgh_remote_port) {
		db_printf("remote=0x%x(", msgh->msgh_remote_port);
		ipc_print_type_name(MACH_MSGH_BITS_REMOTE(msgh->msgh_bits));
		db_printf("),");
	} else {
		db_printf("remote=null,\n");
	}

	if (msgh->msgh_local_port) {
		db_printf("local=0x%x(", msgh->msgh_local_port);
		ipc_print_type_name(MACH_MSGH_BITS_LOCAL(msgh->msgh_bits));
		db_printf(")\n");
	} else {
		db_printf("local=null\n");
	}

	if (msgh->msgh_bits & MACH_MSGH_BITS_COMPLEX) {	
		ipc_msg_print_untyped((mach_msg_body_t *) (msgh + 1));
	}
}

void
ipc_msg_print_untyped(
	mach_msg_body_t		*body)
{
    mach_msg_descriptor_t *saddr, *send;

    db_printf("%d descriptors: \n", body->msgh_descriptor_count);

    saddr = (mach_msg_descriptor_t *) (body + 1);
    send = saddr + body->msgh_descriptor_count;

    for ( ; saddr < send; saddr++ ) {
	
	switch (saddr->type.type) {
	    
	    case MACH_MSG_PORT_DESCRIPTOR: {
		char *disp;	
		mach_msg_port_descriptor_t *dsc;

		dsc = &saddr->port;
		switch(dsc->disposition) {
		    case MACH_MSG_TYPE_MOVE_RECEIVE:
			disp = "MOVE_RECEIVE";
			break;
		    case MACH_MSG_TYPE_MOVE_SEND:
			disp = "MOVE_SEND";
			break;
		    case MACH_MSG_TYPE_MOVE_SEND_ONCE:
			disp = "MOVE_SEND_ONCE";
			break;
		    case MACH_MSG_TYPE_COPY_SEND:
			disp = "COPY_SEND";
			break;
		    case MACH_MSG_TYPE_MAKE_SEND:
			disp = "MAKE_SEND";
			break;
		    case MACH_MSG_TYPE_MAKE_SEND_ONCE:
			disp = "MAKE_SEND_ONCE";
			break;
		    default:
			disp = "unknown";
			break;
		}
		
		db_printf("-- PORT name = 0x%x disp = %s\n", 
			   dsc->name, disp);
		break;
	    }
	    case MACH_MSG_OOL_DESCRIPTOR: {
		char *copy;
		mach_msg_ool_descriptor_t *dsc;
		
		dsc = &saddr->out_of_line;
		switch(dsc->copy) {
		    case MACH_MSG_PHYSICAL_COPY:
			copy = "PHYSICAL";
			break;
		    case MACH_MSG_VIRTUAL_COPY:
			copy = "VIRTUAL";
			break;
		    case MACH_MSG_OVERWRITE:
			copy = "OVERWRITE";
			break;
		    case MACH_MSG_ALLOCATE:
			copy = "ALLOCATE";
			break;
		    case MACH_MSG_KALLOC_COPY_T:
			copy = "KALLOC_COPY_T";
			break;
		    case MACH_MSG_PAGE_LIST_COPY_T:
			copy = "PAGE_LIST_COPY_T";
			break;
		    default:
			copy = "unknown";
			break;
		}

		db_printf("-- OOL_DATA addr = 0x%x size = 0x%x copy = %s %s\n",
		          dsc->address, dsc->size, copy,
			  dsc->deallocate ? "DEALLOC" : "");
		break;
	    } 
	    case MACH_MSG_OOL_PORTS_DESCRIPTOR : {
		char *copy;
		char *disp;	
		mach_msg_ool_ports_descriptor_t *dsc;

		dsc = &saddr->ool_ports;
		switch(dsc->disposition) {
		    case MACH_MSG_TYPE_MOVE_RECEIVE:
			disp = "MOVE_RECEIVE";
			break;
		    case MACH_MSG_TYPE_MOVE_SEND:
			disp = "MOVE_SEND";
			break;
		    case MACH_MSG_TYPE_MOVE_SEND_ONCE:
			disp = "MOVE_SEND_ONCE";
			break;
		    case MACH_MSG_TYPE_COPY_SEND:
			disp = "COPY_SEND";
			break;
		    case MACH_MSG_TYPE_MAKE_SEND:
			disp = "MAKE_SEND";
			break;
		    case MACH_MSG_TYPE_MAKE_SEND_ONCE:
			disp = "MAKE_SEND_ONCE";
			break;
		    default:
			disp = "unknown";
			break;
		}
		
		switch(dsc->copy) {
		    case MACH_MSG_PHYSICAL_COPY:
			copy = "PHYSICAL";
			break;
		    case MACH_MSG_VIRTUAL_COPY:
			copy = "VIRTUAL";
			break;
		    case MACH_MSG_OVERWRITE:
			copy = "OVERWRITE";
			break;
		    case MACH_MSG_ALLOCATE:
			copy = "ALLOCATE";
			break;
		    default:
			copy = "unknown";
			break;
		}

		db_printf("-- OOL_PORTS addr = 0x%x count = 0x%x disp = %s copy = %s %s\n",
		          dsc->address, dsc->count, disp, copy, 
			  dsc->deallocate ? "DEALLOC" : "");
		break;
	    }
	    default : {
		printf("-- UNKNOWN DESCRIPTOR 0x%x\n",
		       saddr->type.type);
	    }
	}
    }
}
#endif	/* MACH_KDB */
