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

#ifndef _NDR_H_
#define _NDR_H_

typedef struct {
    unsigned char       mig_vers;
    unsigned char       if_vers;
    unsigned char       reserved1;
    unsigned char       mig_encoding;
    unsigned char       int_rep;
    unsigned char       char_rep;
    unsigned char       float_rep;
    unsigned char       reserved2;
} NDR_record_t;

/*
 * MIG supported protocols for Network Data Representation
 */
#define  NDR_PROTOCOL_2_0      0

/*
 * NDR 2.0 format flag type definition and values.
 */
#define  NDR_INT_BIG_ENDIAN    0
#define  NDR_INT_LITTLE_ENDIAN 1
#define  NDR_FLOAT_IEEE        0
#define  NDR_FLOAT_VAX         1
#define  NDR_FLOAT_CRAY        2
#define  NDR_FLOAT_IBM         3
#define  NDR_CHAR_ASCII        0
#define  NDR_CHAR_EBCDIC       1

extern NDR_record_t NDR_record;

#endif /* _NDR_H_ */
