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
 * Revision 2.3.4.2  92/04/30  11:49:19  bernadat
 * 	Changes from TRUNK:
 * 	We add one more partition so that we can point to the "whole"
 * 	  	disk.  It is at slot MAXPARTITIONS.
 * 	  	[92/04/01            rvb]
 * 
 * Revision 2.3.4.1  92/02/18  18:39:17  jeffreyh
 * 	Increase number of disk partitions to 16 to be compatible with 2.5
 * 	[92/02/08            bernadat]
 * 
 * Revision 2.3  91/05/14  15:43:50  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/05/13  06:02:21  af
 * 	Created, starting from Reno's disklabel.h (copyright included).
 * 	[91/05/03            af]
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
 * Copyright (c) 1987, 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)disklabel.h	7.10 (Berkeley) 6/27/88
 */

#ifndef	_DISK_STATUS_H_
#define	_DISK_STATUS_H_

/*
 * Each disk has a label which includes information about the hardware
 * disk geometry, filesystem partitions, and drive specific information.
 * The label is in block 0 or 1, possibly offset from the beginning
 * to leave room for a bootstrap, etc.
 */

#define LABELSECTOR	0			/* sector containing label */
#define LABELOFFSET	64			/* offset of label in sector */
#define DISKMAGIC	((unsigned long) 0x82564557)	/* The disk magic number */
#ifndef MAXPARTITIONS
#define	MAXPARTITIONS	16
#endif


#ifndef LOCORE
struct disklabel {
	unsigned long	d_magic;		/* the magic number */
	short	d_type;			/* drive type */
	short	d_subtype;		/* controller/d_type specific */
	char	d_typename[16];		/* type name, e.g. "eagle" */
	/* 
	 * d_packname contains the pack identifier and is returned when
	 * the disklabel is read off the disk or in-core copy.
	 * d_boot0 and d_boot1 are the (optional) names of the
	 * primary (block 0) and secondary (block 1-15) bootstraps
	 * as found in /usr/mdec.  These are returned when using
	 * getdiskbyname(3) to retrieve the values from /etc/disktab.
	 */
#if defined(MACH_KERNEL) || defined(STANDALONE)
	char	d_packname[16];			/* pack identifier */ 
#else
	union {
		char	un_d_packname[16];	/* pack identifier */ 
		struct {
			char *un_d_boot0;	/* primary bootstrap name */
			char *un_d_boot1;	/* secondary bootstrap name */
		} un_b; 
	} d_un; 
#define d_packname	d_un.un_d_packname
#define d_boot0		d_un.un_b.un_d_boot0
#define d_boot1		d_un.un_b.un_d_boot1
#endif	/* ! MACH_KERNEL or STANDALONE */
			/* disk geometry: */
	unsigned long	d_secsize;		/* # of bytes per sector */
	unsigned long	d_nsectors;		/* # of data sectors per track */
	unsigned long	d_ntracks;		/* # of tracks per cylinder */
	unsigned long	d_ncylinders;		/* # of data cylinders per unit */
	unsigned long	d_secpercyl;		/* # of data sectors per cylinder */
	unsigned long	d_secperunit;		/* # of data sectors per unit */
	/*
	 * Spares (bad sector replacements) below
	 * are not counted in d_nsectors or d_secpercyl.
	 * Spare sectors are assumed to be physical sectors
	 * which occupy space at the end of each track and/or cylinder.
	 */
	unsigned short	d_sparespertrack;	/* # of spare sectors per track */
	unsigned short	d_sparespercyl;		/* # of spare sectors per cylinder */
	/*
	 * Alternate cylinders include maintenance, replacement,
	 * configuration description areas, etc.
	 */
	unsigned long	d_acylinders;		/* # of alt. cylinders per unit */

