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
 * Definitions for task ledger line items
 *
 *	LEDGER_REAL_ITEMS are actually stored in the ledger,
 *	while the data for the rest is "forwarded" elsewhere.
 *
 *	LEDGER_NITEMS is the total, including both sorts.
 */
#define ITEM_THREADS		0	/* number of threads	*/
#define ITEM_TASKS		1	/* number of tasks	*/

#define ITEM_VM	   		2	/* virtual space (bytes)*/

#ifdef	OSF1_SERVER
#define LEDGER_REAL_ITEMS	3	/* number "real" items	*/
#endif	/* OSF1_SERVER */
#define LEDGER_N_ITEMS		3	/* Total line items	*/

#define LEDGER_UNLIMITED	0	/* ignored item.maximum	*/
