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
 * Revision 2.13.3.2  92/04/30  12:00:28  bernadat
 * 	Initialize d_partitions[MAXPARTITIONS] in all cases.
 * 	Changes from TRUNK:
 * 		On bad errors, be loquacious and decode all sense data.
 * 		Do not retry IO_INTERNAL operations, cuz we donno what it is.
 * 		[92/04/06            af]
 * 		With af, fixed berkeley label processing not to trash tgt->ior.
 * 		Use PARTITION_ABSOLUTE to read absolute sectors from the 
 * 		disk, by reading/writing on the absolute partition.
 * 		[92/04/01            rvb]
 * 
 * Revision 2.13.3.1  92/03/28  10:15:25  jeffreyh
 * 	Call machine dependant set_status/get_status
 * 	if flavor is not a known one.
 * 	[92/03/04            bernadat]
 * 	Changes from TRUNK
 * 	[92/03/10  13:22:02  jeffreyh]
 * 
 * Revision 2.14  92/02/23  22:44:21  elf
 * 	Changed the interface of a number of functions not to
 * 	require the scsi_softc pointer any longer.  It was
 * 	mostly unused, now it can be found via tgt->masterno.
 * 	Added DEV_GET_SIZE flavor for get status.
 * 	Disabled PERF code.
 * 	[92/02/22  19:06:12  af]
 * 
 * Revision 2.13  91/10/09  16:17:00  af
 * 	Do not tolerate a negative io_count (sanity).
 * 	[91/10/05  10:21:09  af]
 * 
 * Revision 2.12.1.1  91/09/01  17:18:33  af
 * 	Fixed blooper in setdisklabel(), now disk labels work on 3.0.
 * 	Zero io_error before using the ior.
 * 
 * Revision 2.12  91/08/24  12:27:52  af
 * 	Spl defs.  Search for label. MultiP locks.
 * 	[91/08/02  03:53:45  af]
 * 
 *	Bcopy casts.
 * 
 * Revision 2.11  91/07/09  23:22:28  danner
 * 	Upgraded to support new label technology.
 * 	[91/07/09  11:15:50  danner]
 * 
 * Revision 2.10  91/06/25  20:56:16  rpd
 * 	Tweaks to make gcc happy.
 * 
 * Revision 2.9  91/06/19  11:57:01  rvb
 * 	File moved here from mips/PMAX since it is now "MI" code, also
 * 	used by Vax3100 and soon -- the omron luna88k.
 * 	[91/06/04            rvb]
 * 
 * Revision 2.8  91/05/14  17:26:29  mrt
 * 	Correcting copyright
 * 
 * Revision 2.7  91/05/13  06:04:25  af
 * 	Revision 2.6.1.2  91/04/05  13:08:43  af
 * 	Fixed a type declaration, now an HBA with unlimited DMA
 * 	capability sez so by setting the max_dma_size to -1.
 * 
 * 	Revision 2.6.1.1  91/03/29  17:03:03  af
 * 	Use long forms of read/write commands on disks bigger than 1Gb,
 * 	which we automatically recognize from the read_capacity.
 * 	Allocate io_req_t records properly.
 * 	Check for transactions that aborted due to scsi bus resets.
 * 	Issue a request_sense on bad transactions, log soft errors
 * 	and only make fail bad errors.
 * 	[91/05/12  16:03:45  af]
 * 
 * Revision 2.6.1.2  91/04/05  13:08:43  af
 * 	Fixed a type declaration, now an HBA with unlimited DMA
 * 	capability sez so by setting the max_dma_size to -1.
 * 
 * Revision 2.6.1.1  91/03/29  17:03:03  af
 * 	Use long forms of read/write commands on disks bigger than 1Gb,
 * 	which we automatically recognize from the read_capacity.
 * 	Allocate io_req_t records properly.
 * 	Check for transactions that aborted due to scsi bus resets.
 * 	Issue a request_sense on bad transactions, log soft errors
 * 	and only make fail bad errors.
 * 
 * Revision 2.6  91/02/14  14:35:22  mrt
 * 	Added (optional and disabled) code for checksumming.
 * 	[91/02/12  12:58:27  af]
 * 
 * 	Bug in strategy routine: for xfers larger than
 * 	max_dma was not setting the io_count right at
 * 	the end of the transfer.  Now new fsck works.
 * 	[90/12/10  17:26:47  af]
 * 
 * Revision 2.5  91/02/05  17:43:47  mrt
 * 	Added author notices
 * 	[91/02/04  11:16:50  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:15:37  mrt]
 * 
 * Revision 2.4  90/12/20  17:01:09  jeffreyh
 * 	Bug in strategy routine: for xfers larger than
 * 	max_dma was not setting the io_count right at
 * 	the end of the transfer.  Now new fsck works.
 * 	[90/12/10  17:26:47  af]
 * 
 * Revision 2.3  90/12/05  23:33:57  af
 * 	Use and prefer BSD/OSF labels.
 * 	[90/12/03  23:33:43  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:43:47  af
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
 *	File: rz_disk.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/90
 *
 *	Top layer of the SCSI driver: interface with the MI.
 *	This file contains operations specific to disk-like devices.
 */

#include <kern/spl.h>		/* spl definitions */
#include <mach/std_types.h>
#include <machine/machparam.h>
#include <kern/misc_protos.h>

#include <scsi/scsi_defs.h>
#include <scsi/rz.h>
#include <scsi/rz_labels.h>
#include <device/ds_routines.h>
#include <device/misc_protos.h>

/*
 *	Function prototypes for internal routines
 */

/*	Checksum a disk label				*/
unsigned short	dkcksum(
	struct disklabel *lp);

/*	Grab the I/O request				*/
static scsi_ret_t	grab_it(
				target_info_t	*tgt,
				io_req_t	ior);
/*	Verify checksum on block			*/
void	scdisk_bcheck(
		long		bno,
		unsigned int	*addr,
		unsigned int	size);

/*	Compute checksum on block			*/
void	scdisk_checksum(
		long 		bno,
		unsigned int	*addr,
		unsigned int	size);

/*	Write back a label to the disk			*/
scsi_ret_t	scdisk_writelabel(
			target_info_t	*tgt);

/*	Perform checks and copy a disk label		*/
scsi_ret_t	setdisklabel(
			struct disklabel *lp,
			struct disklabel *nlp);

/*	ULTRIX-compatible I/O control			*/
scsi_ret_t	ul_disk_ioctl(
			target_info_t	*tgt,
			int		flavor,
			dev_status_t	status,
			unsigned int	status_count);

/*
 * Label used if the disk does not have one
 */
struct disklabel scsi_default_label =
{
	DISKMAGIC, DTYPE_SCSI, 0,
	"SCSI", "",
	0, 1, 1, 1, 1, 1, 0, 0, 0,
	3600, 1, 1, 1, 0, 0, 0,
	{0,}, {0,},
	DISKMAGIC, 0,
	8, 8192, 8192,
	{{ -1, 0, 1024, FS_BSDFFS, 8, 3 },
	 { -1, 0, 1024, FS_BSDFFS, 8, 3 },
	 { -1, 0, 1024, FS_BSDFFS, 8, 3 },
	 { -1, 0, 1024, FS_BSDFFS, 8, 3 },
	 { -1, 0, 1024, FS_BSDFFS, 8, 3 },
	 { -1, 0, 1024, FS_BSDFFS, 8, 3 },
	 { -1, 0, 1024, FS_BSDFFS, 8, 3 },
	 { -1, 0, 1024, FS_BSDFFS, 8, 3 }}
};

/*
 * Specialized side of the open routine for disks
 */
scsi_ret_t
scdisk_open(
	target_info_t		*tgt,
	io_req_t	        req)
{
	register int 	i;
	register scsi_ret_t	ret = SCSI_RET_SUCCESS;
	unsigned int		disk_size, secs_per_cyl, sector_size;
	unsigned int		nsectors, ntracks, ncylinders;
	scsi_rcap_data_t	*cap;
	struct disklabel	*label;
	io_req_t		ior;
	void			(*readfun)(
					target_info_t	*tgt,
					unsigned int	secno,
					io_req_t	ior) = scdisk_read;
	char			*data, *rdata = 0;

	if (tgt->flags & TGT_ONLINE)
		return SCSI_RET_SUCCESS;

	tgt->lun = rzlun(req->io_unit);

	/*
	 * Dummy ior for proper sync purposes
	 */
	io_req_alloc(ior);
	ior->io_next = 0;
	ior->io_count = 0;

#if 0
	/*
	 * Set the LBN to DEV_BSIZE with a MODE SELECT.
	 * xxx do a MODE SENSE instead ?
	 */
	for (i = 0; i < 5; i++) {
		ior->io_op = IO_INTERNAL;
		ior->io_error = 0;
		ret = scdisk_mode_select(tgt, DEV_BSIZE, ior, 0, 0, FALSE);
		if (ret == SCSI_RET_SUCCESS)
			break;
		if (ret == SCSI_RET_RETRY) {
			timeout((timeout_fcn_t)wakeup, tgt, 2*hz);
			await(tgt);
		}
		if (ret == SCSI_RET_DEVICE_DOWN)
			goto done;
	}
	if (ret != SCSI_RET_SUCCESS) {
		scsi_error( tgt, SCSI_ERR_MSEL, ret, 0);
		ret = D_INVALID_SIZE;
		goto done;
	}

#endif
	/*
	 * Do a READ CAPACITY to get max size. Check LBN too.
	 */
	for (i = 0; i < 5; i++) {
		ior->io_op = IO_INTERNAL;
		ior->io_error = 0;
		ret = scsi_read_capacity(tgt, 0, ior);
		if (ret == SCSI_RET_SUCCESS)
			break;
		if (ret == SCSI_RET_RETRY) {
			timeout((timeout_fcn_t)wakeup, tgt, 2*hz);
			await(tgt);
		}
		if (ret == SCSI_RET_DEVICE_DOWN)
			goto done;
	}
	if (ret != SCSI_RET_SUCCESS) {
		scsi_error( tgt, SCSI_ERR_MSEL, ret, 0);
		ret = D_INVALID_SIZE;
		goto done;
	}

	cap = (scsi_rcap_data_t*) tgt->cmd_ptr;
	disk_size = (cap->lba1<<24) |
	  	    (cap->lba2<<16) |
		    (cap->lba3<< 8) |
		     cap->lba4;
	if (scsi_debug)
		printf("rz%d holds %d blocks\n", tgt->unit_no, disk_size);

	tgt->block_size = (cap->blen1<<24) |
	  		  (cap->blen2<<16) |
	    		  (cap->blen3<<8 ) |
			   cap->blen4;

	if (scsi_debug) {
		printf("rz%d block size is %d bytes/block\n",
			       tgt->unit_no, tgt->block_size);
	}

	if (tgt->block_size > RZDISK_MAX_SECTOR) {
	 	 ret = D_INVALID_SIZE;
	  	goto done;
       	}

        rdata = (char *) kalloc(2*tgt->block_size);
	if (round_page(rdata) == round_page(rdata + tgt->block_size))
	    data = rdata;
	else
	    data = (char *)round_page(rdata);
	
	
	if (disk_size > SCSI_CMD_READ_MAX_LBA)
		tgt->flags |= TGT_BIG;

	/*
	 * Mandatory long-form commands ?
	 */
	if (BGET(scsi_use_long_form,tgt->masterno,tgt->target_id))
		tgt->flags |= TGT_BIG;
	if (tgt->flags & TGT_BIG)
		readfun = scsi_long_read;

	/*
	 * Find out about the phys disk geometry
	 */

	ior->io_op = IO_INTERNAL;
	ior->io_error = 0;

#if	PARAGON860

	/*
	 * The NCR RAID controller does not support a read capacity command
	 * with the Partial Medium Indicator (PMI) bit set.  Therefore we
	 * have to calculate the number of sectors per cylinder from data
	 * in the mode select pages.  This method should work for standalone
	 * disks as well.
	 */

	/*
	 * Read page 3 to find the number of sectors/track and bytes/sector
	 */
	ret = scsi_mode_sense(tgt, 0x03, 0xff, ior);
	/* scsi_error(...) */
	{
		scsi_mode_sense_page3_t	*page3;

		page3 = (scsi_mode_sense_page3_t *)
			(((scsi_mode_sense_data_t *)tgt->cmd_ptr) + 1);

		nsectors    = (page3->sectors_per_track_msb << 8) |
			       page3->sectors_per_track_lsb;

		sector_size = (page3->bytes_per_sector_msb  << 8) |
			       page3->bytes_per_sector_lsb;
	}

	ior->io_op = IO_INTERNAL;
	ior->io_error = 0;

	/*
	 * Read page 4 to find the number of cylinders and tracks/cylinder
	 */
	ret = scsi_mode_sense(tgt, 0x04, 0xff, ior);
	/* scsi_error(...) */
	{
		scsi_mode_sense_page4_t	*page4;

		page4 = (scsi_mode_sense_page4_t *)
			(((scsi_mode_sense_data_t *)tgt->cmd_ptr) + 1);

		ncylinders = (page4->number_of_cylinders_msb << 16) |
			     (page4->number_of_cylinders     <<  8) |
			      page4->number_of_cylinders_lsb;

		ntracks    = page4->number_of_heads;
	}

	/*
	 * Calculate the sectors per cylinder (sec/track * tracks/cyl)
	 */
	secs_per_cyl = nsectors * ntracks;

	if (scsi_debug) {
		printf("rz%d: %d bytes/sec %d sec/track\n", tgt->unit_no,
			sector_size, nsectors);
		printf("     %d tracks/cyl %d cyl/unit\n",
			ntracks, ncylinders);
	}

#else	/* PARAGON860 */

	ret = scsi_read_capacity( tgt, 1, ior);
	/* scsi_error(...) */
	cap = (scsi_rcap_data_t*) tgt->cmd_ptr;
	secs_per_cyl =	(cap->lba1<<24) | (cap->lba2<<16) |
			(cap->lba3<< 8) |  cap->lba4;
	secs_per_cyl += 1;
	sector_size =	(cap->blen1<<24) | (cap->blen2<<16) |
			(cap->blen3<<8 ) |  cap->blen4;
	if (scsi_debug)
	printf("rz%d: %d sect/cyl %d bytes/sec\n", tgt->unit_no,
		secs_per_cyl, sector_size);

#endif	/* PARAGON860 */

#ifdef	SCSI2
	/*
	 * ... and a bunch of other things for scsi2
	 */
#endif	/* SCSI2 */

	/*
	 * Get partition table off pack
	 */

	/* first look for a BSD label */	
	ior->io_data = data;
	ior->io_count = tgt->block_size;
	ior->io_op = IO_READ;
	ior->io_error = 0;
	tgt->ior = ior;
	(*readfun)( tgt, LABELOFFSET/tgt->block_size, ior);
	iowait(ior);

	{
		/* Search for BSD label, might be a bit further along */
		register int	i;
		boolean_t	found;

		for (i = LABELOFFSET % tgt->block_size, found = FALSE;
		     i < (tgt->block_size-sizeof(struct disklabel));
		     i += sizeof(int)) {
			label = (struct disklabel *) &data[i];
			if (label->d_magic  == DISKMAGIC &&
			    label->d_magic2 == DISKMAGIC) {
				found = TRUE;
				break;
			}
		}
		if (!found) label = 0;
	}

	if (label) {
		if (scsi_debug)
			printf("{Using BSD label}");
		tgt->dev_info.disk.l = *label;
	} else {
		/* then look for a vendor's label, but first
		   fill in defaults and what we found */

		label = &tgt->dev_info.disk.l;
		*label = scsi_default_label;
		label->d_secsize    = sector_size;
#ifdef	PARAGON860
		label->d_nsectors   = nsectors;
		label->d_ntracks    = ntracks;
		label->d_ncylinders = ncylinders;
#endif	/* PARAGON860 */
		label->d_secpercyl  = secs_per_cyl;
		label->d_secperunit = disk_size;

		ior->io_data = data;
		if (!rz_vendor_label(tgt, label, ior)) {

			printf("%s rz%d, %s\n",
				"WARNING: No valid disk label on",
				tgt->unit_no,
				"using defaults");
			/* Validate partitions a and c for initialization */
			tgt->dev_info.disk.l.d_partitions[0].p_offset = 0;
			tgt->dev_info.disk.l.d_partitions[0].p_size   = disk_size;
			tgt->dev_info.disk.l.d_partitions[2].p_offset = 0;
			tgt->dev_info.disk.l.d_partitions[2].p_size   = disk_size;
		}
		label->d_checksum = 0;
		label->d_checksum = dkcksum(label);
	}

	tgt->dev_info.disk.l.d_partitions[MAXPARTITIONS].p_size = -1;
	tgt->dev_info.disk.l.d_partitions[MAXPARTITIONS].p_offset = 0;

	ret = SCSI_RET_SUCCESS;
done:
        if (rdata)
                kfree((vm_offset_t) rdata, 2 * tgt->block_size);
	io_req_free(ior);
	return ret;
}

/*
 * Specialized side of the close routine for disks
 */
scsi_ret_t
scdisk_close(
	int		dev,
	target_info_t	*tgt)
{
	io_return_t	ret = SCSI_RET_SUCCESS;

	tgt->lun = rzlun(dev);

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
	}

	return ret;
}

