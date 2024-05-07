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
 * Revision 2.1.1.7.1.10  92/09/15  17:33:41  jeffreyh
 * 	Remove norma dead name logic (fproxy_cancel).
 * 	[92/09/15            dlb]
 * 	New norma uid scheme using a bit array
 * 	[92/08/07            sjs]
 * 
 * 	Call norma_ipc_node_fproxy_cancel when removing migrated principal.
 * 	[92/08/07            dlb]
 * 
 * 	Optimizations: 
 * 	Combine norma_port_{insert,remove} with _locked versions
 * 		since latter are unused outside this file (lookup_locked is used).
 * 	Fold generation of new norma uid into norma_port_insert.
 * 	norma_port_tabled() becomes a macro in ipc_node.h
 * 	[92/08/06            dlb]
 * 
 * 	Simplify check in norma_port_remove_locked because ip_norma_forward
 * 	is now set in root node case.  Also, don't reinitialize no more
 * 	senders when deciding not to remove a dead port.
 * 	[92/08/06            dlb]
 * 
 * Revision 2.1.1.7.1.9  92/06/24  18:00:47  jeffreyh
 * 	Proxies now maintain sotransit; simplify norma_port_remove_try.
 * 	[92/06/01            dlb]
 * 
 * Revision 2.1.1.7.1.8  92/05/27  00:49:09  jeffreyh
 * 	Do not remove ports that are being used to forward other
 * 	requests.  This should change in the future.
 * 	[92/05/18            sjs]
 * 
 * Revision 2.1.1.7.1.7  92/04/08  15:45:33  jeffreyh
 * 	Improve tests in norma_port_remove_try().
 * 	[92/04/01            dlb]
 * 
 * Revision 2.1.1.7.1.6  92/03/28  10:11:32  jeffreyh
 * 	When leaving a proxy to act as a forwarder, reinitialize
 * 	its no senders request so it can receive send rights.
 * 	[92/03/25            dlb]
 * 
 * 	Change norma_port_remove_try to check transit counts instead
 * 	of right counts.
 * 	[92/03/25            dlb]
 * 
 * Revision 2.1.1.7.1.4  92/02/21  11:24:24  jsb
 * 	Added new routine norma_port_remove_try.
 * 	[92/02/20  17:15:49  jsb]
 * 
 * 	Added db_show_{uid,all_uids}_verbose.
 * 	[92/02/20  10:33:47  jsb]
 * 
 * Revision 2.1.1.7.1.3  92/01/21  21:51:19  jsb
 * 	Now uses hash table. Now uses netipc lock instead of spls.
 * 	Removed seqno debugging hack. Added norma_port_iterate and
 * 	reimplemented db_show_uid (nee norma_list_all_ports) with it.
 * 	Added support for show all {proxies, principals}. Use kdbprintf
 * 	instead of printf for these routines for pagination.
 * 	[92/01/16  22:46:09  jsb]
 * 
 * Revision 2.1.1.7.1.2  92/01/03  16:37:27  jsb
 * 	Use ipc_port_release instead of ip_release to allow port deallocation.
 * 	[91/12/31  21:42:40  jsb]
 * 
 * 	Added norma_port_tabled function.
 * 	[91/12/31  17:17:02  jsb]
 * 
 * 	In norma_port_remove, refuse to remove a proxy that started out as a
 * 	principal, since our forwarding algorithms require it to stay around.
 * 	[91/12/31  12:19:08  jsb]
 * 
 * 	Hacked norma_new_uid to incorporate incarnation in generated uids.
 * 	Threw in a stub for norma_ipc_cleanup_incarnation.
 * 	[91/12/29  16:08:32  jsb]
 * 
 * 	Lots of ugly but useful debugging support.
 * 	[91/12/28  18:45:37  jsb]
 * 
 * 	Made norma_list_all_ports print port addresses as well as uids.
 * 	[91/12/25  16:57:46  jsb]
 * 
 * 	First checkin. Split from norma/ipc_transit.c.
 * 	[91/12/24  14:26:58  jsb]
 * 
 * Revision 2.1.1.7.1.1  92/01/03  08:54:06  jsb
 * 	First NORMA branch checkin.
 * 
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
 *	File:	norma/ipc_list.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Maintains list of norma ports.
 */

