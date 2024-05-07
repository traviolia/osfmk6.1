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
#ifndef _MACH_I386_RPC_H_
#define _MACH_I386_RPC_H_

#include <mach/kern_return.h>
#include <mach/port.h>

/*
 * Definition of RPC "glue code" operations vector -- entry
 * points needed to accomplish short-circuiting
 */
typedef struct rpc_glue_vector {
	kern_return_t	(*rpc_simple)(int, int, void *);
	boolean_t	(*copyin)(mach_port_t, void *);
	boolean_t	(*copyout)(mach_port_t, void *);
	boolean_t	(*copyinstr)(mach_port_t, void *);
	kern_return_t	(*thread_switch)(void *);
	kern_return_t	(*thread_depress_abort)(mach_port_t);
} *rpc_glue_vector_t;

/*
 * Macros used to dereference glue code ops vector -- note
 * hard-wired references to global defined below.  Also note
 * that most of the macros assume their caller has stacked
 * the target args somewhere, so that they can pass just the
 * address of the first arg to the short-circuited
 * implementation.
 */
#define CAN_SHCIRCUIT(name)	(_rpc_glue_vector->name != 0)
#define RPC_SIMPLE(port, rtn_num, argc, argv) \
	((*(_rpc_glue_vector->rpc_simple))(rtn_num, argc, (void *)(&(port))))
#define COPYIN(map, from, to, count) \
	((*(_rpc_glue_vector->copyin))(map, (void *)(&(from))))
#define COPYOUT(map, from, to, count) \
	((*(_rpc_glue_vector->copyout))(map, (void *)(&(from))))
#define COPYINSTR(map, from, to, max, actual) \
	((*(_rpc_glue_vector->copyinstr))(map, (void *)(&(from))))
#define THREAD_SWITCH(thread, option, option_time) \
	((*(_rpc_glue_vector->thread_switch))((void *)(&(thread))))
#define	THREAD_DEPRESS_ABORT(act)	\
	((*(_rpc_glue_vector->thread_depress_abort))(act))

/*
 * The implementation of glue functions for i386
 */
extern kern_return_t machine_rpc_simple(int, int, void *);
extern boolean_t klcopyin(mach_port_t, void *);
extern boolean_t klcopyout(mach_port_t, void *);
extern boolean_t klcopyinstr(mach_port_t, void *);
extern kern_return_t klthread_switch(void *);
extern kern_return_t klthread_depress_abort(mach_port_t);

/*
 * An rpc_glue_vector_t defined either by the kernel or by crt0
 */
extern rpc_glue_vector_t _rpc_glue_vector;

/*
 * User machine dependent macros for mach rpc
 */
#define MACH_RPC(sig_ptr, sig_size, id, dest, arg_list)			   \
	mach_rpc_trap(dest, id, (mach_rpc_signature_t) sig_ptr, sig_size)

/*
 * Kernel machine dependent macros for mach rpc
 *
 * User args (argv) begin two words above the frame pointer (past saved ebp 
 * and return address) on the user stack. Return code is stored in register
 * ecx, by convention (must be a caller-saves register, to survive return
 * from server work function). The user space instruction pointer is eip,
 * and the user stack pointer is uesp.
 */
#define MACH_RPC_ARGV(act)	( (char *)(USER_REGS(act)->ebp + 8) )
#define MACH_RPC_RET(act)	( (kern_return_t)USER_REGS(act)->ecx )
#define MACH_RPC_UIP(act)	( USER_REGS(act)->eip )
#define MACH_RPC_USP(act)	( USER_REGS(act)->uesp )


#endif	/* _MACH_I386_RPC_H_ */
