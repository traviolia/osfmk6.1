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
 * Revision 2.14.7.2  92/03/03  16:21:01  jeffreyh
 * 	Eliminate keep_wired argument from vm_map_copyin().
 * 	[92/02/21  10:13:57  dlb]
 * 	Increased zome_map_size for 2 servers
 * 	[92/02/24            jeffreyh]
 * 
 * Revision 2.15  92/01/14  16:45:03  rpd
 * 	Changed host_zone_info for CountInOut.
 * 	[92/01/14            rpd]
 * 
 * Revision 2.14  91/05/18  14:34:46  rpd
 * 	Added check_simple_locks.
 * 	Moved ADD_TO_ZONE, REMOVE_FROM_ZONE here.
 * 	Moved extraneous zone GC declarations here.
 * 	[91/03/31            rpd]
 * 
 * 	Minor cleanup in zget_space.
 * 	[91/03/28            rpd]
 * 	Changed to use zdata to initialize zalloc_next_space.
 * 	[91/03/22            rpd]
 * 
 * Revision 2.13  91/05/14  16:50:36  mrt
 * 	Correcting copyright
 * 
 * Revision 2.12  91/03/16  14:53:32  rpd
 * 	Updated for new kmem_alloc interface.
 * 	[91/03/03            rpd]
 * 	Added continuation argument to thread_block.
 * 	[91/02/16            rpd]
 * 
 * Revision 2.11  91/02/05  17:31:25  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:21:52  mrt]
 * 
 * Revision 2.10  91/01/08  15:18:28  rpd
 * 	Added zalloc_wasted_space.
 * 	[91/01/06            rpd]
 * 	Removed COLLECT_ZONE_GARBAGE.
 * 	[91/01/03            rpd]
 * 
 * 	Changed zinit to make zones by default *not* collectable.
 * 	[90/12/29            rpd]
 * 	Added consider_zone_gc.
 * 	[90/11/11            rpd]
 * 
 * Revision 2.9  90/12/20  16:39:11  jeffreyh
 * 	[90/12/19  10:36:55  jeffreyh]
 * 
 * 	10-Dec-90  Jeffrey Heller  (jeffreyh)  at OSF
 * 	Merge in changes from OSF/1 done by jvs@osf
 * 	Zone's are now collectable by default, 
 * 	zchange now takes a collectable argument
 *      include machine/machparam.h for splhigh
 * 
 * Revision 2.8  90/11/05  14:32:08  rpd
 * 	Added zone_check option to zfree.
 * 	[90/10/29            rpd]
 * 
 * Revision 2.7  90/06/19  22:59:49  rpd
 * 	Added zi_collectable field to zone_info structure.
 * 	[90/06/05            rpd]
 * 
 * Revision 2.6  90/06/02  14:57:28  rpd
 * 	Made zone_ignore_overflow TRUE by default.
 * 	When a zone overflows, increase its max_size.
 * 	[90/05/11  17:00:24  rpd]
 * 
 * 	Added host_zone_info.
 * 	[90/03/26  22:28:05  rpd]
 * 
 * Revision 2.5  90/05/03  15:47:04  dbg
 * 	Add host_zone_info.
 * 	[90/04/06            dbg]
 * 
 * Revision 2.4  90/02/22  20:04:23  dbg
 * 	Initialize zone_page_table_lock before using it.
 * 	[90/02/16            dbg]
 * 
 * Revision 2.3  89/11/29  14:09:25  af
 * 	Nullify zone_page_alloc/init if 'garbage' not here.
 * 	[89/10/29  14:23:56  af]
 * 
 * 	Could not compile without the 'garbage' thing cuz a def mizzing.
 * 	[89/10/29  09:35:22  af]
 * 
 * Revision 2.2  89/08/11  17:56:21  rwd
 * 	Added collectible zones.  Collectible zones allow storage to be
 * 	returned to system via kmem_free when pages are no longer used.
 * 	This option should only be used when zone memory is virtual
 * 	(rather than physical as in a MIPS architecture).
 * 	[89/07/22            rfr]
 * 
 * Revision 2.11  89/05/30  10:38:40  rvb
 * 	Make zalloc storage pointers external, so they can be initialized from
 * 	the outside.
 * 	[89/05/30  08:28:14  rvb]
 * 
 * Revision 2.10  89/05/11  14:41:30  gm0w
 * 	Keep all zones on a list that host_zone_info can traverse.
 * 	This fixes a bug in host_zone_info: it would try to lock
 * 	uninitialized zones.  Fixed zinit to round elem_size up
 * 	to a multiple of four.  This prevents zget_space from handing
 * 	out improperly aligned objects.
 * 	[89/05/08  21:34:17  rpd]
 * 
 * Revision 2.9  89/05/06  15:47:11  rpd
 * 	From jsb: Added missing argument to kmem_free in zget_space.
 * 
 * Revision 2.8  89/05/06  02:57:35  rpd
 * 	Added host_zone_info (under MACH_DEBUG).
 * 	Fixed zget to increase cur_size when the space comes from zget_space.
 * 	Use MACRO_BEGIN/MACRO_END, decl_simple_lock_data where appropriate.
 * 	[89/05/06  02:43:29  rpd]
 * 
 * Revision 2.7  89/04/18  16:43:20  mwyoung
 * 	Document zget_space.  Eliminate MACH_XP conditional.
 * 	[89/03/26            mwyoung]
 * 	Make handling of zero allocation size unconditional.  Clean up
 * 	allocation code.
 * 	[89/03/16            mwyoung]
 * 
 * Revision 2.6  89/03/15  15:04:46  gm0w
 * 	Picked up code from rfr to allocate data from non pageable zones
 * 	from a single pool.
 * 	[89/03/09            mrt]
 * 
 * Revision 2.5  89/03/09  20:17:50  rpd
 * 	More cleanup.
 * 
 * Revision 2.4  89/02/25  18:11:15  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 2.3  89/01/18  00:50:51  jsb
 * 	Vnode support: interpret allocation size of zero in zinit as meaning
 * 	PAGE_SIZE.
 * 	[89/01/17  20:57:39  jsb]
 * 
 * Revision 2.2  88/12/19  02:48:41  mwyoung
 * 	Fix include file references.
 * 	[88/12/19  00:33:03  mwyoung]
 * 	
 * 	Add and use zone_ignore_overflow.
 * 	[88/12/14            mwyoung]
 * 
 *  8-Jan-88  Rick Rashid (rfr) at Carnegie-Mellon University
 *	Made pageable zones really pageable.  Turned spin locks to sleep
 *	locks for pageable zones.
 *
 * 30-Dec-87  David Golub (dbg) at Carnegie-Mellon University
 *	Delinted.
 *
 * 20-Oct-87  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Allocate zone memory from a separate kernel submap, to avoid
 *	sleeping with the kernel_map locked.
 *
 *  1-Oct-87  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Added zchange().
 *
 * 30-Sep-87  Richard Sanzi (sanzi) at Carnegie-Mellon University
 *	Deleted the printf() in zinit() which is called when zinit is
 *	creating a pageable zone.
 *
 * 12-Sep-87  Avadis Tevanian (avie) at Carnegie-Mellon University
 *	Modified to use list of elements instead of queues.  Actually,
 *	this package now uses macros defined in zalloc.h which define
 *	the list semantics.
 *
 * 30-Mar-87  Avadis Tevanian (avie) at Carnegie-Mellon University
 *	Update zone's cur_size field when it is crammed (zcram).
 *
 * 23-Mar-87  Avadis Tevanian (avie) at Carnegie-Mellon University
 *	Only define zfree if there is no macro version.
 *
 * 17-Mar-87  David Golub (dbg) at Carnegie-Mellon University
 *	De-linted.
 *
 * 12-Feb-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Added zget - no waiting version of zalloc.
 *
 * 22-Jan-87  Michael Young (mwyoung) at Carnegie-Mellon University
 *	De-linted.
 *
 * 12-Jan-87  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Eliminated use of the old interlocked queuing package;
 *	random other cleanups.
 *
 *  9-Jun-85  Avadis Tevanian (avie) at Carnegie-Mellon University
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
 *	File:	kern/zalloc.c
 *	Author:	Avadis Tevanian, Jr.
 *
 *	Zone-based memory allocator.  A zone is a collection of fixed size
 *	data blocks for which quick allocation/deallocation is possible.
 */