#include <mach_kdb.h>

#include <machine/machparam.h>
#include <kern/mach_param.h>
#include <kern/misc_protos.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_object.h>
#include <norma/ipc_node.h>

/*
 * Definitions for norma special ports.
 */
extern unsigned long	node_incarnation;
extern ipc_port_t	host_special_port[MAX_SPECIAL_ID];

/*
 * Forward definitions.
 */
#if	MACH_KDB
void		norma_port_iterate(
			void		(*function)(ipc_port_t, void *),
			void		*args);

void		norma_port_by_uid_compare(
			ipc_port_t	port,
			void		*args);

ipc_port_t	norma_port_by_uid(
			unsigned long	uid);

void		db_show_uid(
			ipc_port_t	port,
			void		*args);

void		db_show_uid_verbose(
			ipc_port_t	port,
			void		*args);

void		db_show_proxy(
			ipc_port_t	port,
			void		*args);

void		db_show_principal(
			ipc_port_t	port,
			void		*args);

void		db_show_dead(
			ipc_port_t	port,
			void		*args);

void		db_count_proxy(
			ipc_port_t	port,
			void		*args);

void		db_count_principal(
			ipc_port_t	port,
			void		*args);

void		db_count_null_port(
			ipc_port_t	port,
			void		*args);

void		db_count_pullrecv_proxy(
			ipc_port_t	port,
			void		*args);

void		db_count_kotype(
			ipc_port_t	port,
			void		*args);

void		db_count_dead_port(
			ipc_port_t	port,
			void		*args);

int		db_count_all_proxies(void);

int		db_count_all_principals(void);

int		db_count_all_null_ports(void);

int		db_count_all_pullrecv_proxies(void);

int		db_count_all_kotypes(void);

int		db_count_all_dead_ports(void);

void		db_norma_memory(void);

#endif	/* MACH_KDB */

/*
 * A bit array scheme is used to garauntee norma uids are unique.
 * Note that all uses of uid_bitar must be under lock conditions.
 */
#define	LONGBITS	(sizeof(long) * 8)

unsigned long uid_bitar[PORT_MAX / LONGBITS];
long	uid_bit_ptr = (MAX_SPECIAL_KERNEL_ID + 10) / LONGBITS + 1;
spinlock_t uid_bitar_lock;

#ifdef noyet
norma_ipc_cleanup_incarnation(
	unsigned long	remote)
{
	/*
	 * XXX
	 * What exactly do we clean up???
	 */
}
#endif

#define	HASH_SIZE	128		/* must be power of two */
#define	HASH_MASK	(HASH_SIZE - 1)
#define	UID_HASH(uid)	((IP_NORMA_NODE(uid) + IP_NORMA_LID(uid)) & HASH_MASK)

struct norma_port_table {
	ipc_port_t	port;		/* port associated */
	boolean_t	locked;		/* queue lock held */
	unsigned int	wanted;		/* queue lock wanted */
	spinlock_t	lock;		/* associated lock */
} norma_port_table[HASH_SIZE];

int c_norma_port_lookup = 0;
int c_norma_port_insert = 0;
int c_norma_port_remove = 0;

void
norma_uid_array_init(void)
{
	register i;
	register unsigned long *iv = uid_bitar;

	for (i = 0; i < PORT_MAX / LONGBITS; i++)
		*iv++ = 0xffffffff;
	for (i = 0; i < HASH_SIZE; i++)
		spinlock_init(&norma_port_table[i].lock);
	spinlock_init(&uid_bitar_lock);
}
/*
 * norma_port_lookup is called with queue_lock held. Upon return,
 * 	the port is locked. The wait argument forces the caller to
 *	wait for the completion of a concurrent insertion.
 */

