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
/*
 *	File:	mach/vm_region.h
 *
 *	Define the attributes of a task's memory region
 *
 */

#ifndef	_MACH_VM_REGION_H_
#define _MACH_VM_REGION_H_

#include <mach/boolean.h>
#include <mach/vm_prot.h>
#include <mach/vm_inherit.h>
#include <mach/vm_behavior.h>

/*
 *	Types defined:
 *
 *	vm_region_info_t	memory region attributes
 */

#define VM_REGION_INFO_MAX      (1024)
typedef int	*vm_region_info_t;
typedef int	 vm_region_flavor_t;
typedef int	 vm_region_info_data_t[VM_REGION_INFO_MAX];

#define VM_REGION_BASIC_INFO	10

struct vm_region_basic_info {
	vm_prot_t		protection;
	vm_prot_t		max_protection;
	vm_inherit_t		inheritance;
	boolean_t		shared;
	boolean_t		reserved;
	vm_offset_t		offset;
	vm_behavior_t		behavior;
	unsigned short		user_wired_count;
};

typedef struct vm_region_basic_info		*vm_region_basic_info_t;
typedef struct vm_region_basic_info		 vm_region_basic_info_data_t;

#define VM_REGION_BASIC_INFO_COUNT		\
	(sizeof(vm_region_basic_info_data_t)/sizeof(int))

#endif	/*_MACH_VM_REGION_H_*/
