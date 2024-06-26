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
 * Revision 2.10  91/08/24  11:52:28  af
 * 	Fixed spl munging, for 3min.
 * 	[91/08/02  01:49:09  af]
 * 
 * Revision 2.9  91/06/25  20:54:18  rpd
 * 	Tweaks to make gcc happy.
 * 
 * Revision 2.8  91/06/19  11:53:54  rvb
 * 	mips->DECSTATION; vax->VAXSTATION
 * 	[91/06/12  14:01:54  rvb]
 * 
 * 	File moved here from mips/PMAX since it tries to be generic;
 * 	it is used on the PMAX and could be used on the Vax3100.
 * 	It would need a few ifdef vax & ifdef mips for the latter.
 * 	[91/06/04            rvb]
 * 
 * Revision 2.7  91/05/14  17:24:22  mrt
 * 	Correcting copyright
 * 
 * Revision 2.6  91/02/14  14:34:41  mrt
 * 	Factored out delay() function, and made it box-indep.
 * 	Added accurate_config_delay() which calls delay()'s
 * 	configuration code.  Modified ackrtclock() to
 * 	invoke it on first call.
 * 	Tell the user what the CPU clock speed loks like, 
 * 	distinguish between DS3100 and DS2100 based on clock speed.
 * 	[91/02/12  13:03:16  af]
 * 
 * Revision 2.5  91/02/05  17:42:35  mrt
 * 	Added author notices
 * 	[91/02/04  11:15:06  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:13:45  mrt]
 * 
 * Revision 2.4  90/12/05  23:32:39  af
 * 
 * 
 * Revision 2.3  90/12/05  20:47:31  af
 * 	Made file conditional.
 * 	[90/12/03  23:28:03  af]
 * 
 * Revision 2.2  90/08/27  22:06:05  dbg
 * 	Initialized cpu_speed to a non-zero value.  Apparently,
 * 	we need delays much earlier than we can possibly know
 * 	what this machine looks like.  I know this will bite back.
 * 	[90/08/21            af]
 * 	Made apparent who is resetting the clock back to 1972.
 * 	[90/08/20  10:30:19  af]
 * 
 * 	Created, from Motorola's MC146818 specs.
 * 	[90/08/17            af]
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
 *	File:	mc_clock.c
 *	Author: Alessandro Forin
 *	Date:	8/90
 *
 *	Driver for the MC146818 Clock
 */

#include <mc.h>
#if	NMC > 0
#include <platforms.h>

#include <mach/boolean.h>
#include <mach/machine.h>
#include <kern/cpu_number.h>
#include <machine/machparam.h>		/* spl definitions */

#include <sys/types.h>
#include <sys/time.h>
#include <kern/time_out.h>
#include <chips/mc_clock.h>
#include <machine/machparam.h>

#ifdef	DECSTATION
#include <mach/mips/vm_types.h>
#include <mips/mips_cpu.h>
#include <mips/clock.h>
#endif	/*DECSTATION*/


#ifdef	DECSTATION
/*
 * Both the Pmax and the 3max implementations of the chip map
 * bytes of the chip's RAM to 32 bit words (low byte).
 * For convenience, we redefine here the chip's RAM layout
 * making padding explicit. 
 */

typedef struct {
	volatile unsigned char	mc_second;
								char pad0[3];
	volatile unsigned char	mc_alarm_second;
								char pad1[3];
	volatile unsigned char	mc_minute;
								char pad2[3];
	volatile unsigned char	mc_alarm_minute;
								char pad3[3];
	volatile unsigned char	mc_hour;
								char pad4[3];
	volatile unsigned char	mc_alarm_hour;
								char pad5[3];
	volatile unsigned char	mc_day_of_week;
								char pad6[3];
	volatile unsigned char	mc_day_of_month;
								char pad7[3];
	volatile unsigned char	mc_month;
								char pad8[3];
	volatile unsigned char	mc_year;
								char pad9[3];
	volatile unsigned char	mc_register_A;
								char pad10[3];
	volatile unsigned char	mc_register_B;
								char pad11[3];
	volatile unsigned char	mc_register_C;
								char pad12[3];
	volatile unsigned char	mc_register_D;
								char pad13[3];
	unsigned char		mc_non_volatile_ram[50 * 4];	/* unused */
} mc_clock_ram_t;

#else	/*DECSTATION*/

typedef mc_clock_t mc_clock_ram_t;	/* No padding needed */

#endif	/*DECSTATION*/


/*
 * Did we start the clock or not ?
 */