ipc_port_t
norma_port_lookup(
	unsigned long	uid,
	boolean_t	wait)
{
	register ipc_port_t p;
	struct norma_port_table *pt;

	c_norma_port_lookup++;
	if (uid == 0)
		return IP_NULL;

	pt = &norma_port_table[UID_HASH(uid)];
	spinlock_lock(&pt->lock);

	if (wait) {
		while (pt->locked) {
			pt->wanted++;
			thread_sleep_spinlock((int)pt, &pt->lock, FALSE);
			spinlock_lock(&pt->lock);
		}
		pt->locked = TRUE;
	}

	for (p = pt->port; p; p = p->ip_norma_next) {
		if (p->ip_norma_uid == uid) {
			if (wait) {
				pt->locked = FALSE;
				if (pt->wanted > 0) {
					pt->wanted--;
					thread_wakeup_one((int)pt);
				}
			}
			ip_lock(p);
			/*
			 * If p is about to be removed from the list,
			 * then ignore it.
			 */
			if (p->ip_norma_tabled) {
				spinlock_unlock(&pt->lock);
				return p;
			}
			ip_unlock(p);
		}
	}
	spinlock_unlock(&pt->lock);
	return IP_NULL;
}

/*
 * norma_port_insert is called with a locked port.
 *	Upon return, the port remains locked. The wait flag wakes up
 *	other threads that might be waiting for the insertion.
 */
void
norma_port_insert(
	ipc_port_t	port,
	boolean_t	wait)
{
	struct norma_port_table *pt;
	register unsigned long uid;
	unsigned long bentry;
	int wbit;


	c_norma_port_insert++;
	assert(!port->ip_norma_tabled);
	ip_reference(port);
	ip_unlock(port);

	/*
	 * If uid is zero, allocate one (newly exported port).
	 * If caller had another uid in mind, it's already in
	 * port->ip_norma_uid.
	 *
	 * This scheme will guarantee unique uids so long as 
	 * MAX_SPECIAL_KERNEL_ID < 64K (fits in 16 bits).
	 */
	uid = port->ip_norma_uid;

	if (uid == 0) {
		spinlock_lock(&uid_bitar_lock);
		bentry = uid_bitar[uid_bit_ptr];
		wbit = ffs(bentry) - 1;

		uid = uid_bit_ptr * LONGBITS + wbit;
		uid_bitar[uid_bit_ptr] = bentry ^ (1 << wbit);

		/*
		 * point the index to the next available entry.
		 * XXX we currently dont worry about running out of bits,
		 * XXX which should result in a panic.
		 */
		while (uid_bitar[uid_bit_ptr] == 0) {
			if (++uid_bit_ptr >= PORT_MAX / LONGBITS)
				uid_bit_ptr = (MAX_SPECIAL_KERNEL_ID + 10) /
				    	      LONGBITS + 1;
		}
		spinlock_unlock(&uid_bitar_lock);
		uid = IP_NORMA_UID(node_self(), uid);
	}
	assert(uid != 0);

	pt = &norma_port_table[UID_HASH(uid)];
	spinlock_lock(&pt->lock);
	ip_lock(port);
	if (port->ip_norma_tabled) {
		/*
		 * Port has concurrently been tabled, then
		 * return the uid to the pool and return.
		 */
		spinlock_unlock(&pt->lock);
		spinlock_lock(&uid_bitar_lock);
		bentry = IP_NORMA_LID(uid);
		wbit = bentry & (LONGBITS - 1);
		bentry = bentry / LONGBITS;
		/*
		 * XXX This assert should go away in the
		 * XXX near future.
		 */
		assert((uid_bitar[bentry] & (1 << wbit)) == 0);
		uid_bitar[bentry] |= 1 << wbit;

		/*
		 *	If the uid bit pointer points at
		 *	this uid entry, we risk a quick
		 *	recycle.  Bump the pointer.
		 */
		if (bentry == uid_bit_ptr) {
			do {
				if (++uid_bit_ptr >= PORT_MAX / LONGBITS)
					uid_bit_ptr =
						(MAX_SPECIAL_KERNEL_ID + 10) /
							LONGBITS + 1;
			} while (uid_bitar[uid_bit_ptr] == 0);
		}
		spinlock_unlock(&uid_bitar_lock);
		return;
	}
	port->ip_norma_tabled = TRUE;
	port->ip_norma_uid = uid;
	port->ip_norma_next = pt->port;
	pt->port = port;
	if (wait) {
		pt->locked = FALSE;
		if (pt->wanted > 0) {
			pt->wanted--;
			thread_wakeup_one((int)pt);
		}
	}
	spinlock_unlock(&pt->lock);
}

