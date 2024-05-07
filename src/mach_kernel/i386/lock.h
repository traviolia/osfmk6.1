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
 * Revision 2.4.4.1  92/02/18  18:47:03  jeffreyh
 * 	Do not use simple lock macros if MACH_MP_DEBUG or
 * 	MACH_LOCK_MON are set.
 * 	[91/12/06            bernadat]
 * 
 * 	Dont use inline macros in case of debug
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.4  91/11/12  11:50:47  rvb
 * 	Added simple_lock_pause.
 * 	[91/11/12            rpd]
 * 
 * Revision 2.3  91/05/14  16:11:00  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/05/08  12:39:08  dbg
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
 * Machine-dependent simple locks for the i386.
 */

#ifndef	_I386_LOCK_H_
#define	_I386_LOCK_H_

#include <cpus.h>

#define	NEED_ATOMIC	((NCPUS > 1) || MACH_RT)

typedef char			hw_lock_t;

#if defined(__GNUC__)

/*
 *	General bit-lock routines.
 */

#define	bit_lock(bit,l)							\
	__asm__ volatile("	jmp	1f	\n			\
		 	0:	btl	%0, %1	\n			\
				jb	0b	\n			\
			1:	lock		\n			\
				btsl	%0,%1	\n			\
				jb	0b"			:	\
								:	\
			"r" (bit), "m" (*(volatile int *)(l))	:	\
			"memory");

#define	bit_unlock(bit,l)						\
	__asm__ volatile("	lock		\n			\
				btrl	%0,%1"			:	\
								:	\
			"r" (bit), "m" (*(volatile int *)(l)));

/*
 *      Set or clear individual bits in a long word.
 *      The locked access is needed only to lock access
 *      to the word, not to individual bits.
 */

#define	i_bit_set(bit,l)						\
	__asm__ volatile("	lock		\n			\
				btsl	%0,%1"			:	\
								:	\
			"r" (bit), "m" (*(l)));

#define	i_bit_clear(bit,l)						\
	__asm__ volatile("	lock		\n			\
				btrl	%0,%1"			:	\
								:	\
			"r" (bit), "m" (*(l)));

extern char __inline__	xchgb(char * cp, char new);

extern void __inline__	atomic_incl(long * p, long delta);
extern void __inline__	atomic_incs(short * p, short delta);
extern void __inline__	atomic_incb(char * p, char delta);

extern void __inline__	atomic_decl(long * p, long delta);
extern void __inline__	atomic_decs(short * p, short delta);
extern void __inline__	atomic_decb(char * p, char delta);

extern long __inline__	atomic_getl(long * p);
extern short __inline__	atomic_gets(short * p);
extern char __inline__	atomic_getb(char * p);

extern void __inline__	atomic_setl(long * p, long value);
extern void __inline__	atomic_sets(short * p, short value);
extern void __inline__	atomic_setb(char * p, char value);

extern char __inline__	xchgb(char * cp, char new)
{
	register char	old = new;

	__asm__ volatile ("	xchgb	%0,%2"			:
			"=q" (old)				:
			"0" (new), "m" (*(volatile char *)cp) : "memory");
	return (old);
}

extern void __inline__ atomic_incl(long * p, long delta)
{
#if NEED_ATOMIC
	__asm__ volatile ("	lock		\n		\
				addl    %0,%1"		:	\
							:	\
				"r" (delta), "m" (*(volatile long *)p));
#else /* NEED_ATOMIC */
	*p += delta;
#endif /* NEED_ATOMIC */
}

extern void __inline__ atomic_incs(short * p, short delta)
{
#if NEED_ATOMIC
	__asm__ volatile ("	lock		\n		\
				addw    %0,%1"		:	\
							:	\
				"q" (delta), "m" (*(volatile short *)p));
#else /* NEED_ATOMIC */
	*p += delta;
#endif /* NEED_ATOMIC */
}

extern void __inline__ atomic_incb(char * p, char delta)
{
#if NEED_ATOMIC
	__asm__ volatile ("	lock		\n		\
				addb    %0,%1"		:	\
							:	\
				"q" (delta), "m" (*(volatile char *)p));
#else /* NEED_ATOMIC */
	*p += delta;
#endif /* NEED_ATOMIC */
}

extern void __inline__	atomic_decl(long * p, long delta)
{
#if NCPUS > 1
	__asm__ volatile ("	lock		\n		\
				subl	%0,%1"		:	\
							:	\
				"r" (delta), "m" (*(volatile long *)p));
#else /* NCPUS > 1 */
	*p -= delta;
#endif /* NCPUS > 1 */
}

extern void __inline__ atomic_decs(short * p, short delta)
{
#if NEED_ATOMIC
	__asm__ volatile ("	lock		\n		\
				subw    %0,%1"		:	\
							:	\
				"q" (delta), "m" (*(volatile short *)p));
#else /* NEED_ATOMIC */
	*p -= delta;
#endif /* NEED_ATOMIC */
}

extern void __inline__ atomic_decb(char * p, char delta)
{
#if NEED_ATOMIC
	__asm__ volatile ("	lock		\n		\
				subb    %0,%1"		:	\
							:	\
				"q" (delta), "m" (*(volatile char *)p));
#else /* NEED_ATOMIC */
	*p -= delta;
#endif /* NEED_ATOMIC */
}

extern long __inline__	atomic_getl(long * p)
{
	return (*p);
}

extern short __inline__	atomic_gets(short * p)
{
	return (*p);
}

extern char __inline__	atomic_getb(char * p)
{
	return (*p);
}

extern void __inline__	atomic_setl(long * p, long value)
{
	*p = value;
}

extern void __inline__	atomic_sets(short * p, short value)
{
	*p = value;
}

extern void __inline__	atomic_setb(char * p, char value)
{
	*p = value;
}

#else	/* !defined(__GNUC__) */

extern void	i_bit_set(
	int index,
	void *addr);

extern void	i_bit_clear(
	int index,
	void *addr);

extern void bit_lock(
	int index,
	void *addr);

extern void bit_unlock(
	int index,
	void *addr);

/*
 * All other routines defined in __GNUC__ case lack
 * definitions otherwise. - XXX
 */

#endif	/* !defined(__GNUC__) */

#if	NORMA_IPC       /* Temporary */
#define mutex_try       _mutex_try
#define mutex_lock      _mutex_lock
#else	/* NORMA_IPC */
#if	MACH_LDEBUG || !MACH_RT
#define	mutex_try	_mutex_try
#define	mutex_lock	_mutex_lock
#else	/* MACH_LDEBUG || !MACH_RT */
#define	mutex_try(m)	(!xchgb ((&(m)->locked), 1))
#define	mutex_lock(m)	(mutex_try (m) ? (void) 1 : _mutex_lock (m))
#endif	/* MACH_LDEBUG || !MACH_RT */
#endif	/* NORMA_IPC */

extern void		kernel_preempt_check (void);

#endif	/* _I386_LOCK_H_ */

