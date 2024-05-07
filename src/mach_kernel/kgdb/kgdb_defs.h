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

#ifndef	_KGDB_DEFS_H_
#define	_KGDB_DEFS_H_

#include <stdarg.h>
#include <mach/boolean.h>
#include <kgdb/gdb_defs.h>
#include <machine/kgdb_defs.h>
#include <machine/kgdb_setjmp.h>

extern boolean_t kgdb_debug;
extern boolean_t kgdb_initialized;

#define KGDB_DEBUG(args) if (kgdb_debug) kgdb_printf args

extern kgdb_jmp_buf_t *kgdb_recover;

extern const char *kgdb_panic_msg;

extern char kgdb_data[];		/* In/Out data area */

/*
 * request/response format
 */
typedef struct kgdb_cmd_pkt {
	char		k_cmd;
	vm_offset_t	k_addr;
	int		k_size;
	vm_offset_t	k_data;
} kgdb_cmd_pkt_t;

#define KGDB_CMD_NO_ADDR 0xffffffff

/*
 * Kernel kgdb state 
 */
extern int kgdb_state;

#define	STEP_NONE	0
#define	STEP_ONCE	1

/*
 * Exported interfaces
 */
extern void kgdb_printf( const char *format, ...);

extern void kgdb_panic(
	const char 		*message);

extern void kgdb_enter(
	int		generic_type,
 	int		type,
	kgdb_regs_t	*regs);

extern void kgdb_init(void);

extern void kgdb_machine_init(void);

extern int kgdb_fromhex(
	int		val);

extern int kgdb_tohex(
	int		val);

extern void kgdb_putpkt(
	kgdb_cmd_pkt_t	*snd);

extern void kgdb_getpkt(
	kgdb_cmd_pkt_t	*rcv);

extern void kgdb_set_single_step(
	kgdb_regs_t	*regs);

extern void kgdb_clear_single_step(
	kgdb_regs_t	*regs);

boolean_t kgdb_in_single_step(void);

#endif	/* _KGDB_DEFS_H_ */