/*
 * norma_port_remove is called with a locked port
 *	which is consumed upon return.
 */
void
norma_port_remove(
	ipc_port_t	port)
{
	register unsigned long uid;
	register ipc_port_t p, *pp;
	register int node;
	int wbit, bentry;

	c_norma_port_remove++;
	assert(port->ip_norma_tabled);
	assert(port->ip_references > 0);

	uid = port->ip_norma_uid;
	assert(uid != 0);

	/*
	 * Don't remove proxies that are forwarding pointers to receive
	 * rights.  They get cleaned up separately.
	 */
	if (port->ip_norma_is_proxy && port->ip_norma_forward) {
		/*
		 *	Reinitialize the nsrequest field in case this port
		 *	acquires any more rights (it was likely cleared
		 *	on the way into here).  This logic doesn't
		 *	belong here either.
		 */
		if (ip_active(port)) {
		        port->ip_nsrequest = ip_nsproxym(port);
		}
		ip_unlock(port);
		printf1("norma_port_remove: not removing port %x\n", port);
		return;
	}

	/*
	 * return the uid to the pool
	 */
	if ((node = IP_NORMA_NODE(uid)) == node_self()){
		spinlock_lock(&uid_bitar_lock);
		bentry = IP_NORMA_LID(uid);
		wbit = bentry & (LONGBITS - 1);
		bentry = bentry / LONGBITS;
		/*
		 * XXX This assert should go away in the
		 * XXX near future.
		 */
		assert((uid_bitar[bentry] & (1 << wbit)) == 0);
		uid_bitar[bentry] |= 1 << wbit;

		/*
		 *	If the uid bit pointer points at
		 *	this uid entry, we risk a quick
		 *	recycle.  Bump the pointer.
		 */
		if (bentry == uid_bit_ptr) {
			do {
				if (++uid_bit_ptr >= PORT_MAX / LONGBITS)
					uid_bit_ptr =
						(MAX_SPECIAL_KERNEL_ID + 10) /
							LONGBITS + 1;
			} while (uid_bitar[uid_bit_ptr] == 0);
		}
		spinlock_unlock(&uid_bitar_lock);
	}
	port->ip_norma_tabled = FALSE;
	ip_unlock(port);

	/*
	 * We can go ahead now.
	 */
	spinlock_lock(&norma_port_table[UID_HASH(uid)].lock);
	pp = &norma_port_table[UID_HASH(uid)].port;
	for (p = *pp; p; pp = &p->ip_norma_next, p = *pp) {
		if (p == port) {
			*pp = port->ip_norma_next;
			port->ip_norma_next = port;
			spinlock_unlock(&norma_port_table[UID_HASH(uid)].lock);
			printf1("norma_port_remove(0x%x:%x): refs %d\n",
				port, uid, port->ip_references - 1);
			ipc_port_release(port);
			return;
		}
	}
	panic("norma_port_remove(0x%x:%x): not found\n", port, uid);
}

/*
 * norma_port_remove_try is called with a locked port
 *	which is consumed upon return.
 */
void
norma_port_remove_try(
	ipc_port_t	port)
{
	/*
	 *	This can be called on both ports and proxies.
	 *	For ports, we want to know that there are no
	 *	rights in the norma system (transits are zero).
	 *	For ports we leave the port tabled
	 *	as long as it is not dead.  Also don't remove
	 *	untabled ports (they're already gone).
	 */
	if (port->ip_norma_stransit == 0 && port->ip_norma_sotransit == 0 &&
	    port->ip_norma_tabled &&
	    (IP_NORMA_IS_PROXY(port) || !ip_active(port))) {
		printf1("norma_port_remove_try: releasing 0x%x:%x\n",
			port, port->ip_norma_uid);
		norma_port_remove(port); /* consumes lock */
	} else
		ip_check_unlock(port);
}


#if	MACH_KDB
#include <ddb/db_output.h>