/*
 * Disk strategy routine
 */

void
scdisk_strategy(
	register io_req_t	ior)
{
	target_info_t  *tgt;
	register scsi_softc_t	*sc;
	scsi_ret_t      ret;
	register int    i = ior->io_unit, part;
	register unsigned rec, sec, max;
	spl_t		last_spl;
        int recs_per_sec;

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
	part = rzpartition(i);

	/*
	 * Validate request 
	 */

	/* readonly ? */
	if ((tgt->flags & TGT_READONLY) &&
	    (ior->io_op & (IO_READ|IO_INTERNAL) == 0)) {
		ior->io_error = D_READ_ONLY;
		ior->io_op |= IO_ERROR;
		ior->io_residual = ior->io_count;
		iodone(ior);
		return;
	}

	rec = ior->io_recnum;
	recs_per_sec = tgt->dev_info.disk.l.d_secsize / tgt->block_size;
	max = tgt->dev_info.disk.l.d_partitions[part].p_size;
	if (max == -1)
		max = tgt->dev_info.disk.l.d_secperunit -
			tgt->dev_info.disk.l.d_partitions[part].p_offset;
	max *= recs_per_sec;
	i = btodb(ior->io_count + tgt->block_size - 1);
	if (((rec + i) > max) || (ior->io_count < 0) ||
#if later
	    ((rec <= LABELSECTOR)      &&	/* don't clobber the disk label */
	     (!(ior->io_op & IO_READ)) &&
	     ((tgt->flags & TGT_WRITE_LABEL) == 0))
#else
	    FALSE
#endif
	    ) {
		ior->io_error = D_INVALID_SIZE;
		ior->io_op |= IO_ERROR;
		ior->io_residual = ior->io_count;
		iodone(ior);
		return;
	}
	/*
	 * Find location on disk: secno and cyl (for disksort) 
	 */
	rec += tgt->dev_info.disk.l.d_partitions[part].p_offset * recs_per_sec;
	ior->io_residual = 
	        (rec / recs_per_sec) / tgt->dev_info.disk.l.d_secpercyl;

	/*
	 * Enqueue operation 
	 */
	last_spl = splbio();
	spinlock_lock(&tgt->target_lock);
	if (tgt->ior) {
		disksort(tgt->ior, ior);
		spinlock_unlock(&tgt->target_lock);
	} else {
		ior->io_next = 0;
		tgt->ior = ior;
		spinlock_unlock(&tgt->target_lock);
		scdisk_start(tgt,FALSE);
	}
	splx(last_spl);
}