#include <zone_debug.h>
#include <norma_vm.h>
#include <mach_kdb.h>

#include <kern/macro_help.h>
#include <kern/sched.h>
#include <kern/spl.h>
#include <kern/misc_protos.h>
#include <kern/time_out.h>
#include <kern/zalloc.h>
#include <kern/rtalloc.h>
#include <mach/vm_param.h>
#include <vm/vm_kern.h>
#include <machine/machparam.h>

#define ADD_TO_ZONE(zone, element)					\
MACRO_BEGIN								\
		*((vm_offset_t *)(element)) = (zone)->free_elements;	\
		(zone)->free_elements = (vm_offset_t) (element);	\
		(zone)->count--;					\
MACRO_END

#define REMOVE_FROM_ZONE(zone, ret, type)				\
MACRO_BEGIN								\
	(ret) = (type) (zone)->free_elements;				\
	if ((ret) != (type) 0) {					\
		(zone)->count++;					\
		(zone)->free_elements = *((vm_offset_t *)(ret));	\
	}								\
MACRO_END

#if	ZONE_DEBUG
#define zone_debug_enabled(z) z->active_zones.next
#endif	/* ZONE_DEBUG */

/*
 * Support for garbage collection of unused zone pages:
 */

struct zone_page_table_entry {
	struct	zone_page_table_entry	*next;
	short 	in_free_list;
	short	alloc_count;
};

extern struct zone_page_table_entry * zone_page_table;
extern vm_offset_t zone_map_min_address;

#define lock_zone_page_table() spinlock_lock(&zone_page_table_lock)
#define unlock_zone_page_table() spinlock_unlock(&zone_page_table_lock)

#define	zone_page(addr) \
    (&(zone_page_table[(atop(((vm_offset_t)addr) - zone_map_min_address))]))

/* Forwards */
void		zone_page_init(
				vm_offset_t	addr,
				vm_size_t	size,
				int		value);

void		zone_page_alloc(
				vm_offset_t	addr,
				vm_size_t	size);

void		zone_add_free_page_list(
				struct zone_page_table_entry	**free_list,
				vm_offset_t	addr,
				vm_size_t	size);
void		zone_page_dealloc(
				vm_offset_t	addr,
				vm_size_t	size);

void		zone_page_in_use(
				vm_offset_t	addr,
				vm_size_t	size);

void		zone_page_free(
				vm_offset_t	addr,
				vm_size_t	size);

#if	ZONE_DEBUG && MACH_KDB
int		zone_count(
				zone_t		z,
				unsigned	all);
#endif	/* ZONE_DEBUG && MACH_KDB */

zone_t		zone_zone;	/* this is the zone containing other zones */

boolean_t	zone_ignore_overflow = TRUE;

