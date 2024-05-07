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

#include <i386/thread.h>

extern void		get_root_device(void);
extern void		picinit(void);
extern void		ovbcopy(
				char		* from,
				char		* to,
				int		bytes);
extern void		delay(
				int		n);
extern void		slave_clock(void);
extern void		interrupt_processor(
				int		cpu);
extern void		mp_probe_cpus(void);
extern void		remote_kdb(void);
extern boolean_t	is_486(void);
extern void		cpu_shutdown(void);
extern void		fix_desc(
				void		* desc,
				int		num_desc);
extern void		cnpollc(
				boolean_t	on);
extern void		form_pic_mask(void);
extern void		intnull(
				int		unit);
extern void 		hardclock(
				int 	vect,	
			        int 	ipl,
			        char 	*ret_addr,
			        struct 	i386_interrupt_state *regs);

extern void		blkclr(
			       const char	*from,
			       int		nbytes);

extern void		kdb_kintr(void);
extern void		kdb_console(void);

extern unsigned	long  	ntohl(unsigned long);
extern char *		machine_boot_info(
				char		*buf,
				vm_size_t	buf_len);
