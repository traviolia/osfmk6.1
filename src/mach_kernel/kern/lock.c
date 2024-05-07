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
 * Revision 2.7.2.1  92/03/03  16:20:08  jeffreyh
 * 	Changes from TRUNK
 * 	[92/02/26  11:54:39  jeffreyh]
 * 
 * Revision 2.8  92/01/23  15:20:56  rpd
 * 	Fixed lock_done and lock_read_to_write to only wakeup
 * 	if the number of readers is zero (from Sanjay Nadkarni).
 * 	Fixed db_show_all_slocks.
 * 	[92/01/20            rpd]
 * 
 * Revision 2.7  91/07/01  08:25:03  jsb
 * 	Implemented db_show_all_slocks.
 * 	[91/06/29  16:52:53  jsb]
 * 
 * Revision 2.6  91/05/18  14:32:07  rpd
 * 	Added simple_locks_info, to keep track of which locks are held.
 * 	[91/05/02            rpd]
 * 	Added check_simple_locks.
 * 	[91/03/31            rpd]
 * 
 * Revision 2.5  91/05/14  16:43:40  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:27:31  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:14:31  mrt]
 * 
 * Revision 2.3  90/06/02  14:54:56  rpd
 * 	Allow the lock to be taken in simple_lock_try.
 * 	[90/03/26  22:09:13  rpd]
 * 
 * Revision 2.2  90/01/11  11:43:18  dbg
 * 	Upgrade to mainline: use simple_lock_addr when calling
 * 	thread_sleep.
 * 	[89/12/11            dbg]
 * 
 * Revision 2.1  89/08/03  15:45:34  rwd
 * Created.
 * 
 * Revision 2.3  88/09/25  22:14:45  rpd
 * 	Changed explicit panics in sanity-checking simple locking calls
 * 	to assertions.
 * 	[88/09/12  23:03:22  rpd]
 * 
 * Revision 2.2  88/07/20  16:35:47  rpd
 * Add simple-locking sanity-checking code.
 * 
 * 23-Jan-88  Richard Sanzi (sanzi) at Carnegie-Mellon University
 *	On a UNIPROCESSOR, set lock_wait_time = 0.  There is no reason
 *	for a uni to spin on a complex lock.
 *
 * 18-Nov-87  Avadis Tevanian (avie) at Carnegie-Mellon University
 *	Eliminated previous history.
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
 *	File:	kern/lock.c
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 *	Date:	1985
 *
 *	Locking primitives implementation
 */

#include <cpus.h>
#include <mach_kdb.h>

#include <kern/lock.h>
#include <kern/misc_protos.h>
#include <kern/thread.h>
#include <kern/sched_prim.h>
#include <kern/ipc_sched.h>
#include <kern/xpr.h>
#include <string.h>

#if	MACH_KDB
#include <ddb/db_command.h>
#include <ddb/db_output.h>
#include <ddb/db_sym.h>
#include <ddb/db_print.h>
#endif	/* MACH_KDB */


#if	NCPUS > 1
int lock_wait_time = 100;
#else	/* NCPUS > 1 */

	/*
	 *	It is silly to spin on a uni-processor as if we
	 *	thought something magical would happen to the
	 *	want_write bit while we are executing.
	 */

int lock_wait_time = 0;
#endif	/* NCPUS > 1 */

int	spinlock_count[NCPUS];

/* Forward */

void	db_print_spinlock(
			spinlock_t	* addr);

void	db_print_mutex(
			mutex_t		* addr);

/*
 *	Routine:	lock_init
 *	Function:
 *		Initialize a lock; required before use.
 *		Note that clients declare the "struct lock"
 *		variables and then initialize them, rather
 *		than getting a new one from this module.
 */
void
lock_init(
	lock_t		* l,
	boolean_t	can_sleep)
{
	(void) memset((void *) l, 0, sizeof(lock_t));
	spinlock_init(&l->interlock);
	l->want_write = FALSE;
	l->want_upgrade = FALSE;
	l->read_count = 0;
	l->can_sleep = can_sleep;
}


/*
 *	Sleep locks.  These use the same data structure and algorithm
 *	as the spin locks, but the process sleeps while it is waiting
 *	for the lock.  These work on uniprocessor systems.
 */