vm_map_t	zone_map = VM_MAP_NULL;
#if	NORMA_VM
vm_size_t	zone_map_size = 16 * 1024 * 1024;
#else	/* NORMA_VM */
vm_size_t	zone_map_size = 12 * 1024 * 1024;
#endif	/* NORMA_VM */

/*
 *	The VM system gives us an initial chunk of memory.
 *	It has to be big enough to allocate the zone_zone
 *	and some initial kernel data structures, like kernel maps.
 *	It is advantageous to make it bigger than really necessary,
 *	because this memory is more efficient than normal kernel
 *	virtual memory.  (It doesn't have vm_page structures backing it
 *	and it may have other machine-dependent advantages.)
 *	So for best performance, zdata_size should approximate
 *	the amount of memory you expect the zone system to consume.
 */

vm_offset_t	zdata;
vm_size_t	zdata_size = 420 * 1024;

#define lock_zone_init(zone)	spinlock_init(&zone->lock)
#define lock_try_zone(zone)	spinlock_try(&zone->lock)
#define lock_zone(zone)		spinlock_lock(&zone->lock)
#define unlock_zone(zone)	spinlock_unlock(&zone->lock)

vm_offset_t		zget_space(
				vm_offset_t size);

spinlock_t	zget_space_lock;
vm_offset_t	zalloc_next_space;
vm_offset_t	zalloc_end_of_space;
vm_size_t	zalloc_wasted_space;

/*
 *	Garbage collection map information
 */
spinlock_t			zone_page_table_lock;
struct zone_page_table_entry *	zone_page_table;
vm_offset_t			zone_map_min_address;
vm_offset_t			zone_map_max_address;
int				zone_pages;

#define	ZONE_PAGE_USED  0
#define ZONE_PAGE_UNUSED -1


/*
 *	Protects first_zone, last_zone, num_zones,
 *	and the next_zone field of zones.
 */
spinlock_t		all_zones_lock;
zone_t			first_zone;
zone_t			*last_zone;
int			num_zones;

/*
 *	zinit initializes a new zone.  The zone data structures themselves
 *	are stored in a zone, which is initially a static structure that
 *	is initialized by zone_init.
 */
zone_t
zinit(
	vm_size_t	size,		/* the size of an element */
	vm_size_t	max,		/* maximum memory to use */
	vm_size_t	alloc,		/* allocation size */
	char		*name)		/* a name for the zone */
{
	register zone_t		z;

	if (zone_zone == ZONE_NULL)
		z = (zone_t) zget_space(sizeof(struct zone));
	else
		z = (zone_t) zalloc(zone_zone);
	if (z == ZONE_NULL)
		panic("zinit");

 	if (alloc == 0)
		alloc = PAGE_SIZE;

	if (size == 0)
		size = sizeof(z->free_elements);
	/*
	 *	Round off all the parameters appropriately.
	 */

	if ((max = round_page(max)) < (alloc = round_page(alloc)))
		max = alloc;

	z->free_elements = 0;
	z->cur_size = 0;
	z->max_size = max;
	z->elem_size = ((size-1) + sizeof(z->free_elements)) -
			((size-1) % sizeof(z->free_elements));

	z->alloc_size = alloc;
	z->zone_name = name;
	z->count = 0;
	z->doing_alloc = FALSE;
	z->exhaustible = z->sleepable = FALSE;
	z->collectable = FALSE;
	z->expandable  = TRUE;

#if	ZONE_DEBUG
	z->active_zones.next = z->active_zones.prev = 0;	
	zone_debug_enable(z);
#endif	/* ZONE_DEBUG */
	lock_zone_init(z);

	/*
	 *	Add the zone to the all-zones list.
	 */

	z->next_zone = ZONE_NULL;
	spinlock_lock(&all_zones_lock);
	*last_zone = z;
	last_zone = &z->next_zone;
	num_zones++;
	spinlock_unlock(&all_zones_lock);

	return(z);
}

/*
 *	Cram the given memory into the specified zone.
 */
void
zcram(
	register zone_t		zone,
	vm_offset_t		newmem,
	vm_size_t		size)
{
	register vm_size_t	elem_size;

	if (newmem == (vm_offset_t) 0) {
		panic("zcram - memory at zero");
	}
	elem_size = zone->elem_size;

	lock_zone(zone);
	while (size >= elem_size) {
		ADD_TO_ZONE(zone, newmem);
		zone_page_alloc(newmem, elem_size);
		zone->count++;	/* compensate for ADD_TO_ZONE */
		size -= elem_size;
		newmem += elem_size;
		zone->cur_size += elem_size;
	}
	unlock_zone(zone);
}

/*
 * Contiguous space allocator for non-paged zones. Allocates "size" amount
 * of memory from zone_map.
 */

vm_offset_t
zget_space(
	vm_offset_t size)
{
	vm_offset_t	new_space = 0;
	vm_offset_t	result;
	vm_size_t	space_to_add;

	spinlock_lock(&zget_space_lock);
	while ((zalloc_next_space + size) > zalloc_end_of_space) {
		/*
		 *	Add at least one page to allocation area.
		 */

		space_to_add = round_page(size);

		if (new_space == 0) {
			/*
			 *	Memory cannot be wired down while holding
			 *	any locks that the pageout daemon might
			 *	need to free up pages.  [Making the zget_space
			 *	lock a complex lock does not help in this
			 *	regard.]
			 *
			 *	Unlock and allocate memory.  Because several
			 *	threads might try to do this at once, don't
			 *	use the memory before checking for available
			 *	space again.
			 */

			spinlock_unlock(&zget_space_lock);

			if (kmem_alloc_wired(zone_map,
					     &new_space, space_to_add)
							!= KERN_SUCCESS)
				return(0);
			zone_page_init(new_space, space_to_add,
							ZONE_PAGE_USED);
			spinlock_lock(&zget_space_lock);
			continue;
		}

		
		/*
	  	 *	Memory was allocated in a previous iteration.
		 *
		 *	Check whether the new region is contiguous
		 *	with the old one.
		 */

		if (new_space != zalloc_end_of_space) {
			/*
			 *	Throw away the remainder of the
			 *	old space, and start a new one.
			 */
			zalloc_wasted_space +=
				zalloc_end_of_space - zalloc_next_space;
			zalloc_next_space = new_space;
		}

		zalloc_end_of_space = new_space + space_to_add;

		new_space = 0;
	}
	result = zalloc_next_space;
	zalloc_next_space += size;		
	spinlock_unlock(&zget_space_lock);

	if (new_space != 0)
		kmem_free(zone_map, new_space, space_to_add);

	return(result);
}


