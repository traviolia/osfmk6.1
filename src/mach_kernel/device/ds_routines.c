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
 * Revision 2.26.2.9  92/09/15  18:16:27  jeffreyh
 * 	Device forward changes. 
 * 	[92/09/15            jeffreyh]
 * 
 * Revision 2.26.2.8  92/09/15  17:14:34  jeffreyh
 * 	Dev forward changes from Intel
 * 	[92/09/10            jeffreyh]
 * 
 * 22-Jun-92  Alan Langerman (alanl) at Open Software Foundation
 *    Add assertions verifying copy object is of page list flavor.
 *
 * Revision 2.26.2.7  92/05/28  18:16:43  jeffreyh
 * 	Change arguments to remote_device to pass new type field.
 * 
 * Revision 2.26.2.6  92/03/28  10:04:41  jeffreyh
 * 	Calculate minimum transfer size in device_write_get and
 * 	pass it to kmem_io_map_copyout.  This makes large block
 * 	sizes work with tapes.
 * 	[92/03/20  14:09:52  dlb]
 * 
 * Revision 2.26.2.5  92/03/03  16:13:49  jeffreyh
 * 	Increase DEVICE_IO_MAP_SIZE to 2 megs
 * 	[92/03/03  13:53:25  jeffreyh]
 * 
 * 	Change panic message to match previous change.
 * 	[92/02/21  10:11:55  dlb]
 * 
 * 	Use page lists for device reads going to default pager.
 * 	[92/02/20  15:16:19  dlb]
 * 
 * 	Temporary change to return data read from devices as a page
 * 	list instead of an entry list.  The keep_wired logic has
 * 	to be updated to convert the default pager to this mechanism
 * 	when making this change permanent.
 * 	[92/02/19  17:36:50  dlb]
 * 	[David L. Black 92/02/22  17:03:11  dlb@osf.org]
 * 		Check protection argument to device_map.
 * 
 * Revision 2.26.2.4  92/02/18  18:39:30  jeffreyh
 * 	Increased DEVICE_IO_MAP_SIZE to 1 Meg
 * 	[91/12/06            bernadat]
 * 
 * Revision 2.26.2.3  92/01/21  21:49:45  jsb
 * 	Added ds_notify and ds_no_senders routines in preparation for using
 * 	no-senders notifications to close unreferenced devices.
 * 	[92/01/21  18:15:38  jsb]
 * 
 * Revision 2.26.2.2  92/01/09  18:43:29  jsb
 * 	Use remote_device() instead of norma_get_special_port().
 * 	[92/01/04  18:15:21  jsb]
 * 
 * Revision 2.26.2.1  92/01/03  16:34:41  jsb
 * 	Corrected log.
 * 	[91/12/24  13:51:07  jsb]
 * 
 * Revision 2.26  91/12/10  13:25:33  jsb
 * 	Merged in 2.25.1.1 from dlb.
 * 	[91/12/10  11:16:31  jsb]
 * 
 * Revision 2.25.1.1  91/12/05  10:55:51  dlb
 * 	4-Dec-91 David L. Black (dlb) at Open Software Foundation
 * 	Change ds_read_done to call vm_map_copyin_page_list directly
 * 	if destination of reply is remote.
 * 
 * Revision 2.25  91/11/14  16:52:41  rpd
 * 	Replaced master_device_port_at_node call with
 *	call to norma_get_special_port.
 * 	[91/11/00  00:00:00  jsb]
 * 
 * Revision 2.24  91/10/09  16:05:37  af
 * 	Fixed device_write_get to check kmem_io_map_copyout return code.
 * 	Enabled wait_for_space in device_io_map.
 * 	[91/09/17            rpd]
 * 
 * Revision 2.23  91/09/12  16:37:22  bohman
 * 	Changed device_write_inband() to not require a reply port.
 * 	Fixed device_write_get() to allow inband calls.  In this case,
 * 	an io_inband buffer is allocated and the data is copied into it.
 * 	Fixed device_write_dealloc() to correctly deallocate io_inband
 * 	buffers.
 * 	Fixed ds_read_done() to free io_inband buffers only if one was
 * 	actually allocated.
 * 	[91/09/11  17:06:50  bohman]
 * 
 * Revision 2.22  91/08/28  11:11:16  jsb
 * 	From rpd: increased DEVICE_IO_MAP_SIZE; documented why
 * 	device_write_get cannot be used for in-band data.
 * 	[91/08/22  15:28:19  jsb]
 * 
 * 	In device_write_get, always set the wait parameter to something;
 * 	by default, it's FALSE.
 * 	[91/08/16  14:19:31  jsb]
 * 
 * 	Support synchronous wait by writers when vm continuations are present.
 * 	Optimize device_write_dealloc.  Fix MP bug in iowait/iodone.
 * 	Convert from bsize to dev_info entry in device op vector.
 * 	[91/08/12  17:27:15  dlb]
 * 
 * 	Page lists working reliably: delete old code.
 * 	[91/08/06  17:16:09  dlb]
 * 
 * 	Clean up and add continuation support for device_write page lists.
 * 	[91/08/05  17:30:38  dlb]
 * 
 * 	First version of support for vm page lists in device_write.
 * 	Still needs cleanup and continuation support.  Old code left
 * 	under #ifdef 0.  
 * 	[91/07/31  14:42:24  dlb]
 * 
 * Revision 2.21  91/08/24  11:55:43  af
 * 	Spls definitions.
 * 	[91/08/02  02:44:45  af]
 * 
 * Revision 2.20  91/08/03  18:17:33  jsb
 * 	Device_write_get doesn't need to do anything for loaned ior's.
 * 	[91/08/02  12:13:15  jsb]
 * 
 * 	Create the right flavor of copy object in ds_read_done.
 * 	Replace NORMA_BOOT conditionals with NORMA_DEVICE.
 * 	Free loaned ior's directly in iodone().
 * 	[91/07/27  22:45:09  jsb]
 * 
 * Revision 2.19  91/06/25  10:26:57  rpd
 * 	Changed mach_port_t to ipc_port_t where appropriate.
 * 	Removed device_reply_search and device_reply_terminate.
 * 	[91/05/28            rpd]
 * 
 * Revision 2.18  91/06/17  15:43:58  jsb
 * 	Renamed NORMA conditionals.
 * 	[91/06/17  09:58:59  jsb]
 * 
 * Revision 2.17  91/05/18  14:29:52  rpd
 * 	Added vm/memory_object.h.
 * 	[91/03/22            rpd]
 * 
 * Revision 2.16  91/05/14  15:47:34  mrt
 * 	Correcting copyright
 * 
 * Revision 2.15  91/03/16  14:43:02  rpd
 * 	Updated for new kmem_alloc interface.
 * 	[91/03/03            rpd]
 * 	Added io_done_thread_continue.
 * 	[91/02/13            rpd]
 * 	Removed thread_swappable.
 * 	[91/01/18            rpd]
 * 
 * Revision 2.14  91/02/05  17:09:25  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  17:28:57  mrt]
 * 
 * Revision 2.13  91/01/08  15:09:38  rpd
 * 	Added continuation argument to thread_block.
 * 	[90/12/08            rpd]
 * 
 * Revision 2.12  90/12/14  10:59:39  jsb
 * 	Moved mechanism for mapping global to local device names
 * 	to the machine-dependent function dev_forward_name.
 * 	[90/12/14  09:37:18  jsb]
 * 
 * 	Added device request forwarding to support inter-node device access.
 * 	[90/12/14  08:30:53  jsb]
 * 
 * Revision 2.11  90/10/25  14:44:32  rwd
 * 	Let ds_device_write proceed w/o a valid reply port.  This is used
 * 	by the unix server ether_output routine.
 * 	[90/10/22            rwd]
 * 	Fixed ds_write_done to use ds_device_write_inband_reply
 * 	when appropriate.
 * 	[90/10/18            rpd]
 * 	Check for invalid reply ports.
 * 	[90/10/17            rwd]
 * 
 * Revision 2.10  90/09/09  14:31:27  rpd
 * 	Use decl_simple_lock_data.
 * 	[90/08/30            rpd]
 * 
 * Revision 2.9  90/06/02  14:48:00  rpd
 * 	Cleaned up check for default pager in ds_read_done.
 * 	[90/04/29            rpd]
 * 
 * 	Fixed ds_read_done to leave memory wired if the read reply
 * 	is being sent to the default pager.
 * 	[90/04/05            rpd]
 * 	Converted to new IPC.  Purged MACH_XP_FPD.
 * 	[90/03/26  21:55:28  rpd]
 * 
 * Revision 2.8  90/02/22  20:02:12  dbg
 * 	Use vm_map_copy routines.
 * 	[90/01/25            dbg]
 * 
 * Revision 2.7  90/01/11  11:42:01  dbg
 * 	De-lint.
 * 	[89/12/06            dbg]
 * 
 * Revision 2.6  89/11/29  14:08:54  af
 * 	iodone() should set the IO_DONE flag.
 * 	[89/11/03  16:58:16  af]
 * 
 * Revision 2.5  89/11/14  10:28:19  dbg
 * 	Make read and write handle zero-length transfers correctly (used
 * 	to implement select).
 * 	[89/10/27            dbg]
 * 
 * Revision 2.4  89/09/08  11:24:17  dbg
 * 	Converted to run in kernel context.
 * 	Add list of wired pages to tail of IOR allocated for write.
 * 	Reorganized file: moved open/close to beginning, map to end.
 * 	[89/08/23            dbg]
 * 
 * Revision 2.3  89/08/31  16:18:46  rwd
 * 	Added ds_read_inband and support
 * 	[89/08/15            rwd]
 * 
 * Revision 2.2  89/08/05  16:06:39  rwd
 * 	Added ds_write_inband for use by tty and ds_device_map_device.
 * 	[89/07/17            rwd]
 * 
 * 12-Apr-89  David Golub (dbg) at Carnegie-Mellon University
 *	Added device_reply_terminate.
 *
 *  3-Mar-89  David Golub (dbg) at Carnegie-Mellon University
 *	Created.
 *
 */
/* CMU_ENDHIST */

