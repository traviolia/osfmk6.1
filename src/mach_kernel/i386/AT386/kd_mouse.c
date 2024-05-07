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
 * Revision 2.8.5.4  92/06/24  17:59:27  jeffreyh
 * 	Updated to new at386_io_lock() interface.
 * 	[92/06/03            bernadat]
 * 
 * Revision 2.8.5.3  92/04/30  11:56:49  bernadat
 * 	Changes for Systempro support
 * 	[92/04/08            bernadat]
 * 
 * Revision 2.8.5.2  92/03/28  10:06:41  jeffreyh
 * 	Changes from MK71
 * 	[92/03/20  12:15:46  jeffreyh]
 * 
 * Revision 2.9  92/02/19  16:30:17  elf
 * 	On ps2_mouse_close, wait a bit and then drain console.  For
 * 	T2200 and sometime the IBM axion bus extender, data will show
 * 	up after a while (a 0) w/o interrupt and your kbd would hang.
 * 	[92/01/30            rvb]
 * 
 * 	Allow for only "keyboard mouse".  Does not use serial line.
 * 	[92/01/25            rvb]
 * 
 * Revision 2.8.5.1  92/02/18  18:55:35  jeffreyh
 * 	Parallelized event handling
 * 	[91/07/16            bernadat]
 * 
 * 	Fixed ifdefs for at386_io_lock()
 * 	[91/06/28            bernadat]
 * 
 * 	Support for Corollary MP, switch to master for i386 ports R/W
 * 	Support fro Microsoft mouse
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.8  91/10/07  17:26:02  af
 * 	Add support for ibm ps2 "keyboard" mouse.  Try to be general
 * 	about it to support other "keyboard mice", but we don't have
 * 	any.
 * 	[91/09/25            rvb]
 * 	Add support for MicroSoft 2 button Mouse from 2.5
 * 	[91/09/04            rvb]
 * 
 * Revision 2.7  91/08/24  11:58:51  af
 * 	New MI autoconf.
 * 	[91/08/02  02:49:38  af]
 * 
 * Revision 2.6  91/05/14  16:27:26  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/02/05  17:19:17  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:45:43  mrt]
 * 
 * Revision 2.4  91/01/08  17:33:25  rpd
 * 	Trim $ Header
 * 	[90/11/27  11:44:31  rvb]
 * 
 * Revision 2.3  90/11/26  14:50:33  rvb
 * 	jsb bet me to XMK34, sigh ...
 * 	[90/11/26            rvb]
 * 	Synched 2.5 & 3.0 at I386q (r1.5.1.5) & XMK35 (r2.3)
 * 	[90/11/15            rvb]
 * 
 * Revision 2.2  90/05/21  13:27:25  dbg
 * 	Convert for pure kernel.
 * 	[90/04/24            dbg]
 * 
 * Revision 1.5.1.4  90/07/10  11:45:04  rvb
 * 	Allow mouse minor to select the serial line the mouse is on.
 * 	[90/06/15            rvb]
 * 
 * Revision 1.5.1.3  90/02/28  15:50:25  rvb
 * 	Fix numerous typo's in Olivetti disclaimer.
 * 	[90/02/28            rvb]
 * 
 * Revision 1.5.1.2  90/01/08  13:30:55  rvb
 * 	Add Olivetti copyright.
 * 	[90/01/08            rvb]
 * 
 * Revision 1.5.1.1  89/10/22  11:34:32  rvb
 * 	Received from Intel October 5, 1989.
 * 	[89/10/13            rvb]
 * 
 * Revision 1.5  89/07/17  10:41:24  rvb
 * 	Olivetti Changes to X79 upto 5/9/89:
 * 	[89/07/11            rvb]
 * 
 * Revision 1.4.1.1  89/04/27  12:21:17  kupfer
 * Merge X79 with our latest and greatest.
 * 
 * Revision 1.2.1.1  89/04/27  11:54:40  kupfer
 * X79 from CMU.
 * 
 * Revision 1.4  89/03/09  20:06:46  rpd
 * 	More cleanup.
 * 
 * Revision 1.3  89/02/26  12:42:38  gm0w
 * 	Changes for cleanup.
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
 *  File:         kd_mouse.c
 *  Description:  mouse driver as part of keyboard/display driver
 * 
 *  $ Header: $
 * 
 *  Copyright Ing. C. Olivetti & C. S.p.A. 1989.
 *  All rights reserved.
 * 
 *   Copyright 1988, 1989 by Olivetti Advanced Technology Center, Inc.,
 * Cupertino, California.
 * 
 * 		All Rights Reserved
 * 
 *   Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appears in all
 * copies and that both the copyright notice and this permission notice
 * appear in supporting documentation, and that the name of Olivetti
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 * 
 *   OLIVETTI DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL OLIVETTI BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
 * NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUR OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Hacked up support for serial mouse connected to COM1, using Mouse
 * Systems 5-byte protocol at 1200 baud.  This should work for
 * Mouse Systems, SummaMouse, and Logitek C7 mice.
 *
 * The interface provided by /dev/mouse is a series of events as
 * described in machine/kd.h.
 */

