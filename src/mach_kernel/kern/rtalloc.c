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
 * Revision 2.9  91/05/14  16:43:17  mrt
 * 	Correcting copyright
 * 
 * Revision 2.8  91/03/16  14:50:37  rpd
 * 	Updated for new kmem_alloc interface.
 * 	[91/03/03            rpd]
 * 
 * Revision 2.7  91/02/05  17:27:22  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:14:12  mrt]
 * 
 * Revision 2.6  90/06/19  22:59:06  rpd
 * 	Made the big kalloc zones collectable.
 * 	[90/06/05            rpd]
 * 
 * Revision 2.5  90/06/02  14:54:47  rpd
 * 	Added kalloc_max, kalloc_map_size.
 * 	[90/03/26  22:06:39  rpd]
 * 
 * Revision 2.4  90/01/11  11:43:13  dbg
 * 	De-lint.
 * 	[89/12/06            dbg]
 * 
 * Revision 2.3  89/09/08  11:25:51  dbg
 * 	MACH_KERNEL: remove non-MACH data types.
 * 	[89/07/11            dbg]
 * 
 * Revision 2.2  89/08/31  16:18:59  rwd
 * 	First Checkin
 * 	[89/08/23  15:41:37  rwd]
 * 
 * Revision 2.6  89/08/02  08:03:28  jsb
 * 	Make all kalloc zones 8 MB big. (No more kalloc panics!)
 * 	[89/08/01  14:10:17  jsb]
 * 
 * Revision 2.4  89/04/05  13:03:10  rvb
 * 	Guarantee a zone max of at least 100 elements or 10 pages
 * 	which ever is greater.  Afs (AllocDouble()) puts a great demand
 * 	on the 2048 zone and used to blow away.
 * 	[89/03/09            rvb]
 * 
 * Revision 2.3  89/02/25  18:04:39  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 2.2  89/01/18  02:07:04  jsb
 * 	Give each kalloc zone a meaningful name (for panics);
 * 	create a zone for each power of 2 between MINSIZE
 * 	and PAGE_SIZE, instead of using (obsoleted) NQUEUES.
 * 	[89/01/17  10:16:33  jsb]
 * 
 *
 * 13-Feb-88  John Seamons (jks) at NeXT
 *	Updated to use kmem routines instead of vmem routines.
 *
 * 21-Jun-85  Avadis Tevanian (avie) at Carnegie-Mellon University
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
 *	File:	kern/rtalloc.c
 *	Author:	Ron Widyono
 *	Date:	1994
 *
 *	General real-time kernel memory allocator.  This allocator is designed
 *	to be used by the kernel to manage dynamic memory fast, but exclusively
 *      for real-time purposes.  In fact, only real-time IPC users compete for
 *      these resources.
 *
 *      This is a dirty implementation--a clone of kalloc.  A cleaner imple-
 *      mentation is to have both k_zones and rt_zones use common code.
 */

#include <mach/boolean.h>
#include <mach/machine/vm_types.h>
#include <mach/vm_param.h>
#include <kern/misc_protos.h>
#include <kern/zalloc.h>
#include <kern/rtalloc.h>
#include <vm/vm_kern.h>
#include <vm/vm_object.h>
#include <vm/vm_map.h>

extern void	Debugger(const char *);

vm_map_t rtalloc_map;
vm_size_t rtalloc_map_size = 8 * 1024 * 1024;
vm_size_t rtalloc_max;
vm_size_t rtalloc_max_prerounded;

/*
 *	All allocations of size less than rtalloc_max are rounded to the
 *	next highest power of 2.  This allocator is built on top of
 *	the zone allocator.  A zone is created for each potential size
 *	that we are willing to get in small blocks.
 *
 *	We assume that rtalloc_max is not greater than 64K;
 *	thus 16 is a safe array size for rt_zone and rt_zone_name.
 *
 *	Note that rtalloc_max is somewhat confusingly named.
 *	It represents the first power of two for which no zone exists.
 *	rtalloc_max_prerounded is the smallest allocation size, before
 *	rounding, for which no zone exists.
 */

