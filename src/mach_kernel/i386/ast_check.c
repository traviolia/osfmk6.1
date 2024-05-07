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
 * Revision 2.3.9.2  92/04/30  11:49:41  bernadat
 * 	Adaptations for Corollary and Systempro
 * 	[92/04/08            bernadat]
 * 
 * Revision 2.3.9.1  92/02/18  18:43:26  jeffreyh
 * 	Support for the Corollary MP
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.3  91/05/14  16:03:21  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/05/08  12:30:25  dbg
 * 	Copied for i386.
 * 	[91/03/21            dbg]
 * 
 * Revision 2.2  90/08/27  22:13:09  dbg
 * 	Created.
 * 	[90/07/18            dbg]
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
#include <cpus.h>
#include <platforms.h>

#if	NCPUS > 1 && !AT386

/*
 * Handle signalling ASTs on other processors.
 *
 * Initial i386 implementation does nothing.
 */

#include <kern/processor.h>

/*
 * Initialize for remote invocation of ast_check.
 */
init_ast_check(processor)
	processor_t	processor;
{
#ifdef lint
	processor++;
#endif /* lint */
}

/*
 * Cause remote invocation of ast_check.  Caller is at splsched().
 */
cause_ast_check(processor)
	processor_t	processor;
{
#ifdef lint
	processor++;
#endif /* lint */
}

#endif	/* NCPUS > 1 && !AT386 */
