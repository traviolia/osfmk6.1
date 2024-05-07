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
 * Revision 2.3.2.4  92/09/15  17:34:09  jeffreyh
 * 	Moved norma_port_tabled macro here from ipc_list.c
 * 	[92/08/06            dlb]
 * 
 * 	Moved node_is_valid test here from ipc_special.c
 * 	[92/07/23            jeffreyh]
 * 
 * Revision 2.3.2.3  92/04/08  15:45:58  jeffreyh
 * 	Add IP_NORMA_MAX_NODE.
 * 	[92/04/07            dlb]
 * 
 * 	Define IP_NORMA_NULL and IP_NORMA_DEAD.  Latter steals one
 * 	value from IP_NORMA_MAX_LID.
 * 	[92/04/02            dlb]
 * 
 * Revision 2.3.2.2  92/01/09  18:45:45  jsb
 * 	Use MAX_SPECIAL_KERNEL_ID, not MAX_SPECIAL_ID, in IP_NORMA_SPECIAL.
 * 	[92/01/04  18:26:48  jsb]
 * 
 * Revision 2.3.2.1  92/01/03  16:37:54  jsb
 * 	Added IP_NORMA_{MAX_LID,SPECIAL} macros, and declaration
 * 	of host_special_port array.
 * 	[91/12/24  14:25:35  jsb]
 * 
 * Revision 2.3  91/12/13  14:00:49  jsb
 * 	Moved MAX_SPECIAL_ID to mach/norma_special_ports.h.
 * 
 * Revision 2.2  91/11/14  16:46:02  rpd
 * 	Created.
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
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
 *	File:	norma/ipc_node.h
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Definitions for norma nodes.
 */

#ifndef	_NORMA_IPC_NODE_H_
#define	_NORMA_IPC_NODE_H_

#include <mach_kdb.h>

#include <norma_ether.h>
#include <mach/boolean.h>
#include <mach/norma_special_ports.h>
#include <ipc/ipc_port.h>
#include <kern/host.h>

/*
 * A uid (unique id) = node + lid (local id)
 * We embed the node id in the uid because receive rights rarely move.
 */

/*
 * 12 bits for node		-> max 4096 nodes
 * 20 bits for local id		-> max 1,048,575 exported ports per node
 *
 * (Byte allignment and intuitive node:lid placement aids debugging)
 */
#define	IP_NORMA_NODE(uid)	(((unsigned long)(uid) >> 20) & 0x00000fff)
#define	IP_NORMA_LID(uid)	((unsigned long)(uid) & 0x000fffff)
#define	IP_NORMA_UID(node, lid)	(((node) << 20) | (lid))

#define	IP_NORMA_MAX_LID	IP_NORMA_LID(0xfffffffe)
#define	IP_NORMA_SPECIAL(uid)	(IP_NORMA_LID(uid) > 0 && \
				 IP_NORMA_LID(uid) <= MAX_SPECIAL_KERNEL_ID)

#define IP_NORMA_MAX_NODE	(0xfff)
#define IP_NORMA_FAKE_NODE	(0xffffffff)

extern ipc_port_t		host_special_port[MAX_SPECIAL_ID];

/*
 * Bounds checking macro for nodes.
 */
#if	NORMA_ETHER
#define	node_is_valid(n)	(netipc_node_valid(n))
#else
#define	node_is_valid(n)	(n < IP_NORMA_MAX_NODE)
#endif

extern boolean_t		netipc_node_valid(
					unsigned long	node);

extern unsigned long		node_self(void);

extern void			netipc_network_init(void);

/*
 *	Reserved values for passing NULL and DEAD ports
 */

#define IP_NORMA_NULL		0
#define IP_NORMA_DEAD		IP_NORMA_LID(0xffffffff)

/*
 * Definitions for send right reference counting.
 */
extern void			norma_transit_yield_init(void);

extern kern_return_t		norma_ipc_send_stransit_request(
					unsigned long	node,
					unsigned long	uid,
					int		*stransit,
					unsigned	*new_node);

extern void			norma_ipc_send_yield_transits(
					unsigned long	dest_node,
					unsigned long	uid,
					int		stransit,
					int		sotransit);

extern void			norma_transit_yield_threadcall(void);

extern boolean_t		norma_transit_yield_timeout(void);

/*
 * Definitions for norma_port_table management.
 */
extern void			norma_uid_array_init(void);

extern ipc_port_t		norma_port_lookup(
					unsigned long	uid,
					boolean_t	wait);

extern void			norma_port_insert(
					ipc_port_t	port,
					boolean_t	wait);

extern void			norma_port_remove(
					ipc_port_t	port);

extern void			norma_port_remove_try(
					ipc_port_t	port);

#if	MACH_KDB
extern void			norma_port_table_statistics(void);

extern int			db_show_all_uids(void);

extern int			db_show_all_uids_verbose(void);

extern int			db_show_all_proxies(void);

extern int			db_show_all_principals(void);

extern int			db_show_all_deads(void);

#endif	/* MACH_KDB */

#endif	/* _NORMA_IPC_NODE_H_ */
