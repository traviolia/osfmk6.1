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
 * Revision 2.3.2.2  92/04/30  12:00:40  bernadat
 * 	Changes from TRUNK:
 * 		Let "grab_bob_label" return failure so we take the default.
 * 		[92/04/01            rvb]
 * 
 * Revision 2.3.2.1  92/03/28  10:15:38  jeffreyh
 * 	Pick up changes from MK71
 * 	[92/03/20  13:32:14  jeffreyh]
 * 
 * Revision 2.4  92/02/23  22:44:31  elf
 * 	Actually, scan for all possible (bogus) label formats.
 * 	This makes it possible to cross-mount disks even if
 * 	they do not have the standard BSD label.
 * 	[92/02/22            af]
 * 
 * Revision 2.3  91/08/24  12:28:06  af
 * 	Created, splitting out DEC-specific code from rz_disk.c and
 * 	adding some more.
 * 	[91/06/26            af]
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
 *	File: rz_labels.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	6/91
 *
 *	Routines for various vendor's disk label formats.
 */

#include <platforms.h>
#include <mach/std_types.h>
#include <kern/misc_protos.h>

#include <scsi/scsi_defs.h>
#include <scsi/rz_labels.h>
#include <scsi/scsi_entries.h>

#include <device/ds_routines.h>

/*
 *	Function prototypes for internal routines
 */

/*	Find and convert DEC label to BSD		*/
boolean_t	rz_dec_label(
			target_info_t		*tgt,
			struct disklabel	*label,
			io_req_t		ior);

/*	Find and convert Omron label to BSD		*/
boolean_t	rz_omron_label(
			target_info_t		*tgt,
			struct disklabel	*label,
			io_req_t		ior);

/*	Find and convert Intel BIOS label to BSD	*/
boolean_t	rz_bios_label(
			target_info_t		*tgt,
			struct disklabel	*label,
			io_req_t		ior);

scsi_ret_t get_phys_parms(
	target_info_t		*tgt,
	struct disklabel	*label,
	io_req_t		ior);	

/*
 * Find and convert from a DEC label to BSD
 */
boolean_t
rz_dec_label(
	target_info_t		*tgt,
	struct disklabel	*label,
	io_req_t		ior)
{
	/* here look for a DEC label */
	register scsi_dec_label_t	*part;
	char				*data;
	int				i;

	ior->io_count = tgt->block_size;
	ior->io_op = IO_READ;
	tgt->ior = ior;
	scdisk_read( tgt, DEC_LABEL_BYTE_OFFSET/tgt->block_size, ior);
	iowait(ior);
	data = (char *)ior->io_data;
	part = (scsi_dec_label_t*)&data[DEC_LABEL_BYTE_OFFSET%tgt->block_size];
	if (part->magic == DEC_LABEL_MAGIC) {
		if (scsi_debug)
		printf("{Using DEC label}");
		for (i = 0; i < 8; i++) {
			label->d_partitions[i].p_size = part->partitions[i].n_sectors;
			label->d_partitions[i].p_offset = part->partitions[i].offset;
		}
		return TRUE;
	}
	return FALSE;
}

/*
 * Find and convert from a Omron label to BSD
 */
boolean_t
rz_omron_label(
	target_info_t		*tgt,
	struct disklabel	*label,
	io_req_t		ior)
{
	/* here look for an Omron label */
	register scsi_omron_label_t	*part;
	char				*data;
	int				i;

	ior->io_count = tgt->block_size;
	ior->io_op = IO_READ;
	tgt->ior = ior;
	scdisk_read( tgt, OMRON_LABEL_BYTE_OFFSET/tgt->block_size, ior);
	iowait(ior);
	data = (char *)ior->io_data;
	part = (scsi_omron_label_t*)&data[OMRON_LABEL_BYTE_OFFSET%tgt->block_size];
	if (part->magic == OMRON_LABEL_MAGIC) {
		if (scsi_debug)
		printf("{Using OMRON label}");
		for (i = 0; i < 8; i++) {
			label->d_partitions[i].p_size = part->partitions[i].n_sectors;
			label->d_partitions[i].p_offset = part->partitions[i].offset;
		}
		bcopy(part->packname, label->d_packname, 16);
		label->d_ncylinders = part->ncyl;
		label->d_acylinders = part->acyl;
		label->d_ntracks = part->nhead;
		label->d_nsectors = part->nsect;
		/* Many disks have this wrong, therefore.. */
#if 0
		label->d_secperunit = part->maxblk;
#else
		label->d_secperunit = label->d_ncylinders * label->d_ntracks *
					label->d_nsectors;
#endif
		return TRUE;
	}
	return FALSE;
}

/*
 * Find and convert from a Intel BIOS label to BSD
 */