boolean_t todr_running = FALSE;		/* status */

boolean_t mc_new_century = FALSE;	/* "year" info overfloweth */

static int days_per_month[12] = {
	31, 28,	31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static unsigned int	mc_read();		/* forward */
static void		mc_wait_for_uip();

/*
 * Where is the chip's RAM mapped
 */
static mc_clock_ram_t	*rt_clock = (mc_clock_ram_t *)PHYS_TO_K1SEG(0x1d000000);

/*
 * (Auto?)Configuration
 */
void
set_clock_address(addr)
	vm_offset_t addr;
{
	rt_clock = (mc_clock_ram_t *)addr;
}


/*
 * Timed delays
 */
extern unsigned int cpu_speed;

void
config_delay(speed)
{
	/*
	 * This is just an initial estimate, later on with the clock
	 * running we'll tune it more accurately.
	 */
	cpu_speed = speed;
}

static volatile int had_intr;

accurate_config_delay(spllevel)
	unsigned		spllevel;
{
	register unsigned int	i, s;
	int			inner_loop_count;

	/* find "spllevel - 1" */
	s = spllevel | ((spllevel >> 1) & SR_INT_MASK);
	splx(s);

	/* wait till we have an interrupt pending */
	had_intr = 0;
	while (!had_intr)
		continue;

	had_intr = 0;
	i = delay_timing_function(1, &had_intr, &inner_loop_count);

	splx(spllevel);

	i *= hz;
	cpu_speed = i / (inner_loop_count * 1000000);

	/* roundup clock speed */
	i /= 100000;
	if ((i % 10) >= 5)
		i += 5;
	printf("Estimating CPU clock at %d Mhz\n", i / 10);
	if (isa_pmax() && cpu_speed != MC_DELAY_PMAX) {
		printf("%s\n", "This machine looks like a DEC 2100");
		machine_slot[cpu_number()].cpu_subtype = CPU_SUBTYPE_MIPS_R2000;
	}
}


/*
 *	Module implementation proper (exported functions)
 */
static int config_step = 3;

boolean_t
ackrtclock(spllevel)
{
	/*
	 * Interrupt flags are read-to-clear.
	 */
	if (config_step > 2)
		return (rt_clock->mc_register_C & MC_REG_C_IRQF);
	had_intr = (rt_clock->mc_register_C & MC_REG_C_IRQF) ? 1 : 0;
	if (config_step++ == 0)
		accurate_config_delay(spllevel);
}

void
startrtclock()
{
	/*
	 * Start real-time clock.
	 * All we should need to do is to enable interrupts, but
	 * since we do not know what OS last ran on this box
	 * we'll reset it all over again.  Just kidding..
	 */
	unsigned	unix_seconds_now;

	/*
	 * Check for battery backup power.  If we do not have it,
	 * warn the user (time will be bogus only after power up)
	 */
	if ((rt_clock->mc_register_D & MC_REG_D_VRT) == 0)
		printf("WARNING: clock batteries are low\n");

	/*
	 * Read the current time settings, check if the year info
	 * has been screwed up.  
	 */
	unix_seconds_now = mc_read();

	if (unix_seconds_now < (SECYR * (1990 - YRREF)))
		printf("The prom has clobbered the clock\n");

	time.tv_sec = (long)unix_seconds_now;
	resettodr();

	todr_running = TRUE;
}

void
stopclocks()
{
	/*
	 * Disable interrupts, but keep the chip running.
	 * Note we are called at splhigh and an interrupt
	 * might be pending already.
	 */

	ackrtclock(0);
	rt_clock->mc_register_B &= ~(MC_REG_B_UIE|MC_REG_B_AIE|MC_REG_B_PIE);
	todr_running = FALSE;
	config_step = 0;
}


/*
 * Set time-of-day.  Must be called at splhigh()
 */
void
resettodr()
{
	register mc_clock_ram_t *clock = rt_clock;
	register unsigned years, months, days, hours, minutes, seconds;
	register unsigned unix_seconds = time.tv_sec;
	int             frequence_selector, temp;
	int             bogus_hz = 0;

	/*
	 * Convert U*x time into absolute time 
	 */

	years = YRREF;
	while (1) {
		seconds = SECYR;
		if (LEAPYEAR(years))
			seconds += SECDAY;
		if (unix_seconds < seconds)
			break;
		unix_seconds -= seconds;
		years++;
	}

	months = 0;
	while (1) {
		seconds = days_per_month[months++] * SECDAY;
		if (months == 2 /* February */ && LEAPYEAR(years))
			seconds += SECDAY;
		if (unix_seconds < seconds)
			break;
		unix_seconds -= seconds;
	}

	days = unix_seconds / SECDAY;
	unix_seconds -= SECDAY * days++;

	hours = unix_seconds / SECHOUR;
	unix_seconds -= SECHOUR * hours;

	minutes = unix_seconds / SECMIN;
	unix_seconds -= SECMIN * minutes;

	seconds = unix_seconds;

	/*
	 * Trim years into 0-99 range.
	 */
	if ((years -= 1900) > 99) {
		years -= 100;
		mc_new_century = TRUE;
	}

	/*
	 * Check for "hot dates" 
	 */
	if (days >= 28 && days <= 30 &&
	    hours == 23 && minutes == 59 &&
	    seconds >= 58)
		seconds = 57;

	/*
	 * Select the interrupt frequency based on system params 
	 */
	switch (hz) {
	case 1024:
		frequence_selector = MC_BASE_32_KHz | MC_RATE_1024_Hz;
		break;
	case 512:
		frequence_selector = MC_BASE_32_KHz | MC_RATE_512_Hz;
		break;
	case 256:
		frequence_selector = MC_BASE_32_KHz | MC_RATE_256_Hz;
		break;
	case 128:
		frequence_selector = MC_BASE_32_KHz | MC_RATE_128_Hz;
		break;
	case 64:
default_frequence:
		frequence_selector = MC_BASE_32_KHz | MC_RATE_64_Hz;
		break;
	default:
		bogus_hz = hz;
		hz = 64;
		tick = 1000000 / 64;
		goto default_frequence;
	}

	/*
	 * Stop updates while we fix it 
	 */
	mc_wait_for_uip(clock);
	clock->mc_register_B = MC_REG_B_STOP;
	wbflush();

	/*
	 * Ack any pending interrupts 
	 */
	temp = clock->mc_register_C;

	/*
	 * Reset the frequency divider, in case we are changing it. 
	 */
	clock->mc_register_A = MC_BASE_RESET;

	/*
	 * Now update the time 
	 */
	clock->mc_second = seconds;
	clock->mc_minute = minutes;
	clock->mc_hour   = hours;
	clock->mc_day_of_month = days;
	clock->mc_month  = months;
	clock->mc_year   = years;

	/*
	 * Spec says the VRT bit can be validated, but does not say how. I
	 * assume it is via reading the register. 
	 */
	temp = clock->mc_register_D;

	/*
	 * Reconfigure the chip and get it started again 
	 */
	clock->mc_register_A = frequence_selector;
	clock->mc_register_B = MC_REG_B_24HM | MC_REG_B_DM | MC_REG_B_PIE;

	/*
	 * Print warnings, if we have to 
	 */
	if (bogus_hz != 0)
		printf("Unacceptable value (%d Hz) for hz, reset to %d Hz\n",
			bogus_hz, hz);
}


/*
 * Internal functions
 */

static void
mc_wait_for_uip(clock)
	mc_clock_ram_t *clock;
{
	while (clock->mc_register_A & MC_REG_A_UIP)
		delay(MC_UPD_MINIMUM >> 2);
}

static unsigned int
mc_read()
{
	/*
	 * Note we only do this at boot time
	 */
	register unsigned years, months, days, hours, minutes, seconds;
	register mc_clock_ram_t *clock = rt_clock;;

	/*
	 * If the chip is updating, wait 
	 */
	mc_wait_for_uip(clock);

	years = clock->mc_year;
	months = clock->mc_month;
	days = clock->mc_day_of_month;
	hours = clock->mc_hour;
	minutes = clock->mc_minute;
	seconds = clock->mc_second;

	/*
	 * Convert to Unix time 
	 */
	seconds += minutes * SECMIN;
	seconds += hours * SECHOUR;
	seconds += (days - 1) * SECDAY;
	if (months > 2 /* February */ && LEAPYEAR(years))
		seconds += SECDAY;
	while (months > 1)
		seconds += days_per_month[--months - 1];

	/*
	 * Note that in ten years from today (Aug,1990) the new century will
	 * cause the trouble that mc_new_century attempts to avoid. 
	 */
	if (mc_new_century)
		years += 100;
	years += 1900;	/* chip base year in YRREF's century */

	for (--years; years >= YRREF; years--) {
		seconds += SECYR;
		if (LEAPYEAR(years))
			seconds += SECDAY;
	}

	return seconds;
}


#endif	NMC > 0
