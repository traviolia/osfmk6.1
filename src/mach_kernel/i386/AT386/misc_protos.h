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

#include <chips/busses.h>		/* for struct bus_device */
#include <sys/types.h>			/* for dev_t */
#include <mach/machine/vm_types.h>	/* for vm_offset_t */

extern void		setup_main(void);
extern void		i386_init(void);
extern void		cninit(void);
extern void		machine_init(void);
extern void		probeio(void);
extern void		clock_config(void);
extern void		kdreboot(void);
extern void		machine_startup(void);
extern vm_offset_t	utime_map(
				dev_t		dev,
				vm_offset_t	off,
				int		prot);
extern void		take_dev_irq(
				struct bus_device *dev);
extern void		kdb_kintr(void);
extern void		kdb_console(void);
extern void		kgdb_kintr(void);
extern void		feep(void);
extern void		pause(void);
extern void		bcopy16(
				char		* from,
				char		* to,
				int		count);
extern void		himem_init(void);
