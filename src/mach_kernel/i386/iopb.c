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
 * Revision 2.3.4.2  92/06/24  17:58:24  jeffreyh
 * 	Integrated Mt Xinu fix from slatteng@mtxinu.COM for X terminal
 * 	support
 * 	[92/06/09            sjs]
 * 
 * Revision 2.3.4.1  92/03/03  16:15:58  jeffreyh
 * 	Pick up changes from TRUNK
 * 	[92/02/26  11:27:06  jeffreyh]
 * 
 * Revision 2.4  92/01/03  20:07:32  dbg
 * 	Fix locking.  Move all interface routines here.
 * 	[91/11/07            dbg]
 * 
 * Revision 2.3  91/05/14  16:09:50  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/05/08  12:38:27  dbg
 * 	Created.
 * 	[91/03/21            dbg]
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
 * Code to manipulate IO permission bitmaps.
 */

#include <mach/boolean.h>
#include <mach/kern_return.h>

#include <ipc/ipc_port.h>

#include <kern/kalloc.h>
#include <kern/rtalloc.h>
#include <kern/lock.h>
#include <kern/queue.h>
#include <kern/thread.h>
#include <kern/misc_protos.h>

#include <device/device_types_kernel.h>

#include <i386/io_port.h>
#include <i386/iopb.h>
#include <i386/seg.h>
#include <i386/iopb_entries.h>

/*
 * A set of ports for an IO device.
 */
struct io_port {
	device_t	device;		/* Mach device */
	queue_chain_t	dev_list;	/* link in device list */
	queue_chain_t	io_use_list;	/* List of threads that use it */
	io_reg_t	*io_port_list;	/* list of IO ports that use it */
					/* list ends with IO_REG_NULL */
};
typedef struct io_port	*io_port_t;

/*
 * Lookup table for device -> io_port mapping
 * (a linked list - I don't expect too many)
 */
queue_head_t	device_to_io_port_list;

/*
 * Cross-reference:
 *	all threads that have IO ports mapped
 *	all IO ports that have threads mapped
 */
struct io_use {
	queue_chain_t	psq;	/* Links from port set */
	queue_chain_t	tsq;	/* links from tss */
	io_port_t	ps;	/* Port set */
	iopb_tss_t	ts;	/* Task segment */
};
typedef	struct io_use	*io_use_t;

/*
 * Big lock for the whole mess.
 */
spinlock_t	iopb_lock;

/* Forward */

extern void		io_bitmap_init(
				isa_iopb		bp);
extern void		io_bitmap_set(
				isa_iopb		bp,
				io_reg_t		*bit_list);
extern void		io_bitmap_clear(
				isa_iopb		bp,
				io_reg_t		*bit_list);
extern io_port_t	device_to_io_port_lookup(
				device_t		device);
extern void		io_tss_init(
				iopb_tss_t		io_tss);


/*
 * Initialize the package.
 */
void
iopb_init(void)
{
	queue_init(&device_to_io_port_list);
	spinlock_init(&iopb_lock);
}

/*
 * Initialize bitmap (set all bits to OFF == 1)
 */
void
io_bitmap_init(
	isa_iopb	bp)
{
	register unsigned char *b;
	register int	s;

	s = sizeof(isa_iopb);
	b = bp;

	do {
	    *b++ = ~0;
	} while (--s >= 0);
}

/*
 * Set selected bits in bitmap to ON == 0
 */
void
io_bitmap_set(
	isa_iopb	bp,
	io_reg_t	*bit_list)
{
	io_reg_t	io_bit;

	while ((io_bit = *bit_list++) != IO_REG_NULL) {
	    bp[io_bit>>3] &= ~(1 << (io_bit & 0x7));
	}
}

/*
 * Set selected bits in bitmap to OFF == 1
 */
void
io_bitmap_clear(
	isa_iopb	bp,
	io_reg_t	*bit_list)
{
	io_reg_t	io_bit;

	while ((io_bit = *bit_list++) != IO_REG_NULL) {
	    bp[io_bit>>3] |= (1 << (io_bit & 0x7));
	}
}

/*
 * Lookup an io-port set by device
 */
io_port_t
device_to_io_port_lookup(
	device_t	device)
{
	register io_port_t io_port;

	queue_iterate(&device_to_io_port_list, io_port, io_port_t, dev_list) {
	    if (io_port->device == device) {
		return io_port;
	    }
	}
	return 0;
}