/*
 *(C)UNIX System Laboratories, Inc. all or some portions of this file are
 *derived from material licensed to the University of California by
 *American Telephone and Telegraph Co. or UNIX System Laboratories,
 *Inc. and are reproduced herein with the permission of UNIX System
 *Laboratories, Inc.
 */

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
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date: 	3/89
 */

#include <mach_kdb.h>
#include <norma_device.h>

#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/mig_errors.h>
#include <mach/port.h>
#include <mach/vm_param.h>
#include <mach/notify.h>
#include <mach/mach_server.h>
#include <mach/mach_host_server.h>

#include <machine/machparam.h>		/* spl definitions */

#include <ipc/ipc_port.h>
#include <ipc/ipc_space.h>

#include <kern/spl.h>
#include <kern/ast.h>
#include <kern/counters.h>
#include <kern/queue.h>
#include <kern/zalloc.h>
#include <kern/thread.h>
#include <kern/task.h>
#include <kern/sched_prim.h>
#include <kern/misc_protos.h>

#include <vm/memory_object.h>
#include <vm/pmap.h>
#include <vm/vm_map.h>
#include <vm/vm_kern.h>

#include <device/device_types.h>
#include <device/dev_hdr.h>
#include <device/conf.h>
#include <device/io_req.h>
#include <device/ds_routines.h>
#include <device/net_status.h>
#include <device/net_io.h>
#include <device/device_port.h>
#include <device/device_reply.h>
#include <device/device_server.h>

#include <machine/machparam.h>

#if	NORMA_DEVICE
#include <norma/ipc_node.h>
#include <norma/ipc_net.h>
#include <norma/dev_forward.h>
#endif	/* NORMA_DEVICE */

#if	PARAGON860 || iPSC386 || iPSC860
int	ndf_noise=0;
#endif

/* Forward */

extern void		log_thread_action (char *, long, long, long);

extern void		io_done_thread_continue(void);
extern io_return_t	ds_no_senders(
				mach_no_senders_notification_t
						* notification);

#define	log_io_map(m,ior)					\
	log_thread_action (m, (long)ior, *(long *)&ior->io_data,\
		(long)ior->io_op)

#define	splio	splsched	/* XXX must block ALL io devices */

#define io_done_queue_request(q)				\
	MACRO_BEGIN						\
	if ((io_done_queue_t) (q) != IO_DONE_QUEUE_NULL) {	\
		spl_t		s = splio();			\
		io_done_queue_lock((io_done_queue_t) (q));	\
		((io_done_queue_t) (q))->io_in_progress++;	\
		io_done_queue_unlock((io_done_queue_t) (q));	\
		splx(s);					\
	}							\
	MACRO_END

io_return_t
ds_device_open(
	ipc_port_t		open_port,
	ipc_port_t		reply_port,
	mach_msg_type_name_t 	reply_port_type,
	dev_mode_t		mode,
	char *			name,
	device_t		*device_p)	/* out */
{
	register device_t	device;
	register io_return_t	result;
	register io_req_t	ior;
	char			namebuf[64];
	ipc_port_t		notify;
	static int		mynode= -1; /* whoami ? */


	/*
	 * Open must be called on the master device port.
	 */
	if (open_port != master_device_port)
	    return (D_INVALID_OPERATION);

	/*
	 * There must be a reply port.
	 */
	if (!IP_VALID(reply_port)) {
	    printf("ds_* invalid reply port\n");
	    Debugger("ds_* reply_port");
	    return (MIG_NO_REPLY);	/* no sense in doing anything */
	}

#if	NORMA_DEVICE
	if ( mynode == -1 )
		mynode = node_self();
	/*
	 * Translate device name if needed:
	 * With the server sending to the correct MK, device forwarding becomes
	 * an issue only when booting; trying to read pagingfile & server
	 * from the boot file system.
	 */
	/*
	 * Map global device name to <node> + local device name.
	 */
	if (name[0] != '<')
		name = dev_forward_name(name, namebuf, sizeof(namebuf));

	/*
	 * Look for explicit node specifier, e.g., <2>sd0a.
	 * If found, then forward request to correct device server.
	 * If not found, then remove '<n>' and process locally.
	 *
	 * XXX should handle send-right reply_port as well as send-once XXX
	 */
	if (name[0] == '<') {
		char *n;
		int node = 0;

		for (n = &name[1]; *n != '>'; n++) {
			if (*n >= '0' && *n <= '9') {
				node = 10 * node + (*n - '0');
			} else {
				return (D_NO_SUCH_DEVICE);
			}
		}
		if (node == mynode) {
			name = &n[1];	/* skip trailing '>' */
		} else {
			forward_device_open_send(remote_device(node,
						      MACH_MSG_TYPE_PORT_SEND),
						 reply_port, mode, name);
			return (MIG_NO_REPLY);
		}
	}
#endif	/* NORMA_DEVICE */

#if	PARAGON860 || iPSC386 || iPSC860
if (ndf_noise) printf("local: %s@%d\n",name,mynode); /* XXX i860 */
#endif	/* PARAGON860 || iPSC386 || iPSC860 */

	/*
	 * Find the local device.
	 */
	device = device_lookup(name);
	if (device == DEVICE_NULL) {
#if	PARAGON860 || iPSC386 || iPSC860
if (ndf_noise) printf("device_lookup(%s) failed\n",name);
#endif	/* PARAGON860 || iPSC386 || iPSC860 */
	    return (D_NO_SUCH_DEVICE);
	}

	/*
	 * If the device is being opened or closed,
	 * wait for that operation to finish.
	 */
	device_lock(device);
	while (device->state == DEV_STATE_OPENING ||
		device->state == DEV_STATE_CLOSING) {
	    device->io_wait = TRUE;
	    thread_sleep_mutex((int)device, &device->lock, TRUE);
	    device_lock(device);
	}

	/*
	 * If the device is already open, increment the open count
	 * and return.
	 */
	if (device->state == DEV_STATE_OPEN) {

	    if (device->flag & D_EXCL_OPEN) {
		/*
		 * Cannot open a second time.
		 */
		device_unlock(device);
		device_deallocate(device);
		return (D_ALREADY_OPEN);
	    }

	    device->open_count++;
	    device_unlock(device);
	    *device_p = device;
	    return (D_SUCCESS);
	    /*
	     * Return deallocates device reference while acquiring
	     * port.
	     */
	}

	/*
	 * Allocate the device port and register the device before
	 * opening it.
	 */
	device->state = DEV_STATE_OPENING;
	device_unlock(device);

	/*
	 * Allocate port, keeping a reference for it.
	 */
	device->port = ipc_port_alloc_kernel();
	if (device->port == IP_NULL) {
	    device_lock(device);
	    device->state = DEV_STATE_INIT;
	    device->port = IP_NULL;
	    if (device->io_wait) {
		device->io_wait = FALSE;
		thread_wakeup((int)device);
	    }
	    device_unlock(device);
	    device_deallocate(device);
	    return (KERN_RESOURCE_SHORTAGE);
	}

	dev_port_enter(device);

	/*
	 * Request no-senders notifications on device port.
	 */
	notify = ipc_port_make_sonce(device->port);
	ip_lock(device->port);
	ipc_port_nsrequest(device->port, 1, notify, &notify);
	assert(notify == IP_NULL);

	/*
	 * Open the device.
	 */
	io_req_alloc(ior);

	ior->io_device	= device;
	ior->io_unit	= device->dev_number;
	ior->io_op	= IO_OPEN | IO_CALL;
	ior->io_mode	= mode;
	ior->io_error	= 0;
	ior->io_done	= ds_open_done;
	ior->io_reply_port = reply_port;
	ior->io_reply_port_type = reply_port_type;

	result = (*device->dev_ops->d_open)(device->dev_number, (int)mode, ior);
	if (result == D_IO_QUEUED)
	    return (MIG_NO_REPLY);

	/*
	 * Return result via ds_open_done.
	 */
	ior->io_error = result;
	(void) ds_open_done(ior);

	io_req_free(ior);

	return (MIG_NO_REPLY);	/* reply already sent */
}

boolean_t
ds_open_done(ior)
	register io_req_t	ior;
{
	kern_return_t		result;
	register device_t	device;
	ipc_port_t		port;

	device = ior->io_device;
	result = ior->io_error;

	if (result != D_SUCCESS) {
	    /*
	     * Open failed.  Deallocate port and device.
	     */
	    dev_port_remove(device);
	    ipc_port_dealloc_kernel(device->port);
	    device->port = IP_NULL;

	    device_lock(device);
	    device->state = DEV_STATE_INIT;
	    if (device->io_wait) {
		device->io_wait = FALSE;
		thread_wakeup((int)device);
	    }
	    device_unlock(device);

	    device_deallocate(device);
	    if (IP_VALID(ior->io_reply_port))
		(void) ds_device_open_reply(ior->io_reply_port,
					    ior->io_reply_port_type,
					    result, IP_NULL);
	}
	else {
	    /*
	     * Open succeeded.
	     */
	    device_lock(device);
	    device->state = DEV_STATE_OPEN;
	    device->open_count = 1;
	    if (device->io_wait) {
		device->io_wait = FALSE;
		thread_wakeup((int)device);
	    }

	    if (IP_VALID(ior->io_reply_port)) {
		port = device->port;
		ipc_port_reference(port);
		device_unlock(device);
		(void) ds_device_open_reply(ior->io_reply_port,
					    ior->io_reply_port_type,
					    result, port);
		ipc_port_release(port);
	    } else
		device_unlock(device);
	}

	device_deallocate(device);
	return (TRUE);
}

