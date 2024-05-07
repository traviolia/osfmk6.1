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
 * Revision 2.1.2.1.3.1  92/02/18  19:08:45  jeffreyh
 * 	Created. Might need some work if used on anything but a 386.
 * 	[92/02/11  07:56:50  bernadat]
 */
/* CMU_ENDHIST */

/*
 */

/*
 * 	Support For MP Debugging
 *		if MACH_MP_DEBUG is on, we use alternate locking
 *		routines do detect dealocks
 *	Support for MP lock monitoring (MACH_LOCK_MON).
 *		Registers use of locks, contention.
 *		Depending on hardware also records time spent with locks held
 */

#include <cpus.h>
#include <mach_mp_debug.h>
#include <mach_lock_mon.h>
#include <time_stamp.h>

#include <sys/types.h>
#include <mach/machine/vm_types.h>
#include <mach/boolean.h>
#include <kern/thread.h>
#include <kern/lock.h>

extern spinlock_t	kdb_lock;
extern spinlock_t	printf_lock;

#if	NCPUS > 1 && MACH_LOCK_MON

#if	TIME_STAMP
extern	unsigned time_stamp;
#else	TIME_STAMP
#define	time_stamp 0
#endif	TIME_STAMP

#define LOCK_INFO_MAX	     (1024*32)
#define LOCK_INFO_HASH_COUNT 1024
#define LOCK_INFO_PER_BUCKET	(LOCK_INFO_MAX/LOCK_INFO_HASH_COUNT)


#define HASH_LOCK(lock)	((long)lock>>5 & (LOCK_INFO_HASH_COUNT-1))

struct lock_info {
	unsigned	success;
	unsigned	fail;
	unsigned	masked;
	unsigned	stack; 
	unsigned	time;
	spinlock_t	* lock;
	unsigned int	caller;
};

struct lock_info_bucket {
	struct lock_info info[LOCK_INFO_PER_BUCKET];
};

struct lock_info_bucket lock_info[LOCK_INFO_HASH_COUNT];
struct lock_info default_lock_info;
unsigned default_lock_stack = 0;

extern int curr_ipl[];



struct lock_info *
locate_lock_info(lock)
spinlock_t	** lock;
{
	struct lock_info *li =  &(lock_info[HASH_LOCK(*lock)].info[0]);
	register i;
	register my_cpu = cpu_number();

	for (i=0; i < LOCK_INFO_PER_BUCKET; i++, li++)
		if (li->lock) {
			if (li->lock == *lock)
				return(li);
		} else {
			li->lock = *lock;
			li->caller = *((unsigned int *)lock - 1);
			return(li);
		}
	db_printf("out of lock_info slots\n");
	li = &default_lock_info;
	return(li);
}
		

simple_lock(lock)
decl_simple_lock_data(, *lock)
{
	register struct lock_info *li = locate_lock_info(&lock);
	register my_cpu = cpu_number();

	if (current_thread()) 
		li->stack = current_thread()->lock_stack++;
	if (curr_ipl[my_cpu])
		li->masked++;
	if (_simple_lock_try(lock))
		li->success++;
	else {
		_simple_lock(lock);
		li->fail++;
	}
	li->time = time_stamp - li->time;
}

simple_lock_try(lock)
decl_simple_lock_data(, *lock)
{
	register struct lock_info *li = locate_lock_info(&lock);
	register my_cpu = cpu_number();

	if (curr_ipl[my_cpu])
		li->masked++;
	if (_simple_lock_try(lock)) {
		li->success++;
		li->time = time_stamp - li->time;
		if (current_thread())
			li->stack = current_thread()->lock_stack++;
		return(1);
	} else {
		li->fail++;
		return(0);
	}
}

simple_unlock(lock)
decl_simple_lock_data(, *lock)
{
	register unsigned stamp = time_stamp;
	register unsigned *time = &locate_lock_info(&lock)->time;
	register unsigned *lock_stack;

	*time = stamp - *time;
	_simple_unlock(lock);
	if (current_thread()) {
		lock_stack = &current_thread()->lock_stack;
		if (*lock_stack)
			(*lock_stack)--;
	}
}

lip() {
	lis(4, 1, 0);
}

#define lock_info_sort lis

unsigned scurval, ssum;
struct lock_info *sli;

