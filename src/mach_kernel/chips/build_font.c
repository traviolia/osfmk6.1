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
 * Revision 2.6  91/06/19  11:46:19  rvb
 * 	File moved here from mips/PMAX since it tries to be generic;
 * 	it is used on the PMAX and the Vax3100.
 * 	[91/06/04            rvb]
 * 
 * Revision 2.5  91/05/14  17:19:54  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:40:00  mrt
 * 	Added author notices
 * 	[91/02/04  11:12:16  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:09:59  mrt]
 * 
 * Revision 2.3  90/12/05  23:30:32  af
 * 	Add Log entry to output file so that it can be RCS-ed.
 * 	[90/12/03  23:11:04  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:11:00  af
 * 	Created.
 * 	[90/10/07            af]
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
 *	File: build_font.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/90
 *
 *
 *	Takes a font description file and generates a C source
 *	appropriate for use as kernel font on mips/vax boxes.
 *	This basically means encoding and mirroring the bitmaps.
 */

#include <stdio.h>

main(argc,argv)
	char **argv;
{
	int		fd;
	FILE		*fout;
	int             i, j, k, n, l;
	int		first, last;
	char		*fname = "kernel_font.data";
	char		buf[16*9];
	int		verbose = 0;

	if (argc > 1 && argv[1][0] == '+')
		verbose++, argc--, argv++;

	first = 0;
	last = 190;	/* 8-bit ASCII, offset by 'space' */
	if (argc > 1) {
		first = atoi(argv[1]);
		last = first + 1;
	}
	if (argc > 2)
		last = atoi(argv[2]) + 1;
	if (argc > 3)
		fname = argv[3];

	fd = open(fname, 0, 0);
	fout = fopen("kernel_font.c", "w");

	fprintf(fout, "/* \n\
 * Mach Operating System\n\
 * Copyright (c) 1989 Carnegie-Mellon University\n\
 * All rights reserved.  The CMU software License Agreement specifies\n\
 * the terms and conditions for use and redistribution.\n\
 */\n\
/*\n\
 * THIS FILE WAS GENERATED BY %s FROM %s\n\
 * IF YOU NEED TO, BE SURE YOU EDIT THE REAL THING!\n\
 */\n\
/*\n\
 * HISTORY\n\
 *  3-Sep-90  Alessandro Forin (af) at Carnegie-Mellon University\n\
 *	Created.\n\
 * $Log: build_font.c,v $
 * Revision 1.1.7.1  1994/09/23  01:04:19  ezf
 * 	change marker to not FREE
 * 	[1994/09/22  21:02:48  ezf]
 *
 * Revision 1.1.6.2  1994/09/22  21:02:48  ezf
 * 	change marker to not FREE
 *
 * Revision 1.1.3.2  1993/06/02  22:59:57  jeffc
 * 	Added to OSF/1 R1.3 from NMK15.0.
 * 	[1993/06/02  20:49:51  jeffc]
 *
 * Revision 1.1.2.2  1993/06/02  20:49:51  jeffc
 * 	Added to OSF/1 R1.3 from NMK15.0.
 *
 * Revision 1.1  1992/09/30  01:59:35  robert
 * Initial revision
 *
 * Revision 2.6  91/06/19  11:46:19  rvb
 * 	File moved here from mips/PMAX since it tries to be generic;
 * 	it is used on the PMAX and the Vax3100.
 * 	[91/06/04            rvb]
 * 
 * Revision 2.5.1.1  91/06/06  14:08:04  rvb
 * 	File moved here from mips/PMAX since it tries to be generic;
 * 	it is used on the PMAX and the Vax3100.
 * 	[91/06/04            rvb]
 * 
 * Revision 2.5  91/05/14  17:19:54  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:40:00  mrt
 * 	Added author notices
 * 	[91/02/04  11:12:16  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:09:59  mrt]
 * 
 * Revision 2.3.1.2  91/02/04  11:12:16  mrt
 * 	Added author notices
 * 
 * Revision 2.3.1.1  91/02/02  12:09:59  mrt
 * 	Changed to use new Mach copyright
 * 
 * Revision 2.3  90/12/05  23:30:32  af
 * 	Add Log entry to output file so that it can be RCS-ed.
 * 	[90/12/03  23:11:04  af]
 * 
 * Revision 2.1.2.1  90/12/03  23:11:04  af
 * 	Add Log entry to output file so that it can be RCS-ed.
 * 
 * Revision 2.1.1.2  90/11/01  03:14:32  af
 * 	Add Log entry to output file so that it can be RCS-ed.
 * \n\
 */\n\
\n\
/*\n\
 *	Object:\n\
 *		kfont_7x14			EXPORTED array\n\
 *\n\
 *		Kernel font for printable ASCII chars\n\
 *\n\
 * The smallest index in this array corresponds to a\n\
 * space. So, we start at 0x20 in the ascii table.\n\
 * Note that glyphs are mirrored (byteorder, I think)\n\
 * the commented bitmap shows how they really look like\n\
 */\n\
\n\
unsigned char kfont_7x14[] = {\n", argv[0], fname);

skip_comments:
	read(fd, buf, 1);
	if (buf[0] == '#') {
		do
			read(fd, buf, 1);
		while (buf[0] != '\n');
		goto skip_comments;
	}
	lseek(fd, -1, 1);	/* put char back */

	/* if must skip some */
	for (l = 0; l < first; l++)
		read(fd, buf, 2+(9*15));

	/* scan for real now */
	for (i = first; i < last; i++) {
		/* read one full glyph */
		if (read(fd, buf, 2+(9*15)) < 0)
			break;
		if (verbose)
			printf("Character '%c':\n\t", buf[0]);
		/* index and char itself in comments */
		fprintf(fout, "/* %3x '%c' */\n", i, 0x20+i);

		/* encode and mirror each one of the 15 scanlines */
		for (n = 0; n < 15; n++) {
			unsigned char	cc[8], swap = 0;
			/* 8 bits per scanline */
			for (k = 2+(n*9), j = 0; j < 8; k++, j++) {
				if (verbose)
					printf("%c", (buf[k] == '1') ? '@' : ' ');
				swap = ((buf[k] - '0') << 7) | (swap >> 1);
				cc[j] = buf[k];
			}
			fprintf(fout,"\t/* %8s */\t%#2x,\n", cc, (unsigned char)swap);
			if (verbose)
				printf("\n\t");
		}
	}
	fprintf(fout, "};\n");
	fclose(fout);
}