io_return_t
ds_device_close(
	device_t	device)
{
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	device_lock(device);

	/*
	 * If device will remain open, do nothing.
	 */
	if (--device->open_count > 0) {
	    device_unlock(device);
	    return (D_SUCCESS);
	}

	/*
	 * If device is being closed, do nothing.
	 */
	if (device->state == DEV_STATE_CLOSING) {
	    device_unlock(device);
	    return (D_SUCCESS);
	}

	/*
	 * Mark device as closing, to prevent new IO.
	 * Outstanding IO will still be in progress.
	 */
	device->state = DEV_STATE_CLOSING;
	device_unlock(device);

	/*
	 * ? wait for IO to end ?
	 *   only if device wants to
	 */

	/*
	 * Remove the device-port association.
	 */
	dev_port_remove(device);
	ipc_port_dealloc_kernel(device->port);

	/*
	 * Close the device
	 */
	(*device->dev_ops->d_close)(device->dev_number);

	/*
	 * Finally mark it closed.  If someone else is trying
	 * to open it, the open can now proceed.
	 */
	device_lock(device);
	device->state = DEV_STATE_INIT;
	if (device->io_wait) {
	    device->io_wait = FALSE;
	    thread_wakeup((int)device);
	}
	device_unlock(device);

	return (D_SUCCESS);
}

/*
 * Write to a device.
 */
io_return_t
ds_device_write_common(
	device_t		device,
	ipc_port_t		reply_port,
	mach_msg_type_name_t	reply_port_type,
	dev_mode_t		mode,
	recnum_t		recnum,
	io_buf_ptr_t		data,
	unsigned int		data_count,
	int			op,
	int			*bytes_written)	/* out */
{
	register io_req_t	ior;
	register io_return_t	result;

	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/*
	 * XXX Need logic to reject ridiculously big requests.
	 */

	/* XXX note that a CLOSE may proceed at any point */

	/*
	 * Package the write request for the device driver
	 */
	io_req_alloc(ior);

	ior->io_device		= device;
	ior->io_unit		= device->dev_number;
	ior->io_op		= op;
	ior->io_mode		= mode;
	ior->io_recnum		= recnum;
	ior->io_data		= data;
	ior->io_count		= data_count;
	ior->io_total		= data_count;
	ior->io_alloc_size	= 0;
	ior->io_residual	= 0;
	ior->io_error		= 0;
	ior->io_done		= ds_write_done;
	ior->io_copy		= VM_MAP_COPY_NULL;
	ior->io_reply_port	= reply_port;
	ior->io_reply_port_type	= reply_port_type;

	/*
	 * The ior keeps an extra reference for the device.
	 */
	device_reference(device);

#if 0
	if (data == 0)
	    panic("ds_device_write: no data");
#endif

	/*
	 * And do the write ...
	 *
	 * device_write_dealloc returns false if there's more
	 * to do; it has updated the ior appropriately and expects
	 * its caller to reinvoke it on the device.
	 */

	do {

		result = (*device->dev_ops->d_write)(device->dev_number, ior);

		/*
		 * If the IO was queued, delay reply until it is finished.
		 */
		if (result == D_IO_QUEUED) {
			assert(!(op & IO_SYNC));
			if (op & IO_QUEUE)
				io_done_queue_request(reply_port);
			return (MIG_NO_REPLY);
		}
		/*
		 * Discard the local mapping of the data.
		 */

	} while (!device_write_dealloc(ior));

	/*
	 * For Synchronous IO, return status.
	 */
	if (ior->io_error) /* check if there was an error set rag */
		result = ior->io_error;

	/*
	 * Return the number of bytes actually written.
	 */
	*bytes_written = ior->io_total - ior->io_residual;

	/*
	 * Remove the extra reference.
	 */
	device_deallocate(device);

	io_req_free(ior);
	return (result);
}

/*
 * Write to a device, but memory is in message.
 */
io_return_t
ds_device_write_inband_common(
	device_t		device,
	ipc_port_t		reply_port,
	mach_msg_type_name_t	reply_port_type,
	dev_mode_t		mode,
	recnum_t		recnum,
	io_buf_ptr_inband_t	data,
	unsigned int		data_count,
	int			op,
	int			*bytes_written) /* out */
{
	register io_req_t	ior;
	register io_return_t	result;
	io_done_queue_t		queue;

	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/* XXX note that a CLOSE may proceed at any point */

	/*
	 * Package the write request for the device driver.
	 */
	io_req_alloc(ior);

	ior->io_device		= device;
	ior->io_unit		= device->dev_number;
	ior->io_op		= op;
	ior->io_mode		= mode;
	ior->io_recnum		= recnum;
	ior->io_data		= data;
	ior->io_count		= data_count;
	ior->io_total		= data_count;
	ior->io_alloc_size	= 0;
	ior->io_residual	= 0;
	ior->io_error		= 0;
	ior->io_done		= ds_write_done;
	ior->io_reply_port	= reply_port;
	ior->io_reply_port_type = reply_port_type;

	/*
	 * The ior keeps an extra reference for the device.
	 */
	device_reference(device);

#if 0
	if (data == 0)
	    panic("ds_device_write: no data");
#endif

	/*
	 * And do the write.
	 */
	result = (*device->dev_ops->d_write)(device->dev_number, ior);

	/*
	 * If the IO was queued, delay reply until it is finished.
	 */
	if (result == D_IO_QUEUED) {
	    if (op & IO_QUEUE)
		io_done_queue_request(reply_port);
	    return (MIG_NO_REPLY);
	}

	/*
	 * If a queued request, enqueue the requests 
	 */
	if (op & IO_QUEUE &&
	    (queue = (io_done_queue_t) reply_port) != IO_DONE_QUEUE_NULL) {
		spl_t		s;

		s = splio();
	    	ior->io_op |= IO_DONE;
	    	io_done_queue_lock(queue);
	    	enqueue_tail(&queue->io_done_list, (queue_entry_t)ior);
	    	thread_wakeup((int)queue);
	    	io_done_queue_unlock(queue);
		splx(s);
	        return (MIG_NO_REPLY);
	}

	/*
	 * Return the number of bytes actually written.
	 */
	*bytes_written = ior->io_total - ior->io_residual;

	/*
	 * Remove the extra reference.
	 */
	device_deallocate(device);

	io_req_free(ior);
	return (result);
}

/*
 * Wire down incoming memory to give to device in scatter/gather list form.
 */
kern_return_t
device_write_get_sg(
	register io_req_t	ior,
	boolean_t		*wait)
{
	vm_map_copy_t		io_copy;
        struct io_sglist        *sgp;
	register kern_return_t	result;
	kern_return_t           kr;
	int			bsize;
	int			nentries;
	vm_size_t		min_size;

	/*
	 * By default, caller does not have to wait.
	 */
	*wait = FALSE;


	/*
	 * Nothing to do if no data.
	 */
	if (ior->io_count == 0)
	    return (KERN_SUCCESS);

	/*
	 * Loaned iors already have valid data.
	 */
	if (ior->io_op & IO_LOANED)
	    return (KERN_SUCCESS);

	/*
	 *	Figure out how much data to move this time.  If the device
	 *	won't return a block size, then we have to do the whole
	 *	request in one shot (ditto if this is a block fragment),
	 *	otherwise, move at least one block's worth.
	 */
	result = (*ior->io_device->dev_ops->d_dev_info)(
					ior->io_device->dev_number,
					D_INFO_BLOCK_SIZE,
					(char *)&bsize);

	if (result != KERN_SUCCESS || ior->io_count < (vm_size_t) bsize)
		min_size = (vm_size_t) ior->io_count;
	else
		min_size = (vm_size_t) bsize;

	/*
	 *	Collect the pages from this page list into a scatter/gather list.
	 *	io_data will point to the scatter/gather list.
	 *	io_alloc_size of the total length of the transfer.
	 *      ARD - make sure message is VM_MAP_COPY_PAGE_LIST type.
	 */
	io_copy = (vm_map_copy_t) ior->io_data;
#if	NORMA_IPC
	if (io_copy->type != VM_MAP_COPY_PAGE_LIST) {
		assert(io_copy->type == VM_MAP_COPY_OBJECT);
		kr = vm_map_object_to_page_list(&io_copy);
		assert(kr == KERN_SUCCESS);
		ior->io_data = (io_buf_ptr_t) io_copy;
	}
#endif	/* NORMA_IPC */
	assert(io_copy->type == VM_MAP_COPY_PAGE_LIST);
        nentries = vm_map_copy_page_count(io_copy);
        io_sglist_alloc(sgp, nentries);
        result = kmem_io_page_list_to_sglist(io_copy, min_size, sgp);
        if (result != KERN_SUCCESS) {
                io_sglist_free(sgp);
                return (result);
        }

        ior->io_sgp = sgp;
        ior->io_alloc_size = sgp->iosg_hdr.length;

	if (ior->io_count > sgp->iosg_hdr.length) {

		/*
		 *	Operation has to be split.  Reset io_count for how
		 *	much we can do this time.
		 */
		assert(vm_map_copy_has_cont(io_copy));
		assert(ior->io_count == io_copy->size);
		ior->io_count = sgp->iosg_hdr.length;

		/*
		 *	Caller must wait synchronously.
		 */
		ior->io_op &= ~IO_CALL;
		*wait = TRUE;		
	}

	ior->io_copy = io_copy;			/* vm_map_copy to discard */
	return (KERN_SUCCESS);
}

/*
 * Write to a device.
 */
io_return_t
ds_device_write(
	device_t		device,
	ipc_port_t		reply_port,
	mach_msg_type_name_t	reply_port_type,
	dev_mode_t		mode,
	recnum_t		recnum,
	io_buf_ptr_t		data,
	unsigned int		data_count,
	int			*bytes_written)	/* out */
{
	return(ds_device_write_common(device, reply_port, reply_port_type,
			       mode, recnum, data, data_count,
			       (IO_WRITE | IO_CALL), bytes_written));
}

/*
 * Write to a device, but memory is in message.
 */
io_return_t
ds_device_write_inband(
	device_t		device,
	ipc_port_t		reply_port,
	mach_msg_type_name_t	reply_port_type,
	dev_mode_t		mode,
	recnum_t		recnum,
	io_buf_ptr_t		data,
	unsigned int		data_count,
	int			*bytes_written)	/* out */
{
	return(ds_device_write_inband_common(device, reply_port,
	       reply_port_type, mode, recnum, data, data_count,
	       (IO_WRITE | IO_CALL | IO_INBAND), bytes_written));
}

/*
 * Wire down incoming memory to give to device.
 */
