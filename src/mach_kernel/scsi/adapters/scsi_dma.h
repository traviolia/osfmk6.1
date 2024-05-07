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
 * Revision 2.3  91/11/12  11:17:22  rvb
 * 	Added end_msgin().
 * 	[91/10/30  13:42:14  af]
 * 
 * Revision 2.2  91/08/24  12:27:29  af
 * 	Created.
 * 	[91/08/02  04:14:25  af]
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
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS AS-IS
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
 *	File: scsi_dma.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	7/91
 *
 *	DMA operations that an HBA driver might invoke.
 *
 */

/*
 * This defines much more than usually needed, mainly
 * to cover for the case of no DMA at all and/or only
 * DMA from/to a specialized buffer ( which means the
 * CPU has to copy data into/outof it ).
 */

typedef struct {
	opaque_t	(*init)(
				/* int unit,
				   vm_offset_t base */);

	void		(*new_target)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);

	void		(*map)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);

	int		(*start_cmd)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);

	void		(*end_xfer)(
				/* opaque_t dma_state,
				   target_info_t *tgt,
				   int xferred */);

	void		(*end_cmd)(
				/* opaque_t dma_state,
				   target_info_t *tgt,
				   io_req_t ior */);

	int		(*start_datain)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);

	int		(*start_msgin)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);

	void		(*end_msgin)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);

	boolean_t	(*start_dataout)(
				/* opaque_t dma_state,
				   target_info_t *tgt,
				   unsigned *regp,
				   unsigned value */);

	int		(*restart_datain_1)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);

	int		(*restart_datain_2)(
				/* opaque_t dma_state,
				   target_info_t *tgt,
				   int xferred */);

	void		(*restart_datain_3)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);

	int		(*restart_dataout_1)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);

	int		(*restart_dataout_2)(
				/* opaque_t dma_state,
				   target_info_t *tgt,
				   int xferred */);

	int		(*restart_dataout_3)(
				/* opaque_t dma_state,
				   target_info_t *tgt,
				   unsigned *regp */);

	void		(*restart_dataout_4)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);

	boolean_t	(*disconn_1)(
				/* opaque_t dma_state,
				   target_info_t *tgt,
				   int xferred */);

	boolean_t	(*disconn_2)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);

	boolean_t	(*disconn_3)(
				/* opaque_t dma_state,
				   target_info_t *tgt,
				   int xferred */);

	boolean_t	(*disconn_4)(
				/* opaque_t dma_state,
				   target_info_t *tgt,
				   int xferred */);

	boolean_t	(*disconn_5)(
				/* opaque_t dma_state,
				   target_info_t *tgt,
				   int xferred */);

	void		(*disconn_callback)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);

} scsi_dma_ops_t;

