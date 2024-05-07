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
 * Revision 2.4  91/05/14  17:02:37  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:37:24  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:22:17  mrt]
 * 
 * Revision 2.2  90/01/22  23:05:53  af
 * 	Created.
 * 	[89/12/08            af]
 * 
 *
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
 *	File:	mach/vm_attributes.h
 *	Author:	Alessandro Forin
 *
 *	Virtual memory attributes definitions.
 *
 *	These definitions are in addition to the machine-independent
 *	ones (e.g. protection), and are only selectively supported
 *	on specific machine architectures.
 *
 */

#ifndef	VM_ATTRIBUTES_H_
#define	VM_ATTRIBUTES_H_

/*
 *	Types of machine-dependent attributes
 */
typedef unsigned int	vm_machine_attribute_t;

#define	MATTR_CACHE		1	/* cachability */
#define MATTR_MIGRATE		2	/* migrability */
#define	MATTR_REPLICATE		4	/* replicability */

/*
 *	Values for the above, e.g. operations on attribute
 */
typedef int		vm_machine_attribute_val_t;

#define MATTR_VAL_OFF		0	/* (generic) turn attribute off */
#define MATTR_VAL_ON		1	/* (generic) turn attribute on */
#define MATTR_VAL_GET		2	/* (generic) return current value */

#define MATTR_VAL_CACHE_FLUSH	6	/* flush from all caches */
#define MATTR_VAL_DCACHE_FLUSH	7	/* flush from data caches */
#define MATTR_VAL_ICACHE_FLUSH	8	/* flush from instruction caches */

#endif	/* VM_ATTRIBUTES_H_ */