kern_return_t
device_write_get(
	io_req_t		ior,
	boolean_t		*wait)
{
	vm_map_copy_t		io_copy;
	vm_offset_t		new_addr;
	register kern_return_t	result;
	kern_return_t		kr;
	int			bsize;
	vm_size_t		min_size;

	/*
	 * By default, caller does not have to wait.
	 */
	*wait = FALSE;

	/*
	 * Nothing to do if no data.
	 */
	if (ior->io_count == 0)
	    return (KERN_SUCCESS);

	/*
	 * Loaned iors already have valid data.
	 */
	if (ior->io_op & IO_LOANED)
	    return (KERN_SUCCESS);

	/*
	 * Inband case.
	 */
	if (ior->io_op & IO_INBAND) {
	    assert(ior->io_count <= sizeof (io_buf_ptr_inband_t));
	    new_addr = zalloc(io_inband_zone);
	    bcopy(ior->io_data, (char *)new_addr, ior->io_count);
	    ior->io_data = (io_buf_ptr_t)new_addr;
	    ior->io_alloc_size = sizeof (io_buf_ptr_inband_t);

	    return (KERN_SUCCESS);
	}
	else {
		boolean_t sgio = FALSE;
		/*
		 *	Figure out if this device can to scatter/gather I/O.
		 */
		result = (*ior->io_device->dev_ops->d_dev_info)(
					ior->io_device->dev_number,
					D_INFO_SGLIST_IO,
					(char *)&sgio);

		if ((result == KERN_SUCCESS) && sgio) {
			/*
			 * XXX	scatter/gather devices can't do kernel buf.
			 */
			if (ior->io_op & IO_KERNEL_BUF)
	panic("Kernel Buffer not supported for scatter/gather devices yet.");

			/*
			 * Yes, note it and do the setup.
			 */
			ior->io_op |= IO_SGLIST;
			return (device_write_get_sg(ior, wait));
		}
        }

	/*
	 *	Kernel buffer case (kernel loaded server).
	 *	vm_map_copyin_page_list (w/ pmap_enter opt)
	 *	must be called because buffer may not be wired.
	 *
	 * XXX	current_map() assumes that driver entry point
	 * XXX	didn't do any request queueing.
	 */
	if (ior->io_op & IO_KERNEL_BUF) {
		result = vm_map_copyin_page_list(current_map(),
				(vm_offset_t) ior->io_data, ior->io_count,
				(VM_PROT_READ | VM_MAP_COPYIN_OPT_PMAP_ENTER),
				&ior->io_copy, FALSE);

		ior->io_alloc_size = ior->io_count;

		/*
		 * XXX	Can't handle continuations!!  Might be able to
		 * XXX	recover by clearing IO_KERNEL_BUF and using code
		 * XXX	below (???).
		 */
		if (vm_map_copy_has_cont(ior->io_copy))
			panic("device_write_get: write too large!!");

		return (result);
	}

	/*
	 *	Figure out how much data to move this time.  If the device
	 *	won't return a block size, then we have to do the whole
	 *	request in one shot (ditto if this is a block fragment),
	 *	otherwise, move at least one block's worth.
	 */
	result = (*ior->io_device->dev_ops->d_dev_info)(
					ior->io_device->dev_number,
					D_INFO_BLOCK_SIZE,
					(char *)&bsize);

	if (result != KERN_SUCCESS || ior->io_count < (vm_size_t) bsize)
		min_size = (vm_size_t) ior->io_count;
	else
		min_size = (vm_size_t) bsize;

	/*
	 *	Map the pages from this page list into memory.
	 *	io_data records location of data.
	 *	io_alloc_size is the vm size of the region to deallocate.
	 */
	io_copy = (vm_map_copy_t) ior->io_data;
#if	NORMA_IPC
	if (io_copy->type != VM_MAP_COPY_PAGE_LIST) {
		assert(io_copy->type == VM_MAP_COPY_OBJECT);
		kr = vm_map_object_to_page_list(&io_copy);
		assert(kr == KERN_SUCCESS);
		ior->io_data = (io_buf_ptr_t) io_copy;
	}
#endif	/* NORMA_IPC */
	assert(io_copy->type == VM_MAP_COPY_PAGE_LIST);
	result = kmem_io_map_copyout(device_io_map,
				     (vm_offset_t *)&ior->io_data,
				     (vm_size_t *)&ior->io_alloc_size,
				     io_copy, min_size, VM_PROT_READ);
	if (result != KERN_SUCCESS)
	    return (result);

log_io_map ("device_write_get", ior);

	if ((ior->io_data + ior->io_count) > 
	    ((io_buf_ptr_t)(trunc_page(ior->io_data) + ior->io_alloc_size))) {

		/*
		 *	Operation has to be split.  Reset io_count for how
		 *	much we can do this time.
		 */
		assert(vm_map_copy_has_cont(io_copy));
		assert(ior->io_count == io_copy->size);
		ior->io_count = ior->io_alloc_size -
			(ior->io_data - ((char *)new_addr));

		/*
		 *	Caller must wait synchronously. Remember whether
		 *	it was IO_CALL or IO_QUEUE.
		 */
		ior->io_save = ior->io_op;
		ior->io_op &= ~(IO_CALL|IO_QUEUE);
		*wait = TRUE;		
	}

	ior->io_copy = io_copy;			/* vm_map_copy to discard */
	return (KERN_SUCCESS);
}

/*
 * Clean up memory allocated for IO.
 */
boolean_t
device_write_dealloc(
	io_req_t	ior)
{
	vm_map_copy_t	new_copy = VM_MAP_COPY_NULL;
	register
	vm_map_copy_t	io_copy;
	kern_return_t	result;
	vm_offset_t	size_to_do;
	int		bsize;	

	if (ior->io_alloc_size == 0)
	    return (TRUE);

	/*
	 * Inband case.
	 */
	if (ior->io_op & IO_INBAND) {
	    zfree(io_inband_zone, (vm_offset_t)ior->io_data);

	    return (TRUE);
	}
	
	/*
	 * Kernel buffer case.  Free page list that is keeping pages wired.
	 */
	if (ior->io_op & IO_KERNEL_BUF) {
	    vm_map_copy_discard(ior->io_copy);

	    return (TRUE);
	}

	if ((io_copy = ior->io_copy) == VM_MAP_COPY_NULL)
	    return (TRUE);
	assert(io_copy->type == VM_MAP_COPY_PAGE_LIST);

        if (ior->io_op & IO_SGLIST) {
                io_sglist_free(ior->io_sgp);
	} else {
		/*
		 *	To prevent a possible deadlock with the default pager,
		 *	we have to release space in the device_io_map before
		 *	we allocate any memory.  (Which vm_map_copy_invoke_cont
		 *	might do.)  See the discussion in ds_init.
		 */

log_io_map ("device_write_dealloc", ior);
		kmem_io_map_deallocate(device_io_map,
				       trunc_page(ior->io_data),
				       (vm_size_t) ior->io_alloc_size);
	}

	if (vm_map_copy_has_cont(io_copy)) {

		/*
		 *	Remember how much is left, then 
		 *	invoke or abort the continuation.
		 */
		size_to_do = io_copy->size - ior->io_count;
		if (ior->io_error == 0) {
			vm_map_copy_invoke_cont(io_copy, &new_copy, &result);
		}
		else {
			vm_map_copy_abort_cont(io_copy);
			result = KERN_FAILURE;
		}

		if (result == KERN_SUCCESS && new_copy != VM_MAP_COPY_NULL) {
			register int	res;

			/*
			 *	We have a new continuation, reset the ior to
			 *	represent the remainder of the request.  Must
			 *	adjust the recnum because drivers assume
			 *	that the residual is zero.
			 */
			ior->io_op &= ~(IO_DONE|IO_CALL|IO_QUEUE);
			ior->io_op |= (ior->io_save & (IO_CALL|IO_QUEUE));

			res = (*ior->io_device->dev_ops->d_dev_info)(
					ior->io_device->dev_number,
					D_INFO_BLOCK_SIZE,
					(char *)&bsize);

			if (res != D_SUCCESS)
				panic("device_write_dealloc: No block size");
			
			ior->io_recnum += ior->io_count/bsize;
			ior->io_count = new_copy->size;
		}
		else {

			/*
			 *	No continuation.  Add amount we didn't get
			 *	to into residual.
			 */
			ior->io_residual += size_to_do;
		}
	}

	/*
	 *	Clean up the state for the IO that just completed.
	 */
        vm_map_copy_discard(ior->io_copy);
	ior->io_copy = VM_MAP_COPY_NULL;
	ior->io_data = (char *) new_copy;

	/*
	 *	Return FALSE if there's more IO to do.
	 */

	return(new_copy == VM_MAP_COPY_NULL);
}

/*
 * Send write completion message to client, and discard the data.
 */
boolean_t
ds_write_done(
	io_req_t	ior)
{
	/*
	 *	device_write_dealloc discards the data that has been
	 *	written, but may decide that there is more to write.
	 */
	while (!device_write_dealloc(ior)) {
		register io_return_t	result;
		register device_t	device;

		/*
		 *     More IO to do -- invoke it.
		 */
		device = ior->io_device;
		result = (*device->dev_ops->d_write)(device->dev_number, ior);

		/*
		 * If the IO was queued, return FALSE -- not done yet.
		 */
		if (result == D_IO_QUEUED) {
			if (ior->io_op & IO_QUEUE)
				io_done_queue_request(ior->io_reply_port);
		    	return (FALSE);
		}
	}

	/*
	 *	Now the write is really complete.  Send reply.
	 */

	if (!(ior->io_op & (IO_SYNC|IO_QUEUE)) && IP_VALID(ior->io_reply_port))
	{
	    (void) (*((ior->io_op & IO_INBAND) ?
		      ds_device_write_reply_inband :
		      ds_device_write_reply))(ior->io_reply_port,
					      ior->io_reply_port_type,
					      ior->io_error,
					      (int) (ior->io_total -
						     ior->io_residual));
	}
	device_deallocate(ior->io_device);

	return (TRUE);
}

/*
 * Read from a device.
 */