void
lock_write(
	register lock_t	* l)
{
	register int	i;

	spinlock_lock(&l->interlock);

	/*
	 *	Try to acquire the want_write bit.
	 */
	while (l->want_write) {
		if ((i = lock_wait_time) > 0) {
			spinlock_unlock(&l->interlock);
			while (--i > 0 && l->want_write)
				continue;
			spinlock_lock(&l->interlock);
		}

		if (l->can_sleep && l->want_write) {
			l->waiting = TRUE;
			thread_sleep_spinlock((int) l, &l->interlock, FALSE);
			spinlock_lock(&l->interlock);
		}
	}
	l->want_write = TRUE;

	/* Wait for readers (and upgrades) to finish */

	while ((l->read_count != 0) || l->want_upgrade) {
		if ((i = lock_wait_time) > 0) {
			spinlock_unlock(&l->interlock);
			while (--i > 0 && (l->read_count != 0 ||
					l->want_upgrade))
				continue;
			spinlock_lock(&l->interlock);
		}

		if (l->can_sleep && (l->read_count != 0 || l->want_upgrade)) {
			l->waiting = TRUE;
			thread_sleep_spinlock((int) l, &l->interlock, FALSE);
			spinlock_lock(&l->interlock);
		}
	}
	spinlock_unlock(&l->interlock);
}

void
lock_done(
	register lock_t	* l)
{
	boolean_t	do_wakeup = FALSE;

	spinlock_lock(&l->interlock);

	if (l->read_count != 0)
		l->read_count--;
	else
	if (l->want_upgrade)
	 	l->want_upgrade = FALSE;
	else
	 	l->want_write = FALSE;

	/*
	 *	There is no reason to wakeup a waiting thread
	 *	if the read-count is non-zero.  Consider:
	 *		we must be dropping a read lock
	 *		threads are waiting only if one wants a write lock
	 *		if there are still readers, they can't proceed
	 */

	if (l->waiting && (l->read_count == 0)) {
		l->waiting = FALSE;
		do_wakeup = TRUE;
	}

	spinlock_unlock(&l->interlock);

	if (do_wakeup)
		thread_wakeup((int) l);
}

void
lock_read(
	register lock_t	* l)
{
	register int	i;

	spinlock_lock(&l->interlock);

	while (l->want_write || l->want_upgrade) {
		if ((i = lock_wait_time) > 0) {
			spinlock_unlock(&l->interlock);
			while (--i > 0 && (l->want_write || l->want_upgrade))
				continue;
			spinlock_lock(&l->interlock);
		}

		if (l->can_sleep && (l->want_write || l->want_upgrade)) {
			l->waiting = TRUE;
			thread_sleep_spinlock((int) l, &l->interlock, FALSE);
			spinlock_lock(&l->interlock);
		}
	}

	l->read_count++;
	spinlock_unlock(&l->interlock);
}

#if	MACH_RT || (NCPUS == 1)
	/* lock_read_to_write is unused */
#else
	/* vm_map.h is only user */
/*
 *	Routine:	lock_read_to_write
 *	Function:
 *		Improves a read-only lock to one with
 *		write permission.  If another reader has
 *		already requested an upgrade to a write lock,
 *		no lock is held upon return.
 *
 *		Returns TRUE if the upgrade *failed*.
 */

boolean_t
lock_read_to_write(
	register lock_t	* l)
{
	register int	i;
	boolean_t	do_wakeup = FALSE;

	spinlock_lock(&l->interlock);

	l->read_count--;

	if (l->want_upgrade) {
		/*
		 *	Someone else has requested upgrade.
		 *	Since we've released a read lock, wake
		 *	him up.
		 */
		if (l->waiting && (l->read_count == 0)) {
			l->waiting = FALSE;
			do_wakeup = TRUE;
		}

		spinlock_unlock(&l->interlock);
		if (do_wakeup)
			thread_wakeup((int) l);
		return (TRUE);
	}

	l->want_upgrade = TRUE;

	while (l->read_count != 0) {
		if ((i = lock_wait_time) > 0) {
			spinlock_unlock(&l->interlock);
			while (--i > 0 && l->read_count != 0)
				continue;
			spinlock_lock(&l->interlock);
		}

		if (l->can_sleep && l->read_count != 0) {
			l->waiting = TRUE;
			thread_sleep_spinlock((int) l, &l->interlock, FALSE);
			spinlock_lock(&l->interlock);
		}
	}

	spinlock_unlock(&l->interlock);
	return (FALSE);
}
#endif	/* !MACH_RT vm_map is only user */

void
lock_write_to_read(
	register lock_t	* l)
{
	boolean_t	do_wakeup = FALSE;

	spinlock_lock(&l->interlock);

	l->read_count++;
	if (l->want_upgrade)
		l->want_upgrade = FALSE;
	else
	 	l->want_write = FALSE;

	if (l->waiting) {
		l->waiting = FALSE;
		do_wakeup = TRUE;
	}

	spinlock_unlock(&l->interlock);
	if (do_wakeup)
		thread_wakeup((int) l);
}