			/* hardware characteristics: */
	/*
	 * d_interleave, d_trackskew and d_cylskew describe perturbations
	 * in the media format used to compensate for a slow controller.
	 * Interleave is physical sector interleave, set up by the formatter
	 * or controller when formatting.  When interleaving is in use,
	 * logically adjacent sectors are not physically contiguous,
	 * but instead are separated by some number of sectors.
	 * It is specified as the ratio of physical sectors traversed
	 * per logical sector.  Thus an interleave of 1:1 implies contiguous
	 * layout, while 2:1 implies that logical sector 0 is separated
	 * by one sector from logical sector 1.
	 * d_trackskew is the offset of sector 0 on track N
	 * relative to sector 0 on track N-1 on the same cylinder.
	 * Finally, d_cylskew is the offset of sector 0 on cylinder N
	 * relative to sector 0 on cylinder N-1.
	 */
	unsigned short	d_rpm;			/* rotational speed */
	unsigned short	d_interleave;		/* hardware sector interleave */
	unsigned short	d_trackskew;		/* sector 0 skew, per track */
	unsigned short	d_cylskew;		/* sector 0 skew, per cylinder */
	unsigned long	d_headswitch;		/* head switch time, usec */
	unsigned long	d_trkseek;		/* track-to-track seek, usec */
	unsigned long	d_flags;		/* generic flags */
#define NDDATA 5
	unsigned long	d_drivedata[NDDATA];	/* drive-type specific information */
#define NSPARE 5
	unsigned long	d_spare[NSPARE];	/* reserved for future use */
	unsigned long	d_magic2;		/* the magic number (again) */
	unsigned short	d_checksum;		/* xor of data incl. partitions */

			/* filesystem and partition information: */
	unsigned short	d_npartitions;		/* number of partitions in following */
	unsigned long	d_bbsize;		/* size of boot area at sn0, bytes */
	unsigned long	d_sbsize;		/* max size of fs superblock, bytes */
	struct	partition {		/* the partition table */
		unsigned long	p_size;		/* number of sectors in partition */
		unsigned long	p_offset;	/* starting sector */
		unsigned long	p_fsize;	/* filesystem basic fragment size */
		unsigned char	p_fstype;	/* filesystem type, see below */
		unsigned char	p_frag;		/* filesystem fragments per block */
		unsigned short	p_cpg;		/* filesystem cylinders per group */
	} d_partitions[MAXPARTITIONS+1];	/* actually may be more */
};
#else /* LOCORE */
	/*
	 * offsets for asm boot files.
	 */
	.set	d_secsize,40
	.set	d_nsectors,44
	.set	d_ntracks,48
	.set	d_ncylinders,52
	.set	d_secpercyl,56
	.set	d_secperunit,60
	.set	d_end_,276		/* size of disk label */
#endif /* LOCORE */

/* d_type values: */
#define	DTYPE_SMD		1		/* SMD, XSMD; VAX hp/up */
#define	DTYPE_MSCP		2		/* MSCP */
#define	DTYPE_DEC		3		/* other DEC (rk, rl) */
#define	DTYPE_SCSI		4		/* SCSI */
#define	DTYPE_ESDI		5		/* ESDI interface */
#define	DTYPE_ST506		6		/* ST506 etc. */
#define	DTYPE_FLOPPY		10		/* floppy */

#ifdef DKTYPENAMES
static char *dktypenames[] = {
	"unknown",
	"SMD",
	"MSCP",
	"old DEC",
	"SCSI",
	"ESDI",
	"type 6",
	"type 7",
	"type 8",
	"type 9",
	"floppy",
	0
};
#define DKMAXTYPES	(sizeof(dktypenames) / sizeof(dktypenames[0]) - 1)
#endif

/*
 * Filesystem type and version.
 * Used to interpret other filesystem-specific
 * per-partition information.
 */
#define	FS_UNUSED	0		/* unused */
#define	FS_SWAP		1		/* swap */
#define	FS_V6		2		/* Sixth Edition */
#define	FS_V7		3		/* Seventh Edition */
#define	FS_SYSV		4		/* System V */
#define	FS_V71K		5		/* V7 with 1K blocks (4.1, 2.9) */
#define	FS_V8		6		/* Eighth Edition, 4K blocks */
#define	FS_BSDFFS	7		/* 4.2BSD fast file system */

