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
 * Revision 2.3  91/05/14  16:40:19  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/03/16  15:15:51  rpd
 * 	Created.
 * 	[91/03/13            rpd]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
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

#include <mach_counters.h>

#include <kern/counters.h>

/*
 *	We explicitly initialize the counters to make
 *	them contiguous in the kernel's data space.
 *	This makes them easier to examine with ddb.
 */

mach_counter_t c_thread_handoff_hits = 0;
mach_counter_t c_thread_handoff_misses = 0;
mach_counter_t c_thread_invoke_csw = 0;
mach_counter_t c_thread_invoke_hits = 0;
mach_counter_t c_thread_invoke_misses = 0;

#if	MACH_COUNTERS
mach_counter_t c_action_thread_block = 0;
mach_counter_t c_ast_taken_block = 0;
mach_counter_t c_clock_ticks = 0;
mach_counter_t c_exception_raise_block = 0;
mach_counter_t c_exception_raise_state_block = 0;
mach_counter_t c_exception_raise_state_identity_block = 0;
mach_counter_t c_idle_thread_block = 0;
mach_counter_t c_idle_thread_handoff = 0;
mach_counter_t c_io_done_thread_block = 0;
mach_counter_t c_ipc_mqueue_receive_block_kernel = 0;
mach_counter_t c_ipc_mqueue_receive_block_user = 0;
mach_counter_t c_ipc_mqueue_send_block = 0;
mach_counter_t c_mach_msg_trap_block_exc = 0;
mach_counter_t c_mach_msg_trap_block_fast = 0;
mach_counter_t c_mach_msg_trap_block_slow = 0;
mach_counter_t c_net_thread_block = 0;
mach_counter_t c_reaper_thread_block = 0;
mach_counter_t c_sched_thread_block = 0;
mach_counter_t c_stacks_current = 0;
mach_counter_t c_stacks_max = 0;
mach_counter_t c_stacks_min = 0;
mach_counter_t c_stacks_total = 0;
mach_counter_t c_swapin_thread_block = 0;
mach_counter_t c_swtch_block = 0;
mach_counter_t c_swtch_pri_block = 0;
mach_counter_t c_thread_halt_self_block = 0;
mach_counter_t c_thread_switch_block = 0;
mach_counter_t c_thread_switch_handoff = 0;
mach_counter_t c_threads_current = 0;
mach_counter_t c_threads_max = 0;
mach_counter_t c_threads_min = 0;
mach_counter_t c_threads_total = 0;
mach_counter_t c_vm_fault_page_block_backoff_kernel = 0;
mach_counter_t c_vm_fault_page_block_backoff_user = 0;
mach_counter_t c_vm_fault_page_block_busy_kernel = 0;
mach_counter_t c_vm_fault_page_block_busy_user = 0;
mach_counter_t c_vm_fault_retry_on_w_prot;
mach_counter_t c_vm_fault_wait_on_unlock;
mach_counter_t c_vm_page_wait_block = 0;
mach_counter_t c_vm_pageout_block = 0;
mach_counter_t c_vm_pageout_scan_block = 0;
#endif	/* MACH_COUNTERS */