#if	0	/* Unused */
/*
 *	Routine:	lock_try_write
 *	Function:
 *		Tries to get a write lock.
 *
 *		Returns FALSE if the lock is not held on return.
 */

boolean_t
lock_try_write(
	register lock_t	* l)
{
	spinlock_lock(&l->interlock);

	if (l->want_write || l->want_upgrade || l->read_count) {
		/*
		 *	Can't get lock.
		 */
		spinlock_unlock(&l->interlock);
		return(FALSE);
	}

	/*
	 *	Have lock.
	 */

	l->want_write = TRUE;
	spinlock_unlock(&l->interlock);
	return(TRUE);
}

/*
 *	Routine:	lock_try_read
 *	Function:
 *		Tries to get a read lock.
 *
 *		Returns FALSE if the lock is not held on return.
 */

boolean_t
lock_try_read(
	register lock_t	* l)
{
	spinlock_lock(&l->interlock);

	if (l->want_write || l->want_upgrade) {
		spinlock_unlock(&l->interlock);
		return(FALSE);
	}

	l->read_count++;
	spinlock_unlock(&l->interlock);
	return(TRUE);
}
#endif		/* Unused */

#if	MACH_KDB

void
db_show_one_lock(
	lock_t  *lock)
{
	db_printf("Read_count = 0x%x, %swant_upgrade, %swant_write, ",
		  lock->read_count,
		  lock->want_upgrade ? "" : "!",
		  lock->want_write ? "" : "!");
	db_printf("%swaiting, %scan_sleep\n", 
		  lock->waiting ? "" : "!", lock->can_sleep ? "" : "!");
#if	0 /* MACH_LDEBUG */
	db_printf("Last shared   by : ");
	if (lock->read_pc == VM_MAX_KERNEL_ADDRESS)
		db_printf("<nobody>");
	else
		db_printsym(lock->read_pc, DB_STGY_PROC);
	db_printf("\nLast owned    by : ");
	if (lock->write_pc == VM_MAX_KERNEL_ADDRESS)
		db_printf("<nobody>");
	else
		db_printsym(lock->write_pc, DB_STGY_PROC);
	db_printf("\nLast upgraded by : ");
	if (lock->upgrade_pc == VM_MAX_KERNEL_ADDRESS)
		db_printf("<nobody>");
	else
		db_printsym(lock->upgrade_pc, DB_STGY_PROC);
	db_printf("\nLast reduced  by : ");
	if (lock->reduce_pc == VM_MAX_KERNEL_ADDRESS)
		db_printf("<nobody>");
	else
		db_printsym(lock->reduce_pc, DB_STGY_PROC);
	db_printf("\nLast released by : ");
	if (lock->done_pc == VM_MAX_KERNEL_ADDRESS)
		db_printf("<nobody>");
	else
		db_printsym(lock->done_pc, DB_STGY_PROC);
	db_putchar('\n');
#endif	/* MACH_LDEBUG */
#if	0 /* MACH_SLOCKS */
	db_printf("Interlock ");
	db_show_one_slock(&lock->interlock);
#endif	/* MACH_SLOCKS */
}
#endif	/* MACH_KDB */

/*
 * The C portion of the mutex package.  These routines are only invoked
 * if the optimized assembler routines can't do the work.
 */

/*
 * mutex_lock_wait: Invoked if the assembler routine mutex_lock () fails
 * because the mutex is already held by another thread.  Called with the
 * interlock locked and returns with the interlock unlocked.
 */

void
mutex_lock_wait (
	mutex_t		* m)
{
	m->waiters++;
	thread_sleep_interlock ((int) m, &m->interlock, FALSE);
}

/*
 * mutex_unlock_wakeup: Invoked if the assembler routine mutex_unlock ()
 * fails because there are thread(s) waiting for this mutex.  Called and
 * returns with the interlock locked.
 */

void
mutex_unlock_wakeup (
	mutex_t		* m)
{
	m->waiters--;
	thread_wakeup_one ((int) m);
}


/*
 * mutex_pause: Called by former callers of simple_lock_pause().
 */

void
mutex_pause(void)
{
	thread_will_wait_with_timeout (current_thread(), 1);
	thread_block (0);
}

#if	MACH_KDB
/*
 * Routines to print out spinlocks and mutexes in a nicely-formatted
 * fashion.
 */