/*#define CHECKSUM*/
#ifdef	CHECKSUM
int max_checksum_size = 0x2000;
#endif	/* CHECKSUM */

/*
 * Start/completion routine for disks
 */
void
scdisk_start(
	target_info_t	*tgt,
	boolean_t	done)
{
	register io_req_t	ior = tgt->ior;
	register io_req_t	rdone = NULL;
	register unsigned	part, secno;
	scsi_ret_t		ret;

	if (ior == 0)
		return;

	if (tgt->flags & TGT_BBR_ACTIVE) {
		scdisk_bbr_start( tgt, done);
		return;
		}

	if (done) {
		register unsigned int	xferred;
		unsigned int		max_dma_data;

		max_dma_data = scsi_softc[tgt->masterno]->max_dma_data;

		/* see if we must retry */
		if ((tgt->done == SCSI_RET_RETRY) &&
		    ((ior->io_op & IO_INTERNAL) == 0)) {
			delay(1000000);/*XXX*/
			goto start;
		} else
		/* got a bus reset ? pifff.. */
		if ((tgt->done == (SCSI_RET_ABORTED|SCSI_RET_RETRY)) &&
		    ((ior->io_op & IO_INTERNAL) == 0)) {
			if (xferred = ior->io_residual) {
				ior->io_data -= xferred;
				ior->io_count += xferred;
				ior->io_recnum -= xferred / tgt->block_size;
				ior->io_residual = 0;
			}
			goto start;
		} else
		/*
		 * Quickly check for errors: if anything goes wrong
		 * we do a request sense, see if that is what we did.
		 */
		if ((tgt->cur_cmd == SCSI_CMD_REQUEST_SENSE) &&
			(!rzpassthru(ior->io_unit))) {
			scsi_sense_data_t	*sns;
			unsigned int		blockno;
			char			*outcome;

			ior->io_op = ior->io_temporary;

			sns = (scsi_sense_data_t *)tgt->cmd_ptr;
			if (sns->addr_valid)
				blockno = sns->u.xtended.info0 << 24 |
					  sns->u.xtended.info1 << 16 |
					  sns->u.xtended.info2 <<  8 |
					  sns->u.xtended.info3;
			else {
				part     = rzpartition(ior->io_unit);
				blockno  = 
				  tgt->dev_info.disk.l.d_partitions[part].p_offset
				    * (tgt->dev_info.disk.l.d_secsize / tgt->block_size);
				blockno += ior->io_recnum;
			}

			if (scsi_check_sense_data(tgt, sns)) {
				ior->io_error = 0;
				outcome = "Recovered";
			} else {
				outcome = "Unrecoverable";
				ior->io_error = D_IO_ERROR;
				ior->io_op |= IO_ERROR;
			}
			if ((tgt->flags & TGT_OPTIONAL_CMD) == 0) {
			    printf("%s Error, rz%d: %s%s%d\n", outcome,
				tgt->target_id + (tgt->masterno * 8),
				(ior->io_op & IO_READ) ? "Read" :
				 ((ior->io_op & IO_INTERNAL) ? "(command)" : "Write"),
				" disk error, phys block no. ", blockno);

			    scsi_print_sense_data(sns);

			    /*
			     * On fatal read/write errors try replacing the bad block
			     * The bbr routine will return TRUE iff it took control
			     * over the target for all subsequent operations. In this
			     * event, the queue of requests is effectively frozen.
			     */
			    if (ior->io_error && 
				((sns->error_class == SCSI_SNS_XTENDED_SENSE_DATA) &&
				 ((sns->u.xtended.sense_key == SCSI_SNS_HW_ERR) ||
				  (sns->u.xtended.sense_key == SCSI_SNS_MEDIUM_ERR))) &&
			    	scdisk_bad_block_repl(tgt, blockno))
				    return;
			}
		}

		/*
	 	 * See if we had errors
		 */
		else if ((tgt->done != SCSI_RET_SUCCESS) &&
			(!rzpassthru(ior->io_unit))) {

			if (tgt->done == SCSI_RET_NEED_SENSE) {

				ior->io_temporary = ior->io_op;
				ior->io_op = IO_INTERNAL;
				scsi_request_sense(tgt, ior, 0);
				return;

			} else if (tgt->done == SCSI_RET_DEVICE_DOWN) {
				ior->io_error = D_DEVICE_DOWN;
				ior->io_op |= IO_ERROR;
			} else {
				printf("%s%x\n", "?rz_disk Disk error, ret=x", tgt->done);
				ior->io_error = D_IO_ERROR;
				ior->io_op |= IO_ERROR;
			}
		}
		/*
		 * No errors.
		 * See if we requested more than the max
		 * (We use io_residual in a flip-side way here)
		 */
		else if (ior->io_count > (xferred = max_dma_data)) {
			ior->io_residual += xferred;
			ior->io_count -= xferred;
			ior->io_data += xferred;
			ior->io_recnum += xferred / tgt->block_size;
			goto start;
		}
		else if (xferred = ior->io_residual) {
			ior->io_data -= xferred;
			ior->io_count += xferred;
			ior->io_recnum -= xferred / tgt->block_size;
			ior->io_residual = 0;
		} /* that's it */

#ifdef	CHECKSUM
		if ((ior->io_op & IO_READ) && (ior->io_count < max_checksum_size)) {
			part = rzpartition(ior->io_unit);
			secno = ior->io_recnum 
			  + tgt->dev_info.disk.l.d_partitions[part].p_offset
			    * (tgt->dev_info.disk.l.d_secsize / tgt->block_size);
			scdisk_bcheck(secno, ior->io_data, ior->io_count);
		}
#endif	/* CHECKSUM */

		/* If this is a pass-through device, save the target result */
		if (rzpassthru(ior->io_unit)) ior->io_error = tgt->done;

		/* dequeue next one */
		{
			io_req_t	next;

			spinlock_lock(&tgt->target_lock);
			next = ior->io_next;
			tgt->ior = next;
			spinlock_unlock(&tgt->target_lock);

			if (next == 0) {
			        iodone(ior);
				return;
			}

			rdone = ior;
			ior = next;
		}

#ifdef	CHECKSUM
		if (((ior->io_op & IO_READ) == 0) && (ior->io_count < max_checksum_size)) {
			part = rzpartition(ior->io_unit);
			secno = ior->io_recnum 
			  + tgt->dev_info.disk.l.d_partitions[part].p_offset
			    * (tgt->dev_info.disk.l.d_secsize / tgt->block_size);
			scdisk_checksum(secno, ior->io_data, ior->io_count);
		}
#endif	/* CHECKSUM */
	}
	ior->io_residual = 0;
start:
	scdisk_start_rw( tgt, ior);
	if (rdone != NULL)
		iodone(rdone);
}

