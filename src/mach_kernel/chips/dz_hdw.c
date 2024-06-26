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
 * Revision 2.11  91/08/24  11:51:49  af
 * 	Spl for 3min, padding of regmap here, a lot of code migrated
 * 	elsewhere to become chip-indep, pseudo-dma is gone.
 * 	Did not think I changed so much, oops.
 * 	[91/08/02  02:31:34  af]
 * 
 * Revision 2.10  91/06/25  20:53:35  rpd
 * 	Tweaks to make gcc happy.
 * 	[91/06/25            rpd]
 * 
 * Revision 2.9  91/06/19  11:47:32  rvb
 * 	mips->DECSTATION; vax->VAXSTATION
 * 	[91/06/12  14:01:30  rvb]
 * 
 * 	File moved here from mips/PMAX since it tries to be generic;
 * 	it is used on the PMAX and the Vax3100.
 * 	[91/06/04            rvb]
 * 
 * Revision 2.8  91/05/14  17:21:08  mrt
 * 	Correcting copyright
 * 
 * Revision 2.7  91/05/13  06:03:44  af
 * 	Made use of modem control code optional: patch dz_uses_modem_control
 * 	if you need it, or call dz_set_modem_control().
 * 	[91/05/12  16:06:26  af]
 * 
 * Revision 2.6  91/02/14  14:33:44  mrt
 * 	In interrupt routine, drop priority as now required.
 * 	[91/02/12  12:45:12  af]
 * 
 * 	Modified dz_param never to use 7bits per char.
 * 	Now we can use the serial lines even in non-raw mode,
 * 	which means we can login, for instance.
 * 	[90/12/31            af]
 * 
 * Revision 2.5  91/02/05  17:40:35  mrt
 * 	Added author notices
 * 	[91/02/04  11:13:03  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:10:54  mrt]
 * 
 * Revision 2.4  91/01/08  16:18:17  rpd
 * 	Modified dz_param never to use 7bits per char.
 * 	Now we can use the serial lines even in non-raw mode,
 * 	which means we can login, for instance.
 * 	[90/12/31            af]
 * 
 * Revision 2.3  90/12/05  23:31:02  af
 * 	Extensive additions for modem support, pmaxen.
 * 	Still to be tested: autoanswer modems.
 * 	[90/12/03  23:16:10  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:37:41  af
 * 	Created, from the DEC specs:
 * 	"DECstation 3100 Desktop Workstation Functional Specification"
 * 	Workstation Systems Engineering, Palo Alto, CA. Aug 28, 1990.
 * 	and
 * 	"DECstation 5000/200 KN02 System Module Functional Specification"
 * 	Workstation Systems Engineering, Palo Alto, CA. Aug 27, 1990.
 * 	[90/09/03            af]
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
 *	File: dz_hdw.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	9/90
 *
 *	Hardware-level operations for the DZ Serial Line Driver
 */
#include <dz_.h>
#if	NDZ_ > 0
#include <bm.h>
#include <platforms.h>

#include <mach_kdb.h>

#include <machine/machparam.h>		/* spl definitions */

#include <sys/types.h>
#include <device/io_req.h>
#include <device/tty.h>

#include <chips/busses.h>
#include <chips/screen_defs.h>
#include <chips/serial_defs.h>

#include <chips/dz_7085.h>


#ifdef	DECSTATION
#include <mips/mips_cpu.h>
#include <mips/PMAX/kn01.h>
#define	DZ_REGS_DEFAULT		(caddr_t)PHYS_TO_K1SEG(KN01_SYS_DZ)
#define	PAD(n)			char n[6];
#endif	/*DECSTATION*/

#ifdef	VAXSTATION
#define	DZ_REGS_DEFAULT		0
#define wbflush()
#define check_memory(addr,dow)  ((dow) ? wbadaddr(addr,4) : badaddr(addr,4))
#define	PAD(n)			char n[2];
#endif	/*VAXSTATION*/

#ifndef	PAD
#define	PAD(n)
#endif

typedef struct {
	volatile unsigned short	dz_csr;		/* Control and Status */
								PAD(pad0)
	volatile unsigned short	dz_rbuf;	/* Rcv buffer (RONLY) */
								PAD(pad1)
	volatile unsigned short	dz_tcr;		/* Xmt control (R/W)*/
	 							PAD(pad2)
	volatile unsigned short	dz_tbuf;	/* Xmt buffer (WONLY)*/
#	define 			dz_lpr dz_rbuf	/* Line parameters (WONLY)*/
#	define 			dz_msr dz_tbuf	/* Modem status (RONLY)*/
								PAD(pad3)
} dz_padded_regmap_t;


/* this is ok both for rcv (char) and xmt (csr) */
#define	LINEOF(x)	(((x) >> 8) & 0x3)

/*
 * Driver status
 */
struct dz7085_softc {
	dz_padded_regmap_t	*regs;
	unsigned short	breaks;
	unsigned short	fake;	/* missing rs232 bits */
	int		polling_mode;
	unsigned short	prev_msr;
	char		softCAR;
} dz7085_softc_data[NDZ_];

typedef struct dz7085_softc *dz7085_softc_t;

dz7085_softc_t	dz7085_softc[NDZ_];

static void		check_car();
static void		check_ring();

dz7085_softCAR(unit, line, on)
{
	if (on)
		dz7085_softc[unit]->softCAR |= 1<<line;
	else
		dz7085_softc[unit]->softCAR &= ~(1 << line);
}

static
short	dz7085_speeds[] =
	{ 0, DZ_LPAR_50, DZ_LPAR_75, DZ_LPAR_110, DZ_LPAR_134_5, DZ_LPAR_150,
	  0, DZ_LPAR_300, DZ_LPAR_600, DZ_LPAR_1200, DZ_LPAR_1800, DZ_LPAR_2400,
	  DZ_LPAR_4800, DZ_LPAR_9600, DZ_LPAR_MAX_SPEED, 0 };


/*
 * Definition of the driver for the auto-configuration program.
 */

int	dz7085_probe(), dz7085_attach(), dz7085_intr();

caddr_t	dz7085_std[NDZ_] = { DZ_REGS_DEFAULT, };
struct	bus_device *dz7085_info[NDZ_];
struct	bus_driver dz_driver = 
        { dz7085_probe, 0, dz7085_attach, 0, dz7085_std, "dz", dz7085_info,};

/*
 * Adapt/Probe/Attach functions
 */

static boolean_t 	dz7085_full_modem = FALSE;
boolean_t		dz7085_uses_modem_control = FALSE;/* patch this with adb */

set_dz_address( unit, regs, has_modem)
	caddr_t		regs;
	boolean_t	has_modem;
{
	extern int	dz7085_probe(), dz7085_param(), dz7085_start(),
			dz7085_putc(), dz7085_getc(),
			dz7085_pollc(), dz7085_mctl(), dz7085_softCAR();

	dz7085_std[unit] = regs;
	dz7085_full_modem = has_modem & dz7085_uses_modem_control;

	/* Do this here */
	console_probe		= dz7085_probe;
	console_param		= dz7085_param;
	console_start		= dz7085_start;
	console_putc		= dz7085_putc;
	console_getc		= dz7085_getc;
	console_pollc		= dz7085_pollc;
	console_mctl		= dz7085_mctl;
	console_softCAR		= dz7085_softCAR;

}

dz7085_probe( xxx, ui)
	struct bus_device *ui;
{
	int             unit = ui->unit;
	dz7085_softc_t      sc;
	register int	cntr;
	register dz_padded_regmap_t	*regs;

	static int probed_once = 0;

	regs = (dz_padded_regmap_t *)dz7085_std[unit];	/* like the old days! */
	if (regs == 0)
		return 0;
	/*
	 * If this is not there we are toast 
	 */
	if (check_memory(regs, 0))
		return 0;

	if (probed_once++)
		return 1;

	sc = &dz7085_softc_data[unit];
	dz7085_softc[unit] = sc;
	sc->regs = regs;

	for (cntr = unit*NDZ_LINE; cntr < NDZ_LINE*(unit+1); cntr++) {
		console_tty[cntr]->t_addr = (char*)regs;
	}

	/* pmaxen et al. lack many modem bits */
	dz7085_set_modem_control(sc, dz7085_full_modem);

	regs->dz_tcr = 0;/* disable all lines, drop RTS,DTR */
	return 1;
}

boolean_t dz7085_timer_started = FALSE;

dz7085_attach(ui)
	register struct bus_device *ui;
{
	int unit = ui->unit;
	extern dz7085_scan();
	extern int tty_inq_size;
	int i;

	/* We only have 4 ttys, but always at 9600
	 * Give em a lot of room
	 */
	tty_inq_size = 2048;
	for (i = 0; i < (NDZ_*NDZ_LINE); i++)
		ttychars(console_tty[i]);

	if (!dz7085_timer_started) {
		dz7085_timer_started = TRUE;
		dz7085_scan();
	}

#if	NBM > 0
	if (SCREEN_ISA_CONSOLE()) {
		printf("\n sl0: "); lk201_attach(0, unit);
		printf("\n sl1: "); mouse_attach(0, unit);
		printf("\n sl2: \n sl3: ");
		if (rcline == 3) printf("( rconsole )");
	} else {
#endif	/*NBM > 0*/
		printf("\n sl0:\n sl1:\n sl2:\n sl3: ( alternate console )");
#if	NBM > 0
	}
#endif
}

/*
 * Would you like to make a phone call ?
 */
dz7085_set_modem_control(sc, on)
	dz7085_softc_t      sc;
	boolean_t	on;
{
	if (on)
		/* your problem if the hardware then is broke */
		sc->fake = 0;
	else
		sc->fake = DZ_MSR_CTS3|DZ_MSR_DSR3|DZ_MSR_CD3|
			   DZ_MSR_CTS2|DZ_MSR_CD2;
}

/*
 * Polled I/O (debugger)
 */
dz7085_pollc(unit, on)
	boolean_t		on;
{
	dz7085_softc_t		sc = dz7085_softc[unit];

	if (on) {
		sc->polling_mode++;
#if	NBM > 0
		screen_on_off(unit, TRUE);
#endif	NBM > 0
	} else
		sc->polling_mode--;
}

/*
 * Interrupt routine
 */
dz_intr(unit,spllevel)
{
	dz7085_softc_t		sc = dz7085_softc[unit];
	register dz_padded_regmap_t	*regs = sc->regs;
	register short		csr;

	csr = regs->dz_csr;

	if (csr & DZ_CSR_TRDY) {
		register int             c;

		c = cons_simple_tint(unit*NDZ_LINE + LINEOF(csr), FALSE);
		if (c == -1) {
			/* no more data for this line */
			regs->dz_tcr &= ~(1 << LINEOF(csr));
			c = cons_simple_tint(unit*NDZ_LINE + LINEOF(csr), TRUE);
			/* because funny race possible ifnot */
		}
		if (c != -1) {
			regs->dz_tbuf = (c & 0xff) | sc->breaks;
			/* and leave it enabled */
		}
	}
	if (sc->polling_mode)
		return;

	while (regs->dz_csr & DZ_CSR_RDONE) {
		short           c = regs->dz_rbuf;
		int		oldspl;

#ifdef	DECSTATION
		oldspl = splx(spllevel);
		cons_simple_rint(unit*NDZ_LINE+LINEOF(c), LINEOF(c),
				c&0xff, c&0xff00);
		splx(oldspl);
#endif	/*DECSTATION*/
#ifdef	VAXSTATION
		cons_simple_rint(unit*NDZ_LINE+LINEOF(c), LINEOF(c),
				c&0xff, c&0xff00);
#endif	/*VAXSTATION*/
	}
}

/*
 * Start transmission on a line
 */
dz7085_start(tp)
	struct tty *tp;
{
	register dz_padded_regmap_t	*regs;
	register int		line;

	line = minor(tp->t_dev);

	regs = (dz_padded_regmap_t*)tp->t_addr;
	regs->dz_tcr |= (1<<(line&3));

	/* no, we do not need a char out to interrupt */
}

/*
 * Get a char from a specific DZ line
 */
dz7085_getc( unit, line, wait, raw )
	boolean_t	wait;
	boolean_t	raw;
{
	dz7085_softc_t      sc = dz7085_softc[unit];
	int             s = spltty();
	register dz_padded_regmap_t *regs = sc->regs;
	unsigned short  c;
	int             rl;

again:
	/*
	 * wait till something in silo 
	 */
	while ((regs->dz_csr & DZ_CSR_RDONE) == 0 && wait)
		delay(10);
	c = regs->dz_rbuf;

	/*
	 * check if right line. For keyboard, rconsole is ok too 
	 */
	rl = LINEOF(c);
	if (wait && (line != rl) &&
	    !((line == DZ_LINE_KEYBOARD) && rcline == rl))
		goto again;
	/*
	 * bad chars not ok 
	 */
	if ((c & (DZ_RBUF_PERR | DZ_RBUF_OERR | DZ_RBUF_FERR)) && wait)
		goto again;

	splx(s);

	/*
	 * if nothing found return -1 
	 */
	if ( ! (c & DZ_RBUF_VALID))
		return -1;

#if	NBM > 0
	if ((rl == DZ_LINE_KEYBOARD) && !raw && SCREEN_ISA_CONSOLE())
		return lk201_rint(SCREEN_CONS_UNIT(), c, wait, sc->polling_mode);
	else
#endif	NBM > 0
		return c & DZ_RBUF_CHAR;
}

/*
 * Put a char on a specific DZ line
 */
dz7085_putc( unit, line, c )
{
	dz7085_softc_t      sc = dz7085_softc[unit];
	register dz_padded_regmap_t *regs = sc->regs;
	int             s = spltty();

	/*
	 * do not change the break status of other lines 
	 */
	c = (c & 0xff) | sc->breaks;

	/*
	 * Xmit line info only valid if TRDY,
	 * but never TRDY if no xmit enabled
	 */
	if ((regs->dz_tcr & DZ_TCR_LNENB) == 0)
		goto select_it;

	while ((regs->dz_csr & DZ_CSR_TRDY) == 0)
		delay(100);

	/*
	 * see if by any chance we are already on the right line 
	 */
	if (LINEOF(regs->dz_csr) == line)
		regs->dz_tbuf = c;
	else {
		unsigned short tcr;
select_it:
		tcr = regs->dz_tcr;
		regs->dz_tcr = (1 << line) | (tcr & 0xff00);
		wbflush();

		do
			delay(2);
		while ((regs->dz_csr & DZ_CSR_TRDY) == 0 ||
		       (LINEOF(regs->dz_csr) != line));

		regs->dz_tbuf = c;
		wbflush();

		/* restore previous settings */
		regs->dz_tcr = tcr;
	}

	splx(s);
}


dz7085_param(tp, line)
	register struct tty	*tp;
	register int		line;
{
	register dz_padded_regmap_t *regs;
	register int lpr;
 
	line = minor(tp->t_dev);
	regs = dz7085_softc[line/NDZ_LINE]->regs;

	/*
	 * Do not let user fool around with kbd&mouse
	 */
#if	NBM > 0
	if (screen_captures(line)) {
		tp->t_ispeed = tp->t_ospeed = B4800;
		tp->t_flags |= TF_LITOUT;
	}
#endif	NBM > 0
	regs->dz_csr = DZ_CSR_MSE|DZ_CSR_RIE|DZ_CSR_TIE;
	if (tp->t_ispeed == 0) {
		(void) (*console_mctl)(tp->t_dev, TM_HUP, DMSET);	/* hang up line */
		return;
	}
/* 19200/38400 here */
	lpr = dz7085_speeds[tp->t_ispeed] | (line&DZ_LPAR_LINE) | DZ_LPAR_ENABLE;
	lpr |= DZ_LPAR_8BITS;

	if ((tp->t_flags & (TF_ODDP|TF_EVENP)) == TF_ODDP)
		lpr |= DZ_LPAR_ODD_PAR;

	if (tp->t_ispeed == B110)
		lpr |= DZ_LPAR_STOP;
	regs->dz_lpr = lpr;
}
 
/*
 * This is a total mess: not only are bits spread out in
 * various registers, but we have to fake some for pmaxen.
 */
dz7085_mctl(dev, bits, how)
	dev_t dev;
	int bits, how;
{
	register dz_padded_regmap_t *regs;
	register int unit;
	register int tcr, msr, brk, n_tcr, n_brk;
	int b, s;
	dz7085_softc_t      sc;

	unit = minor(dev);

	/* no modem support on lines 0 & 1 */
/* XXX break on 0&1 */
	if ((unit & 2) == 0)
		return TM_LE|TM_DTR|TM_CTS|TM_CAR|TM_DSR;

	b = 1 ^ (unit & 1);	/* line 2 ? */
	
	sc = dz7085_softc[unit>>2];
	regs = sc->regs;
	s = spltty();

	tcr = ((regs->dz_tcr | (sc->fake>>4)) & 0xf00) >> (8 + b*2);
	brk = (sc->breaks >> (8 + (unit&3))) & 1;	/* THE break bit */

	n_tcr = (bits & (TM_RTS|TM_DTR)) >> 1;
	n_brk = (bits & TM_BRK) >> 9;

	/* break transitions, must 'send' a char out */
	bits = (brk ^ n_brk) & 1;

	switch (how) {
	case DMSET:
		tcr = n_tcr;
		brk = n_brk;
		break;

	case DMBIS:
		tcr |= n_tcr;
		brk |= n_brk;
		break;

	case DMBIC:
		tcr &= ~n_tcr;
		brk = 0;
		break;

	case DMGET:
		msr = ((regs->dz_msr|sc->fake) & 0xf0f) >> (b*8);
		(void) splx(s);
		return  (tcr<<1)|/* DTR, RTS */
			((msr&1)<<5)|/* CTS */
			((msr&2)<<7)|/* DSR */
			((msr&0xc)<<4)|/* CD, RNG */
			(brk << 9)|/* BRK */
			TM_LE;
	}
	n_tcr =	(regs->dz_tcr & ~(3 << (8 + b*2))) |
		(tcr << (8 + b*2));

	regs->dz_tcr = n_tcr;
	sc->fake = (sc->fake & 0xf0f) | (n_tcr<<4&0xf000);

	sc->breaks = (sc->breaks & ~(1 << (8 + (unit&3)))) |
		    (brk << (8 + (unit&3)));
	if(bits) (*console_putc)( unit>>2, unit&3, 0);/* force break, now */
	(void) splx(s);
	return 0;/* useless to compute it */
}

/*
 * Periodically look at the CD signals:
 * they do not generate interrupts.
 */
dz7085_scan()
{
	register i;
	register dz_padded_regmap_t *regs;
	register msr;
	register struct tty *tp;
 
	for (i = 0; i < NDZ_; i++) {
		dz7085_softc_t      sc = dz7085_softc[i];
		register int	temp;

		if (sc == 0)
			continue;
		regs = sc->regs;

		tp = console_tty[i * NDZ_LINE];

		msr = regs->dz_msr | (sc->fake & 0xf0f);
		if (temp = sc->softCAR) {
			if (temp & 0x4)
				msr |= DZ_MSR_CD2;
			if (temp & 0x8)
				msr |= DZ_MSR_CD3;
		}

		/* Lines 0 and 1 have carrier on by definition */
		/* [horrid casts cuz compiler stupid] */
		check_car((char*)tp + 0*sizeof(struct tty), 1);
		check_car((char*)tp + 1*sizeof(struct tty), 1);
		check_car((char*)tp + 2*sizeof(struct tty), msr & DZ_MSR_CD2);
		check_car((char*)tp + 3*sizeof(struct tty), msr & DZ_MSR_CD3);

		/* nothing else to do if no msr transitions */
		if ((temp = sc->prev_msr) == msr)
			continue;
		else
			sc->prev_msr = msr;

		/* see if we have an incoming call */
#define	RING	(DZ_MSR_RI2|DZ_MSR_RI3)
		if ((msr & RING) != (temp & RING)) {
/*printf("%s %x->%x\n", "ET Phone RI", temp & RING, msr & RING);*/
			check_ring((char*)tp + 2*sizeof(struct tty),
				msr & DZ_MSR_RI2, temp & DZ_MSR_RI2);
			check_ring((char*)tp + 3*sizeof(struct tty),
				msr & DZ_MSR_RI3, temp & DZ_MSR_RI3);
		}
	}
	timeout(dz7085_scan, (caddr_t)0, 2*hz);
}

static dz7085_hup(tp)
	register struct tty *tp;
{
	(*console_mctl)(tp->t_dev, TM_DTR, DMBIC);
}

static void check_car(tp, car)
	register struct tty *tp;
{
	if (car) {
		/* cancel modem timeout if need to */
		if (car & (DZ_MSR_CD2|DZ_MSR_CD3))
			untimeout(dz7085_hup, (caddr_t)tp);

		/* I think this belongs in the MI code */
		if (tp->t_state & TS_WOPEN)
			tp->t_state |= TS_ISOPEN;
		/* carrier present */
		if ((tp->t_state & TS_CARR_ON) == 0)
			(void)ttymodem(tp, 1);
	} else if ((tp->t_state&TS_CARR_ON) && ttymodem(tp, 0) == 0)
		(*console_mctl)( tp->t_dev, TM_DTR, DMBIC);
}

int	dz7085_ring_timeout	= 60;	/* seconds, patchable */

static void check_ring(tp, ring, oring)
	register struct tty *tp;
{
	if (ring == oring)
		return;
	if (ring) {
		(*console_mctl)( tp->t_dev, TM_DTR, DMBIS);
		/* give it ample time to find the right carrier */
		timeout(dz7085_hup, (caddr_t)tp, dz7085_ring_timeout*hz);
	}
}
#endif	NDZ_ > 0

