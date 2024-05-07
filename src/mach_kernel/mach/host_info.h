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
 * Revision 2.4.2.1  92/01/09  18:44:17  jsb
 * 	From durriya@ri.osf.org: defined kernel_boot_info_t.
 * 	[92/01/08  15:01:53  jsb]
 * 
 * Revision 2.4  91/05/14  16:51:48  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:31:58  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:17:13  mrt]
 * 
 * Revision 2.2  90/06/02  14:57:58  rpd
 * 	Added HOST_LOAD_INFO and related definitions.
 * 	[90/04/27            rpd]
 * 	Created for new host/processor technology.
 * 	[90/03/26  23:50:51  rpd]
 * 
 * 	Cleanup changes.
 * 	[89/08/02            dlb]
 * 	Add sched_info flavor to return minimum times for use by
 * 	external schedulers.
 * 	[89/06/08            dlb]
 * 	Added kernel_version type definitions.
 * 	[88/12/02            dlb]
 * 
 * Revision 2.4  89/10/15  02:05:31  rpd
 * 	Minor cleanups.
 * 
 * Revision 2.3  89/10/11  17:32:15  dlb
 * 	Include mach/machine/vm_types.h instead of mach/vm_param.h
 * 	[89/10/11            dlb]
 * 
 * Revision 2.2  89/10/11  14:36:55  dlb
 * 	Add sched_info flavor to return minimum times for use by
 * 	external schedulers.
 * 	[89/06/08            dlb]
 * 
 * 	Added kernel_version type definitions.
 * 	[88/12/02            dlb]
 * 
 * 30-Nov-88  David Black (dlb) at Carnegie-Mellon University
 *	Created.  2 flavors so far: basic info,  slot numbers.
 *
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
 *	File:	mach/host_info.h
 *
 *	Definitions for host_info call.
 */

#ifndef	_MACH_HOST_INFO_H_
#define	_MACH_HOST_INFO_H_

#include <mach/vm_statistics.h>
#include <mach/machine.h>
#include <mach/machine/vm_types.h>

/*
 *	Generic information structure to allow for expansion.
 */
typedef int	*host_info_t;		/* varying array of int. */

#define	HOST_INFO_MAX	(1024)		/* max array size */
typedef int	host_info_data_t[HOST_INFO_MAX];

#define KERNEL_VERSION_MAX (512)
typedef char	kernel_version_t[KERNEL_VERSION_MAX];

#define KERNEL_BOOT_INFO_MAX (4096)
typedef char	kernel_boot_info_t[KERNEL_BOOT_INFO_MAX];

/*
 *	Currently defined information.
 */
/* host_info() */
typedef	int		host_flavor_t;
#define HOST_BASIC_INFO		1	/* basic info */
#define HOST_SCHED_INFO		3	/* scheduling info */
#define HOST_RESOURCE_SIZES	4	/* kernel struct sizes */
#define HOST_PRIORITY_INFO	5	/* priority information */

struct host_basic_info {
	int		max_cpus;	/* max number of cpus possible */
	int		avail_cpus;	/* number of cpus now available */
	vm_size_t	memory_size;	/* size of memory in bytes */
	cpu_type_t	cpu_type;	/* cpu type */
	cpu_subtype_t	cpu_subtype;	/* cpu subtype */
};

typedef	struct host_basic_info	host_basic_info_data_t;
typedef struct host_basic_info	*host_basic_info_t;
#define HOST_BASIC_INFO_COUNT \
		(sizeof(host_basic_info_data_t)/sizeof(int))

struct host_sched_info {
	int		min_timeout;	/* minimum timeout in milliseconds */
	int		min_quantum;	/* minimum quantum in milliseconds */
};

typedef	struct host_sched_info	host_sched_info_data_t;
typedef struct host_sched_info	*host_sched_info_t;
#define HOST_SCHED_INFO_COUNT \
		(sizeof(host_sched_info_data_t)/sizeof(int))

struct kernel_resource_sizes {
	vm_size_t	task;
        vm_size_t	thread;
        vm_size_t	port;
        vm_size_t	memory_region;
        vm_size_t	memory_object;
};

typedef struct kernel_resource_sizes	kernel_resource_sizes_data_t;
typedef struct kernel_resource_sizes	*kernel_resource_sizes_t;
#define HOST_RESOURCE_SIZES_COUNT \
		(sizeof(kernel_resource_sizes_data_t)/sizeof(int))

struct host_priority_info {
    	int		kernel_priority;
    	int		system_priority;
    	int		server_priority;
    	int		user_priority;
    	int		depress_priority;
    	int		idle_priority;
    	int		minimum_priority;
	int		maximum_priority;
};

typedef struct host_priority_info	host_priority_info_data_t;
typedef struct host_priority_info	*host_priority_info_t;
#define HOST_PRIORITY_INFO_COUNT \
		(sizeof(host_priority_info_data_t)/sizeof(int))

/* host_statistics() */
#define	HOST_LOAD_INFO		1	/* System loading stats */
#define HOST_VM_INFO		2	/* Virtual memory stats */

struct host_load_info {
	long		avenrun[3];	/* scaled by LOAD_SCALE */
	long		mach_factor[3];	/* scaled by LOAD_SCALE */
};

typedef struct host_load_info	host_load_info_data_t;
typedef struct host_load_info	*host_load_info_t;
#define	HOST_LOAD_INFO_COUNT \
		(sizeof(host_load_info_data_t)/sizeof(int))

/* in <mach/vm_statistics.h> */
#define	HOST_VM_INFO_COUNT \
		(sizeof(vm_statistics_data_t)/sizeof(int))

#endif	/* _MACH_HOST_INFO_H_ */