int first_rt_zone = -1;
struct zone *rt_zone[16];
static char *rt_zone_name[16] = {
	"rtalloc.1",		"rtalloc.2",
	"rtalloc.4",		"rtalloc.8",
	"rtalloc.16",		"rtalloc.32",
	"rtalloc.64",		"rtalloc.128",
	"rtalloc.256",		"rtalloc.512",
	"rtalloc.1024",		"rtalloc.2048",
	"rtalloc.4096",		"rtalloc.8192",
	"rtalloc.16384",	"rtalloc.32768"
};

/*
 *  Max number of elements per zone.  zinit rounds things up correctly
 *  Doing things this way permits each zone to have a different maximum size
 *  based on need, rather than just guessing; it also
 *  means its patchable in case you're wrong!
 */
unsigned long rt_zone_max[16] = {
      RT_ZONE_MAX_1,	
      RT_ZONE_MAX_2,	
      RT_ZONE_MAX_4,	
      RT_ZONE_MAX_8,	
      RT_ZONE_MAX_16,	
      RT_ZONE_MAX_32,	
      RT_ZONE_MAX_64,	
      RT_ZONE_MAX_128,	
      RT_ZONE_MAX_256,	
      RT_ZONE_MAX_512,	
      RT_ZONE_MAX_1024,	
      RT_ZONE_MAX_2048,	
      RT_ZONE_MAX_4096,	
      RT_ZONE_MAX_8192,	
      RT_ZONE_MAX_16384,
      RT_ZONE_MAX_32768
};

/*
 *	Initialize the memory allocator.  This should be called only
 *	once on a system wide basis (i.e. first processor to get here
 *	does the initialization).
 *
 *	This initializes all of the zones.
 */

void
rtalloc_init(
	void)
{
	kern_return_t retval;
	vm_offset_t min, addr;
	vm_size_t size;
	register int i;

	retval = kmem_suballoc(kernel_map, &min, rtalloc_map_size,
			       FALSE, TRUE, &rtalloc_map);
	if (retval != KERN_SUCCESS)
		panic("rtalloc_init: kmem_suballoc failed");

	/*
	 *	Ensure that zones up to size 8192 bytes exist.
	 *	This is desirable because messages are allocated
	 *	with rtalloc, and messages up through size 8192 are common.
	 */

	rtalloc_max = 16 * 1024;
	rtalloc_max_prerounded = rtalloc_max / 2 + 1;

	/*
	 *	Allocate a zone for each size we are going to handle.
	 *	We specify non-paged memory.  Make zone exhaustible.
	 */
	for (i = 0, size = 1; size < rtalloc_max; i++, size <<= 1) {
		if (size < RTALLOC_MINSIZE) {
			rt_zone[i] = 0;
			continue;
		}
		if (size == RTALLOC_MINSIZE) {
			first_rt_zone = i;
		}
		rt_zone[i] = zinit(size, rt_zone_max[i] * size, size,
				  rt_zone_name[i]);

		/*
		 * This call should be removed once zone_change()
		 * is implemented
		 */
		zchange(rt_zone[i], FALSE, TRUE, FALSE);
		
		/*
		 * This code is for use after zone_change() is implemented
		 */
/*
		zone_change(rt_zone[i], Z_EXHAUST, TRUE);
		zone_change(rt_zone[i], Z_COLLECT, FALSE);
		zone_change(rt_zone[i], Z_EXPAND, FALSE);
*/

		/*
		 * Get space from the zone_map.  Since these zones are
		 * not collectable, no pages containing elements from these
		 * zones will ever be reclaimed by the garbage collection
		 * scheme below.
		 */

		zprealloc(rt_zone[i], rt_zone_max[i] * size);
	}
}

vm_offset_t
rtalloc(
	vm_size_t	size)
{
	register int zindex;
	register vm_size_t allocsize;
	register vm_offset_t ret;

	/*
	 * If size is too large for a zone, then use kmem_alloc.
	 * (We use kmem_alloc instead of kmem_alloc_wired so that
	 * rtrealloc can use kmem_realloc.)
	 */

	if (size >= rtalloc_max_prerounded) {
		vm_offset_t addr;

		printf("rtalloc: size requested %d is greater than rtalloc_max\n", size);

		if (kmem_alloc(rtalloc_map, &addr, size) != KERN_SUCCESS)
			addr = 0;
		return(addr);
	}

	/* compute the size of the block that we will actually allocate */

	allocsize = RTALLOC_MINSIZE;
	zindex = first_rt_zone;
	while (allocsize < size) {
		allocsize <<= 1;
		zindex++;
	}

	/* allocate from the appropriate zone */

	assert(allocsize < rtalloc_max);
	if ((ret = zalloc(rt_zone[zindex])) == 0)
		printf("rtalloc: zone %s exhausted\n", rt_zone_name[zindex]);
	return(ret);
}

