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

#ifndef	_MISC_PROTOS_H_
#define	_MISC_PROTOS_H_

#include <norma_ipc.h>
#include <norma_ether.h>

#include <stdarg.h>
#include <machine/setjmp.h>
#include <mach/boolean.h>
#include <mach/machine/vm_types.h>
#include <ipc/ipc_types.h>

/* Set a bit in a bit array */
extern void setbit(
	int		which,
	int		*bitmap);

/* Clear a bit in a bit array */
extern void clrbit(
	int		which,
	int		*bitmap);

/* Find the first set bit in a bit array */
extern int ffsbit(
	int		*bitmap);
extern int ffs(
	unsigned long	mask);

/* Move arbitrarily-aligned data from one array to another */
extern void bcopy(
	const char	*from,
	char		*to,
	int		nbytes);

/* Zero an arbitrarily-aligned array */
extern void bzero(
	const char	*from,
	int		nbytes);

/* Move arbitrarily-aligned data from a user space to kernel space */
extern boolean_t copyin(
	const char	*user_addr,
	char		*kernel_addr,
	int		nbytes);

/* Move a NUL-terminated string from a user space to kernel space */
extern boolean_t copyinstr(
	const char	*user_addr,
	char		*kernel_addr,
	int		max,
	int		*actual);

/* Move arbitrarily-aligned data from a user space to kernel space */
extern boolean_t copyinmsg(
	const char	*user_addr,
	char		*kernel_addr,
	int		nbytes);

/* Move arbitrarily-aligned data from a kernel space to user space */
extern boolean_t copyout(
	const char	*kernel_addr,
	char		*user_addr,
	int		nbytes);

/* Move arbitrarily-aligned data from a kernel space to user space */
extern boolean_t copyoutmsg(
	const char	*kernel_addr,
	char		*user_addr,
	int		nbytes);

extern void printf(const char *format, ...);

extern void printf_init(void);

extern void panic(char *string, ...);

extern void panic_init(void);

extern void log(int level, char *fmt, ...);

extern void _doprnt(
	const char	*fmt,
	va_list		*argp,
 	void		(*putc)(char),
	int		radix);

extern void safe_gets(
	char	*str,
	int	maxlen);

extern void cnputc(
	char	ch);

extern int cngetc(void);

extern int cnmaygetc(void);

extern int _setjmp(
	jmp_buf_t	*jmp_buf);

extern int _longjmp(
	jmp_buf_t	*jmp_buf,
	int		value);

extern void bootstrap_create(void);

extern void halt_cpu(void);

extern void halt_all_cpus(
		boolean_t	reboot);

extern void Debugger(
		const char	* message);

extern void delay(
		int		n);

extern char *machine_boot_info(
		char		*buf,
		vm_size_t	buf_len);

#if	NORMA_IPC
extern boolean_t no_bootstrap_task(void);

extern ipc_port_t get_root_master_device_port(void);

#if	!NORMA_ETHER
extern char *norma_machine_get_info(void);
#endif	/* !NORMA_ETHER */

#endif	/* NORMA_IPC */

#endif	/* _MISC_PROTOS_H_ */