/*
 *	Initialize the "zone of zones" which uses fixed memory allocated
 *	earlier in memory initialization.  zone_bootstrap is called
 *	before zone_init.
 */
void
zone_bootstrap(void)
{
	spinlock_init(&all_zones_lock);
	first_zone = ZONE_NULL;
	last_zone = &first_zone;
	num_zones = 0;

	spinlock_init(&zget_space_lock);
	zalloc_next_space = zdata;
	zalloc_end_of_space = zdata + zdata_size;
	zalloc_wasted_space = 0;

	zone_zone = ZONE_NULL;
	zone_zone = zinit(sizeof(struct zone), 128 * sizeof(struct zone),
			  sizeof(struct zone), "zones");
}

void
zone_init(void)
{
	kern_return_t	retval;
	vm_offset_t	zone_min;
	vm_offset_t	zone_max;

	vm_size_t	zone_table_size;

	retval = kmem_suballoc(kernel_map, &zone_min, zone_map_size,
			       FALSE, TRUE, &zone_map);
	if (retval != KERN_SUCCESS)
		panic("zone_init: kmem_suballoc failed");
	zone_max = zone_min + round_page(zone_map_size);

	/*
	 * Setup garbage collection information:
	 */

 	zone_table_size = atop(zone_max - zone_min) * 
		sizeof(struct zone_page_table_entry);
	if (kmem_alloc_wired(zone_map, (vm_offset_t *) &zone_page_table,
			     zone_table_size) != KERN_SUCCESS)
		panic("zone_init");
	zone_min = (vm_offset_t)zone_page_table + round_page(zone_table_size);
	zone_pages = atop(zone_max - zone_min);
	zone_map_min_address = zone_min;
	zone_map_max_address = zone_max;
	spinlock_init(&zone_page_table_lock);
	zone_page_init(zone_min, zone_max - zone_min, ZONE_PAGE_UNUSED);
}


/*
 *	zalloc returns an element from the specified zone.
 */
vm_offset_t
zalloc(
	register zone_t	zone)
{
	vm_offset_t	addr;

	assert (spinlock_count[cpu_number()] == 0);

	if (zone == ZONE_NULL)
		panic ("zalloc: null zone");

	lock_zone(zone);
	REMOVE_FROM_ZONE(zone, addr, vm_offset_t);
	while (addr == 0) {
		/*
 		 *	If nothing was there, try to get more
		 */
		if (zone->doing_alloc) {
			/*
			 *	Someone is allocating memory for this zone.
			 *	Wait for it to show up, then try again.
			 */
			assert_wait((int)&zone->doing_alloc, TRUE);
			/* XXX say wakeup needed */
			unlock_zone(zone);
			thread_block((void (*)(void)) 0);
			lock_zone(zone);
		}
		else {
			if ((zone->cur_size + zone->elem_size) >
			    zone->max_size) {
				if (zone->exhaustible)
					break;
				/*
				 * Printf calls logwakeup, which calls
				 * select_wakeup which will do a zfree
				 * (which tries to take the select_zone
				 * lock... Hang.  Release the lock now
				 * so it can be taken again later.
				 * NOTE: this used to be specific to
				 * the select_zone, but for
				 * cleanliness, we just unlock all
				 * zones before this.
				 */
				if (zone->expandable) {
					/*
					 * We're willing to overflow certain
					 * zones, but not without complaining.
					 *
					 * This is best used in conjunction
					 * with the collecatable flag. What we
					 * want is an assurance we can get the
					 * memory back, assuming there's no
					 * leak. 
					 */
					zone->max_size += (zone->max_size >> 1);
				} else if (!zone_ignore_overflow) {
					unlock_zone(zone);
					printf("zone \"%s\" empty.\n",
						zone->zone_name);
					panic("zalloc");
				}
			}

			unlock_zone(zone);

			if (zone->collectable) {
				if (kmem_alloc_wired(zone_map,
						     &addr, zone->alloc_size)
							!= KERN_SUCCESS)
					panic("zalloc: zone map full");
				zone_page_init(addr, zone->alloc_size,
							ZONE_PAGE_USED);
				zcram(zone, addr, zone->alloc_size);
				lock_zone(zone);
				REMOVE_FROM_ZONE(zone, addr, vm_offset_t);
			} else {
				addr = zget_space(zone->elem_size);
				if (addr == 0)
					panic("zalloc");

				lock_zone(zone);
				zone->count++;
				zone->cur_size += zone->elem_size;
#if	ZONE_DEBUG
				if (zone_debug_enabled(zone))
					enqueue_tail(&zone->active_zones,
						     (queue_entry_t)addr);
#endif	/* ZONE_DEBUG */
				unlock_zone(zone);
				zone_page_alloc(addr, zone->elem_size);
#if	ZONE_DEBUG
				if (zone_debug_enabled(zone))
					addr += sizeof(queue_chain_t);
#endif	/* ZONE_DEBUG */
				return(addr);
			}
		}
	}

#if	ZONE_DEBUG
	if (zone_debug_enabled(zone)) {
		enqueue_tail(&zone->active_zones, (queue_entry_t)addr);
		addr += sizeof(queue_chain_t);
	}
#endif	/* ZONE_DEBUG */
	unlock_zone(zone);
	return(addr);
}


