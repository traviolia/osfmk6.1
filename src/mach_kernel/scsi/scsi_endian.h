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
 * Revision 2.2  91/06/19  14:29:16  rvb
 * 	Created.
 * 	[91/06/11            af]
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
 *	File: scsi_endian.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	6/91
 *
 *	Byte/Bit order issues are solved here.
 */


#ifndef	_SCSI_ENDIAN_H_
#define	_SCSI_ENDIAN_H_	1

/*
 * Macros to take care of bitfield placement within a byte.
 * It might be possible to extend these to something that
 * takes care of multibyte structures, using perhaps the
 * type ("t") parameter.  Someday.
 */
#if	BYTE_MSF

#define	BITFIELD_2(t,a,b)		t b,a
#define	BITFIELD_3(t,a,b,c)		t c,b,a
#define	BITFIELD_4(t,a,b,c,d)		t d,c,b,a
#define	BITFIELD_5(t,a,b,c,d,e)		t e,d,c,b,a
#define	BITFIELD_6(t,a,b,c,d,e,f)	t f,e,d,c,b,a
#define	BITFIELD_7(t,a,b,c,d,e,f,g)	t g,f,e,d,c,b,a
#define	BITFIELD_8(t,a,b,c,d,e,f,g,h)	t h,g,f,e,d,c,b,a

#else	/*BYTE_MSF*/

#define	BITFIELD_2(t,a,b)		t a,b
#define	BITFIELD_3(t,a,b,c)		t a,b,c
#define	BITFIELD_4(t,a,b,c,d)		t a,b,c,d
#define	BITFIELD_5(t,a,b,c,d,e)		t a,b,c,d,e
#define	BITFIELD_6(t,a,b,c,d,e,f)	t a,b,c,d,e
#define	BITFIELD_7(t,a,b,c,d,e,f,g)	t a,b,c,d,e,f,g
#define	BITFIELD_8(t,a,b,c,d,e,f,g,h)	t a,b,c,d,e,f,g,h

#endif	/*BYTE_MSF*/

#endif	/*_SCSI_ENDIAN_H_*/