void
scdisk_start_rw(
	target_info_t		*tgt,
	register io_req_t	ior)
{
	unsigned int	part, secno;
	register boolean_t	long_form;

	tgt->lun = rzlun(ior->io_unit);
	part = rzpartition(ior->io_unit);
	secno = ior->io_recnum
	  + tgt->dev_info.disk.l.d_partitions[part].p_offset
	    * (tgt->dev_info.disk.l.d_secsize / tgt->block_size);

	/* Use long form if either big block addresses or
	   the size is more than we can fit in one byte */
	long_form = (tgt->flags & TGT_BIG) ||
		    (ior->io_count > (256 * tgt->block_size));
	if (ior->io_op & IO_READ)
		(long_form ? scsi_long_read : scdisk_read)
					(tgt, secno, ior);
	else if ((ior->io_op & IO_INTERNAL) == 0)
		(long_form ? scsi_long_write : scdisk_write)
					(tgt, secno, ior);
	return;
}

#include <sys/ioctl.h>
#ifdef	ULTRIX_COMPAT
#include <mips/PMAX/rzdisk.h>
#endif	/*ULTRIX_COMPAT*/

io_return_t
scdisk_get_status(
	int		dev,
	target_info_t	*tgt,
	int		flavor,
	dev_status_t	status,
	unsigned int	*status_count)
{
	struct disklabel *lp;

	lp = &tgt->dev_info.disk.l;

	tgt->lun = rzlun(dev);

	switch (flavor) {
	case DEV_GET_SIZE:
		
		status[DEV_GET_SIZE_DEVICE_SIZE] =
		  lp->d_partitions[rzpartition(dev)].p_size * lp->d_secsize;
		status[DEV_GET_SIZE_RECORD_SIZE] = tgt->block_size;
		*status_count = DEV_GET_SIZE_COUNT;
		break;

	case DIOCGDINFO:
		*(struct disklabel *)status = *lp;
		*status_count = sizeof(struct disklabel)/sizeof(int);
		break;

#if	PARAGON860
#define DIOCGPINFO      _IOR('d', 113, struct partition)
	/* Why dont we use DIOCGDINFO ?? */
	case DIOCGPINFO:
		*(struct partition *)status = 
                    lp->d_partitions[rzpartition(dev)];
		*status_count = sizeof(struct partition)/sizeof(int);
		break;
#endif	/* PARAGON860 */

	default:
#ifdef	i386
		return(scsi_i386_get_status(dev, tgt, flavor, status, status_count));
#else	/* i386 */
 		return(D_INVALID_OPERATION);
#endif	/* i386 */
	}
	return D_SUCCESS;
}

