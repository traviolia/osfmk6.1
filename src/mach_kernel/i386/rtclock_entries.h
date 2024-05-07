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

extern int		rtc_config(void);
extern int		rtc_init(void);
extern kern_return_t	rtc_gettime(
				tvalspec_t		* curtime);
extern kern_return_t	rtc_settime(
				tvalspec_t		* curtime);
extern kern_return_t	rtc_getattr(
				clock_flavor_t		flavor,
				clock_attr_t		ttr,
				mach_msg_type_number_t	* count);
extern kern_return_t	rtc_setattr(
				clock_flavor_t		flavor,
				clock_attr_t		ttr,
				mach_msg_type_number_t	count);
extern kern_return_t	rtc_maptime(
				ipc_port_t		* pager);
extern void		rtc_setalrm(
				tvalspec_t		* alarmtime);
extern void		rtclock_reset(void);
extern int		rtclock_intr(void);
extern vm_offset_t	rtclock_map(
				dev_t			dev,
				vm_offset_t		off,
				int			prot);
