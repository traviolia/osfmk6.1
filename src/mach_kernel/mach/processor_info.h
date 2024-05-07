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
 * Revision 2.4  91/05/14  16:58:46  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:35:31  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:20:39  mrt]
 * 
 * Revision 2.2  90/06/02  14:59:49  rpd
 * 	Created for new host/processor technology.
 * 	[90/03/26  23:51:38  rpd]
 * 
 * 	Merge to X96
 * 	[89/08/02  23:12:21  dlb]
 * 
 * 	Add scheduling flavor of information.
 * 	[89/07/25  18:52:18  dlb]
 * 
 * 	Add load average and mach factor to processor set basic info.
 * 	[89/02/09            dlb]
 * 
 * Revision 2.3  89/10/15  02:05:54  rpd
 * 	Minor cleanups.
 * 
 * Revision 2.2  89/10/11  14:41:03  dlb
 * 	Add scheduling flavor of information.
 * 
 * 	Add load average and mach factor to processor set basic info.
 * 	[89/02/09            dlb]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 *	File:	mach/processor_info.h
 *	Author:	David L. Black
 *	Date:	1988
 *
 *	Data structure definitions for processor_info, processor_set_info
 */

#ifndef	_MACH_PROCESSOR_INFO_H_
#define _MACH_PROCESSOR_INFO_H_

#include <mach/machine.h>

/*
 *	Generic information structure to allow for expansion.
 */
typedef int	*processor_info_t;	/* varying array of int. */

#define PROCESSOR_INFO_MAX	(1024)	/* max array size */
typedef int	processor_info_data_t[PROCESSOR_INFO_MAX];


typedef int	*processor_set_info_t;	/* varying array of int. */

#define PROCESSOR_SET_INFO_MAX	(1024)	/* max array size */
typedef int	processor_set_info_data_t[PROCESSOR_SET_INFO_MAX];


typedef int	*processor_slot_t;	/* varying array of int. */

/*
 *	Currently defined information.
 */
typedef int	processor_flavor_t;
#define	PROCESSOR_BASIC_INFO	1		/* basic information */

struct processor_basic_info {
	cpu_type_t	cpu_type;	/* type of cpu */
	cpu_subtype_t	cpu_subtype;	/* subtype of cpu */
	boolean_t	running;	/* is processor running */
	int		slot_num;	/* slot number */
	boolean_t	is_master;	/* is this the master processor */
};

typedef	struct processor_basic_info	processor_basic_info_data_t;
typedef struct processor_basic_info	*processor_basic_info_t;
#define PROCESSOR_BASIC_INFO_COUNT \
		(sizeof(processor_basic_info_data_t)/sizeof(int))

/*
 *	Scaling factor for load_average, mach_factor.
 */
#define	LOAD_SCALE	1000		

typedef int	processor_set_flavor_t;
#define	PROCESSOR_SET_BASIC_INFO	5	/* basic information */

struct processor_set_basic_info {
	int		processor_count;	/* How many processors */
	int		default_policy;		/* When others not enabled */
};

typedef	struct processor_set_basic_info	processor_set_basic_info_data_t;
typedef struct processor_set_basic_info	*processor_set_basic_info_t;
#define PROCESSOR_SET_BASIC_INFO_COUNT \
		(sizeof(processor_set_basic_info_data_t)/sizeof(int))


#define PROCESSOR_SET_LOAD_INFO		4	/* scheduling statistics */

struct processor_set_load_info {
        int             task_count;             /* How many tasks */
        int             thread_count;           /* How many threads */
        int             load_average;           /* Scaled */
        int             mach_factor;            /* Scaled */
};

typedef struct processor_set_load_info processor_set_load_info_data_t;
typedef struct processor_set_load_info *processor_set_load_info_t;
#define PROCESSOR_SET_LOAD_INFO_COUNT	\
                (sizeof(processor_set_load_info_data_t)/sizeof(int))


/*
 *      New scheduling control interface
 */
#define PROCESSOR_SET_ENABLED_POLICIES                   3

#define PROCESSOR_SET_TIMESHARE_DEFAULT                 10
#define PROCESSOR_SET_TIMESHARE_LIMITS                  11

#define PROCESSOR_SET_RR_DEFAULT                        20
#define PROCESSOR_SET_RR_LIMITS                         21

#define PROCESSOR_SET_FIFO_DEFAULT                      30
#define PROCESSOR_SET_FIFO_LIMITS                       31

#endif	/* _MACH_PROCESSOR_INFO_H_ */