io_return_t
scdisk_set_status(
	int		dev,
	target_info_t	*tgt,
	int		flavor,
	dev_status_t	status,
	unsigned int	status_count)
{
	io_return_t error = D_SUCCESS;
	struct disklabel *lp;

	lp = &tgt->dev_info.disk.l;

	tgt->lun = rzlun(dev);

	switch (flavor) {
	case DIOCSRETRIES:
		if (status_count != sizeof(int))
			return D_INVALID_SIZE;
		scsi_bbr_retries = *(int *)status;
		break;

	case DIOCWLABEL:
		if (*(int*)status)
			tgt->flags |= TGT_WRITE_LABEL;
		else
			tgt->flags &= ~TGT_WRITE_LABEL;
		break;
	case DIOCSDINFO:
	case DIOCWDINFO:
		if (status_count != sizeof(struct disklabel) / sizeof(int))
			return D_INVALID_SIZE;
		error = setdisklabel(lp, (struct disklabel*) status);
		if (error || (flavor == DIOCSDINFO))
			return error;
		error = scdisk_writelabel(tgt);
		break;

#if notyet
	case DIOCWFORMAT:
	case DIOCSBAD:	/* ?? how ? */
#endif
	default:
#ifdef	i386
		error = scsi_i386_set_status(dev, tgt, flavor, status, status_count);
#else	/* i386 */
 		error = D_INVALID_OPERATION;
#endif	/* i386 */
	}
	return error;
}

