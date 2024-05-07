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
 * Revision 2.7.5.4  92/06/24  17:58:40  jeffreyh
 * 	Updated to new at386_io_lock() interface.
 * 	Fixed locking bugs, spl() must be called before
 * 	simple_lock on tty lock.
 * 	Must bind to master before calling char_write() (Corollary)
 * 	[92/06/03            bernadat]
 * 
 * Revision 2.7.5.3  92/04/30  11:54:50  bernadat
 * 	Fix call to debugger, use kdb_kintr().
 * 	Added missing unlock/lock on tp->t_lock in comwaitforempty().
 * 	Set COM0 to 7bits/even at boot time to allow COM0 as console
 * 	[92/04/08            bernadat]
 * 
 * Revision 2.7.5.2  92/03/28  10:05:50  jeffreyh
 * 	Wait for chars buffered in the serial chip to be sent before
 * 	closing.
 * 	[92/03/27            emcmanus]
 * 	04-Mar-92  emcmanus at gr.osf.org
 * 		Drain tty output before closing.  (Perhaps this should be up to the
 * 		user process using the new TTY_DRAIN request, instead.)
 * 		Also drain before starting a break, including any characters in the
 * 		serial chip's buffer.
 * 		Status interrupts modified so breaks and bad parity can be reported
 * 		to the user via the new out-of-band mechanism.
 * 		Call comintr() after initialising device registers in comparam();
 * 		otherwise interrupts never get going on some machines.
 * 	[92/03/10  07:56:25  bernadat]
 * 	Changes from TRUNK
 * 	[92/03/10  14:12:10  jeffreyh]
 * 
 * Revision 2.9  92/02/19  15:08:12  elf
 * 	Made comprobe more selective. Try to recognize chip.
 * 	[92/01/20            kivinen]
 * 
 * Revision 2.8  92/01/03  20:10:27  dbg
 * 	Don't drop software CARR_ON if carrier drops - modem drops
 * 	carrier but still can talk to machine.
 * 	[91/10/30            dbg]
 * 
 * Revision 2.7.5.1  92/02/18  18:42:50  jeffreyh
 * 	Picked up changes from latest. Below is the 2.8 log message from dbg:
 * 	Don't drop software CARR_ON if carrier drops - modem drops
 * 	carrier but still can talk to machine.
 * 	[92/02/18            jeffreyh]
 * 
 * Revision 2.7.4.1  92/02/13  18:44:50  jeffreyh
 * 	Added missing locks in comintr()
 * 	[91/09/09            bernadat]
 * 
 * 	Support for Corollary MP, switch to master for i386 ports R/W
 * 	Allow COM1 to be the console if no VGA (Z1000)
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.7  91/10/07  17:25:14  af
 * 	Add some improvements from 2.5 version.
 * 	[91/09/04  22:05:25  rvb]
 * 
 * Revision 2.6  91/08/24  11:57:21  af
 * 	New MI autoconf.
 * 	[91/08/02  02:50:03  af]
 * 
 * Revision 2.5  91/05/14  16:21:14  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/14  14:42:06  mrt
 * 	Merge of dbg's latest working com.c onto the old com.c
 * 	with the new autoconf and other major changes.
 * 	[91/01/28  15:26:13  rvb]
 * 
 * Revision 2.3  91/02/05  17:16:33  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:42:21  mrt]
 * 
 * Revision 2.2  90/11/26  14:49:26  rvb
 * 	jsb bet me to XMK34, sigh ...
 * 	[90/11/26            rvb]
 * 	Apparently first version is r2.2
 * 	[90/11/25  10:44:41  rvb]
 * 
 * 	Synched 2.5 & 3.0 at I386q (r2.3.1.6) & XMK35 (r2.2)
 * 	[90/11/15            rvb]
 * 
 * Revision 2.3.1.5  90/08/25  15:43:06  rvb
 * 	I believe that nothing of the early Olivetti code remains.
 * 	Copyright gone.
 * 	[90/08/21            rvb]
 * 
 * 	Use take_<>_irq() vs direct manipulations of ivect and friends.
 * 	[90/08/20            rvb]
 * 
 * 		Moved and rewrote much of the code to improve performance.
 * 	     Still suffers from overruns.
 * 	[90/08/14            mg32]
 * 
 * Revision 2.3.1.4  90/07/10  11:43:07  rvb
 * 	Rewrote several functions to look more like vax-BSD dh.c.
 * 	[90/06/25            mg32]
 * 
 * 	New style probe/attach.
 * 	Also com_struct has been radically reworked, ...
 * 	[90/06/15            rvb]
 * 
 * Revision 2.3.1.3  90/02/28  15:49:12  rvb
 * 	Fix numerous typo's in Olivetti disclaimer.
 * 	[90/02/28            rvb]
 * 
 * Revision 2.3.1.2  90/01/08  13:32:00  rvb
 * 	Add Olivetti copyright.
 * 	[90/01/08            rvb]
 * 
 * Revision 2.3.1.1  89/12/21  18:01:29  rvb
 * 	Changes from Ali Ezzet.
 * 
 * Revision 2.2.0.0  89/07/17  10:39:30  rvb
 * 	New from Olivetti.
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

