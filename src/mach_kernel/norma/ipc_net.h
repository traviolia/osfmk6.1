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
 * Revision 2.2.2.3  92/09/15  17:34:06  jeffreyh
 * 	With jeffreyh:  stole bits from the message type field
 * 	for use in suspend/resume.  Defined NETIPC_PAGE_TRACE,
 * 	debugging code for tracing page flows between nodes.
 * 	[92/08/20            alanl]
 * 
 * 	Changed pg_norma_uid to pg_seqid; with recent changes,
 * 	it is possible to have more than one kmsg on the incomplete
 * 	list that came from the same port/node.  The norma_uid is
 * 	insufficient to distinguish this case, whereas the sequence
 * 	id will do just fine.  [With jeffreyh.]
 * 
 * Revision 2.2.2.2  92/06/24  18:01:20  jeffreyh
 * 	Added pg_norma_uid to pginfo structure.
 * 	[92/06/10            jeffreyh]
 * 
 * Revision 2.2.2.1  92/01/21  21:51:40  jsb
 * 	Added file/author/date comment.
 * 	[92/01/21  19:44:13  jsb]
 * 
 * 	Conditionalized NETIPC_CHECKSUM on NORMA_ETHER. Removed MAXVEC
 * 	definition. Removed lint-inspired includes of ipc_{pset,space}.h.
 * 	[92/01/16  22:09:32  jsb]
 * 
 * 	Added NETIPC_CHECKSUM conditional and netipc_hdr checksum field.
 * 	[92/01/14  21:32:29  jsb]
 * 
 * 	Changed ctl_status type.
 * 	[92/01/13  19:34:52  jsb]
 * 
 * 	De-linted.
 * 	[92/01/13  10:14:47  jsb]
 * 
 * 	Moved protocol dependent definitions into norma/ipc_unreliable.c.
 * 	Added ctl_status to netipc_hdr (which shouldn't be exported anyway).
 * 	[92/01/11  17:07:26  jsb]
 * 
 * 	Old contents moved to norma/ipc_netvec.h.
 * 	Now contains definitions shared by files split from norma/ipc_net.c.
 * 	[92/01/10  20:38:53  jsb]
 * 
 */ 
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1992 Carnegie Mellon University
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
 *	File:	norma/ipc_net.h
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Definitions for reliable delivery and flow control for NORMA_IPC.
 */

#ifndef	_NORMA_IPC_NET_H_
#define	_NORMA_IPC_NET_H_

#include <norma_ether.h>
#include <mach_kdb.h>
#include <dynamic_num_nodes.h>

#include <machine/machparam.h>
#include <vm/vm_page.h>
#include <mach/vm_param.h>
#include <mach/message.h>
#include <kern/assert.h>
#include <kern/host.h>
#include <kern/lock.h>
#include <kern/queue.h>
#include <kern/thread.h>
#include <norma/ipc_netvec.h>
#include <stdarg.h>

#if	NORMA_ETHER
#include <kern/time_out.h>
#define	NETIPC_CHECKSUM	1
#else
/*
 * XXX
 * The checksumming code is not likely to work on the ipsc until
 * the netipc_recv routines for i386 and i860 are changed to set
 * the vector sizes to reflect the size of the incoming data.
 */
#define	NETIPC_CHECKSUM	0
#endif

#define	NETIPC_TYPE_INVALID	0x00000000L
#define	NETIPC_TYPE_KMSG	0xabcd0f00L
#define	NETIPC_TYPE_PAGE	0xabcd0f01L
#define	NETIPC_TYPE_CTL		0xabcd0f02L
#define	NETIPC_TYPE_MASK	0x000000ffL
#define NETIPC_TYPE_MAX		3L

#define	NETIPC_PAGE_TRACE	MACH_ASSERT

struct pginfo {
	unsigned long	pg_msgh_offset;
	boolean_t	pg_page_first;
	boolean_t	pg_page_last;
	boolean_t	pg_copy_last;
	unsigned long	pg_copy_offset;
	unsigned long	pg_copy_size;
	unsigned long	pg_first_seqid;
	unsigned long	pg_ool_bytes;	/* total expected length of ool data */
					/* ...trade-off:  can send once with */
					/* kmsg, but then forced to look up */
					/* kmsg for each page... */
#if	NETIPC_PAGE_TRACE
	unsigned long	pg_remote_port;	/* from dp_remote_port */
	unsigned long	pg_copy_index;	/* from dp_copy_index */
	unsigned long	pg_copy_npages;	/* from dp_copy_npages */
	unsigned long	pg_msgh_id;	/* from ikm_header->msgh_id */
#endif
};

struct netipc_hdr {
#if	NETIPC_CHECKSUM
	unsigned long	checksum;
#endif	/* NETIPC_CHECKSUM */
	unsigned long	type;
	unsigned long	seqid;
	struct pginfo	pg;
	unsigned long	remote;
	unsigned long	ctl;
	unsigned long	ctl_seqid;
	kern_return_t	ctl_status;
	unsigned long	ctl_data;
	unsigned long	incarnation;
};

/*
 *	The upper two bits of the low 16-bit word of the
 *	netipc_hdr.type field are stolen for flow control.
 *	Eventually, this will be a single boolean bit.
 */
#define	NODE_MASK		(0xffff0fff)
#define	NODE_OK			(0x00004000)
#define	NODE_RESUMED		(0x0000c000)

/*
 *	Defintions for the receiving pool of buffers
 */
enum netarray_state {
	NETARRAY_IDLE,		/* No fragment has been received */
	NETARRAY_RECEIVING,	/* At least one fragment has been received */
	NETARRAY_COMPLETED,	/* Reassembly is completed */
	NETARRAY_BUSY,		/* Input packet is busy */
	NETARRAY_ERROR		/* Error in packet input */
};

#define	NUM_NETARRAY_INIT	4	/* number of initial buffers */
#define	NUM_NETARRAY_VEC	2	/* # buffers in vector */

struct netarray {
	queue_chain_t		queue;
	enum netarray_state	state;
#if	NORMA_ETHER
	timer_elt_data_t	timer;
	unsigned long		ip_src;
	unsigned short		f_id;
#endif
	unsigned short		nvec;
	boolean_t		(*drop)(struct netarray *);
	vm_page_t		page[NUM_NETARRAY_VEC-1];
	struct netvec		vec[NUM_NETARRAY_VEC];
	struct netipc_hdr	hdr;
	unsigned long		data;
	spinlock_t		lock;
};
typedef struct netarray netipc_netarray_data_t;
typedef struct netarray *netipc_netarray_t;

#define	netipc_array_lock_init(np)	spinlock_init(&(np)->lock)
#define	netipc_array_lock(np)		spinlock_lock(&(np)->lock)
#define	netipc_array_unlock(np)		spinlock_unlock(&(np)->lock)

extern boolean_t	netarray_wanted; /* a rcv packet got no buffers */
extern unsigned		netarray_count;	 /* cur # of rcv fragment buffers */
extern unsigned		netarray_max;	 /* max # of rcv fragment buffers */
extern unsigned		netarray_pages;	 /* max # of reserved rcv pages */
extern unsigned		netarray_min;	 /* min # of reserved rcv pages */
extern queue_head_t	netarray_free;	 /* rcv fragment buffers free list */
extern queue_head_t	netarray_busy;	 /* rcv fragment buffers busy list */
extern spinlock_t	netipc_netarray_lock;

#define	netipc_netarray_lock_init()	spinlock_init(&netipc_netarray_lock)
#define	netipc_netarray_lock()		spinlock_lock(&netipc_netarray_lock)
#define	netipc_netarray_unlock()	spinlock_unlock(&netipc_netarray_lock)

/*
 * Declaration of netipc_lock handling macros.
 */
extern spinlock_t	netipc_lock;

/* XXX - kludge lock_held operation here */
#define	netipc_thread_lock_held()	(1) /* XXX XXX XXX */
#define	netipc_thread_lock_init()	spinlock_init(&netipc_lock)
#define	netipc_thread_lock()		spinlock_lock(&netipc_lock)
#define	netipc_thread_lock_try()	spinlock_try(&netipc_lock)
#define	netipc_thread_unlock()		spinlock_unlock(&netipc_lock)

/*
 * Some devices want virtual addresses, others want physical addresses.
 *
 * KVTODEV:	Kernel virtual address to device address
 * DEVTOKV:	Device address to kernel virtual address
 * VPTODEV:	vm_page_t to device address
 *
 * XXX These should be defined somewhere else.
 */
#if	NORMA_ETHER	|| i860
/*
 * Device uses virtual addresses.
 */
#define	KVTODEV(addr)	((unsigned long) (addr))
#define	DEVTOKV(addr)	((unsigned long) (addr))
#define	VPTODEV(m)	(VPTOKV(m))
#else	/*NORMA_ETHER*/
/*
 * Device uses physical addresses.
 */
#define	KVTODEV(addr)	((unsigned long) kvtophys(addr))
#define	DEVTOKV(addr)	((unsigned long) phystokv(addr))
#define	VPTODEV(m)	((m)->phys_addr)
#endif	/*NORMA_ETHER*/

#define	VPTOKV(m)	phystokv((m)->phys_addr)

#if	DYNAMIC_NUM_NODES

extern int	max_num_nodes;

#define MAX_WINDOW_SIZE 1

#define WX	(MAX_WINDOW_SIZE + 1)

typedef struct retry {
        int     ticks;
        int     backoff;
} retry_t;

/*
 * Protocol control structure
 * (struct pcb is already in use)
 */
typedef struct pcs	*pcs_t;
#define	PCS_NULL	((pcs_t) 0)
struct pcs {
	unsigned long	pcs_remote;
	unsigned long	pcs_last_received;
	unsigned long	pcs_last_sent;
	unsigned long	pcs_nacked;
	unsigned long	pcs_unacked_packetid[WX];
	unsigned long	pcs_incarnation;
	unsigned long	pcs_new_incarnation;
	unsigned long	pcs_ctl;
	unsigned long	pcs_ctl_seqid;
	kern_return_t	pcs_ctl_status;
	unsigned long	pcs_ctl_data;
	boolean_t	pcs_suspend_sent;	/* out:  suspend was sent */
	unsigned long	pcs_suspend_seqid;	/* suspended packet id */
	retry_t		pcs_resume;
	retry_t		pcs_ticks;
	queue_chain_t	pcs_timer;
	queue_chain_t	pcs_send;
	queue_chain_t	pcs_suspend;		/* in:  remote suspended us  */
	unsigned long	pcs_last_ack_status;
	unsigned long	pcs_last_ack_data;
};

extern struct pcs *netipc_pcs;

/*
 * see i860paragon/model_dep.c and norma/ipc_unreliable.c
 */
#define MAX_NUM_NODES	max_num_nodes

extern void			alloc_norma_control_structures(void);

#endif	/* DYNAMIC_NUM_NODES */

#define	NORMA_PROXY_PORT_QUEUE_LIMIT	1

extern void			netipc_send(
					unsigned long		remote,
					struct netvec		*vec,
					unsigned int		count);

/* ipc_clean.c */
extern void			ipc_kmsg_uncopyout_header_to_network(
					mach_msg_header_t	*msgh,
					ipc_port_t		dest);

extern void			ipc_kmsg_uncopyout_to_network(
					struct ipc_kmsg		*kmsg);

extern void			norma_ipc_uncopyout_all_kmsgs(
					ipc_port_t		port,
					struct ipc_kmsg_queue	*from_kmsgs,
					struct ipc_kmsg_queue	*to_kmsgs);

extern void			norma_ipc_dead_destination(
					ipc_port_t		port);

extern void			norma_ipc_error_receiving(
					struct ipc_kmsg		**kmsgp);

extern void			norma_ipc_clean_receive_errors(void);

/* ipc_deadname.c */
extern void			norma_ipc_dnrequest_init(
					ipc_port_t		port);

extern void			norma_ipc_notify_dead_name(
					ipc_port_t		port,
					unsigned long		name);

/* ipc_input.c */
extern void			norma_ipc_finish_receiving(
					struct ipc_kmsg		**kmsgp);

extern void			netipc_ast(void);

extern void			norma_deliver_kmsg(
					struct ipc_kmsg		*kmsg,
					unsigned long		remote,
					unsigned long		seqid);

extern kern_return_t		norma_deliver_page_continuation(
					vm_map_copy_t		cont_args,
					vm_map_copy_t		*copy_result);

extern void			norma_deliver_page(
					netipc_netarray_t	np,
					vm_page_t		page,
					struct pginfo		*page_info,
					unsigned long		remote,
					boolean_t		slowly);

extern void			norma_deliver_page_slowly(
					netipc_netarray_t	np);

extern boolean_t		norma_remote_to_kmsg(
					unsigned long		remote,
					unsigned long		first_seqid,
					boolean_t		must_find,
					struct ipc_kmsg		**result_kmsg);

extern void			netipc_input_init(void);

/* ipc_kserver.c */
extern void			kserver_pageout_support_wakeup(void);

extern void			norma_ipc_kobject_send(
					struct ipc_kmsg		*kmsg);

extern void			norma_kserver_wakeup(
					ipc_port_t		port);

extern void			norma_kserver_startup(void);

/* ipc_migrate.c */
extern unsigned long		norma_ipc_send_rright(
					ipc_port_t		port);

extern ipc_port_t		norma_ipc_receive_rright(
					unsigned long		uid,
					unsigned long		source_node);

/* ipc_net.c */
extern void			netipc_drain_intr_request(void);

extern void			netipc_send_intr(void);

extern void			netipc_recv_intr(void);

extern void			netipc_timeout_intr(void);

extern void			netipc_resume_intr(void);

extern void			netipc_netarray_init_max(void);

extern netipc_netarray_t	netipc_netarray_allocate(
					boolean_t		can_block);

extern void			netipc_netarray_drop(
					netipc_netarray_t	np);

extern boolean_t		netipc_netarray_page(
					netipc_netarray_t	np,
					unsigned		index);

extern void			netipc_page_put(
					vm_page_t 		m,
					boolean_t		borrowed_vm,
					char			*caller);
extern boolean_t		netipc_reserve_page(void);

extern boolean_t		netipc_able_continue_recv(void);

extern boolean_t		netipc_reserve_page_force(void);

extern void			netipc_reserve_cancel(void);

extern void			netipc_page_init(void);

extern vm_map_copy_t		netipc_copy_grab(void);

extern void			netipc_copy_ungrab(
					vm_map_copy_t		copy);

extern void			netipc_thread_wakeup(void);

extern void			netipc_replenish(
					boolean_t		always);

extern void			netipc_replenish_fallbacks(void);

extern vm_page_t		netipc_page_grab(void);

extern void			netipc_thread(void);

extern void			printf1(const char* fmt, ...);

extern void			printf2(const char* fmt, ...);

extern void			printf3(const char *fmt, ...);

extern void			printf4(const char *fmt, ...);

extern void			printf5(const char *fmt, ...);

extern void			printf6(const char *fmt, ...);

extern void			printf7(const char *fmt, ...);

extern void			fret(const char *fmt, ...);

extern void			mumble(const char *fmt, ...);

/* ipc_output.c */
extern void			netipc_output_init(void);

extern void			netipc_port_dequeue_sync(
					ipc_port_t		port);

#if	NORMA_PROXY_PORT_QUEUE_LIMIT
extern mach_msg_return_t	norma_ipc_send(
					struct ipc_kmsg		*kmsg,
					mach_msg_option_t	option,
					mach_msg_timeout_t	timeout);
#else	/* NORMA_PROXY_PORT_QUEUE_LIMIT */
extern mach_msg_return_t	norma_ipc_send(
					struct ipc_kmsg		*kmsg);
#endif	/* NORMA_PROXY_PORT_QUEUE_LIMIT */

extern void			netipc_recv_ack_with_status(
					unsigned long		packetid,
					unsigned long		seqid,
					kern_return_t		status,
					unsigned long		data);

extern void			netipc_recv_resume_node(
					unsigned long		remote);

extern boolean_t		netipc_send_new(
					unsigned long		remote,
					unsigned long		seqid);

extern void			netipc_send_old(
					unsigned long		packetid,
					unsigned long		seqid);

extern void			netipc_safe_ikm_reclaim(void);

extern void			netipc_output_replenish_pages(void);

extern void			netipc_output_replenish(void);

/* ipc_special.c */
extern void			norma_ipc_init(void);

extern kern_return_t		norma_port_location_hint(
					task_t			task,
					ipc_port_t		port,
					int			*node);

extern void			norma_unset_special_port(
					ipc_port_t		port);

extern ipc_port_t		local_special(
					unsigned long		id,
					mach_msg_type_name_t	type);

extern ipc_port_t		remote_special(
					unsigned long		node,
					unsigned long		id,
					mach_msg_type_name_t	type);

extern ipc_port_t		remote_device(
					unsigned long		node,
					mach_msg_type_name_t	type);

extern ipc_port_t		remote_host(
					unsigned long		node,
					mach_msg_type_name_t	type);

extern ipc_port_t		remote_host_priv(
					unsigned long		node,
					mach_msg_type_name_t	type);

extern ipc_port_t		remote_host_paging(
					unsigned long		node,
					mach_msg_type_name_t	type);

/* ipc_transit.c */
extern unsigned long		norma_ipc_send_port(
					ipc_port_t		port,
					mach_msg_type_name_t	type_name);

extern boolean_t		norma_ipc_prepare_dest(
					ipc_port_t		port,
					mach_msg_type_name_t	type_name);

extern void			norma_ipc_unprepare_dest(
					ipc_port_t		port,
					mach_msg_type_name_t	type_name);

extern void			norma_ipc_send_dest(
					ipc_port_t		port,
					mach_msg_type_name_t	type_name);

extern ipc_port_t		norma_ipc_receive_port(
					unsigned long		uid,
					mach_msg_type_name_t	type_name,
					unsigned long		source_node);

extern ipc_port_t		norma_ipc_receive_sright(
					unsigned long		uid);

extern ipc_port_t		norma_ipc_receive_soright(
					unsigned long		uid);

extern kern_return_t		norma_ipc_receive_dest(
					unsigned long	     uid,
					mach_msg_type_name_t type_name,
					ipc_port_t	     *remote_port,
        				unsigned long	     *ack_error_data);

extern kern_return_t		norma_ipc_receive_migrating_dest(
					unsigned long	     uid,
					mach_msg_type_name_t type_name,
					ipc_port_t	     *remote_port,
        				unsigned long	     *ack_error_data);

extern void			norma_ipc_port_destroy(
					ipc_port_t		port);

extern void			norma_ipc_notify_no_senders(
					ipc_port_t		port);

extern void			norma_ipc_yield_transits(
					host_t			host_priv,
					unsigned long		uid,
					int			stransit,
					int			sotransit);

/* ipc_unreliable.c */

extern void			_netipc_timeout_intr(void);

extern void			netipc_pretend_no_ack(
					unsigned long		remote,
					unsigned long		packetid,
					unsigned long		seqid);

extern void			netipc_resume_timeout(void);

extern void			pcs_suspend_node(
					unsigned long		remote);

extern void			netipc_start(
					unsigned long		remote);

extern unsigned long		Xnetipc_next_seqid(
					unsigned long		remote);

extern	void			norma_kmsg_put(
					struct ipc_kmsg		*kmsg);

extern boolean_t		norma_critical_message(
					mach_msg_id_t		id,
					boolean_t		super_only);

extern boolean_t		norma_critical_port(
					ipc_port_t		port);

extern void			_netipc_recv_intr(
					netipc_netarray_t	np);

extern void			norma_ipc_ack(
					netipc_netarray_t	np,
					kern_return_t		status,
					unsigned long		data);

extern void			norma_ipc_drop(
					netipc_netarray_t	np);

extern void			_netipc_send_intr(void);

extern void			netipc_init(void);

extern void			netipc_send_with_timeout(
					unsigned long		remote,
					struct netvec		*vec,
					unsigned int		count,
					unsigned long		packetid,
					unsigned long		seqid);

#if	MACH_KDB
#include <machine/db_machdep.h>

extern void			netipc_packet_print(
					db_addr_t		addr);

extern void			netipc_pacstate_print(
					db_addr_t		addr);

extern void			netipc_pcs_print(
					db_addr_t		addr);

extern void			db_norma_ipc(void);
#endif	/* MACH_KDB */

#endif /* _NORMA_IPC_NET_H_ */
