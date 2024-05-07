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
 * Revision 2.5  91/05/14  15:48:37  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:09:44  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  17:29:39  mrt]
 * 
 * Revision 2.3  90/09/09  14:31:33  rpd
 * 	Use decl_simple_lock_data.
 * 	[90/08/30            rpd]
 * 
 * Revision 2.2  90/01/11  11:42:08  dbg
 * 	Add locking.
 * 	[89/11/27            dbg]
 * 
 * Revision 2.1  89/08/03  15:28:52  rwd
 * Created.
 * 
 * 13-Mar-89  David Golub (dbg) at Carnegie-Mellon University
 *	Added packet filter.
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
 *	Taken from (bsd)net/if.h.  Modified for MACH kernel.
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)if.h	7.3 (Berkeley) 6/27/88
 */

#ifndef	_IF_HDR_
#define	_IF_HDR_

#include <kern/lock.h>
#include <kern/queue.h>

/*
 * Queue for network output and filter input.
 */
struct ifqueue {
	queue_head_t	ifq_head;	/* queue of io_req_t */
	unsigned	ifq_len;	/* length of queue */
	unsigned	ifq_maxlen;	/* maximum length of queue */
	unsigned	ifq_drops;	/* number of packets dropped
					   because queue full */
	queue_entry_t	ifq_wait;	/* first entry for which
					   a thread is waiting */
	spinlock_t	ifq_lock;	/* lock for queue and counters */
};

/*
 * Header for network interface drivers.
 */
struct ifnet {
	queue_chain_t if_chain;		/* list of interfaces with filters */
	short	if_unit;		/* unit number */
	short	if_flags;		/* up/down, broadcast, etc. */
	short	if_timer;		/* time until if_watchdog called */
	short	if_mtu;			/* maximum transmission unit */
	short	if_header_size;		/* length of header */
	short	if_header_format;	/* format of hardware header */
	short	if_address_size;	/* length of hardware address */
	short	if_alloc_size;		/* size of read buffer to allocate */
	char	*if_address;		/* pointer to hardware address */
	struct	ifqueue if_snd;		/* output queue */
	queue_head_t if_rcv_port_list;	/* input filter list */
	mutex_t	if_rcv_port_list_lock;	/* lock for filter list */

/* statistics */
	int	if_ipackets;		/* packets received */
	int	if_ierrors;		/* input errors */
	int	if_opackets;		/* packets sent */
	int	if_oerrors;		/* output errors */
	int	if_collisions;		/* collisions on csma interfaces */
	int	if_rcvdrops;		/* packets received but dropped */
};

#define	IFF_UP		0x0001		/* interface is up */
#define	IFF_BROADCAST	0x0002		/* interface can broadcast */
#define	IFF_DEBUG	0x0004		/* turn on debugging */
#define	IFF_LOOPBACK	0x0008		/* is a loopback net */
#define	IFF_POINTOPOINT	0x0010		/* point-to-point link */
#define	IFF_RUNNING	0x0040		/* resources allocated */
#define	IFF_NOARP	0x0080		/* no address resolution protocol */
#define	IFF_PROMISC	0x0100		/* receive all packets */
#define	IFF_MULTICAST	0x0200		/* support multicast addressing */
#define	IFF_BRIDGE	0x0400		/* support token ring routing field */
#define	IFF_SNAP	0x0800		/* support extended sap header */
#define	IFF_SG		0x1000		/* support scatter-gather messages */
#define	IFF_ALLMULTI	0x2000		/* receive all multicast packets */

/* internal flags only: */
#define	IFF_CANTCHANGE	(IFF_BROADCAST | IFF_POINTOPOINT | IFF_RUNNING | \
			 IFF_MULTICAST)

/*
 * Output queues (ifp->if_snd)
 * have queues of messages stored on ifqueue structures.  Entries
 * are added to and deleted from these structures by these macros, which
 * should be called with ipl raised to splimp().
 * XXX locking XXX
 */

#define	IF_QFULL(ifq)		((ifq)->ifq_maxlen > 0 && \
				 (ifq)->ifq_len >= (ifq)->ifq_maxlen)
#define	IF_DROP(ifq)		((ifq)->ifq_drops++)
#define	IF_ENQUEUE(ifq, ior) {					\
	spinlock_lock(&(ifq)->ifq_lock);			\
	enqueue_tail(&(ifq)->ifq_head, (queue_entry_t)ior);	\
	(ifq)->ifq_len++;					\
	spinlock_unlock(&(ifq)->ifq_lock);			\
}

#define	IF_ENQUEUE_WAIT(ifq, ior) { 				\
	spinlock_lock(&(ifq)->ifq_lock); 			\
	enqueue_tail(&(ifq)->ifq_head, (queue_entry_t)ior); 	\
	(ifq)->ifq_len++; 					\
	if ((ior->io_op & IO_SYNC) == 0) 			\
	    while (IF_QFULL(ifq)) { 				\
		if ((ifq)->ifq_wait == (queue_entry_t)0) 	\
		    (ifq)->ifq_wait = (queue_entry_t)(ior); 	\
		thread_sleep_spinlock((int)(ior), &(ifq)->ifq_lock, FALSE); \
		spinlock_lock(&(ifq)->ifq_lock); 		\
	    } 							\
	spinlock_unlock(&(ifq)->ifq_lock); 			\
}

#define	IF_PREPEND(ifq, ior) {					\
	spinlock_lock(&(ifq)->ifq_lock);			\
	enqueue_head(&(ifq)->ifq_head, (queue_entry_t)ior);	\
	(ifq)->ifq_len++;					\
	spinlock_unlock(&(ifq)->ifq_lock);			\
}

#define	IF_DEQUEUE(ifq, ior) { 					\
	spinlock_lock(&(ifq)->ifq_lock); 			\
	if (((ior) = (io_req_t)dequeue_head(&(ifq)->ifq_head)) != 0) { \
	    (ifq)->ifq_len--;					\
	    if ((ifq)->ifq_wait != (queue_entry_t)0) { 		\
		assert((ifq)->ifq_maxlen > 0); 			\
		if ((ifq)->ifq_len < (ifq)->ifq_maxlen) { 	\
		    thread_wakeup((int)(ifq)->ifq_wait); 	\
		    (ifq)->ifq_wait = queue_next((ifq)->ifq_wait); \
		    if (queue_end(&(ifq)->ifq_head, (ifq)->ifq_wait)) \
			(ifq)->ifq_wait = (queue_entry_t)0; 	\
		} 						\
	    } 							\
	}							\
	spinlock_unlock(&(ifq)->ifq_lock); 			\
}

#define	IFQ_MAXLEN	50

#define	IFQ_INIT(ifq) {				\
	queue_init(&(ifq)->ifq_head);		\
	spinlock_init(&(ifq)->ifq_lock);	\
	(ifq)->ifq_len = 0;			\
	(ifq)->ifq_maxlen = IFQ_MAXLEN;		\
	(ifq)->ifq_drops = 0;			\
	(ifq)->ifq_wait = (queue_entry_t)0; 	\
}

#define	IFNET_SLOWHZ	1		/* granularity is 1 second */

#endif	/* _IF_HDR_ */
