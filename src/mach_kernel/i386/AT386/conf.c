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
 * Revision 2.10.6.3  92/03/28  10:05:58  jeffreyh
 * 	Tapes must also be confiured with 16 partitions
 * 	to get the right unit number with rz macros.
 * 	[92/03/04            bernadat]
 * 
 * Revision 2.10.6.2  92/03/03  16:17:12  jeffreyh
 * 	Changes from TRUNK
 * 	[92/02/26  11:37:47  jeffreyh]
 * 
 * Revision 2.11  92/01/03  20:39:53  dbg
 * 	Added devinfo routine to scsi to accomodate MI change
 * 	that screwed up extra large writes.
 * 	[91/12/26  11:06:54  af]
 * 
 * Revision 2.10.6.1  92/02/18  18:51:26  jeffreyh
 * 	Suppressed old scsi driver
 * 	[91/09/27            bernadat]
 * 
 * 	Added high resolution clock device
 * 	(Jimmy Benjamin @ gr.orf.org)
 * 	[91/09/19            bernadat]
 * 
 * 	Added SCSI support
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.10  91/08/28  11:11:37  jsb
 * 	Fixed field-describing comment in dev_name_list definition.
 * 	[91/08/27  17:52:06  jsb]
 * 
 * 	Convert bsize entries to devinfo entries.  Add nodev entries for
 *	devices that don't support devinfo.
 * 	[91/08/15  18:43:13  jsb]
 * 
 * 	Add block size entries for hd and fd.
 * 	[91/08/12  17:32:55  dlb]
 * 
 * Revision 2.9  91/08/24  11:57:26  af
 * 	Added SCSI disks, tapes, and cpus.
 * 	[91/08/02  02:56:08  af]
 * 
 * Revision 2.8  91/05/14  16:22:01  mrt
 * 	Correcting copyright
 * 
 * Revision 2.7  91/02/14  14:42:13  mrt
 * 	Allow com driver and distinguish EtherLinkII from wd8003
 * 	[91/01/28  15:27:02  rvb]
 * 
 * Revision 2.6  91/02/05  17:16:44  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:42:38  mrt]
 * 
 * Revision 2.5  91/01/08  17:32:42  rpd
 * 	Support for get/set status on hd and fd.
 * 	Also fd has 64 minor devices per unit.
 * 	Switch wd8003 -> ns8390
 * 	[91/01/04  12:17:15  rvb]
 * 
 * Revision 2.4  90/10/01  14:23:02  jeffreyh
 * 	added wd8003 ethernet driver
 * 	[90/09/27  18:23:53  jeffreyh]
 * 
 * Revision 2.3  90/05/21  13:26:53  dbg
 * 	Add mouse, keyboard, IOPL devices.
 * 	[90/05/17            dbg]
 * 
 * Revision 2.2  90/05/03  15:41:34  dbg
 * 	Add 3c501 under name 'et'.
 * 	[90/04/27            dbg]
 * 
 * 	Created.
 * 	[90/02/20            dbg]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 * Device switch for i386 AT bus.
 */

#include <platforms.h>
#include <sys/types.h>
#include <device/tty.h>
#include <device/conf.h>
#include <device/if_ether.h>
#include <kern/clock.h>
#include <chips/busses.h>

#include <i386/rtclock_entries.h>

#define	rtclockname		"rtclock"
#define	timename		"time"

#include <hd.h>
#if	NHD > 0
#include <i386/AT386/hd_entries.h>
#define	hdname			"hd"
#endif	/* NHD > 0 */

#include <fd.h>
#if	NFD > 0
#include <i386/AT386/fd_entries.h>
#define	fdname			"fd"
#endif	/* NFD > 0 */

#include <aha.h>
#if	NAHA > 0
#include <scsi/scsi_defs.h>
#define rzname "sd"
#define	tzname "st"

#endif	/*NAHA > 0*/

#include <wt.h>
#if	NWT > 0
#define	wtname			"wt"
#endif	/* NWT > 0 */

#include <pc586.h>
#if	NPC586 > 0
#include <i386/AT386/if_pc586.h>
#include <i386/AT386/if_pc586_entries.h>
#define	pc586name		"pc"
#endif /* NPC586 > 0 */

#include <ns8390.h>
#if	NNS8390 > 0
#include <i386/AT386/if_ns8390_entries.h>
#define	ns8390wdname		"wd"
#define	ns8390elname		"el"
#endif /* NNS8390 > 0 */

#include <at3c501.h>
#if	NAT3C501 > 0
#include <i386/AT386/if_3c501_entries.h>
#define	at3c501name		"et"
#endif /* NAT3C501 > 0 */

#include <par.h>
#if	NPAR > 0
#define	parname		"par"
#endif /* NPAR > 0 */

#include <i386/AT386/kd.h>
#include <i386/AT386/kdsoft.h>
#define	kdname			"kd"

#include <com.h>
#if	NCOM > 0
#include <i386/AT386/com_entries.h>
#define	comname			"com"
#endif	/* NCOM > 0 */

