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
 * Revision 2.3.10.1  92/02/18  18:50:48  jeffreyh
 * 	Support for Corollary MP
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.3  91/05/14  16:19:14  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/05/08  12:44:00  dbg
 * 	Created (from old i386/start.s).
 * 	[91/04/26  14:40:03  dbg]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
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
 * Startup code for an i386 on an AT.
 * Kernel is loaded starting at 1MB.
 * Protected mode, paging disabled.
 *
 * %esp ->	boottype
 *		size of extended memory (K)
 *		size of conventional memory (K)
 *		boothowto
 *		esym (if KDB set up)
 *
 */

#include <platforms.h>
#include <mach_kdb.h>

#include <i386/asm.h>

#ifdef	CBUS
#include <cbus/cbus.h>
#endif  /* CBUS */

	popl	EXT(boottype)+KVTOPHYS	/ get boottype
	popl	EXT(extmem)+KVTOPHYS	/ extended memory, in K
	popl	EXT(cnvmem)+KVTOPHYS	/ conventional memory, in K
  	popl	%edx
	movl	%edx,EXT(boothowto)+KVTOPHYS	/ boothowto
	popl	%eax			/ get boot_string & esym

	/* must move bootstring now */
	movl	%eax, %esi
	lea	PA(EXT(boot_string_store)), %edi
	movl	PA(EXT(boot_string_sz)), %ecx
	cld
	rep
	movsb

#ifdef	CBUS
	addl	$CBUS_START, %eax
#endif	/* CBUS */