void
rtrealloc(
	vm_offset_t	*addrp,
	vm_size_t	old_size,
	vm_size_t	new_size)
{
	register int zindex;
	register vm_size_t allocsize;
	vm_offset_t old_addr;

	/* can only be used for increasing allocation size */

	assert(new_size > old_size);

	/* if old_size is zero, then we are simply allocating */

	if (old_size == 0) {
		*addrp = rtalloc(new_size);
		return;
	}

	/* if old block was kmem_alloc'd, then use kmem_realloc if necessary */

	if (old_size >= rtalloc_max_prerounded) {
		old_addr = *addrp;
		old_size = round_page(old_size);
		new_size = round_page(new_size);
		if (new_size > old_size) {
			if (kmem_realloc(rtalloc_map, old_addr, old_size, addrp,
					 new_size) != KERN_SUCCESS) {
				panic("rtrealloc: kmem_realloc");
				*addrp = 0;
			}
		}
		return;
	}

	/* compute the size of the block that we actually allocated */

	allocsize = RTALLOC_MINSIZE;
	zindex = first_rt_zone;
	while (allocsize < old_size) {
		allocsize <<= 1;
		zindex++;
	}

	/* if new size fits in old block, then return */

	if (new_size <= allocsize) {
		return;
	}

	/* if new size does not fit in zone, kmem_alloc it, else zalloc it */

	old_addr = *addrp;
	if (new_size >= rtalloc_max_prerounded) {
		if (kmem_alloc(rtalloc_map, addrp, new_size) != KERN_SUCCESS) {
			panic("rtrealloc: kmem_alloc");
			*addrp = 0;
			return;
		}
	} else {
		register int new_zindex;

		allocsize <<= 1;
		new_zindex = zindex + 1;
		while (allocsize < new_size) {
			allocsize <<= 1;
			new_zindex++;
		}
		*addrp = zalloc(rt_zone[new_zindex]);
	}

	/* copy existing data */

	bcopy((char *)old_addr, (char *)*addrp, old_size);

	/* free old block, and return */

	zfree(rt_zone[zindex], old_addr);
}

vm_offset_t
rtget(
	vm_size_t	size)
{
	register int zindex;
	register vm_size_t allocsize;

	/* size must not be too large for a zone */

	if (size >= rtalloc_max_prerounded) {
		/* This will never work, so we might as well panic */
		panic("rtget");
	}

	/* compute the size of the block that we will actually allocate */

	allocsize = RTALLOC_MINSIZE;
	zindex = first_rt_zone;
	while (allocsize < size) {
		allocsize <<= 1;
		zindex++;
	}

	/* allocate from the appropriate zone */

	assert(allocsize < rtalloc_max);
	return(zget(rt_zone[zindex]));
}

void
rtfree(
	vm_offset_t	data,
	vm_size_t	size)
{
	register int zindex;
	register vm_size_t freesize;

	/* if size was too large for a zone, then use kmem_free */

	if (size >= rtalloc_max_prerounded) {
		kmem_free(rtalloc_map, data, size);
		return;
	}

	/* compute the size of the block that we actually allocated from */

	freesize = RTALLOC_MINSIZE;
	zindex = first_rt_zone;
	while (freesize < size) {
		freesize <<= 1;
		zindex++;
	}

	/* free to the appropriate zone */

	if (rt_zone[zindex]->count == 0) {
	    printf("Assertion failed: file \"%s\", line %d: %s\n", __FILE__,
		   __LINE__, "rt_zone[zindex]->count != 0");
	    Debugger("assertion failure");
	}
	assert(freesize < rtalloc_max);
	zfree(rt_zone[zindex], data);
}