/* Perform some checks and then copy a disk label */
scsi_ret_t
setdisklabel(
	struct disklabel *lp,
	struct disklabel *nlp)
{
	if (nlp->d_magic != DISKMAGIC || nlp->d_magic2 != DISKMAGIC ||
	    (dkcksum(nlp) != 0))
		return D_INVALID_OPERATION;
	*lp = *nlp;
	return D_SUCCESS;
}

/* Checksum a disk label */
unsigned short
dkcksum(
	struct disklabel *lp)
{
	register unsigned short *start, *end, sum = 0;

	start = (unsigned short *)lp;
	end = (unsigned short*)&lp->d_partitions[lp->d_npartitions];
	while (start < end) sum ^= *start++;
	return sum;
}

static scsi_ret_t
grab_it(
	target_info_t	*tgt,
	io_req_t	ior)
{
	spl_t		s;

	s = splbio();
	spinlock_lock(&tgt->target_lock);
	if (!tgt->ior)
		tgt->ior = ior;
	spinlock_unlock(&tgt->target_lock);
	splx(s);

	if (tgt->ior != ior)
		return D_ALREADY_OPEN;
	return D_SUCCESS;
}

/* Write back a label to the disk */
scsi_ret_t
scdisk_writelabel(
	target_info_t	*tgt)
{
	io_req_t	ior;
	char		*data;
	struct disklabel *label;
	io_return_t	error;

	io_req_alloc(ior);
        data = (char *) kalloc(tgt->block_size);
	ior->io_next = 0;
	ior->io_data = data;
	ior->io_count = tgt->block_size;
	ior->io_op = IO_READ;
	ior->io_error = 0;

	if (grab_it(tgt, ior) != D_SUCCESS) {
		error = D_ALREADY_OPEN;
		goto ret;
	}


	scdisk_read( tgt, LABELSECTOR, ior);
	iowait(ior);
	if (error = ior->io_error)
		goto ret;

	label = (struct disklabel *) &data[LABELOFFSET % tgt->block_size];
	*label = tgt->dev_info.disk.l;

	ior->io_next = 0;
	ior->io_data = data;
	ior->io_count = 0;
	ior->io_count = tgt->block_size;
	ior->io_op = IO_WRITE;
	ior->io_error = 0;
	tgt->ior = ior;

	while (grab_it(tgt, ior) != D_SUCCESS)	;	/* ahem */

	scdisk_write( tgt, LABELSECTOR, ior);
	iowait(ior);

	error = ior->io_error;
ret:
	kfree((vm_offset_t) data, tgt->block_size);
	io_req_free(ior);
	return error;
}