#include <lpr.h>
#if	NLPR > 0
#define	lprname			"lpr"
#endif	/* NLPR > 0 */

#include <qd.h>
#if	NQD > 0
#define	qdname			"qd"
#endif	/* NQD > 0 */

#include <blit.h>
#if NBLIT > 0
#include <i386/AT386/blitvar.h>
#define	blitname		"blit"
#endif

#include <i386/AT386/kbd_entries.h>
#define	kbdname			"kbd"

#include <i386/AT386/kd_mouse_entries.h>
#define	mousename		"mouse"

#include <i386/AT386/iopl_entries.h>
#define	ioplname		"iopl"

#include <vga.h>
#include <i386/AT386/vga_entries.h>
#define vganame			"vga"

#include <hi_res_clock.h>
#include <cpus.h>
#if	HI_RES_CLOCK && NCPUS > 1
#include <i386/hi_res_clock.h>
#define hi_res_clk_name         "hi_res_clock"
#endif  /* HI_RES_CLOCK && NCPUS > 1 */

#include <gprof.h>
#if     GPROF
#define gprofname               "gprof"
#endif

#include <test_device.h>
#if	NTEST_DEVICE > 0
#include <device/test_device_entries.h>
#define testdevname		"test_device"
#endif	/* NTEST_DEVICE > 0 */

#include <i386/AT386/misc_protos.h>

/*
 * List of devices - console must be at slot 0
 */
struct dev_ops	dev_name_list[] =
{
	/*name,		open,		close,		read,
	  write,	getstat,	setstat,	mmap,
	  async_in,	reset,		port_death,	subdev,
	  dev_info */

	{ kdname,	kdopen,		kdclose,	kdread,
	  kdwrite,	kdgetstat,	kdsetstat,	kdmmap,
	  NO_ASYNC,	NULL_RESET,	kdportdeath,	0,
	  NO_DINFO },

	{ timename,	NULL_OPEN,	NULL_CLOSE,	NULL_READ,
	  NULL_WRITE,	NULL_GETS,	NULL_SETS,	utime_map,
	  NO_ASYNC,	NULL_RESET,	NULL_DEATH,	0,
	  NO_DINFO },

	{ rtclockname,	NULL_OPEN,	NULL_CLOSE,	NULL_READ,
	  NULL_WRITE,	NULL_GETS,	NULL_SETS,	rtclock_map,
	  NO_ASYNC,	NULL_RESET,	NULL_DEATH,	0,
	  NO_DINFO },

#if	NHD > 0
	{ hdname,	hdopen,		hdclose,	hdread,
	  hdwrite,	hdgetstat,	hdsetstat,	NULL_MMAP,
	  NO_ASYNC,	NULL_RESET,	NULL_DEATH,	16,
	  hddevinfo },
#endif	/* NHD > 0 */

#if	NFD > 0
	{ fdname,	fdopen,		fdclose,	fdread,
	  fdwrite,	fdgetstat,	fdsetstat,	NULL_MMAP,
	  NO_ASYNC,	NULL_RESET,	NULL_DEATH,	64,
	  fddevinfo },
#endif	/* NFD > 0 */

#if	NAHA > 0
	{ rzname,	rz_open,	rz_close,	rz_read,
	  rz_write,	rz_get_status,	rz_set_status,	NO_MMAP,
	  NO_ASYNC,	NULL_RESET,	NULL_DEATH,	16,
	  rz_devinfo },

	{ tzname,	rz_open,	rz_close,	rz_read,
	  rz_write,	rz_get_status,	rz_set_status,	NO_MMAP,
	  NO_ASYNC,	NULL_RESET,	NULL_DEATH,	16,
	  NO_DINFO },

#endif	/*NAHA > 0*/

#if	NWT > 0
	{ wtname,	wtopen,		wtclose,	wtread,
	  wtwrite,	NULL_GETS,	NULL_SETS,	NULL_MMAP,
	  NO_ASYNC,	NULL_RESET,	NULL_DEATH,	0,
	  NO_DINFO },
#endif	/* NWT > 0 */

#if	NPC586 > 0
	{ pc586name,	pc586open,	NULL_CLOSE,	NULL_READ,
	  pc586output,	pc586getstat,	pc586setstat,	NULL_MMAP,
	  pc586setinput,NULL_RESET,	NULL_DEATH, 	0,
	  NO_DINFO },
#endif

#if	NAT3C501 > 0
	{ at3c501name,	at3c501open,	NULL_CLOSE,	NULL_READ,
	  at3c501output,at3c501getstat,	at3c501setstat,	NULL_MMAP,
	  at3c501setinput, NULL_RESET,	NULL_DEATH, 	0,
	  NO_DINFO },
#endif

#if	NNS8390 > 0
	{ ns8390wdname,	wd8003open,	NULL_CLOSE,	NULL_READ,
	  ns8390output, ns8390getstat,	ns8390setstat,	NULL_MMAP,
	  ns8390setinput, NULL_RESET,	NULL_DEATH, 	0,
	  NO_DINFO },

