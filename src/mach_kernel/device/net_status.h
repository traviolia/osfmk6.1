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
 * Revision 2.6.3.2  92/03/28  10:04:57  jeffreyh
 * 	Pick up changes from TRUNK
 * 	[92/03/11            jeffreyh]
 * 
 * Revision 2.8  92/02/25  15:34:48  elf
 * 	Add support for ux, NET_DSTADDR, to say whether it is ok
 * 	to establish a point to point for this interface.
 * 	[92/02/21            rvb]
 * 
 * Revision 2.7  92/01/03  20:04:18  dbg
 * 	Add: NETF_PUSHHDR, NETF_PUSHSTK, NETF_PUSHIND, NETF_PUSHHDRIND.
 * 	[91/12/23            dbg]
 * 
 * Revision 2.6  91/05/14  16:00:25  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/02/05  17:10:08  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  17:30:26  mrt]
 * 
 * Revision 2.4  90/09/09  23:20:05  rpd
 * 	Added mapped_size info for mapped interfaces.
 * 	[90/08/30  17:41:27  af]
 * 
 * Revision 2.3  90/08/27  21:55:30  dbg
 * 	Allow more low-priority filters.  Fix from Dan Julin.
 * 	[90/08/27            dbg]
 * 
 * Revision 2.2  90/06/02  14:48:24  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  21:58:55  rpd]
 * 
 * Revision 2.1  89/08/03  15:29:01  rwd
 * Created.
 * 
 * 14-Mar-89  David Golub (dbg) at Carnegie-Mellon University
 *	Created.
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
 *	Date:	3/89
 *
 * 	Status information for network interfaces.
 */

#ifndef	_DEVICE_NET_STATUS_H_
#define	_DEVICE_NET_STATUS_H_

#include <device/device_types.h>
#include <mach/message.h>
#include <mach/ndr.h>

/*
 * General interface status
 */
struct net_status {
	int	min_packet_size;	/* minimum size, including header */
	int	max_packet_size;	/* maximum size, including header */
	int	header_format;		/* format of network header */
	int	header_size;		/* size of network header */
	int	address_size;		/* size of network address */
	int	flags;			/* interface status */
	int	mapped_size;		/* if mappable, virtual mem needed */
	int	max_queue_size;		/* maximum size of output queue */
};
#define	NET_STATUS_COUNT	(sizeof(struct net_status)/sizeof(int))
#define	NET_STATUS		(('n'<<16) + 1)

/*
 * Header formats, as given by RFC 826/1010 for ARP:
 */
#define	HDR_ETHERNET		1	/* Ethernet hardware address */
#define	HDR_EXP_ETHERNET	2	/* 3Mhz experimental Ethernet
					   hardware address */
#define	HDR_PRO_NET		4	/* Proteon ProNET Token Ring */
#define	HDR_CHAOS		5	/* Chaosnet */
#define	HDR_802			6	/* IEEE 802 networks */


/*
 * A network address is an array of bytes.  In order to return
 * this in an array of (long) integers, it is returned in net order.
 * Use 'ntohl' on each element of the array to retrieve the original
 * ordering.
 */
#define	NET_ADDRESS		(('n'<<16) + 2)

#define	NET_DSTADDR		(('n'<<16) + 3)

/*
 * A multicast address is a variable length array split into 2 elements of the
 * same multicast address size. They are the range (first and last) multicast
 * addresses that it is required to add or to delete in the interface list of
 * allowed multicast addresses.
 */
#define NET_ADDMULTI		(('n'<<16) + 4)

#define NET_DELMULTI		(('n'<<16) + 5)

/*
 * Input packet filter definition
 */
#define	NET_MAX_FILTER		64
#define	NET_FILTER_STACK_DEPTH	32

/*
 *  We allow specification of up to NET_MAX_FILTER (short) words of a filter
 *  command list to be applied to incoming packets to determine if
 *  those packets should be given to a particular network input filter.
 *  
 *  Each network filter specifies the filter command list via net_add_filter.
 *  Each filter command list specifies a sequences of actions which leave a
 *  boolean value on the top of an internal stack.  Each word of the
 *  command list specifies an action from the set {PUSHLIT, PUSHZERO,
 *  PUSHWORD+N} which respectively push the next word of the filter, zero,
 *  or word N of the incoming packet on the stack, and a binary operator
 *  from the set {EQ, LT, LE, GT, GE, AND, OR, XOR} which operates on the
 *  top two elements of the stack and replaces them with its result.  The
 *  special action NOPUSH and the special operator NOP can be used to only
 *  perform the binary operation or to only push a value on the stack.
 *  
 *  If the final value of the filter operation is true, then the packet is
 *  accepted for the filter.
 *  
 */