io_return_t
ds_device_read_common(
	device_t		device,
	ipc_port_t		reply_port,
	mach_msg_type_name_t	reply_port_type,
	dev_mode_t		mode,
	recnum_t		recnum,
	int			bytes_wanted,
	int			op,
	io_buf_ptr_t		*data,		/* out */
	unsigned int		*data_count)	/* out */
{
	register io_req_t	ior;
	register io_return_t	result;

#ifdef lint
	*data = *data;
	*data_count = *data_count;
#endif /* lint */

	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/* XXX note that a CLOSE may proceed at any point */

	/*
	 * There must be a reply port unless the call is synchronous.
	 */
	if (!(op & (IO_SYNC|IO_QUEUE)) && !IP_VALID(reply_port)) {
	    printf("ds_* invalid reply port\n");
	    Debugger("ds_* reply_port");
	    return (MIG_NO_REPLY);	/* no sense in doing anything */
	}

	/*
	 * Package the read request for the device driver
	 */
	io_req_alloc(ior);

	ior->io_device		= device;
	ior->io_unit		= device->dev_number;
	ior->io_op		= op;
	ior->io_mode		= mode;
	ior->io_recnum		= recnum;
	ior->io_data		= 0;		/* driver must allocate data */
	ior->io_count		= bytes_wanted;
	ior->io_alloc_size	= 0;		/* no data allocated yet */
	ior->io_residual	= 0;
	ior->io_error		= 0;
	ior->io_done		= ds_read_done;
	ior->io_reply_port	= reply_port;
	ior->io_reply_port_type	= reply_port_type;

	/*
	 * The ior keeps an extra reference for the device.
	 */
	device_reference(device);

	/*
	 * And do the read.
	 */
	result = (*device->dev_ops->d_read)(device->dev_number, ior);

	/*
	 * If the IO was queued, delay reply until it is finished.
	 */
	if (result == D_IO_QUEUED) {
		assert(!(op & IO_SYNC));
		if (op & IO_QUEUE)
			io_done_queue_request(reply_port);
		return (MIG_NO_REPLY);
	}

	/*
	 * Return result via ds_read_done.
	 * If there was a problem queueing the IO, report it.
	 * Otherwise, return status of IO.
	 */
	if (result != D_SUCCESS)
		ior->io_error = result;
	else
		result = ior->io_error;

	(void) ds_read_done(ior);
	*data = ior->io_data;
	*data_count = ior->io_total;
	io_req_free(ior);

	/*
	 *	Reply has already been sent if not synchronous.
	 */
	return ((op & IO_SYNC) ? result : MIG_NO_REPLY);
}

/*
 * Read from a device, but return the data 'inband.'
 */
io_return_t
ds_device_read_inband_common(
	register device_t	device,
	ipc_port_t		reply_port,
	mach_msg_type_name_t	reply_port_type,
	dev_mode_t		mode,
	recnum_t		recnum,
	int			bytes_wanted,
	int			op)
{
	register io_req_t	ior;
	register io_return_t	result;

	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/* XXX note that a CLOSE may proceed at any point */

	/*
	 * There must be a reply port.
	 */
	if (!IP_VALID(reply_port)) {
	    printf("ds_* invalid reply port\n");
	    Debugger("ds_* reply_port");
	    return (MIG_NO_REPLY);	/* no sense in doing anything */
	}

	/*
	 * Package the read for the device driver
	 */
	io_req_alloc(ior);

	ior->io_device		= device;
	ior->io_unit		= device->dev_number;
	ior->io_op		= op;
	ior->io_mode		= mode;
	ior->io_recnum		= recnum;
	ior->io_data		= 0;		/* driver must allocate data */
	ior->io_count		= 
	    ((bytes_wanted < sizeof(io_buf_ptr_inband_t)) ?
		bytes_wanted : sizeof(io_buf_ptr_inband_t));
	ior->io_alloc_size	= 0;		/* no data allocated yet */
	ior->io_residual	= 0;
	ior->io_error		= 0;
	ior->io_done		= ds_read_done;
	ior->io_reply_port	= reply_port;
	ior->io_reply_port_type	= reply_port_type;

	/*
	 * The ior keeps an extra reference for the device.
	 */
	device_reference(device);

	/*
	 * Do the read.
	 */
	result = (*device->dev_ops->d_read)(device->dev_number, ior);

	/*
	 * If the io was queued, delay reply until it is finished.
	 */
	if (result == D_IO_QUEUED) {
	    if (op & IO_QUEUE)
		io_done_queue_request(reply_port);
	    return (MIG_NO_REPLY);
	}

	/*
	 * If a queued request, enqueue the requests 
	 */
	if (op & IO_QUEUE) {
	    	io_done_queue_t	queue = (io_done_queue_t) reply_port;
		spl_t		s;

		s = splio();
	    	ior->io_op |= IO_DONE;
	    	io_done_queue_lock(queue);
	    	enqueue_tail(&queue->io_done_list, (queue_entry_t)ior);
	    	thread_wakeup((int)queue);
	    	io_done_queue_unlock(queue);
		splx(s);
	        return (MIG_NO_REPLY);
	}

	/*
	 * Return result, via ds_read_done.
	 */
	ior->io_error = result;
	(void) ds_read_done(ior);
	io_req_free(ior);

	return (MIG_NO_REPLY);	/* reply has already been sent. */
}

/*
 * Read from a device with overwrite.
 */
io_return_t
ds_device_read_overwrite_common(
	device_t		device,
	ipc_port_t		reply_port,
	mach_msg_type_name_t	reply_port_type,
	dev_mode_t		mode,
	recnum_t		recnum,
	int			bytes_wanted,
	int			op,
	vm_address_t		data,
	unsigned int		*data_count)	/* out */
{
	register io_req_t	ior;
	register io_return_t	result;

#ifdef lint
	*data = *data;
	*data_count = *data_count;
#endif /* lint */

	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/* XXX note that a CLOSE may proceed at any point */

	/*
	 * There must be a reply port.
	 */
	if (!(op & (IO_SYNC|IO_QUEUE)) && !IP_VALID(reply_port)) {
	    printf("ds_* invalid reply port\n");
	    Debugger("ds_* reply_port");
	    return (MIG_NO_REPLY);	/* no sense in doing anything */
	}

	/*
	 * Package the read request for the device driver
	 */
	io_req_alloc(ior);

	ior->io_device		= device;
	ior->io_unit		= device->dev_number;
	ior->io_op		= op;
	ior->io_mode		= mode;
	ior->io_recnum		= recnum;
	ior->io_uaddr		= data;		/* user address to overwrite */
	ior->io_map 		= current_act()->map;
	ior->io_count		= bytes_wanted;
	ior->io_alloc_size	= 0;		/* no data allocated yet */
	ior->io_residual	= 0;
	ior->io_error		= 0;
	ior->io_done		= ds_read_done;
	ior->io_reply_port	= reply_port;
	ior->io_reply_port_type	= reply_port_type;

	/*
	 * The ior keeps an extra reference for the device.
	 */
	device_reference(device);

	/*
	 * And do the read.
	 */
	result = (*device->dev_ops->d_read)(device->dev_number, ior);

	/*
	 * If the IO was queued, delay reply until it is finished.
	 */
	if (result == D_IO_QUEUED) {
		assert(!(op & IO_SYNC));
		if (op & IO_QUEUE)
			io_done_queue_request(reply_port);
		return (MIG_NO_REPLY);
	}

	/*
	 * Return result via ds_read_done.
	 * If there was a problem queueing the IO, report it.
	 * Otherwise, return status of IO.
	 */
	if (result != D_SUCCESS)
		ior->io_error = result;
	else
		result = ior->io_error;

	(void) ds_read_done(ior);
	*data_count = ior->io_total;
	io_req_free(ior);

	/*
	 *	Reply has already been sent if not synchronous.
	 */
	return ((op & IO_SYNC) ? result : MIG_NO_REPLY);
}

/*
 * Allocate wired-down memory for device read with scatter/gather lists.
 */
kern_return_t
device_read_alloc_sg(
	io_req_t		ior,
	vm_size_t		size)
{
	vm_offset_t		addr;
	kern_return_t		kr;
        int                     nentries, ii;
        struct io_sglist        *sgp;
	vm_size_t	        xfer_length;

        xfer_length = size;
	size = round_page(size);
        nentries = size/PAGE_SIZE;
        io_sglist_alloc(sgp, nentries);

        kr = kmem_alloc(kernel_map, &addr, size);
        if (kr != KERN_SUCCESS) {
                io_sglist_free(sgp);
                return (kr);
        }

	ior->io_data = (io_buf_ptr_t) addr;
        ior->io_alloc_size = size;

        /*
         * Fill in the scatter/gather list.
         */
        for (ii = 0; ii < nentries; ii++) {
                sgp->iosg_list[ii].iosge_length = PAGE_SIZE;
                sgp->iosg_list[ii].iosge_phys =
                        pmap_extract(vm_map_pmap(kernel_map), addr);
                addr += PAGE_SIZE;
        }
        sgp->iosg_list[nentries-1].iosge_length -= (size - xfer_length);
        sgp->iosg_hdr.length = xfer_length;
        ior->io_sgp = sgp;
	ior->io_op |= IO_SGLIST;
	return(KERN_SUCCESS);
}

/*
 * Read from a device.
 */
io_return_t
ds_device_read(
	device_t		device,
	ipc_port_t		reply_port,
	mach_msg_type_name_t	reply_port_type,
	dev_mode_t		mode,
	recnum_t		recnum,
	int			bytes_wanted,
	io_buf_ptr_t		*data,		/* out */
	unsigned int		*data_count)	/* out */
{
	return(ds_device_read_common(device, reply_port, reply_port_type,
			      mode, recnum, bytes_wanted,
			      (IO_READ | IO_CALL), data, data_count));
}

/*
 * Read from a device, but return the data 'inband.'
 */
