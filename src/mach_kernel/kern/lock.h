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
 * Revision 2.8  91/11/12  11:51:58  rvb
 * 	Added simple_lock_pause.
 * 	[91/11/12            rpd]
 * 
 * Revision 2.7  91/05/18  14:32:17  rpd
 * 	Added check_simple_locks.
 * 	[91/03/31            rpd]
 * 
 * Revision 2.6  91/05/14  16:43:51  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/05/08  12:47:17  dbg
 * 	When actually using the locks (on multiprocessors), import the
 * 	machine-dependent simple_lock routines from machine/lock.h.
 * 	[91/04/26  14:42:23  dbg]
 * 
 * Revision 2.4  91/02/05  17:27:37  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:14:39  mrt]
 * 
 * Revision 2.3  90/11/05  14:31:18  rpd
 * 	Added simple_lock_taken.
 * 	[90/11/04            rpd]
 * 
 * Revision 2.2  90/01/11  11:43:26  dbg
 * 	Upgraded to match mainline:
 * 	 	Added decl_simple_lock_data, simple_lock_addr macros.
 * 	 	Rearranged complex lock structure to use decl_simple_lock_data
 * 	 	for the interlock field and put it last (except on ns32000).
 * 	 	[89/01/15  15:16:47  rpd]
 * 
 * 	Made all machines use the compact field layout.
 * 
 * Revision 2.1  89/08/03  15:49:42  rwd
 * Created.
 * 
 * Revision 2.2  88/07/20  16:49:35  rpd
 * Allow for sanity-checking of simple locking on uniprocessors,
 * controlled by new option MACH_LDEBUG.  Define composite
 * MACH_SLOCKS, which is true iff simple locking calls expand
 * to code.  It can be used to #if-out declarations, etc, that
 * are only used when simple locking calls are real.
 * 
 *  3-Nov-87  David Black (dlb) at Carnegie-Mellon University
 *	Use optimized lock structure for multimax also.
 *
 * 27-Oct-87  Robert Baron (rvb) at Carnegie-Mellon University
 *	Use optimized lock "structure" for balance now that locks are
 *	done inline.
 *
 * 26-Jan-87  Avadis Tevanian (avie) at Carnegie-Mellon University
 *	Invert logic of no_sleep to can_sleep.
 *
 * 29-Dec-86  David Golub (dbg) at Carnegie-Mellon University
 *	Removed BUSYP, BUSYV, adawi, mpinc, mpdec.  Defined the
 *	interlock field of the lock structure to be a simple-lock.
 *
 *  9-Nov-86  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Added "unsigned" to fields in vax case, for lint.
 *
 * 21-Oct-86  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Added fields for sleep/recursive locks.
 *
 *  7-Oct-86  David L. Black (dlb) at Carnegie-Mellon University
 *	Merged Multimax changes.
 *
 * 26-Sep-86  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Removed reference to "caddr_t" from BUSYV/P.  I really
 *	wish we could get rid of these things entirely.
 *
 * 24-Sep-86  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Changed to directly import boolean declaration.
 *
 *  1-Aug-86  David Golub (dbg) at Carnegie-Mellon University
 *	Added simple_lock_try, sleep locks, recursive locking.
 *
 * 11-Jun-86  Bill Bolosky (bolosky) at Carnegie-Mellon University
 *	Removed ';' from definitions of locking macros (defined only
 *	when NCPU < 2). so as to make things compile.
 *
 * 28-Feb-86  Bill Bolosky (bolosky) at Carnegie-Mellon University
 *	Defined adawi to be add when not on a vax.
 *
 * 07-Nov-85  Michael Wayne Young (mwyoung) at Carnegie-Mellon University
 *	Overhauled from previous version.
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
 *	File:	kern/lock.h
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 *	Date:	1985
 *
 *	Locking primitives definitions
 */

#ifndef	_KERN_LOCK_H_
#define	_KERN_LOCK_H_

/*
 * Configuration variables:
 *
 *	MACH_LDEBUG:    record pc and thread of callers
 *
 */
	
#include <cpus.h>
#include <mach_ldebug.h>
#include <mach/boolean.h>
#include <kern/kern_types.h>
#include <kern/spl.h>
#include <machine/lock.h>

#include <mach_kdb.h>

/*
 *	A simple spin lock.
 *	Do not change the order of the fields in this structure without
 *	changing the machine-dependent assembler routines which depend
 *	on them.
 */

#define	SPINLOCK_TAG	0x53535353

typedef struct {
	hw_lock_t	interlock;
#if	MACH_LDEBUG
	int		type;
	int		pc;
	int		thread;
	unsigned long	duration[2];
#endif	/* MACH_LDEBUG */
} spinlock_t;