/*
 *	zget returns an element from the specified zone
 *	and immediately returns nothing if there is nothing there.
 *
 *	This form should be used when you can not block (like when
 *	processing an interrupt).
 */
vm_offset_t
zget(
	register zone_t	zone)
{
	register vm_offset_t	addr;

	if (zone == ZONE_NULL)
		panic ("zalloc: null zone");

	if (!lock_try_zone(zone))
		return(0);
	REMOVE_FROM_ZONE(zone, addr, vm_offset_t);
#if	ZONE_DEBUG
	if (addr && zone_debug_enabled(zone)) {
		enqueue_tail(&zone->active_zones, (queue_entry_t)addr);
		addr += sizeof(queue_chain_t);
	}
#endif	/* ZONE_DEBUG */
	unlock_zone(zone);

	return(addr);
}

#if	ZONE_DEBUG
boolean_t zone_check = TRUE;
#else	/* ZONE_DEBUG */
boolean_t zone_check = FALSE;
#endif	/* ZONE_DEBUG */

void
zfree(
	register zone_t	zone,
	vm_offset_t	elem)
{
	lock_zone(zone);
#if	ZONE_DEBUG
	if (zone_debug_enabled(zone)) {
		queue_t tmp_elem;

		elem -= sizeof(queue_chain_t);
		if (zone_check) {
			/* check the zone's consistency */

			for (tmp_elem = queue_first(&zone->active_zones);
			     !queue_end(tmp_elem, &zone->active_zones);
			     tmp_elem = queue_next(tmp_elem))
				if (elem == (vm_offset_t)tmp_elem)
					break;
			if (elem != (vm_offset_t)tmp_elem)
				panic("zfree()ing element from wrong zone");
		}
		remqueue(&zone->active_zones, (queue_t) elem);
	}
#endif	/* ZONE_DEBUG */
	if (zone_check) {
		vm_offset_t this;

		/* check the zone's consistency */

		for (this = zone->free_elements;
		     this != 0;
		     this = * (vm_offset_t *) this)
			if (this <= VM_MIN_KERNEL_ADDRESS || this == elem)
				panic("zfree");
	}
	/*
	 * If elements have one or more pages, and memory is low,
	 * put it directly back into circulation rather than
	 * back into a zone, where a non-vm_privileged task can grab it.
	 * This lessens the impact of a privileged task cycling reserved
	 * memory into a publicly accessible zone.
	 */
	if (zone->elem_size >= PAGE_SIZE && vm_pool_low()){
		assert( !(zone->elem_size & (zone->alloc_size-1)) );
		zone->count--;
		zone->cur_size -= zone->elem_size;
		zone_page_init(elem, zone->elem_size, ZONE_PAGE_UNUSED);
		unlock_zone(zone);
		kmem_free(zone_map, elem, zone->elem_size);
		return;
	}
	ADD_TO_ZONE(zone, elem);
	unlock_zone(zone);
}

void
zcollectable(
	zone_t		zone)
{
	zone->collectable = TRUE;
}

void
zchange(
	zone_t		zone,
	boolean_t	sleepable,
	boolean_t	exhaustible,
	boolean_t	collectable)
{
	zone->sleepable = sleepable;
	zone->exhaustible = exhaustible;
	zone->collectable = collectable;
	lock_zone_init(zone);
}

/*
 *	zprealloc preallocates wired memory, exanding the specified
 *      zone to the specified size
 */
void
zprealloc(
	zone_t	zone,
	vm_size_t size)
{
        vm_offset_t addr;

	if (size != 0) {
		if (kmem_alloc_wired(zone_map, &addr, size) != KERN_SUCCESS)
		  panic("zprealloc");
		zone_page_init(addr, size, ZONE_PAGE_USED);
		zcram(zone, addr, size);
	}
}

/*
 *  Zone garbage collection subroutines
 *
 *  These routines have in common the modification of entries in the
 *  zone_page_table.  The latter contains one entry for every page
 *  in the zone_map.  
 *
 *  For each page table entry in the given range:
 *
 *	zone_page_in_use        - decrements in_free_list
 *	zone_page_free          - increments in_free_list
 *	zone_page_init          - initializes in_free_list and alloc_count
 *	zone_page_alloc         - increments alloc_count
 *	zone_page_dealloc       - decrements alloc_count
 *	zone_add_free_page_list - adds the page to the free list
 *   
 *  Two counts are maintained for each page, the in_free_list count and
 *  alloc_count.  The alloc_count is how many zone elements have been
 *  allocated from a page.  (Note that the page could contain elements
 *  that span page boundaries.  The count includes these elements so
 *  one element may be counted in two pages.) In_free_list is a count
 *  of how many zone elements are currently free.  If in_free_list is
 *  equal to alloc_count then the page is eligible for garbage
 *  collection.
 *
 *  Alloc_count and in_free_list are initialized to the correct values
 *  for a particular zone when a page is zcram'ed into a zone.  Subsequent
 *  gets and frees of zone elements will call zone_page_in_use and 
 *  zone_page_free which modify the in_free_list count.  When the zones
 *  garbage collector runs it will walk through a zones free element list,
 *  remove the elements that reside on collectable pages, and use 
 *  zone_add_free_page_list to create a list of pages to be collected.
 */