io_return_t
ds_device_read_inband(device, reply_port, reply_port_type, mode, recnum,
		      bytes_wanted, data, data_count)
	register device_t	device;
	ipc_port_t		reply_port;
	mach_msg_type_name_t	reply_port_type;
	dev_mode_t		mode;
	recnum_t		recnum;
	int			bytes_wanted;
	io_buf_ptr_inband_t	data;		/* pointer to OUT array */
	unsigned int		*data_count;	/* out */
{

#ifdef lint
	*data = *data;
	*data_count = *data_count;
#endif /* lint */

	return(ds_device_read_inband_common(device, reply_port,
		reply_port_type, mode, recnum, bytes_wanted,
		(IO_READ | IO_CALL | IO_INBAND)));
}

/*
 * Allocate wired-down memory for device read.
 */
kern_return_t
device_read_alloc(
	io_req_t	ior,
	vm_size_t	size)
{
	vm_offset_t		addr;
	kern_return_t		kr;
	boolean_t		sgio = FALSE;

	/*
	 * Nothing to do if no data.
	 */
	if (ior->io_count == 0) {
	    ior->io_data = NULL;
	    return (KERN_SUCCESS);
	}

	if (ior->io_op & IO_INBAND) {
		ior->io_data = (io_buf_ptr_t) zalloc(io_inband_zone);
		ior->io_alloc_size = sizeof(io_buf_ptr_inband_t);
		return (KERN_SUCCESS);
	}

	/*
	 *	Figure out if this device can do scatter/gather I/O.
	 */
	kr = (*ior->io_device->dev_ops->d_dev_info)(
				ior->io_device->dev_number,
				D_INFO_SGLIST_IO,
				(char *)&sgio);
	if (kr != KERN_SUCCESS)
		sgio = FALSE;
	
	if (ior->io_op & IO_OVERWRITE) {
		/*
		 * XXX  Just punt on scatter/gather for now.  This
		 * XXX  is probably a simple matter of extracting
		 * XXX  the guts of device_read_alloc_sg into a
		 * XXX  routine that converts a page list to an sglist
		 * XXX  and calling it in the appropriate places below.
		 */
		if (sgio)
	panic("Overwrite not supported for scatter/gather devices yet");

		if (ior->io_op & IO_KERNEL_BUF) {

			/*
			 *	Set up kernel buffer for device.
			 */
			kr = vm_map_copyin_page_list(ior->io_map,
				ior->io_uaddr,
				ior->io_count,
				(VM_PROT_READ | VM_PROT_WRITE |
				 VM_MAP_COPYIN_OPT_PMAP_ENTER),
				&ior->io_copy,
				FALSE);

			if (kr != KERN_SUCCESS)
				return (kr);

			/*
			 * XXX	Can't handle continuations!!  Might be able to
			 * XXX	recover by clearing IO_KERNEL_BUF and using
			 * XXX  code below (???).
			 */
			if (vm_map_copy_has_cont(ior->io_copy))
		panic("device_read_alloc: overwrite read too large!!");

			ior->io_data = (io_buf_ptr_t) ior->io_uaddr;
			ior->io_alloc_size = ior->io_count;
		}
		else {

			/*
			 *	Map user pages directly for device.  Pages in a
			 *	page list are wired.  This code does not do any
			 *	splitting of large operations.
			 */
			kr = vm_map_copyin_page_list(ior->io_map,
				ior->io_uaddr,
				ior->io_count,
				VM_PROT_READ|VM_PROT_WRITE,
				&ior->io_copy,
				FALSE);

			if (kr != KERN_SUCCESS)
				return (kr);

			kr = kmem_io_map_copyout(device_io_map,
				(vm_offset_t *)&ior->io_data,
				(vm_size_t *)&ior->io_alloc_size,
				ior->io_copy,
				ior->io_count,
				VM_PROT_READ|VM_PROT_WRITE);

			if (kr != KERN_SUCCESS) {
				vm_map_copy_discard(ior->io_copy);
				return (kr);
			}

log_io_map ("device_read_alloc", ior);

		}
	}

	/*
	 *	This is the !IO_OVERWRITE case.
	 */
	else {
		if (sgio) {
			kr = device_read_alloc_sg(ior, size);
			if (kr != KERN_SUCCESS)
			    return (kr);
		}
		else {
			size = round_page(size);
			kr = kmem_alloc(kernel_map, &addr, size);
			if (kr != KERN_SUCCESS)
				return (kr);
			ior->io_data = (io_buf_ptr_t) addr;
			ior->io_alloc_size = size;
		}
	}

	return (KERN_SUCCESS);
}

boolean_t
ds_read_done(
	io_req_t		ior)
{
	vm_offset_t		start_data, end_data;
	vm_offset_t		start_sent, end_sent;
	register int		size_read;

	if (ior->io_error)
	    size_read = 0;
	else
	    size_read = ior->io_count - ior->io_residual;

	start_data  = (vm_offset_t)ior->io_data;
	end_data    = start_data + size_read;

	start_sent  = (ior->io_op & IO_INBAND) ? start_data :
						trunc_page(start_data);
	end_sent    = (ior->io_op & IO_INBAND) ? 
		start_data + ior->io_alloc_size : round_page(end_data);

	/*
	 * Zero memory that the device did not fill.  Overwrite case
	 * doesn't need to do this because device wrote directly to
	 * target buffer.  Amount of data read is returned to client,
	 * so short read is handled there, not here.
	 */
	if ((ior->io_op & IO_OVERWRITE) == 0) {
		if (start_sent < start_data)
		    bzero((char *)start_sent, start_data - start_sent);
		if (end_sent > end_data)
		    bzero((char *)end_data, end_sent - end_data);
	}

	/*
	 * Touch the data being returned, to mark it dirty.
	 * If the pages were filled by DMA, the pmap module
	 * may think that they are clean.
	 *
	 * XXX This is really stupid!  Device driver should tell
	 * XXX pmap module if it did something like DMA.
	 */
	{
	    register vm_offset_t	touch;
	    register int		c;

	    for (touch = start_sent; touch < end_sent; touch += PAGE_SIZE) {
		c = *(volatile char *)touch;
		*(volatile char *)touch = c;
	    }
	}

	/*
	 * Send the data to the reply port - this
	 * unwires and deallocates it.
	 */
	if (ior->io_op & IO_INBAND && ior->io_op & IO_QUEUE) {
		return(TRUE);
	}
	else if (ior->io_op & IO_INBAND) {
		assert((ior->io_op & IO_SYNC) == 0);
		(void)ds_device_read_reply_inband(ior->io_reply_port,
					      ior->io_reply_port_type,
					      ior->io_error,
					      (char *) start_data,
					      size_read);
	}
	else if (ior->io_op & IO_OVERWRITE) {

		/*
		 *	The clever code in device_read_alloc has arranged
		 *	for the bytes to land exactly where they were wanted.
		 *	In the kernel buf case, no kernel mapping was needed.
		 *	Just have a little clean up left to do here.
		 */

		if ((ior->io_op & IO_KERNEL_BUF) == 0) {

log_io_map ("ds_read_done", ior);

			kmem_io_map_deallocate(device_io_map,
					trunc_page(ior->io_data),
					(vm_size_t) ior->io_alloc_size);
		}

		vm_map_copy_discard(ior->io_copy);

		ior->io_copy = VM_MAP_COPY_NULL;
		ior->io_data = (io_buf_ptr_t) 0;

		if (ior->io_op & (IO_SYNC | IO_QUEUE))
			ior->io_total = size_read;
		else 
			(void)ds_device_read_reply_overwrite(
					ior->io_reply_port,
					ior->io_reply_port_type,
					ior->io_error,
					size_read);

	}
	else {
		vm_map_copy_t copy;
		kern_return_t kr;

		kr = vm_map_copyin(kernel_map, start_data, size_read,
					   TRUE, &copy);

		if (kr != KERN_SUCCESS) {
			ior->io_error = kr;
		}
		else if (ior->io_op & (IO_SYNC | IO_QUEUE)) {
			if (copy != VM_MAP_COPY_NULL) {
				kr = vm_map_copyout(current_map(),
					(vm_offset_t *) (&ior->io_data), copy);
			}
			if (kr != KERN_SUCCESS) {
				ior->io_error = kr;
				ior->io_total = 0;
				vm_map_copy_discard(copy);
			}
			else
				ior->io_total = size_read;
		}
		else {
			(void)ds_device_read_reply(ior->io_reply_port,
						   ior->io_reply_port_type,
						   ior->io_error,
						   (char *) copy,
						   size_read);
		}
	}

	/*
	 * Free any memory that was allocated but not sent.
	 */
	if (ior->io_count != 0) {
		if (ior->io_op & IO_INBAND) {
			if (ior->io_alloc_size > 0)
				zfree(io_inband_zone,
				      (vm_offset_t)ior->io_data);
		} else {
			register vm_offset_t	end_alloc;

			end_alloc = start_sent + round_page(ior->io_alloc_size);
			if (end_alloc > end_sent)
				kmem_free(kernel_map, end_sent,
						     end_alloc - end_sent);
	                if (ior->io_op & IO_SGLIST)
				io_sglist_free(ior->io_sgp);
		}

	}

	device_deallocate(ior->io_device);

	return (TRUE);
}

io_return_t
ds_device_set_status(
	device_t		device,
	int			flavor,
	dev_status_t		status,
	unsigned int		status_count)
{
	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/* XXX note that a CLOSE may proceed at any point */

	return ((*device->dev_ops->d_setstat)(device->dev_number,
					      flavor,
					      status,
					      status_count));
}

io_return_t
ds_device_get_status(
	device_t		device,
	int			flavor,
	dev_status_t		status,		/* pointer to OUT array */
	unsigned int		*status_count)	/* in/out */
{
	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/* XXX note that a CLOSE may proceed at any point */

	return ((*device->dev_ops->d_getstat)(device->dev_number,
					      flavor,
					      status,
					      status_count));
}

io_return_t
ds_device_set_filter(
	device_t		device,
	ipc_port_t		receive_port,
	int			priority,
	filter_t		filter[],	/* pointer to IN array */
	unsigned int		filter_count)
{
	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/* XXX note that a CLOSE may proceed at any point */

	/*
	 * Request is absurd if no receive port is specified.
	 */
	if (!IP_VALID(receive_port))
	    return (D_INVALID_OPERATION);

	return ((*device->dev_ops->d_async_in)(device->dev_number,
					       receive_port,
					       priority,
					       filter,
					       filter_count));
}