/*
 * [exported]
 * Create an io_port set
 */
void
io_port_create(
	device_t	device,
	io_reg_t	*io_port_list)
{
	register io_port_t io_port, old_io_port;

	io_port = (io_port_t) kalloc(sizeof(struct io_port));

	spinlock_lock(&iopb_lock);
	if (device_to_io_port_lookup(device) != 0) {
	    spinlock_unlock(&iopb_lock);
	    kfree((vm_offset_t) io_port, sizeof(struct io_port));
	    return;
	}

	io_port->device = device;
	queue_init(&io_port->io_use_list);
	io_port->io_port_list = io_port_list;

	/*
	 * Enter in lookup list.
	 */
	queue_enter(&device_to_io_port_list, io_port, io_port_t, dev_list);

	spinlock_unlock(&iopb_lock);
}

/*
 * [exported]
 * Destroy an io port set, removing any IO mappings.
 */
void
io_port_destroy(
	device_t	device)
{
	io_port_t	io_port;
	io_use_t	iu;

	spinlock_lock(&iopb_lock);
	io_port = device_to_io_port_lookup(device);
	if (io_port == 0) {
	    spinlock_unlock(&iopb_lock);
	    return;
	}

	queue_iterate(&io_port->io_use_list, iu, io_use_t, psq) {
	    iopb_tss_t	io_tss;
	    io_tss = iu->ts;
	    io_bitmap_clear(io_tss->bitmap, io_port->io_port_list);
	    queue_remove(&io_tss->io_port_list, iu, io_use_t, tsq);
	}
	queue_remove(&device_to_io_port_list, io_port, io_port_t, dev_list);
	spinlock_unlock(&iopb_lock);

	while (!queue_empty(&io_port->io_use_list)) {
	    iu = (io_use_t) queue_first(&io_port->io_use_list);
	    queue_remove(&io_port->io_use_list, iu, io_use_t, psq);
	    kfree((vm_offset_t)iu, sizeof(struct io_use));
	}

	kfree((vm_offset_t)io_port, sizeof(struct io_port));
}

/*
 * Initialize an IO TSS.
 */
void
io_tss_init(
	iopb_tss_t	io_tss)
{
	vm_offset_t	addr = (vm_offset_t) io_tss;
	vm_size_t	size = (char *)&io_tss->barrier - (char *)io_tss;

	bzero((char *)&io_tss->tss, sizeof(struct i386_tss));
	io_tss->tss.io_bit_map_offset
			= (char *)&io_tss->bitmap - (char *)io_tss;
	io_tss->tss.ss0 = KERNEL_DS;
	io_bitmap_init(io_tss->bitmap);
	io_tss->barrier = ~0;
	queue_init(&io_tss->io_port_list);
	io_tss->iopb_desc[0] = ((size-1) & 0xffff)
		| ((addr & 0xffff) << 16);
	io_tss->iopb_desc[1] = ((addr & 0x00ff0000) >> 16)
		| ((ACC_TSS|ACC_PL_K|ACC_P) << 8)
		| ((size-1) & 0x000f0000)
		| (addr & 0xff000000);
}

/*
 * [exported]
 * Create an IOPB_TSS
 */
iopb_tss_t
iopb_create(void)
{
	register iopb_tss_t ts;

	ts = (iopb_tss_t) kalloc(sizeof (struct iopb_tss));
	io_tss_init(ts);
	return (ts);
}

/*
 * [exported]
 * Destroy an IOPB_TSS
 */
void
iopb_destroy(
	iopb_tss_t	io_tss)
{
	io_use_t	iu;
	io_port_t	io_port;

	spinlock_lock(&iopb_lock);

	queue_iterate(&io_tss->io_port_list, iu, io_use_t, tsq) {
	    io_port = iu->ps;
	    /* skip bitmap clear - entire bitmap will vanish */
	    queue_remove(&io_port->io_use_list, iu, io_use_t, psq);
	}

	spinlock_unlock(&iopb_lock);

	while (!queue_empty(&io_tss->io_port_list)) {
	    iu = (io_use_t) queue_first(&io_tss->io_port_list);
	    queue_remove(&io_tss->io_port_list, iu, io_use_t, tsq);
	    kfree((vm_offset_t)iu, sizeof(struct io_use));
	}

	kfree((vm_offset_t)io_tss, sizeof(struct iopb_tss));
}