#ifdef	DKTYPENAMES
static char *fstypenames[] = {
	"unused",
	"swap",
	"Version 6",
	"Version 7",
	"System V",
	"4.1BSD",
	"Eighth Edition",
	"4.2BSD",
	0
};
#define FSMAXTYPES	(sizeof(fstypenames) / sizeof(fstypenames[0]) - 1)
#endif

/*
 * flags shared by various drives:
 */
#define		D_REMOVABLE	0x01		/* removable media */
#define		D_ECC		0x02		/* supports ECC */
#define		D_BADSECT	0x04		/* supports bad sector forw. */
#define		D_RAMDISK	0x08		/* disk emulator */
#define		D_CHAIN		0x10		/* can do back-back transfers */

/*
 * Drive data for SMD.
 */
#define	d_smdflags	d_drivedata[0]
#define		D_SSE		0x1		/* supports skip sectoring */
#define	d_mindist	d_drivedata[1]
#define	d_maxdist	d_drivedata[2]
#define	d_sdist		d_drivedata[3]

/*
 * Drive data for ST506.
 */
#define d_precompcyl	d_drivedata[0]
#define d_gap3		d_drivedata[1]		/* used only when formatting */

/*
 * IBM controller info (d_precompcyl used, too)
 */
#define	d_step		d_drivedata[2]

#ifndef LOCORE
/*
 * Structure used to perform a format
 * or other raw operation, returning data
 * and/or register values.
 * Register identification and format
 * are device- and driver-dependent.
 */
struct format_op {
	char	*df_buf;
	int	df_count;		/* value-result */
	recnum_t	df_startblk;
	int	df_reg[8];		/* result */
};

/*
 * Disk-specific ioctls.
 */
		/* get and set disklabel; DIOCGPART used internally */
#define DIOCGDINFO	_IOR('d', 101, struct disklabel)/* get */
#define DIOCSDINFO	_IOW('d', 102, struct disklabel)/* set */
#define DIOCWDINFO	_IOW('d', 103, struct disklabel)/* set, update disk */

/* do format operation, read or write */
#define DIOCRFORMAT	_IOWR('d', 105, struct format_op)
#define DIOCWFORMAT	_IOWR('d', 106, struct format_op)

#define DIOCSSTEP	_IOW('d', 107, int)	/* set step rate */
#define DIOCSRETRIES	_IOW('d', 108, int)	/* set # of retries */
#define DIOCWLABEL	_IOW('d', 109, int)	/* write en/disable label */

#define DIOCSBAD	_IOW('d', 110, struct dkbad)	/* set kernel dkbad */

/*
 * These are really generic SCSI ioctls and structures
 * but will probably be used with disks most often
 */
/*
 * Structure used to perform SCSI pass-through commands
 */
struct scsi_io {
#define SCSI_CDB_LENGTH 12
	unsigned char	cdb[SCSI_CDB_LENGTH];
	unsigned long	cmd_count;
#define SCSI_NONE	(0 << 0)
#define MEM_TO_SCSI	(1 << 0)
#define SCSI_TO_MEM	(1 << 1)
	unsigned long	direction;
#define SCSI_GOOD			0x00
#define SCSI_CHECK_CONDITION		0x01
#define SCSI_CONDITION_MET		0x02
#define SCSI_BUSY			0x03
#define SCSI_INTERMEDIATE		0x04
#define SCSI_INTERMEDIATE_CONDITION_MET	0x05
#define SCSI_RESERVATION_CONFLICT	0x06
#define SCSI_COMMAND_TERMINATED		0x07
#define SCSI_QUEUE_FULL			0x08
	unsigned long	status;
	unsigned long	buf_len;
#define SCSI_BUF_MAX	(2 * 1024)
	unsigned char	buf[SCSI_BUF_MAX];
};

/* SCSI pass-through command */
#define DIOCSCSI	_IOWR('d', 111, struct scsi_io)

/*
 * Structure used to get SCSI device physical information
 */
struct scsi_phys {
	unsigned long	slot;
	unsigned char	controller;
	unsigned char	target_id;
	unsigned char	lun;
};

/* Get SCSI device physical information */
#define DIOCGPHYS	_IOR('d', 112, struct scsi_phys)

#endif	/* LOCORE */

#endif	/* _DISK_STATUS_H_ */