io_return_t
ds_device_map(
	device_t		device,
	vm_prot_t		protection,
	vm_offset_t		offset,
	vm_size_t		size,
	ipc_port_t		*pager,	/* out */
	boolean_t		unmap)	/* ? */
{
#ifdef	lint
	unmap = unmap;
#endif	/* lint */
	if (protection & ~VM_PROT_ALL)
		return (KERN_INVALID_ARGUMENT);
	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/* XXX note that a CLOSE may proceed at any point */

	return (device_pager_setup(device, protection, offset, size,
				    pager));
}

/*
 * Doesn't do anything (yet).
 */

io_return_t
ds_no_senders(
	mach_no_senders_notification_t	*notification)
{
	printf("ds_no_senders called! device_port=0x%x count=%d\n",
	       notification->not_header.msgh_remote_port,
	       notification->not_count);
	return (KERN_SUCCESS);
}

boolean_t
ds_notify(
	mach_msg_header_t *msg)
{
	switch (msg->msgh_id) {
		case MACH_NOTIFY_NO_SENDERS:
		ds_no_senders((mach_no_senders_notification_t *) msg);
		return TRUE;

		default:
		printf("ds_notify: strange notification %d\n", msg->msgh_id);
		return FALSE;
	}
}

boolean_t
ds_master_notify(
	mach_msg_header_t *msg)
{
	extern ipc_port_t master_device_port;

	assert(msg->msgh_remote_port == (mach_port_t)master_device_port);

	switch (msg->msgh_id) {
	case MACH_NOTIFY_DEAD_NAME: {
		mach_dead_name_notification_t *m;

		m = (mach_dead_name_notification_t *)msg;
		if (net_unset_filter((ipc_port_t)m->not_port))
			return TRUE;
		break;
	}
	default:
		break;
	}

	printf("ds_master_notify: strange notification %d\n", msg->msgh_id);
	return FALSE;
}

queue_head_t		io_done_list;
spinlock_t		io_done_list_lock;

void
iodone(
	io_req_t	ior)
{
	spl_t		s;
	io_done_queue_t	queue;

	/*
	 * If this ior was loaned to us, return it directly.
	 */
	if (ior->io_op & IO_LOANED) {
		(*ior->io_done)(ior);
		return;
	}
	/*
 	 * If IO_CALL put it on the queue for the io_done thread.
	 * If IO_QUEUE put it on the specified I/O completion queue,
	 * interlock with io_done_queue_wait(), and post a
	 * wakeup on the io_done_queue.  Otherwise, interlock with
	 * io_wait() and post a wakeup on the ior_req.
	 */
	s = splio();
	if ((ior->io_op & IO_QUEUE) != 0 &&
	    (queue = (io_done_queue_t) ior->io_reply_port) !=
	     IO_DONE_QUEUE_NULL) {
log_io_map ("iodone (to port queue)", ior);
	    ior->io_op |= IO_DONE;
	    io_done_queue_lock(queue);
	    enqueue_tail(&queue->io_done_list, (queue_entry_t)ior);
	    queue->io_in_progress--;
	    thread_wakeup((int)queue);
	    io_done_queue_unlock(queue);
	}
	else if ((ior->io_op & (IO_QUEUE|IO_CALL)) == 0) {
/*log_io_map ("iodone (ior-wanted mode)", ior);*/
	    ior_lock(ior);
	    ior->io_op |= IO_DONE;
	    ior->io_op &= ~IO_WANTED;
	    ior_unlock(ior);
	    thread_wakeup((int)ior);
	} else {
/*log_io_map ("iodone (io_done_list)", ior);*/
	    ior->io_op |= IO_DONE;
	    spinlock_lock(&io_done_list_lock);
	    enqueue_tail(&io_done_list, (queue_entry_t)ior);
	    thread_wakeup((int)&io_done_list);
	    spinlock_unlock(&io_done_list_lock);
	}
	splx(s);
}


int	iodone_thread_fixpri = -1;

void
io_done_thread(void)
{
    {
	thread_t			thread;
	processor_set_t			pset;
	kern_return_t			ret;
	policy_base_t			base;
	policy_limit_t			limit;
	policy_fifo_base_data_t		fifo_base;
	policy_fifo_limit_data_t	fifo_limit;

	/*
	 * Set thread privileges.
	 */
	thread = current_thread();
	current_thread()->vm_privilege = TRUE;
	stack_privilege(current_thread());

	/*
	 * Set thread priority and scheduling policy.
	 */
	pset = thread->processor_set;
	base = (policy_base_t) &fifo_base; 
	limit = (policy_limit_t) &fifo_limit;
	if (iodone_thread_fixpri == -1) {
		fifo_base.base_priority = BASEPRI_KERNEL+2; 
		fifo_limit.max_priority = BASEPRI_KERNEL+2;
	} else {
		fifo_base.base_priority = iodone_thread_fixpri;
		fifo_limit.max_priority = iodone_thread_fixpri;
	}
	ret = thread_set_policy(thread->top_act, pset, POLICY_FIFO, 
				base, POLICY_FIFO_BASE_COUNT,
				limit, POLICY_FIFO_LIMIT_COUNT);
	if (ret != KERN_SUCCESS)
		printf("WARNING: io_done_thread is being TIMESHARED!\n");
    }

    for (;;) {
	    spl_t		s;
	    register io_req_t	ior;

	    s = splio();
	    spinlock_lock(&io_done_list_lock);
	    while ((ior = (io_req_t)dequeue_head(&io_done_list)) != 0) {
		spinlock_unlock(&io_done_list_lock);
		(void) splx(s);

		if ((*ior->io_done)(ior)) {
		    /*
		     * IO done - free io_req_elt
		     */
		    io_req_free(ior);
		}
		/* else routine has re-queued it somewhere */

		s = splio();
		spinlock_lock(&io_done_list_lock);
	    }

	    assert_wait((int) &io_done_list, FALSE);
	    spinlock_unlock(&io_done_list_lock);
	    (void) splx(s);
	    counter(c_io_done_thread_block++);
	    thread_block((void (*)(void)) 0);
    }
    /*NOTREACHED*/
}

#define	DEVICE_IO_MAP_SIZE	(2 * 1024 * 1024)

void
ds_init(void)
{
	kern_return_t	retval;
	vm_offset_t	device_io_min, device_io_max;

	queue_init(&io_done_list);
	spinlock_init(&io_done_list_lock);

	retval = kmem_suballoc(kernel_map,
			       &device_io_min,
			       DEVICE_IO_MAP_SIZE,
			       FALSE,
			       TRUE,
			       &device_io_map);
	if (retval != KERN_SUCCESS)
		panic("ds_init: kmem_suballoc failed");
	device_io_max = device_io_min + round_page(DEVICE_IO_MAP_SIZE);

	/*
	 *	If the kernel receives many device_write requests, the
	 *	device_io_map might run out of space.  To prevent
	 *	device_write_get from failing in this case, we enable
	 *	wait_for_space on the map.  This causes kmem_io_map_copyout
	 *	to block until there is sufficient space.
	 *	(XXX Large writes may be starved by small writes.)
	 *
	 *	There is a potential deadlock problem with this solution,
	 *	if a device_write from the default pager has to wait
	 *	for the completion of a device_write which needs to wait
	 *	for memory allocation.  Hence, once device_write_get
	 *	allocates space in device_io_map, no blocking memory
	 *	allocations should happen until device_write_dealloc
	 *	frees the space.  (XXX A large write might starve
	 *	a small write from the default pager.)
	 */
	device_io_map->wait_for_space = TRUE;

	io_inband_zone = zinit(sizeof(io_buf_ptr_inband_t),
			    1000 * sizeof(io_buf_ptr_inband_t),
			    10 * sizeof(io_buf_ptr_inband_t),
			    "io inband buffers");
}

void
iowait(
	io_req_t	ior)
{
    spl_t s;

    s = splio();
    ior_lock(ior);
    while ((ior->io_op&IO_DONE)==0) {
	assert_wait((int)ior, FALSE);
	ior_unlock(ior);
	thread_block((void (*)(void)) 0);
        ior_lock(ior);
    }
    ior_unlock(ior);
    splx(s);
}

/*
 * Allocate an I/O completion queue kernel object.
 */
io_return_t
ds_io_done_queue_create(
	host_t		host,
	io_done_queue_t	*io_done_queue)
{
	io_done_queue_t 	queue;

	if (host == HOST_NULL)
		return(KERN_INVALID_ARGUMENT);

	/*
	 * Allocate the I/O completion queue header
	 */
	queue = (io_done_queue_t) kalloc(sizeof(struct io_done_queue));
	if (queue == IO_DONE_QUEUE_NULL)
		return(KERN_RESOURCE_SHORTAGE);

	/*
	 * Allocate the associated kernel port and register the object
	 */
	queue->port = ipc_port_alloc_kernel();
	if (queue->port == IP_NULL) {
		kfree((vm_offset_t) queue, sizeof(struct io_done_queue));
		return(KERN_RESOURCE_SHORTAGE);
	}
	ipc_kobject_set(queue->port, (ipc_kobject_t) queue, IKOT_IO_DONE_QUEUE);

	/*
	 * Initialize the remaining header elements
	 */
	spinlock_init(&queue->ref_lock);
	spinlock_init(&queue->lock);
	queue_init(&queue->io_done_list);
	queue->ref_count = 1;
	queue->io_in_progress = 0;

	*io_done_queue = queue;

	return(KERN_SUCCESS);
}

/*
 * Deallocate an I/O completion queue
 */
