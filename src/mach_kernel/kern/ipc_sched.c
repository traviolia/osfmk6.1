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
 * Revision 2.10  91/07/31  17:45:30  dbg
 * 	Check for new thread bound to wrong processor in thread_handoff.
 * 	[91/07/25            dbg]
 * 
 * 	Fix timeout race.
 * 	[91/05/23            dbg]
 * 
 * 	Revise scheduling state machine.
 * 	[91/05/22            dbg]
 * 
 * Revision 2.9  91/06/25  10:28:37  rpd
 * 	Added some wait_result assertions.
 * 	[91/05/30            rpd]
 * 
 * Revision 2.8  91/05/18  14:31:50  rpd
 * 	Updated thread_handoff to check stack_privilege.
 * 	[91/03/30            rpd]
 * 
 * Revision 2.7  91/05/14  16:42:33  mrt
 * 	Correcting copyright
 * 
 * Revision 2.6  91/03/16  14:50:09  rpd
 * 	Rewrote ipc_thread_switch as thread_handoff,
 * 	with new stack_handoff replacing stack_switch.
 * 	Renamed ipc_thread_{go,will_wait,will_wait_with_timeout}
 * 	to thread_{go,will_wait,will_wait_with_timeout}.
 * 	[91/02/17            rpd]
 * 	Removed ipc_thread_switch_hits.
 * 	[91/01/28            rpd]
 * 	Allow swapped threads on the run queues.
 * 	[91/01/17            rpd]
 * 
 * Revision 2.5  91/02/05  17:26:53  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:13:23  mrt]
 * 
 * Revision 2.4  91/01/08  15:15:52  rpd
 * 	Added KEEP_STACKS support.
 * 	[91/01/06            rpd]
 * 	Added ipc_thread_switch_hits, ipc_thread_switch_misses counters.
 * 	[91/01/03  22:07:15  rpd]
 * 
 * 	Modified ipc_thread_switch to deal with pending timeouts.
 * 	[90/12/20            rpd]
 * 	Removed ipc_thread_go_and_block.
 * 	Added ipc_thread_switch.
 * 	[90/12/08            rpd]
 * 
 * Revision 2.3  90/08/27  22:02:40  dbg
 * 	Pass correct number of arguments to thread_swapin.
 * 	[90/07/17            dbg]
 * 
 * Revision 2.2  90/06/02  14:54:22  rpd
 * 	Created for new IPC.
 * 	[90/03/26  23:45:59  rpd]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
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

#include <cpus.h>
#include <mach_host.h>

#include <mach/message.h>
#include <kern/counters.h>
#include <kern/cpu_number.h>
#include <kern/lock.h>
#include <kern/thread.h>
#include <kern/sched_prim.h>
#include <kern/processor.h>
#include <kern/time_out.h>
#include <kern/thread_swap.h>
#include <kern/ipc_sched.h>
#include <kern/misc_protos.h>
#include <machine/pmap.h>

/*
 *	Utah: These functions really belong in kern/sched_prim.c.
 *
 *		Functions?  What functions?
 */
static int this_is_not_an_empty_file = 0; /* ;-) */
