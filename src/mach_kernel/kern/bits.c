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
/*
 * C version of bit manipulation routines now required by kernel.
 * Should be replaced with assembler versions in any real port.
 */
#include <mach/machine/vm_param.h>	/* for BYTE_SIZE */

#define INT_SIZE	(BYTE_SIZE * sizeof (int))

/*
 * Set indicated bit in bit string.
 */
void
setbit(int bitno, int *s)
{
	for ( ; INT_SIZE <= bitno; bitno -= INT_SIZE, ++s)
		;
	*s |= 1 << bitno;
}

/*
 * Clear indicated bit in bit string.
 */
void
clrbit(int bitno, int *s)
{
	for ( ; INT_SIZE <= bitno; bitno -= INT_SIZE, ++s)
		;
	*s &= ~(1 << bitno);
}

/*
 * Find first bit set in bit string.
 */
int
ffsbit(int *s)
{
	int offset, mask;

	for (offset = 0; !*s; offset += INT_SIZE, ++s)
		;
	for (mask = 1; mask; mask <<= 1, ++offset)
		if (mask & *s)
			return (offset);
	/*
	 * Shouldn't get here
	 */
	return (0);
}