#include <mach/boolean.h>
#include <sys/types.h>
#include <platforms.h>
#include <cpus.h>
#include <kern/spl.h>
#include <kern/misc_protos.h>
#include <device/errno.h>
#include <device/io_req.h>
#include <device/ds_routines.h>
#include <i386/AT386/mp/mp.h>
#ifdef	CBUS
#include <cbus/cbus.h>
#else	/* CBUS */
#define at386_io_lock_state()
#define at386_io_lock(op)	(TRUE)
#define at386_io_unlock()
#endif	/* CBUS */
#include <i386/ipl.h>
#include <i386/pio.h>
#include <i386/misc_protos.h>
#include <chips/busses.h>
#include <i386/AT386/kd.h>
#include <i386/AT386/kdsoft.h>
#include <i386/AT386/kd_queue.h>
#include <i386/AT386/kd_entries.h>
#include <i386/AT386/kd_mouse_entries.h>
#include <i386/AT386/i8250.h>

intr_t oldvect;				/* old interrupt vector */
static int oldunit;
static spl_t oldspl;
extern	struct	bus_device *cominfo[];

kd_event_queue mouse_queue;		/* queue of mouse events */
boolean_t mouse_in_use = FALSE;
mpqueue_head_t	mouse_read_queue;

/*
 * The state of the 3 buttons is encoded in the low-order 3 bits (both
 * here and in other variables in the driver).
 */
u_char lastbuttons;		/* previous state of mouse buttons */
#define MOUSE_UP	1
#define MOUSE_DOWN	0
#define MOUSE_ALL_UP	0x7

/* Forward */

extern void		mouseintr(
				int			unit);
extern void		mouse_enqueue(
				kd_event		* ev);
extern boolean_t	mouse_read_done(
				io_req_t		ior);
extern void		init_mouse_hw(
				i386_ioport_t		base_addr);
extern void		serial_mouse_open(
				dev_t			dev);
extern void		kd_mouse_open(
				dev_t			dev,
				int			pic);
extern void		serial_mouse_close(
				dev_t			dev);
extern void		kd_mouse_close(
				dev_t			dev,
				int			pic);
extern void		mouse_packet_mouse_system_mouse(
				u_char			mousebuf[]);

extern void		mouse_packet_microsoft_mouse(
				u_char			mousebuf[]);
extern void		ibm_ps2_mouse_open(
				dev_t			dev);
extern void		ibm_ps2_mouse_close(
				dev_t			dev);
extern void		mouse_packet_ibm_ps2_mouse(
				u_char			mousebuf[]);
extern void		mouse_packet_port_mouse(
				u_char			mousebuf[]);


/*
 * init_mouse_hw - initialize the serial port.
 */

void
init_mouse_hw(
	i386_ioport_t	base_addr)
{
	at386_io_lock_state();

	at386_io_lock(MP_DEV_WAIT);
	outb(base_addr + RIE, 0);
	outb(base_addr + RLC, LCDLAB);
	outb(base_addr + RDLSB, BCNT1200 & 0xff);
	outb(base_addr + RDMSB, (BCNT1200 >> 8) & 0xff);
	outb(base_addr + RLC, LC8);
	outb(base_addr + RMC, MCDTR | MCRTS | MCOUT2);
	outb(base_addr + RIE, IERD | IELS);
	at386_io_unlock();
}


/*
 * mouseopen - Verify that the request is read-only, initialize,
 * and remember process group leader.
 */

/*
 * Low 3 bits of minor are the com port #.
 * The high 5 bits of minor are the mouse type
 */
#define	MOUSE_SYSTEM_MOUSE	0
#define MICROSOFT_MOUSE		1
#define IBM_MOUSE		2
#define PORT_MOUSE		3
#define NO_MOUSE		4
static int mouse_type;
static int mousebufsize;

