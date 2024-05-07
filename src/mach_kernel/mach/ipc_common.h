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

#ifndef	_MACH_IPC_COMMON_H_
#define _MACH_IPC_COMMON_H_

typedef unsigned int ipc_kobject_t;	/* for kern/ipc_kobject.h	*/

typedef unsigned int ipc_object_refs_t;	/* for ipc/ipc_object.h		*/
typedef unsigned int ipc_object_bits_t;
typedef unsigned int ipc_object_type_t;

/*
 * There is no lock in the ipc_object; it is in the enclosing kernel
 * data structure (rpc_common_data) used by both ipc_port and ipc_pset.
 * The ipc_object is used to both tag and reference count these two data
 * structures, and (Noto Bene!) pointers to either of these or the
 * ipc_object at the head of these are freely cast back and forth; hence
 * the ipc_object MUST BE FIRST in the ipc_common_data.
 * 
 * This peculiar structuring avoids having anything in user code depend
 * on the kernel configuration (with which lock size varies), since the
 * ipc_common_data also comprises the bulk of an rpc_port, used by
 * user-mode RPC code.
 */
struct ipc_object {
	ipc_object_refs_t io_references;
	ipc_object_bits_t io_bits;
};

/*
 * Common sub-structure at the head of each
 * ipc_port, ipc_pset, and rpc_port.  An rpc_port is
 * visible to both user-mode and kernel-loaded rpc code,
 * as well as to the kernel port/set structures.
 */
typedef struct ipc_common_data {
	struct ipc_object	icd_object;
	ipc_kobject_t		icd_kobject;
	struct rpc_subsystem *	icd_subsystem;
	unsigned int		icd_sobject;
	unsigned int		icd_sbits;
	mach_port_t		icd_receiver_name;
} *ipc_common_t;

#endif	/* _MACH_IPC_COMMON_H_ */