/*
 * Add an IO mapping to a thread.
 */
kern_return_t
i386_io_port_add(
	thread_t	thread,
	device_t	device)
{
	pcb_t		pcb;
	iopb_tss_t	io_tss, new_io_tss;
	io_port_t	io_port;
	io_use_t	iu, old_iu;

	if (thread == THREAD_NULL
	 || device == DEVICE_NULL)
	    return KERN_INVALID_ARGUMENT;

	pcb = thread->top_act->mact.pcb;

	new_io_tss = 0;
	iu = (io_use_t) kalloc(sizeof(struct io_use));

    Retry:
	spinlock_lock(&iopb_lock);

	/* find the io_port_t for the device */
	io_port = device_to_io_port_lookup(device);
	if (io_port == 0) {
	    /*
	     * Device does not have IO ports available.
	     */
	    spinlock_unlock(&iopb_lock);
	    if (new_io_tss)
		kfree((vm_offset_t)new_io_tss, sizeof(struct iopb_tss));
	    kfree((vm_offset_t) iu, sizeof(struct io_use));
	    return KERN_INVALID_ARGUMENT;
	}

	/* Have the IO port. */

	/* Make sure the thread has a TSS. */

	spinlock_lock(&pcb->lock);
	io_tss = pcb->ims.io_tss;
	if (io_tss == 0) {
	    if (new_io_tss == 0) {
		/*
		 * Allocate an IO-tss.
		 */
		spinlock_unlock(&pcb->lock);
		spinlock_unlock(&iopb_lock);

		new_io_tss = (iopb_tss_t) kalloc(sizeof(struct iopb_tss));
		io_tss_init(new_io_tss);

		goto Retry;
	    }
	    io_tss = new_io_tss;
	    pcb->ims.io_tss = io_tss;
	    new_io_tss = 0;
	}

	/*
	 * Have io_port and io_tss.
	 * See whether device is already mapped.
	 */
	queue_iterate(&io_tss->io_port_list, old_iu, io_use_t, tsq) {
	    if (old_iu->ps == io_port) {
		/*
		 * Already mapped.
		 */
		spinlock_unlock(&pcb->lock);
		spinlock_unlock(&iopb_lock);

		kfree((vm_offset_t)iu, sizeof(struct io_use));
		if (new_io_tss)
		    kfree((vm_offset_t)new_io_tss, sizeof(struct iopb_tss));
		return KERN_SUCCESS;
	    }
	}

	/*
	 * Add mapping.
	 */
	iu->ps = io_port;
	iu->ts = io_tss;
	queue_enter(&io_port->io_use_list, iu, io_use_t, psq);
	queue_enter(&io_tss->io_port_list, iu, io_use_t, tsq);
	io_bitmap_set(io_tss->bitmap, io_port->io_port_list);

	spinlock_unlock(&pcb->lock);
	spinlock_unlock(&iopb_lock);

	if (new_io_tss)
	    kfree((vm_offset_t)new_io_tss, sizeof(struct iopb_tss));
	return KERN_SUCCESS;

}

/*
 * Remove an IO mapping from a thread.
 */
kern_return_t
i386_io_port_remove(
	thread_t	thread,
	device_t	device)
{
	pcb_t		pcb;
	iopb_tss_t	io_tss;
	io_port_t	io_port;
	io_use_t	iu;

	if (thread == THREAD_NULL
	 || device == DEVICE_NULL)
	    return KERN_INVALID_ARGUMENT;

	pcb = thread->top_act->mact.pcb;

	spinlock_lock(&iopb_lock);

	/* find the io_port_t for the device */

	io_port = device_to_io_port_lookup(device);
	if (io_port == 0) {
	    /*
	     * Device does not have IO ports available.
	     */
	    spinlock_unlock(&iopb_lock);
	    return KERN_INVALID_ARGUMENT;
	}

	spinlock_lock(&pcb->lock);
	io_tss = pcb->ims.io_tss;
	if (io_tss == 0) {
	    spinlock_unlock(&pcb->lock);
	    spinlock_unlock(&iopb_lock);
	    return KERN_INVALID_ARGUMENT;	/* not mapped */
	}

	/*
	 * Find the mapping.
	 */
	queue_iterate(&io_tss->io_port_list, iu, io_use_t, tsq) {
	    if (iu->ps == io_port) {
		/*
		 * Found mapping.  Remove it.
		 */
		io_bitmap_clear(io_tss->bitmap, io_port->io_port_list);

		queue_remove(&io_port->io_use_list, iu, io_use_t, psq);
		queue_remove(&io_tss->io_port_list, iu, io_use_t, tsq);

		spinlock_unlock(&pcb->lock);
		spinlock_unlock(&iopb_lock);

		kfree((vm_offset_t)iu, sizeof(struct io_use));

		return KERN_SUCCESS;
	    }
	}

	/*
	 * No mapping.
	 */
	return KERN_INVALID_ARGUMENT;
}

