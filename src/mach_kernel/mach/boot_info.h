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
 * Revision 2.2  92/01/03  20:19:42  dbg
 * 	Created.
 * 	[91/09/06            dbg]
 * 
 */
/* CMU_ENDHIST */
/*
 */
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

#ifndef	_MACH_BOOT_INFO_H_
#define	_MACH_BOOT_INFO_H_

/*
 * Structure of Mach kernel boot file.
 */
#include <mach/machine/vm_types.h>

/*
 *	A Mach kernel boot file consists of the Mach
 *	kernel image and the bootstrap image, glued
 *	together.
 *
 *	The first part of the file is a normal executable
 *	(bootable) file: a.out, coff, or whatever.  The
 *	text and data sizes are set to include the entire
 *	file.  (Some machines do not allow a zero-length
 *	data segment).
 *
 *	The rest of the file sits where the kernel BSS
 *	should be.  A boot_info record describes the
 *	sizes of the next 3 sections.  Following this
 *	are the kernel symbol table, the bootstrap image
 *	(including its symbol table), and the loader
 *	information for the bootstrap image.  Each
 *	of these sections is padded to an integer (4-byte)
 *	boundary.
 *
 *	When the file is loaded into memory, the kernel
 *	text and data segments are at their normal locations.
 *
 *	The boot_info structure appears at the start of
 *	the bss (at 'edata[]'):
 */

struct boot_info {
	vm_size_t	sym_size;		/* size of kernel symbols */
	vm_size_t	boot_size;		/* size of bootstrap image */
	vm_size_t	load_info_size;		/* size of loader information
						   for bootstrap image */
};

/*
 *	The 3 sections must be moved out of BSS for the kernel to run:
 *
 *	The kernel symbol table follows the BSS (at 'end[]').
 *
 *	The bootstrap image is on the first page boundary (machine page
 *	size) following the kernel symbol table.
 *
 *	The loader information immediately follows the bootstrap image.
 */

/*
 *	Loader information for bootstrap image:
 */

#define AOUT_F 1
#define ROSE_F 2
#define COFF_F 3

struct loader_info {
    int         format;         /* symbol table format (A.OUT or ROSE) */  
    vm_offset_t	text_start;	/* text start in memory */
    vm_size_t	text_size;	/* text size */
    vm_offset_t	text_offset;	/* text offset in file */
    vm_offset_t	data_start;	/* data+bss start in memory */
    vm_size_t	data_size;	/* data size */
    vm_offset_t	data_offset;	/* data offset in file */
    vm_size_t	bss_size;	/* BSS size */
    vm_offset_t str_offset;     /* strings table offset in file */
    vm_size_t   str_size;       /* strings table size */
    vm_offset_t	sym_offset[4];	/* symbol table offset in file */
    vm_size_t	sym_size[4];	/* symbol table size */
    vm_offset_t	entry_1;	/* 2 words for entry address */
    vm_offset_t	entry_2;
} ;

#define	EX_NOT_EXECUTABLE	6000

#endif	/* _MACH_BOOT_INFO_H_ */
