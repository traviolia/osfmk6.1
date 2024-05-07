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
 * Revision 2.5.3.4  92/05/27  00:58:40  jeffreyh
 * 	Set up a timeout on fragmented packets - if the frag is not
 * 	completed in frag_wait Hz, we punt the frag.  This avoids 
 * 	deadlock from dropped packets in fragments.
 * 	[92/04/30            sjs]
 * 
 * Revision 2.5.3.3  92/01/21  21:51:03  jsb
 * 	Use a field in fragment header to record true length, since udp
 * 	length field can no longer be trusted due to ETHERMIN logic.
 * 	[92/01/21  21:30:47  jsb]
 * 
 * 	Don't send packets smaller than ETHERMIN.
 * 	[92/01/21  19:35:34  jsb]
 * 
 * 	Changed parameters to netipc_net_packet in preparation for saving
 * 	packets that arrive before a new recv_netvec has been supplied.
 * 	Added fragment information to debugging printfs.
 * 	[92/01/17  18:36:34  jsb]
 * 
 * 	Moved node_incarnation declaration from here to norma/ipc_net.c.
 * 	[92/01/17  14:38:17  jsb]
 * 
 * 	Fixed udp checksum bug. Drop packets whose length (as claimed by
 * 	driver) differs from length as recorded in udp header.
 * 	Set recv_netvec to zero before calling netipc_recv_intr
 * 	so that any new incoming packets won't corrupt netvec.
 * 	(This shouldn't happen anyway because interrupts will be blocked.)
 * 	Panic if fragment send completions occur out-of-order.
 * 	(This also shouldn't happen.)
 * 	[92/01/16  22:05:57  jsb]
 * 
 * 	Replaced small, incomprehensible, and incorrect loop in netipc_send
 * 	responsible for copying from fragments into vector elements with
 * 	comprehensible and correct code. De-linted.
 * 	[92/01/14  21:45:52  jsb]
 * 
 * 	Moved c_netipc_frag_drop* count definitions here from norma/ipc_net.c.
 * 	[92/01/10  20:37:07  jsb]
 * 
 * Revision 2.5.3.2  92/01/09  18:45:07  jsb
 * 	Added netipc_node_valid.
 * 	From durriya@ri.osf.org: added norma_ether_boot_info.
 * 	[92/01/08  16:45:45  jsb]
 * 
 * 	Added comment about reentrancy above netipc_send.
 * 	Replaced small, incomprehensible, and incorrect loop in netipc_send
 * 	responsible for copying from vector elements into fragments with
 * 	comprehensible and correct code.
 * 	[92/01/08  10:02:33  jsb]
 * 
 * 	Moved ntohl, etc. here from norma/ipc_net.c.
 * 	[92/01/04  22:11:30  jsb]
 * 
 * Revision 2.5.3.1  92/01/03  16:37:12  jsb
 * 	Made node_incarnation be unsigned long, and changed its name.
 * 	(It used to be node_instance. I also changed the log message below.)
 * 	[91/12/29  10:10:30  jsb]
 * 
 * 	Added code to set node_incarnation, using config info if available;
 * 	node_incarnation will allow norma ipc to cope with node crashes.
 * 	Fixed code that waits for nd table info to arrive from network;
 * 	it now detects and processes the first packet that arrives.
 * 	Split nd_reply routine into nd_reply, which calls either
 * 	nd_table_reply or nd_config_reply based on magic number.
 * 	Removed magic number checking code from the latter two routines,
 * 	and removed call to nd_config from netipc_net_packet.
 * 	[91/12/27  17:15:00  jsb]
 * 
 * 	Corrected log. Fixed merge error in nd_reply.
 * 	[91/12/24  14:34:55  jsb]
 * 
 * Revision 2.5  91/12/14  14:32:44  jsb
 * 	Picked up bootconfig code from sjs@osf.org.
 * 
 * Revision 2.4  91/11/14  16:52:20  rpd
 *	Added XLAS/XLA/XSA macros to solve alignment problems.
 * 	[91/11/00            jsb]
 * 
 * Revision 2.3  91/08/28  11:15:54  jsb
 * 	Generalized netipc_swap_device code.
 * 	[91/08/16  14:30:15  jsb]
 * 
 * 	Added fields to support nrp (node table resolution protocol),
 * 	allowing removal of hardwired table of node/ip/ether addresses.
 * 	[91/08/15  08:58:04  jsb]
 * 
 * Revision 2.2  91/08/03  18:19:14  jsb
 * 	Restructured to work with completely unmodified ethernet driver.
 * 	Added code to automatically find a usable ethernet device.
 * 	Removed NODE_BY_SUBNET case. Perform initialization earlier.
 * 	[91/08/02  14:21:08  jsb]
 * 
 * 	Use IO_LOANED technology.
 * 	[91/07/27  22:56:42  jsb]
 * 
 * 	Added fragmentation.
 * 	[91/07/27  18:54:37  jsb]
 * 
 * 	First checkin.
 * 	[91/07/24  23:38:03  jsb]
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
 *	File:	norma/ipc_ether.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Functions for NORMA_IPC over ethernet.
 */

#include <mach_kdb.h>

#include <machine/machparam.h>
#include <machine/mach_param.h>	/* HZ */
#include <machine/ntoh.h>
#include <vm/vm_kern.h>
#include <vm/vm_page.h>
#include <mach/vm_param.h>
#include <mach/port.h>
#include <mach/message.h>
#include <kern/assert.h>
#include <kern/host.h>
#include <kern/lock.h>
#include <kern/kalloc.h>
#include <kern/sched_prim.h>
#include <kern/ipc_sched.h>
#include <kern/ipc_kobject.h>
#include <kern/zalloc.h>
#include <kern/spl.h>
#include <kern/misc_protos.h>
#include <ipc/ipc_mqueue.h>
#include <ipc/ipc_thread.h>
#include <ipc/ipc_kmsg.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_pset.h>
#include <ipc/ipc_space.h>
#include <device/conf.h>
#include <device/io_req.h>
#include <device/if_hdr.h>
#include <device/net_io.h>
#include <device/net_status.h>
#include <device/ds_routines.h>
#include <norma/ipc_net.h>
#include <norma/ipc_node.h>
#include <norma/ipc_ether.h>
#include <norma/tr.h>
#include <string.h>

#define	MAXFRAG			8
#define	FHP_OVERHEAD		sizeof(struct netipc_fragment_header)

typedef struct netipc_fragment_header	*netipc_fragment_header_t;

struct netipc_fragment_header {
	unsigned short	f_offset;
	unsigned char	f_id;
	unsigned char	f_last;
	unsigned long	f_length;
};

int	frag_wait = (15 * HZ) / 10; /* 1.5 sec wait before checking on frags */

int NoiseEther = 0;

struct io_req			netipc_ior[MAXFRAG];
char				netipc_ether_buf[MAXFRAG * ETHERMTU];

struct netvec			*recv_netvec = 0;
int				recv_netvec_count;

struct netdevice *netipc_device;	/* device list root pointer */

/*
 * Forward.
 */
struct node_addr	*node_addr_by_node(
				unsigned long		node);
#if	MACH_KDB
unsigned long		node_id(
				unsigned short		*node_ether_addr);
#endif	/* MACH_KDB */

unsigned long		node_ip_id(
				unsigned long		ip_addr);

void			request_ior_done(void);

void			nd_request(
				struct netdevice 	*np);

void			nd_config_parse_incarnation(
				char			*s);

void			nd_config_reply(
				netipc_udpip_header_t	uhp,
				unsigned long		count);

void			nd_table_reply(
				netipc_udpip_header_t	uhp,
				unsigned long		count);

void			nd_table_update_reply(
				netipc_udpip_header_t	uhp,
				unsigned long		count);

void			nd_reply(
				netipc_udpip_header_t	uhp,
				unsigned long		count);

void			netipc_recv_copy(
				ipc_kmsg_t		kmsg);

void			fragment_timeout_intr(
				netipc_netarray_t	np);

void			netipc_ether_send_complete(
				io_req_t		ior);



/*
 * Quick-and-dirty macros for getting around alignment problems
 * XLA -> Transfer Long in an Alignment friendly way.
 * XLAS is statement form of XLA which includes a temporary declaration,
 * allowing second parameter to be an arbitary (aligned) expression.
 */
#if	i386
/*
 * Unaligned assignment is okay
 */
#define XLA(a,b)	(*(long *)&(a) = (long)(b))
#define XLAS(a,b)	{*(long *)&(a) = (long)(b);}
#else
/*
 * Unaligned assignment is not okay
 */
#define	_xsa_(a,b,o)	(*(o+(short *)&(a)) = *(o+(short *)&(b)))
#define XLA(a,b)	(_xsa_((a),(b),0), _xsa_((a),(b),1))
#define XLAS(a,b)	{long _l = (long)(b); XLA((a),_l);}
#endif

#define	BCOPY(src, dst, len)	bcopy((char *)(src), (char *)(dst), (int)(len))
#define	HTONS(s)		htons((unsigned short)(s))
#define	NTOHS(s)		ntohs((unsigned short)(s))
#define	HTONL(l)		htonl((unsigned long)(l))
#define	NTOHL(l)		ntohl((unsigned long)(l))

int netipc_ip_id = 1;

#define	NODE_INVALID	-1
int _node_self = NODE_INVALID;
extern unsigned long node_incarnation;
struct node_addr *node_self_addr;

#define	MAX_NODE_TABLE_SIZE	256

struct node_addr	node_table[MAX_NODE_TABLE_SIZE];
int			node_table_size = 0;
spinlock_t		node_table_lock;

#define	NODE_ID(index)	(index)

struct node_addr *
node_addr_by_node(
	unsigned long	node)
{
	return &node_table[node];
}

boolean_t
netipc_node_valid(
	unsigned long	node)
{
	boolean_t ret;
	int s;

	s = splimp();
	spinlock_lock(&node_table_lock);
	ret = (node < node_table_size);
	spinlock_unlock(&node_table_lock);
	splx(s);
	return ret;
}

unsigned long
node_self(void)
{
	if (_node_self == NODE_INVALID) {
		printf("... premature node_self() call\n");
		netipc_network_init();
	}
	if (_node_self == NODE_INVALID) {
		panic("node_self: I don't know what node I am\n");
	}
	return _node_self;
}

#if	MACH_KDB
unsigned long
node_id(
	unsigned short	*node_ether_addr)
{
	int i;

	if (node_table_size == 0) {
		panic("bad node_table_size\n");
	}
	for (i = 0; i < node_table_size; i++) {
		if (node_ether_addr[0] == node_table[i].node_ether_addr[0] &&
		    node_ether_addr[1] == node_table[i].node_ether_addr[1] &&
		    node_ether_addr[2] == node_table[i].node_ether_addr[2]) {
			return NODE_ID(i);
		}
	}
	return NODE_INVALID;
}
#endif

unsigned long
node_ip_id(
	unsigned long	ip_addr)
{
	int i;

	if (node_table_size == 0) {
		panic("bad node_table_size\n");
	}
	for (i = 0; i < node_table_size; i++) {
		if (node_table[i].node_ip_addr == ip_addr) {
			return NODE_ID(i);
		}
	}
	return NODE_INVALID;
}

char netipc_boot_config[BOOT_CONFIG_SIZE];	/* boot configuration block */

char *
norma_ether_boot_info(void)
{
	return netipc_boot_config;
}

boolean_t nd_request_in_progress;
boolean_t nd_request_wanted;
boolean_t node_table_wanted;

void
netipc_network_init(void)
{
	struct netdevice *np, *npp;
	unsigned long addr;
	int s;

	/*
	 * Return if already initialized.
	 */
	if (netipc_device) {
		return;
	}

	spinlock_init(&node_table_lock);

	/*
	 * Get list of all possible devices.
	 */
	if ((netipc_device = netipc_get_netdevice()) == (struct netdevice *)0)
		panic("netipc_network_init: could not find network device");

	/*
	 * Set address and calculate node number.
	 */
	bzero((char *)node_table,
	      MAX_NODE_TABLE_SIZE * sizeof(struct node_addr));
	printf("netipc: waiting for node table");

	s = splimp();
	spinlock_lock(&node_table_lock);
	for (;;) {
		node_table_wanted = FALSE;
		for (np = netipc_device;
		     np != (struct netdevice *)0; np = np->next) {
			nd_request_in_progress = TRUE;
			nd_request_wanted = FALSE;
			spinlock_unlock(&node_table_lock);
			nd_request(np);
			spinlock_lock(&node_table_lock);
			while (nd_request_in_progress) {
				nd_request_wanted = TRUE;
				assert_wait((int)&node_self_addr, FALSE);
				spinlock_unlock(&node_table_lock);
				thread_block((void (*)(void)) 0);
				spinlock_lock(&node_table_lock);
			}
		}
		if (node_self_addr)
			break;
		node_table_wanted = TRUE;
		assert_wait((int)&node_self_addr, TRUE);
		spinlock_unlock(&node_table_lock);
		thread_set_timeout(5 * hz);
		thread_block((void (*)(void)) 0);
		spinlock_lock(&node_table_lock);
		if (node_self_addr)
			break;
		cnputc('.');
	}
	spinlock_unlock(&node_table_lock);
	splx(s);
	cnputc('\n');

	/*
	 * Get rid of useless interfaces. It should just remain one interface. 
	 *
	 * XXX It should be possible to use more than one interface
	 *	to output packets (one given interface per node).
	 */
	np = netipc_device;
	netipc_device = 0;
	while (np != (struct netdevice *)0) {
		npp = np->next;
		if (np->addr[0] == node_self_addr->node_ether_addr[0] &&
		    np->addr[1] == node_self_addr->node_ether_addr[1] &&
		    np->addr[2] == node_self_addr->node_ether_addr[2]) {
			np->next = netipc_device;
			netipc_device = np;
		} else {
			/*
			 * Close useless device.
			 */
			if (np->dev_ops->d_close)
				(*np->dev_ops->d_close)(np->unit);
			device_deallocate(np->dev);
			kfree((vm_offset_t)np, sizeof (*np));
		}
		np = npp;
	}

	addr = NTOHL(node_self_addr->node_ip_addr);
	printf("netipc: using %s%d\n",
	       netipc_device->dev_ops->d_name, netipc_device->unit);
	printf("netipc: node %d internet %d.%d.%d.%d ethernet %s\n",
	       node_self(), (addr >> 24) & 0xFF, (addr >> 16) & 0xFF,
	       (addr >> 8) & 0xFF, addr & 0xFF,
	       ether_sprintf((unsigned char *)
			     node_self_addr->node_ether_addr));
	if (netipc_boot_config[0] != '\0')
		printf("netipc: boot config:\n%s", netipc_boot_config);
	printf("netipc: incarnation %d\n", node_incarnation);
}

void
request_ior_done(void)
{
	int s;

	s = splimp();
	spinlock_lock(&node_table_lock);
	nd_request_in_progress = FALSE;
	if (nd_request_wanted) {
		nd_request_wanted = FALSE;
		thread_wakeup((int)&node_self_addr);
	}
	spinlock_unlock(&node_table_lock);
	splx(s);
}

void
nd_request(
	struct netdevice *np)
{
	netipc_ether_header_t ehp;
	netipc_udpip_header_t uhp;
	struct nd_request *ndrq;
	int i, total_length, udp_length;
	unsigned long checksum;
	io_return_t kr;

	ehp  = (netipc_ether_header_t) netipc_ether_buf;
	uhp  = (netipc_udpip_header_t) (((char *) ehp) + EHLEN);
	ndrq = (struct nd_request *) (uhp + 1);

	udp_length = UDP_OVERHEAD + sizeof(struct nd_request);
	total_length = EHLEN + IP_OVERHEAD + udp_length;

	netipc_ior[0].io_count = total_length;
	netipc_ior[0].io_data = (char *) ehp;
	netipc_ior[0].io_done = (boolean_t (*)(io_req_t))request_ior_done;
	netipc_ior[0].io_op = IO_LOANED;

	ehp->e_ptype = HTONS(ETHERTYPE_IP);
	ehp->e_src[0] = np->addr[0];
	ehp->e_src[1] = np->addr[1];
	ehp->e_src[2] = np->addr[2];
	ehp->e_dest[0] = 0xffff;
	ehp->e_dest[1] = 0xffff;
	ehp->e_dest[2] = 0xffff;

	uhp->ip_vhl = (IPVERSION << 4) | 5;
	uhp->ip_type_of_service = 0;
	uhp->ip_total_length = HTONS(udp_length + IP_OVERHEAD);
	uhp->ip_id = HTONS(netipc_ip_id++);
	uhp->ip_fragment_offset = HTONS(0);
	uhp->ip_time_to_live = 0xff;
	uhp->ip_protocol = UDP_PROTOCOL;
	uhp->ip_checksum = 0;

	XLAS(uhp->ip_src, 0);
	XLAS(uhp->ip_dst, HTONL(0xffffffff));

	for (checksum = i = 0; i < 10; i++) {
		checksum += ((unsigned short *) uhp)[i];
	}
	checksum = (checksum & 0xffff) + (checksum >> 16);
	checksum = (checksum & 0xffff) + (checksum >> 16);
	uhp->ip_checksum = (~checksum & 0xffff);

	uhp->udp_source_port = HTONS(IPPORT_NDREPLY);
	uhp->udp_dest_port = HTONS(IPPORT_NDREQUEST);
	uhp->udp_length = HTONS(udp_length);
	uhp->udp_checksum = 0;

	XLAS(ndrq->ndrq_magic, HTONL(NDRQ_MAGIC));
	XLAS(ndrq->ndrq_start, HTONS(0));

	BCOPY((char *)np->addr, ndrq->ndrq_eaddr, 6);

	/*
	 * Send the packet.
	 */
	kr = (*np->dev_ops->d_write)(np->unit, &netipc_ior[0]);

	if (kr != D_SUCCESS && kr != D_IO_QUEUED)
		request_ior_done();
}

void
nd_config_parse_incarnation(
	char	*s)
{
	char *incarnation_env = "INCARNATION=";
	unsigned long incarnation = 0;

	if (strncmp(s, incarnation_env, strlen(incarnation_env))) {
		return;
	}
	for (s += strlen(incarnation_env); *s >= '0' && *s <= '9'; s++) {
		incarnation = 10 * incarnation + (*s - '0');
	}
	if (incarnation != 0) {
		node_incarnation = incarnation;
	}
}

void
nd_config_reply(
	netipc_udpip_header_t	uhp,
	unsigned long		count)
{
	struct nd_config *ndrc = (struct nd_config *) (uhp + 1);
	int bootconfigsize, bootmsgcnt, bootoffset;

	bootmsgcnt = NTOHS(ndrc->ndrc_msg_size);
	bootoffset = NTOHS(ndrc->ndrc_offset);
	if ((bootoffset + bootmsgcnt) > sizeof(netipc_boot_config)) {
		printf("nd_config: Too much config data!");
		return;
	}
	bootconfigsize = NTOHS(ndrc->ndrc_config_size);
	BCOPY(ndrc->ndrc_boot_data, &netipc_boot_config[bootoffset],
	       bootmsgcnt);
	netipc_boot_config[bootconfigsize] = '\0';
	if (bootoffset + bootmsgcnt >= bootconfigsize) {
		nd_config_parse_incarnation(netipc_boot_config);
	}
}

void
nd_table_reply(
	netipc_udpip_header_t	uhp,
	unsigned long		count)
{
	struct nd_reply *ndrp = (struct nd_reply *) (uhp + 1);
	int i;
	int s;

	_node_self = NTOHS(ndrp->ndrp_node_self);
	node_table_size = NTOHS(ndrp->ndrp_table_size);
	if (node_table_size < 0 || node_table_size > MAX_NODE_TABLE_SIZE) {
		panic("nd_table_reply: bad table size %d\n", node_table_size);
	}
	for (i = 0; i < node_table_size; i++) {
		struct nd *nd = &ndrp->ndrp_table[i];
		struct node_addr *na = &node_table[i];

		if (! nd->nd_valid) {
			na->node_ip_addr = 0;
			bzero((char *)na->node_ether_addr, 6);
			continue;
		} else {
			na->node_ip_addr = nd->nd_iaddr;
			BCOPY(nd->nd_eaddr, na->node_ether_addr, 6);
		}
	}
	s = splimp();
	spinlock_lock(&node_table_lock);
	node_self_addr = &node_table[_node_self];
	if (node_table_wanted) {
		node_table_wanted = FALSE;
		thread_wakeup((int) &node_self_addr);
	}
	spinlock_unlock(&node_table_lock);
	splx(s);
	netipc_netarray_init_max();
}

void
nd_table_update_reply(
	netipc_udpip_header_t	uhp,
	unsigned long		count)
{
	struct nd_reply *ndrp = (struct nd_reply *) (uhp + 1);
	int i, node, table_size;
	int s;

	node = NTOHS(ndrp->ndrp_node_self);
	table_size = NTOHS(ndrp->ndrp_table_size);
	if (table_size < 0 || table_size > MAX_NODE_TABLE_SIZE ||
	    table_size < node_table_size) {
		printf("nd_table_update: bad table size %d\n", table_size);
		return;
	}
	if (node != _node_self) {
		printf("nd_table_update: changing node_self id from %d to %d\n",
		       _node_self, node);
		return;
	}

	s = splimp();
	spinlock_lock(&node_table_lock);
	for (i = 0; i < table_size; i++) {
		struct nd *nd = &ndrp->ndrp_table[i];
		struct node_addr *na = &node_table[i];

		if (! nd->nd_valid) {
			na->node_ip_addr = 0;
			bzero((char *)na->node_ether_addr, 6);
		} else {
			/* 
			 * We check if the new table is coherent with ours, 
			 * since we allow add-ons but no replacements.
			 */
			if (na->node_ip_addr == nd->nd_iaddr)
				continue;
			if (na->node_ip_addr == NULL) {
				na->node_ip_addr = nd->nd_iaddr;
				BCOPY(nd->nd_eaddr, na->node_ether_addr, 6);
				printf("Adding node %d (ip %x) to cluster\n",
				       i, na->node_ip_addr);
			} else {
				printf("Bad table uptade for node %d ip %x\n",
				       i, na->node_ip_addr);
				node = NODE_INVALID;
				break;
			}
		}
	}
	node_table_size = table_size;
	spinlock_unlock(&node_table_lock);
	splx(s);
	netipc_netarray_init_max();
}

void
nd_reply(
	netipc_udpip_header_t	uhp,
	unsigned long		count)
{
	struct nd_reply *ndrp = (struct nd_reply *) (uhp + 1);

	if (node_self_addr) {
		/*
		 * Allow table info to be updated
		 */
		if (ndrp->ndrp_magic == HTONL(NDRP_MAGIC))
			nd_table_update_reply(uhp, count);
		/*
		 * Table and config info already initialized.
		 */
		return;
	}
	if (ndrp->ndrp_magic == HTONL(NDRP_MAGIC)) {
		nd_table_reply(uhp, count);
	} else if (ndrp->ndrp_magic == HTONL(NDRC_MAGIC)) {
		nd_config_reply(uhp, count);
	} else {
		printf("nd_reply: bad magic\n");
	}
}

char netipc_swap_device[3] = "hd";
int netipc_swap_node_1 = 1;
int netipc_swap_node_2 = 1;
int netipc_swap_xor = 1;

char *
dev_forward_name(
	char	*name,
	char	*namebuf,
	int	namelen)
{
	if (netipc_swap_node_1 == netipc_swap_node_2) {
		return name;
	}
	if (name[0] == netipc_swap_device[0] &&
	    name[1] == netipc_swap_device[1]) {
		namebuf[0] = '<';
		if (node_self() == netipc_swap_node_1) {
			namebuf[1] = '0' + netipc_swap_node_2;
		} else {
			namebuf[1] = '0' + netipc_swap_node_1;
		}
		namebuf[2] = '>';
		namebuf[3] = name[0];			/* e.g. 'h' */
		namebuf[4] = name[1];			/* e.g. 'd' */
		namebuf[5] = name[2] ^ netipc_swap_xor;	/* major */
		namebuf[6] = name[3];			/* minor */
		namebuf[7] = '\0';
		printf("[ %s -> %s ]\n", name, namebuf);
		return namebuf;
	}
	return name;
}

int c_netipc_frag_recv = 0;
int c_netipc_frag_drop0	= 0;
int c_netipc_frag_drop1	= 0;
int c_netipc_frag_drop2	= 0;
int c_netipc_frag_drop3	= 0;
int c_netipc_frag_drop4	= 0;
int c_netipc_frag_drop5	= 0;
int c_netipc_frag_drop6	= 0;
int c_netipc_frag_timeout = 0;

boolean_t
netipc_net_packet(
	ipc_kmsg_t	kmsg,
	unsigned long	count)
{
	register struct netipc_ether_header *ehp;
	register struct netipc_udpip_header *uhp;

	ehp = (netipc_ether_header_t) &net_kmsg(kmsg)->header[0];
	uhp = (netipc_udpip_header_t) &net_kmsg(kmsg)->packet[sizeof(char *)];
	count = count - sizeof(struct packet_header);

	/*
	 * Only consider udp packets
	 */
	if (ehp->e_ptype != HTONS(ETHERTYPE_IP) ||
/*	    node_ip_id(uhp->ip_src) == -1 ||		ignore broadcasts? */
	    (node_self_addr != (struct node_addr *) 0 &&
	     node_self_addr->node_ip_addr != uhp->ip_dst) ||
	    uhp->ip_protocol != UDP_PROTOCOL) {
		return FALSE;
	}

	/*
	 * Ignore packets whose length (as claimed by driver) differs from
	 * length as recorded in udp header.
	 */
	if (count != NTOHS(uhp->udp_length) + IP_OVERHEAD) {
		c_netipc_frag_drop0++;
		return FALSE;
	}

	/*
	 * Steal the kmsg if we recognize the udp port.
	 */
	if (uhp->udp_dest_port == NETIPC_UDPPORT) {
		if (NoiseEther) {
			netipc_fragment_header_t fhp =
			    (netipc_fragment_header_t) (uhp + 1);
			printf("R s=%d.%x, d=%d.%x f=%04d/%d/%d len=%d\n",
			       node_ip_id(uhp->ip_src),
			       NTOHL(uhp->ip_src),
			       node_ip_id(uhp->ip_dst),
			       NTOHL(uhp->ip_dst),
			       NTOHS(fhp->f_offset),
			       fhp->f_id,
			       fhp->f_last,
			       NTOHS(uhp->udp_length) + EHLEN + IP_OVERHEAD);
		}
		netipc_recv_copy(kmsg);
		return TRUE;
	} else if (NTOHS(uhp->udp_dest_port) == IPPORT_NDREPLY) {
		nd_reply(uhp, count);
		return TRUE;
	} else {
		return FALSE;
	}
}

void
netipc_recv_copy(
	ipc_kmsg_t	kmsg)
{
	netipc_udpip_header_t uhp;
	netipc_fragment_header_t fhp;
	char *buf;
	unsigned long count;
	int i, offset;
	netipc_netarray_t np;
	TR_DECL("netipc_recv_copy");

	/*
	 * Return immediately if network module is not yet initialized.
	 * It is not worth to take the netipc_netarray lock, since
	 * netarray_count is only incremented.
	 */
	if (netarray_count == 0) {
		c_netipc_frag_drop5++;
		return;
	}

	uhp = (netipc_udpip_header_t) &net_kmsg(kmsg)->packet[sizeof(char *)];
	fhp = (netipc_fragment_header_t) (uhp + 1);
	XLA(count, fhp->f_length); count = NTOHL(count);
	buf = (char *) (fhp + 1);
	offset = NTOHS(fhp->f_offset);

	/*
	 * Look for fragment in the reassembly list
	 */
	netipc_netarray_lock();
	queue_iterate(&netarray_busy, np, netipc_netarray_t, queue)
		if (np->ip_src == uhp->ip_src &&
		    np->hdr.type != NETIPC_TYPE_CTL)
			break;

	if (queue_end(&netarray_busy, (queue_entry_t)np)) {
		if (queue_empty(&netarray_free)) {
			if (netarray_count < netarray_max) {
				np = netipc_netarray_allocate(FALSE);
				if (np == (netipc_netarray_t)0) {
					netarray_wanted = TRUE;
					netipc_netarray_unlock();
					netipc_thread_wakeup();
					c_netipc_frag_drop5++;
					return;
				}
				netarray_count++;
			} else {
				netipc_netarray_unlock();
				c_netipc_frag_drop5++;
				return;
			}
		} else {
			queue_remove_first(&netarray_free, np,
					   netipc_netarray_t, queue);
		}
		queue_enter(&netarray_busy, np, netipc_netarray_t, queue);
		netipc_array_lock(np);
		np->ip_src = uhp->ip_src;
		netipc_netarray_unlock();
		np->state = NETARRAY_IDLE;
		c_netipc_frag_recv++;

	} else {
		netipc_array_lock(np);
		netipc_netarray_unlock();
		switch (np->state) {
		case NETARRAY_IDLE:
			panic("Incorrect state in receiving IP fragments\n");

		case NETARRAY_RECEIVING:
			/*
			 * New nonfragged message replaces old frag.
			 * XXX clearly suboptimal, see discussion above
			 * XXX assumes only one fragged message in transit
			 */
			if (fhp->f_last == 0) {
				c_netipc_frag_drop3++;
				reset_timeout_check(&np->timer);
			} else
				c_netipc_frag_recv++;
			break;

		default:
			/*
			 * XXX Only one received packet per site at the moment
			 */
			netipc_array_unlock(np);
			c_netipc_frag_drop1++;
			return;
		}
	}

	/*
	 * First check for non fragmented packets.
	 */
	if (fhp->f_last == 0) {
		/*
		 * Fill first vector.
		 */
		BCOPY(buf, np->vec[0].addr, np->vec[0].size);
		buf += np->vec[0].size;
		count -= np->vec[0].size;
		assert(count >= 0);

		/*
		 * If second element is enough, then fill it.
		 */
		if (np->hdr.type == NETIPC_TYPE_CTL) {
			assert(count <= np->vec[1].size);
			BCOPY(buf, np->vec[1].addr, np->vec[1].size);
			buf += np->vec[1].size;
			count -= np->vec[1].size;
			np->state = NETARRAY_COMPLETED;
			netipc_array_unlock(np);
			netipc_recv_intr();
			return;
		}

		/*
		 * If it is impossible to get pages, then delete packet.
		 */
		if (!netipc_netarray_page(np, 1)) {
			np->state = NETARRAY_ERROR;
			netipc_array_unlock(np);
			netipc_netarray_drop(np);
			c_netipc_frag_drop5++;
			return;
		}

		/*
		 * Now fill all pages with received packet.
		 */
		for (i = 1; i < np->nvec && count > np->vec[i].size; i++) {
			BCOPY(buf, np->vec[i].addr, np->vec[i].size);
			buf += np->vec[i].size;
			count -= np->vec[i].size;
		}
		if (i == np->nvec) {
			np->state = NETARRAY_ERROR;
			netipc_array_unlock(np);
			netipc_netarray_drop(np);
			c_netipc_frag_drop6++;
			return;
		}

		BCOPY(buf, np->vec[i].addr, count);
		np->vec[i].size = count;
		np->state = NETARRAY_COMPLETED;
		netipc_array_unlock(np);
		netipc_recv_intr();
		return;
	}

	/*
	 * The packet was fragmented.
	 * If this is the first frag of a message, set up frag info.
	 */
	if (np->state == NETARRAY_IDLE) {
		np->f_id = 0;
		np->state = NETARRAY_RECEIVING;
		set_timeout_setup(&np->timer, 
				  (timeout_fcn_t)fragment_timeout_intr,
				  np, frag_wait, PROCESSOR_NULL);
	}

	/*
	 * If we missed a frag, drop the whole thing!
	 *
	 * XXX This should be changed to allow reordering of packets!
	 */
	if (np->f_id != fhp->f_id) {
		reset_timeout_check(&np->timer);
		np->state = NETARRAY_ERROR;
		netipc_array_unlock(np);
		netipc_netarray_drop(np);
		c_netipc_frag_drop4++;
		return;
	}

	/*
	 * Skip to the right offset in the receive buffer,
	 * and copy it into the appropriate vectors.
	 *
	 * First skip past all elements that we won't touch.
	 */
	for (i = 0; offset >= np->vec[i].size; i++) {
		offset -= np->vec[i].size;
		if (offset == 0) {
			i++;
			break;
		}
	}
	assert(offset >= 0);
	assert(i < np->nvec);

	/*
	 * A non-zero offset will affect at most one element.
	 * Deal with it now so that we can stop worrying about offset.
	 */
	if (offset > 0) {
		/*
		 * If this is the last element, and the last fragment,
		 * then set size.
		 */
		if (count <= np->vec[i].size - offset) {
			BCOPY(buf, np->vec[i].addr + offset, count);
			if (fhp->f_id == fhp->f_last) {
				np->vec[i].size = count + offset;
			}
			goto did_copy;
		}
		/*
		 * This is not the last element. Copy and continue.
		 */
		BCOPY(buf, np->vec[i].addr + offset,
		      np->vec[i].size - offset);
		buf += np->vec[i].size - offset;
		count -= np->vec[i].size - offset;
		i++;

		/*
		 * Now, Allocate a new page to copy packet data.
		 */
		if (!netipc_netarray_page(np, i)) {
			reset_timeout_check(&np->timer);
			np->state = NETARRAY_ERROR;
			netipc_array_unlock(np);
			netipc_netarray_drop(np);
			c_netipc_frag_drop5++;
			return;
		}
	}

	/*
	 * Fill all the middle elements.
	 */
	for (; count > np->vec[i].size; i++) {
		BCOPY(buf, np->vec[i].addr, np->vec[i].size);
		buf += np->vec[i].size;
		count -= np->vec[i].size;
		if (count > 0) {
			assert(i+1 < np->nvec);
			if (!netipc_netarray_page(np, i+1)) {
				reset_timeout_check(&np->timer);
				np->state = NETARRAY_ERROR;
				netipc_array_unlock(np);
				netipc_netarray_drop(np);
				c_netipc_frag_drop5++;
				return;
			}
		}
	}

	/*
	 * Fill the last element; if it's the last fragment, set size.
	 */
	BCOPY(buf, np->vec[i].addr, count);
	if (fhp->f_id == fhp->f_last) {
		np->vec[i].size = count;
	}

did_copy:
	/*
	 * If this is the last frag, hand it up, after setting the
	 * size for all remaining elements to zero.
	 * Otherwise, simply note that we received this frag.
	 */
	if (fhp->f_id == fhp->f_last) {
		reset_timeout_check(&np->timer);
		np->state = NETARRAY_COMPLETED;
		netipc_array_unlock(np);
		netipc_recv_intr();
	} else {
		if (np->vec[i].size == count &&
		    !netipc_netarray_page(np, i+1)) {
			reset_timeout_check(&np->timer);
			np->state = NETARRAY_ERROR;
			netipc_array_unlock(np);
			netipc_netarray_drop(np);
			c_netipc_frag_drop5++;
			return;
		}
		np->f_id++;
		netipc_array_unlock(np);
	}
}

/*
 * Fragment timeout.  If awakened, it will release the current fragment
 * in progress; higher levels will retransmit.
 */
void
fragment_timeout_intr(
	netipc_netarray_t	np)
{
	unsigned i;
	int s;

	s = splimp();
	netipc_array_lock(np);
	np->state = NETARRAY_ERROR;
	netipc_array_unlock(np);
	netipc_netarray_drop(np);
	splx(s);
	c_netipc_frag_timeout++;
}

/* (net as in usable, not as in network) */
#define	NETMTU	(ETHERMTU - EHLEN - IP_OVERHEAD - UDP_OVERHEAD - FHP_OVERHEAD)

int netipc_fragment_last_uncompleted = 0;

void
netipc_ether_send_complete(
	io_req_t	ior)
{
	register netipc_udpip_header_t uhp;
	register netipc_fragment_header_t fhp;

	uhp = (netipc_udpip_header_t) (((char *) ior->io_data) + EHLEN);
	fhp = (netipc_fragment_header_t) (uhp + 1);
	if (fhp->f_id != netipc_fragment_last_uncompleted) {
		panic("netipc_ether_send_complete: f_id %d != %d!\n",
		      fhp->f_id, netipc_fragment_last_uncompleted);
	}
	netipc_fragment_last_uncompleted++;
	if (fhp->f_id == fhp->f_last) {
		netipc_send_intr();
	}
}

/*
 * This routine is not reentrant and is not expected to be.
 * norma/ipc_net.c uses netipc_sending to prevent multiple entrances.
 * If we have a reentrant form, we can upcall on netipc_send_intr
 * to indicate that we can accept another netipc_send call.
 */
void
netipc_send(
	unsigned long	remote,
	struct netvec	*vec,
	unsigned int	count)
{
	int i, f, frag_count, data_len;
	struct node_addr *node_dest_addr;
	char *buf;
	netipc_ether_header_t ehp;
	netipc_udpip_header_t uhp;
	netipc_fragment_header_t fhp;
	int total_length, udp_length;

	/*
	 * Silently drop packets destined for invalid nodes.
	 * Sender should previously have called netipc_node_valid.
	 * XXX
	 * We should have sender check our return value.
	 * If this were done, we wouldn't have to call netipc_send_intr.
	 */
	if (! netipc_node_valid(remote)) {
		netipc_send_intr();
		return;
	}

	/*
	 * Compute total size and number of fragments
	 */
	data_len = 0;
	for (i = 0; i < count; i++) {
		data_len += vec[i].size;
	}
	frag_count = (data_len + NETMTU - 1) / NETMTU;
	if (frag_count > MAXFRAG) {
		panic("netipc_send: size %d: too many (%d) fragments\n",
		       data_len, frag_count);
	}

	/*
	 * Get destination address
	 */
	node_dest_addr = node_addr_by_node(remote);

	/*
	 * Construct headers for each fragment; copy data into a contiguous
	 * chunk (yuck). Use loaned ior's.
	 */
	netipc_fragment_last_uncompleted = 0;
	for (f = 0; f < frag_count; f++) {
		ehp = (netipc_ether_header_t) &netipc_ether_buf[f * ETHERMTU];
		uhp = (netipc_udpip_header_t) (((char *) ehp) + EHLEN);
		fhp = (netipc_fragment_header_t) (uhp + 1);

		if (data_len > NETMTU) {
			udp_length = ETHERMTU - EHLEN - IP_OVERHEAD;
			XLAS(fhp->f_length,
			     HTONL(udp_length - UDP_OVERHEAD - FHP_OVERHEAD));
			total_length = ETHERMTU;
			data_len -= NETMTU;
		} else {
			udp_length = UDP_OVERHEAD + FHP_OVERHEAD + data_len;
			XLAS(fhp->f_length, HTONL(data_len));
			if (udp_length < ETHERMIN - IP_OVERHEAD) {
				udp_length = ETHERMIN - IP_OVERHEAD;
			}
			total_length = EHLEN + IP_OVERHEAD + udp_length;
			data_len = 0;
		}

		netipc_ior[f].io_count = total_length;
		netipc_ior[f].io_data = (char *) ehp;
		netipc_ior[f].io_done = (boolean_t (*)(io_req_t))
			netipc_ether_send_complete;
		netipc_ior[f].io_op = IO_LOANED;

		ehp->e_ptype = HTONS(ETHERTYPE_IP);
		ehp->e_src[0] = node_self_addr->node_ether_addr[0];
		ehp->e_src[1] = node_self_addr->node_ether_addr[1];
		ehp->e_src[2] = node_self_addr->node_ether_addr[2];
		ehp->e_dest[0] = node_dest_addr->node_ether_addr[0];
		ehp->e_dest[1] = node_dest_addr->node_ether_addr[1];
		ehp->e_dest[2] = node_dest_addr->node_ether_addr[2];

		uhp->ip_vhl = (IPVERSION << 4) | 5;
		uhp->ip_type_of_service = 0;
		uhp->ip_total_length = HTONS(udp_length + IP_OVERHEAD);
		uhp->ip_id = HTONS(netipc_ip_id++);
		uhp->ip_fragment_offset = HTONS(0);
		uhp->ip_time_to_live = 0xff;
		uhp->ip_protocol = UDP_PROTOCOL;
		uhp->ip_checksum = -1;

		XLA(uhp->ip_src, node_self_addr->node_ip_addr);
		XLA(uhp->ip_dst, node_dest_addr->node_ip_addr);

		uhp->udp_source_port = HTONS(NETIPC_UDPPORT);
		uhp->udp_dest_port = HTONS(NETIPC_UDPPORT);
		uhp->udp_length = HTONS(udp_length);
		uhp->udp_checksum = -1;

		buf = (char *) (fhp + 1);

		if (frag_count == 0) {
			fhp->f_offset = 0;
			fhp->f_id = 0;
			fhp->f_last = 0;

			/*
			 * Everything will fit into one fragment,
			 * so just copy everything into it.
			 */
			for (i = 0; i < count; i++) {
				BCOPY(vec[i].addr, buf, vec[i].size);
				buf += vec[i].size;
			}
		} else {
			unsigned short offset = f * NETMTU;
			unsigned short remain = NETMTU;

			fhp->f_offset = HTONS(offset);
			fhp->f_id = f;
			fhp->f_last = frag_count - 1;

			/*
			 * First skip all elements which have
			 * already been completely sent.
			 */
			for (i = 0; offset >= vec[i].size; i++) {
				assert(i < count);
				assert(offset >= vec[i].size);
				offset -= vec[i].size;
			}
			assert((long) offset < vec[i].size);

			/*
			 * Offset is now the offset into the current
			 * element at which we begin copying.
			 * If this element is all we need, then copy
			 * as much of this element as will fit.
			 */
			assert(i < count);
			if (vec[i].size - offset >= remain) {
				BCOPY(vec[i].addr + offset, buf, remain);
				goto done_copying_into_fragments;
			}

			/*
			 * Copy all of this element, starting at offset.
			 * Move to the next element.
			 */
			BCOPY(vec[i].addr + offset, buf, vec[i].size - offset);
			buf += (vec[i].size - offset);
			remain -= (vec[i].size - offset);
			i++;

			/*
			 * For each remaining element: if it's all we need,
			 * then copy all that will fit and stop. Otherwise,
			 * copy all of it, and continue.
			 */
			for (; i < count; i++) {
				if (vec[i].size >= remain) {
					BCOPY(vec[i].addr, buf, remain);
					break;
				}
				BCOPY(vec[i].addr, buf, vec[i].size);
				buf += vec[i].size;
				remain -= vec[i].size;
			}
done_copying_into_fragments:
			;
		}

		if (NoiseEther) {
			printf("S s=%d.%x, d=%d.%x f=%04d/%d/%d len=%d\n",
			       node_ip_id(uhp->ip_src),
			       NTOHL(node_self_addr->node_ip_addr),
			       node_ip_id(uhp->ip_dst),
			       NTOHL(node_dest_addr->node_ip_addr),
			       NTOHS(fhp->f_offset),
			       fhp->f_id,
			       fhp->f_last,
			       udp_length + EHLEN + IP_OVERHEAD);
		}

		/*
		 * Send the packet.
		 */
		(*netipc_device->dev_ops->d_write)(netipc_device->unit,
						   &netipc_ior[f]);
	}
}