/*
 * Return the IO ports mapped into a thread.
 */

kern_return_t
i386_io_port_list(thread, list, list_count)
	thread_t	thread;
	device_t	**list;
	unsigned int	*list_count;
{
	register pcb_t	pcb;
	register iopb_tss_t io_tss;
	unsigned int	count, alloc_count;
	device_t	*devices;
	vm_size_t	size_needed, size;
	vm_offset_t	addr;
	int		i;
	boolean_t rt = FALSE; /* ### This boolean is FALSE, because there
			       * currently exists no mechanism to determine
			       * whether or not the reply port is an RT port
			       */

	if (thread == THREAD_NULL)
	    return KERN_INVALID_ARGUMENT;

	pcb = thread->top_act->mact.pcb;

	alloc_count = 16;		/* a guess */

	do {
	    size_needed = alloc_count * sizeof(ipc_port_t);
	    if (size_needed <= size)
		break;

	    if (size != 0)
		FREE(addr, size, rt);

	    assert(size_needed > 0);
	    size = size_needed;

	    addr = ALLOC(size, rt);
	    if (addr == 0)
		return KERN_RESOURCE_SHORTAGE;

	    devices = (device_t *)addr;
	    count = 0;

	    spinlock_lock(&iopb_lock);
	    spinlock_lock(&pcb->lock);
	    io_tss = pcb->ims.io_tss;
	    if (io_tss != 0) {
		register io_use_t iu;

		queue_iterate(&io_tss->io_port_list, iu, io_use_t, tsq) {
		    if (++count < alloc_count) {
			*devices = iu->ps->device;
			device_reference(*devices);
			devices++;
		    }
		}
	    }
	    spinlock_unlock(&pcb->lock);
	    spinlock_unlock(&iopb_lock);
	} while (count > alloc_count);

	if (count == 0) {
	    /*
	     * No IO ports
	     */
	    *list = 0;
	    *list_count = 0;

	    if (size != 0)
		FREE(addr, size, rt);
	}
	else {
	    /*
	     * If we allocated too much, must copy.
	     */
	    size_needed = count * sizeof(ipc_port_t);
	    if (size_needed < size) {
		vm_offset_t	new_addr;

		new_addr = ALLOC(size_needed, rt);
		if (new_addr == 0) {
		    for (i = 0; i < count; i++)
			device_deallocate(devices[i]);
		    FREE(addr, size, rt);
		    return KERN_RESOURCE_SHORTAGE;
		}

		bcopy((char *)addr, (char *)new_addr, size_needed);
		FREE(addr, size, rt);
		devices = (device_t *)new_addr;
	    }

	    for (i = 0; i < count; i++)
		((ipc_port_t *)devices)[i] =
			convert_device_to_port(devices[i]);
	}
	*list = devices;
	*list_count = count;

	return KERN_SUCCESS;
}

/*
 * Check whether an IO device is mapped to a particular thread.
 * Used to support the 'iopl' device automatic mapping.
 */
boolean_t
iopb_check_mapping(
	thread_t	thread,
	device_t	device)
{
	pcb_t		pcb;
	io_port_t	io_port;
	io_use_t	iu;

	pcb = thread->top_act->mact.pcb;

	spinlock_lock(&iopb_lock);

	/* Find the io port for the device */

	io_port = device_to_io_port_lookup(device);
	if (io_port == 0) {
	    spinlock_unlock(&iopb_lock);
	    return FALSE;
	}

	/* Look up the mapping in the device`s mapping list. */

	queue_iterate(&io_port->io_use_list, iu, io_use_t, psq) {
	    if (iu->ts == pcb->ims.io_tss) {
		/*
		 * Device is mapped.
		 */
		spinlock_unlock(&iopb_lock);
		return TRUE;
	    }
	}
	spinlock_unlock(&iopb_lock);
	return FALSE;
}