io_return_t
ds_io_done_queue_terminate(
	io_done_queue_t queue)
{
	spl_t		s;
	io_req_t	ior;

	/*
	 * Remove port-to-queue mapping, preventing any additional
	 * asynchronous requests on this queue.
	 */
	ipc_kobject_set(queue->port, IKO_NULL, IKOT_NONE);
	ipc_port_dealloc_kernel(queue->port);
	io_done_queue_deallocate(queue);	

	/*
	 * Wakeup any waiters
	 */
	s = splio();
	io_done_queue_lock(queue);
	thread_wakeup((int) queue);
	io_done_queue_unlock(queue);	
	splx(s);

	/*
	 * Clean up any completed requests and wait for any pending ones.
	 * At this point queue->ref_count cannot be incremented since
	 * the port-to-queue mapping is gone.
	 */
	for (;;) {
		s = splio();
		io_done_queue_lock(queue);
		while ((ior = (io_req_t) dequeue_head(&queue->io_done_list)) !=
								(io_req_t) 0) {
			io_done_queue_unlock(queue);	
			splx(s);

			assert((ior->io_op & IO_QUEUE) != 0);

			if ((*ior->io_done)(ior)) {

				/*	
		 	 	 * Free the ior and drop the reference on the 
		 	 	 * queue held by the pending ior
		 	 	 */
				io_req_free(ior);
				io_done_queue_deallocate(queue);
			}

			s = splio();
			io_done_queue_lock(queue);
		}

		/*
		 * Wait for pending ior's
		 */
		if (queue->io_in_progress > 0) {
			log_thread_action("ds_io_done_queue_terminate",
					(long)queue, 0, 0);
			assert_wait((int) queue, TRUE);
			io_done_queue_unlock(queue);	
			splx(s);
			thread_block((void (*)(void)) 0);
		}
		else {
			assert(queue->ref_count == 1);
			break;
		}
	}

	/*
	 * Drop the refererence held by the caller
	 */
	io_done_queue_deallocate(queue);	

	return(KERN_SUCCESS);
}

/*
 * Remove a reference to an I/O completion queue, and deallocate the
 * structure if no references are left.
 */
void
io_done_queue_deallocate(
	io_done_queue_t		queue)
{

	if (queue == IO_DONE_QUEUE_NULL)
		return;

	spinlock_lock(&queue->ref_lock);
	if (--queue->ref_count > 0) {
		spinlock_unlock(&queue->ref_lock);
		return;
	}

	/*
	 * Free the I/O completion queue
	 */
	kfree((vm_offset_t) queue, sizeof(struct io_done_queue));
}

/*
 * Wait on an I/O completion queue.  Completions are processed serially.
 */
io_return_t
ds_io_done_queue_wait(
	io_done_queue_t		queue,
	io_done_result_t	*result,
	io_done_result_t	*ures)
{
	spl_t			s;
	io_req_t		ior;

	for (;;) {
		s = splio();
		io_done_queue_lock(queue);

		/*
	 	 * Check that the queue still exists.  The object may
	 	 * have been terminated.
	 	 */
		if (queue->port == IP_NULL) {
			io_done_queue_unlock(queue);	
			splx(s);
			return(KERN_TERMINATED);
		}

		/*
	 	 * Check queue for completed I/O requests
	 	 */
		while ((ior = (io_req_t) dequeue_head(&queue->io_done_list)) ==
								(io_req_t) 0) {

			/*
		 	 * Nothing pending, block
		 	 */

			log_thread_action("ds_io_done_queue_wait",
					(long)queue, 0, 0);

			assert_wait((int) queue, TRUE);
			io_done_queue_unlock(queue);	
			splx(s);
			thread_block((void (*)(void)) 0);

			/*
	 	 	 * Check that the queue still exists.  The wakeup
			 * may be because the object has been terminated.
	 	 	 */
			if (queue->port == IP_NULL)
				return(KERN_TERMINATED);

			s = splio();
			io_done_queue_lock(queue);
		}

		io_done_queue_unlock(queue);	
		splx(s);

		assert((ior->io_op & IO_QUEUE) != 0);

		/*
	 	 * We have a completion.  Invoke the completion
	 	 * handler.  Otherwise the request has been queued
	 	 * again and will complete later.
	 	 */
		if ((*ior->io_done)(ior)) {
			kern_return_t ret = KERN_SUCCESS;

			/*
		 	 * Marshall the results for the user
		 	 */	
			result->qd_reqid = (mach_port_t)ior->io_reply_port_type;
			result->qd_code = ior->io_error;

			if (ior->io_op & IO_READ) {		/* read */
				int	err;

				result->qd_type = IO_DONE_READ;
				if (ior->io_op & IO_INBAND) {
					result->qd_count = ior->io_count - 
							ior->io_residual;
					ret = copyout(ior->io_data, 
						   (char *) &ures->qd_inline,
						   result->qd_count);
					if (ior->io_alloc_size > 0)
					    zfree(io_inband_zone, 
						  (vm_offset_t)ior->io_data);
					if (ret)
					    ret = KERN_INVALID_ADDRESS;
					result->qd_data = 
					    (io_buf_ptr_t )&ures->qd_inline;
				}
				else if (ior->io_op & IO_OVERWRITE) {
					result->qd_count = ior->io_total;
					result->qd_type = IO_DONE_OVERWRITE;
				}
				else {
					result->qd_count = ior->io_total;
					result->qd_type = IO_DONE_READ;
					result->qd_data = ior->io_data;
				}
			}
			else {					/* write */
				result->qd_type = IO_DONE_WRITE;
				result->qd_count = ior->io_total - 
							ior->io_residual;
			}

			/*	
		 	 * Free the ior and drop the reference on the 
		 	 * queue held by the pending ior
		 	 */
			io_req_free(ior);
			io_done_queue_deallocate(queue);

			return(ret);
		}
	}
}

#if     MACH_KDB
#include <ddb/db_output.h>
#define printf	db_printf

/* Forward */

void db_io_sglist_hdr_print(
	struct io_sglist_hdr	*iogh);

void db_io_sg_entry_print(
	struct io_sg_entry	*ioge);

void db_io_sglist_print(
	io_sglist_t	iog);

void db_io_req_print(
	io_req_t	ior);

/*
 * Print a device IO scatter/gather header.
 */
void
db_io_sglist_hdr_print(
	struct io_sglist_hdr	*iogh)
{
	iprintf("length = %d, nentries = %d", iogh->length, iogh->nentries); 
}

/*
 * Print a device IO scatter/gather entry.
 */
void
db_io_sg_entry_print(
	struct io_sg_entry	*ioge)
{
	iprintf("phy_addr = 0x%x, length = %d",
		ioge->iosge_phys, ioge->iosge_length); 
}

/*
 * Print a device IO scatter/gather list.
 */
void db_io_sglist_print(
	io_sglist_t	iog)
{
	unsigned i;

	printf("io_sglist 0x%x\n", iog);
	indent += 2;
	db_io_sglist_hdr_print(&iog->iosg_hdr);
	indent += 2;
	for (i = 0; i < iog->iosg_hdr.nentries; i++) {
		if (i % 3) {
			printf("\n");
			iprintf("%d = (", i);
		} else
			printf(" %d = (", i);
		db_io_sg_entry_print(&iog->iosg_list[i]);
		printf(")");
	}
	indent -= 4;
}

/*
 * Print a device IO request.
 */
void
db_io_req_print(
	io_req_t	ior)
{
	char		*s;

	printf("ioreq 0x%x\n", ior);
	indent += 2;
	
	iprintf("next = 0x%x, prev = 0x%x, device = 0x%x, dev_ptr= 0x%x\n",
	       ior->io_next, ior->io_prev, ior->io_device, ior->io_dev_ptr);

	iprintf("unit = %d, recnum = %d, mode = 0x%x",
	       ior->io_unit, ior->io_recnum, ior->io_mode);
	if (ior->io_mode & D_READ) {
		printf(" <READ");
		s = ",";
	} else
		s = " <";
	if (ior->io_mode & D_WRITE) {
		printf("%sWRITE", s);
		s = ",";
	}
	if (ior->io_mode & D_NODELAY) {
		printf("%sNODELAY", s);
		s = ",";
	}
	if (ior->io_mode & D_NOWAIT)
		printf("%sNOWAIT>", s);
	else if (*s == ',')
		printf(">");
	printf("\n");

	iprintf("op = 0x%x <", ior->io_op);
	switch (ior->io_op & (IO_READ|IO_OPEN)) {
	case IO_WRITE:
		printf("WRITE");
		break;
	case IO_READ:
		printf("READ");
		break;
	case IO_OPEN:
		printf("OPEN");
		break;
	}
	if (ior->io_op & IO_DONE)
		printf(",DONE");
	if (ior->io_op & IO_ERROR)
		printf(",ERROR");
	if (ior->io_op & IO_BUSY)
		printf(",BUSY");
	if (ior->io_op & IO_WANTED)
		printf(",WANTED");
	if (ior->io_op & IO_BAD)
		printf(",BAD");
	if (ior->io_op & IO_CALL)
		printf(",CALL");
	if (ior->io_op & IO_INBAND)
		printf(",INBAND");
	if (ior->io_op & IO_INTERNAL)
		printf(",INTERNAL");
	if (ior->io_op & IO_LOANED)
		printf(",LOANED");
	if (ior->io_op & IO_SGLIST)
		printf(",SGLIST");
	if (ior->io_op & IO_SYNC)
		printf(",SYNC");
	if (ior->io_op & IO_OVERWRITE)
		printf(",OVERWRITE");
	printf(">\n");

	iprintf("data = 0x%x, sglist = 0x%x, uaddr = 0x%x, map = 0x%x\n",
	       ior->io_data, ior->io_sgp, ior->io_uaddr, ior->io_map);

	iprintf("count = %d, alloc_size = %d, residual = %d, error = 0x%x\n",
	       ior->io_count, ior->io_alloc_size,
	       ior->io_residual, ior->io_error);

	iprintf("io_done = 0x%x, reply_port = 0x%x, reply_port_type = 0x%x\n",
	       ior->io_done, ior->io_reply_port, ior->io_reply_port_type);

	iprintf("link = 0x%x, rlink = 0x%x, io_copy = 0x%x, io_total = %d\n",
	       ior->io_link, ior->io_rlink, ior->io_copy, ior->io_total);
	indent -= 2;
}
#endif	/* MACH_KDB */
