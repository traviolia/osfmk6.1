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
 * Revision 2.3  91/08/24  12:25:10  af
 * 	Moved padding of regmap in impl file.
 * 	[91/08/02  04:22:39  af]
 * 
 * Revision 2.2  91/06/19  16:28:35  rvb
 * 	From the NCR data sheets
 * 	"NCR 5380 Family, SCSI Protocol Controller Data Manual"
 * 	NCR Microelectronics Division, Colorado Spring, 6/98 T01891L
 * 	[91/04/21            af]
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
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
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
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 */
/*
 *	File: scsi_5380.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	5/91
 *
 *	Defines for the NCR 5380 (SCSI chip), aka Am5380
 */

/*
 * Register map
 */

typedef struct {
	volatile unsigned char	sci_data;	/* r:  Current data */
#define			sci_odata sci_data	/* w:  Out data */
	volatile unsigned char	sci_icmd;	/* rw: Initiator command */
	volatile unsigned char	sci_mode;	/* rw: Mode */
	volatile unsigned char	sci_tcmd;	/* rw: Target command */
	volatile unsigned char	sci_bus_csr;	/* r:  Bus Status */
#define			sci_sel_enb sci_bus_csr	/* w:  Select enable */
	volatile unsigned char	sci_csr;	/* r:  Status */
#define			sci_dma_send sci_csr	/* w:  Start dma send data */
	volatile unsigned char	sci_idata;	/* r:  Input data */
#define			sci_trecv sci_idata	/* w:  Start dma receive, target */
	volatile unsigned char	sci_iack;	/* r:  Interrupt Acknowledge  */
#define			sci_irecv sci_iack	/* w:  Start dma receive, initiator */
} sci_regmap_t;


/*
 * Initiator command register
 */

#define SCI_ICMD_DATA		0x01		/* rw: Assert data bus   */
#define SCI_ICMD_ATN		0x02		/* rw: Assert ATN signal */
#define SCI_ICMD_SEL		0x04		/* rw: Assert SEL signal */
#define SCI_ICMD_BSY		0x08		/* rw: Assert BSY signal */
#define SCI_ICMD_ACK		0x10		/* rw: Assert ACK signal */
#define SCI_ICMD_LST		0x20		/* r:  Lost arbitration */
#define SCI_ICMD_DIFF	SCI_ICMD_LST		/* w:  Differential cable */
#define SCI_ICMD_AIP		0x40		/* r:  Arbitration in progress */
#define SCI_ICMD_TEST	SCI_ICMD_AIP		/* w:  Test mode */
#define SCI_ICMD_RST		0x80		/* rw: Assert RST signal */


/*
 * Mode register
 */

#define SCI_MODE_ARB		0x01		/* rw: Start arbitration */
#define SCI_MODE_DMA		0x02		/* rw: Enable DMA xfers */
#define SCI_MODE_MONBSY		0x04		/* rw: Monitor BSY signal */
#define SCI_MODE_DMA_IE		0x08		/* rw: Enable DMA complete interrupt */
#define SCI_MODE_PERR_IE	0x10		/* rw: Interrupt on parity errors */
#define SCI_MODE_PAR_CHK	0x20		/* rw: Check parity */
#define SCI_MODE_TARGET		0x40		/* rw: Target mode (Initiator if 0) */
#define SCI_MODE_BLOCKDMA	0x80		/* rw: Block-mode DMA handshake (MBZ) */


/*
 * Target command register
 */

#define SCI_TCMD_IO		0x01		/* rw: Assert I/O signal */
#define SCI_TCMD_CD		0x02		/* rw: Assert C/D signal */
#define SCI_TCMD_MSG		0x04		/* rw: Assert MSG signal */
#define SCI_TCMD_PHASE_MASK	0x07		/* r:  Mask for current bus phase */
#define SCI_TCMD_REQ		0x08		/* rw: Assert REQ signal */
#define	SCI_TCMD_LAST_SENT	0x80		/* ro: Last byte was xferred
						 *     (not on 5380/1) */

#define	SCI_PHASE(x)		SCSI_PHASE(x)

/*
 * Current (SCSI) Bus status
 */

#define SCI_BUS_DBP		0x01		/* r:  Data Bus parity */
#define SCI_BUS_SEL		0x02		/* r:  SEL signal */
#define SCI_BUS_IO		0x04		/* r:  I/O signal */
#define SCI_BUS_CD		0x08		/* r:  C/D signal */
#define SCI_BUS_MSG		0x10		/* r:  MSG signal */
#define SCI_BUS_REQ		0x20		/* r:  REQ signal */
#define SCI_BUS_BSY		0x40		/* r:  BSY signal */
#define SCI_BUS_RST		0x80		/* r:  RST signal */

#define	SCI_CUR_PHASE(x)	SCSI_PHASE((x)>>2)

/*
 * Bus and Status register
 */

#define SCI_CSR_ACK		0x01		/* r:  ACK signal */
#define SCI_CSR_ATN		0x02		/* r:  ATN signal */
#define SCI_CSR_DISC		0x04		/* r:  Disconnected (BSY==0) */
#define SCI_CSR_PHASE_MATCH	0x08		/* r:  Bus and SCI_TCMD match */
#define SCI_CSR_INT		0x10		/* r:  Interrupt request */
#define SCI_CSR_PERR		0x20		/* r:  Parity error */
#define SCI_CSR_DREQ		0x40		/* r:  DMA request */
#define SCI_CSR_DONE		0x80		/* r:  DMA count is zero */

