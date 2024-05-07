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
 * Revision 2.5.2.10  92/09/15  17:34:15  jeffreyh
 * 	Converted debug logging from ntr to tr.
 * 	[92/09/10            alanl]
 * 	With Jeffreyh:  add dp_reconstruct_complete to packet.
 * 	Added norma trace records, under NETIPC_TRACE conditional.
 * 	We have extensively revised the suspend/resume protocol.
 * 	Added netipc_pacstate_print routine.
 * 	[92/08/20            alanl]
 * 
 * 	Use pg_first_seqid instead of pg_norma_uid.  This change
 * 	permits unique identification of kmsgs waiting on the
 * 	incomplete list; more than one kmsgs from the same port/
 * 	node can be sitting on the list.  [With jeffreyh.]
 * 	Add assertions to guarantee that vm_map_copies passed
 * 	to these routines have the expected page list type.
 * 	[92/06/02            alanl]
 * 
 * Revision 2.5.2.9  92/06/24  18:01:27  jeffreyh
 * 	Removed debugging printf
 * 	[92/06/18            jeffreyh]
 * 
 * 	Change netipc_recv_resource_shortage to put a port on a 
 * 	suspended list instead of retargeting it. 
 * 	[92/06/10            jeffreyh]
 * 
 * 	Changes to enable packets in trasit to be suspended
 * 		1) netipc_packet array was changed to an
 * 		   array of netipc_packet_state
 * 		2) Added norma_ipc_suspend macro for ports.
 * 		3) Send over norma uid as part of page structure
 * 		4) New routine  netipc_recv_resource_shortage
 * 		   that suspendes a send when the remote node
 * 		   is short on memory.
 * 		5) New routine netipc_recv_resume_node to mark
 * 		   that a node may be resumed
 * 		5) New routine reconstruct_suspended_dp to resume
 * 		   sending a port that had been suspended.
 * 	Enhanced debugging code.
 * 	[92/06/09            jeffreyh]
 * 
 * 	Deal with remote port changing to/from proxy if a routine
 * 	called by norma_ipc_send blocks.
 * 	[92/06/02            dlb]
 * 
 * Revision 2.5.2.8  92/05/28  18:20:51  jeffreyh
 * 	Changed destination sending logic to call prepare routine before
 * 	sending.  Removed send migrating destination logic.  Correct
 * 	netip_port_dequeue_sync logic.
 * 	[92/05/28            dlb]
 * 
 * Revision 2.5.2.7  92/05/27  00:59:56  jeffreyh
 * 	Add netipc_port_is_queued for callers outside this file.
 * 	[92/05/26            dlb]
 * 
 * 	norma_ipc_unqueue now has synchronization logic to wakeup threads
 * 	waiting for network services to terminate;
 * 	netipc_port_dequeue_sync now causes them to wait.
 * 	[92/05/15            sjs]
 * 
 * Revision 2.5.2.6.1.2  92/05/08  10:25:01  jeffreyh
 * 	Interface to netipc_page_put changes to indicate
 * 	whether page being put was borrowed from reserved
 * 	VM pool.
 * 	[92/05/07            alanl]
 * 
 * 	Don't copyout norma kmsgs to network in norma_ipc_send().
 * 	This can happen when a receive right migrates twice and the
 * 	messages queued on it when the first migrate happens aren't
 * 	all received before the second migrate happens.
 * 	[92/05/08            dlb]
 * 
 * Revision 2.5.2.6.1.1  92/05/06  17:42:05  jeffreyh
 * 	Add logic to dequeue port if we discover that it's no longer a
 * 	proxy (i.e., it's being migrated here).  Add syncronization stub
 * 	to allow migrate code to wait for port dequeue; just panics for
 * 	now.
 * 	[92/05/05            dlb]
 * 
 * 	Pulled netipc_safe_vm_map_copy_discard_list scanning code
 * 	out of netipc_thread and into a special routine.  This routine
 * 	is still used from netipc_thread but is also called from
 * 	kserver_pageout_support_thread when memory is low and
 * 	netipc_thread can't run.  [Alanl and Jeffrey.]
 * 
 * Revision 2.5.2.6  92/04/08  15:46:00  jeffreyh
 * 	Removed debugging i860 ifdefs.
 * 	[92/04/08            andyp]
 * 
 * 	Temporary debugging logic.
 * 	[92/04/06            dlb]
 * 
 * 	Move IP_NULL logic into norma_ipc_send_port.
 * 	[92/04/02            dlb]
 * 
 * 	Remove "retarget to node self" printf.  Code in
 * 	norma_ipc_receive_rright has been fixed.
 * 	[92/03/31            dlb]
 * 
 * Revision 2.5.2.5  92/03/28  10:11:58  jeffreyh
 * 	Wakeup the netipc_thread more often on the iPSC.
 * 	[92/03/20            andyp]
 * 
 * Revision 2.5.2.4  92/02/21  14:31:59  jsb
 * 	Removed code incorrectly duplicated by bmerge.
 * 
 * Revision 2.5.2.3  92/02/21  11:24:50  jsb
 * 	Moved ipc_kmsg_copyout_to_network here from ipc/ipc_kmsg.c.
 * 	Renamed norma_ipc_destroy_proxy to norma_ipc_dead_destination.
 * 	[92/02/21  10:36:14  jsb]
 * 
 * 	In norma_ipc_send, convert kmsg to network format.
 * 	[92/02/21  09:07:00  jsb]
 * 
 * 	Changed for norma_ipc_send_port now returning uid.
 * 	[92/02/20  17:15:23  jsb]
 * 
 * 	Added netipc_thread_wakeup to netipc_safe_vm_map_copy_invoke_cont.
 * 	Added logic to convert node number to netipc packet address in
 * 	netipc_packet_print.
 * 	[92/02/18  17:37:14  jsb]
 * 
 * 	Perform norma_ipc_send_*_dest only after successful acknowledgement.
 * 	This allows simplification of reference counting for destination
 * 	ports. The old scheme kept a single reference for the port while it
 * 	was queued. The new scheme keeps a reference for the port for every
 * 	kmsg it has queued, which is released in norma_ipc_send_*_dest.
 * 	The only explicit reference counting management required by the
 * 	new scheme is the acquisition of a port reference for a proxy before
 * 	calling norma_ipc_destroy_proxy, which expects the caller to supply
 * 	a reference. Eliminated netipc_port_release and netipc_free_port_list,
 * 	since norma_ipc_send_*_dest now handle releasing references after
 * 	message delivery. Changed safe kmsg freeing code to call
 * 	norma_ipc_send_*_dest (which must not be called at interrupt level).
 * 	Also changed usage of ip_norma_queue_next field to allow elimination
 * 	of ip_norma_queued field.
 * 	[92/02/18  09:14:14  jsb]
 * 
 * Revision 2.5.2.2  92/02/18  19:15:50  jeffreyh
 * 	iPSC changes from Intel.
 * 	[92/02/18            jeffreyh]
 * 	[intel] added debugging callhere stuff, routine for
 * 	netipc_vm_map_copy_cont_check(), all for iPSC.
 * 	[92/02/13  13:10:00  jeffreyh]
 * 
 * Revision 2.5.2.1  92/01/21  21:52:17  jsb
 * 	More de-linting.
 * 	[92/01/17  11:40:20  jsb]
 * 
 * 	Added definition of, and call to, netipc_safe_ikm_free.
 * 	[92/01/16  22:13:17  jsb]
 * 
 * 	Minor de-linting.
 * 	[92/01/14  22:01:43  jsb]
 * 
 * 	Reworked interface with underlying protocol module; see comment
 * 	that begins 'This is now how we cooperate...' below.
 * 	[92/01/14  09:29:51  jsb]
 * 
 * 	De-linted. Added netipc_packet_print.
 * 	[92/01/13  10:16:50  jsb]
 * 
 * 	Moved netipc_packet definitions, processing, allocation, to here.
 * 	Moved netipc_ack status demultiplexing here.
 * 	[92/01/11  17:35:35  jsb]
 * 
 * 	Moved old contents to norma/ipc_wire.c.
 * 	Now contains functions split from norma/ipc_net.c.
 * 	[92/01/10  20:40:51  jsb]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1990,1991,1992 Carnegie Mellon University
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
 *	File:	norma/ipc_output.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1990
 *
 *	Functions to support ipc between nodes in a single Mach cluster.
 */

#include <ipc/ipc_kmsg.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_space.h>
#include <norma/ipc_node.h>
#include <norma/ipc_net.h>
#include <kern/macro_help.h>
#include <kern/misc_protos.h>
#include <norma/tr.h>

#if	PARAGON860
#include <i860/PARAGON/misc_protos.h>
#endif	/* PARAGON860 */

/*
 * This is now how we cooperate with the reliable transmission module
 * underneath us:
 *	1. When we have something to send, we call netipc_start.
 *	2. When he can send it, he upcalls netipc_send_new.
 *	3. We then call netipc_send_{kmsg,page,...} as appropriate.
 *	   From these routines, we call netipc_send_with_timeout,
 *	   specifying a packetid and a seqid.
 *	4. If he decides a packet needs to be retransmitted, he calls
 *	   netipc_send_old with the same packetid and seqid.
 *	5. He receives acknowledgements, eliminates redundant ones,
 *	   and calls netipc_recv_ack_with_status with same packetid, seqid.
 *	6. From ack_with_status we multiplex on status and call one
 *	   of netipc_recv_{success,retarget,dead,not_found}.
 *	7. Netipc_recv_success calls netipc_start if there is more to send.
 *
 * The rationale here is
 *	1. Keep retransmission knowledge in protocol module
 *	2. Keep kmsg, copy object, etc. knowledge here.
 *	3. Do windowing based on limitations imposed by both
 *	   layers. E.g. continuations limit how much of a copy
 *	   object we can send at a time.
 *	4. Allow for smarter protocol modules in the future while
 *	   retaining the no-copy aspect provided by kmsg-walking
 *	   code in this module.
 *	5. Allow for simpler protocol modules for reliable interconnects.
 *
 * Things left to do:
 *	1. Actually add windowing
 *	2. Have this module do its own seqid allocation, separate from
 *	   protocol seqids. This eliminates need for Xnetipc_next_seqid.
 *	3. Eliminate array-based lookup of netipc_packets
 */

#define	NORMA_REALIGN_OOL_DATA	0

#define	DP_TYPE_KMSG		0L
#define	DP_TYPE_PAGE		1L
#define	DP_TYPE_KMSG_MORE	2L
#define	DP_TYPE_OOL_PORTS	3L
#define DP_TYPE_MAX		4L

typedef struct netipc_packet		*netipc_packet_t;
#define	NETIPC_PACKET_NULL		((netipc_packet_t) 0)

unsigned int netipc_out_pkt_stats[DP_TYPE_MAX];

struct netipc_packet {
	unsigned long	dp_type;
	unsigned long	dp_remote;
	unsigned long	dp_seqid;
	unsigned long	dp_first_seqid;
	unsigned long	dp_last_seqid;
	unsigned long	dp_last_unacked;
	ipc_kmsg_t	dp_kmsg;
	unsigned long	dp_offset;
	netipc_packet_t	dp_next;
	ipc_port_t	dp_remote_port;
	vm_map_copy_t	dp_copy;
	unsigned long	dp_copy_index;
	unsigned long	dp_copy_npages;
	unsigned long	dp_copy_last;
	unsigned long	dp_page_list_base;
	boolean_t	dp_has_continuation;
	boolean_t	dp_being_continued;
	boolean_t	dp_reconstruct_complete;
};

/*
 *	The netipc_packet_state structure contains the state of a
 *	connection to a remote node.  The connection to a remote
 *	node may be "normal", that is to say that packets are being
 *	sent without any restrictions or special cases.  However,
 *	the remote node may indicate a memory shortage condition,
 *	in which case the sending side must suspend transmission.
 *
 *	Because paging messages must receive special, high-priority
 *	treatment, we can't simply stop all messages from flowing
 *	to the receiving node when the receiver declares a memory
 *	shortage.  Instead, we must suspend the *current* message
 *	being transmitted and start a new transmission from the next
 *	port queued for the receiver.  (We can't start transmitting
 *	the next message from the current port because that would
 *	violate the in-order delivery assumptions of Mach IPC.)
 *
 *	The suspension rules are these:
 *
 *	o if the packet in transmission is a kmsg (i.e.,
 *	the first packet of a kmsg), then the port sending
 *	the kmsg can be added to the list of suspended ports
 *
 *	o multiple ports can be added to the suspended port list
 *
 *	o if the packet in transmission is a page in
 *	the middle of a kmsg, then the entire netipc_packet
 *	containing the state of the message transmission
 *	can be shifted to the suspended packet variable
 *
 *	o only one packet can ever be suspended
 *
 *	The netipc_packet_state's state variable indicates whether
 *	the suspended packet can be restarted at the next possible
 *	point (in dp_advance).
 */
typedef struct netipc_packet_state {
	netipc_packet_t		netipc_packet;
	netipc_packet_t		suspended_packet;
	ipc_port_t		suspended_ports;
	unsigned int		state;
} *netipc_packet_state_t;

#define STATE_CLEAR		0L
#define STATE_RESUME		1L

#if	DYNAMIC_NUM_NODES

int	max_num_nodes=1024;

struct netipc_packet_state *netipc_packet_state;

#else   /* DYNAMIC_NUM_NODES */

#if	PARAGON860
#if	MCMSG
#define	MAX_NUM_NODES		1024	/* XXX */
#else	/* MCMSG */
#define	MAX_NUM_NODES		1	/* XXX */
#endif	/* MCMMSG */
#else	/* MPARAGON860 */
#define	MAX_NUM_NODES		256	/* XXX */
#endif	/* MPARAGON860 */

struct netipc_packet_state netipc_packet_state[MAX_NUM_NODES];

#endif	/* DYNAMIC_NUM_NODES */

/*
 * Forward.
 */
ipc_port_t		netipc_dequeue_port(
				netipc_packet_t	dp);

boolean_t		is_one_page_kmsg(
				ipc_kmsg_t	kmsg);

void			ipc_kmsg_copyout_to_network(
				ipc_kmsg_t	kmsg);

void			norma_ipc_queue_first_port(
				ipc_port_t	remote_port,
				netipc_packet_t	dp,
				ipc_port_t	next_port);

void			norma_ipc_queue_port(
				ipc_port_t	remote_port,
				boolean_t	can_block);

void			ipc_kmsg_unrmqueue_first(
				ipc_port_t	port,
				ipc_kmsg_t	kmsg);

void			netipc_send_kmsg(
				unsigned long	remote,
				netipc_packet_t	dp);

void			netipc_send_kmsg_more(
				unsigned long	remote,
				netipc_packet_t	dp);

void			netipc_send_ool_ports(
				unsigned long	remote,
				netipc_packet_t	dp);

void			netipc_send_page(
				unsigned long	remote,
				netipc_packet_t	dp);

void			netipc_send_page_with_continuation(
				unsigned long	remote,
				netipc_packet_t	dp);

void			netipc_next_copy_object(
				netipc_packet_t	dp);

void			netipc_set_seqid(
				netipc_packet_t	dp,
				unsigned long	seqid);

boolean_t		reconstruct_suspended_dp(
				netipc_packet_state_t	ds);

void			netipc_mark_unsuspended(
				ipc_port_t	port_list);

void			netipc_remove_from_port_list(
				ipc_port_t	port,
				ipc_port_t	*port_list);

void			netipc_append_port_list(
				ipc_port_t	*new_list,
				ipc_port_t	old_list);

static boolean_t	dp_advance(
				netipc_packet_t	dp);

void			dp_finish(
				netipc_packet_t	dp);

void			netipc_recv_success(
				netipc_packet_t	dp,
				unsigned long	seqid);

void			netipc_recv_retarget(
				netipc_packet_t	dp,
				unsigned long	seqid,
				unsigned long	new_remote);

void			netipc_recv_dead(
				netipc_packet_t	dp,
				unsigned long	seqid);

void			netipc_recv_not_found(
				netipc_packet_t	dp,
				unsigned long	seqid);

void			netipc_recv_resource_shortage(
				netipc_packet_t	dp,
				unsigned long	seqid,
				unsigned long	data);

void			netipc_suspend_port(
				netipc_packet_state_t	ds);

void			netipc_send_dp(
				unsigned long	remote,
				netipc_packet_t	dp);

netipc_packet_t		netipc_packet_allocate(
				int		behaviour);

void			netipc_packet_deallocate(
				netipc_packet_t	dp);

void			netipc_packet_list_fill(void);

void			netipc_safe_vm_map_copy_invoke_cont(
				netipc_packet_t	dp);

void			netipc_vm_map_copy_invoke(
				netipc_packet_t	dp);

void			netipc_safe_vm_map_copy_discard(
				vm_map_copy_t	copy);

void			netipc_safe_ikm_free(
				ipc_kmsg_t	kmsg);

void			netipc_vm_map_copy_thread(void);

void			netipc_set_node_state(
				unsigned long	*typep,
				unsigned long	remote);

unsigned int		c_netipc_vm_map_copy_invoke_cont = 0;
unsigned int		c_netipc_vm_map_copy_processed = 0;

/*
 * netipc_packet_allocate() behaviour if no netipc packet available.
 */
#define	NETIPC_PACKET_WAIT	0	/* wait for a netipc packet */
#define	NETIPC_PACKET_ABORT	1	/* abort if no netipc packet */
#define	NETIPC_PACKET_DEFER	2	/* defer if no netipc packet */

/*
 * MP notes: the ip_norma_queue_next and ip_norma_suspended fields
 *	of the port structure are protected by the netipc_thread_lock.
 */
#define	norma_ipc_queued(port)		((port)->ip_norma_queued)
#define	norma_ipc_unqueue(port)	\
	(port)->ip_norma_queued = FALSE; \
	if ((port)->ip_norma_sync) { \
		(port)->ip_norma_sync = FALSE; \
		thread_wakeup_one((int) &(port)->ip_norma_xmm_object_refs); \
	}
#define	norma_ipc_zombie(port)		((port)->ip_norma_is_zombie)

/*
 * Putting a bogus value in ip_norma_queue_next makes the port look
 * like it's already queued. This will prevent norma_ipc_queue_port
 * from sticking it on the queue again and starting another send.
 */

#define norma_ipc_suspend(port) \
	(port)->ip_norma_queued = TRUE; \
	((port)->ip_norma_queue_next = (ipc_port_t) -666)
/*
 * This should be conditionalized on machine and type of interconnect.
 * For now, we assume that everyone will be happy with 32 bit alignment.
 */
#define	WORD_SIZE	4
#define	WORD_MASK	(WORD_SIZE - 1)

#define ROUND_WORD(x)	((((unsigned long)(x)) + WORD_MASK) & ~WORD_MASK)
#define TRUNC_WORD(x)	(((unsigned long)(x)) & ~WORD_MASK)
#define	WORD_ALIGNED(x)	((((unsigned long)(x)) & WORD_MASK) == 0)

zone_t			netipc_packet_zone;

ipc_kmsg_t		netipc_kmsg_cache;
vm_size_t		netipc_kmsg_first_half;
int			netipc_kmsg_cache_hits;		/* debugging */
int			netipc_kmsg_cache_misses;	/* debugging */
int			netipc_kmsg_splits;		/* debugging */

struct netipc_hdr	send_hdr_p;
struct netipc_hdr	send_hdr_k;
struct netipc_hdr	send_hdr_m;
struct netipc_hdr	send_hdr_o;

struct netvec		netvec_p[3];
struct netvec		netvec_k[3];
struct netvec		netvec_m[3];
struct netvec		netvec_o[3];

struct vm_map_copy	netipc_kmsg_more_copy;
struct vm_map_copy	netipc_ool_ports_copy;

/*
 * Count of number of pages cleaned by netipc_output_replenish_pages.
 */
unsigned int netipc_output_replenish_cleaned = 0;

/*
 * vm_map_copy_discard_cont is not an interesting continuation, that is,
 * it does not affect the way a copy object is sent, because it will
 * not result in any new page lists.
 */
#define	vm_map_copy_has_interesting_cont(copy) \
  (vm_map_copy_has_cont(copy) && (copy)->cpy_cont != vm_map_copy_discard_cont)

spinlock_t netipc_continuing_packet_lock;

void
netipc_output_init(void)
{
	/*
	 * Initialize send_hdr_k and netvec_k
	 */
	send_hdr_k.type = NETIPC_TYPE_KMSG;
	send_hdr_k.remote = node_self();
	netvec_k[0].addr = KVTODEV(&send_hdr_k);
	netvec_k[0].size = sizeof(struct netipc_hdr);

	/*
	 * Initialize send_hdr_p and netvec_p
	 */
	send_hdr_p.type = NETIPC_TYPE_PAGE;
	send_hdr_p.remote = node_self();
	netvec_p[0].addr = KVTODEV(&send_hdr_p);
	netvec_p[0].size = sizeof(struct netipc_hdr);

	/*
	 * Initialize send_hdr_m and netvec_m
	 */
	send_hdr_m.type = NETIPC_TYPE_PAGE;
	send_hdr_m.remote = node_self();
	netvec_m[0].addr = KVTODEV(&send_hdr_m);
	netvec_m[0].size = sizeof(struct netipc_hdr);

	/*
	 * Initialize send_hdr_o and netvec_o
	 */
	send_hdr_o.type = NETIPC_TYPE_PAGE;
	send_hdr_o.remote = node_self();
	netvec_o[0].addr = KVTODEV(&send_hdr_o);
	netvec_o[0].size = sizeof(struct netipc_hdr);

	netipc_packet_zone = zinit(sizeof(struct netipc_packet), 512*1024,
				   PAGE_SIZE, "netipc packet");

	spinlock_init(&netipc_continuing_packet_lock);
	(void) kernel_thread(kernel_task, netipc_vm_map_copy_thread, (char *)0);
	netipc_packet_list_fill();
}


/*
 * Called from netipc_recv_retarget and netipc_recv_dead.
 */
ipc_port_t
netipc_dequeue_port(
	netipc_packet_t	dp)
{
	ipc_port_t remote_port;

	assert(netipc_thread_lock_held());
	assert(dp);
	printf1("netipc_dequeue_port(%d)\n", dp->dp_remote);
	netipc_set_seqid(dp, dp->dp_first_seqid);
	assert(dp->dp_type == DP_TYPE_KMSG);	/* is at start of dp (kmsg) */

	/*
	 * Remove this port from this node's queue.
	 * Leave the port referenced.
	 */
	remote_port = dp->dp_remote_port;
	assert(remote_port->ip_norma_dest_node == dp->dp_remote);
	dp->dp_remote_port = dp->dp_remote_port->ip_norma_queue_next;
	ip_lock(remote_port);
	norma_ipc_unqueue(remote_port);

	/*
	 * Move kmsg from dp back onto port.
	 */
	assert(dp->dp_kmsg != IKM_NULL);
	ipc_kmsg_unrmqueue_first(remote_port, dp->dp_kmsg);
	remote_port->ip_msgcount++;
	ip_unlock(remote_port);

	/*
	 * If there is another port, start it sending;
	 * otherwise, release dp.
	 */
	if (dp->dp_remote_port != IP_NULL) {
		printf1("== dequeue_port: advancing to port 0x%x\n",
			dp->dp_remote_port);
		(void) dp_advance(dp);
		netipc_start(dp->dp_remote);
	} else {
		unsigned long remote = dp->dp_remote;
		printf1("== dequeue_port: no more ports\n");
		netipc_packet_deallocate(dp);
		netipc_packet_state[remote].netipc_packet = 
			(netipc_packet_t) 0;
	}
	return remote_port;
}

#define	kmsg_size(kmsg)							\
	(ikm_plus_overhead((kmsg)->ikm_header.msgh_size +		\
			   ((mach_msg_format_0_trailer_t *)		\
			    ((vm_offset_t)&(kmsg)->ikm_header +		\
			     round_msg((kmsg)->ikm_header.msgh_size))	\
			    )->msgh_trailer_size))

boolean_t
is_one_page_kmsg(
	ipc_kmsg_t	kmsg)
{
	if (kmsg->ikm_header.msgh_bits & MACH_MSGH_BITS_COMPLEX_DATA) {
		return FALSE;
	}
	if (kmsg_size(kmsg) > PAGE_SIZE) {
		return FALSE;
	}
	return TRUE;
}

/*
 *	netipc_port_dequeue_sync
 *
 *	This is called by norma_ipc_receive_rright when it converts a
 *	proxy to a real port to wait for the proxy to be dequeued if
 *	it was queued to send.
 *
 * XXX	Synchronization should be done using netipc_thread_lock.
 * XXX	The problem is that the lock must be released before
 * XXX	blocking -- but the amount of processing that can happen
 * XXX	from netipc_thread_unlock is horrendous.  Fall back on
 * XXX	spl synchronization instead.
 */
void
netipc_port_dequeue_sync(
	ipc_port_t	port)
{
	while (norma_ipc_queued(port)) {
		/*
		 * wait for the port to drop from the send list.
		 */
		port->ip_norma_sync = TRUE;
		assert_wait((int) &port->ip_norma_xmm_object_refs, TRUE);
		ip_unlock(port);
		thread_block((void (*)) 0);
		ip_lock(port);
	}
}

/*
 *	Routine:	ipc_kmsg_copyout_to_network
 *	Purpose:
 *		Prepare a copied-in message for norma_ipc_send.
 *		This means translating ports to uids, translating
 *		entry-list copy objects into page list copy objects,
 *		and setting MACH_MSG_BITS_COMPLEX_XXX bits.
 *		Derived from ipc_kmsg_copyin_from_kernel.
 *	Conditions:
 *		Nothing locked.
 */
void
ipc_kmsg_copyout_to_network(
	ipc_kmsg_t	kmsg)
{
    	mach_msg_descriptor_t	*saddr, *eaddr;
    	mach_msg_body_t		*body;
	kern_return_t kr;
	vm_size_t	ool_length;	/* sum of sizes of ool data */
	int i;
	vm_offset_t			paddr = 0;
	vm_size_t			space_needed = 0;
	vm_map_t	map = current_map(); /* XXX should it be an arg ? */

    	body = (mach_msg_body_t *) (&kmsg->ikm_header + 1);
    	saddr = (mach_msg_descriptor_t *) (body + 1);
    	eaddr = (mach_msg_descriptor_t *) saddr + body->msgh_descriptor_count;
	ool_length = 0;

    	for ( i=0 ; saddr <  eaddr; saddr++, i++) {

	    switch (saddr->type.type) {

	        case MACH_MSG_PORT_DESCRIPTOR: {
		    mach_msg_port_descriptor_t 	*dsc;
		    mach_msg_type_name_t 	name;
		    ipc_port_t 			*port;
	
		    dsc = &saddr->port;
		    name = dsc->disposition;
		    port = (ipc_port_t *)&dsc->name;
		    kmsg->ikm_header.msgh_bits |= MACH_MSGH_BITS_COMPLEX_PORTS;
		    *port = (ipc_port_t) norma_ipc_send_port(*port, name);
		    break;	    
	        }
	        case MACH_MSG_OOL_PORTS_DESCRIPTOR: {
		    vm_size_t 				length;
		    ipc_port_t            		*ports;
		    int					j;
		    mach_msg_type_name_t    		name;
		    mach_msg_ool_ports_descriptor_t 	*dsc;

		    dsc = &saddr->ool_ports;
		    length = dsc->count * sizeof(mach_port_t);
		
		    if (length == 0) {
		    	dsc->address = (void *) 0;
			break;
		    }

		    name = dsc->disposition;
		    ports = (ipc_port_t *) dsc->address;
		    kmsg->ikm_header.msgh_bits |=
		                        (MACH_MSGH_BITS_COMPLEX_PORTS|
					 MACH_MSGH_BITS_COMPLEX_DATA);
		    for (j = 0; j < dsc->count; j++) {
		        ports[j] = (ipc_port_t) norma_ipc_send_port(ports[j], name);
		    }
		    ool_length += dsc->count * sizeof(mach_port_t);
		    break;
	        }
	        case MACH_MSG_OOL_DESCRIPTOR: {
		    mach_msg_ool_descriptor_t *dsc;

		    dsc = &saddr->out_of_line;
		    if (dsc->size == 0) {
		        assert(dsc->address == (void *) 0);
			continue;
		    }

		    kmsg->ikm_header.msgh_bits |= MACH_MSGH_BITS_COMPLEX_DATA;
		    kr = vm_map_convert_to_page_list((vm_map_copy_t *) &(dsc->address));
		    if (kr != KERN_SUCCESS) {
			/*
			 * XXX
			 * vm_map_convert_to_page_list has failed.
			 * Now what? If the error had been detected
			 * before the MiG stub was invoked
			 * the stub would do the right thing,
			 * but it's a bit late now.
			 * Probably the best we can do is return a null
			 * copy object. Need to adjust number accordingly.
			 *
			 * XXX
			 * Discard original copy object?
			 */
			printf("XXX convert_to_network: page_list: %d", kr);
			dsc->size = 0;
			dsc->address = (void *)0;
		   } else
			ool_length += dsc->size;
		}
	        default: {
		    continue;
		}
	    }
	}
	kmsg->ikm_ool_bytes = ool_length;
}

/*
 * Main entry point from regular ipc code.
 * The destination port (remote_port) is locked.
 * Returns KERN_NOT_RECEIVER if remote_port migrates,
 * in which case the caller is obligated to resend
 * it locally.
 *
 * MP notes: Upon return, the port is unlocked.
 */
unsigned int c_norma_ipc_send_criticals = 0;
unsigned int c_norma_ipc_send_wait = 0;
unsigned int c_norma_ipc_send_blocked = 0;
unsigned int c_ikm_wait_for_send = 0;
unsigned int c_ikm_no_wait_for_send = 0;
#if	NORMA_PROXY_PORT_QUEUE_LIMIT
unsigned int c_norma_ipc_qlimit_block = 0;
unsigned int c_norma_ipc_send_proxy_no_more = 0;
unsigned int norma_proxy_queue_limit = 1;
#endif

mach_msg_return_t
norma_ipc_send(
#if	NORMA_PROXY_PORT_QUEUE_LIMIT
	ipc_kmsg_t		kmsg,
	mach_msg_option_t	option,
	mach_msg_timeout_t	timeout
#else
	ipc_kmsg_t		kmsg
#endif
	       )
{
	register mach_msg_header_t *msgh;
	ipc_port_t local_port, remote_port;
	mach_msg_bits_t bits;
	int kmsg_excess;
	netipc_packet_state_t	ds;
	unsigned long		remote;
#if	NORMA_PROXY_PORT_QUEUE_LIMIT
	ipc_thread_t		self;
#endif

	msgh = (mach_msg_header_t *) &kmsg->ikm_header;
	remote_port = (ipc_port_t) msgh->msgh_remote_port;
	local_port = (ipc_port_t) msgh->msgh_local_port;
	bits = msgh->msgh_bits;

	assert(IP_NORMA_IS_PROXY(remote_port));

	if (kmsg_size(kmsg) > PAGE_SIZE) {
		/*
		 * Now that we are actually using this bit in this case,
		 * we should simplify some of the tests below.
		 */
		msgh->msgh_bits = (bits |= MACH_MSGH_BITS_COMPLEX_DATA);
	}

	/*
	 * Convert the message to NORMA (network) format.  Unfortunately,
	 * both the forward (copyout_to_network) and backward (uncopyout_
	 * to_network) routines may block to allocate memory (e.g., for
	 * proxy creation), during which time the remote port could migrate
	 * and change to/from a proxy.
	 */
	while (TRUE) {

		/*
		 * Prepare the remote port for sending and get the receiver's
		 * uid.  Migrated messages have already been prepared.
		 */
		if (bits & MACH_MSGH_BITS_MIGRATED) {
  			kmsg->ikm_has_dest_right = FALSE;
			kmsg->ikm_wait_for_send = FALSE;
		}
		else {
			kmsg->ikm_forward_proxy = IP_NULL;
			kmsg->ikm_has_dest_right = norma_ipc_prepare_dest(
				remote_port, MACH_MSGH_BITS_REMOTE(bits));
		}
		assert(remote_port->ip_norma_uid != 0);
		msgh->msgh_remote_port = (mach_port_t)
						remote_port->ip_norma_uid;

		/*
		 * Convert the message to network format.  Norma kmsgs are
		 * already in this format, and need no such conversion.
		 *
		 * XXX There has to be a better way to do the local port.
		 * XXX Does the type of the local port need to change in the
		 * XXX message header?
		 *
		 * XXX Port locking is definitely WRONG in this code!!
		 */
		if (kmsg->ikm_size != IKM_SIZE_NORMA) {
		    ip_unlock(remote_port);
		    if (local_port) {
		  	msgh->msgh_local_port = (mach_port_t)
			    norma_ipc_send_port(local_port,
				ipc_object_copyin_type(
				    MACH_MSGH_BITS_LOCAL(bits)));
		    }
		    if (msgh->msgh_bits & MACH_MSGH_BITS_COMPLEX) {
			ipc_kmsg_copyout_to_network(kmsg);
		    }
		    ip_lock(remote_port);
		}

		/*
		 *	If it's still a proxy, we're done; go queue message.
		 */
		if (IP_NORMA_IS_PROXY(remote_port))
			break;

		/*
		 *	Undo the conversion to network format.
		 */

#if	NORMA_PROXY_PORT_QUEUE_LIMIT
		/*
		 *	Very tacky:  jump back into the middle
		 *	of the loop when we detect a proxy that
		 *	turned into a principal, below.  The
		 *	message will be uncopied-out and then
		 *	under lock we will recheck the proxy
		 *	status.  The worst that can happen --
		 *	in theory -- is that we will return to
		 *	ipc_mqueue_send and start this whole
		 *	process over again.
		 */
	    proxy_no_more:
#endif
		ipc_kmsg_uncopyout_header_to_network(msgh, remote_port);
		if (msgh->msgh_bits & MACH_MSGH_BITS_COMPLEX) {
			ip_unlock(remote_port);
			ipc_kmsg_uncopyout_to_network(kmsg);
			ip_lock(remote_port);
		}

		/*
		 *	If it's not a proxy, we're done; return failure.
		 *	See ipc_mqueue_send() for recovery.
		 */
		if (!IP_NORMA_IS_PROXY(remote_port)) {
			ip_unlock(remote_port);
			return KERN_NOT_RECEIVER;
		}
	}

	/*
	 * If message was not sent by the kernel, indicate that
	 * we will wait for the message to be completely sent.
	 * There is no reason to hold netipc_thread_lock for
	 * this computation.
	 */
	if (!kmsg->ikm_no_wait && kmsg->ikm_must_wait) {
		kmsg->ikm_wait_for_send = TRUE;
		++c_ikm_wait_for_send;
	} else {
		kmsg->ikm_wait_for_send = FALSE;
		++c_ikm_no_wait_for_send;
	}
	kmsg->ikm_send_completed = FALSE;

#if	NORMA_PROXY_PORT_QUEUE_LIMIT
	/*
	 *	Observe the port queue limit for proxy ports.
	 *	Messages will only back up on a proxy port
	 *	if the principal's node is experiencing
	 *	NORMA IPC resource shortages.  We are not
	 *	directly exporting the principal's port
	 *	queue limit, only the notion that the
	 *	receiving node is in trouble.  It is important
	 *	to slow down message generation on the proxy
	 *	side for two reasons:
	 *		- limit consumption of sending node resources
	 *		- prevent receiver from being slammed with
	 *		many pending messages generated while the
	 *		receiver was reclaiming NORMA IPC resources
	 *	We only pause here if:
	 *		- the port is a proxy
	 *		- the port is active
	 *		- the message count is over the queue limit
	 *		- the message need not be sent immediately
	 *		- the message is not paging-critical
	 *	The sender is resumed when the message is
	 *	acknowledged by the receiving node.
	 *
	 * XXX	It is essential that these manipulations take
	 * XXX	place under netipc_thread_lock because these
	 * XXX	fields will also be manipulated by the send-side
	 * XXX	interrupt-level routine, dp_advance.  The ip_lock
	 * XXX	is useless for synchronization in this situation.
	 */
	if (!IP_NORMA_IS_PROXY(remote_port) ||
	    !ip_active(remote_port)) {
		++c_norma_ipc_send_proxy_no_more;
		printf("norma_ipc_send:  proxy no more!\n");
		goto proxy_no_more;
	}
	if ((remote_port->ip_msgcount >= norma_proxy_queue_limit) &&
	    !(option & MACH_SEND_ALWAYS) &&
	    !norma_critical_message(kmsg->ikm_header.msgh_id, FALSE)) {
		kmsg->ikm_wait_for_send = TRUE;
		++c_norma_ipc_qlimit_block;
	}
#endif	/* NORMA_PROXY_PORT_QUEUE_LIMIT */

	/*
	 * add the size of the kmsg that is larger than a page to the
	 * ool_bytes count since it will appear as ool data to the receiver.
	 */
	kmsg_excess = kmsg_size(kmsg) - PAGE_SIZE;
	if (kmsg_excess > 0) {
		kmsg->ikm_ool_bytes += kmsg_excess;
	}

	/*
	 * Block interrupts while queueing message and port.
	 * Take the locks in the right order.
	 */
	if (!netipc_thread_lock_try()) {
		ip_unlock(remote_port);
		netipc_thread_lock();
		ip_lock(remote_port);
	}

	/*
	 * Enqueue the kmsg on the port.
	 */
	assert(remote_port->ip_pset == /*IPS_NULL*/0);	/* XXX ??? */
	remote_port->ip_msgcount++;
	ipc_kmsg_enqueue_macro(&remote_port->ip_messages.imq_messages, kmsg);

	/*
	 *	Check for the case of enqueueing a critical
	 *	message	on a suspended port.  If so, remove
	 *	the port from the suspended list, mark it
	 *	as not being suspended, and set it up to be
	 *	queued for transmission.  As a side-effect,
	 *	norma_critical_port() will have pushed critical
	 *	messages to the front of the queue so messages
	 *	from the port will be allowed by the receiver
	 *	rather than resulting in another suspend.
	 *	(This reordering happens only for special kernel
	 *	cases that do not rely on in-order IPC semantics.)
	 *
	 *	Note that checking for norma_critical_message
	 *	isn't strictly necessary:  in fact,
	 *	norma_critical_port does a better job of deciding
	 *	which messages really matter.  However, the __port
	 *	call is significantly more expensive.
	 */
	if (remote_port->ip_norma_suspended == TRUE &&
	    norma_critical_message(kmsg->ikm_header.msgh_id, TRUE)) {
		assert(norma_ipc_queued(remote_port));
		if (norma_critical_port(remote_port)) {
			/*
			 *	Here comes the fun.  Unsuspend
			 *	the port so that it will be
			 *	queued for transmission below.
			 */
			++c_norma_ipc_send_criticals;
			remote = remote_port->ip_norma_dest_node;
			ds = &netipc_packet_state[remote];
			netipc_remove_from_port_list(remote_port,
						     &ds->suspended_ports);
			remote_port->ip_norma_suspended = FALSE;
			remote_port->ip_norma_queue_next = remote_port;
			remote_port->ip_norma_queued = FALSE;
		}
	}

	/*
	 * Enqueue the port on the queue of ports with something to send.
	 */
	norma_ipc_queue_port(remote_port, TRUE); /* consumes lock */

	if (kmsg->ikm_wait_for_send) {
		++c_norma_ipc_send_wait;
		while (!kmsg->ikm_send_completed && !kmsg->ikm_send_aborted) {
			assert_wait((int)kmsg, FALSE);
			netipc_thread_unlock();
			++c_norma_ipc_send_blocked;
			thread_block((void (*)) 0);
			netipc_thread_lock();
		}
		kmsg->ikm_wait_for_send = FALSE;

		/*
		 *      If the kmsg has got an error during send, then
		 *      it must be destroyed to free message resources.
		 */
		if (kmsg->ikm_send_aborted) {
			ip_lock(remote_port);
			assert(!ip_active(remote_port));
			ipc_kmsg_uncopyout_header_to_network(msgh,
							     remote_port);
			ip_unlock(remote_port);
			if (msgh->msgh_bits & MACH_MSGH_BITS_COMPLEX)
				ipc_kmsg_uncopyout_to_network(kmsg);
			ipc_kmsg_destroy(kmsg, IP_RT(remote_port));
		} else {
			if (kmsg->ikm_size == IKM_SIZE_NORMA) {
				netipc_page_put(kmsg->ikm_page, FALSE,
						"norma_ipc_send");
			}
			else {
				if (kmsg->ikm_size != IKM_SAVED_KMSG_SIZE ||
				    !ikm_cache_put(kmsg)) {
					/* FALSE for now */
					ikm_free(kmsg, FALSE);
				}
			}
		}
	}
	netipc_thread_unlock();
	return KERN_SUCCESS;
}

/*
 * MP notes: Upon call, port is locked. Upon return, it is unlocked.
 */
void
norma_ipc_queue_first_port(
	ipc_port_t	remote_port,
	netipc_packet_t	dp,
	ipc_port_t	next_port)
{
	ipc_port_t port;
	ipc_kmsg_t kmsg;
	ipc_kmsg_queue_t kmsgs;
	netipc_packet_state_t ds;
	unsigned long remote;
#if	NORMA_PROXY_PORT_QUEUE_LIMIT && 0
	ipc_thread_queue_t senders;
	ipc_thread_t sender;
#endif
	TR_DECL("norma_ipc_queue_first");

	assert(netipc_thread_lock_held());
	remote = remote_port->ip_norma_dest_node;
	tr2("entry, remote 0x%x", remote);

	ds = &netipc_packet_state[remote];

	/*
	 * Pull first kmsg from port.
	 */
	imq_lock(&remote_port->ip_messages);
	kmsgs = &remote_port->ip_messages.imq_messages;
	kmsg = ipc_kmsg_queue_first(kmsgs);
	assert(kmsg != IKM_NULL);
	ipc_kmsg_rmqueue_first_macro(kmsgs, kmsg);
	remote_port->ip_msgcount--;
	imq_unlock(&remote_port->ip_messages);
	remote_port->ip_norma_queued = TRUE;
	ip_unlock(remote_port);
	assert(remote_port != next_port);
	remote_port->ip_norma_queue_next = next_port;

	ds->netipc_packet = dp;
	dp->dp_first_seqid = Xnetipc_next_seqid(remote);
	dp->dp_last_unacked = dp->dp_first_seqid;
	dp->dp_type = DP_TYPE_KMSG;
	dp->dp_remote = remote;
	dp->dp_remote_port = remote_port;
	dp->dp_kmsg = kmsg;
	dp->dp_reconstruct_complete = FALSE;
	dp->dp_being_continued = FALSE;
	if (is_one_page_kmsg(kmsg)) {
		dp->dp_last_seqid = dp->dp_first_seqid;
	} else {
		dp->dp_last_seqid = 0;
	}

}


/*
 * Put port on list of ports trying to send to port->ip_norma_dest_node.
 * If there are no other ports, then start a send.
 *
 * MP notes: Upon call, port is locked. Upon return, it is unlocked.
 */
void
norma_ipc_queue_port(
	ipc_port_t	remote_port,
	boolean_t	can_block)
{
	ipc_port_t port;
	unsigned long remote;
	netipc_packet_t dp, newdp;
	netipc_packet_state_t ds;
	TR_DECL("norma_ipc_queue_port");

	assert(netipc_thread_lock_held());
	tr3("entry, port 0x%x remote 0x%x", remote_port, remote);
	newdp = NETIPC_PACKET_NULL;

 restart:
	remote = remote_port->ip_norma_dest_node;
	ds = &netipc_packet_state[remote];

	/*
	 * If it's on the list, return.
	 */
	if (norma_ipc_queued(remote_port)) {
		if (!norma_ipc_zombie(remote_port)) {
			assert((ds->netipc_packet != NETIPC_PACKET_NULL) ||
			       (ds->suspended_packet != NETIPC_PACKET_NULL) ||
			       (ds->suspended_ports != IP_NULL));
		}
		ip_unlock(remote_port);
		if (newdp != NETIPC_PACKET_NULL)
			netipc_packet_deallocate(newdp);
		return;
	}

	/*
	 * If there are other ports already on the list,
	 * then queue the port and return.
	 */
	dp = ds->netipc_packet;
	if (dp != NETIPC_PACKET_NULL) {
		remote_port->ip_norma_queued = TRUE;
		ip_unlock(remote_port);
		for (port = dp->dp_remote_port;
		     port->ip_norma_queue_next;
		     port = port->ip_norma_queue_next) {
			continue;
		}
		remote_port->ip_norma_queue_next = IP_NULL;
		port->ip_norma_queue_next = remote_port;
		if (newdp != NETIPC_PACKET_NULL)
			netipc_packet_deallocate(newdp);
		return;
	}

	/*
	 *	Allocate and initialize a dp.  Because
	 *	netipc_packet_allocate can drop and re-take
	 *	netipc_thread_lock, allocate a packet before
	 *	manipulating the state of the port.
	 */
	if (newdp == NETIPC_PACKET_NULL) {
		if (can_block) {
			ip_unlock(remote_port);
			newdp = netipc_packet_allocate(NETIPC_PACKET_WAIT);
			if (newdp == NETIPC_PACKET_NULL)
				panic("netipc_packet_allocate. bogus panic\n");
			ip_lock(remote_port);
			goto restart;
		}
		newdp = netipc_packet_allocate(NETIPC_PACKET_DEFER);
		if (newdp != NETIPC_PACKET_NULL)
			goto restart;

		/*
		 * packet processing will take place as soon as
		 * netipc_packets will have been made available.
		 * For now, put port in the suspended list with
		 * ip_norma_suspended flag FALSE.
		 */
		remote_port->ip_norma_queued = TRUE;
		ip_unlock(remote_port);
		if (ds->suspended_ports == IP_NULL) {
			ds->suspended_ports = remote_port;
		} else {
			for (port = ds->suspended_ports;
			     port->ip_norma_queue_next != IP_NULL;
			     port = port->ip_norma_queue_next) {
				assert(!port->ip_norma_suspended);
				continue;
			}
			port->ip_norma_queue_next = remote_port;
		}
		remote_port->ip_norma_queue_next = IP_NULL;
		remote_port->ip_norma_suspended = FALSE;

	} else {
		norma_ipc_queue_first_port(remote_port, newdp, IP_NULL);
		/*
		 * Send it if we can.
		 */
		netipc_start(remote);
	}
}

/*
 * Return to port message queue a kmsg removed via ipc_kmsg_rmqueue_first.
 *
 * MP notes: Port is locked.
 */
void
ipc_kmsg_unrmqueue_first(
	ipc_port_t	port,
	ipc_kmsg_t	kmsg)
{
	register ipc_kmsg_queue_t queue;

	/*
	 *	If the port is no longer a proxy, then the kmsg has
	 *	to be converted from norma to internal format.
	 */
	if (!IP_NORMA_IS_PROXY(port)) {
		ipc_kmsg_uncopyout_header_to_network(&kmsg->ikm_header, port);
		ipc_kmsg_uncopyout_to_network(kmsg);
	}

	assert(netipc_thread_lock_held());
	printf1("*** ipc_kmsg_unrmqueue_first(0x%x, 0x%x)\n", queue, kmsg);
	imq_lock(&port->ip_messages);
	queue = &port->ip_messages.imq_messages;
	if (queue->ikmq_base == IKM_NULL) {
		kmsg->ikm_next = kmsg;
		kmsg->ikm_prev = kmsg;
	} else {
		register ipc_kmsg_t first = queue->ikmq_base;
		register ipc_kmsg_t last = first->ikm_prev;

		kmsg->ikm_next = first;
		kmsg->ikm_prev = last;
		first->ikm_prev = kmsg;
		last->ikm_next = kmsg;
	}
	queue->ikmq_base = kmsg;
	imq_unlock(&port->ip_messages);
}

ipc_port_t netipc_dead_port_list = IP_NULL;

/*
 * Called from netipc_send_dp.
 */
void
netipc_send_kmsg(
	unsigned long	remote,
	netipc_packet_t	dp)
{
	unsigned int netvec_count;
	vm_offset_t length;

	assert(netipc_thread_lock_held());

	/*
	 * Kmsgs are word aligned.
	 */
	assert(WORD_ALIGNED(dp->dp_kmsg));

	/*
	 * Fill in send_hdr_k.
	 */
	send_hdr_k.seqid = dp->dp_seqid;
	netipc_set_node_state(&send_hdr_k.type, remote);

	/*
	 * Fill in netvec_k.
	 * Cache KVTODEV and page-splitting computations.
	 * (Kmsgs occasionally cross page boundaries, unfortunately.)
	 *
	 * This routine attempts to cache the results of KVTODEV
	 * since it is relatively expensive.
	 * This caching may be less effective now since we've added
	 * flow control, since we don't immediately stuff the kmsg back
	 * into the ikm_cache, which means it might not be the kmsg
	 * we see next. Perhaps we can use the kmsg->ikm_page field
	 * for caching physaddr?
	 */
	if (dp->dp_kmsg != netipc_kmsg_cache) {
		vm_offset_t data = (vm_offset_t) dp->dp_kmsg;
		netipc_kmsg_cache = dp->dp_kmsg;

		netipc_kmsg_first_half = round_page(data) - data;
			
		netvec_k[1].addr = KVTODEV(data);
		netvec_k[2].addr = KVTODEV(data + netipc_kmsg_first_half);
		netipc_kmsg_cache_misses++;
	} else {
		netipc_kmsg_cache_hits++;
	}

	/*
	 * Calculate how much of kmsg to send.
	 */
	length = kmsg_size(dp->dp_kmsg);
	length = ROUND_WORD(length);
	if (length > PAGE_SIZE) {
		length = PAGE_SIZE;
	}

	/*
	 * Set vector, with either one or two pieces for kmsg.
	 */
	if (length > netipc_kmsg_first_half) {
		netvec_k[1].size = netipc_kmsg_first_half;
		netvec_k[2].size = length - netipc_kmsg_first_half;
		netvec_count = 3;
		netipc_kmsg_splits++;
	} else {
		netvec_k[1].size = length;
		netvec_count = 2;
	}

	/*
	 * Start the send, and the associated timer.
	 */
	netipc_send_with_timeout(remote, netvec_k, netvec_count,
				 (unsigned long) dp, dp->dp_seqid);
}

/*
 * Called from netipc_send_dp.
 *
 * Derived from netipc_send_kmsg and netipc_send_page.
 * Sends from a kmsg but uses page packets.
 */
void
netipc_send_kmsg_more(
	unsigned long	remote,
	netipc_packet_t	dp)
{
	vm_size_t first_half, length;
	vm_offset_t data, offset;
	TR_DECL("netipc_send_kmsg_more");

	assert(dp->dp_type == DP_TYPE_KMSG_MORE);
	assert(dp->dp_copy != VM_MAP_COPY_NULL);
	assert(dp->dp_copy->type == VM_MAP_COPY_PAGE_LIST);

	assert(netipc_thread_lock_held());

	/*
	 * Kmsgs are word aligned.
	 */
	assert(WORD_ALIGNED(dp->dp_kmsg));

	/*
	 * Calculate where in the kmsg to start,
	 * and how much to send.
	 */
	offset = PAGE_SIZE * dp->dp_copy_index + PAGE_SIZE;
	data = (vm_offset_t) dp->dp_kmsg + offset;
	length = kmsg_size(dp->dp_kmsg) - offset;
	length = ROUND_WORD(length);
	if (length > PAGE_SIZE) {
		length = PAGE_SIZE;
	}

	/*
	 * Fill in send_hdr_m.
	 */
	netipc_set_node_state(&send_hdr_m.type, remote);
	send_hdr_m.pg.pg_copy_offset = 0;
	send_hdr_m.pg.pg_msgh_offset = 0;
	send_hdr_m.pg.pg_page_first = (dp->dp_copy_index == 0);
	send_hdr_m.pg.pg_page_last = (dp->dp_copy_index ==
				      dp->dp_copy_npages - 1);
	send_hdr_m.pg.pg_copy_size = dp->dp_copy->size;
	send_hdr_m.pg.pg_copy_last = dp->dp_copy_last;
	send_hdr_m.seqid = dp->dp_seqid;
	send_hdr_m.pg.pg_first_seqid = dp->dp_first_seqid;
	send_hdr_m.pg.pg_ool_bytes = dp->dp_kmsg->ikm_ool_bytes;
#if	NETIPC_PAGE_TRACE
	send_hdr_m.pg.pg_remote_port = (unsigned long)
		dp->dp_remote_port->ip_norma_uid;
	send_hdr_m.pg.pg_copy_index = (unsigned long) dp->dp_copy_index;
	send_hdr_m.pg.pg_msgh_id = (unsigned long)
		dp->dp_kmsg->ikm_header.msgh_id;
	send_hdr_m.pg.pg_copy_npages = (unsigned long) dp->dp_copy_npages;
#endif

	/*
	 * If data crosses a page boundary, we need to point netvec_m
	 * to both physical pages involved.
	 */
	first_half = round_page(data) - data;
	if (length > first_half) {
		netvec_m[1].addr = KVTODEV(data);
		netvec_m[1].size = first_half;

		netvec_m[2].addr = KVTODEV(data + first_half);
		netvec_m[2].size = length - first_half;

		netipc_send_with_timeout(remote, netvec_m, 3,
					 (unsigned long) dp, dp->dp_seqid);
	} else {
		netvec_m[1].addr = KVTODEV(data);
		netvec_m[1].size = length;

		netipc_send_with_timeout(remote, netvec_m, 2,
					 (unsigned long) dp, dp->dp_seqid);
	}
}

/*
 * Called from netipc_send_dp.
 *
 * Derived from netipc_send_kmsg_more.
 * Sends from a kalloc'd region containing out-of-line ports,
 * but uses page packets.
 */
void
netipc_send_ool_ports(
	unsigned long	remote,
	netipc_packet_t	dp)
{
	vm_size_t first_half, length;
	vm_offset_t data, offset;
	TR_DECL("netipc_send_ool_ports");

	assert(netipc_thread_lock_held());
	assert(dp->dp_type == DP_TYPE_OOL_PORTS);
	assert(dp->dp_copy != VM_MAP_COPY_NULL);
	assert(dp->dp_copy->type == VM_MAP_COPY_PAGE_LIST);

	data = * (vm_offset_t *)
	    ((vm_offset_t) &dp->dp_kmsg->ikm_header + dp->dp_offset);

	/*
	 * Kalloc'd regions for out-of-line ports are word aligned.
	 */
	assert(WORD_ALIGNED(data));

	/*
	 * Calculate where in the kmsg to start,
	 * and how much to send.
	 */
	offset = PAGE_SIZE * dp->dp_copy_index;
	data += offset;
	length = dp->dp_copy->size - offset;
	length = ROUND_WORD(length);
	if (length > PAGE_SIZE) {
		length = PAGE_SIZE;
	}

	/*
	 * Fill in send_hdr_o.
	 */
	netipc_set_node_state(&send_hdr_o.type, remote);
	send_hdr_o.pg.pg_copy_offset = 0;
	send_hdr_o.pg.pg_msgh_offset = dp->dp_offset;
	send_hdr_o.pg.pg_page_first = (dp->dp_copy_index == 0);
	send_hdr_o.pg.pg_page_last = (dp->dp_copy_index ==
				      dp->dp_copy_npages - 1);
	send_hdr_o.pg.pg_copy_size = dp->dp_copy->size;
	send_hdr_o.pg.pg_copy_last = dp->dp_copy_last;
	send_hdr_o.seqid = dp->dp_seqid;
	send_hdr_o.pg.pg_first_seqid = dp->dp_first_seqid;
	send_hdr_o.pg.pg_ool_bytes = dp->dp_kmsg->ikm_ool_bytes;
#if	NETIPC_PAGE_TRACE
	send_hdr_o.pg.pg_remote_port = (unsigned long)
		dp->dp_remote_port->ip_norma_uid;
	send_hdr_o.pg.pg_copy_index = (unsigned long) dp->dp_copy_index;
	send_hdr_o.pg.pg_msgh_id = (unsigned long)
		dp->dp_kmsg->ikm_header.msgh_id;
	send_hdr_o.pg.pg_copy_npages = (unsigned long) dp->dp_copy_npages;
#endif
	/*
	 * If data crosses a page boundary, we need to point netvec_o
	 * to both physical pages involved.
	 */
	first_half = round_page(data) - data;
	if (length > first_half) {
		netvec_o[1].addr = KVTODEV(data);
		netvec_o[1].size = first_half;

		netvec_o[2].addr = KVTODEV(data + first_half);
		netvec_o[2].size = length - first_half;

		netipc_send_with_timeout(remote, netvec_o, 3,
					 (unsigned long) dp, dp->dp_seqid);
	} else {
		netvec_o[1].addr = KVTODEV(data);
		netvec_o[1].size = length;

		netipc_send_with_timeout(remote, netvec_o, 2,
					 (unsigned long) dp, dp->dp_seqid);
	}
}

/*
 * Called from netipc_send_dp.
 */
void
netipc_send_page(
	unsigned long	remote,
	netipc_packet_t	dp)
{
	vm_page_t *page_list;
	unsigned long align;
	unsigned long length;
	unsigned long offset;
	TR_DECL("netipc_send_page");

	assert(dp->dp_copy != VM_MAP_COPY_NULL);
	assert(dp->dp_copy->type == VM_MAP_COPY_PAGE_LIST);

	if (dp->dp_copy_index == 0) {
		dp->dp_page_list_base = 0;
		dp->dp_has_continuation =
		    vm_map_copy_has_interesting_cont(dp->dp_copy);
		dp->dp_being_continued = FALSE;
	}
	if (dp->dp_has_continuation) {
		netipc_send_page_with_continuation(remote, dp);
		return;
	}

	assert(netipc_thread_lock_held());
	assert(dp->dp_copy_index < dp->dp_copy_npages);

	/*
	 * Calculate length and offset.
	 * Round both to word boundaries.
	 */
#if	NORMA_REALIGN_OOL_DATA
	offset = (dp->dp_copy->offset & page_mask);
	align = (offset & WORD_MASK);
	if (dp->dp_copy_index < dp->dp_copy_npages - 1) {
		/*
		 * This is not the last page and therefore length will
		 * be a whole page. We just need to make offset word aligned.
		 */
		offset -= align;
		length = PAGE_SIZE;
	} else if (offset == 0) {
		/*
		 * Offset is page aligned and therefore word aligned.
		 * We just need to set length.
		 */
		length = (dp->dp_copy->size & page_mask);
		if (length == 0) {
			length = PAGE_SIZE;
		} else {
			length = ROUND_WORD(length);
		}
	} else {
		/*
		 * This is the last page, and this page list did not
		 * start on a page boundary.
		 *
		 * This code should correspond to the code in
		 * netipc_next_copy_object to calculate dp_copy_npages.
		 */
		vm_offset_t end = offset + dp->dp_copy->size;
		offset -= align;
		end = ROUND_WORD(end);
		length = ((end - offset) & page_mask);
		if (length == 0) {
			length = PAGE_SIZE;
		}
	}
#else	/* NORMA_REALIGN_OOL_DATA */
	offset = 0;
	align = dp->dp_copy->offset & page_mask;
	if (dp->dp_copy_index < dp->dp_copy_npages - 1) {
		length = PAGE_SIZE;
	} else {
		length = ((dp->dp_copy->size + align) & page_mask);
		if (length == 0) {
			length = PAGE_SIZE;
		} else {
			length = ROUND_WORD(length);
		}
	}
#endif	/* NORMA_REALIGN_OOL_DATA */

	assert(WORD_ALIGNED(offset));
	assert(WORD_ALIGNED(length));
	assert(length > 0);
	assert(length <= PAGE_SIZE);

#if	1
	/*
	 *	A bug in ipc_ether.c, or in the Ethernet driver,
	 *	occasionally leads to some fields in send_hdr_p
	 *	getting trashed.  The right thing to do is track
	 *	down and fix this bug but for the moment, because
	 *	the bug is machine-specific, we do the wrong thing.
	 */
	send_hdr_p.type = NETIPC_TYPE_PAGE;
	send_hdr_p.remote = node_self();
#endif
	netipc_set_node_state(&send_hdr_p.type, remote);
	send_hdr_p.pg.pg_copy_offset = align;
	send_hdr_p.pg.pg_msgh_offset = dp->dp_offset;
	send_hdr_p.pg.pg_copy_size = dp->dp_copy->size;
	send_hdr_p.pg.pg_page_first = (dp->dp_copy_index == 0);
	send_hdr_p.pg.pg_page_last = (dp->dp_copy_index ==
				      dp->dp_copy_npages - 1);
	send_hdr_p.pg.pg_copy_last = dp->dp_copy_last;
	send_hdr_p.seqid = dp->dp_seqid;
	send_hdr_p.pg.pg_first_seqid = dp->dp_first_seqid;
	send_hdr_p.pg.pg_ool_bytes = dp->dp_kmsg->ikm_ool_bytes;
#if	NETIPC_PAGE_TRACE
	send_hdr_p.pg.pg_remote_port = (unsigned long)
		dp->dp_remote_port->ip_norma_uid;
	send_hdr_p.pg.pg_copy_index = (unsigned long) dp->dp_copy_index;
	send_hdr_p.pg.pg_msgh_id = (unsigned long)
		dp->dp_kmsg->ikm_header.msgh_id;
	send_hdr_p.pg.pg_copy_npages = (unsigned long) dp->dp_copy_npages;
#endif
	/*
	 * If data crosses a page boundary, we need to point netvec_p
	 * to both physical pages involved.
	 */
	page_list = &dp->dp_copy->cpy_page_list[dp->dp_copy_index];
	if (offset + length > PAGE_SIZE) {
		vm_offset_t first_half = PAGE_SIZE - offset;

		netvec_p[1].addr = VPTODEV(page_list[0]) + offset;
		netvec_p[1].size = first_half;

		netvec_p[2].addr = VPTODEV(page_list[1]);
		netvec_p[2].size = length - first_half;

		netipc_send_with_timeout(remote, netvec_p, 3,
					 (unsigned long) dp, dp->dp_seqid);
	} else {
		netvec_p[1].addr = VPTODEV(page_list[0]) + offset;
		netvec_p[1].size = length;

		netipc_send_with_timeout(remote, netvec_p, 2,
					 (unsigned long) dp, dp->dp_seqid);
	}
}

/*
 * Like netipc_send_page, but can deal with copy objects with continuations.
 * Does not try to be tricky about changing allignment, which is okay, because
 * beginning/end page fragementation is less significant for the large copy
 * objects that typically have continuations.
 *
 * XXX
 * This turns out not to be that different from netipc_send_page,
 * so should probably remerge the two.
 */
void
netipc_send_page_with_continuation(
	unsigned long	remote,
	netipc_packet_t	dp)
{
	unsigned long align;
	unsigned long length;
	int index;
	TR_DECL("netipc_send_page_with_continuation");

	assert(netipc_thread_lock_held());
	assert(dp->dp_has_continuation);

	/*
	 * If we are currently being continued, return right away.
	 */
	if (dp->dp_being_continued) {
		printf3("being continued\n");
		return;
	}

	/*
	 * Calculate index into current page list from
	 * dp_copy_index, the current number of pages sent.
	 * If dp_copy is a continuation, these numbers won't be the same.
	 */
	index = dp->dp_copy_index - dp->dp_page_list_base;
	printf3("send_page_with_cont dp=0x%x c_idx=%d c_npages=%d idx=%d\n",
		dp, dp->dp_copy_index, dp->dp_copy_npages, index);
	assert(dp->dp_copy != VM_MAP_COPY_NULL);
	assert(dp->dp_copy->type == VM_MAP_COPY_PAGE_LIST);
	assert(index >= 0 && index <= dp->dp_copy->cpy_npages);

	/*
	 * We may be at the end of the current page list,
	 * in which case we need to call the copy continuation.
	 * We cannot do this ourselves, since the operation might
	 * block. We therefore let the netipc thread do it.
	 * It will call this routine again with dp_page_list_base reset.
	 */
	if (index == dp->dp_copy->cpy_npages) {
		netipc_safe_vm_map_copy_invoke_cont(dp);
		return;
	}

	/*
	 * Calculate length. Round to word boundary.
	 */
	align = dp->dp_copy->offset & page_mask;
	if (index < dp->dp_copy_npages - 1) {
		length = PAGE_SIZE;
	} else {
		length = ((dp->dp_copy->size + align) & page_mask);
		if (length == 0) {
			length = PAGE_SIZE;
		} else {
			length = ROUND_WORD(length);
		}
	}

	assert(WORD_ALIGNED(length));
	assert(length > 0);
	assert(length <= PAGE_SIZE);

#if	1
	/*
	 *	A bug in ipc_ether.c, or in the Ethernet driver,
	 *	occasionally leads to some fields in send_hdr_p
	 *	getting trashed.  The right thing to do is track
	 *	down and fix this bug but for the moment, because
	 *	the bug is machine-specific, we do the wrong thing.
	 */
	send_hdr_p.type = NETIPC_TYPE_PAGE;
	send_hdr_p.remote = node_self();
#endif
	netipc_set_node_state(&send_hdr_p.type, remote);
	send_hdr_p.pg.pg_copy_offset = align;
	send_hdr_p.pg.pg_msgh_offset = dp->dp_offset;
	send_hdr_p.pg.pg_copy_size = dp->dp_copy->size;
	send_hdr_p.pg.pg_page_first = (dp->dp_copy_index == 0);
	send_hdr_p.pg.pg_page_last = (dp->dp_copy_index ==
				      dp->dp_copy_npages - 1);
	send_hdr_p.pg.pg_copy_last = dp->dp_copy_last;
	send_hdr_p.seqid = dp->dp_seqid;
	send_hdr_p.pg.pg_first_seqid = dp->dp_first_seqid;
	send_hdr_p.pg.pg_ool_bytes = dp->dp_kmsg->ikm_ool_bytes;
#if	NETIPC_PAGE_TRACE
	send_hdr_p.pg.pg_remote_port = (unsigned long)
		dp->dp_remote_port->ip_norma_uid;
	send_hdr_p.pg.pg_copy_index = (unsigned long) dp->dp_copy_index;
	send_hdr_p.pg.pg_msgh_id = (unsigned long)
		dp->dp_kmsg->ikm_header.msgh_id;
	send_hdr_p.pg.pg_copy_npages = (unsigned long) dp->dp_copy_npages;
#endif
	
	netvec_p[1].addr = VPTODEV(dp->dp_copy->cpy_page_list[index]);
	netvec_p[1].size = length;

	netipc_send_with_timeout(remote, netvec_p, 2,
				 (unsigned long) dp, dp->dp_seqid);
}

/*
 * Advance dp->dp_copy to the next copy object in the list.
 */
void
netipc_next_copy_object(
	netipc_packet_t	dp)
{
    	mach_msg_descriptor_t	*saddr, *eaddr;
	ipc_kmsg_t kmsg = dp->dp_kmsg;
    	mach_msg_body_t		*body;

	assert(netipc_thread_lock_held());
	assert(dp->dp_offset >= sizeof(kmsg->ikm_header));
	assert(dp->dp_type == DP_TYPE_PAGE ||
	       dp->dp_type == DP_TYPE_KMSG_MORE ||
	       dp->dp_type == DP_TYPE_OOL_PORTS);

    	body = (mach_msg_body_t *) (&kmsg->ikm_header + 1);
    	saddr = (mach_msg_descriptor_t *) ((vm_offset_t) &kmsg->ikm_header + 
 							dp->dp_offset);
    	eaddr = (mach_msg_descriptor_t *) (body + 1) +
	  	                          body->msgh_descriptor_count;

	dp->dp_copy = VM_MAP_COPY_NULL;
	dp->dp_copy_index = 0;
	dp->dp_copy_last = TRUE;

	if (dp->dp_type == DP_TYPE_KMSG_MORE) {
		dp->dp_copy = &netipc_kmsg_more_copy;
		dp->dp_copy->type = VM_MAP_COPY_PAGE_LIST;
		dp->dp_copy->cpy_cont = VM_MAP_COPY_CONT_NULL;
		dp->dp_copy->cpy_cont_args = VM_MAP_COPYIN_ARGS_NULL;
		dp->dp_copy->size = kmsg_size(kmsg) - PAGE_SIZE;
		dp->dp_copy_npages = atop(round_page(dp->dp_copy->size));
		assert(dp->dp_copy->size > 0);
		assert(dp->dp_copy_npages > 0);
	} else if (dp->dp_offset > sizeof(kmsg->ikm_header) +
						sizeof(mach_msg_size_t)) {
		printf4("nextipc_next_copy_object: multiple copy objects\n");
		/* skip copy object to get to next type record */
		saddr++; 
		printf4("nextipc_next_copy_object: saddr=0x%x, eaddr=0x%x\n",
		       saddr, eaddr);
	}

    	for ( ; saddr <  eaddr; saddr++) {

	    if (* (vm_map_copy_t *) saddr == VM_MAP_COPY_NULL ||
		saddr->type.type == MACH_MSG_PORT_DESCRIPTOR) {

                /* There is no need to put inline ports in copy
		   object. As in case of typed messages, they should
		   be skipped in this procedure. It is important
		   that this code comes before next check for previous
		   copy */

		continue;
	    }
	    if (dp->dp_copy) {
	    	printf4("setting dp_copy_last false!\n");
		dp->dp_copy_last = FALSE;
		break;
	    }
	    switch (saddr->type.type) {
	    case MACH_MSG_OOL_PORTS_DESCRIPTOR : {
		mach_msg_ool_ports_descriptor_t	*dsc;
		vm_size_t length;
			
		dsc = &saddr->ool_ports;
		length = dsc->count * sizeof(mach_port_t);
		dp->dp_type = DP_TYPE_OOL_PORTS;
		dp->dp_copy = &netipc_ool_ports_copy;
		dp->dp_copy->type = VM_MAP_COPY_PAGE_LIST;
		dp->dp_copy->cpy_cont = VM_MAP_COPY_CONT_NULL;
		dp->dp_copy->cpy_cont_args = VM_MAP_COPYIN_ARGS_NULL;
		dp->dp_copy->size = length;
		dp->dp_copy_npages = atop(round_page(length));
		dp->dp_offset = (unsigned long) &dsc->address - (vm_offset_t)&kmsg->ikm_header;
		assert(dp->dp_copy->size > 0);
		break;
	    }
	    case MACH_MSG_OOL_DESCRIPTOR : {
		mach_msg_ool_descriptor_t *dsc;
		vm_size_t length;

		dsc = &saddr->out_of_line;
		length = dsc->size;
		if (length == 0) {
			dsc->address = 0;
			continue;
		}

		dp->dp_copy = (vm_map_copy_t) dsc->address;
		
		/* hygiene: implies you can never go back!  */
		dsc->address = (void *)0;
		dp->dp_offset = (unsigned long) &dsc->address - (vm_offset_t) &kmsg->ikm_header;
		dp->dp_type = DP_TYPE_PAGE;

		assert(dp->dp_copy != VM_MAP_COPY_NULL);
		assert(dp->dp_copy->type == VM_MAP_COPY_PAGE_LIST);
		assert(dp->dp_copy->size == length);

		if (vm_map_copy_has_interesting_cont(dp->dp_copy)) {
			/*
			 * This copy object has a continuation,
			 * which means that we won't change alignment,
			 * thus dp_copy_npages, the number of pages that
			 * the copy object will have on the destination,
			 * is the same as the number of pages that it
			 * has here. We cannot use dp->dp_copy->cpy_npages
			 * since that is just the number of pages in the
			 * first page list in the copy object.
			 */
/*			panic("0x%x has continuation\n", dp->dp_copy);*/
			dp->dp_copy_npages =
			    atop(round_page(dp->dp_copy->offset +
					    dp->dp_copy->size) -
				 trunc_page(dp->dp_copy->offset));
			assert(dp->dp_copy_npages >= dp->dp_copy->cpy_npages);
		} else {
			/*
			 * This copy object does not have a continuation,
			 * and therefore things are simple enough that we
			 * will bother to change alignment if we can send
			 * the copy object in one fewer pages than it
			 * currently occupies, which is possible when the
			 * total amount used by the first and last pages
			 * is no larger than a page, after taking word
			 * alignment into account.
			 */
			vm_offset_t offset = dp->dp_copy->offset & page_mask;
			vm_offset_t end = offset + dp->dp_copy->size;
#if	NORMA_REALIGN_OOL_DATA
			offset = TRUNC_WORD(offset);
			end = ROUND_WORD(end);
			dp->dp_copy_npages = atop(round_page(end - offset));
#else	/* NORMA_REALIGN_OOL_DATA */
			dp->dp_copy_npages = atop(round_page(end) -
						  trunc_page(offset));
#endif	/* NORMA_REALIGN_OOL_DATA */
			assert(dp->dp_copy_npages ==
			       dp->dp_copy->cpy_npages ||
			       dp->dp_copy_npages ==
			       dp->dp_copy->cpy_npages - 1);
		}
		break;
	    }
	    default: {
		panic("netipc_next_copy_object(): bad descriptor");
	    }
	}
	assert(dp->dp_copy);
    }
}

/*
 * This routine is overly general because it was written before it was clear
 * that copy object page lists cannot be backed up once their continuation
 * has been called.
 *
 * Note: dp_last_seqid is set when we first visit it.
 */
unsigned long netipc_ikm_free_dp_copies = 0;

void
netipc_set_seqid(
	netipc_packet_t	dp,
	unsigned long	seqid)
{
	TR_DECL("netipc_set_seqid");
	assert(netipc_thread_lock_held());

	/*
	 * Are we there already?
	 */
	if (dp->dp_seqid == seqid) {
		return;
	}

	/*
	 * We must be in the correct dp.
	 */
	assert(dp->dp_first_seqid <= seqid);
	assert(dp->dp_last_seqid == 0 || seqid <= dp->dp_last_seqid);

	/*
	 * If we want to be at the kmsg, go there.
	 */
	if (dp->dp_first_seqid == seqid) {
		dp->dp_type = DP_TYPE_KMSG;
		dp->dp_seqid = seqid;
		return;
	}
	  
	/*
	 * If we are in the right copy object, just change the index.
	 */
	if (dp->dp_type != DP_TYPE_KMSG) {
		int index = dp->dp_copy_index + (seqid - dp->dp_seqid);
		if (index >= 0 && index < dp->dp_copy_npages) {
			dp->dp_copy_index = index;
			dp->dp_seqid = seqid;
			return;
		}
	}

#if	1
	assert(dp->dp_seqid <= seqid);	/* ZZZ */
#else
	/*
	 * N.B.  While this code used to work, we have changed
	 * an important assumption.  After a connection has
	 * been suspended and resumed, rather than incrementing
	 * dp_first_seqid to the next valid seqid, we leave it
	 * alone so that pages tagged with first_seqid can be
	 * matched by the receiver to a pending kmsg.  This change
	 * invalidates the code below.  If this code needs to be
	 * reactivated, we could tag pages with a dp_kmsg_first_seqid
	 * as part of the pginfo structure.
	 */
	/*
	 * We might be too far forward and thus need to back up.
	 * The easiest way of backing up is to start at the kmsg
	 * and walk forward. This isn't necessary the most efficient way!
	 *
	 * Note that this cannot happen if this is a simple message.
	 *
	 * XXX
	 * Page-list continuations limit how far we can back up.
	 */
	if (dp->dp_seqid > seqid) {
		dp->dp_seqid = dp->dp_first_seqid;
		assert(dp->dp_first_seqid < seqid);
	}
#endif

	/*
	 * If we are currently at the kmsg, advance to the first copy object.
	 * Otherwise, advance seqid to next copy object.
	 *
	 * XXX where do we make a fast check for simple messages?
	 */
	if (dp->dp_seqid == dp->dp_first_seqid) {
		dp->dp_seqid++;
		if (kmsg_size(dp->dp_kmsg) > PAGE_SIZE) {
			dp->dp_type = DP_TYPE_KMSG_MORE;
		} else {
			dp->dp_type = DP_TYPE_PAGE;
		}
		dp->dp_offset = sizeof(dp->dp_kmsg->ikm_header)
					+ sizeof(mach_msg_size_t);
	} else {
		printf4(">> %d ", dp->dp_seqid);
		dp->dp_seqid -= dp->dp_copy_index; /* beginning of this obj */
		printf4(">> %d ", dp->dp_seqid);
		dp->dp_seqid += dp->dp_copy_npages; /* begin of next */
		printf4("-> %d\n", dp->dp_seqid);
	}

	/*
	 * Examine each copy object to see whether it contains seqid.
	 * If it does, set index appropriately and return.
	 *
	 * XXX
	 * This should no longer be a for loop. We should only
	 * need to walk to the next copy object.
	 */
	for (;;) {
		vm_map_copy_t   old_copy = dp->dp_copy;

		tr2("starting with copy 0x%x",dp->dp_copy);
		netipc_next_copy_object(dp);
		tr2("next changes to 0x%x",dp->dp_copy);
		if (dp->dp_copy_last && dp->dp_last_seqid == 0) {
			dp->dp_last_seqid =
			    dp->dp_seqid + dp->dp_copy_npages - 1;
		}
		assert(seqid >= dp->dp_seqid);
		if (seqid < dp->dp_seqid + dp->dp_copy_npages) {
			dp->dp_copy_index = seqid - dp->dp_seqid;
			dp->dp_seqid = seqid;
			/*
			 * hygiene:  Discard previous copy object when done
			 */
			if (old_copy && old_copy != dp->dp_copy &&
			    old_copy->type == VM_MAP_COPY_PAGE_LIST &&
			    old_copy != &netipc_kmsg_more_copy &&
			    old_copy != &netipc_ool_ports_copy) {
				netipc_ikm_free_dp_copies++;
				tr2("freeing 0x%x",old_copy);
				netipc_safe_vm_map_copy_discard(old_copy);
			}
			tr2("return copy 0x%x",dp->dp_copy);
			return;
		}
		assert(! dp->dp_copy_last);
		dp->dp_seqid += dp->dp_copy_npages;
	}
}

unsigned int c_reconstruct_suspended_dp=0;
unsigned int c_reconstruct_drop_port=0;
unsigned int c_reconstruct_suspended_packet = 0;
unsigned int c_reconstruct_suspended_port_list = 0;
unsigned int c_reconstruct_suspended_port_first = 0;
/* 
 * Called to reconstruct one dp out of the current dp and a suspended dp.
 */
boolean_t
reconstruct_suspended_dp(
	netipc_packet_state_t	ds)
{
	netipc_packet_t		dp, olddp;
	unsigned long		seqid_diff, next_seqid;
	ipc_port_t		port, other_ports;
	struct ipc_kmsg_queue	*kmsgs;
	boolean_t		have_suspended_packet;
	TR_DECL("reconstruct_suspended_dp");;

	tr2("entry, ds 0x%x", ds);
	assert(ds->state == STATE_RESUME);
	assert(netipc_thread_lock_held());
	++c_reconstruct_suspended_dp;

	assert(ds->suspended_packet != NETIPC_PACKET_NULL ||
	       ds->suspended_ports != IP_NULL);

	have_suspended_packet = FALSE;
	if (ds->suspended_packet != NETIPC_PACKET_NULL) {
		tr2("suspended packet 0x%x", ds->suspended_packet);
		++c_reconstruct_suspended_packet;
		/* 
		 * Save current dp and put suspended dp back into the state 
		 * as the current dp
		 */
		olddp = ds->netipc_packet;
		dp = ds->suspended_packet;
		ds->suspended_packet = NETIPC_PACKET_NULL;
		ds->netipc_packet = dp;
		assert(dp->dp_remote_port->ip_norma_queue_next
		       == (ipc_port_t) -666);

		assert(dp->dp_last_seqid > 0 || dp->dp_copy_last == FALSE);
		assert(olddp==NETIPC_PACKET_NULL ||
		       dp->dp_remote==olddp->dp_remote);

		/*
		 * Figure out how much we have missed
		 */
		next_seqid = Xnetipc_next_seqid(dp->dp_remote);
		seqid_diff = next_seqid - dp->dp_seqid;
		have_suspended_packet = TRUE;
		tr2("setting next_seqid 0x%x", next_seqid);

		/*
		 * Now add that difference to all the sequid's
		 */
		dp->dp_seqid = next_seqid;

		/*
		 * Under other circumstances, adjusting the dp_first_seqid
		 * by seqid_diff would be a necessary thing to do.  However,
		 * dp_first_seqid is used by the receiver to match an
		 * incoming page to an existing kmsg.  Changing the value
		 * will cause the receiver to miss the page.
		 */

		if (dp->dp_last_seqid)
			dp->dp_last_seqid = dp->dp_last_seqid + seqid_diff;
		dp->dp_last_unacked = dp->dp_seqid;
	
		/* 
		 * Queue the port from the current dp as the next port on the
		 * dp that we are unsuspending.
		 */
		if (olddp != NETIPC_PACKET_NULL) {
			tr2("queueing port 0x%x from olddp",
			    olddp->dp_remote_port);
			port = olddp->dp_remote_port;
			kmsgs = &port->ip_messages.imq_messages;
	
			/*
			 * If the port that was in progress has 
			 * no more kmsgs queued, then dequeue it
			 */
			if (ipc_kmsg_queue_empty(kmsgs)){
				++c_reconstruct_drop_port;
				port = olddp->dp_remote_port->ip_norma_queue_next;
				ip_lock(olddp->dp_remote_port);
				norma_ipc_unqueue(olddp->dp_remote_port);
				ip_unlock(olddp->dp_remote_port);
			}

			dp->dp_remote_port->ip_norma_queue_next = port;
		} else {
			dp->dp_remote_port->ip_norma_queue_next = IP_NULL;
			tr1("no olddp remote port to enqueue");
		}
		assert(dp->dp_remote_port->ip_norma_queue_next
		       != (ipc_port_t) -666);
	}

	ds->state = STATE_CLEAR;

	/*
	 *	Shuffle any suspended ports from
	 *	the suspended port list back to the
	 *	netipc_packet from whence they came.
	 */
	if (ds->suspended_ports != IP_NULL) {
		ipc_port_t	port, next;

		tr2("suspended ports 0x%x", ds->suspended_ports);
		dp = ds->netipc_packet;
		netipc_mark_unsuspended(ds->suspended_ports);
		if (dp != NETIPC_PACKET_NULL) {
			tr2("append to &dp_remote_port 0x%x",
			    &dp->dp_remote_port);
			++c_reconstruct_suspended_port_list;
			netipc_append_port_list(&dp->dp_remote_port,
						ds->suspended_ports);
		} else {
			tr1("reconstruct suspended port first");
			++c_reconstruct_suspended_port_first;
			port = ds->suspended_ports;
			/*
			 *	Allocate and initialize a dp.
			 */
			dp = netipc_packet_allocate(NETIPC_PACKET_DEFER);
			if (dp == NETIPC_PACKET_NULL) {
				/*
				 * packets processing of suspended ports will
				 * take place as soon as netipc_packets will
				 * have been made available.
				 */
				return FALSE;
			}
			ip_lock(port);
			norma_ipc_queue_first_port(port, dp,
						   port->ip_norma_queue_next);
			ds->suspended_ports = IP_NULL;
			assert(ds->netipc_packet != NETIPC_PACKET_NULL);
			ds->netipc_packet->dp_reconstruct_complete = TRUE;
			return TRUE;
			
		}
		if (have_suspended_packet == FALSE) {
			/*
			 *	Create sequence numbers and queue
			 *	a new kmsg.
			 */
			(void) dp_advance(dp);
		}
		ds->suspended_ports = IP_NULL;
	}

	assert(dp != NETIPC_PACKET_NULL);
	dp->dp_reconstruct_complete = TRUE;
	tr1("exit");
	return TRUE;
}

void
netipc_mark_unsuspended(
	ipc_port_t	port_list)
{
	ipc_port_t	port;

	assert(netipc_thread_lock_held());
	for (port = port_list; port; port = port->ip_norma_queue_next) {
		assert(port->ip_norma_suspended == TRUE);
		port->ip_norma_suspended = FALSE;
	}
}


/*
 *	Delete a port from a list of ports.
 *	Port must be present in the list.
 */
void
netipc_remove_from_port_list(
	ipc_port_t	port,
	ipc_port_t	*port_list)
{
	ipc_port_t	*p;

	assert(netipc_thread_lock_held());
	for (p = port_list; *p; p = &((*p)->ip_norma_queue_next))
		if (*p == port) {
			(*p) = port->ip_norma_queue_next;
			return;
		}
	panic("netipc_remove_from_port_list");
}


void
netipc_append_port_list(
	ipc_port_t	*new_list,
	ipc_port_t	old_list)
{
	ipc_port_t	port;
	TR_DECL("netipc_append_port_list");

	assert(netipc_thread_lock_held());
	tr3("entry, new_list 0x%x old_list 0x%x", new_list, old_list);
	if (*new_list != IP_NULL) {
		for (port = *new_list;
		     port->ip_norma_queue_next;
		     port = port->ip_norma_queue_next) {
			continue;
		}
		port->ip_norma_queue_next = old_list;
	} else {
		*new_list = old_list;
	}
}


/*
 * Called when dp->dp_kmsg has been completely sent, and it's time
 * to move to the next kmsg destined for dp->dp_remote, either from
 * the current port or from the next one.
 *
 * Returns true if there is still something to send.
 */
static boolean_t
dp_advance(
	netipc_packet_t			dp)
{
	ipc_kmsg_t			kmsg;
	struct ipc_kmsg_queue		*kmsgs;
	netipc_packet_state_t		ds;
	unsigned long			remote;
	TR_DECL("dp_advance");

	remote = dp->dp_remote;
	ds = &netipc_packet_state[remote];

	tr2("entry, remote 0x%x", remote);
	assert(netipc_thread_lock_held());

	/*
	 * Check to see if we should do a resume
	 * of a suspended port
	 */
	if (ds->state != STATE_CLEAR) {
		return reconstruct_suspended_dp(ds);
	}

	assert(dp != NETIPC_PACKET_NULL);

	/*
	 * Find next kmsg on the current port.
	 */
	ip_lock(dp->dp_remote_port);
	imq_lock(&dp->dp_remote_port->ip_messages);
	kmsgs = &dp->dp_remote_port->ip_messages.imq_messages;
	kmsg = ipc_kmsg_queue_first(kmsgs);

	/*
	 * If there are no more kmsgs on this port,
	 * move to the next port and check there.  Also move on if
	 * the port is no longer a proxy; this happens if the receive
	 * right got moved here.
	 */
	if (kmsg == IKM_NULL || !IP_NORMA_IS_PROXY(dp->dp_remote_port)) {
		ipc_port_t port;
		
		/*
		 * If there are no more ports waiting to send
		 * to this node, reset and return.
		 */
		imq_unlock(&dp->dp_remote_port->ip_messages);
		port = dp->dp_remote_port->ip_norma_queue_next;
		norma_ipc_unqueue(dp->dp_remote_port);
		ip_unlock(dp->dp_remote_port);
		if (port == IP_NULL) {
			unsigned long remote = dp->dp_remote;
			netipc_packet_deallocate(dp);
			netipc_packet_state[remote].netipc_packet =
				NETIPC_PACKET_NULL;
			return FALSE;
		}
		dp->dp_remote_port = port;
			
		/*
		 * Find first kmsg on the new port.
		 */
		ip_lock(dp->dp_remote_port);
		imq_lock(&port->ip_messages);
		kmsgs = &port->ip_messages.imq_messages;
		kmsg = ipc_kmsg_queue_first(kmsgs);
	}
	
	/*
	 * Remove the kmsg from the port.
	 */
	assert(kmsg != IKM_NULL);
	ipc_kmsg_rmqueue_first_macro(kmsgs, kmsg);
	dp->dp_remote_port->ip_msgcount--;
	imq_unlock(&dp->dp_remote_port->ip_messages);
	ip_unlock(dp->dp_remote_port);
		
	/*
	 * Reset dp and return success.
	 */
	dp->dp_kmsg = kmsg;
	dp->dp_type = DP_TYPE_KMSG;
	dp->dp_first_seqid = Xnetipc_next_seqid(dp->dp_remote);
	dp->dp_last_unacked = dp->dp_first_seqid;
	if (is_one_page_kmsg(kmsg)) {
		dp->dp_last_seqid = dp->dp_first_seqid;
	} else {
		dp->dp_last_seqid = 0;
	}

	return TRUE;
}

/*
 * Free kmsg and last copy object associated with dp->dp_kmsg.
 */
void
dp_finish(
	netipc_packet_t	dp)
{
	register ipc_kmsg_t kmsg = dp->dp_kmsg;
	mach_msg_bits_t bits;
	TR_DECL("dp_finish");

	assert(netipc_thread_lock_held());
	
	/*
	 * Queue kmsg to be freed, after getting bits and storing remote port.
	 */
	printf2("-free %d..%d\n", dp->dp_first_seqid, dp->dp_last_seqid);
	bits = kmsg->ikm_header.msgh_bits;
	kmsg->ikm_header.msgh_remote_port = (mach_port_t) dp->dp_remote_port;

	/*
	 * If there is a thread waiting in norma_ipc_send for the kmsg to be
	 * sent, wake it up and let it free the kmsg.
	 */
	if (kmsg->ikm_wait_for_send) {
		kmsg->ikm_send_completed = TRUE;
		thread_wakeup((int)kmsg);
	} else {
		/*
		 * If a fake proxy was used to forward messages of a migrated
		 * receive right port, then it must be deallocated if it is its
		 * last kmsg.
		 */
		if (kmsg->ikm_forward_proxy != IP_NULL) {
			ip_lock(kmsg->ikm_forward_proxy);
			if (--kmsg->ikm_forward_proxy->ip_references == 1) {
				ip_unlock(kmsg->ikm_forward_proxy);
				ipc_port_dealloc_special(
						kmsg->ikm_forward_proxy,
						ipc_space_remote);
			} else
				ip_unlock(kmsg->ikm_forward_proxy);
		}
		netipc_safe_ikm_free(kmsg);
	}
#if	NORMA_PROXY_PORT_QUEUE_LIMIT
	/*
	 *	This wakeup should no longer be necessary.
	 *	Delete and confirm.  XXX alanl@osf.org
	 */
	thread_wakeup((int) dp->dp_remote_port);
#endif	/* NORMA_PROXY_PORT_QUEUE_LIMIT */

	/*
	 * Discard last copy object.
	 */
	if (bits & MACH_MSGH_BITS_COMPLEX_DATA) {
		if (! dp->dp_copy) {
			netipc_set_seqid(dp, dp->dp_last_seqid);
			assert(dp->dp_copy != VM_MAP_COPY_NULL);
			assert(dp->dp_copy->type == VM_MAP_COPY_PAGE_LIST);
		}
		if (dp->dp_copy != &netipc_kmsg_more_copy &&
		    dp->dp_copy != &netipc_ool_ports_copy) {
			tr2("freeing copy 0x%x",dp->dp_copy);
			netipc_safe_vm_map_copy_discard(dp->dp_copy);
			dp->dp_copy = VM_MAP_COPY_NULL; /* hygiene */
		}
	}
	
}


/*
 * Received successful ack of seqid.
 */
void
netipc_recv_success(
	netipc_packet_t	dp,
	unsigned long	seqid)
{
	assert(netipc_thread_lock_held());
	assert(seqid == dp->dp_last_unacked);
	dp->dp_last_unacked++;
	if (dp->dp_last_seqid && dp->dp_last_seqid == seqid) {
		dp_finish(dp);
		if (dp_advance(dp)) {
			netipc_start(dp->dp_remote);
		}
	} else {
		netipc_start(dp->dp_remote);
	}
}

void
netipc_recv_retarget(
	netipc_packet_t	dp,
	unsigned long	seqid,
	unsigned long	new_remote)
{
	ipc_port_t remote_port;

	assert(netipc_thread_lock_held());
	assert(seqid == dp->dp_first_seqid);

	/*
	 * Handle acknowledgement stuff, and find port.
	 */
	remote_port = netipc_dequeue_port(dp);
	if (remote_port == IP_NULL) {
		return;
	}

	/*
	 * Reset destination node field of destination port.
	 * If new destination is still remote, then start a send;
	 * otherwise, queued messages will be absorbed automatically by
	 * norma_ipc_receive_rright.
	 */
	ip_lock(remote_port);
	remote_port->ip_norma_dest_node = new_remote;
	if (new_remote != node_self()) {
		assert(IP_NORMA_IS_PROXY(remote_port));
		norma_ipc_queue_port(remote_port, FALSE); /* consumes lock */
	} else
		ip_unlock(remote_port);
}

void
netipc_recv_dead(
	netipc_packet_t	dp,
	unsigned long	seqid)
{
	ipc_port_t remote_port;

	assert(netipc_thread_lock_held());
	assert(seqid == dp->dp_first_seqid);

	/*
	 * Handle acknowledgement stuff, and find port.
	 */
	remote_port = netipc_dequeue_port(dp);
	if (remote_port == IP_NULL) {
		return;
	}
	printf1("*** netipc_recv_dead! 0x%x:%x\n",
	       remote_port, remote_port->ip_norma_uid);

	/*
	 * Put the port on the dead port list, so that the netipc thread
	 * can find it and call norma_ipc_destroy_proxy.
	 *
	 * Using ip_norma_queue_next makes the port look
	 * like it's already queued. This will prevent norma_ipc_queue_port
	 * from sticking it on the queue again and starting another send.
	 */
	ip_lock(remote_port);
	assert(! norma_ipc_queued(remote_port));
	remote_port->ip_norma_is_zombie = TRUE;
	remote_port->ip_norma_queued = TRUE;
	ip_unlock(remote_port);
	remote_port->ip_norma_queue_next = netipc_dead_port_list;
	netipc_dead_port_list = remote_port;
	netipc_thread_wakeup();
}

void
netipc_recv_not_found(
	netipc_packet_t	dp,
	unsigned long	seqid)
{
	assert(netipc_thread_lock_held());
	/*
	 * XXX For now, we handle this as if the port had died.
	 */
	printf1("netipc_recv_not_found!\n");
	netipc_recv_dead(dp, seqid);
}


/*
 * Called when we receive an ack from another node 
 * telling us that they are short on memory and do not want to process
 * the packet in transit at this time. After receiving
 * this message we need to take action to be able to send 
 * the node new data, that it might find more important then what
 * we are currently sending. It would be nice to know what packets they 
 * want, and in fact they really only want messages to/from the 
 * default pager, but as we can not tell what messages those
 * are, we will leave it to the destination node to filter the packets.
 */
int	c_netipc_recv_dp_suspend_dp = 0;
int	c_netipc_suspend_port = 0;
int	c_netipc_suspend_no_packet = 0;

void
netipc_recv_resource_shortage(
	netipc_packet_t	dp,
	unsigned long	seqid,
	unsigned long	data)
{
	ipc_port_t		port;
	netipc_packet_t		newdp;
	ipc_kmsg_t		kmsg;
	struct ipc_kmsg_queue	*kmsgs;
	unsigned long		remote = dp->dp_remote;
	netipc_packet_state_t	ds = &netipc_packet_state[remote];
	TR_DECL("netipc_recv_resource_shortage");

	assert(netipc_thread_lock_held());
	tr3("entry, remote 0x%x ds->state 0x%x", remote, ds->state);

	/*
	 *	If we will have to suspend the packet, try
	 *	to allocate a new packet structure now.
	 *	If we can't allocate a new packet structure,
	 *	then we effectively ignore the SUSPEND request.
	 *	We will eventually send a SYNC to the receiver,
	 *	who will respond with another SUSPEND, at which
	 *	point we will retry the packet allocation.  Note
	 *	that netipc_packet_allocate will schedule a thread
	 *	to refill the netipc packet list.
	 */
	if (seqid != dp->dp_first_seqid) {
		newdp = netipc_packet_allocate(NETIPC_PACKET_ABORT);
		if (newdp == NETIPC_PACKET_NULL) {
			++c_netipc_suspend_no_packet;
			netipc_pretend_no_ack(dp->dp_remote,
					      (unsigned long) dp, seqid);
			return;
		}
	}

	ds->state = STATE_CLEAR;
	pcs_suspend_node(remote);

	/* 
	 * If it is the first packet containing the kmsg 
	 * We can just take it off the queue, and save it for later
	 */
	if (seqid == dp->dp_first_seqid) {
		netipc_suspend_port(ds);
		return;
	}

	/* 
	 * At this point we are in the middle of a kmsg in transit. 
	 * This packet in progress is one of n pages. The remote node
	 * will not accept more packets from it due to memory problems.
	 * We need to suspend the current packet and then create a new
	 * dp in order to send messages from other ports that the remote node
	 * might need in order to page. We can only suspend one packet
	 * per remote node. 
	 */
	tr1("in the middle of a kmsg's pages");
	++c_netipc_recv_dp_suspend_dp;
	assert(ds->suspended_packet == NETIPC_PACKET_NULL);
	ds->suspended_packet = ds->netipc_packet;

	assert(newdp != NETIPC_PACKET_NULL);
	newdp->dp_being_continued = FALSE;
	newdp->dp_has_continuation = FALSE;
	newdp->dp_remote = remote;
	ds->netipc_packet = newdp;
	dp->dp_reconstruct_complete = FALSE;

	/*
	 * A lot of the code below was stolen from dp_advance. 
	 * We should look at cutting down the redundancy
	 */

	/*
	 * If there are no more ports waiting to send
	 * to this node, reset and return.
	 */
	port = dp->dp_remote_port->ip_norma_queue_next;
	ip_lock(dp->dp_remote_port);
 	norma_ipc_suspend(dp->dp_remote_port);
	ip_unlock(dp->dp_remote_port);
 	if (port == IP_NULL) {
		netipc_packet_deallocate(newdp);
		ds->netipc_packet = NETIPC_PACKET_NULL;
		return;
	}

	/* 
	 * Stuff the next port from the old dp into the new dp 
	 */
	newdp->dp_remote_port = port;

	/*
	 * Find first kmsg on the new port.
	 */
	ip_lock(port);
	imq_lock(&port->ip_messages);
	kmsgs = &port->ip_messages.imq_messages;
	kmsg = ipc_kmsg_queue_first(kmsgs);

	/*
	 * Remove the kmsg from the port.
	 */
	assert(kmsg != IKM_NULL);
	ipc_kmsg_rmqueue_first_macro(kmsgs, kmsg);
	newdp->dp_remote_port->ip_msgcount--;
	imq_unlock(&port->ip_messages);
	ip_unlock(port);

	/*
	 * Setup new dp and return.
	 */
	newdp->dp_kmsg = kmsg;
	newdp->dp_type = DP_TYPE_KMSG;
	newdp->dp_first_seqid = Xnetipc_next_seqid(remote);
	newdp->dp_last_unacked = newdp->dp_first_seqid;

	if (is_one_page_kmsg(kmsg)) {
		newdp->dp_last_seqid = newdp->dp_first_seqid;
	} else {
		newdp->dp_last_seqid = 0;
	}

	/*
	 * Start sending the new packet if we can 
	 */
	netipc_start(remote);
	return;
}


/*
 *	Suspend the port on the front of this node's
 *	port list.
 */
void
netipc_suspend_port(
	netipc_packet_state_t	ds)
{
	ipc_port_t	remote_port;
	ipc_port_t	port;
	netipc_packet_t	dp;
	TR_DECL("netipc_suspend_port");

	assert(netipc_thread_lock_held());
	tr2("entry, remote 0x%x", ds->netipc_packet->dp_remote);
	++c_netipc_suspend_port;

	dp = ds->netipc_packet;
	remote_port = netipc_dequeue_port(dp);
	ip_lock(remote_port);

	/*
	 *	Oh me oh my.  memory_object_data_write_completed
	 *	messages can get stuck behind other messages.
	 *	We can't allow a port with such messages to be
	 *	suspended.  As a very important side-effect,
	 *	norma_critical_port reorders the messages on the
	 *	port, putting any completed messages first.
	 *	Of course, this little	hack is not safe for any
	 *	other message -- so don't try this at home, kids.
	 */
	if (norma_critical_port(remote_port) == TRUE) {
		norma_ipc_queue_port(remote_port, FALSE); /* consumes lock */
		return;
	}

	/*
	 *	Nulling out the queue_next field has the effect
	 *	of putting on a limbo queue - it will not get
	 *	queued up while in this state.
	 */
	remote_port->ip_norma_queued = TRUE;
	ip_unlock(remote_port);

	remote_port->ip_norma_queue_next = IP_NULL;
	remote_port->ip_norma_suspended = TRUE;
	port = ds->suspended_ports;
	if (port) {
		for (port = ds->suspended_ports;
		     port->ip_norma_queue_next;
		     port = port->ip_norma_queue_next) {
			assert(port->ip_norma_suspended == TRUE);
			continue;
		}
		port->ip_norma_queue_next = remote_port;
	} else {
		ds->suspended_ports = remote_port;
	}
}

int	c_netipc_recv_ack_kernel_short = 0;

void
netipc_recv_ack_with_status(
	unsigned long	packetid,
	unsigned long	seqid,
	kern_return_t	status,
	unsigned long	data)
{
	netipc_packet_t dp;

	dp = (netipc_packet_t) packetid;
	assert(dp != NETIPC_PACKET_NULL);
	dp->dp_reconstruct_complete = FALSE;
	if (status == KERN_SUCCESS) {
		netipc_recv_success(dp, seqid);
	} else if (status == KERN_NOT_RECEIVER) {
		netipc_recv_retarget(dp, seqid, data);
	} else if (status == KERN_INVALID_RIGHT) {
		netipc_recv_dead(dp, seqid);
	} else if (status == KERN_INVALID_NAME) {
		netipc_recv_not_found(dp, seqid);
	} else if (status == KERN_RESOURCE_SHORTAGE) {
		netipc_recv_resource_shortage(dp, seqid, data);
		++c_netipc_recv_ack_kernel_short;
	} else {
		panic("status %d from receive_dest\n", status);
	}
}


/*
 *	A memory-short node sent us a resume notification.
 *	Unwind all the suspended transmission state.  It is
 *	entirely possible for the other node to send this
 *	node multiple resume messages, but the right thing
 *	to do every time is to re-activate any suspended
 *	ports or a suspended packet.  On a multiprocessor,
 *	reactivation may be annoyingly racy.	XXX
 */
unsigned long	c_netipc_recv_resume_bogus = 0;

void
netipc_recv_resume_node(
	unsigned long		remote)
{
	netipc_packet_state_t	ds;
	netipc_packet_t		dp;
	TR_DECL("netipc_recv_resume_node");

	tr2("entry, remote 0x%x", remote);
	assert(netipc_thread_lock_held());
	ds = &netipc_packet_state[remote];
	if (ds->suspended_packet == NETIPC_PACKET_NULL &&
	    ds->suspended_ports == IP_NULL) {
		++c_netipc_recv_resume_bogus;
		return;
	}
	tr2("resuming packet 0x%x", ds->netipc_packet);
	ds->state = STATE_RESUME;
	/*
	 *	No transmission in progress.  Reconstruct
	 *	the state of the earlier, suspended world
	 *	and kick-start the next transmission.
	 */
		if (ds->netipc_packet == NETIPC_PACKET_NULL &&
		    reconstruct_suspended_dp(ds))
			netipc_start(remote);
}

void
netipc_send_dp(
	unsigned long	remote,
	netipc_packet_t	dp)
{
	TR_DECL("netipc_send_dp");

	assert(dp->dp_being_continued == FALSE);
	assert(dp->dp_type >= DP_TYPE_KMSG && dp->dp_type < DP_TYPE_MAX);
	netipc_out_pkt_stats[dp->dp_type]++;
	if (dp->dp_type == DP_TYPE_KMSG) {
		tr4("kmsg msgh_id 0x%x rem 0x%x seq 0x%x",
		    dp->dp_kmsg->ikm_header.msgh_id, remote,
		    dp->dp_seqid);
		tr3("uid 0x%x kmsg size 0x%x",
		    dp->dp_remote_port->ip_norma_uid, dp->dp_kmsg->ikm_size);
		netipc_send_kmsg(remote, dp);
	} else if (dp->dp_type == DP_TYPE_PAGE) {
		tr4("page rem 0x%x seq 0x%x uid 0x%x", remote,
		    dp->dp_seqid, dp->dp_remote_port->ip_norma_uid);
		netipc_send_page(remote, dp);
	} else if (dp->dp_type == DP_TYPE_KMSG_MORE) {
		tr4("kmsg_more rem 0x%x seq 0x%x uid 0x%x", remote,
		    dp->dp_seqid, dp->dp_remote_port->ip_norma_uid);
		netipc_send_kmsg_more(remote, dp);
	} else {
		assert(dp->dp_type == DP_TYPE_OOL_PORTS);
		tr4("ool_ports rem 0x%x seq 0x%x uid 0x%x", remote,
		    dp->dp_seqid, dp->dp_remote_port->ip_norma_uid);
		netipc_send_ool_ports(remote, dp);
	}
}

/*
 * Called from lower level when we have previously stated that we
 * have more to send and when the send interface is not busy.
 *
 * Seqid is the new seqid that should be used if there is something to send.
 */
unsigned long c_netipc_send_while_continuing = 0;
boolean_t
netipc_send_new(
	unsigned long	remote,
	unsigned long	seqid)
{
	register netipc_packet_t dp;

	assert(netipc_thread_lock_held());
	dp = netipc_packet_state[remote].netipc_packet;
	if (dp == NETIPC_PACKET_NULL) {
		return FALSE;
	}
	assert(dp->dp_remote == remote);
	/*
	 *	Unfortunately, at a lower level it is
	 *	possible for NORMA to decide to restart
	 *	a transmission on this packet even though
	 *	the packet happens to be in the middle of
	 *	completing a page-list continuation.
	 */
	if (dp->dp_being_continued == TRUE)
		return FALSE;
	if (dp->dp_last_seqid && seqid > dp->dp_last_seqid) {
		++c_netipc_send_while_continuing;
		return FALSE;
	}
	if (seqid > dp->dp_last_unacked) {
		return FALSE;	/* stop-and-wait */
	}
	netipc_set_seqid(dp, seqid);
	netipc_send_dp(remote, dp);
	return TRUE;
}

/*
 * Called from lower level when we have to retransmit something that
 * we have already sent.
 */
void
netipc_send_old(
	unsigned long	packetid,
	unsigned long	seqid)
{
	netipc_packet_t dp;

	dp = (netipc_packet_t) packetid;
	assert(dp);
	netipc_set_seqid(dp, seqid);
	netipc_send_dp(dp->dp_remote, dp);
}

netipc_packet_t netipc_continuing_packet_list = NETIPC_PACKET_NULL;

/*
 *	The netipc_packet_list contains a cache of packet structures.
 *	This cache can be used to satisfy requests from thread context
 *	and from interrupt level.  However, a minimum number of packets
 *	is reserved solely for interrupt level uses; thread context
 *	requests can be satisfied using zalloc if need be.
 */

#define	NETIPC_PACKET_LIST_RESERVED	5
#define	NETIPC_PACKET_LIST_REFILL	5

netipc_packet_t	netipc_packet_list = NETIPC_PACKET_NULL;
int		netipc_packet_count = 0;
int		netipc_packet_waiting;
int		netipc_packet_list_refill = NETIPC_PACKET_LIST_REFILL;
int		netipc_packet_list_reserved = NETIPC_PACKET_LIST_RESERVED;

unsigned int	c_netipc_packet_list_zalloc = 0;
unsigned int	c_netipc_packet_list_empty = 0;

netipc_packet_t
netipc_packet_allocate(
	int	behaviour)
{
	 netipc_packet_t dp;

	 assert(netipc_thread_lock_held());
	 if (netipc_packet_count > netipc_packet_list_reserved ||
	     (behaviour != NETIPC_PACKET_WAIT && netipc_packet_count > 0)) {
		 dp = netipc_packet_list;
		 assert(dp != NETIPC_PACKET_NULL);
		 netipc_packet_list = dp->dp_next;
		 --netipc_packet_count;

	 } else if (behaviour == NETIPC_PACKET_WAIT) {
		 ++c_netipc_packet_list_zalloc;
		 while (netipc_packet_count < netipc_packet_list_reserved) {
			 netipc_thread_unlock();
			 dp = (netipc_packet_t) zalloc(netipc_packet_zone);
			 netipc_thread_lock();
			 netipc_packet_deallocate(dp);
		 }
		 netipc_thread_unlock();
		 dp = (netipc_packet_t) zalloc(netipc_packet_zone);
		 netipc_thread_lock();

	 } else {
		 dp = (netipc_packet_t) zget(netipc_packet_zone);
		 if (dp == NETIPC_PACKET_NULL) {
			 /*
			  *	No packet in list, can't zalloc a packet
			  *	because we're not allowed to block.
			  *	Wakeup the netipc thread to refill
			  *	the packet list.
			  */
			 assert(netipc_packet_count == 0);
			 if (behaviour == NETIPC_PACKET_DEFER)
				 netipc_packet_waiting++;
			 ++c_netipc_packet_list_empty;
			 netipc_thread_wakeup();
			 return NETIPC_PACKET_NULL;
		 }
	 }

	 assert(dp != NETIPC_PACKET_NULL);
	 dp->dp_copy = VM_MAP_COPY_NULL; /* hygiene */
	 return dp;
}

void
netipc_packet_deallocate(
	netipc_packet_t	dp)
{
	ipc_port_t port;
	netipc_packet_state_t ds;
	unsigned i;

	assert(netipc_thread_lock_held());

	if (netipc_packet_count == 0 && netipc_packet_waiting > 0) {
		/*
		 * Check for any packet waiting for a netipc_packet.
		 */
		for (i = 0; i < MAX_NUM_NODES; i++) {
			ds = &netipc_packet_state[i];
			port = ds->suspended_ports;
			if (port != IP_NULL && !port->ip_norma_suspended) {
				netipc_packet_waiting--;
				netipc_mark_unsuspended(port);
				norma_ipc_queue_first_port(port, dp,
						port->ip_norma_queue_next);
				ds->suspended_ports = IP_NULL;
				assert(ds->netipc_packet == dp);
				dp->dp_reconstruct_complete = TRUE;

				/*
				 * Send it if we can.
				 */
				netipc_start(port->ip_norma_dest_node);
				return;
			}
		}
	}

	assert(netipc_packet_waiting == 0);
	dp->dp_next = netipc_packet_list;
	netipc_packet_list = dp;
	netipc_packet_count++;
}

void
netipc_packet_list_fill(void)
{
	netipc_packet_t dp;

	netipc_thread_lock();
	while (netipc_packet_count < netipc_packet_list_refill) {
		netipc_thread_unlock();
		dp = (netipc_packet_t) zalloc(netipc_packet_zone);
		netipc_thread_lock();
		netipc_packet_deallocate(dp);
	}
	netipc_thread_unlock();
}

/*
 * Currently requires a thread wakeup every VM_MAP_COPY_PAGE_LIST_MAX pages.
 * Does this matter? Can we do better?
 */
void
netipc_safe_vm_map_copy_invoke_cont(
	netipc_packet_t	dp)
{
	assert(netipc_thread_lock_held());
	assert(! dp->dp_being_continued);
	dp->dp_being_continued = TRUE;
	printf3("netipc_safe_vm_map_copy_invoke_cont(dp=0x%x)\n", dp);
	spinlock_lock(&netipc_continuing_packet_lock);
	dp->dp_next = netipc_continuing_packet_list;
	netipc_continuing_packet_list = dp;
	thread_wakeup((int) &netipc_vm_map_copy_thread);
	spinlock_unlock(&netipc_continuing_packet_lock);
	++c_netipc_vm_map_copy_invoke_cont;
}

void
netipc_vm_map_copy_invoke(
	netipc_packet_t	dp)
{
	kern_return_t kr;
	vm_map_copy_t old_copy, new_copy;

	/*
	 * Get the old copy object and save its npages value.
	 */
	assert(netipc_thread_lock_held());
	printf3("netipc_vm_map_copy_invoke_cont(dp=0x%x)\n", dp);
	old_copy = dp->dp_copy;
	assert(old_copy != VM_MAP_COPY_NULL);
	assert(old_copy->type == VM_MAP_COPY_PAGE_LIST);

	/*
	 * Unlock, and invoke the continuation.
	 * If the continuation succeeds, discard the old copy object, and lock.
	 * If it fails... not sure what to do.
	 */
	netipc_thread_unlock();
	vm_map_copy_invoke_cont(old_copy, &new_copy, &kr);
	if (kr != KERN_SUCCESS) {
		/*
		 * XXX
		 * What do we do here?
		 * What should appear in the receiver's address space?
		 *
		 * Should we abort the send at this point?
		 * We cannot, really, since we let the sender
		 * continue... didn't we?
		 * I guess we shouldn't.
		 */
		netipc_thread_lock();
		panic("netipc_vm_map_copy_invoke: kr=%d%x\n", kr, kr);
		return;
	}
	vm_map_copy_discard(old_copy);
	netipc_thread_lock();

	/*
	 * The continuation invocation succeeded.
	 * Adjust page_list_base and reset being_continued flag.
	 */
	dp->dp_page_list_base = dp->dp_copy_index;
	assert(new_copy != VM_MAP_COPY_NULL);
	assert(new_copy->type == VM_MAP_COPY_PAGE_LIST);
	dp->dp_copy = new_copy;
	dp->dp_being_continued = FALSE;
}

/*
 * XXX
 * Use the int type field to implement a linked list.
 *
 * XXX
 * It's really quite unfortunate to have to do a wakeup each time
 * we want to discard a copy. It would be much better for the sending
 * thread -- if he's still waiting -- to do the discard.
 * We could also check to see whether the pages were stolen, in
 * which case it's not as important to release the pages quickly.
 */
vm_map_copy_t	netipc_safe_vm_map_copy_discard_list = VM_MAP_COPY_NULL;
int		netipc_safe_vm_map_copy_discard_count = 0;

void
netipc_safe_vm_map_copy_discard(
	vm_map_copy_t	copy)
{
	TR_DECL("netipc_safe_vm_map_copy_discard");

	assert(netipc_thread_lock_held());
	assert(copy != VM_MAP_COPY_NULL);
	assert(copy->type == VM_MAP_COPY_PAGE_LIST);
	tr2("freeing copy 0x%x",copy);
	copy->type = (int) netipc_safe_vm_map_copy_discard_list;
	netipc_safe_vm_map_copy_discard_list = copy;
	netipc_safe_vm_map_copy_discard_count++;
	kserver_pageout_support_wakeup(); /* handles freeing discard list */
}

ipc_kmsg_t netipc_safe_ikm_free_list = IKM_NULL;
int	netipc_ikm_free_list_count = 0;
int	netipc_ikm_free_list_max = 0;

void
netipc_safe_ikm_free(
	ipc_kmsg_t	kmsg)
{
	assert(netipc_thread_lock_held());
	kmsg->ikm_next = netipc_safe_ikm_free_list;
	kmsg->ikm_prev = IKM_BOGUS; /* XXX */
	netipc_safe_ikm_free_list = kmsg;
	netipc_ikm_free_list_count++;
	if (netipc_ikm_free_list_count > netipc_ikm_free_list_max) {
		netipc_ikm_free_list_max = netipc_ikm_free_list_count;
	}
	kserver_pageout_support_wakeup(); /* free these kmsgs */
}

/*
 *	Reclaim kmsg pages.
 *
 *	Caveats:
 *		- memory allocation is required for send_dest, below;
 *		we assume that the memory usage is small (send_dest
 *		ultimately may invoke yield_transits, which sends a
 *		small message) so that at most one page will be needed
 *		to fill the appropriate small kalloc zone.
 *		- we assume that nothing invoked out of this routine blocks
 *		except for the memory allocation noted above (and that
 *		memory allocation will be filled from the reserved vm pool)
 *		- therefore, callers should have vm privilege
 */
void
netipc_safe_ikm_reclaim(void)
{
	ipc_kmsg_t kmsg;
	ipc_port_t dest;
	mach_msg_bits_t bits;
	boolean_t has_dest_right;

	netipc_thread_lock();
	while (netipc_safe_ikm_free_list != IKM_NULL) {
		/*
		 * Lock, grab kmsg, and grab dest and bits from kmsg
		 * before it is freed.
		 */
		kmsg = netipc_safe_ikm_free_list;
		netipc_safe_ikm_free_list = kmsg->ikm_next;
		kmsg->ikm_next = IKM_BOGUS; /* XXX */
		netipc_ikm_free_list_count--;
		dest = (ipc_port_t) kmsg->ikm_header.msgh_remote_port;
		bits = kmsg->ikm_header.msgh_bits;
		has_dest_right = kmsg->ikm_has_dest_right;
		netipc_thread_unlock();

		/*
		 * Free kmsg.
		 */
		if (kmsg->ikm_size == IKM_SIZE_NORMA) {
			netipc_page_put(kmsg->ikm_page, FALSE,
					"netipc_safe_ikm_reclaim");
		}
		else {
			if (kmsg->ikm_size != IKM_SAVED_KMSG_SIZE ||
			    !ikm_cache_put(kmsg)) {
				/* FALSE for now */
				ikm_free(kmsg, FALSE);
			}
		}

		/*
		 * Perform deferred copyout (including release) of dest.
		 * Migrated messages don't do this.
		 */
		if (! (bits & MACH_MSGH_BITS_MIGRATED)) {
			assert(dest->ip_references > 0);
			if (has_dest_right) {
				norma_ipc_send_dest(dest,
					MACH_MSGH_BITS_REMOTE(bits));
			}
			else {
				assert(MACH_MSGH_BITS_REMOTE(bits) ==
				       MACH_MSG_TYPE_PORT_SEND_ONCE);
				ipc_port_release(dest);
			}
		}
		netipc_thread_lock();
	}
	netipc_thread_unlock();
}


/*
 * Drain the netipc_safe_vm_map_copy_discard_list,
 * one page at a time.  This routine is invoked
 * two ways:
 *	- from netipc_ouput_replenish
 *	- from kserver_pageout_support_continue
 */
void
netipc_output_replenish_pages(void)
{
    vm_map_copy_t copy;

    netipc_thread_lock();
    while (netipc_safe_vm_map_copy_discard_list != VM_MAP_COPY_NULL) {
	copy = netipc_safe_vm_map_copy_discard_list;
	netipc_safe_vm_map_copy_discard_list
	    = (vm_map_copy_t) copy->type;
	netipc_safe_vm_map_copy_discard_count--;
	netipc_thread_unlock();
	copy->type = VM_MAP_COPY_PAGE_LIST;
	vm_map_copy_discard(copy);
	netipc_output_replenish_cleaned++;
	netipc_thread_lock();
    }
    netipc_thread_unlock();
}

/*
 *	Dedicated thread for handling continuations
 *	in outgoing messages.  This thread can block.
 */
void
netipc_vm_map_copy_thread(void)
{
	netipc_packet_t		dp;
	thread_set_own_priority(BASEPRI_KERNEL+3);	/* high priority */

	/*
	 *	While the continuing packet list remains non-empty,
	 *	grab a packet and invoke the copy object's continuation.
	 *	Restart output on the packet when done.
	 */

	for (;;) {
		spinlock_lock(&netipc_continuing_packet_lock);
		while (netipc_continuing_packet_list != NETIPC_PACKET_NULL) {
			dp = netipc_continuing_packet_list;
			netipc_continuing_packet_list = dp->dp_next;
			spinlock_unlock(&netipc_continuing_packet_lock);
			netipc_thread_lock();
			netipc_vm_map_copy_invoke(dp);
			netipc_start(dp->dp_remote);
			netipc_thread_unlock();
			++c_netipc_vm_map_copy_processed;
			spinlock_lock(&netipc_continuing_packet_lock);
		}
		assert_wait((int) netipc_vm_map_copy_thread, FALSE);
		spinlock_unlock(&netipc_continuing_packet_lock);
		thread_block((void (*)(void)) 0);
	}
}

void
netipc_output_replenish(void)
{
	while (netipc_dead_port_list != IP_NULL) {
		ipc_port_t port;
		netipc_thread_lock();
		port = netipc_dead_port_list;
		netipc_dead_port_list = port->ip_norma_queue_next;
		netipc_thread_unlock();
		ip_lock(port);
		ip_reference(port);
		norma_ipc_dead_destination(port); /* consumes lock */
	}
	netipc_output_replenish_pages();
	netipc_packet_list_fill();
}


void
netipc_set_node_state(
	unsigned long	*typep,
	unsigned long	remote)
{
	netipc_packet_t		dp;

	dp = netipc_packet_state[remote].netipc_packet;
	if (dp->dp_reconstruct_complete)
		*typep = (*typep & NODE_MASK) | NODE_RESUMED;
	else
		*typep = (*typep & NODE_MASK) | NODE_OK;
}


#include <mach_kdb.h>
#if	MACH_KDB
#include <ddb/db_output.h>
#include <machine/db_machdep.h>

#define	printf	kdbprintf

/*
 * Forward.
 */
void		db_port_list_print(
			ipc_port_t	port);

int		db_port_queue_print(
			ipc_port_t	port);

int		db_kmsg_queue_print(
			ipc_kmsg_t	kmsg,
			int		count);

/*
 *	Routine:	netipc_packet_print
 *	Purpose:
 *		Pretty-print a netipc packet for ddb.
 */

void
netipc_packet_print(
	db_addr_t	addr)
{
	netipc_packet_t dp;

	if (addr < MAX_NUM_NODES) {
		dp = netipc_packet_state[(unsigned int) dp].netipc_packet;
		if (dp == NETIPC_PACKET_NULL) {
			printf("null netipc packet\n");
			return;
		}
	} else
		dp = (netipc_packet_t)addr;
  
	printf("netipc packet 0x%x\n", dp);

	indent += 2;

	iprintf("type=%d", dp->dp_type);
	switch ((int) dp->dp_type) {
		case DP_TYPE_KMSG:
		printf("[kmsg]");
		break;

		case DP_TYPE_PAGE:
		printf("[page]");
		break;

		case DP_TYPE_KMSG_MORE:
		printf("[kmsg_more]");
		break;

		case DP_TYPE_OOL_PORTS:
		printf("[ool_ports]");
		break;

		default:
		printf("[bad type]");
		break;
	}
	printf(", remote=%d", dp->dp_remote);
	printf(", seqid=%d", dp->dp_seqid);
	printf(", first_seqid=%d", dp->dp_first_seqid);
	printf(", last_seqid=%d\n", dp->dp_last_seqid);

	iprintf("last_unacked=%d", dp->dp_last_unacked);
	printf(", reconstruct_complete=%d\n", dp->dp_reconstruct_complete);
	iprintf("kmsg=0x%x", dp->dp_kmsg);
	printf(", offset=%d", dp->dp_offset);
	printf("[0x%x]", dp->dp_offset + (char *) &dp->dp_kmsg->ikm_header);
	printf(", next=0x%x\n", dp->dp_next);

	iprintf("remote_port=0x%x", dp->dp_remote_port);
	printf(" copy=0x%x", dp->dp_copy);
	printf(" index=%d", dp->dp_copy_index);
	printf(" npages=%d\n", dp->dp_copy_npages);

	iprintf("last=%d", dp->dp_copy_last);
	printf(" base=%d", dp->dp_page_list_base);
	printf(" has_cont=%d", dp->dp_has_continuation);
	printf(" being_cont=%d\n", dp->dp_being_continued);

	indent -=2;
}

void
netipc_pacstate_print(
	db_addr_t	addr)
{
	netipc_packet_state_t	ds;
	unsigned int	remote;

	if ((unsigned int) ds < MAX_NUM_NODES) {
		remote = (unsigned int) ds;
		ds = &netipc_packet_state[remote];
	} else {
		remote = (unsigned int) -1;
		ds = (netipc_packet_state_t)addr;
	}
  
	printf("netipc packet state 0x%x\n", ds);

	indent += 2;

	iprintf("remote=%d netipc_packet=0x%x, suspended_packet=0x%x\n",
		remote, ds->netipc_packet, ds->suspended_packet);
	iprintf("suspended_ports=0x%x, state=%d (%s)\n",
		ds->suspended_ports, ds->state,
		ds->state == STATE_CLEAR ? "STATE_CLEAR" :
		ds->state == STATE_RESUME ? "STATE_RESUME" :
		"ILLEGAL VALUE");

	indent -=2;
}


/*
 *	Traverse a port list, printing all messages on each port.
 *	Note gross use of global variable to alter formatting
 *	of output by db_kmsg_queue_print; we do this because
 *	it's only a debugging routine and this way we can call
 *	db_kmsg_queue_print directly from the debugger command
 *	line without having to add additional parameters to
 *	db_kmsg_queue_print's interface.
 */

vm_size_t	db_kmsg_list_il_total = 0;
vm_size_t	db_kmsg_list_ool_total = 0;

void
db_port_list_print(
	ipc_port_t	port)
{
	ipc_port_t	p;
	int		kmsgs_found = 0;
	vm_size_t	port_list_il_total, port_list_ool_total;

	
	p = port;
	db_kmsg_list_il_total = 0;
	db_kmsg_list_ool_total = 0;
	while (p != IP_NULL) {
		iprintf("Port 0x%x:\n", port);
		indent += 2;
		kmsgs_found += db_port_queue_print(p);
		port_list_il_total += db_kmsg_list_il_total;
		port_list_ool_total += db_kmsg_list_ool_total;
		p = p->ip_norma_queue_next;
		if (p == port)
			break;
		indent -= 2;
	}
	iprintf("Port list at port 0x%x:  %d kmsgs %d il bytes %d ool bytes\n",
		port, kmsgs_found, port_list_il_total, port_list_ool_total);
}


/*
 *	Process all of the messages on a port - prints out the
 *	number of occurances of each message type, and the first
 *	kmsg with a particular msgh_id.
 */
int
db_port_queue_print(
	ipc_port_t	port)
{
	ipc_kmsg_t	kmsg;

	kmsg = ipc_kmsg_queue_first(&port->ip_messages.imq_messages);
	return db_kmsg_queue_print(kmsg, port->ip_msgcount);
}


/*
 *	Print out all the kmsgs in a queue.  Aggregate kmsgs with
 *	identical message ids into a single entry.  Count up the
 *	amount of inline and out-of-line data consumed by each
 *	and every kmsg.  Note that the amount of ool data is computed
 *	using a field (ikm_ool_bytes) present only under NORMA_IPC.
 */
int
db_kmsg_queue_print(
	ipc_kmsg_t	kmsg,
	int		count)
{
	ipc_kmsg_t	ikmsg = kmsg;
	register	icount;
	mach_msg_id_t	cur_id;
	vm_size_t	inline_total, ool_total;
	int		inline_norma_page_total, ool_norma_page_total;
	int		total = 0;

	iprintf("Count  msgh_id  kmsg addr inline bytes");
	printf("   VM pages  ool bytes   VM pages\n");
	inline_total = ool_total = (vm_size_t) 0;
	inline_norma_page_total = ool_norma_page_total = 0;
	cur_id = kmsg->ikm_header.msgh_id;
	for(icount = 0; count > 0; count--, kmsg = kmsg->ikm_next) {
		if (kmsg->ikm_header.msgh_id != cur_id) {
			iprintf("(%3d) <%6d> 0x%x   %10d %10d\n", icount,
				cur_id, ikmsg, inline_total, ool_total);
			cur_id = kmsg->ikm_header.msgh_id;
			icount = 1;
			ikmsg = kmsg;
			inline_total = ool_total = 0;
			inline_norma_page_total = ool_norma_page_total = 0;
		} else {
			icount++;
		}
		++total;
		/*
		 *	inline_total -- number of inline message bytes,
		 *		including mach_msg header.
		 *	ool_total -- number of ool message bytes,
		 *		not including the second page of a
		 *		long inline message (which NORMA
		 *		sends as an ool page)
		 *	inline_norma_page_total -- number of wired or
		 *		gobbled NORMA pages consumed by inline
		 *		message headers; this count only matters
		 *		for inbound messages
		 *	ool_norma_page_total -- same thing for ool pages
		 */
		inline_total += kmsg_size(kmsg);
		if (kmsg->ikm_size == IKM_SIZE_NORMA)
			++inline_norma_page_total;
		ool_total += kmsg->ikm_ool_bytes;
		if (kmsg_size(kmsg) > PAGE_SIZE) {
			ool_total -= (kmsg_size(kmsg) - PAGE_SIZE);
			++inline_norma_page_total;
		}
		db_kmsg_list_il_total += inline_total;
		db_kmsg_list_ool_total += ool_total;
	}
	iprintf("(%3d) <%6d> 0x%x   %10d %10d %10d %10d\n", icount,
		cur_id, ikmsg, inline_total, inline_norma_page_total,
		ool_total, ool_norma_page_total);
	return total;
}

#endif	/* MACH_KDB */

#if	DYNAMIC_NUM_NODES

#include <vm/vm_kern.h>

/*
 * Dynamically allocate NORMA protocol control structures sized according
 * to the system we are currently running on. The number of NORMA nodes is
 * computed by BOOT_MESH_X * BOOT_MESH_Y; see i860paragon/model_dep.c.
 */

void
alloc_norma_control_structures(void)
{
	char			*cp;
	int			np_size, pcs_size, save;
	kern_return_t		kr;

	/* DOT check */
	if ( MAX_NUM_NODES == 0 )
		return;

	/* compute size of netipc_packet_states rounding up to a double-word */
	np_size = sizeof(struct netipc_packet_state) * MAX_NUM_NODES;
	np_size = (np_size + sizeof(double)) & ~(sizeof(double)-1);

	pcs_size = (sizeof(struct pcs) * MAX_NUM_NODES);

	/* ask for wired kernel memory */
	kr = kmem_alloc_wired(kernel_map,
			      (vm_offset_t *)&cp, np_size + pcs_size);
	if ( kr != KERN_SUCCESS)
		panic("alloc_norma_control_structures() kmem_alloc_wired() failed.");
	bzero( cp, np_size+pcs_size );

	netipc_packet_state = (struct netipc_packet_state *)cp;
	netipc_pcs = (struct pcs *)(cp + np_size);
#if	DNN_DEBUG || defined(PARAGON860)
	if ( boot_mk_verbose ) {
		int	save;

		save = atop(round_page((sizeof(struct netipc_packet_state) *
				1024) + (sizeof(struct pcs) * 1024)));
		pcs_size = atop(round_page(np_size+pcs_size));
		printf("NORMA:\n %d nodes PCS: %d pages, saved: %d pages over 1024 nodes.\n",
			MAX_NUM_NODES, pcs_size, save - pcs_size);
	}
#endif
}

#endif	/* DYNAMIC_NUM_NODES */