char *spinlock_labels = "ENTRY    ILK THREAD   DURATION CALLER";
char *mutex_labels =    "ENTRY    LOCKED WAITERS   THREAD CALLER";

void
db_show_one_spinlock (
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char		* modif)
{
	spinlock_t	* saddr = (spinlock_t *)addr;

	if (saddr == (spinlock_t *)0 || !have_addr) {
		db_error ("No spinlock\n");
	}
#if	MACH_LDEBUG
	else if (saddr->type != SPINLOCK_TAG)
		db_error ("Not a spinlock\n");
#endif	/* MACH_LDEBUG */

	db_printf ("%s\n", spinlock_labels);
	db_print_spinlock (saddr);
}

void
db_print_spinlock (
	spinlock_t	* addr)
{

	db_printf ("%08x %3d", addr, addr->interlock);
#if	MACH_LDEBUG
	db_printf (" %08x", addr->thread);
	db_printf (" %08x ", addr->duration[1]);
	db_printsym (addr->pc, DB_STGY_ANY);
#endif	/* MACH_LDEBUG */
	db_printf ("\n");
}

void
db_show_one_mutex (
	db_expr_t	addr,
	boolean_t	have_addr,
	db_expr_t	count,
	char		* modif)
{
	mutex_t		* maddr = (mutex_t *)addr;

	if (maddr == (mutex_t *)0 || !have_addr)
		db_error ("No mutex\n");
#if	MACH_LDEBUG
	else if (maddr->type != MUTEX_TAG)
		db_error ("Not a mutex\n");
#endif	/* MACH_LDEBUG */

	db_printf ("%s\n", mutex_labels);
	db_print_mutex (maddr);
}

void
db_print_mutex (
	mutex_t		* addr)
{
	db_printf ("%08x %6d %7d", addr, addr->locked, addr->waiters);
#if	MACH_LDEBUG
	db_printf (" %08x ", addr->thread);
	db_printsym (addr->pc, DB_STGY_ANY);
#endif	/* MACH_LDEBUG */
	db_printf ("\n");
}
#endif	/* MACH_KDB */

#if	MACH_LDEBUG
extern void	meter_spinlock_lock (
			spinlock_t	* l);
extern void	meter_spinlock_unlock (
			spinlock_t	* l);
extern void	cyctm05_stamp (
			unsigned long	* start);
extern void	cyctm05_diff (
			unsigned long	* start,
			unsigned long	* end,
			unsigned long	* diff);

spinlock_t	loser;

void
meter_spinlock_lock(
		spinlock_t	* lp)
{
#if 0
	cyctm05_stamp (lp->duration);
#endif
}

int			long_spinlock_crash;
int			long_spinlock_time = 0x600;
extern spinlock_t	kd_tty;		/* HACK - not really a spinlock! */

void
meter_spinlock_unlock(
		spinlock_t	* lp)
{
#if 0
	unsigned long	stime[2], etime[2], delta[2];

	if (lp == &kd_tty)			/* XXX */
		return;				/* XXX */

	stime[0] = lp->duration[0];
	stime[1] = lp->duration[1];

	cyctm05_stamp (etime);

	if (etime[1] < stime[1])		/* XXX */
		return;				/* XXX */

	cyctm05_diff (stime, etime, delta);

	if (delta[1] >= 0x10000)		/* XXX */
		return;				/* XXX */

	lp->duration[0] = delta[0];
	lp->duration[1] = delta[1];

	if (loser.duration[1] < lp->duration[1])
		loser = *lp;

	assert (!long_spinlock_crash || delta[1] < long_spinlock_time);
#endif
}

#if	NCPUS == 1
spinlock_t	* lock_stack[32];
int		lock_stack_count;
boolean_t	lock_stack_enabled = TRUE;

void
lock_stack_push(
		spinlock_t	* lock);
void
lock_stack_pop(
		spinlock_t	* lock);

void
lock_stack_push(
		spinlock_t	* lock)
{
	if (lock_stack_enabled) {
		lock_stack[lock_stack_count++] = lock;
	}
}

void
lock_stack_pop(
		spinlock_t	* lock)
{
	int			i;

	if (!lock_stack_enabled)
		return;

	lock_stack_count--;
	for (i = 0; i <= lock_stack_count; i++) {
		if (lock_stack[i] == lock) {
			if (i != lock_stack_count) {
				lock_stack[i] = lock_stack[lock_stack_count];
			}
			return;
		}
	}

	lock_stack_enabled = FALSE;
	panic ("lock stack botch!");
}

#endif	/* NCPUS == 1 */
#endif	/* MACH_LDEBUG */
