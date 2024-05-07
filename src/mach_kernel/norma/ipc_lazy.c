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
 *	Joseph S. Barrera III 1992
 *
 *	Functions for lazy evaluation of send right reference counting...
 *	and perhaps someday, if useful, dead-name notifications?
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1992 Carnegie Mellon University
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

#include <ipc/ipc_port.h>
#include <ipc/ipc_space.h>
#include <kern/host.h>
#include <kern/misc_protos.h>
#include <norma/norma_internal.h>
#include <norma/norma_internal_server.h>
#include <norma/ipc_node.h>

#define	MAX_TRANSITS		128

typedef struct transit_element	*transit_element_t;
typedef struct transit_vector	*transit_vector_t;

#define	TRANSIT_ELEMENT_NULL	((transit_element_t) 0)
#define	TRANSIT_VECTOR_NULL	((transit_vector_t) 0)

struct transit_element {
	unsigned long	uid;
	unsigned long	stransit;
	unsigned long	sotransit;
};

#define	TLENGTH		(sizeof(struct transit_element) / sizeof(int))

struct transit_vector {
	unsigned long	dest_node;
	int		count;
	boolean_t	flushing;	/* flushing transit vector */
	boolean_t	wanted;		/* waiting for transit vector */
	spinlock_t	lock;
	struct transit_element transit_element[MAX_TRANSITS];
};

/*
 * transit_vector_lock protects access to the transit_vector queue,
 * transit_vector_count and norma_transit_yield_timeout variables.
 * XXX Only one element in the transit vector queue for the moment.
 */
struct transit_vector	transit_vector_only;
int			transit_vector_count;
spinlock_t		transit_vector_lock;
int			norma_transit_yield_timeouts = 0;

int JX5 = 0;
int JX11 = 0;

/*
 * Forward.
 */
transit_vector_t	norma_ipc_get_transit_vector(
				unsigned long	dest_node);

transit_vector_t	norma_ipc_lookup_transit_vector(
				unsigned long	dest_node);

void			norma_ipc_flush_all_yield_transits(void);

void			norma_ipc_flush_yield_transits(
				transit_vector_t	tv);

/*
 * Module initialization.
 */
void
norma_transit_yield_init(void)
{
	spinlock_init(&transit_vector_lock);
	spinlock_init(&transit_vector_only.lock);
}

/*
 * Get transit vector buffer for given node.
 * If necessary, flush old vector for another node.
 */
transit_vector_t
norma_ipc_get_transit_vector(
	unsigned long	dest_node)
{
	transit_vector_t tv;

	/*
	 * XXX
	 * Right now, we only have one active vector...
	 */
	tv = &transit_vector_only;
	spinlock_lock(&tv->lock);
	for (;;) {
		if (tv->dest_node == dest_node) {
			if (!tv->flushing)
				return tv;
		} else if (tv->count == 0) {
			tv->dest_node = dest_node;
			return tv;
		}
		norma_ipc_flush_yield_transits(tv);
	}
}

/*
 * Get current transit vector buffer for given node, or null if there is none.
 * MP notes : Upon return, the current transit vector is locked and there is
 *		no flushing currently in progress.
 */
transit_vector_t
norma_ipc_lookup_transit_vector(
	unsigned long	dest_node)
{
	transit_vector_t tv;

	/*
	 * XXX
	 * Right now, we only have one active vector...
	 */
	tv = &transit_vector_only;
	spinlock_lock(&tv->lock);
	while (tv->dest_node == dest_node && tv->count > 0) {
		if (!tv->flushing)
			return tv;
		tv->wanted = TRUE;
		thread_sleep_spinlock((int)tv, &tv->lock, FALSE);
		spinlock_lock(&tv->lock);
	}
	spinlock_unlock(&tv->lock);
	return TRANSIT_VECTOR_NULL;
}

int ssr_calls = 0;
int ssr_hits = 0;

kern_return_t
norma_ipc_send_stransit_request(
	unsigned long	node,
	unsigned long	uid,
	int		*stransit,
	unsigned	*new_node)
{
	ipc_port_t host;
	int i;
	transit_vector_t tv;
	transit_element_t te;

	/*
	 * Check to see if we have some stransits in the laundry.
	 */
	tv = norma_ipc_lookup_transit_vector(node);
	if (tv != TRANSIT_VECTOR_NULL) {
		for (i = 0; i < tv->count; i++) {
			te = &tv->transit_element[i];
			if (te->uid == uid) {

				/*
				 * We have found the matching transit element.
				 * If it has no stransits, give up now.
				 */
				if (te->stransit == 0) {
					break;
				}

				/*
				 * Move stransits from element to caller.
				 * If there are no sotransits in the element,
				 * then it is now empty, so deallocate the
				 * element. In any case, we are done.
				 */
				*stransit = te->stransit;
				if (te->sotransit == 0) {
					*te = tv->transit_element[--tv->count];
					spinlock_unlock(&tv->lock);
					spinlock_lock(&transit_vector_lock);
					transit_vector_count--;
					spinlock_unlock(&transit_vector_lock);
				} else {
					te->stransit = 0;
					spinlock_unlock(&tv->lock);
				}
				return KERN_SUCCESS;
			}
		}
		spinlock_unlock(&tv->lock);
	}

	/*
	 * Fall back on rpc.
	 */
	host = remote_host_priv(node, MACH_MSG_TYPE_PORT_SEND);
	return r_norma_ipc_stransit_request(host, uid, stransit, new_node);
}

int ipc_lazy_enable = 1;

