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
 * Revision 2.9.9.2  92/03/28  10:05:40  jeffreyh
 * 	Allow boot from floppy device
 * 	[92/03/04            bernadat]
 * 
 * Revision 2.9.9.1  92/02/18  18:49:22  jeffreyh
 * 	Get root partition from boot args
 * 	[91/07/04            bernadat]
 * 
 * 	Support for SCSI disk
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.9  91/06/19  11:55:36  rvb
 * 	cputypes.h->platforms.h
 * 	[91/06/12  13:45:24  rvb]
 * 
 * Revision 2.8  91/05/14  16:16:26  mrt
 * 	Correcting copyright
 * 
 * Revision 2.7  91/05/08  12:42:00  dbg
 * 	Include platforms.h to get CPU names.
 * 	[91/03/21            dbg]
 * 
 * Revision 2.6  91/02/05  17:14:38  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:37:59  mrt]
 * 
 * Revision 2.5  90/12/04  14:46:35  jsb
 * 	iPSC2 -> iPSC386.
 * 	[90/12/04  11:19:36  jsb]
 * 
 * Revision 2.4  90/11/24  15:14:53  jsb
 * 	Added missing newline.
 * 	[90/11/24  11:45:51  jsb]
 * 
 * Revision 2.3  90/09/23  16:47:18  jsb
 * 	If iPSC386, root device is sd (scsi), not hd.
 * 
 * Revision 2.2  90/05/03  15:37:31  dbg
 * 	Created.
 * 	[90/02/20            dbg]
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

#include <platforms.h>
#include <strings.h>
#include <mach/boolean.h>
#include <mach/machine/vm_types.h>
#include <sys/types.h>
#include <sys/reboot.h>
#include <device/conf.h>
#include <device/ds_routines.h>
#include <kern/misc_protos.h>
#include <i386/misc_protos.h>

extern int boottype;

/*
 * Get root device.  Temporarily hard-coded.
 */
char root_string[5];

#if	iPSC386
char *root_name = "sd0a";
get_root_device()
{
	printf("root on %s\n", root_name);
}
#else	/* iPSC386 */
char *root_name = root_string;

/*
 *	 (4) (4) (4) (4)  (8)     (8)
 *	--------------------------------
 *	|MA | AD| CT| UN| PART  | TYPE |
 *	--------------------------------
 */

char *maj_devs[4] = { "hd", "fd", 0, "sd"};

void
get_root_device(void)
{
	int type = boottype & 0xff;
	int part = (boottype >> 8) & 0xff;
	int unit = (boottype >> 16) & 0x7;
	boolean_t ask = (boothowto & RB_ASKNAME);

	if ( (type > sizeof (maj_devs) / sizeof (char *)) ||
	     maj_devs[type] == 0) {
	}

	root_string[0] = maj_devs[type][0];
	root_string[1] = maj_devs[type][1];
	root_string[2] = '0' + unit;
	root_string[3] = 'a' + part;
	root_string[4] = 0;

	for (;;) {
		char name[5];
		dev_ops_t boot_dev_ops;

		if (ask) {
			printf("root device [%s]? ", root_name);
			safe_gets(name, sizeof name);
			if (*name != 0)
				strcpy(root_name, name);
		}
		if (dev_name_lookup(root_name, &boot_dev_ops, &unit)) {
			dev_set_indirection("boot_device", boot_dev_ops, unit);
			break;
		}
		ask = TRUE;
	}
	printf("boot from %s\n", root_name);
}

#include <mach_kdb.h>

void boot_strcat(
		char		*s1,
		unsigned int	s1_length,	   
		const char	*s2);


/*
 * Safe version of strcat, checks if s2 can fit in s1[0]-s1[s1_szl]
 */

void
boot_strcat(
	char		*s1,
	unsigned int	s1_sz,	   
	const char	*s2)
{
	if (strlen(s1)+strlen(s2)+2 > s1_sz)
		panic("machine_boot_info: strcat() would fail\n");
	else {
	  	if (strlen(s1))
	  		strcat(s1, " ");
		strcat(s1, s2);
	}
}


char *
machine_boot_info(
	char		*buf,
	vm_size_t	buf_len)
{
	extern char *boot_string;
	char *p, *q;

	*buf = 0;

	/*
	 * First copy the root name
	 */
	boot_strcat(buf, buf_len, root_name);

	/*
	 * Pass boothowto server flags since they are not in boot_string
	 */

#if	MACH_KDB
	if (boothowto & RB_KDB)
		boot_strcat(buf, buf_len, "-d");
#endif	/* MACH_KDB */

	if (boothowto & RB_ASKNAME)
		boot_strcat(buf, buf_len, "-aq");
	if (boothowto & RB_SINGLE)
		boot_strcat(buf, buf_len, "-s");
	if (boothowto & RB_HALT)
		boot_strcat(buf, buf_len, "-h");


	/*
	 * Add boot_string
	 * syntax is hd(0,a)/kernel:server args\n")
	 * OSF1 server wants args first, move hd(0,a)/kernel:server arg at end
	 * (boostrap needs hd(0,a)/kernel:server prefix)
	 */
	for (p = boot_string; *p && *p == ' '; p++)
		continue; 				/* Skip spaces */

	if (*p == '-') {
		/* no hd(0,a)/kernel:server prefix */
	  	boot_strcat(buf, buf_len, q);	        /* Copy flags */
	} else {
		for (q = p; *q && *q != ' '; q++)
			continue; 			/* Find flags */
		if (*q != '\0') {
			boot_strcat(buf, buf_len, q);	/* Copy flags */
			*q = '\0';
			boot_strcat(buf, buf_len, p);	/* Copy bootfiles */
			*q = ' ';			/* Restore separator */
		} else
			boot_strcat(buf, buf_len, p);	/* Copy bootfiles */
	}
	return(buf);
}
#endif	/* iPSC386 */