#ifdef	CHECKSUM

#define SUMSIZE 0x10000
#define SUMHASH(b)	(((b)>>1) & (SUMSIZE - 1))
struct {
	long blockno;
	long sum;
} scdisk_checksums[SUMSIZE];

void
scdisk_checksum(
	long 			bno,
	register unsigned int	*addr,
	unsigned int		size)
{
	register int i = size/sizeof(int);
	register unsigned int sum = -1;

	while (i-- > 0)
		sum ^= *addr++;
	scdisk_checksums[SUMHASH(bno)].blockno = bno;
	scdisk_checksums[SUMHASH(bno)].sum = sum;
}

void
scdisk_bcheck(
	long			bno,
	register unsigned int	*addr,
	unsigned int		size)
{
	register int i = size/sizeof(int);
	register unsigned int sum = -1;
	unsigned int *start = addr;

	if (scdisk_checksums[SUMHASH(bno)].blockno != bno) {
if (scsi_debug) printf("No checksum for block x%x\n", bno);
		return;
	}

	while (i-- > 0)
		sum ^= *addr++;

	if (scdisk_checksums[SUMHASH(bno)].sum != sum) {
		printf("Bad checksum (x%x != x%x), bno x%x size x%x at x%x\n",
			sum,
			scdisk_checksums[bno & (SUMSIZE - 1)].sum,
			bno, size, start);
		gimmeabreak();
		scdisk_checksums[SUMHASH(bno)].sum = sum;
	}
}


