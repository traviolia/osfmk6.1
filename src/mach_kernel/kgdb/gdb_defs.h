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
 * 
 */
/*
 * OSF Research Institute MK6.1 (unencumbered) 1/31/1995
 */

#ifndef	GDB_DEFS_H_
#define	GDB_DEFS_H_

#include <machine/gdb_defs.h>

/*
 * GDB DEPENDENT DEFINITIONS
 *
 * The following definitions match data descriptions in the gdb source file
 * gdb/edm-remote.c.  They cannot be independently modified.
 */

#define PBUFSIZ 1024
#define KGDB_PROTOCOL_VERSION 1

/*
 * Communications protocol characters
 */
#define	STX	0x02	/* Start of packet */
#define	ETX	0x03	/* End of packet */
#define ACK	0x06	/* Positive Acknowlegement */
#define NAK	0x15	/* Negative Acknowlegement */

/*
 * Entry/Error flags
 */
#define	KGDB_BREAK_POINT	0	/* Break point trap */
#define	KGDB_SINGLE_STEP	1	/* Single step tran */
#define KGDB_COMMAND		2	/* Command from from gdb */
#define KGDB_EXCEPTION		3	/* Unexpected kernel exception */
#define KGDB_KEXCEPTION		4	/* Unexpected kgdb exception */
#define KGDB_KERROR		5	/* Internal kgdb error */
#define KGDB_MEM_ERROR		6	/* Internal kgdb error */

#define KGDB_SIGTRAP		5

#define ERROR_SHIFT		16
#define ERROR_MASK		0xFFFF

/*
 * END OF GDB DEPENDENT DEFINITIONS
 */

#endif	/* GDB_DEFS_H_ */

