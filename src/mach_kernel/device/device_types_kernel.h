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
 * Revision 2.6  91/06/25  10:26:46  rpd
 * 	Changed the convert_foo_to_bar functions
 * 	to use ipc_port_t instead of mach_port_t.
 * 	[91/05/27            rpd]
 * 
 * Revision 2.5  91/05/14  15:43:37  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:09:18  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  17:28:48  mrt]
 * 
 * Revision 2.3  90/06/02  14:47:56  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  21:54:19  rpd]
 * 
 * Revision 2.2  89/09/08  11:24:03  dbg
 * 	Created.
 * 	[89/08/01            dbg]
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
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date: 	8/89
 */

#ifndef	_DEVICE_DEVICE_TYPES_KERNEL_H_
#define	_DEVICE_DEVICE_TYPES_KERNEL_H_

/*
 * Kernel-only type definitions for device server.
 */

#include <mach/port.h>
#include <device/dev_hdr.h>

extern device_t		dev_port_lookup(
					ipc_port_t	port);
extern struct ipc_port	*convert_device_to_port(
					device_t	dev);
extern io_done_queue_t	io_done_queue_port_lookup(
					ipc_port_t	port);
extern struct ipc_port	*convert_io_done_queue_to_port(
					io_done_queue_t	queue);

#endif	/* _DEVICE_DEVICE_TYPES_KERNEL_H_ */
