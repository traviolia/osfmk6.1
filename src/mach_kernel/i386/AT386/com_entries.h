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

extern io_return_t	comopen(
				dev_t			dev,
				int			flag,
				io_req_t		ior);
extern void		comclose(
				dev_t			dev);
extern int		comread(
				dev_t			dev,
				io_req_t		ior);
extern int		comwrite(
				dev_t			dev,
				io_req_t		ior);
extern io_return_t	comgetstat(
				dev_t			dev,
				int			flavor,
				int			* data,
				unsigned int		* count);
extern io_return_t	comsetstat(
				dev_t			dev,
				int			flavor,
				int			* data,
				unsigned int		count);
extern int		comportdeath(
				dev_t			dev,
				ipc_port_t		port);
extern void		comintr(
				int			unit);
extern void		comreset(void);
extern void		com_putc(
				char		ch);
extern int		com_getc(
				boolean_t	wait);
extern boolean_t	com_is_char(void);

