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
 * Revision 2.6.2.1  92/02/21  11:24:08  jsb
 * 	Added MEMORY_OBJECT_COPY_TEMPORARY.
 * 	[92/02/11  07:56:35  jsb]
 * 
 * Revision 2.6  91/08/28  11:15:22  jsb
 * 	Add defs for memory_object_return_t.
 * 	[91/07/03  14:06:26  dlb]
 * 
 * Revision 2.5  91/05/18  14:35:05  rpd
 * 	Removed memory_manager_default.
 * 	[91/03/22            rpd]
 * 
 * Revision 2.4  91/05/14  16:55:55  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:34:01  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:19:11  mrt]
 * 
 * Revision 2.2  90/06/02  14:58:56  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  22:35:26  rpd]
 * 
 * Revision 2.1  89/08/03  16:02:52  rwd
 * Created.
 * 
 * Revision 2.5  89/02/25  18:38:23  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 2.4  89/02/07  00:54:07  mwyoung
 * Relocated from vm/memory_object.h
 * 
 * Revision 2.3  89/01/30  22:08:42  rpd
 * 	Updated includes to the new style.  Fixed log.
 * 	Made variable declarations use "extern".
 * 	[89/01/25  15:25:20  rpd]
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988 Carnegie Mellon University
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
 *	File:	memory_object.h
 *	Author:	Michael Wayne Young
 *
 *	External memory management interface definition.
 */

#ifndef	_MACH_MEMORY_OBJECT_H_
#define _MACH_MEMORY_OBJECT_H_

/*
 *	User-visible types used in the external memory
 *	management interface:
 */

#include <mach/port.h>
#include <mach/machine/vm_types.h>

typedef	mach_port_t	memory_object_t;
					/* A memory object ... */
					/*  Used by the kernel to retrieve */
					/*  or store data */

typedef	mach_port_t	memory_object_control_t;
					/* Provided to a memory manager; ... */
					/*  used to control a memory object */

typedef	mach_port_t	memory_object_name_t;
					/* Used to describe the memory ... */
					/*  object in vm_regions() calls */

typedef mach_port_t     memory_object_rep_t;
					/* Per-client handle for mem object */
					/*  Used by user programs to specify */
					/*  the object to map */

typedef	int		memory_object_copy_strategy_t;
					/* How memory manager handles copy: */
#define		MEMORY_OBJECT_COPY_NONE		0
					/* ... No special support */
#define		MEMORY_OBJECT_COPY_CALL		1
					/* ... Make call on memory manager */
#define		MEMORY_OBJECT_COPY_DELAY 	2
					/* ... Memory manager doesn't
					 *     change data externally.
					 */
#define		MEMORY_OBJECT_COPY_TEMPORARY 	3
					/* ... Memory manager doesn't
					 *     change data externally, and
					 *     doesn't need to see changes.
					 */
#define		MEMORY_OBJECT_COPY_SYMMETRIC 	4
					/* ... Memory manager doesn't
					 *     change data externally,
					 *     doesn't need to see changes,
					 *     and object will not be
					 *     multiply mapped.
					 *
					 *     XXX
					 *     Not yet safe for non-kernel use.
					 */

#define		MEMORY_OBJECT_COPY_INVALID	5
					/* ...	An invalid copy strategy,
					 *	for external objects which
					 *	have not been initialized.
					 *	Allows copy_strategy to be
					 *	examined without also
					 *	examining pager_ready and
					 *	internal.
					 */

typedef	int		memory_object_return_t;
					/* Which pages to return to manager
					   this time (lock_request) */
#define		MEMORY_OBJECT_RETURN_NONE	0
					/* ... don't return any. */
#define		MEMORY_OBJECT_RETURN_DIRTY	1
					/* ... only dirty pages. */
#define		MEMORY_OBJECT_RETURN_ALL	2
					/* ... dirty and precious pages. */
#define		MEMORY_OBJECT_RETURN_ANYTHING	3
					/* ... any resident page. */

#define		MEMORY_OBJECT_NULL	MACH_PORT_NULL


/*
 *	Types for the memory object flavor interfaces
 */

#define MEMORY_OBJECT_INFO_MAX      (1024) 
typedef int     *memory_object_info_t;      
typedef int	 memory_object_flavor_t;
typedef int      memory_object_info_data_t[MEMORY_OBJECT_INFO_MAX];


#define MEMORY_OBJECT_BEHAVIOR_INFO 	10	
#define MEMORY_OBJECT_PERFORMANCE_INFO	11
#define MEMORY_OBJECT_ATTRIBUTE_INFO	12
#define MEMORY_OBJECT_NORMA_INFO	13


struct memory_object_behave_info {
	memory_object_copy_strategy_t	copy_strategy;	
	boolean_t			temporary;
	boolean_t			invalidate;
	boolean_t			write_completions;
};

struct memory_object_perf_info {
	vm_size_t			cluster_size;
	boolean_t			may_cache;
};

struct memory_object_attr_info {			/* old attr list */
        boolean_t       		object_ready;
        boolean_t       		may_cache;
        memory_object_copy_strategy_t 	copy_strategy;
};

struct memory_object_norma_info {			/* old norma list */
        boolean_t       		object_ready;
        boolean_t       		may_cache;
	boolean_t			write_completions;
        memory_object_copy_strategy_t 	copy_strategy;
	vm_size_t			cluster_size;
};


typedef struct memory_object_behave_info *memory_object_behave_info_t;
typedef struct memory_object_behave_info memory_object_behave_info_data_t;

typedef struct memory_object_perf_info 	*memory_object_perf_info_t;
typedef struct memory_object_perf_info	memory_object_perf_info_data_t;

typedef struct memory_object_attr_info	*memory_object_attr_info_t;
typedef struct memory_object_attr_info	memory_object_attr_info_data_t;

typedef struct memory_object_norma_info	*memory_object_norma_info_t;
typedef struct memory_object_norma_info	memory_object_norma_info_data_t;


#define MEMORY_OBJECT_BEHAVE_INFO_COUNT   	\
                (sizeof(memory_object_behave_info_data_t)/sizeof(int))
#define MEMORY_OBJECT_PERF_INFO_COUNT		\
		(sizeof(memory_object_perf_info_data_t)/sizeof(int))
#define MEMORY_OBJECT_ATTR_INFO_COUNT		\
		(sizeof(memory_object_attr_info_data_t)/sizeof(int))
#define MEMORY_OBJECT_NORMA_INFO_COUNT		\
		(sizeof(memory_object_norma_info_data_t)/sizeof(int))

#define invalid_memory_object_flavor(f)					\
	(f != MEMORY_OBJECT_NORMA_INFO && 				\
	 f != MEMORY_OBJECT_ATTRIBUTE_INFO && 				\
	 f != MEMORY_OBJECT_PERFORMANCE_INFO && 			\
	 f != MEMORY_OBJECT_BEHAVIOR_INFO)


#endif	/* _MACH_MEMORY_OBJECT_H_ */
