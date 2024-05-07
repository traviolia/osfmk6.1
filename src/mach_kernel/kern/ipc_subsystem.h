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

#ifndef	_KERN_IPC_SUBSYSTEM_H_
#define _KERN_IPC_SUBSYSTEM_H_

#include <mach/boolean.h>
#include <mach/port.h>
#include <kern/subsystem.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_object.h>


/* Initialize a subsystem's IPC state */
extern void ipc_subsystem_init(
	subsystem_t		subsystem);

/* Enable a subsystem for IPC access */
extern void ipc_subsystem_enable(
	subsystem_t		subsystem);

/* Disable IPC access to a subsystem */
extern void ipc_subsystem_disable(
	subsystem_t		subsystem);

/* Clean up and destroy a subsystem's IPC state */
extern void ipc_subsystem_terminate(
	subsystem_t		subsystem);

/* Convert from a port to a subsystem */
extern subsystem_t convert_port_to_subsystem(
	ipc_port_t	port);

/* Convert from a subsystem to a port */
extern ipc_port_t convert_subsystem_to_port(
	subsystem_t	subsystem);

#endif	/* _KERN_IPC_SUBSYSTEM_H_ */