#include <com.h>
#if NCOM > 0

#include <cpus.h>
#include <mach_kdb.h>
#include <mach_kgdb.h>
#include <platforms.h>
#include <sys/types.h>
#include <sys/time.h>
#include <device/conf.h>
#include <device/errno.h>
#include <device/buf.h>
#include <device/tty.h>
#include <kern/spl.h>
#include <i386/ipl.h>
#include <i386/pio.h>
#include <i386/misc_protos.h>
#include <chips/busses.h>
#include <i386/AT386/comreg.h>
#include <i386/AT386/kd.h>
#include <i386/AT386/kdsoft.h>
#include <i386/AT386/com_entries.h>
#include <i386/AT386/misc_protos.h>

#include <i386/AT386/mp/mp.h>
#ifdef	CBUS
#include <cbus/cbus.h>
#else	/* CBUS */
#define at386_io_lock_state()
#define at386_io_lock(op)	(TRUE)
#define at386_io_unlock()
#endif	/* CBUS */

#include <kern/misc_protos.h>
#include <kern/time_out.h>

/* Forward */

extern int		comprobe(
				caddr_t			port,
				struct bus_device	* dev);
extern void		comattach(
				struct bus_device	* dev);
extern void		comstart(
					struct tty	* tp);
extern void		comparam(
					int		unit);
extern void		comstop(
					struct tty	* tp,
					int		flags);
extern void		comwaitforempty(
					struct tty	* tp);
extern void		fix_modem_state(
					int		unit,
					int		modem_stat);

struct bus_device *cominfo[NCOM];	/* ??? */

static caddr_t com_std[NCOM] = { 0 };
static struct bus_device *com_info[NCOM];
struct bus_driver comdriver = {
	(probe_t)comprobe, 0, comattach, 0, com_std, "com", com_info, 0, 0, 0};

struct tty com_tty[NCOM];
int commodem[NCOM];
int comcarrier[NCOM] = {0, 0,};

int cons_is_com1 = 0;	/* set to 1 if no PC keyboard, in this case	*/
			/* com1 is the console 				*/
char com_halt_char = '_' & 0x1f; /* CTRL(_) */

#ifndef	PORTSELECTOR
#define ISPEED	9600
#define IFLAGS	(EVENP|ODDP|ECHO|CRMOD)
#else
#define ISPEED	4800
#define IFLAGS	(EVENP|ODDP)
#endif

static struct baud_rate_info com_speeds[] = {
	0,	0,
	50,	0x900,
	75,	0x600,
	110,	0x417,
	134,	0x359,
	150,	0x300,
	300,	0x180,
	600,	0x0c0,
	1200,	0x060,
	1800,	0x040,
	2400,	0x030,
	4800,	0x018,
	9600,	0x00c,
	14400,	0x009,
	19200,	0x006,
	38400,	0x003,
	57600,	0x002,
	115200,	0x001,
	-1,	-1
};

int
comprobe(
	caddr_t			port,
	struct bus_device	*dev)
{
	i386_ioport_t	addr = (long)dev->address;
	int	unit = dev->unit;
	int     oldctl, oldmsb;
	char    *type = "8250";
	int     i;

