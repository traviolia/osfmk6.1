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

extern io_return_t	kdopen(
				dev_t		dev,
				int		flag,
				io_req_t	ior);
extern void		kdclose(
				dev_t		dev);
extern io_return_t	kdread(
				dev_t		dev,
				io_req_t	ior);
extern io_return_t	kdwrite(
				dev_t		dev,
				io_req_t	ior);
extern vm_offset_t	kdmmap(
				dev_t		dev,
				vm_offset_t	off,
				int		prot);
extern boolean_t	kdportdeath(
				dev_t		dev,
				ipc_port_t	port);
extern io_return_t	kdgetstat(
				dev_t		dev,
				int		flavor,
				int *		data,
				unsigned int	*count);
extern io_return_t	kdsetstat(
				dev_t		dev,
				int		flavor,
				int *		data,
				unsigned int	count);
extern void		kd_cmdreg_write(
				u_char		val);
extern int		kd_mouse_write(
				u_char		val);
extern void		kd_mouse_read(
				int		no,
				char		* bufp);
extern void		kd_mouse_drain(void);
extern void		kdreboot(void);
extern void		bmpput(
				csrpos_t	pos,
				char		ch,
				char		chattr);
extern void		bmpmvup(
				csrpos_t	from,
				csrpos_t	to,
				int		count);
extern void		bmpmvdown(
				csrpos_t	from,
				csrpos_t	to,
				int		count);
extern void		bmpclear(
				csrpos_t	to,
				int		count,
				char		chattr);
extern void		bmpsetsetcursor(
				csrpos_t	pos);
extern void		kd_slmscu(
				u_char		* from,
				u_char		* to,
				int		count);
extern void		kd_slmscd(
				u_char		* from,
				u_char		* to,
				int		count);
extern void		kd_slmwd(
				u_char		* pos,
				int		count,
				u_short		val);
extern void		kd_sendcmd(
				u_char		c);
