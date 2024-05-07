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
/* CMU_HIST (from mach_clock.c) */
/*
 * Revision 2.15  91/07/31  17:45:57  dbg
 * 	Implemented host_adjust_time.
 * 	[91/07/30  17:03:54  dbg]
 * 
 * Revision 2.14  91/05/18  14:32:29  rpd
 * 	Fixed host_set_time to update the mapped time value.
 * 	Changed the mapped time value to include a check field.
 * 	[91/03/19            rpd]
 *
 * Revision 2.8  90/10/12  18:07:29  rpd
 * 	Fixed calls to thread_bind in host_set_time.
 * 	Fix from Philippe Bernadat.
 * 	[90/10/10            rpd]
 * 
 * Revision 2.4  90/01/11  11:43:31  dbg
 * 	Switch to master CPU in host_set_time.
 * 	[90/01/03            dbg]
 * 
 * Revision 2.2  89/08/05  16:07:11  rwd
 * 	Added mappable time code.
 * 	[89/08/02            rwd]
 */ 

#include <cpus.h>
#include <stat_time.h>
#include <norma_ipc.h>
#include <mach_prof.h>
#include <mach_assert.h>

#include <mach/boolean.h>
#include <mach/machine.h>
#include <mach/time_value.h>
#include <mach/vm_param.h>
#include <mach/vm_prot.h>
#include <kern/counters.h>
#include <kern/cpu_number.h>
#include <kern/host.h>
#include <kern/lock.h>
#include <kern/mach_param.h>
#include <kern/misc_protos.h>
#include <kern/posixtime.h>
#include <kern/processor.h>
#include <kern/sched.h>
#include <kern/sched_prim.h>
#include <kern/spl.h>
#include <kern/thread.h>
#include <kern/time_out.h>
#include <vm/vm_kern.h>
#include <sys/time.h>
#include <machine/mach_param.h>	/* HZ */

#include <mach/mach_host_server.h>

/* Externs */
extern kern_return_t	bbc_gettime(
				time_value_t *time);

extern kern_return_t	bbc_settime(
				time_value_t *time);

#if	HZ > 100
int		tickadj = 1;		/* can adjust HZ usecs per second */
#else
int		tickadj = 100 / HZ;	/* can adjust 100 usecs per second */
#endif
int		bigadj = 1000000;	/* adjust 10*tickadj if adj>bigadj */

/* module declared variables */
time_value_t		time = { 0, 0 };
int			timedelta = 0;
int			tickdelta = 0;
mapped_time_value_t	*mtime = 0;

#if	MACH_ASSERT
spinlock_t get_uniq_timestamp_lock;
#endif	/* MACH_ASSERT */

#define update_mapped_time(time)				\
MACRO_BEGIN							\
	if (mtime != 0) {					\
		mtime->check_seconds = (time)->seconds;		\
		mtime->microseconds = (time)->microseconds;	\
		mtime->seconds = (time)->seconds;		\
	}							\
MACRO_END

/*
 * Universal (Posix) time initialization.
 */
void
utime_init(void)
{
	vm_offset_t	*vp;

	vp = (vm_offset_t *) &mtime;
	if (kmem_alloc_wired(kernel_map, vp, PAGE_SIZE)	!= KERN_SUCCESS)
		panic("mapable_time_init");
	bzero((char *)mtime, PAGE_SIZE);
	(void)bbc_gettime(&time);
#if	MACH_ASSERT
	spinlock_init(&get_uniq_timestamp_lock);
#endif	/* MACH_ASSERT */
}

/*
 * Universal (Posix) time tick. This is called from the clock
 * interrupt path at splclock() interrupt level.
 */
void
utime_tick(void)
{
	register int	delta;

	delta = tick;
	if (timedelta < 0) {
		delta -= tickdelta;
		timedelta += tickdelta;
	} else
	if (timedelta > 0) {
		delta += tickdelta;
		timedelta -= tickdelta;
	}
	time_value_add_usec(&time, delta);
	update_mapped_time(&time);
}	

/*
 * Read the Universal (Posix) time.
 */
kern_return_t
host_get_time(
	host_t		host,
	time_value_t	*current_time)	/* OUT */
{
	if (host == HOST_NULL)
		return(KERN_INVALID_HOST);

	do {
		current_time->seconds = mtime->seconds;
		current_time->microseconds = mtime->microseconds;
	} while (current_time->seconds != mtime->check_seconds);

	return (KERN_SUCCESS);
}

/*
 * Set the Universal (Posix) time. Privileged call.
 */
kern_return_t
host_set_time(
	host_t		host,
	time_value_t	new_time)
{
	spl_t	s;

	if (host == HOST_NULL)
		return(KERN_INVALID_HOST);

#if	NCPUS > 1
	thread_bind(current_thread(), master_processor);
	if (current_processor() != master_processor)
		thread_block((void (*)(void)) 0);
#endif	/* NCPUS > 1 */

	s = splhigh();
	time = new_time;
	update_mapped_time(&time);
	(void)bbc_settime(&time);
	splx(s);

#if	NCPUS > 1
	thread_bind(current_thread(), PROCESSOR_NULL);
#endif	/* NCPUS > 1 */

	return (KERN_SUCCESS);
}

/*
 * Adjust the Universal (Posix) time gradually.
 */
kern_return_t
host_adjust_time(
	host_t		host,
	time_value_t	newadj,
	time_value_t	*oldadj)	/* OUT */
{
	time_value_t	oadj;
	unsigned int	ndelta;
	spl_t		s;

	if (host == HOST_NULL)
		return (KERN_INVALID_HOST);

	ndelta = (newadj.seconds * 1000000) + newadj.microseconds;

#if	NCPUS > 1
	thread_bind(current_thread(), master_processor);
	if (current_processor() != master_processor)
		thread_block((void (*)(void)) 0);
#endif	/* NCPUS > 1 */

	s = splclock();
	oadj.seconds = timedelta / 1000000;
	oadj.microseconds = timedelta % 1000000;
	if (timedelta == 0) {
		if (ndelta > bigadj)
			tickdelta = 10 * tickadj;
		else
			tickdelta = tickadj;
	}
	if (ndelta % tickdelta)
		ndelta = ndelta / tickdelta * tickdelta;
	timedelta = ndelta;
	splx(s);

#if	NCPUS > 1
	thread_bind(current_thread(), PROCESSOR_NULL);
#endif	/* NCPUS > 1 */

	*oldadj = oadj;

	return (KERN_SUCCESS);
}

#if	MACH_ASSERT
void
get_uniq_timestamp(
	int		*ts)
{
	static time_value_t last;
	int s;

	do {
		ts[1] = time.seconds;
		ts[0] = time.microseconds;
	} while (ts[1] != time.seconds);

	s = splimp();
	spinlock_lock(&get_uniq_timestamp_lock);
	if (ts[0] <= last.microseconds && ts[1] <= last.seconds) {
		ts[0] = last.microseconds + 1;
		ts[1] = last.seconds;
	}

	last.seconds = ts[1];
	last.microseconds = ts[0];
	spinlock_unlock(&get_uniq_timestamp_lock);
	splx(s);
}
#endif