void
norma_port_table_statistics(void)
{
	int i, total, average, len, longest;
	int l1, l2, l3;
	ipc_port_t p;

	total = 0;
	for (i = 0; i < HASH_SIZE; i++) {
		for (p = norma_port_table[i].port; p; p = p->ip_norma_next) {
			total++;
		}
	}
	average = total / HASH_SIZE;
	if (average == 0) {
		average = 1;
	}
	longest = l1 = l2 = l3 = 0;
	for (i = 0; i < HASH_SIZE; i++) {
		len = 0;
		for (p = norma_port_table[i].port; p; p = p->ip_norma_next) {
			len++;
		}
		if (longest < len) {
			longest = len;
		}
		if (len > average) {
			l1++;
			if (len > 2 * average) {
				l2++;
				if (len > 3 * average) {
					l3++;
				}
			}
		}
	}
	kdbprintf("%d ports, %d buckets, average %d per bucket\n",
		  total, HASH_SIZE, total / HASH_SIZE);
	kdbprintf("%d chains longer than %d, %d > %d, %d > %d; longest = %d\n",
		  l1, average, l2, 2 * average, l3, 3 * average, longest);
}

void
norma_port_iterate(
	void	(*function)(ipc_port_t, void *),
	void	*args)
{
	int i;
	ipc_port_t p;

	for (i = 0; i < HASH_SIZE; i++) {
		for (p = norma_port_table[i].port; p; p = p->ip_norma_next) {
			(*function)(p, args);
		}
	}
	(*function)(IP_NULL, args);
}

struct port_and_uid {
	ipc_port_t	port;
	unsigned long	uid;
};

void
norma_port_by_uid_compare(
	ipc_port_t	port,
	void		*args)
{
	struct port_and_uid *pau = (struct port_and_uid *) args;

	if (port && port->ip_norma_uid == pau->uid) {
		pau->port = port;
	}
}

ipc_port_t
norma_port_by_uid(
	unsigned long	uid)
{
	struct port_and_uid pau;

	pau.port = IP_NULL;
	pau.uid = uid;
	norma_port_iterate(norma_port_by_uid_compare, (void *) &pau);
	return pau.port;
}

#include <ddb/db_sym.h>

void
db_show_uid(
	ipc_port_t	port,
	void		*args)
{
	int *countp = (int *) args;

	if (port == IP_NULL) {
		printf("\n");
		return;
	}
	if (++*countp == 4) {
		kdbprintf("%x:%x\n", port, port->ip_norma_uid);
		*countp = 0;
	} else {
		kdbprintf("%x:%x ", port, port->ip_norma_uid);
	}
}

void
db_show_uid_verbose(
	ipc_port_t	port,
	void		*args)
{
	if (port == IP_NULL) {
		return;
	}
#if	MACH_ASSERT
	kdbprintf("%x:%x %6d ",
		  port, port->ip_norma_uid, port->ip_norma_spare2);
	db_printsym(port->ip_norma_spare1, DB_STGY_PROC);
	kdbprintf("\n");
#endif
}

void
db_show_proxy(
	ipc_port_t	port,
	void		*args)
{
	if (port == IP_NULL || port->ip_norma_is_proxy) {
		db_show_uid(port, args);
	}
}

void
db_show_principal(
	ipc_port_t	port,
	void		*args)
{
	if (port == IP_NULL || ! port->ip_norma_is_proxy) {
		db_show_uid(port, args);
	}
}


void
db_show_dead(
	ipc_port_t	port,
	void		*args)
{
	if (port != IP_NULL && !io_active(&port->ip_object))
		db_show_uid(port, args);
}


void
db_count_proxy(
	ipc_port_t	port,
	void		*args)
{
	int *countp = (int *) args;

	if (port != IP_NULL && port->ip_norma_is_proxy)
		++*countp;
}

void
db_count_principal(
	ipc_port_t	port,
	void		*args)
{
	int *countp = (int *) args;

	if (port != IP_NULL && !port->ip_norma_is_proxy)
		++*countp;
}


void
db_count_null_port(
	ipc_port_t	port,
	void		*args)
{
	int *countp = (int *) args;

	if (port == IP_NULL)
		++*countp;
}


void
db_count_pullrecv_proxy(
	ipc_port_t	port,
	void		*args)
{
	int *countp = (int *) args;

#if	MACH_ASSERT
	if (port != IP_NULL && port->ip_norma_is_proxy
	    && (port->ip_norma_spare2 == 556104 ||
		port->ip_norma_spare2 == -556104))
		++*countp;
#else
	*countp = -1;
#endif
}