	if ((unit < 0) || (unit >= NCOM)) {
		printf("com %d out of range\n", unit);
		return(0);
	}
	if (unit == 0 && cons_is_com1)
		return(1);
	oldctl = inb(LINE_CTL(addr));	 /* Save old value of LINE_CTL */
	oldmsb = inb(BAUD_MSB(addr));	 /* Save old value of BAUD_MSB */
	outb(LINE_CTL(addr), 0);	 /* Select INTR_ENAB */    
	outb(BAUD_MSB(addr), 0);
	if (inb(BAUD_MSB(addr)) != 0)
	  {
	    outb(LINE_CTL(addr), oldctl);
	    outb(BAUD_MSB(addr), oldmsb);
	    return 0;
	  }
	outb(LINE_CTL(addr), iDLAB);	 /* Select BAUD_MSB */
	outb(BAUD_MSB(addr), 255);
	if (inb(BAUD_MSB(addr)) != 255)
	  {
	    outb(LINE_CTL(addr), oldctl);
	    outb(BAUD_MSB(addr), oldmsb);
	    return 0;
	  }
	outb(LINE_CTL(addr), 0);	 /* Select INTR_ENAB */
	if (inb(BAUD_MSB(addr)) != 0)	 /* Check that it has kept it's value*/
	  {
	    outb(LINE_CTL(addr), oldctl);
	    outb(BAUD_MSB(addr), oldmsb);
	    return 0;
	  }

	/* Com port found, now check what chip it has */
	
	for(i = 0; i < 256; i++)	 /* Is there Scratch register */
	  {
	    outb(addr + 7, i);
	    if (inb(addr + 7) != i)
	      break;
	  }
	if (i == 256)
	  {				 /* Yes == 450 or 460 */
	    outb(addr + 7, 0);
	    type = "82450 or 16450";
	    outb(INTR_ID(addr), 0xc1);	 /* Enable fifo */
	    if ((inb(INTR_ID(addr)) & 0xc0) != 0)
	      {				 /* Was it successfull */
		/* if both bits are not set then broken xx550 */
		if ((inb(INTR_ID(addr)) & 0xc0) == 0xc0)
		  {
		    type = "82550 or 16550";
		    /* XXX even here the FIFO can be broken !? */
		  }
		else
		  {
		    type = "82550 or 16550 with non-working FIFO";
		  }
		outb(INTR_ID(addr), 0x00); /* Disable fifos */
	      }
	  }
	printf("com%d: %s chip.\n", dev->unit, type);
	return 1;
}

void
comattach(
	struct bus_device	*dev)
{
	u_char	unit = dev->unit;
	i386_ioport_t	addr = (long)dev->address;

	take_dev_irq(dev);
	printf(", port = %x, spl = %d, pic = %d. (DOS COM%d)",
		dev->address, dev->sysdep, dev->sysdep1, unit+1);

	cominfo[unit] = dev;
/*	comcarrier[unit] = addr->flags;*/
	commodem[unit] = 0;

	ttychars(&com_tty[unit]);

	if (unit == 0 && cons_is_com1)
		return;
	
	outb(INTR_ENAB(addr), 0);
	outb(MODEM_CTL(addr), 0);
	while (!(inb(INTR_ID(addr))&1)) {
		inb(LINE_STAT(addr)); 
		inb(TXRX(addr)); 
		inb(MODEM_STAT(addr)); 
	}
}

io_return_t
comopen(
	dev_t		dev,
	int		flag,
	io_req_t	ior)
{
	int		unit = minor(dev);
	struct bus_device	*isai;
	struct tty	*tp;
	spl_t           s;
	at386_io_lock_state();

	if (unit >= NCOM || (isai = cominfo[unit]) == 0 || isai->alive == 0)
		return(D_NO_SUCH_DEVICE);
	tp = &com_tty[unit];
	at386_io_lock(MP_DEV_WAIT);
	s = spltty();

	spinlock_lock(&tp->t_lock);
	if ((tp->t_state & (TS_ISOPEN|TS_WOPEN)) == 0) {
		tp->t_addr = isai->address;
		tp->t_dev = dev;
		tp->t_oproc = comstart;
		tp->t_stop = comstop;
		tp->t_getstat = comgetstat;
		tp->t_setstat = comsetstat;
#ifndef	PORTSELECTOR
		if (tp->t_ispeed == 0) {
#else
			tp->t_state |= TS_HUPCLS;
#endif	/* PORTSELECTOR */
			tp->t_ispeed = ISPEED;
			tp->t_ospeed = ISPEED;
			tp->t_flags = IFLAGS;
			tp->t_state &= ~TS_BUSY;
#ifndef	PORTSELECTOR
		}
#endif	/* PORTSELECTOR */
	}
/*rvb	tp->t_state |= TS_WOPEN; */
	if ((tp->t_state & TS_ISOPEN) == 0)
		comparam(unit);
	if (!comcarrier[unit])	/* not originating */
		tp->t_state |= TS_CARR_ON;
	else {
		int modem_stat = inb(MODEM_STAT((long)tp->t_addr));
		if (modem_stat & iRLSD)
			tp->t_state |= TS_CARR_ON;
		else
			tp->t_state &= ~TS_CARR_ON;
		fix_modem_state(unit, modem_stat);
	} 
	spinlock_unlock(&tp->t_lock);
	splx(s);
	at386_io_unlock();
	return (char_open(dev, tp, flag, ior));
}