io_return_t
mouseopen(
	dev_t		dev,
	int		flags,
	io_req_t	ior)
{
	spl_t	s;

	/*
	 * Disable all interrupts until we get the mouse state machine
	 * set up and ready for interrupts.  This avoids a potential race
	 * with kdintr() taking mouse events before we're ready to receive
	 * them.
	 */
	s = splhi();

	if (mouse_in_use) {
		splx(s);
		return(EBUSY);
	}

	mpqueue_init(&mouse_read_queue);
	kdq_reset(&mouse_queue);
	lastbuttons = MOUSE_ALL_UP;

	switch (mouse_type = ((minor(dev) & 0xf8) >> 3)) {
	case MICROSOFT_MOUSE:
		mousebufsize = 3;
		serial_mouse_open(dev);
		break;
	case MOUSE_SYSTEM_MOUSE:
		mousebufsize = 5;
		serial_mouse_open(dev);
		break;
	case IBM_MOUSE:
	case PORT_MOUSE:
		mousebufsize = 3;
		kd_mouse_open(dev, 12);
		ibm_ps2_mouse_open(dev);
		break;
	case NO_MOUSE:
		break;
	}
	mouse_in_use = TRUE;
	splx(s);
	return(0);
}

/* Must be called at splhi() ! */
void
serial_mouse_open(
	dev_t		dev)
{
	int unit = minor(dev) & 0x7;
	int mouse_pic = cominfo[unit]->sysdep1;
	i386_ioport_t base_addr = (i386_ioport_t)(int)cominfo[unit]->address;

	oldvect = ivect[mouse_pic];
	ivect[mouse_pic] = (intr_t)mouseintr;

	oldunit = iunit[mouse_pic];
	iunit[mouse_pic] = unit;

	init_mouse_hw(base_addr);
}

int mouse_packets = 0;

/* Must be called at splhi() ! */
void
kd_mouse_open(
	dev_t		dev,
	int		mouse_pic)
{
	oldvect = ivect[mouse_pic];
	ivect[mouse_pic] = (intr_t)kdintr;
	oldspl = intpri[mouse_pic];
	intpri[mouse_pic] = SPL6;
	form_pic_mask();
}

/*
 * mouseclose - Disable interrupts on the serial port, reset driver flags, 
 * and restore the serial port interrupt vector.
 */

void
mouseclose(
	dev_t		dev)
{
	spl_t s;

	switch (mouse_type) {
	case MICROSOFT_MOUSE:
		serial_mouse_close(dev);
		break;
	case MOUSE_SYSTEM_MOUSE:
		serial_mouse_close(dev);
		break;
	case IBM_MOUSE:
        case PORT_MOUSE:
		ibm_ps2_mouse_close(dev);
		kd_mouse_close(dev, 12);
		{int i = 20000; for (;i--;); }
		kd_mouse_drain();
		break;
	case NO_MOUSE:
		break;
	}

	kdq_reset(&mouse_queue);		/* paranoia */
	s = splhi();
	mouse_in_use = FALSE;
	splx(s);
}


void
serial_mouse_close(
	dev_t		dev)
{
	spl_t o_pri = splhi();		/* mutex with open() */
	int unit = minor(dev) & 0x7;
	int mouse_pic = cominfo[unit]->sysdep1;
	i386_ioport_t base_addr = (i386_ioport_t)(int)cominfo[unit]->address;
	at386_io_lock_state();

	assert(ivect[mouse_pic] == (intr_t)mouseintr);
	at386_io_lock(MP_DEV_WAIT);
	outb(base_addr + RIE, 0);	/* disable serial port */
	at386_io_unlock();
	ivect[mouse_pic] = oldvect;
	iunit[mouse_pic] = oldunit;

	(void)splx(o_pri);
}

void
kd_mouse_close(
	dev_t		dev,
	int		mouse_pic)
{
	spl_t s = splhi();

	ivect[mouse_pic] = oldvect;
	intpri[mouse_pic] = oldspl;
	form_pic_mask();
	splx(s);
}

/*
 * mouseread - dequeue and return any queued events.
 */