void
norma_ipc_send_yield_transits(
	unsigned long	dest_node,
	unsigned long	uid,
	int		stransit,
	int		sotransit)
{
	transit_vector_t tv;
	transit_element_t te;

	/*
	 * Bypass lazy code if appropriate.
	 */
	if (! ipc_lazy_enable) {
		kern_return_t kr;
		struct transit_element te_space;
		ipc_port_t host;

		te = &te_space;
		te->uid = uid;
		te->stransit = stransit;
		te->sotransit = sotransit;
		host = remote_host_priv(dest_node, MACH_MSG_TYPE_PORT_SEND);
		kr = r_norma_ipc_yield_transits_vector(
			host, (norma_transit_vector_t)te, TLENGTH);
		assert(kr == KERN_SUCCESS);
		return;
	}

	if (JX11) {
		printf("(%x %d,%d) ", uid, stransit, sotransit);
	}

	/*
	 * Allocate or find a transit vector for dest_node.
	 */
	tv = norma_ipc_get_transit_vector(dest_node);

	/*
	 * Record delayed yield.
	 */
	te = &tv->transit_element[tv->count];
	te->uid = uid;
	te->stransit = stransit;
	te->sotransit = sotransit;
	tv->count++;
	assert(tv->count <= MAX_TRANSITS);

	/*
	 * First force a flush of our vector to set flushing state.
	 */
	if (tv->count == MAX_TRANSITS) {
		norma_ipc_flush_yield_transits(tv);
	}
	spinlock_unlock(&tv->lock);

	/*
	 * Force a flush of delayed yields if appropriate.
	 */
	spinlock_lock(&transit_vector_lock);
	transit_vector_count++;
	if (norma_transit_yield_timeouts > 0) {
		spinlock_unlock(&transit_vector_lock);
		norma_ipc_flush_all_yield_transits();
	} else
		spinlock_unlock(&transit_vector_lock);
}

void
norma_ipc_flush_all_yield_transits(void)
{
	transit_vector_t tv;

	/*
	 * XXX
	 * This will get more interesting once we have more than
	 * one transit vector.
	 */
	tv = &transit_vector_only;	/* XXX */
	spinlock_lock(&tv->lock);
	if (tv->count > 0) {
		norma_ipc_flush_yield_transits(tv);
	}
	spinlock_unlock(&tv->lock);
}

/*
 * MP notes: Upon call, the transit_vector is locked. It may be unlocked
 *	during the call.
 */
void
norma_ipc_flush_yield_transits(
	transit_vector_t	tv)
{
	ipc_port_t host;
	kern_return_t kr;
	unsigned long node;

	node = tv->dest_node;
	while (tv->flushing) {
		tv->wanted = TRUE;
		thread_sleep_spinlock((int)tv, &tv->lock, FALSE);
		spinlock_lock(&tv->lock);
	}
	/*
	 * Flush has already been finished if destination node has changed or
	 * if counter is NULL.
	 */
	if (tv->dest_node != node || tv->count == 0) {
		/* Somebody else got here first */
		return;
	}
	tv->flushing = TRUE;
	spinlock_unlock(&tv->lock);

	if (JX5) {
		printf("ticks=%d tv->count=%d\n",
		       norma_transit_yield_timeouts, tv->count);
	}

	spinlock_lock(&transit_vector_lock);
	transit_vector_count -= tv->count;
	norma_transit_yield_timeouts = 0;	/* XXX ??? */
	spinlock_unlock(&transit_vector_lock);

	host = remote_host_priv(tv->dest_node, MACH_MSG_TYPE_PORT_SEND);
	if (JX11) {
		printf("((%d)) ", tv->count);
	}
	kr = r_norma_ipc_yield_transits_vector(
		host, (norma_transit_vector_t)tv->transit_element,
		tv->count * TLENGTH);
	if (kr != KERN_SUCCESS) {
		panic("norma_ipc_flush_yield_transits: r_ failed: %d/%x\n",
		      kr, kr);
	}

	spinlock_lock(&tv->lock);
	tv->count = 0;
	tv->flushing = FALSE;
	if (tv->wanted) {
		tv->wanted = FALSE;
		thread_wakeup((int)tv);
	}
}

kern_return_t
norma_ipc_yield_transits_vector(
	host_t		host_priv,
	int		*transit_vector,
	unsigned	transit_vector_size)
{
	int i, count = transit_vector_size / TLENGTH;
	transit_element_t te = (transit_element_t) transit_vector;

	assert(transit_vector_size % TLENGTH == 0);
	if (JX11) {
		printf("<<%d>> ", count);
		for (i = 0; i < count; i++) {
			printf("<%x %d,%d> ",
			       te[i].uid, te[i].stransit, te[i].sotransit);
		}
	}
	for (i = 0; i < count; i++) {
		norma_ipc_yield_transits(host_priv, te[i].uid, te[i].stransit,
					 te[i].sotransit);
	}
	return KERN_SUCCESS;
}

void
norma_transit_yield_threadcall(void)
{
	if (JX5) {
		printf("yield timeout forcing netipc thread wakeup\n");
	}
	norma_ipc_flush_all_yield_transits();
}

boolean_t
norma_transit_yield_timeout(void)
{
	transit_vector_t tv;
	boolean_t ret;

	tv = &transit_vector_only;	/* XXX */
	spinlock_lock(&transit_vector_lock);
	ret = transit_vector_count != 0 && norma_transit_yield_timeouts++ > 0;
	spinlock_unlock(&transit_vector_lock);
	return ret;
}