void
comclose(
	dev_t		dev)
{
	struct tty	*tp = &com_tty[minor(dev)];
	i386_ioport_t	addr = (long)tp->t_addr;
	spl_t           s;
	at386_io_lock_state();

	at386_io_lock(MP_DEV_WAIT);
	s = spltty();
	spinlock_lock(&tp->t_lock);
	ttydrain(tp);
	comwaitforempty(tp);
	ttyclose(tp);
	if (tp->t_state&TS_HUPCLS || (tp->t_state&TS_ISOPEN)==0) { 
		outb(INTR_ENAB(addr), 0);
		outb(MODEM_CTL(addr), 0);
		tp->t_state &= ~TS_BUSY;
		commodem[minor(dev)] = 0;
	}
	spinlock_unlock(&tp->t_lock);
	splx(s);
	if (minor(dev) == 0 && cons_is_com1) {
		comreset();
		comintr(0);
	}
	at386_io_unlock();
}

io_return_t
comread(
	dev_t		dev,
	io_req_t	ior)
{
	return char_read(&com_tty[minor(dev)], ior);
}

io_return_t
comwrite(
	dev_t		dev,
	io_req_t	ior)
{
	int ret;
	at386_io_lock_state();

	at386_io_lock(MP_DEV_WAIT);
	ret = (int)char_write(&com_tty[minor(dev)], ior);
	at386_io_unlock();
	return(ret);
}

boolean_t
comportdeath(
	dev_t		dev,
	ipc_port_t	port)
{
	return (tty_portdeath(&com_tty[minor(dev)], port));
}

io_return_t
comgetstat(
	dev_t		dev,
	int		flavor,
	int		*data,		/* pointer to OUT array */
	unsigned int	*count)		/* out */
{
	io_return_t	result = D_SUCCESS;
	int		unit = minor(dev);
	at386_io_lock_state();

	switch (flavor) {
	case TTY_MODEM:
		at386_io_lock(MP_DEV_WAIT);
		fix_modem_state(unit,
		    inb(MODEM_STAT((long)cominfo[unit]->address)));
		at386_io_unlock();
		*data = commodem[unit];
		*count = 1;
		break;
	default:
		result = tty_get_status(&com_tty[unit], flavor, data, count);
		break;
	}
	return (result);
}

io_return_t
comsetstat(
	dev_t		dev,
	int		flavor,
	int *		data,
	unsigned int	count)
{
	io_return_t	result = D_SUCCESS;
	int 		unit = minor(dev);
	i386_ioport_t	dev_addr = (long)cominfo[unit]->address;
	spl_t           s;
	struct tty *	tp = &com_tty[unit];
	int		needtounlock = 0;
	at386_io_lock_state();

	at386_io_lock(MP_DEV_WAIT);
	/* Certain commands logically require that we drain whatever
	   output we already have.  This is certainly true for
	   starting breaks, but we don't do it for TTY_MODEM in
	   case the user is trying to do (obsolete) DTR flow control.
	   As currently written, new characters that are written
	   while we are draining will also be waited for.  This
	   makes little sense, but it's not expected that the user
	   will do that. */
	switch (flavor) {
	case TTY_SET_BREAK:
	case TTY_CLEAR_BREAK:
	        s = spltty();
		spinlock_lock(&tp->t_lock);
		ttydrain(tp);
		comwaitforempty(tp);
		needtounlock = 1;
		break;
	}
	switch (flavor) {
	case TTY_SET_BREAK:
		outb(LINE_CTL(dev_addr), inb(LINE_CTL(dev_addr)) | iSETBREAK);
		break;
	case TTY_CLEAR_BREAK:
		outb(LINE_CTL(dev_addr), inb(LINE_CTL(dev_addr)) & ~iSETBREAK);
		break;
	case TTY_MODEM:
		{
			int	old_modem, new_modem;

			old_modem = commodem[unit];
			new_modem = *data;
			if ((old_modem & TM_DTR) == 0 && (new_modem & TM_DTR)) {
				/* set dtr (and RTS) */
				outb(MODEM_CTL(dev_addr), iOUT2|iDTR|iRTS);
				new_modem |= TM_RTS;
			} else if ((old_modem & TM_DTR) && (new_modem & TM_DTR) == 0) {
					/* clear dtr */
				outb(MODEM_CTL(dev_addr), iOUT2|iRTS);
			}
			commodem[unit] = new_modem;
			break;
		}
	case KDSKBENT:
        case KDSETBELL:
		if (cons_is_com1)
			result = kdsetstat(dev, flavor, data, count);
		break;
	default:
		result = tty_set_status(&com_tty[unit], flavor, data, count);
		if (result == D_SUCCESS && flavor == TTY_STATUS)
			comparam(unit);
		break;
	}
	if (needtounlock) {
		spinlock_unlock(&tp->t_lock);
		splx(s);
	}
	at386_io_unlock();
	return result;
}

