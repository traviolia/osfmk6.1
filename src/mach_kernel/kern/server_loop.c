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
 * Revision 2.6  91/05/18  14:33:22  rpd
 * 	Made max_size an argument to the loop.
 * 	Changed from kmem_alloc_wired to kalloc.
 * 	[91/04/04            rpd]
 * 
 * Revision 2.5  91/05/14  16:46:48  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/03/16  14:51:36  rpd
 * 	Updated for new kmem_alloc interface.
 * 	[91/03/03            rpd]
 * 
 * Revision 2.3  91/02/05  17:29:13  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:17:32  mrt]
 * 
 * Revision 2.2  90/06/02  14:56:08  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  22:18:09  rpd]
 * 
 * Revision 2.1  89/08/03  15:52:01  rwd
 * Created.
 * 
 * Revision 2.4  89/01/10  23:31:54  rpd
 * 	Changed to require use of LOCAL_PORT to specify a port set.
 * 	Changed xxx_port_enable to port_set_add.
 * 	[89/01/10  13:33:38  rpd]
 * 
 * Revision 2.3  88/10/18  03:36:35  mwyoung
 * 	Allow the local port (on which a message is to be received) to
 * 	be redefined by this module's client.
 * 	[88/10/01            mwyoung]
 * 
 * Revision 2.2  88/07/23  01:21:04  rpd
 * Changed port_enable to xxx_port_enable.
 * 
 * 11-Jan-88  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Corrected error in timeout handling.
 *
 * 15-Dec-87  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Created.
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
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
 *	File:	kern/server_loop.c
 *
 *	A common server loop for builtin tasks.
 */

/*
 *	Must define symbols for:
 *		SERVER_NAME		String name of this module
 *		SERVER_LOOP		Routine name for the loop
 *		SERVER_DISPATCH		MiG function(s) to handle message
 *
 *	Must redefine symbols for pager_server functions.
 */

#include <mach/port.h>
#include <mach/message.h>
#include <vm/vm_kern.h>		/* for kernel_map */

void SERVER_LOOP(rcv_set, max_size, trailer_mask)
mach_msg_options_t trailer_mask;
{
	register mach_msg_header_t *in_msg;
	register mach_msg_header_t *out_msg;
	register mach_msg_header_t *tmp_msg;
	vm_offset_t messages;
	mach_msg_return_t r;

	/*
	 *	Allocate our message buffers.
	 */

	messages = kalloc(2 * (max_size + MAX_TRAILER_SIZE));
	if (messages == 0)
		panic(SERVER_NAME);
	in_msg = (mach_msg_header_t *) messages;
	out_msg = (mach_msg_header_t *) (messages + max_size + 
		MAX_TRAILER_SIZE);

	/*
	 *	Service loop... receive messages and process them.
	 */

	for (;;) {
		/* receive first message */

	    receive_msg:
		r = mach_msg_overwrite(in_msg, MACH_RCV_MSG|trailer_mask, 
			     0, max_size, rcv_set,
			     MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL,
			     (mach_msg_header_t *) 0, 0);
		if (r == MACH_MSG_SUCCESS)
			break;

		printf("%s: receive failed, 0x%x.\n", SERVER_NAME, r);
	}

	for (;;) {
		/* process request message */

		(void) SERVER_DISPATCH(in_msg, out_msg);

		/* send reply and receive next request */

		if (out_msg->msgh_remote_port == MACH_PORT_NULL ||
		    (!(out_msg->msgh_bits & MACH_MSGH_BITS_COMPLEX) &&
		     ((mig_reply_error_t *)out_msg)->RetCode == MIG_NO_REPLY))
			goto receive_msg;

		r = mach_msg_overwrite(out_msg, 
			     MACH_SEND_MSG|MACH_RCV_MSG|trailer_mask,
			     out_msg->msgh_size, max_size, rcv_set,
			     MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL,
			     (mach_msg_header_t *) 0, 0);
		if (r != MACH_MSG_SUCCESS) {
			printf("%s: send/receive failed, 0x%x.\n",
			       SERVER_NAME, r);
			goto receive_msg;
		}

		/* swap message buffers */

		tmp_msg = in_msg; in_msg = out_msg; out_msg = tmp_msg;
	}
}
