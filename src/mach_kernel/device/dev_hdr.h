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
 * Revision 2.7  91/05/14  15:40:30  mrt
 * 	Correcting copyright
 * 
 * Revision 2.6  91/02/05  17:08:20  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  17:27:12  mrt]
 * 
 * Revision 2.5  90/09/09  14:31:08  rpd
 * 	Use decl_simple_lock_data.
 * 	[90/08/30            rpd]
 * 
 * Revision 2.4  90/08/27  21:54:45  dbg
 * 	Fix type definitions.
 * 	[90/07/16            dbg]
 * 
 * Revision 2.3  90/06/02  14:47:10  rpd
 * 	Updated for new IPC.
 * 	[90/03/26  21:43:28  rpd]
 * 
 * Revision 2.2  89/09/08  11:23:07  dbg
 * 	Rename to 'struct device' and 'device_t'.  Added open-
 * 	state.  Removed most of old flags.
 * 	[89/08/01            dbg]
 * 
 * 12-Apr-89  David Golub (dbg) at Carnegie-Mellon University
 *	Added routine to call a function on each device.
 *
 *  3-Mar-89  David Golub (dbg) at Carnegie-Mellon University
 *	Created.
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
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date: 	3/89
 */

#ifndef	_DEVICE_DEV_HDR_H_
#define	_DEVICE_DEV_HDR_H_

#include <sys/types.h>
#include <mach/port.h>
#include <kern/lock.h>
#include <kern/queue.h>
#include <device/device_typedefs.h>

/*
 * Generic device header.  May be allocated with the device,
 * or built when the device is opened.
 */
struct device {
	mutex_t		ref_lock;	/* lock for reference count */
	int		ref_count;	/* reference count */
	mutex_t		lock;		/* lock for rest of state */
	short		state;		/* state: */
#define	DEV_STATE_INIT		0	/* not open  */
#define	DEV_STATE_OPENING	1	/* being opened */
#define	DEV_STATE_OPEN		2	/* open */
#define	DEV_STATE_CLOSING	3	/* being closed */
	short		flag;		/* random flags: */
#define	D_EXCL_OPEN		0x0001	/* open only once */
	short		open_count;	/* number of times open */
	short		io_in_progress;	/* number of IOs in progress */
	boolean_t	io_wait;	/* someone waiting for IO to finish */

	struct ipc_port *port;		/* open port */
	queue_chain_t	number_chain;	/* chain for lookup by number */
	dev_t		dev_number;	/* device number */
	struct dev_ops	*dev_ops;	/* and operations vector */
};

#define	DEVICE_NULL	((device_t)0)

/*
 * I/O completion queue kernel object.
 */
struct io_done_queue {
	spinlock_t	ref_lock;	/* lock for reference count */
	int		ref_count;	/* reference count */
	spinlock_t	lock;		/* lock for other fields */
	struct ipc_port *port;		/* associated kernel port */
	queue_head_t	io_done_list;	/* list of completed I/O requests */
	int		io_in_progress;	/* pending I/O requests */
};

#define	IO_DONE_QUEUE_NULL	((io_done_queue_t)0)

/*
 * To find and remove device entries
 */
extern device_t		device_lookup(	/* by name */
					char		* name);
extern void		device_reference(
					device_t	device);
extern void		device_deallocate(
					device_t	device);

/*
 * To find and remove port-to-device mappings
 */
extern device_t		dev_port_lookup(
					ipc_port_t	port);
extern void		dev_port_enter(
					device_t	device);
extern void		dev_port_remove(
					device_t	device);

/*
 * To call a routine on each device
 */
extern boolean_t	dev_map(
					boolean_t	(*routine)(
						device_t, ipc_port_t),
					ipc_port_t	port);

/*
 * To find port-to-io_done_queue mappings
 */
extern io_done_queue_t	io_done_queue_port_lookup(
					ipc_port_t	port);
extern void		io_done_queue_reference(
					io_done_queue_t	queue);


extern void		dev_lookup_init(void);

/*
 * To lock and unlock device state and open-count
 */
#define	device_lock(device)	mutex_lock(&(device)->lock)
#define	device_unlock(device)	mutex_unlock(&(device)->lock)

/*
 * To lock and unlock I/O completion queue
 */
#define	io_done_queue_lock(queue)	spinlock_lock(&(queue)->lock)
#define	io_done_queue_unlock(queue)	spinlock_unlock(&(queue)->lock)

#endif	/* _DEVICE_DEV_HDR_H_ */