void
zone_page_in_use(
	vm_offset_t	addr,
	vm_size_t	size)
{
	int i, j;
	if ((addr < zone_map_min_address) ||
	    (addr+size > zone_map_max_address)) return;
	i = atop(addr-zone_map_min_address);
	j = atop((addr+size-1) - zone_map_min_address);
	lock_zone_page_table();
	for (; i <= j; i++) {
		zone_page_table[i].in_free_list--;
	}
	unlock_zone_page_table();
}

void
zone_page_free(
	vm_offset_t	addr,
	vm_size_t	size)
{
	int i, j;
	if ((addr < zone_map_min_address) ||
	    (addr+size > zone_map_max_address)) return;
	i = atop(addr-zone_map_min_address);
	j = atop((addr+size-1) - zone_map_min_address);
	lock_zone_page_table();
	for (; i <= j; i++) {
		/* Set in_free_list to (ZONE_PAGE_USED + 1) if
		 * it was previously set to ZONE_PAGE_UNUSED.
		 */
		if (zone_page_table[i].in_free_list == ZONE_PAGE_UNUSED) {
			zone_page_table[i].in_free_list = 1;
		} else {
			zone_page_table[i].in_free_list++;
		}
	}
	unlock_zone_page_table();
}

void
zone_page_init(
	vm_offset_t	addr,
	vm_size_t	size,
	int		value)
{
	int i, j;
	if ((addr < zone_map_min_address) ||
	    (addr+size > zone_map_max_address)) return;
	i = atop(addr-zone_map_min_address);
	j = atop((addr+size-1) - zone_map_min_address);
	lock_zone_page_table();
	for (; i <= j; i++) {
		zone_page_table[i].alloc_count = value;
		zone_page_table[i].in_free_list = 0;
	}
	unlock_zone_page_table();
}

void
zone_page_alloc(
	vm_offset_t	addr,
	vm_size_t	size)
{
	int i, j;
	if ((addr < zone_map_min_address) ||
	    (addr+size > zone_map_max_address)) return;
	i = atop(addr-zone_map_min_address);
	j = atop((addr+size-1) - zone_map_min_address);
	lock_zone_page_table();
	for (; i <= j; i++) {
		/* Set alloc_count to (ZONE_PAGE_USED + 1) if
		 * it was previously set to ZONE_PAGE_UNUSED.
		 */
		if (zone_page_table[i].alloc_count == ZONE_PAGE_UNUSED) {
			zone_page_table[i].alloc_count = 1;
		} else {
			zone_page_table[i].alloc_count++;
		}
	}
	unlock_zone_page_table();
}

void
zone_page_dealloc(
	vm_offset_t	addr,
	vm_size_t	size)
{
	int i, j;
	if ((addr < zone_map_min_address) ||
	    (addr+size > zone_map_max_address)) return;
	i = atop(addr-zone_map_min_address);
	j = atop((addr+size-1) - zone_map_min_address);
	lock_zone_page_table();
	for (; i <= j; i++) {
		zone_page_table[i].alloc_count--;
	}
	unlock_zone_page_table();
}

void
zone_add_free_page_list(
	struct zone_page_table_entry	**free_list,
	vm_offset_t	addr,
	vm_size_t	size)
{
	int i, j;
	if ((addr < zone_map_min_address) ||
	    (addr+size > zone_map_max_address)) return;
	i = atop(addr-zone_map_min_address);
	j = atop((addr+size-1) - zone_map_min_address);
	lock_zone_page_table();
	for (; i <= j; i++) {
		if (zone_page_table[i].alloc_count == 0) {
			zone_page_table[i].next = *free_list;
			*free_list = &zone_page_table[i];
			zone_page_table[i].alloc_count  = ZONE_PAGE_UNUSED;
			zone_page_table[i].in_free_list = 0;
		}
	}
	unlock_zone_page_table();
}


/* This is used for walking through a zone's free element list.
 */
struct zone_free_entry {
	struct zone_free_entry * next;
};


/*	Zone garbage collection
 *
 *	zone_gc will walk through all the free elements in all the
 *	zones that are marked collectable looking for reclaimable
 *	pages.  zone_gc is called by consider_zone_gc when the system
 *	begins to run out of memory.
 */