/* Wait for transmit buffer to empty.  Interrupts only tell us that
   the serial device can accept another character; there may still
   be buffered characters to be sent. */

void
comwaitforempty(
	struct tty *	tp)
{
	while (!(inb(LINE_STAT((long)tp->t_addr)) & iTSRE)) {
		spinlock_unlock(&tp->t_lock);
		assert_wait(0, TRUE);
		thread_set_timeout(1);
		thread_block((void (*)(void)) 0);
		spinlock_lock(&tp->t_lock);
	}
}

#if	MACH_KDB
#define check_debugger(c)					\
	if (c == com_halt_char && !unit && cons_is_com1) { 	\
		/* if rebootflag is on, reboot the system */	\
		if (rebootflag)					\
			kdreboot();				\
		kdb_kintr();					\
		break;						\
	}
#elif	MACH_KGDB
#define check_debugger(c)					\
	if (c == com_halt_char && !unit && cons_is_com1) {	\
		/* if rebootflag is on, reboot the system */	\
		if (rebootflag)					\
			kdreboot();				\
		kgdb_kintr();					\
		break;						\
	}
#else	/* !MACH_KDB */
#define check_debugger(c)						\
	if (c == com_halt_char && !unit && cons_is_com1 && rebootflag)	\
		kdreboot();						\
	else
#endif	/* !MACH_KDB */

void
comintr(
	int	unit)
{
	register struct tty	*tp = &com_tty[unit];
	i386_ioport_t		addr = (long)cominfo[unit]->address;
	static char 		comoverrun = 0;
	char			line, intr_id;
	int			c, modem_stat, line_stat;
	extern int		rebootflag;

	while (!((intr_id=inb(INTR_ID(addr))) & 1))
	    switch (intr_id) { 
		case MODi: 
		    /* modem change */
			if(!(tp->t_state & TS_ISOPEN))
				return;
			modem_stat = inb(MODEM_STAT(addr));
			if (!(commodem[unit] & TM_CAR) && (modem_stat & iRLSD)) {
				(void) ttymodem(tp, 1);
			} else if ((commodem[unit] & TM_CAR) && !(modem_stat & iRLSD)) {
				if (ttymodem(tp, 0) == 0) {
					/* turn off */
				}
			}
			fix_modem_state(unit, modem_stat);
			break;

		case TRAi:
			if(!(tp->t_state & TS_ISOPEN))
				return;
			spinlock_lock(&tp->t_lock);
			tp->t_state &= ~(TS_BUSY|TS_FLUSH);
			tt_write_wakeup(tp);
				comstart(tp);
			spinlock_unlock(&tp->t_lock);
			break;
		case RECi:
			c = inb(TXRX(addr));
			check_debugger(c);
			if(!(tp->t_state & TS_ISOPEN))
				return;
			spinlock_lock(&tp->t_lock);
			ttyinput(c, tp);
			spinlock_unlock(&tp->t_lock);
			break;
		case LINi: 
			c = inb(TXRX(addr));
			line_stat = inb(LINE_STAT(addr));
			check_debugger(c);
			if(!tp->t_state & TS_ISOPEN)
				return;
			spinlock_lock(&tp->t_lock);
			if (line_stat & (iFE | iBRKINTR)) {
				/* framing error or break */
				ttybreak(c, tp);
			}
			if (line_stat & iPE) {
				if ((tp->t_flags&(EVENP|ODDP)) == EVENP ||
				    (tp->t_flags&(EVENP|ODDP)) == ODDP)
					ttyinputbadparity(c, tp);
				else ttyinput(c, tp);
			}
			spinlock_unlock(&tp->t_lock);
			if (line&iOR && !comoverrun) {
				printf("com%d: overrun\n", unit);
				comoverrun = 1;
			}
			break;
		}
}

