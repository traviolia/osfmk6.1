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

#ifndef	_I386_GDB_DEFS_H_
#define	_I386_GDB_DEFS_H_

/*
 * GDB DEPENDENT DEFINITIONS
 *
 * The following definitions match data descriptions in the gdb source file
 * gdb/include/AT386/tm.h.  They cannot be independently modified.
 */

typedef struct {
	unsigned int	eax;
	unsigned int	ecx;
	unsigned int	edx;
	unsigned int	ebx;
	unsigned int	esp;
	unsigned int	ebp;
	unsigned int	esi;
	unsigned int	edi;
	unsigned int	eip;
	unsigned int	efl;
	unsigned int	cs;
	unsigned int	ss;
	unsigned int	ds;
	unsigned int	es;
	unsigned int	fs;
	unsigned int	gs;
	unsigned int	reason;
} kgdb_regs_t;

#define NUM_REGS 16
#define REGISTER_BYTES (NUM_REGS * 4)

#endif	/* _I386_GDB_DEFS_H_ */