	{ ns8390elname,	eliiopen,	NULL_CLOSE,	NULL_READ,
	  ns8390output, ns8390getstat,	ns8390setstat,	NULL_MMAP,
	  ns8390setinput, NULL_RESET,	NULL_DEATH, 	0,
	  NO_DINFO },
#endif

#if	NPAR > 0
	{ parname,	paropen,	NULL_CLOSE,	NULL_READ,
	  paroutput,	pargetstat,	parsetstat,	NULL_MMAP,
	  parsetinput,	NULL_RESET,	NULL_DEATH, 	0,
	  NO_DINFO },
#endif

#if	NCOM > 0
	{ comname,	comopen,	comclose,	comread,
	  comwrite,	comgetstat,	comsetstat,	NO_MMAP,
	  NO_ASYNC,	NULL_RESET,	comportdeath,	0,
	  NO_DINFO },
#endif

#if	NLPR > 0
	{ lprname,	lpropen,	lprclose,	lprread,
	  lprwrite,	lprgetstat,	lprsetstat,	NO_MMAP,
	  NO_ASYNC,	NULL_RESET,	lprportdeath,	0,
	  NO_DINFO },
#endif

#if	NBLIT > 0
	{ blitname,	blitopen,	blitclose,	NO_READ,
	  NO_WRITE,	blit_get_stat,	NO_SETS,	blitmmap,
	  NO_ASYNC,	NO_RESET,	NO_DEATH,	0,
	  NO_DINFO },
#endif

	{ mousename,	mouseopen,	mouseclose,	mouseread,
	  NO_WRITE,	NULL_GETS,	NULL_SETS,	NO_MMAP,
	  NO_ASYNC,	NULL_RESET,	NULL_DEATH,	0,
	  NO_DINFO },

	{ kbdname,	kbdopen,	kbdclose,	kbdread,
	  NO_WRITE,	kbdgetstat,	kbdsetstat,	NO_MMAP,
	  NO_ASYNC,	NULL_RESET,	NULL_DEATH,	0,
	  NO_DINFO },

	{ ioplname,	ioplopen,	ioplclose,	NO_READ,
	  NO_WRITE,	NO_GETS,	NO_SETS,	ioplmmap,
	  NO_ASYNC,	NULL_RESET,	NULL_DEATH,	0,
	  NO_DINFO },

#if	NVGA > 0
	/* must be AFTER kd */
	{ vganame,	vgaopen,	vgaclose,	NO_READ,
	  NO_WRITE,	vgagetstat,	vgasetstat,	vgammap,
	  NO_ASYNC,	NULL_RESET,	NULL_DEATH,	0,
	  NO_DINFO },
#endif

#if	HI_RES_CLOCK && NCPUS > 1
        { hi_res_clk_name, NULL_OPEN,	NULL_CLOSE,	NO_READ,
          NO_WRITE,        NULL_GETS,	NULL_SETS,	hi_res_clk_mmap,
          NO_ASYNC,        NULL_RESET,	NULL_DEATH,	0,
	  NO_DINFO },
#endif  /* HI_RES_CLOCK && NCPUS > 1 */

#if     GPROF
        { gprofname,    gprofopen,      gprofclose,     gprofread,
          gprofwrite,   NULL_GETS,      NULL_SETS,      NULL_MMAP,
          NO_ASYNC,     NULL_RESET,     NULL_DEATH,     0,
          NO_DINFO },
#endif

#if	NTEST_DEVICE > 0
	{ testdevname,	testdev_open,	testdev_close,	testdev_read,
	  testdev_write,testdev_getstat,testdev_setstat,testdev_mmap,
	  testdev_async_in,testdev_reset,testdev_port_death,	0,
	  testdev_dev_info },
#endif	/* NTEST_DEVICE > 0 */

};
int	dev_name_count = sizeof(dev_name_list)/sizeof(dev_name_list[0]);

/*
 * Indirect list.
 */
struct dev_indirect dev_indirect_list[] = {

	/* console */
	{ "console",		&dev_name_list[0],	0 },

	/* boot device */
	{ "boot_device",	0,			0 }

};
int	dev_indirect_count = sizeof(dev_indirect_list)
				/ sizeof(dev_indirect_list[0]);

/*
 * Clock device subsystem configuration. The clock_list[]
 * table contains the clock structures for all clocks in
 * the system.
 */

extern	struct clock_ops	rtc_ops;
extern	struct clock_ops	bbc_ops;

#include <cyctm.h>
#if	NCYCTM > 0
extern struct clock_ops		cyctm05_ops;
#endif

/*
 * List of clock devices.
 */
struct	clock	clock_list[] = {

	/* REALTIME_CLOCK */
	{ &rtc_ops,	0,		0,		0 },

	/* BATTERY_CLOCK */
	{ &bbc_ops,	0,		0,		0 },

	/* HIGHRES_CLOCK */
#if	NCYCTM > 0
	{ &cyctm05_ops,	0,		0,		0 },
#else
	{ 0,		0,		0,		0 },
#endif
};
int	clock_count = sizeof(clock_list) / sizeof(clock_list[0]);