io_return_t
mouseread(
	dev_t			dev,
	io_req_t		ior)
{
	register int	err, count;
	spl_t		s;

	err = device_read_alloc(ior, (vm_size_t)ior->io_count);
	if (err != KERN_SUCCESS)
	    return (err);

	s = SPLKD();
	kdq_lock(&mouse_queue);

	if (kdq_empty(&mouse_queue)) {
	    if (ior->io_mode & D_NOWAIT) {
		kdq_unlock(&mouse_queue);
		splx(s);
		return (D_WOULD_BLOCK);
	    }
	    ior->io_done = mouse_read_done;
	    mpenqueue_tail(&mouse_read_queue, (queue_entry_t)ior);
	    kdq_unlock(&mouse_queue);
	    splx(s);
	    return (D_IO_QUEUED);
	}
	count = 0;
	while (!kdq_empty(&mouse_queue) && count < ior->io_count) {
	    register kd_event *ev;

	    ev = kdq_get(&mouse_queue);
	    *(kd_event *)(&ior->io_data[count]) = *ev;
	    count += sizeof(kd_event);
	}
	kdq_unlock(&mouse_queue);
	splx(s);
	ior->io_residual = ior->io_count - count;
	return (D_SUCCESS);
}

boolean_t mouse_read_done(
	io_req_t	ior)
{
	register int	count;
	spl_t		s;

	s = SPLKD();
	kdq_lock(&mouse_queue);
	if (kdq_empty(&mouse_queue)) {
	    ior->io_done = mouse_read_done;
	    mpenqueue_tail(&mouse_read_queue, (queue_entry_t)ior);
	    kdq_unlock(&mouse_queue);
	    splx(s);
	    return (FALSE);
	}

	count = 0;
	while (!kdq_empty(&mouse_queue) && count < ior->io_count) {
	    register kd_event *ev;

	    ev = kdq_get(&mouse_queue);
	    *(kd_event *)(&ior->io_data[count]) = *ev;
	    count += sizeof(kd_event);
	}
	kdq_unlock(&mouse_queue);
	splx(s);

	ior->io_residual = ior->io_count - count;
	ds_read_done(ior);

	return (TRUE);
}


/*
 * mouseintr - Get a byte and pass it up for handling.  Called at SPLKD.
 */

void
mouseintr(
	int		unit)
{
	i386_ioport_t base_addr = (i386_ioport_t)(int)cominfo[unit]->address;
	unsigned char id, ls;

	/* get reason for interrupt and line status */
	id = inb(base_addr + RID);
	ls = inb(base_addr + RLS);

	/* handle status changes */
	if (id == IDLS) {
		if (ls & LSDR) {
			inb(base_addr + RDAT);	/* flush bad character */
		}
		return;			/* ignore status change */
	}

	if (id & IDRD) {
		mouse_handle_byte((u_char)(inb(base_addr + RDAT) & 0xff));
	}
}


/*
 * handle_byte - Accumulate bytes until we have an entire packet.
 * If the mouse has moved or any of the buttons have changed state (up
 * or down), enqueue the corresponding events.
 * Called at SPLKD.
 * XXX - magic numbers.
 */
#define MOUSEBUFSIZE	5		/* num bytes def'd by protocol */
	static u_char mousebuf[MOUSEBUFSIZE];	/* 5-byte packet from mouse */
	static short mbindex = 0;	/* index into mousebuf */

void
mouse_handle_byte(
	u_char		ch)
{
	if (mbindex == 0) {
		/*
		 * Ignore all bytes until we see the start of a packet,
		 * otherwise the mouse packets may get out of sequence and
		 * things will get very confusing.
		 */
		switch (mouse_type) {
		case MICROSOFT_MOUSE:
			if ((ch & 0xc0) != 0xc0)
				return;
			break;
		case MOUSE_SYSTEM_MOUSE:
			if ((ch & 0xf8) != 0x80)
			return;
			break;
		case IBM_MOUSE:
			if ((ch == 0xfa) || ((ch & 0x08) != 0x08))
				return;
			break;
		case PORT_MOUSE:
			break;
		}
	}

	mousebuf[mbindex++] = ch;
	if (mbindex < mousebufsize)
		return;
	
	/* got a packet */
	mbindex = 0;

	switch (mouse_type) {
	case MICROSOFT_MOUSE:
		mouse_packet_microsoft_mouse(mousebuf);
		break;
	case MOUSE_SYSTEM_MOUSE:
		mouse_packet_mouse_system_mouse(mousebuf);
		break;
	case IBM_MOUSE:
		mouse_packet_ibm_ps2_mouse(mousebuf);
		break;
	case PORT_MOUSE:
		mouse_packet_port_mouse(mousebuf);
		break;
	}
}