void
comparam(
int		unit)
{
	struct tty	*tp = &com_tty[unit];
	i386_ioport_t	addr = (long)tp->t_addr;
	spl_t		s = spltty();
	int		mode;
	int		ospeed = baud_rate_get_info(tp->t_ospeed, com_speeds);

	if (ospeed < 0 ||
	    (ospeed && tp->t_ispeed && tp->t_ispeed != tp->t_ospeed)) {
		splx(s);
		return;
	}

        if (tp->t_ispeed == 0) {
		tp->t_state |= TS_HUPCLS;
		outb(MODEM_CTL(addr), iOUT2);
		commodem[unit] = 0;
		splx(s);
		return;
	}

	outb(LINE_CTL(addr), iDLAB);
	outb(BAUD_LSB(addr), ospeed & 0xff);
	outb(BAUD_MSB(addr), ospeed >> 8);

	if (tp->t_flags & (RAW|LITOUT|PASS8))
		mode = i8BITS;
	else
		mode = i7BITS;
	switch (tp->t_flags & (EVENP|ODDP)) {
	case 0:
		break;
	case ODDP:
		mode |= iPEN;
		break;
	case EVENP|ODDP:
	case EVENP:
		mode |= iPEN|iEPS;
		break;
	}
	if (tp->t_ispeed == 110)
		/*
		 * 110 baud uses two stop bits -
		 * all other speeds use one
		 */
		mode |= iSTB;

	outb(LINE_CTL(addr), mode);

	outb(INTR_ENAB(addr), iTX_ENAB|iRX_ENAB|iMODEM_ENAB|iERROR_ENAB);
	outb(MODEM_CTL(addr), iDTR|iRTS|iOUT2);
	commodem[unit] |= (TM_DTR|TM_RTS);
	comintr(unit);
        splx(s);
}

void
comstart(
	struct tty	*tp)
{
	char nch;
	at386_io_lock_state();

	/*
	 * The line below is a hack.  It seems that under SLIP, we sometimes lose
	 * interrupts, sigh.  Someday, this should be fixed.  It also may be that
	 * only certain machines manifest this problem.
	 */
/*     	comintr(minor(tp->t_dev));	*/

	if (tp->t_state & (TS_TIMEOUT|TS_TTSTOP|TS_BUSY)) {
		return;
	}
	if (tp->t_outq.c_cc <= TTLOWAT(tp)) {
		tt_write_wakeup(tp);
	}
	if (!tp->t_outq.c_cc)
		return;
	nch = getc(&tp->t_outq);
	if ((tp->t_flags & LITOUT) == 0 && (nch & 0200)) {
	    timeout((timeout_fcn_t)ttrstrt, tp, (nch & 0x7f) + 6);
	    tp->t_state |= TS_TIMEOUT;
	    return;
	}
	at386_io_lock(MP_DEV_WAIT);
	outb(TXRX((long)tp->t_addr), nch);
	tp->t_state |= TS_BUSY;
	at386_io_unlock();
	return;
}

/*
 * Set receive modem state from modem status register.
 */

void
fix_modem_state(
	int	unit,
	int	modem_stat)
{
	int	stat = 0;

	if (modem_stat & iCTS)
	    stat |= TM_CTS;	/* clear to send */
	if (modem_stat & iDSR)
	    stat |= TM_DSR;	/* data set ready */
	if (modem_stat & iRI)
	    stat |= TM_RNG;	/* ring indicator */
	if (modem_stat & iRLSD)
	    stat |= TM_CAR;	/* carrier? */

	commodem[unit] = (commodem[unit] & ~(TM_CTS|TM_DSR|TM_RNG|TM_CAR))
				| stat;
}

