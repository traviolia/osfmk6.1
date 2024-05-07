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

#include <kern/ipc_kobject.h>
#include <kern/ipc_sync.h>
#include <ipc/port.h>
#include <ipc/ipc_space.h>
#include <ipc/ipc_port.h>
#include <mach/sync_server.h>
#include <mach/mach_server.h>
#include <mach/mach_port_server.h>
#include <mach/port.h>


semaphore_t
convert_port_to_semaphore (ipc_port_t port)
{
	semaphore_t semaphore = SEMAPHORE_NULL;

	if (IP_VALID (port)) {
		ip_lock(port);
		if (ip_active (port) && (ip_kotype (port) == IKOT_SEMAPHORE))
			semaphore = (semaphore_t) port->ip_kobject;
		ip_unlock(port);
	}

	return (semaphore);
}

ipc_port_t
convert_semaphore_to_port (semaphore_t semaphore)
{
	ipc_port_t port;

	port = ipc_port_make_send (semaphore->port);
	return (port);
}

lock_set_t
convert_port_to_lock_set (ipc_port_t port)
{
	lock_set_t lock_set = LOCK_SET_NULL;

	if (IP_VALID (port)) {
		ip_lock(port);
		if (ip_active (port) && (ip_kotype (port) == IKOT_LOCK_SET))
			lock_set = (lock_set_t) port->ip_kobject;
		ip_unlock(port);
	}

	return (lock_set);
}

ipc_port_t
convert_lock_set_to_port (lock_set_t lock_set)
{
	ipc_port_t port;

	port = ipc_port_make_send(lock_set->port);
	return (port);
}