void
mouse_packet_mouse_system_mouse(
	u_char	mousebuf[])
{
	u_char buttons, buttonchanges;
	struct mouse_motion moved;

	buttons = mousebuf[0] & 0x7;	/* get current state of buttons */
	buttonchanges = buttons ^ lastbuttons;
	moved.mm_deltaX = (char)mousebuf[1] + (char)mousebuf[3];
	moved.mm_deltaY = (char)mousebuf[2] + (char)mousebuf[4];

	if (moved.mm_deltaX != 0 || moved.mm_deltaY != 0)
		mouse_moved(moved);

	if (buttonchanges != 0) {
		lastbuttons = buttons;
		if (buttonchanges & 1)
			mouse_button(MOUSE_RIGHT, buttons & 1);
		if (buttonchanges & 2)
			mouse_button(MOUSE_MIDDLE, (buttons & 2) >> 1);
		if (buttonchanges & 4)
			mouse_button(MOUSE_LEFT, (buttons & 4) >> 2);
	}
}

/* same as above for microsoft mouse */
/*
 * 3 byte microsoft format used
 *
 * 7  6  5  4  3  2  1  0
 * 1  1  L  R  Y7 Y6 X7 X6
 * 1  0  X5 X4 X3 X3 X1 X0
 * 1  0  Y5 Y4 Y3 Y2 Y1 Y0
 *
 */

void
mouse_packet_microsoft_mouse(
	u_char		mousebuf[])
{
	u_char buttons, buttonchanges;
	struct mouse_motion moved;

	buttons = ((mousebuf[0] & 0x30) >> 4);
			/* get current state of buttons */
#ifdef	gross_hack
	if (buttons == 0x03)	/* both buttons down */
		buttons = 0x04;
#endif	/* gross_hack */
	buttons = (~buttons) & 0x07;	/* convert to not pressed */

	buttonchanges = buttons ^ lastbuttons;
	moved.mm_deltaX = ((mousebuf[0] & 0x03) << 6) | (mousebuf[1] & 0x3F);
	moved.mm_deltaY = ((mousebuf[0] & 0x0c) << 4) | (mousebuf[2] & 0x3F);
	if (moved.mm_deltaX & 0x80)	/* negative, in fact */
		moved.mm_deltaX = moved.mm_deltaX - 0x100;
	if (moved.mm_deltaY & 0x80)	/* negative, in fact */
		moved.mm_deltaY = moved.mm_deltaY - 0x100;
	/* and finally the Y orientation is different for the microsoft mouse */
	moved.mm_deltaY = -moved.mm_deltaY;

	if (moved.mm_deltaX != 0 || moved.mm_deltaY != 0)
		mouse_moved(moved);

	if (buttonchanges != 0) {
		lastbuttons = buttons;
		if (buttonchanges & 1)
			mouse_button(MOUSE_RIGHT, (buttons & 1) ?
						MOUSE_UP : MOUSE_DOWN);
		if (buttonchanges & 2)
			mouse_button(MOUSE_LEFT, (buttons & 2) ?
						MOUSE_UP : MOUSE_DOWN);
		if (buttonchanges & 4)
			mouse_button(MOUSE_MIDDLE, (buttons & 4) ?
						MOUSE_UP : MOUSE_DOWN);
	}
}

void
ibm_ps2_mouse_open(
	dev_t		dev)
{
	char buf[4] = {0,0,0,0};

	lastbuttons = 0;
	mbindex = 0;

	kd_sendcmd(0xa8);	/* enable mouse in kbd */

	kd_cmdreg_write(0x47);	/* allow mouse interrupts */
				/* magic number for ibm? */

	if (kd_mouse_write(0xff) != 0xfa)	/* reset mouse */
		return;

	kd_mouse_read(2, buf);

	kd_mouse_write(0xea);	/* stream mode */
	kd_mouse_write(0xf4);	/* enable */
}

void
ibm_ps2_mouse_close(
	dev_t		dev)
{
	char buf[4] = {0,0,0,0};

	if (kd_mouse_write(0xff) == 0xfa)	/* reset mouse */
		kd_mouse_read(2, buf);

	kd_sendcmd(0xa7);	/* disable mouse in kbd */
	kd_cmdreg_write(0x65);	/* disallow mouse interrupts */
				/* magic number for ibm? */
}

/*
 * 3 byte ibm ps2 format used
 *
 * 7  6  5  4  3  2  1  0
 * YO XO YS XS 1  M  R  L
 * X7 X6 X5 X4 X3 X3 X1 X0
 * Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
 *
 */