boolean_t
rz_bios_label(
	target_info_t		*tgt,
	struct disklabel	*label,
	io_req_t		ior)
{
	/* here look for a BIOS label */
	register scsi_bios_label_t	*part;
	char				*data;
	int				i;

	ior->io_count = tgt->block_size;
	ior->io_op = IO_READ;
	tgt->ior = ior;
	scdisk_read( tgt, BIOS_LABEL_BYTE_OFFSET/tgt->block_size, ior);
	iowait(ior);
	data = (char *)ior->io_data;
	part = (scsi_bios_label_t*)&data[BIOS_LABEL_BYTE_OFFSET%tgt->block_size];
	if (part->magic == BIOS_LABEL_MAGIC) {
		struct bios_partition_info *bpart;
		if (scsi_debug)
		printf("{Using BIOS label}");
		bpart = (struct bios_partition_info *)part->partitions;
		for (i = 0; i < 4; i++) {
			label->d_partitions[i].p_size = bpart[i].n_sectors;
			label->d_partitions[i].p_offset = bpart[i].offset;
		}
#ifdef	AT386	/* this goes away real fast */
		if (tgt->block_size == BOB_LABEL_SECTOR_SIZE &&
                    !grab_bob_label(tgt, label, ior, bpart))
			return FALSE;		/* take default setup of "a" and "c" */
#else
		label->d_npartitions = 4;
#endif
		label->d_bbsize = BIOS_BOOT0_SIZE;
		return TRUE;
	}
	return FALSE;
}
#ifdef        iPSC386
/* 
 * Code lives in i386ipsc/disk_label.c for now. Might someday move to here 
 * rz_ipsc_label will always return true as it is hard coded.
 *
*/
extern        boolean_t rz_ipsc_label();
#define       VENDOR_LABEL   rz_ipsc_label
#endif        /* iPSC386 */




/*
 * Try all of the above
 */
boolean_t
rz_vendor_label(
	target_info_t		*tgt,
	struct disklabel	*label,
	io_req_t		ior)
{
	/* If for any reason there might be problems someday.. */
#ifdef	VENDOR_LABEL
	if (VENDOR_LABEL( tgt, label, ior)) return TRUE;
#endif	/*VENDOR_LABEL*/

	if (rz_dec_label( tgt, label, ior)) return TRUE;
	if (rz_bios_label( tgt, label, ior)) return TRUE;
	if (rz_omron_label( tgt, label, ior)) return TRUE;
	(void) get_phys_parms(tgt, label, ior);
	return FALSE;
}


#define sense_header \
	unsigned char sense_data_legth; \
	unsigned char medium_typed; \
	unsigned char dev_spec_parm; \
	unsigned char block_desc_length

#define block_desc \
	unsigned char density_code; \
	unsigned char nblks_0, nblks_1, nblks_2; \
	unsigned char reserved; \
	unsigned char blk_length_0, blk_length_1, blk_length_2

struct page_code_3 {
	sense_header;
	block_desc;	
	unsigned char		page_code;
	unsigned char		page_length;
	unsigned char		tpz_0, tpz_1;
	unsigned char		alt_spz_0, alt_spz_1;
	unsigned char		alt_tpz_0, alt_tpz_1;
	unsigned char		alt_tplu_0, alt_tplu_1;
	unsigned char		secs_0, secs_1;
	unsigned char		bytes_0, bytes_1;
	unsigned char		interleave_0, inteleave_1;
	unsigned char		track_skew_0, track_skew_1;
	unsigned char		cyl_skew_0, cyl_skew_1;
};

struct page_code_4 {
	sense_header;
	block_desc;	
	unsigned char		page_code;
	unsigned char		page_length;
	unsigned char		cyls_0, cyls_1, cyls_2;
	unsigned char		heads;
};

scsi_ret_t
get_phys_parms(
	target_info_t		*tgt,
	struct disklabel	*label,
	io_req_t		ior)	
{
	int disk_size;

	ior->io_op = IO_INTERNAL;
	ior->io_next = 0;
	ior->io_error = 0;
	ior->io_count = 0;
	ior->io_residual = 0;
	if (scsi_mode_sense(tgt, 3, 32, ior) == SCSI_RET_SUCCESS) {
		struct page_code_3 *p3 = (struct page_code_3 *)tgt->cmd_ptr;
		label->d_nsectors = (p3->secs_0 << 8)+ p3->secs_1;
        } else
		return D_IO_ERROR;

	ior->io_op = IO_INTERNAL;
	ior->io_next = 0;
	ior->io_error = 0;
	if (scsi_mode_sense(tgt, 4, 32, ior) == SCSI_RET_SUCCESS) {
		struct page_code_4 *p4 = (struct page_code_4 *)tgt->cmd_ptr;
		label->d_ncylinders = (((p4->cyls_0 << 8)+ p4->cyls_1) << 8)
		                      + p4->cyls_2;
		label->d_ntracks = p4->heads;
	} else 
		return D_IO_ERROR;

	label->d_secpercyl = label->d_ntracks * label->d_nsectors;
	disk_size = label->d_secpercyl * label->d_ncylinders;

	printf("rz%d: mode_sense says %d secs, %d cyls, %d heads, total %d\n",
	       tgt->unit_no,
	       label->d_nsectors,
	       label->d_ncylinders,
	       label->d_ntracks,
	       disk_size);
	if (label->d_secperunit != disk_size)
		printf("rz%d: read_capacity says %d secs/cyl, %d total blocks ( != %d)\n",
		       tgt->unit_no,
		       label->d_secpercyl,
		       label->d_secperunit,
		       disk_size);
	return D_SUCCESS;
}
