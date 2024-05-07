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
 * Revision 2.2.2.2  92/04/30  12:00:21  bernadat
 * 	Changes from TRUNK:
 * 		On bad errors, be loquacious and decode all sense data.
 * 		Do not retry IO_INTERNAL operations, cuz we donno what it is.
 * 		[92/04/06            af]
 * 
 * Revision 2.2.2.1  92/03/28  10:15:22  jeffreyh
 * 	Pick up changes from MK71
 * 	[92/03/20  13:31:50  jeffreyh]
 * 
 * Revision 2.3  92/02/23  22:44:15  elf
 * 	Changed the interface of a number of functions not to
 * 	require the scsi_softc pointer any longer.  It was
 * 	mostly unused, now it can be found via tgt->masterno.
 * 	[92/02/22  19:30:48  af]
 * 
 * Revision 2.2  91/08/24  12:27:47  af
 * 	Created.
 * 	[91/08/02  03:52:11  af]
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
 *	File: rz_cpu.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	7/91
 *
 *	Top layer of the SCSI driver: interface with the MI.
 *	This file contains operations specific to CPU-like devices.
 *
 * We handle here the case of simple devices which do not use any
 * sophisticated host-to-host communication protocol, they look
 * very much like degenerative cases of TAPE devices.
 *
 * For documentation and debugging, we also provide code to act like one.
 */

#include <mach/std_types.h>

#include <scsi/scsi_defs.h>
#include <scsi/rz.h>

#include <kern/spl.h>		/* spl definitions */
#include <kern/misc_protos.h>

#include <device/ds_routines.h>

/*
 *	Function prototypes for internal functions.
 */

/*	Set SCSI CPU to act dumb for debugging		*/
void	sccpu_act_as_target(
		target_info_t	*self);

/*
 * This function decides which 'protocol' we well speak
 * to a cpu target. For now the decision is left to a
 * global var. XXXXXXX
 */
extern struct scsi_devsw_t scsi_host;
struct scsi_devsw_t	*scsi_cpu_protocol = /* later &scsi_host*/
			&scsi_devsw[SCSI_CPU];

void
sccpu_new_initiator(
	target_info_t	*self,
	target_info_t	*initiator)
{
	initiator->dev_ops = scsi_cpu_protocol;
	if (initiator == self) {
		self->flags = TGT_DID_SYNCH|TGT_FULLY_PROBED|TGT_ONLINE|
			     TGT_ALIVE|TGT_US;
		self->dev_info.cpu.req_pending = FALSE;
	} else {
		initiator->flags = TGT_DID_SYNCH|TGT_FULLY_PROBED|TGT_ONLINE|
			     TGT_ALIVE;
		initiator->dev_info.cpu.req_pending = TRUE;
	}
}

void
sccpu_strategy(
	register io_req_t	ior)
{
	target_info_t  *tgt;
	register scsi_softc_t	*sc;
	scsi_ret_t      ret;
	register int    i = ior->io_unit;
	io_req_t	head, tail;
	spl_t		last_spl;

	/*
	 * Don't do anything for a pass-through device, just acknowledge
	 * the request.  This gives us a null SCSI device.
	 */
	if (rzpassthru(i)) {
		ior->io_residual = 0;
		iodone(ior);
		return;
	}

	sc = scsi_softc[rzcontroller(i)];
	tgt = sc->target[rzslave(i)];

	ior->io_next = 0;
	ior->io_prev = 0;

	last_spl = splbio();
	spinlock_lock(&tgt->target_lock);
	if (head = tgt->ior) {
		/* Queue it up at the end of the list */
		if (tail = head->io_prev)
			tail->io_next = ior;
		else
			head->io_next = ior;
		head->io_prev = ior;	/* tail pointer */
		spinlock_unlock(&tgt->target_lock);
	} else {
		/* Was empty, start operation */
		tgt->ior = ior;
		spinlock_unlock(&tgt->target_lock);
		sccpu_start( tgt, FALSE);
	}
	splx(last_spl);
}