#if	(NCPUS > 1) || MACH_LDEBUG
#define	spinlock_lock(l)	_spinlock_lock(l)
#define	spinlock_unlock(l)	_spinlock_unlock(l)
#define spinlock_try(l)		_spinlock_try(l)
#else	/* (NCPUS > 1) || MACH_LDEBUG */
#if	MACH_RT
#define	spinlock_lock(l)	disable_preemption()
#define	spinlock_unlock(l)	enable_preemption()
#define	spinlock_try(l)		(disable_preemption(), 1)
#else	/* MACH_RT */
#define	spinlock_lock(l)
#define	spinlock_unlock(l)
#define spinlock_try(l)		(1)
#endif	/* MACH_RT */
#endif	/* (NCPUS > 1) || MACH_LDEBUG */

extern void		spinlock_init(
				spinlock_t	* lock);
extern void		_spinlock_lock(
				spinlock_t	* lock);
extern void		_spinlock_unlock(
				spinlock_t	* lock);
extern boolean_t	_spinlock_try(
				spinlock_t	* lock);

extern int		spinlock_count[NCPUS];

/*
 *	A simple mutex lock.
 *	Do not change the order of the fields in this structure without
 *	changing the machine-dependent assembler routines which depend
 *	on them.
 */

#define	MUTEX_TAG	0x4d4d4d4d

typedef struct {
	hw_lock_t		interlock;
	hw_lock_t		locked;
	short			waiters;
#if	MACH_LDEBUG
	int			type;
	int			pc;
	int			thread;
#endif	/* MACH_LDEBUG */
} mutex_t;

#if	MACH_RT || (NCPUS > 1) || MACH_LDEBUG
extern void		_mutex_lock(
				mutex_t		* lock);
extern void		mutex_unlock(
				mutex_t		* lock);
extern boolean_t	_mutex_try(
				mutex_t		* lock);
extern void		mutex_init(
				mutex_t		* lock);
#else	/* MACH_RT || (NCPUS > 1) || MACH_LDEBUG */
#define	_mutex_lock(l)
#define	mutex_unlock(l)
#define	_mutex_try(l)		(1)
#define	mutex_init(l)
#endif	/* MACH_RT || (NCPUS > 1) || MACH_LDEBUG */

extern void		mutex_lock_wait(
				mutex_t		* lock);
extern void		mutex_unlock_wakeup(
				mutex_t		* lock);
extern void		mutex_pause(void);

extern void		interlock_unlock(
				hw_lock_t	* lock);

/*
 *	The general lock structure.  Provides for multiple readers,
 *	upgrading from read to write, and sleeping until the lock
 *	can be gained.
 *
 *	On some architectures, assembly language code in the 'inline'
 *	program fiddles the lock structures.  It must be changed in
 *	concert with the structure layout.
 *
 *	Only the "interlock" field is used for hardware exclusion;
 *	other fields are modified with normal instructions after
 *	acquiring the interlock bit.
 */
typedef struct {
	spinlock_t	interlock;	/* Hardware interlock field */
	unsigned int	read_count:16,	/* Number of accepted readers */
			want_upgrade:1,	/* Read-to-write upgrade waiting */
			want_write:1,	/* Writer is waiting, or
					   locked for write */
			waiting:1,	/* Someone is sleeping on lock */
			can_sleep:1;	/* Can attempts to lock go to sleep? */
} lock_t;

/* Sleep locks must work even if no multiprocessing */

/* Initialize complex lock */
extern void		lock_init(
				lock_t		* l,
				boolean_t	can_sleep);

/* Lock complex lock for write */
extern void		lock_write(
				lock_t		* l);

/* Lock complex lock for read */
extern void		lock_read(
				lock_t		* l);

/* Unlock complex lock */
extern void		lock_done(
				lock_t		* l);

#if	MACH_RT || (NCPUS == 1)
	/* lock_read_to_write is unused */
#else
	/* vm_map is only user */
/* Upgrade read lock to write lock */
extern boolean_t	lock_read_to_write(
				lock_t		* l);
#endif

/* Downgrade write lock to read lock */
extern void		lock_write_to_read(
				lock_t		* l);

#if	0	/* Unused */
/* Attempt write lock.
 * Returns TRUE if successful.
 */
extern boolean_t	lock_try_write(
				lock_t		* l);

/* Attempt read lock.
 * Returns TRUE if successful.
 */
extern boolean_t	lock_try_read(
				lock_t		* l);
#endif		/* Unused */

#define	lock_read_done(l)	lock_done(l)
#define	lock_write_done(l)	lock_done(l)

#if	MACH_KDB

extern void		db_show_one_lock(
				lock_t		* l);

#endif	/* MACH_KDB */

#endif	/* _KERN_LOCK_H_ */