void
zone_gc(void)
{
	int		max_zones;
	zone_t		z;
	int		i;
	struct zone_page_table_entry	*freep;
	struct zone_page_table_entry	*zone_free_page_list;

	spinlock_lock(&all_zones_lock);
	max_zones = num_zones;
	z = first_zone;
	spinlock_unlock(&all_zones_lock);

	zone_free_page_list = (struct zone_page_table_entry *) 0;

	for (i = 0; i < max_zones; i++) {
		struct zone_free_entry * last;
		struct zone_free_entry * elt;
		assert(z != ZONE_NULL);
	/* run this at splhigh so that interupt routines that use zones
	   can not interupt while their zone is locked */
#if 0
		s = splhigh();
#endif/*  0 */
		lock_zone(z);

		if (z->collectable) {

		    /* Count the free elements in each page.  This loop
		     * requires that all in_free_list entries are zero.
		     */
		    elt = (struct zone_free_entry *)(z->free_elements);
		    while ((elt != (struct zone_free_entry *)0)) {
			   zone_page_free((vm_offset_t)elt, z->elem_size);
			   elt = elt->next;
		    }

		    /* Now determine which elements should be removed
		     * from the free list and, after all the elements
		     * on a page have been removed, add the element's
		     * page to a list of pages to be freed.
		     */
		    elt = (struct zone_free_entry *)(z->free_elements);
		    last = elt;
		    while ((elt != (struct zone_free_entry *)0)) {
			if (((vm_offset_t)elt>=zone_map_min_address)&&
			    ((vm_offset_t)elt<=zone_map_max_address)&&
			    (zone_page(elt)->in_free_list ==
			     zone_page(elt)->alloc_count)) {

			    z->cur_size -= z->elem_size;
			    zone_page_in_use((vm_offset_t)elt, z->elem_size);
			    zone_page_dealloc((vm_offset_t)elt, z->elem_size);
			    if (zone_page(elt)->alloc_count == 0 ||
			      zone_page(elt+(z->elem_size-1))->alloc_count==0) {
				    zone_add_free_page_list(
						    &zone_free_page_list, 
						    (vm_offset_t)elt, z->elem_size);
			    }


			    if (elt == last) {
				elt = elt->next;
				z->free_elements =(vm_offset_t)elt;
				last = elt;
			    } else {
				last->next = elt->next;
				elt = elt->next;
			    }
			} else {
			    /* This element is not eligible for collection
			     * so clear in_free_list in preparation for a
			     * subsequent garbage collection pass.
			     */
			    if (((vm_offset_t)elt>=zone_map_min_address)&&
				((vm_offset_t)elt<=zone_map_max_address)) {
				zone_page(elt)->in_free_list = 0;
			    }
			    last = elt;
			    elt = elt->next;
			}
		    }
		}
		unlock_zone(z);	
#if	0	
		splx(s);
#endif /*  0 */
		spinlock_lock(&all_zones_lock);
		z = z->next_zone;
		spinlock_unlock(&all_zones_lock);
	}

	for (freep = zone_free_page_list; freep != 0; freep = freep->next) {
		vm_offset_t	free_addr;

		free_addr = zone_map_min_address + 
			PAGE_SIZE * (freep - zone_page_table);
		kmem_free(zone_map, free_addr, PAGE_SIZE);
	}
}

boolean_t zone_gc_allowed = TRUE;	/* XXX */
unsigned zone_gc_last_tick = 0;
unsigned zone_gc_max_rate = 0;		/* in ticks */

/*
 *	consider_zone_gc:
 *
 *	Called by the pageout daemon when the system needs more free pages.
 */

void
consider_zone_gc(void)
{
	/*
	 *	By default, don't attempt zone GC more frequently
	 *	than once a second.
	 */

	if (zone_gc_max_rate == 0)
		zone_gc_max_rate = hz;

	if (zone_gc_allowed &&
	    (sched_tick > (zone_gc_last_tick + zone_gc_max_rate))) {
		zone_gc_last_tick = sched_tick;
		zone_gc();
	}
}

#include <mach_debug.h>
#if	MACH_DEBUG
#include <mach/kern_return.h>
#include <mach/machine/vm_types.h>
#include <mach_debug/zone_info.h>
#include <kern/host.h>
#include <vm/vm_map.h>
#include <vm/vm_user.h>
#include <vm/vm_kern.h>

#include <mach_debug/mach_debug_server.h>

kern_return_t
host_zone_info(
	host_t			host,
	zone_name_array_t	*namesp,
	unsigned int		*namesCntp,
	zone_info_array_t	*infop,
	unsigned int		*infoCntp)
{
	zone_name_t	*names;
	vm_offset_t	names_addr;
	vm_size_t	names_size;
	zone_info_t	*info;
	vm_offset_t	info_addr;
	vm_size_t	info_size;
	unsigned int	max_zones, i;
	zone_t		z;
	kern_return_t	kr;

	if (host == HOST_NULL)
		return KERN_INVALID_HOST;

	/*
	 *	We assume that zones aren't freed once allocated.
	 *	We won't pick up any zones that are allocated later.
	 */

	spinlock_lock(&all_zones_lock);
	max_zones = num_zones;
	z = first_zone;
	spinlock_unlock(&all_zones_lock);

	if (max_zones <= *namesCntp) {
		/* use in-line memory */

		names = *namesp;
	} else {
		names_size = round_page(max_zones * sizeof *names);
		kr = kmem_alloc_pageable(ipc_kernel_map,
					 &names_addr, names_size);
		if (kr != KERN_SUCCESS)
			return kr;

		names = (zone_name_t *) names_addr;
	}

	if (max_zones <= *infoCntp) {
		/* use in-line memory */

		info = *infop;
	} else {
		info_size = round_page(max_zones * sizeof *info);
		kr = kmem_alloc_pageable(ipc_kernel_map,
					 &info_addr, info_size);
		if (kr != KERN_SUCCESS) {
			if (names != *namesp)
				kmem_free(ipc_kernel_map,
					  names_addr, names_size);
			return kr;
		}

		info = (zone_info_t *) info_addr;
	}

	for (i = 0; i < max_zones; i++) {
		zone_name_t *zn = &names[i];
		zone_info_t *zi = &info[i];
		struct zone zcopy;

		assert(z != ZONE_NULL);

		lock_zone(z);
		zcopy = *z;
		unlock_zone(z);

		spinlock_lock(&all_zones_lock);
		z = z->next_zone;
		spinlock_unlock(&all_zones_lock);

		/* assuming here the name data is static */
		(void) strncpy(zn->zn_name, zcopy.zone_name,
			       sizeof zn->zn_name);

		zi->zi_count = zcopy.count;
		zi->zi_cur_size = zcopy.cur_size;
		zi->zi_max_size = zcopy.max_size;
		zi->zi_elem_size = zcopy.elem_size;
		zi->zi_alloc_size = zcopy.alloc_size;
		zi->zi_sleepable = zcopy.sleepable;
		zi->zi_exhaustible = zcopy.exhaustible;
		zi->zi_collectable = zcopy.collectable;
	}

	if (names != *namesp) {
		vm_size_t used;
		vm_map_copy_t copy;

		used = max_zones * sizeof *names;

		if (used != names_size)
			bzero((char *) (names_addr + used), names_size - used);

		kr = vm_map_copyin(ipc_kernel_map, names_addr, names_size,
				   TRUE, &copy);
		assert(kr == KERN_SUCCESS);

		*namesp = (zone_name_t *) copy;
	}
	*namesCntp = max_zones;

	if (info != *infop) {
		vm_size_t used;
		vm_map_copy_t copy;

		used = max_zones * sizeof *info;

		if (used != info_size)
			bzero((char *) (info_addr + used), info_size - used);

		kr = vm_map_copyin(ipc_kernel_map, info_addr, info_size,
				   TRUE, &copy);
		assert(kr == KERN_SUCCESS);

		*infop = (zone_info_t *) copy;
	}
	*infoCntp = max_zones;

	return KERN_SUCCESS;
}
#endif	/* MACH_DEBUG */

