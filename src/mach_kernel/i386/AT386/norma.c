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
/*
 *	File:	i386/norma.c
 *
 *	Machine-dependent functions for NORMA.
 */

#include <norma_ether.h>
#include <kern/misc_protos.h>
#include <device/conf.h>
#include <device/net_status.h>
#include <device/io_req.h>
#include <sys/types.h>
#include <chips/busses.h>
#include <norma/ipc_ether.h>
#include <machine/ntoh.h>
#include <string.h>

#if	NORMA_ETHER
/*
 * Look for all possible network devices.
 */
struct netdevice *
netipc_get_netdevice(void)
{
	unsigned int count;
	unsigned long eaddr_l[2];
	int unit;
	struct dev_ops *dp;
	struct netdevice *np, *root;
	unsigned long addr;
	struct io_req ior;
	struct bus_device *device;
	device_t dev;

	spinlock_init(&ior.io_req_lock);
	root = (struct netdevice *)0;
	dev_search(dp) {
		if (dp->d_write == NULL_WRITE || dp->d_write == NO_WRITE ||
		    dp->d_getstat == NULL_GETS || dp->d_getstat == NO_GETS ||
		    (dp->d_read != NULL_READ && dp->d_read != NO_READ)) {
			continue;
		}

		for (device = bus_device_init; device->driver; device++) {
			if (!device->alive || strcmp(device->name, dp->d_name))
				continue;
			for (np = root;
			     np != (struct netdevice *)0; np = np->next)
				if (np->dev_ops == dp &&
				    np->unit == device->unit)
					break;
			if (np != (struct netdevice *)0) {
				/*
				 * This network device/unit has already been
				 * successfuly tested.
				 */
				continue;
			}

			if ((dev = device_lookup(device->name)) == DEVICE_NULL)
				continue;

			ior.io_device = dev;
			ior.io_unit = device->unit;
			ior.io_op = IO_OPEN;
			ior.io_mode = D_WRITE;
			ior.io_error = 0;

			if ((*dp->d_open)(device->unit,
					  D_WRITE, &ior) != D_SUCCESS) {
				/*
				 * Open failed. Try another one.
				 */
				device_deallocate(dev);
				continue;
			}
			count = sizeof (eaddr_l) / sizeof (eaddr_l[0]);
			if ((*dp->d_getstat)(device->unit, NET_ADDRESS,
					     (int *)eaddr_l,
					     &count) == D_SUCCESS) {
				/*
				 * Success!
				 */
				if ((np = (struct netdevice *)
				     kalloc(sizeof (struct netdevice)))
				    == (struct netdevice *)0)
					panic("netipc_get_netdevice");
				eaddr_l[0] = ntohl(eaddr_l[0]);
				eaddr_l[1] = ntohl(eaddr_l[1]);
				count *= sizeof (eaddr_l[0]);
				if (count > sizeof (np->addr))
					count = sizeof(np->addr);
				bcopy((char *)eaddr_l, (char *)np->addr, count);
				np->unit = device->unit;
				np->dev_ops = dp;
				np->dev = dev;
				np->next = root;
				root = np;
				continue;
			}
			/*
			 * Opened the wrong device. Close and try another one.
			 */
			if (dp->d_close)
				(*dp->d_close)(device->unit);
			device_deallocate(dev);
		}
	}
	return (root);
}
#endif	/* NORMA_ETHER */
