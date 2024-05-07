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

/*
 * Buffer for x86 saved state used by kgdb_setjmp/kgdb_longjmp
 */
#ifndef	_KGDB_SETJMP_H_
#define	_KGDB_SETJMP_H_

typedef	struct kgdb_jmp_buf {
	int	jmp_buf[6];	/* x86 registers */
} kgdb_jmp_buf_t;

#define	JMP_BUF_NULL (kgdb_jmp_buf_t *) 0

extern int kgdb_setjmp(
	kgdb_jmp_buf_t	*kgdb_jmp_buf);

extern int kgdb_longjmp(
	kgdb_jmp_buf_t	*kgdb_jmp_buf,
	int		value);

#endif	/* _KGDB_SETJMP_H_ */