char *zone_labels = "ENTRY    COUNT  FREE TOTAL MAX_SIZE   ELT ALLOC NAME";

#if	MACH_KDB
#include <ddb/db_command.h>
#include <ddb/db_output.h>

/* Forwards */
void	db_print_zone(
		zone_t		addr);

void
db_print_zone(
	zone_t		addr)
{
	struct zone zcopy;

	zcopy = *addr;
	db_printf("%8x %5x %5x %6x %8x %5x %5x %s ",
		  addr,
		  zcopy.count,
		  zcopy.cur_size/zcopy.elem_size - zcopy.count,
		  zcopy.cur_size,
		  zcopy.max_size,
		  zcopy.elem_size,
		  zcopy.alloc_size,
		  zcopy.zone_name);
	if (zcopy.sleepable)
	  	db_printf("S");
	if (zcopy.exhaustible)
	  	db_printf("H");
	if (zcopy.collectable)
	  	db_printf("C");
	if (zcopy.expandable)
	  	db_printf("X");
	db_printf("\n");
}

/*ARGSUSED*/
void
db_show_one_zone(
        db_expr_t       addr,
        int		have_addr,
        db_expr_t	count,
        char *          modif)
{
	struct zone *z = (zone_t)addr;

	if (z == ZONE_NULL || !have_addr){
		db_error("No Zone\n");
		/*NOTREACHED*/
	}

	db_printf("%s\n", zone_labels);
	db_print_zone(z);
}

/*ARGSUSED*/
void
db_show_all_zones(
        db_expr_t	addr,
        int		have_addr,
        db_expr_t	count,
        char *		modif)
{
	zone_t		z;
	unsigned total = 0;

	/*
	 * Don't risk hanging by unconditionally locking,
	 * risk of incoherent data is small (zones aren't freed).
	 */
	have_addr = spinlock_try(&all_zones_lock);
	count = num_zones;
	z = first_zone;
	if (have_addr) {
		spinlock_unlock(&all_zones_lock);
	}

	db_printf("%s\n", zone_labels);
	for (  ; count > 0; count--) {
		if (!z) {
			db_error("Mangled Zone List\n");
			/*NOTREACHED*/
		}
		db_print_zone(z);
		total += z->cur_size,

		have_addr = spinlock_try(&all_zones_lock);
		z = z->next_zone;
		if (have_addr) {
			spinlock_unlock(&all_zones_lock);
		}
	}
	db_printf("\nTotal              %8x", total);
}

#endif /* MACH_KDB */

#if	ZONE_DEBUG

/* should we care about locks here ? */

#if	MACH_KDB
vm_offset_t
next_element(
	zone_t		z,
	vm_offset_t	elt)
{
	if (!zone_debug_enabled(z))
		return(0);
	elt -= sizeof(queue_chain_t);
	elt = (vm_offset_t) queue_next((queue_t) elt);
	if ((queue_t) elt == &z->active_zones)
		return(0);
	elt += sizeof(queue_chain_t);
	return(elt);
}

vm_offset_t
first_element(
	zone_t		z)
{
	vm_offset_t	elt;

	if (!zone_debug_enabled(z))
		return(0);
	if (queue_empty(&z->active_zones))
		return(0);
	elt = (vm_offset_t) queue_first(&z->active_zones);
	elt += sizeof(queue_chain_t);
	return(elt);
}

int
zone_count(
	zone_t		z,
	unsigned	all)
{
	vm_offset_t	elt;
	int		count = 0;

	for (elt = first_element(z); elt; elt = next_element(z, elt)) {
		if (all)
			db_printf("%8x\n", elt);
		count++;
	}
	return(count);
}
#endif /* MACH_KDB */

#define zone_in_use(z) 	( z->count || z->free_elements )

void
zone_debug_enable(
	zone_t		z)
{
	if (zone_debug_enabled(z) || zone_in_use(z) ||
	    z->alloc_size < (z->elem_size + sizeof(queue_chain_t)))
		return;
	queue_init(&z->active_zones);
	z->elem_size += sizeof(queue_chain_t);
}

void
zone_debug_disable(
	zone_t		z)
{
	if (!zone_debug_enabled(z) || zone_in_use(z))
		return;
	z->elem_size -= sizeof(queue_chain_t);
	z->active_zones.next = z->active_zones.prev = 0;	
}
#endif	/* ZONE_DEBUG */