void
sccpu_start(
	target_info_t	*tgt,
	boolean_t	done)
{
	io_req_t		head, ior = tgt->ior;
	scsi_ret_t		ret;

	/* this is to the doc & debug code mentioned in the beginning */
	if (!done && tgt->dev_info.cpu.req_pending) {
		sccpu_act_as_target( tgt);
		return;
	}

	if (ior == 0)
		return;

	if (done) {

		/* see if we must retry */
		if ((tgt->done == SCSI_RET_RETRY) &&
		    ((ior->io_op & IO_INTERNAL) == 0)) {
			delay(1000000);/*XXX*/
			goto start;
		} else
		/* got a bus reset ? shouldn't matter */
		if ((tgt->done == (SCSI_RET_ABORTED|SCSI_RET_RETRY)) &&
		    ((ior->io_op & IO_INTERNAL) == 0)) {
			goto start;
		} else

		/* check completion status */

		if ((tgt->cur_cmd == SCSI_CMD_REQUEST_SENSE) &&
		    (!rzpassthru(ior->io_unit))) {
			scsi_sense_data_t *sns;

			ior->io_op = ior->io_temporary;
			ior->io_error = D_IO_ERROR;
			ior->io_op |= IO_ERROR;

			sns = (scsi_sense_data_t *)tgt->cmd_ptr;
			if (scsi_debug)
				scsi_print_sense_data(sns);

			if (scsi_check_sense_data(tgt, sns)) {
			    if (sns->u.xtended.ili) {
				if (ior->io_op & IO_READ) {
				    int residue;

				    residue =	sns->u.xtended.info0 << 24 |
						sns->u.xtended.info1 << 16 |
						sns->u.xtended.info2 <<  8 |
						sns->u.xtended.info3;
				    if (scsi_debug)
					printf("Cpu Short Read (%d)\n", residue);
				    /*
				     * NOTE: residue == requested - actual
				     * We only care if > 0
				     */
				    if (residue < 0) residue = 0;/* sanity */
				    ior->io_residual += residue;
				    ior->io_error = 0;
				    ior->io_op &= ~IO_ERROR;
				    /* goto ok */
				}
			    }
			}
		}

		else if ((tgt->done != SCSI_RET_SUCCESS) &&
			 (!rzpassthru(ior->io_unit))) {

		    if (tgt->done == SCSI_RET_NEED_SENSE) {

			ior->io_temporary = ior->io_op;
			ior->io_op = IO_INTERNAL;
			if (scsi_debug)
				printf("[NeedSns x%x x%x]", ior->io_residual, ior->io_count);
			scsi_request_sense(tgt, ior, 0);
			return;

		    } else if (tgt->done == SCSI_RET_RETRY) {
			/* only retry here READs and WRITEs */
			if ((ior->io_op & IO_INTERNAL) == 0) {
				ior->io_residual = 0;
				goto start;
			} else{
				ior->io_error = D_WOULD_BLOCK;
				ior->io_op |= IO_ERROR;
			}
		    } else {
			ior->io_error = D_IO_ERROR;
			ior->io_op |= IO_ERROR;
		    }
		}

		if (scsi_debug)
			printf("[Resid x%x]", ior->io_residual);

		/* If this is a pass-through device, save the target result */
		if (rzpassthru(ior->io_unit)) ior->io_error = tgt->done;

		/* dequeue next one */
		head = ior;

		spinlock_lock(&tgt->target_lock);
		ior = head->io_next;
		tgt->ior = ior;
		if (ior)
			ior->io_prev = head->io_prev;
		spinlock_unlock(&tgt->target_lock);

		iodone(head);

		if (ior == 0)
			return;
	}
	ior->io_residual = 0;
start:
	tgt->lun = rzlun(ior->io_unit);
	if (ior->io_op & IO_READ) {
		tgt->flags &= ~TGT_WRITTEN_TO;
		scsi_receive( tgt, ior );
	} else if ((ior->io_op & IO_INTERNAL) == 0) {
		tgt->flags |= TGT_WRITTEN_TO;
		scsi_send( tgt, ior );

	}
}

/*
 * This is a simple code to make us act as a dumb
 * processor type.  Use for debugging only.
 */
static struct io_req	sccpu_ior;

void
sccpu_act_as_target(
	target_info_t	*self)
{
	target_info_t	*tgt;
	static char	buffer[512] = "For debugging only";

	self->dev_info.cpu.req_pending = FALSE;
/*	tgt = sc->target[self->dev_info.cpu.req_id];*/
	sccpu_ior.io_next = 0;
	sccpu_ior.io_count = (512 < self->dev_info.cpu.req_len) ?
		512 : self->dev_info.cpu.req_len;
	sccpu_ior.io_data = buffer;
	if ((self->cur_cmd = self->dev_info.cpu.req_cmd) == SCSI_CMD_RECEIVE) {
		sccpu_ior.io_op = IO_READ;
	} else {
		sccpu_ior.io_op = IO_WRITE;
	}
	self->ior = &sccpu_ior;
}
