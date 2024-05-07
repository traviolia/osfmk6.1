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
 * Revision 2.3.3.2  92/04/30  11:50:57  bernadat
 * 	Adaptations for Corollary and Systempro
 * 	[92/04/08            bernadat]
 * 
 * Revision 2.3.3.1  92/03/03  16:16:20  jeffreyh
 * 	Changes for Corollary from bernadat
 * 	[92/02/26            jeffreyh]
 * 
 * Revision 2.4  92/01/03  20:08:42  dbg
 * 	Add macros to get and set various privileged registers.
 * 	[91/10/20            dbg]
 * 
 * Revision 2.3  91/05/14  16:15:32  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/05/08  12:41:37  dbg
 * 	Created.
 * 	[91/03/21            dbg]
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
 * Processor registers for i386 and i486.
 */
#ifndef	_I386_PROC_REG_H_
#define	_I386_PROC_REG_H_

/*
 * CR0
 */
#define	CR0_PG	0x80000000		/*	 enable paging */
#define	CR0_CD	0x40000000		/* i486: cache disable */
#define	CR0_NW	0x20000000		/* i486: no write-through */
#define	CR0_AM	0x00040000		/* i486: alignment check mask */
#define	CR0_WP	0x00010000		/* i486: write-protect kernel access */
#define	CR0_NE	0x00000020		/* i486: handle numeric exceptions */
#define	CR0_ET	0x00000010		/*	 extension type is 80387 */
					/*	 (not official) */
#define	CR0_TS	0x00000008		/*	 task switch */
#define	CR0_EM	0x00000004		/*	 emulate coprocessor */
#define	CR0_MP	0x00000002		/*	 monitor coprocessor */
#define	CR0_PE	0x00000001		/*	 enable protected mode */

#ifndef	ASSEMBLER
#ifdef	__GNUC__

extern unsigned int	get_cr0(void);
extern void		set_cr0(
				unsigned int		value);
extern unsigned int	get_cr2(void);

extern __inline__ unsigned int get_cr0(void)
{
	register unsigned int cr0; 
	__asm__ volatile("mov %%cr0, %0" : "=r" (cr0));
	return(cr0);
}

extern __inline__ void set_cr0(unsigned int value)
{
	__asm__ volatile("mov %0, %%cr0" : : "r" (value));
}

extern __inline__ unsigned int get_cr2(void)
{
	register unsigned int cr2;
	__asm__ volatile("mov %%cr2, %0" : "=r" (cr2));
	return(cr2);
}

extern unsigned int	get_cr3(void);
extern void		set_cr3(
				unsigned int		value);

#if	NCPUS > 1 && AT386
/*
 * get_cr3 and set_cr3 are more complicated for the MPs. cr3 is where
 * the cpu number gets stored. The MP versions live in locore.s
 */
#else	/* NCPUS > 1 && AT386 */

extern __inline__ unsigned int get_cr3(void)
{
	register unsigned int cr3;
	__asm__ volatile("mov %%cr3, %0" : "=r" (cr3));
	return(cr3);
}

extern __inline__ void set_cr3(unsigned int value)
{
	__asm__ volatile("mov %0, %%cr3" : : "r" (value));
}
#endif	/* NCPUS > 1 && AT386 */

#define	set_ts() \
	set_cr0(get_cr0() | CR0_TS)

#define	clear_ts() \
	__asm__ volatile("clts")

extern unsigned short	get_tr(void);

extern __inline__ unsigned short get_tr(void)
{
	unsigned short seg; 
	__asm__ volatile("str %0" : "=rm" (seg));
	return(seg);
}

#define	set_tr(seg) \
	__asm__ volatile("ltr %0" : : "rm" ((unsigned short)(seg)) )

extern unsigned short	get_ldt(void);

extern __inline__ unsigned short get_ldt(void)
{
	unsigned short seg;
	__asm__ volatile("sldt %0" : "=rm" (seg));
	return(seg);
}

#define	set_ldt(seg) \
	__asm__ volatile("lldt %0" : : "rm" ((unsigned short)(seg)) )

#endif	/* __GNUC__ */
#endif	/* ASSEMBLER */

#endif	/* _I386_PROC_REG_H_ */