typedef	unsigned short	filter_t;
typedef filter_t	*filter_array_t;


/*  these must sum to 16!  */
#define NETF_NBPA	10			/* # bits / argument */
#define NETF_NBPO	6			/* # bits / operator */

#define	NETF_ARG(word)	((word) & 0x3ff)
#define	NETF_OP(word)	(((word)>>NETF_NBPA)&0x3f)

/*  binary operators  */
#define NETF_NOP	(0<<NETF_NBPA)
#define NETF_EQ		(1<<NETF_NBPA)
#define NETF_LT		(2<<NETF_NBPA)
#define NETF_LE		(3<<NETF_NBPA)
#define NETF_GT		(4<<NETF_NBPA)
#define NETF_GE		(5<<NETF_NBPA)
#define NETF_AND	(6<<NETF_NBPA)
#define NETF_OR		(7<<NETF_NBPA)
#define NETF_XOR	(8<<NETF_NBPA)
#define NETF_COR	(9<<NETF_NBPA)
#define NETF_CAND	(10<<NETF_NBPA)
#define NETF_CNOR	(11<<NETF_NBPA)
#define NETF_CNAND	(12<<NETF_NBPA)
#define NETF_NEQ	(13<<NETF_NBPA)
#define	NETF_LSH	(14<<NETF_NBPA)
#define	NETF_RSH	(15<<NETF_NBPA)
#define	NETF_ADD	(16<<NETF_NBPA)
#define	NETF_SUB	(17<<NETF_NBPA)

/*  stack arguments  */
#define NETF_NOPUSH	0		/* don`t push */
#define NETF_PUSHLIT	1		/* next word in filter */
#define NETF_PUSHZERO	2		/* 0 */
#define	NETF_PUSHIND	14		/* word indexed by stack top */
#define	NETF_PUSHHDRIND	15		/* header word indexed by stack top */
#define NETF_PUSHWORD	16		/* word 0 .. 944 in packet */
#define	NETF_PUSHHDR	960		/* word 0 .. 31  in header */
#define	NETF_PUSHSTK	992		/* word 0 .. 31  in stack */

/* priorities */
#define	NET_HI_PRI	100
#define	NET_PRI_MAX	255

/*
 * Net receive message format.
 *
 * The header and data are packaged separately, since some hardware
 * supports variable-length headers.  We prefix the packet with
 * a packet_hdr structure so that the real data portion begins
 * on a long-word boundary, and so that packet filters can address
 * the type field and packet size uniformly.
 */
#define	NET_RCV_MAX	4095
#define	NET_HDW_HDR_MAX	64

/*
 * Need a msgid for identification for NORMA and other packets
 *	in device.defs reply range.
 */
#define	NORMA_RCV_MSG_ID	2998	/* for NORMA packets */
#define	NET_RCV_MSG_ID		2999	/* for other packets */

struct packet_header {
	unsigned short	length;
	unsigned short	type;	/* network order */
};

struct net_rcv_msg {
	mach_msg_header_t msg_hdr;
	NDR_record_t NDR;
	char		header[NET_HDW_HDR_MAX];
	mach_msg_type_number_t packetCnt;
	char		packet[NET_RCV_MAX];
	mach_msg_format_0_trailer_t trailer;
};

#define	net_rcv_msg_packet_count packetCnt

/*
 * Net receive message sg format.
 *
 * The entire packet is stuffed into 1 large buffer and passed up
 * to the receiver.
 */
struct net_rcv_msg_sg {
	mach_msg_header_t	msg_hdr;
	NDR_record_t NDR;
	mach_msg_type_number_t	packetCnt;
        vm_offset_t		data;
};

#define	net_rcv_msg_sg_packet_count packetCnt

/*
 * Out-of-line net receive message format - good enough for ethernet!
 */
struct net_rcv_msg_sg_data {
	char		header[NET_HDW_HDR_MAX];
	char		packet[NET_RCV_MAX];
};

typedef struct net_rcv_msg_sg 	*net_rcv_msg_sg_t;
typedef struct net_rcv_msg 	*net_rcv_msg_t;

#endif	/* _DEVICE_NET_STATUS_H_ */