#endif /* CHECKSUM */

/*#define PERF */
#ifdef	PERF
int test_read_size = 512;
int test_read_skew = 12;
int test_read_skew_min = 0;
int test_read_nreads = 1000;
int test_read_bdev = 0;

#include <sys/time.h>

/*
 *	Function prototypes for test routines
 */

/*	Test SCSI read capability			*/
void	test_read(
		int	max);

/*	Test SCSI Unit Ready				*/
void	tur_test( void );

/*
 *	Test routines
 */
void
test_read(
	int	max)
{
	int i, ssk, usecs;
	struct timeval start, stop;

	if (max == 0)
		max = test_read_skew + 1;
	ssk = test_read_skew;
	for (i = test_read_skew_min; i < max; i++){
		test_read_skew = i;

		start = time;
		read_test();
		stop = time;

		usecs = stop.tv_usec - start.tv_usec;
		if (usecs < 0) {
			stop.tv_sec -= 1;
			usecs += 1000000;
		}
		printf("Skew %3d size %d count %d time %3d sec %d us\n",
			i, test_read_size, test_read_nreads,
			stop.tv_sec - start.tv_sec, usecs);
	}
	test_read_skew = ssk;
}

read_test()
{
	static char	buffer[8192*2];
	struct io_req	io;
	register int 	i, rec;

	bzero(&io, sizeof(io));
	io.io_unit = test_read_bdev;
	io.io_op = IO_READ;
	io.io_count = test_read_size;
	io.io_data = buffer;

	for (rec = 0, i = 0; i < test_read_nreads; i++) {
		io.io_op = IO_READ;
		io.io_recnum = rec;
		scdisk_strategy(&io);
		rec += test_read_skew;
		iowait(&io);
	}
}

void
tur_test( void )
{
	struct io_req	io;
	register int 	i;
	char		*a, *b;
	struct timeval start, stop;

	bzero(&io, sizeof(io));
	io.io_unit = test_read_bdev;
	io.io_data = (char*)&io;/*unused but kernel space*/

	start = time;
	for (i = 0; i < test_read_nreads; i++) {
		io.io_op = IO_INTERNAL;
		rz_check(io.io_unit, &a, &b);
		scsi_test_unit_ready(b,&io);
	}
	stop = time;
	i = stop.tv_usec - start.tv_usec;
	if (i < 0) {
		stop.tv_sec -= 1;
		i += 1000000;
	}
	printf("%d test-unit-ready took %3d sec %d us\n",
			test_read_nreads,
			stop.tv_sec - start.tv_sec, i);
}

#endif	/* PERF */

/*#define WDEBUG*/
#ifdef	WDEBUG

/*
 *	Function prototypes for debug routines
 */

/*	Write pattern, then read from disk		*/
void	buggotest(
		int		n,
		unsigned int	pattern,
		boolean_t	verbose);

int buggo_write_size = 8192;
int buggo_dev = 1; /* rz0b */
int	buggo_out_buffer[8192/2];
int	buggo_in_buffer[8192/2];

void
buggotest(
	int		n,
	unsigned int	pattern,
	boolean_t	verbose)
{
	struct io_req	io;
	register int 	i, rec;

	if (n <= 0)
		n = 1;

	if(pattern)
		for (i = 0; i < buggo_write_size/4; i++)
			buggo_out_buffer[i] = i + pattern;

	for (i = 0; i < n; i++) {
		register int j;

		buggo_out_buffer[0] = i + pattern;
		buggo_out_buffer[(buggo_write_size/4)-1] = i + pattern;
		bzero(&io, sizeof(io));
		io.io_unit = buggo_dev;
		io.io_data = (char*)buggo_out_buffer;
		io.io_op = IO_WRITE;
		io.io_count = buggo_write_size;
		io.io_recnum = i % 1024;
		scdisk_strategy(&io);

		bzero(buggo_in_buffer, sizeof(buggo_in_buffer));
		iowait(&io);

		if (verbose)
			printf("Done write with %x", io.io_error);

		bzero(&io, sizeof(io));
		io.io_unit = buggo_dev;
		io.io_data = (char*)buggo_in_buffer;
		io.io_op = IO_READ;
		io.io_count = buggo_write_size;
		io.io_recnum = i % 1024;
		scdisk_strategy(&io);
		iowait(&io);

		if (verbose)
			printf("Done read with %x", io.io_error);

		for  (j = 0; j < buggo_write_size/4; j++)
			if (buggo_out_buffer[j] != buggo_in_buffer[j]){
				printf("Difference at %d-th word: %x %x\n",
					buggo_out_buffer[j], buggo_in_buffer[j]);
				return i;
			}
	}
	printf("Test ok\n");
	return n;
}


#endif	/* WDEBUG */