void
mouse_packet_ibm_ps2_mouse(
	u_char		mousebuf[])
{
	u_char buttons, buttonchanges;
	struct mouse_motion moved;

	buttons = 0;
	if ( !(mousebuf[0] & 0x1) ) buttons |= 0x4;
	if ( !(mousebuf[0] & 0x2) ) buttons |= 0x1;
	if ( !(mousebuf[0] & 0x4) ) buttons |= 0x2;
	buttonchanges = buttons ^ lastbuttons;

	moved.mm_deltaX = ((mousebuf[0]&0x10) ? 0xffffff00 : 0 ) | (u_char)mousebuf[1];
	moved.mm_deltaY = ((mousebuf[0]&0x20) ? 0xffffff00 : 0 ) | (u_char)mousebuf[2];

	if (mouse_packets) {
		printf("(%x:%x:%x)", mousebuf[0], mousebuf[1], mousebuf[2]);
		return;
	}

	if (moved.mm_deltaX != 0 || moved.mm_deltaY != 0)
		mouse_moved(moved);

	if (buttonchanges != 0) {
		lastbuttons = buttons;
		if (buttonchanges & 1)
			mouse_button(MOUSE_RIGHT, buttons & 1);
		if (buttonchanges & 2)
			mouse_button(MOUSE_MIDDLE, (buttons & 2) >> 1);
		if (buttonchanges & 4)
			mouse_button(MOUSE_LEFT, (buttons & 4) >> 2);
	}	
}

/*
 * 3 byte port mouse format used
 *
 * 7  6  5  4  3  2  1  0
 * 1  0  0  XS YS L  M  R
 * 0  X6 X5 X4 X3 X3 X2 X0
 * 0  Y6 Y5 Y4 Y3 Y2 Y1 Y0
 *
 */

void
mouse_packet_port_mouse(
	u_char		mousebuf[])
{
	u_char buttons, buttonchanges;
	struct mouse_motion moved;

	buttons = mousebuf[1] & 0x07;
	buttonchanges = buttons ^ lastbuttons;
	moved.mm_deltaX = ((mousebuf[2]&0x80) ? 0xffffff00 : 0 ) | (u_char)mousebuf[2];
	moved.mm_deltaY = ((mousebuf[0]&0x80) ? 0xffffff00 : 0 ) | (u_char)mousebuf[0];
	if (mouse_packets) {
		printf("||(%x:%x:%x)", mousebuf[0], mousebuf[1], mousebuf[2]);
		printf("(x=%d:y=%d)",moved.mm_deltaX,moved.mm_deltaY);
		return;
	}

	if (moved.mm_deltaX != 0 || moved.mm_deltaY != 0)
		mouse_moved(moved);

	if (buttonchanges != 0) {
		lastbuttons = buttons;
		switch (buttonchanges) {
		case 1:
			mouse_button(MOUSE_LEFT, !(buttons & 1));
			break;
		case 2:
			mouse_button(MOUSE_RIGHT, !((buttons & 2) >> 1));
			break;
		case 3:
			mouse_button(MOUSE_MIDDLE, !(buttons == 3));
			break;
		case 4:
			mouse_button(MOUSE_MIDDLE, !(buttons == 4));
			break;
		}
	}
}


/*
 * Enqueue a mouse-motion event.  Called at SPLKD.
 */

void
mouse_moved(
	struct mouse_motion	where)
{
	kd_event ev;

	ev.type = MOUSE_MOTION;
	ev.time = time;
	ev.value.mmotion = where;
	mouse_enqueue(&ev);
}


/*
 * Enqueue an event for mouse button press or release.  Called at SPLKD.
 */

void
mouse_button(
	kev_type	which,
	u_char		direction)
{
	kd_event ev;

	ev.type = which;
	ev.time = time;
	ev.value.up = (direction == MOUSE_UP) ? TRUE : FALSE;
	mouse_enqueue(&ev);
}


/*
 * mouse_enqueue - enqueue an event and wake up selecting processes, if
 * any.  Called at SPLKD.
 */

void
mouse_enqueue(
	kd_event	*ev)
{
	kdq_lock(&mouse_queue);
	if (kdq_full(&mouse_queue)) {
		printf("mouse: queue full\n");
	} else
		kdq_put(&mouse_queue, ev);

	{
	    io_req_t	ior;
	    for(;;) {
		mpdequeue_head(&mouse_read_queue, (queue_entry_t *)&ior);
		if (ior)
			iodone(ior);
		else
			break;
	    }
	}
	kdq_unlock(&mouse_queue);
}

