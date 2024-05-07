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
 * Revision 2.11.3.3  92/05/27  00:54:53  jeffreyh
 * 	       [stans@ssd.intel.com]
 * 	       exobyte tape support.
 * 
 * Revision 2.11.3.2  92/03/28  10:52:28  jeffreyh
 * 	Changes from TRUNK
 * 	[92/03/10  13:21:56  jeffreyh]
 * 
 * Revision 2.13  92/02/23  22:44:12  elf
 * 	Changed the interface of a number of functions not to
 * 	require the scsi_softc pointer any longer.  It was
 * 	mostly unused, now it can be found via tgt->masterno.
 * 	[92/02/22  19:31:18  af]
 * 
 * Revision 2.12  92/01/03  20:44:06  dbg
 * 	Added rz_devinfo().
 * 	[91/12/26  11:10:59  af]
 * 
 * Revision 2.11  91/10/09  16:16:57  af
 * 	Zero io_error before using the ior.
 * 
 * Revision 2.10  91/08/24  12:27:43  af
 * 	Pass along the ior in open calls.
 * 	Removed luna88k specialization, as the device headers have been
 * 	 rationalized. 
 * 	[91/08/02  03:51:40  af]
 * 
 * Revision 2.9  91/07/09  23:22:23  danner
 * 	Added gross ifdef luna88k to use <sd.h> instead of <scsi.h>. Will
 * 	 be fixed as soon as I figure out the configuration tools.
 * 	[91/07/09  11:05:28  danner]
 * 
 * Revision 2.8  91/06/19  11:56:54  rvb
 * 	File moved here from mips/PMAX since it is now "MI" code, also
 * 	used by Vax3100 and soon -- the omron luna88k.
 * 	[91/06/04            rvb]
 * 
 * Revision 2.7  91/05/14  17:26:11  mrt
 * 	Correcting copyright
 * 
 * Revision 2.6  91/05/13  06:04:14  af
 * 	Use a longer timeout on opens (exabytes are so slooooow to
 * 	get ready).  If we are still rewinding the tape make open
 * 	fail.  Start watchdog on first open, for those adapters
 * 	that need it.  Ask for sense data when unit does not
 * 	come online quickly enough. On close, invoke anything
 * 	target-specific that needs to.
 * 	[91/05/12  16:04:29  af]
 * 
 * Revision 2.5.1.1  91/03/29  17:21:00  af
 * 	Use a longer timeout on opens (exabytes are so slooooow to
 * 	get ready).  If we are still rewinding the tape make open
 * 	fail.  Start watchdog on first open, for those adapters
 * 	that need it.  Ask for sense data when unit does not
 * 	come online quickly enough. On close, invoke anything
 * 	target-specific that needs to.
 * 
 * Revision 2.5  91/02/05  17:43:37  mrt
 * 	Added author notices
 * 	[91/02/04  11:16:27  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:15:22  mrt]
 * 
 * Revision 2.4  90/12/20  16:59:59  jeffreyh
 * 	Do not use minphys(), we do not need to trim requests.
 * 	[90/12/11            af]
 * 
 * Revision 2.3  90/12/05  23:33:46  af
 * 	Cut&retry for requests that exceed the HBA's dma capacity.
 * 	[90/12/03  23:32:38  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:43:31  af
 * 	Created.
 * 	[90/10/21            af]
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
 *	File: rz.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/90
 *
 *	Top layer of the SCSI driver: interface with the MI side.
 */

/*
 * This file contains the code that is common to all scsi devices,
 * operations and/or behaviours specific to certain devices live
 * in the corresponding rz_mumble files.
 */

#include <scsi.h>
#include <mach/std_types.h>

#if	(NSCSI>0)

#include <kern/time_out.h>
#include <kern/spl.h>		/* spl definitions */

#include <device/buf.h>
#include <device/conf.h>
#include <device/ds_routines.h>
#include <device/misc_protos.h>
#include <sys/ioctl.h>

#include <scsi/scsi_defs.h>
#include <scsi/rz.h>
#include <kern/misc_protos.h>

/*
 *	Function prototypes for RZ/TZ internal services
 */

/*	Get SCSI device buffer				*/
static io_req_t	getbp(
			int		dev);

/*	Test if device is alive				*/
boolean_t	rz_check(
			int		dev,
			scsi_softc_t	**p_sc,
			target_info_t	**p_tgt);

/*	I/O control					*/
io_return_t	rz_ioctl(
			int		dev,
			int		cmd,
			dev_status_t	data,
			unsigned char	flag);

/*	RZ' private minphys (NOP in mach_kernel)	*/
void		rz_minphys(
			io_req_t	ior);

#if	0
/*	Null routine					*/
void	rz_reset(
			int		adaptor);
#endif	/* 0 */

#if	0
/*	Null routine	(always returns -1)		*/
int	rz_size(
			int		dev);
#endif	/* 0 */

/*	Call device-specific strategy routine		*/
void		rz_strategy(
			io_req_t	ior);

/*	SCSI tape I/O control				*/
io_return_t	tz_ioctl(
			int		dev,
			int		cmd,
			dev_status_t	data,
			unsigned char	flag);

struct scsi_io scsi_passthru;


/*static*/ boolean_t
rz_check(
	int		dev,
	scsi_softc_t	**p_sc,
	target_info_t	**p_tgt)
{
	if (rzcontroller(dev) >= NSCSI ||
	    (*p_sc = scsi_softc[rzcontroller(dev)]) == 0)
		return FALSE;

	*p_tgt = (*p_sc)->target[rzslave(dev)];

	if (!*p_tgt ||
	    !((*p_tgt)->flags&TGT_ALIVE))
		return FALSE;
	return TRUE;
}

/*
 * Open routine
 *
 * On tapes and other devices might have to wait a bit for
 * the unit to come alive. The following patchable variable
 * takes this into account
 */
int rz_open_timeout = 60;/* seconds */

io_return_t
rz_open(
	dev_t		dev,
	dev_mode_t	mode,
	io_req_t	ior)
{
	scsi_softc_t	*sc = 0;
	target_info_t	*tgt;
	scsi_ret_t	ret;
	register int	i;

	if (!rz_check(dev, &sc, &tgt)) {
		/*
		 * Probe it again: might have installed a new device
		 */
		if (!sc || !scsi_probe(sc, &tgt, rzslave(dev), rzlun(dev),ior))
			return D_NO_SUCH_DEVICE;
	}

	/* tapes do not wait for rewind to complete on close */
	if (tgt->ior && !(tgt->flags & TGT_ONLINE))
		return D_WOULD_BLOCK;

	if (scsi_debug)
		printf("opening %s%d..", (*tgt->dev_ops->driver_name)(TRUE), dev&0xff);

	if (sc->watchdog) {
		(*sc->watchdog)((struct watchdog_t *) tgt->hw_state);
		sc->watchdog = 0;
	}

	/*
	 * Don't do anything if a pass-through device is opened.
	 * The controlling program had better know what it's doing!
	 */
	if (rzpassthru(dev)) return 0;	/* XXX */

	/*
	 * Bring the unit online, retrying if necessary.
	 * If the target is spinning up we wait for it.
	 */
	if ( ! (tgt->flags & TGT_ONLINE)) {
		io_req_t	ior;

		io_req_alloc(ior);
		ior->io_next = 0;
		ior->io_count = 0;

		for (i = 0; i < rz_open_timeout; i++) {

			ior->io_op = IO_INTERNAL;
			ior->io_error = 0;
			ret = scsi_test_unit_ready(tgt, ior);

			if (ret == SCSI_RET_SUCCESS)
				break;

			if (ret == SCSI_RET_DEVICE_DOWN) {
				i = rz_open_timeout;
				break;
			}

			if (ret == SCSI_RET_NEED_SENSE) {

				ior->io_op = IO_INTERNAL;
				ior->io_count = 0;
				ior->io_residual = 0;
				tgt->ior = ior;
				scsi_request_sense(tgt, ior, 0);
				iowait(ior);

			}

			if (i == 5) printf("%s%d: %s\n", 
					   (*tgt->dev_ops->driver_name)(TRUE),
					   tgt->target_id,
					   "Waiting to come online..");
			timeout((timeout_fcn_t)wakeup, tgt, hz);
			await(tgt);
		}

		/* lock on removable media */
		if ((i != rz_open_timeout) && (tgt->flags & TGT_REMOVABLE_MEDIA)) {
			ior->io_op = IO_INTERNAL;
			/* too many dont support it. Sigh */
			tgt->flags |= TGT_OPTIONAL_CMD;
			(void) scsi_medium_removal( tgt, FALSE, ior);
			tgt->flags &= ~TGT_OPTIONAL_CMD;
		}

		io_req_free(ior);
		if (i == rz_open_timeout)
			return D_DEVICE_DOWN;
	}
	/*
	 * Perform anything open-time special on the device
	 */
	if (tgt->dev_ops->open) {
		ret = (*tgt->dev_ops->open)(tgt, ior);
		if (ret != SCSI_RET_SUCCESS) {
			 printf("%s%d: open failed x%x\n",
			 (*tgt->dev_ops->driver_name)(TRUE), dev&0xff, ret);
			return ret;
		}
	}
	tgt->flags |= TGT_ONLINE;
	return 0;	/* XXX */
}

void
rz_close(
	dev_t		dev)
{
	scsi_softc_t	*sc;
	target_info_t	*tgt;
	scsi_ret_t	ret;

	if (!rz_check(dev, &sc, &tgt))
		return;

	if (scsi_debug)
		printf("closing %s%d..", (*tgt->dev_ops->driver_name)(TRUE), dev&0xff);

	/*
	 * Don't do anything if a pass-through device is closed.
	 * The controlling program had better know what it's doing!
	 */
	if (rzpassthru(dev)) return;

	if (tgt->dev_ops->close) {
		ret = (*tgt->dev_ops->close)(dev, tgt);
		if (ret != SCSI_RET_SUCCESS) {
			 printf("%s%d: close failed x%x\n",
			 (*tgt->dev_ops->driver_name)(TRUE), dev&0xff, ret);
		}
	}
	if (tgt->flags & TGT_REMOVABLE_MEDIA) {
		io_req_t	ior;

		io_req_alloc(ior);
		ior->io_next = 0;
		ior->io_count = 0;
		ior->io_op = IO_INTERNAL;
		ior->io_error = 0;
		/* too many dont support it. Sigh */
		tgt->flags |= TGT_OPTIONAL_CMD;
		(void) scsi_medium_removal( tgt, TRUE, ior);
		tgt->flags &= ~TGT_OPTIONAL_CMD;
		io_req_free(ior);
		tgt->flags &= ~TGT_ONLINE;

	}

	return;
}

/* our own minphys */
void
rz_minphys(
	io_req_t	ior)
{
}

io_return_t
rz_read(
	dev_t		dev,
	io_req_t	ior)
{
	target_info_t	*tgt;

	tgt = scsi_softc[rzcontroller(dev)]->target[rzslave(dev)];

	return block_io(tgt->dev_ops->strategy, rz_minphys, ior);
}

io_return_t
rz_write(
	dev_t		dev,
	io_req_t	ior)
{
	target_info_t	*tgt;

	tgt = scsi_softc[rzcontroller(dev)]->target[rzslave(dev)];

	if (tgt->flags & TGT_READONLY)
		return D_INVALID_OPERATION;

	return block_io(tgt->dev_ops->strategy, rz_minphys, ior);
}

io_return_t
rz_get_status(
	dev_t		dev,
	int		flavor,
	dev_status_t	status,
	unsigned int	*status_count)
{
	target_info_t	*tgt;

	tgt = scsi_softc[rzcontroller(dev)]->target[rzslave(dev)];

	if (scsi_debug)
		printf("rz_get_status: x%x x%x x%x x%x\n",
			dev, flavor, status, *status_count);

	/*
	 * Return physical information for a pass-through device
	 */
	if (flavor == DIOCGPHYS) {
		struct scsi_phys *phys = (struct scsi_phys *)status;

		if (!rzpassthru(dev))
			return D_INVALID_OPERATION;

#if	iPSC386 || PARAGON860
		phys->slot       = node_self();
#else	/* iPSC386 || PARAGON860 */
		phys->slot       = 0;
#endif	/* iPSC386 || PARAGON860 */
		phys->controller = rzcontroller(dev);
		phys->target_id  = rzslave(dev);
		phys->lun        = rzlun(dev);

		*status_count = sizeof(struct scsi_phys)/sizeof(int);

		return D_SUCCESS;

	} else if (flavor == DIOCSCSI) {
		/*
		 * Not much to do here, everything was handled in rz_set_status
		 * Just copy out the scsi_passthru structure
		 */
		*(struct scsi_io *)status = *(&scsi_passthru);
		*status_count = sizeof(struct scsi_io)/sizeof(int);

		return D_SUCCESS;

	} else return (*tgt->dev_ops->get_status)(dev, tgt, flavor, status, status_count);
}

io_return_t
rz_set_status(
	dev_t		dev,
	int		flavor,
	dev_status_t	status,
	unsigned int	status_count)
{
	target_info_t	*tgt;

	tgt = scsi_softc[rzcontroller(dev)]->target[rzslave(dev)];

	if (scsi_debug)
		printf("rz_set_status: x%x x%x x%x x%x\n",
			dev, flavor, status, status_count);

	/*
	 * Handle generic SCSI device pass-through command
	 */
	if (flavor == DIOCSCSI) {
		struct scsi_io	*p = &scsi_passthru;
		int		datain = FALSE;
		unsigned int	i, cmd;
		unsigned int	old_timeout;
		io_req_t	ior;
		int		ret;

		*(&scsi_passthru) = *(struct scsi_io *)status;

		if (!rzpassthru(dev))
			return D_INVALID_OPERATION;

		if (status_count != sizeof(struct scsi_io) / sizeof(int))
			return D_INVALID_SIZE;

		if (p->cmd_count > 256)
			return D_INVALID_SIZE;

		if (p->buf_len > scsi_softc[tgt->masterno]->max_dma_data)
			return D_INVALID_SIZE;

		/* ++ HACK Alert */
/*		cmd = p->cdb[0]; */
		switch (p->direction) {

			case SCSI_NONE:		/* no data phase */
				cmd = SCSI_CMD_TEST_UNIT_READY;
				break;

			case SCSI_TO_MEM:	/* data in phase */
				cmd = SCSI_CMD_READ;
				datain = TRUE;
				break;

			case MEM_TO_SCSI:	/* data out phase */
				cmd = SCSI_CMD_WRITE;
				break;

			default:
				return D_INVALID_OPERATION;
		}
		/* -- HACK Alert */

		/* Build the I/O request */
		io_req_alloc(ior);
		ior->io_op = IO_INTERNAL | (datain) ? IO_READ : 0;
		ior->io_error = 0;
		ior->io_next = 0;
		ior->io_prev = 0;
		ior->io_unit = dev;
		ior->io_data = (io_buf_ptr_t)p->buf;
		ior->io_count = p->buf_len;
		ior->io_residual = 0;

		/* Grab the target */
		do {
			i = splbio();
			spinlock_lock(&tgt->target_lock);
			if (!tgt->ior)
				tgt->ior = ior;
			spinlock_unlock(&tgt->target_lock);
			splx(i);
		} while (tgt->ior != ior);		/* ahem */

		if (tgt->ior != ior) {
			io_req_free(ior);
			return D_ALREADY_OPEN;
		}

		/* Use the command descriptor LUN field to set the target LUN*/
		tgt->lun = (p->cdb[1] & SCSI_LUN_MASK) >> 5;

		/*
		 * Clear the SCSI command descriptor LUN field as recommended
		 * by the SCSI-2 specification (see section 6.2.2)
		 */
		p->cdb[1] &= ~SCSI_LUN_MASK;

		/* Copy the SCSI command to the target command pointer */
		tgt->cur_cmd = cmd;
		for (i = 0; i < p->cmd_count; i++) {
			tgt->cmd_ptr[i] = p->cdb[i];
		}

		if (p->cdb[0] == SCSI_CMD_FORMAT_UNIT) {
			/* save timeout */
			old_timeout = scsi_watchdog_period;

			/* 1 hour should be enough, I hope */
			scsi_watchdog_period =  1 * 60 * 60;
		}

		/* Execute the SCSI command */
		scsi_go_and_wait(tgt, i, (datain) ? p->buf_len : 0, ior);

		if (p->cdb[0] == SCSI_CMD_FORMAT_UNIT) {
			/* restore timeout */
			scsi_watchdog_period = old_timeout;
		}

		/* Save the amount of data actually transferred */
		p->buf_len = ior->io_count;

		/* Check the command results */
		ret = D_SUCCESS;
		if (ior->io_error != SCSI_RET_SUCCESS) {
			if (ior->io_error == SCSI_RET_NEED_SENSE) {
				p->status = SCSI_CHECK_CONDITION;
			} else if (ior->io_error == SCSI_RET_RETRY) {
				p->status = SCSI_BUSY;
			} else if (ior->io_error == SCSI_RET_DEVICE_DOWN) {
				ret = D_DEVICE_DOWN;
			} else {
				ret = D_IO_ERROR;
			}
		} else p->status = SCSI_GOOD;

		io_req_free(ior);

		return ret;

	} else 
		return (*tgt->dev_ops->set_status)(dev, tgt, flavor,
				     status, status_count);
}

/*
 *	Routine to return information to kernel.
 */
io_return_t
rz_devinfo(
	dev_t		dev,
	int		flavor,
	char		*info)
{
	scsi_softc_t	*sc;
	target_info_t	*tgt;
	io_return_t	result;

	if (!rz_check(dev, &sc, &tgt))
		return D_NO_SUCH_DEVICE;

	result = D_SUCCESS;

	switch (flavor) {
	case D_INFO_BLOCK_SIZE:
		if (rzpassthru(dev)) 
		        *((unsigned int *) info) = RZ_DEFAULT_BSIZE;
		else 
                        *((unsigned int *) info) = tgt->block_size;
		break;
	default:
		result = D_INVALID_OPERATION;
	}

	return(result);
}

#if	0
int
rz_size(
	int	dev);
{	
	return -1;
}

void
rz_reset(adaptor)
	int	adaptor;
{}
#endif

#endif	/* (NSCSI>0) */