unsigned int ikot_types[IKOT_MAX_TYPE];

void
db_count_kotype(
	ipc_port_t	port,
	void		*args)
{
    int *countp = (int *) args;

    if (port != IP_NULL && !port->ip_norma_is_proxy) {
	int type = ip_kotype(port);
	if (type >= 0 && type < IKOT_MAX_TYPE)
	    ikot_types[type]++;
	else
	    ikot_types[IKOT_UNKNOWN]++;
	++*countp;
    }
}


void
db_count_dead_port(
	ipc_port_t	port,
	void		*args)
{
	int *countp = (int *) args;

	if (port != IP_NULL && !io_active(&port->ip_object))
		++*countp;
}

int
db_show_all_uids(void)
{
	int count = 0;

	norma_port_table_statistics();
	norma_port_iterate(db_show_uid, (void *) &count);
	return(count);
}

int
db_show_all_uids_verbose(void)
{
	int count = 0;

	norma_port_table_statistics();
	norma_port_iterate(db_show_uid_verbose, (void *) &count);
	return(count);
}

int
db_show_all_proxies(void)
{
	int count = 0;

	norma_port_iterate(db_show_proxy, (void *) &count);
	return(count);
}

int
db_show_all_principals(void)
{
	int count = 0;

	norma_port_iterate(db_show_principal, (void *) &count);
	return(count);
}

int
db_show_all_deads(void)
{
	int count = 0;

	norma_port_iterate(db_show_dead, (void *) &count);
	return(count);
}

int
db_count_all_proxies(void)
{
	int	count = 0;

	norma_port_iterate(db_count_proxy, (void *) &count);
	kdbprintf("total proxies:  0x%x\n", count);
	return(count);
}

int
db_count_all_principals(void)
{
	int	count = 0;

	norma_port_iterate(db_count_principal, (void *) &count);
	kdbprintf("total principals:  0x%x\n", count);
	return(count);
}

int
db_count_all_null_ports(void)
{
	int	count = 0;

	norma_port_iterate(db_count_null_port, (void *) &count);
	kdbprintf("total null ports:  0x%x\n", count);
	return(count);
}

int
db_count_all_pullrecv_proxies(void)
{
	int	count = 0;

	norma_port_iterate(db_count_pullrecv_proxy, (void *) &count);
	kdbprintf("total pullrecv proxies:  0x%x\n", count);
	return(count);
}

int
db_count_all_kotypes(void)
{
	int	i, count = 0;

	for (i = 0; i < IKOT_MAX_TYPE; ++i)
		ikot_types[i] = 0;
	norma_port_iterate(db_count_kotype, (void *) &count);
	for (i = 0; i < IKOT_MAX_TYPE; ++i)
		kdbprintf("ikot type %d%s ports:  0x%x\n",
				i, ikot_print_array[i], ikot_types[i]);
	kdbprintf("Total count: ");
	return(count);
}

int
db_count_all_dead_ports(void)
{
	int	count = 0;

	norma_port_iterate(db_count_dead_port, (void *) &count);
	kdbprintf("total dead ports:  0x%x\n", count);
	return(count);
}

void
db_norma_memory(void)
{
	/*
	 *	netipc_vm_map_copy_list
	 *	netipc_safe_ikm_free_list
	 *	norma_kmsg_incomplete_queue_head
	 *	norma_kmsg_complete
	 *	kernel_kmsg
	 *	kernel_critical_kmsg
	 *	netipc_page_list
	 *	norma_kmsg_receive_error
	 *	netipc_dead_port_list
	 *	netipc_safe_vm_map_copy_discard_list
	 *	netipc_continuing_packet_list
	 *	all ports in NORMA
	 *	io_done_list
	 *	pcb_free_list (comes from pcb_zone)
	 *
	 *	fix netipc page pool allocation (less aggressive)
	 *	kserver thread allocation (less aggressive)
	 *	fixed priority for all threads (vnode pager, iodone, etc.)
	 *
	 *	shut off netipc as a function of wire count, too
	 *		...detect "shoot-up" conditions
	 *	send messages out more slowly when suspended
	 *	make pageout scan MORE aggressive
	 */
}

#endif	/* MACH_KDB */