lock_info_sort(arg, abs, count)
{
	struct lock_info *li, mean;
	int bucket = 0;
	int i;
	unsigned max_val;
	unsigned old_val = (unsigned)-1;
	struct lock_info *target_li = &lock_info[0].info[0];
	unsigned sum;
	unsigned empty, total;
	unsigned curval;

	printf("\nSUCCESS	FAIL	MASKED	STACK	TIME	LOCK/CALLER\n");
	if (!count)
		count = 8 ;
	while (count && target_li) {
		empty = LOCK_INFO_HASH_COUNT;
		target_li = 0;
		total = 0;
		max_val = 0;
		mean.success = 0;
		mean.fail = 0;
		mean.masked = 0;
		mean.stack = 0;
		mean.time = 0;
		mean.lock = (simple_lock_data_t *) &lock_info;
		mean.caller = (int) &lock_info;
		for (bucket = 0; bucket < LOCK_INFO_HASH_COUNT; bucket++) {
			li = &lock_info[bucket].info[0];
			if (li->lock)
				empty--;
			for (i= 0; i< LOCK_INFO_PER_BUCKET && li->lock; i++, li++) {
				if (li->lock == &kdb_lock || li->lock == &printf_lock)
					continue;
				total++;
				curval = *((int *)li + arg);
				sum = li->success + li->fail;
				if(!sum && !abs)
					continue;
				scurval = curval;
				ssum = sum;
				sli = li;
				if (!abs) switch(arg) {
				case 0:
					break;
				case 1:
				case 2:
					curval = (curval*100) / sum;
					break;
				case 3:
				case 4:
					curval = curval / sum;
					break;
				}
				if (curval > max_val && curval < old_val) {
					max_val = curval;
					target_li = li;
				}
				if (curval == old_val && count != 0) {
					print_lock_info(li);
					count--;
				}
				mean.success += li->success;
				mean.fail += li->fail;
				mean.masked += li->masked;
				mean.stack += li->stack;
				mean.time += li->time;
			}
		}
		if (target_li)
		        old_val = max_val;
	}
	db_printf("\n%d total locks, %d empty buckets", total, empty );
	if (default_lock_info.success) 
		db_printf(", default: %d", default_lock_info.success + default_lock_info.fail); 
	db_printf("\n");
	print_lock_info(&mean);
}

#define lock_info_clear lic

lock_info_clear()
{
	struct lock_info *li;
	int bucket = 0;
	int i;
	for (bucket = 0; bucket < LOCK_INFO_HASH_COUNT; bucket++) {
		li = &lock_info[bucket].info[0];
		for (i= 0; i< LOCK_INFO_PER_BUCKET; i++, li++) {
			bzero(li, sizeof(struct lock_info));
		}
	}
	bzero(&default_lock_info, sizeof(struct lock_info)); 
}

print_lock_info(li)
struct lock_info *li;
{
	int off;
	int sum = li->success + li->fail;
	db_printf("%d	%d/%d	%d/%d	%d/%d	%d/%d	", li->success,
		   li->fail, (li->fail*100)/sum,
		   li->masked, (li->masked*100)/sum,
		   li->stack, li->stack/sum,
		   li->time, li->time/sum);
	db_search_symbol(li->lock, 0, &off);
	if (off < 1024)
		db_printsym(li->lock, 0);
	else {
		db_printsym(li->caller, 0);
		db_printf("(%X)", li->lock);
	}
	db_printf("\n");
}

#endif	NCPUS > 1 && MACH_LOCK_MON

#if	TIME_STAMP

/*
 *	Measure lock/unlock operations
 */

time_lock(loops)
{
	decl_simple_lock_data(, lock)
	register unsigned stamp, i;


	if (!loops)
		loops = 1000;
	simple_lock_init(&lock);
	stamp = time_stamp;
	for (i = 0; i < loops; i++) {
		simple_lock(&lock);
		simple_unlock(&lock);
	}
	stamp = time_stamp - stamp;
	db_printf("%d stamps for simple_locks\n", stamp/loops);
#if	MACH_LOCK_MON
	stamp = time_stamp;
	for (i = 0; i < loops; i++) {
		_simple_lock(&lock);
		_simple_unlock(&lock);
        }
	stamp = time_stamp - stamp;
	db_printf("%d stamps for _simple_locks\n", stamp/loops);
#endif	MACH_LOCK_MON
}
#endif	TIME_STAMP