void
comstop(
	struct tty	*tp,
	int		flags)
{
	if ((tp->t_state & TS_BUSY) && (tp->t_state & TS_TTSTOP) == 0)
	    tp->t_state |= TS_FLUSH;
}

#if	0
void
compr(
	int		unit)
{
        compr_addr(cominfo[unit]->address);
        return(0);
}

void
compr_addr(addr)
{
	printf("TXRX(%x) %x, INTR_ENAB(%x) %x, INTR_ID(%x) %x, LINE_CTL(%x) %x,\n\
MODEM_CTL(%x) %x, LINE_STAT(%x) %x, MODEM_STAT(%x) %x\n",
	TXRX(addr), inb(TXRX(addr)),
	INTR_ENAB(addr), inb(INTR_ENAB(addr)),
	INTR_ID(addr), inb(INTR_ID(addr)),
	LINE_CTL(addr), inb(LINE_CTL(addr)),
	MODEM_CTL(addr), inb(MODEM_CTL(addr)),
	LINE_STAT(addr), inb(LINE_STAT(addr)),
	MODEM_STAT(addr), inb(MODEM_STAT(addr)));
}
#endif	/* 0 */

/*
 * all the followings are used for printf, and must work before com
 * is attached ==> we can not use cominfo
 */

#define COM0_ADDR 0x3f8

void
comreset(void)
{
	int ospeed = baud_rate_get_info(9600, com_speeds);

        inb(INTR_ID(COM0_ADDR));
        inb(LINE_STAT(COM0_ADDR));
        inb(MODEM_STAT(COM0_ADDR));
        inb(TXRX(COM0_ADDR));

	outb(LINE_CTL(COM0_ADDR), iDLAB);
	outb(BAUD_LSB(COM0_ADDR), ospeed & 0xff);
	outb(BAUD_MSB(COM0_ADDR), ospeed >> 8);
/*	outb(LINE_CTL(COM0_ADDR), i8BITS); */
	outb(LINE_CTL(COM0_ADDR), i7BITS | iPEN);
	outb(INTR_ENAB(COM0_ADDR), iTX_ENAB|iRX_ENAB);
	outb(MODEM_CTL(COM0_ADDR), iDTR|iRTS|iOUT2);
}

void
com_putc(
	char		c)
{
	register i;

	outb(INTR_ENAB(COM0_ADDR), 0);
	for (i=0; (!(inb(LINE_STAT(COM0_ADDR)) & iTHRE)) && (i < 1000); i++);
	outb(TXRX(COM0_ADDR),  c);
	for (i=0; (!(inb(LINE_STAT(COM0_ADDR)) & iTHRE)) && (i < 1000); i++);
	outb(INTR_ENAB(COM0_ADDR), iTX_ENAB|iRX_ENAB);
}

int
com_getc(
	boolean_t	wait)
{
	unsigned char	c = 0;

	outb(INTR_ENAB(COM0_ADDR), 0);
	while (!(inb(LINE_STAT(COM0_ADDR)) & iDR)) {
		if (!wait) {
			c = (unsigned char) -1;
			break;
		}
	}
	if (!c)
		c = inb(TXRX(COM0_ADDR));
	outb(INTR_ENAB(COM0_ADDR), iTX_ENAB|iRX_ENAB);
	if (c == K_CR)
	  	c = K_LF;
	return(c);
}

boolean_t
com_is_char(void)
{
	boolean_t	rc;

	outb(INTR_ENAB(COM0_ADDR), 0);
	if (!(inb(LINE_STAT(COM0_ADDR)) & iDR))
		rc = FALSE;
	else
		rc = TRUE;
	outb(INTR_ENAB(COM0_ADDR), iTX_ENAB|iRX_ENAB);
	return(rc);
}

#if MACH_KDB
/* Output the characters that are buffered for com1.  This is intended
   to be called from kdb, via "!comdrain".  When you enter kdb, via the
   keyboard or a breakpoint, there may be useful information that a user
   program has written to the console but that has not yet been
   displayed.  */
void comdrain(void);	/* improper prototype warning avoidance */

void
comdrain(void)
{
	int		c;
	struct tty	*tp = &com_tty[0];

	while (1) {
		tt_write_wakeup(tp);
		if (!tp->t_outq.c_cc)
			break;
		while ((c = getc(&tp->t_outq)) >= 0)
			com_putc(c);
	}
}
#endif /* MACH_KDB */

#endif /* NCOM */
