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
 * Revision 2.20.3.4  92/09/15  17:21:21  jeffreyh
 * 	Export paging and unique naming for default pager file.
 * 	[92/09/10            stans@ssd.intel.com]
 * 
 * 	Do not invoke the default pager on nodes > 0 for the iPSC860.  If
 * 	no pager is set up, this will prevent the various kernels from
 * 	overwriting paged out pages.
 * 	[92/07/13            sjs]
 * 
 * Revision 2.20.3.3  92/04/30  11:58:58  bernadat
 * 	Pass to the server arguments typed at boot prompt.
 * 	[92/03/19            bernadat]
 * 
 * Revision 2.20.3.2  92/03/28  10:09:49  jeffreyh
 * 	Changes from Intel
 * 	[92/03/26  11:52:02  jeffreyh]
 * 
 * 	Added logic to pass -v (verbose) flag to servers
 * 	[92/03/16   	sjs]
 * 
 * Revision 2.20.3.1  92/02/18  19:07:43  jeffreyh
 * 	Changes for iPSC. These should be made more general in the future.
 * 	[92/02/18            jeffreyh]
 * 
 * Revision 2.20  91/12/10  16:32:40  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:51:50  jsb]
 * 
 * Revision 2.19  91/11/12  11:51:53  rvb
 * 	Added task_insert_send_right.
 * 	Changed BOOTSTRAP_MAP_SIZE to 4 meg.
 * 	[91/11/12            rpd]
 * 
 * Revision 2.18  91/09/12  16:37:49  bohman
 * 	Made bootstrap task call mac2 machine dependent code before running
 * 	'startup', which is loaded from the UX file system.  This needs to
 * 	be handled more generally in the future.
 * 	[91/09/11  17:07:59  bohman]
 * 
 * Revision 2.17  91/08/28  11:14:22  jsb
 * 	Changed msgh_kind to msgh_seqno.
 * 	[91/08/10            rpd]
 * 
 * Revision 2.16  91/08/03  18:18:45  jsb
 * 	Moved bootstrap query earlier. Removed all NORMA specific code.
 * 	[91/07/25  18:25:35  jsb]
 * 
 * Revision 2.15  91/07/31  17:44:14  dbg
 * 	Pass host port to boot_load_program and read_emulator_symbols.
 * 	[91/07/30  17:02:40  dbg]
 * 
 * Revision 2.14  91/07/01  08:24:54  jsb
 * 	Removed notion of master/slave. Instead, refuse to start up
 * 	a bootstrap task whenever startup_name is null.
 * 	[91/06/29  16:48:14  jsb]
 * 
 * Revision 2.13  91/06/19  11:55:57  rvb
 * 	Ask for startup program to override default.
 * 	[91/06/18  21:39:17  rvb]
 * 
 * Revision 2.12  91/06/17  15:46:51  jsb
 * 	Renamed NORMA conditionals.
 * 	[91/06/17  10:49:04  jsb]
 * 
 * Revision 2.11  91/06/06  17:06:53  jsb
 * 	Allow slaves to act as masters (for now).
 * 	[91/05/13  17:36:17  jsb]
 * 
 * Revision 2.10  91/05/18  14:31:32  rpd
 * 	Added argument to kernel_thread.
 * 	[91/04/03            rpd]
 * 
 * Revision 2.9  91/05/14  16:40:06  mrt
 * 	Correcting copyright
 * 
 * Revision 2.8  91/02/05  17:25:42  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:11:22  mrt]
 * 
 * Revision 2.7  90/12/14  11:01:58  jsb
 * 	Changes to NORMA_BOOT support. Use real device port, not a proxy;
 * 	new device forwarding code handles forwarding of requests.
 * 	Have slave not bother starting bootstrap task if there is nothing
 * 	for it to run.
 * 	[90/12/13  21:37:57  jsb]
 * 
 * Revision 2.6  90/09/28  16:55:30  jsb
 * 	Added NORMA_BOOT support.
 * 	[90/09/28  14:04:43  jsb]
 * 
 * Revision 2.5  90/06/02  14:53:39  rpd
 * 	Load emulator symbols.
 * 	[90/05/11  16:58:37  rpd]
 * 
 * 	Made bootstrap_task available externally.
 * 	[90/04/05            rpd]
 * 	Converted to new IPC.
 * 	[90/03/26  22:03:39  rpd]
 * 
 * Revision 2.4  90/01/11  11:43:02  dbg
 * 	Initialize bootstrap print routines.  Remove port number
 * 	printout.
 * 	[89/12/20            dbg]
 * 
 * Revision 2.3  89/11/29  14:09:01  af
 * 	Enlarged the bootstrap task's map to accomodate some unnamed
 * 	greedy RISC box.  Sigh.
 * 	[89/11/07            af]
 * 	Made root_name and startup_name non-preallocated, so that
 * 	they can be changed at boot time on those machines like
 * 	mips and Sun where the boot loader passes command line
 * 	arguments to the kernel.
 * 	[89/10/28            af]
 * 
 * Revision 2.2  89/09/08  11:25:02  dbg
 * 	Pass root partition name to default_pager_setup.
 * 	[89/08/31            dbg]
 * 
 * 	Assume that device service has already been created.
 * 	Create bootstrap task here and give it the host and
 * 	device ports.
 * 	[89/08/01            dbg]
 * 
 * 	Call default_pager_setup.
 * 	[89/07/11            dbg]
 * 
 * 12-Apr-89  David Golub (dbg) at Carnegie-Mellon University
 *	Removed console_port.
 *
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
/*
 * Bootstrap the various built-in servers.
 */

#include <mach_kdb.h>
#include <bootstrap_symbols.h>
#include <norma_ipc.h>

#include <mach/port.h>
#include <mach/message.h>


#include <mach/vm_param.h>
#include <mach/task_special_ports.h>
#include <mach/mach_server.h>
#include <mach/bootstrap_server.h>
#include <mach/mach_port_server.h>
#include <ipc/ipc_space.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_entry.h>
#include <kern/host.h>
#include <kern/task.h>
#include <kern/thread.h>
#include <kern/misc_protos.h>
#include <vm/vm_fault.h>
#include <device/device_port.h>
#include <sys/reboot.h>
#include <ddb/db_aout.h>
#include <string.h>

/*
 * Externs (itoa is not ANSI, so we can't use a standard include)
 */
extern int	itoa(
			int	i,
			char	*s);
/*
 * Forward declarations
 */
void bootstrap(void);

vm_offset_t move_bootstrap(void);

mach_port_t task_insert_send_right(
	task_t		task,
	ipc_port_t	port);

static void get_boot_flags(
	char 	*str);

void ovbcopy_ints(
	vm_offset_t	src,
	vm_offset_t	dst,
	vm_size_t	size);

vm_offset_t boot_map(
	void		*data,
	vm_offset_t	offset);

void copy_bootstrap(
	int		*entry);

void user_bootstrap(void);

void build_args_and_stack(int *entry, ...);

ipc_port_t	master_bootstrap_port;

mach_port_t	bootstrap_master_device_port;	/* local name */
mach_port_t	root_master_device_port;	/* local name */
mach_port_t	bootstrap_master_host_port;	/* local name */

extern char	*root_name;
extern char	*boot_string;

task_t	bootstrap_task;

#define	BOOTSTRAP_MAP_SIZE	(4 * 1024 * 1024)

#include <stdarg.h>

#include <mach/boot_info.h>

/*
 *	Bootstrap image is moved out of BSS at startup.
 */
vm_offset_t	boot_start = 0;		/* pointer to bootstrap image */
vm_size_t	boot_size = 0;		/* size of bootstrap image */
vm_offset_t	load_info_start = 0;	/* pointer to bootstrap load info */
vm_size_t	load_info_size = 0;	/* size of bootstrap load info */
vm_offset_t	kern_sym_start = 0;	/* pointer to kernel symbols */
vm_size_t	kern_sym_size = 0;	/* size of kernel symbols */

#if	DEBUG
void
load_info_print(void)
{
	struct loader_info *lp = (struct loader_info *)load_info_start;

	printf("Load info: text (%#x, %#x, %#x)\n",
		lp->text_start, lp->text_size, lp->text_offset);
	printf("           data (%#x, %#x, %#x)\n",
		lp->data_start, lp->data_size, lp->data_offset);
	printf("           bss  (%#x)\n", lp->bss_size);
	printf("           syms (%#x, %#x)\n",
		lp->sym_offset, lp->sym_size);
	printf("	   entry(%#x, %#x)\n",
		lp->entry_1, lp->entry_2);
}
#endif

/*
 *	Moves kernel symbol table, bootstrap image, and bootstrap
 *	load information out of BSS at startup.  Returns the
 *	first unused address.
 *
 *	PAGE_SIZE must be set.
 *
 *	On some machines, this code must run before the page
 *	tables are set up, and therefore must be re-created
 *	in assembly language.
 */

void
ovbcopy_ints(
	vm_offset_t	src,
	vm_offset_t	dst,
	vm_size_t	size)
{
	register int *srcp;
	register int *dstp;
	register unsigned int count;

	srcp = (int *)(src + size);
	dstp = (int *)(dst + size);
	count = size / sizeof(int);

	while (count-- != 0)
	    *--dstp = *--srcp;
}

extern char	edata[];	/* start of BSS */
extern char	end[];		/* end of BSS */

vm_offset_t
move_bootstrap(void)
{
	struct boot_info *bi = (struct boot_info *)edata;

	kern_sym_start = (vm_offset_t) end;
	kern_sym_size  = bi->sym_size;
	/*
	 * Align start of bootstrap on page boundary,
	 * to allow mapping into user space.
	 */
	boot_start = round_page(kern_sym_start + kern_sym_size);
	boot_size  = bi->boot_size;
	load_info_start = boot_start + boot_size;
	load_info_size  = bi->load_info_size;

#if DEBUG
	load_info_print();
#endif

	ovbcopy_ints((vm_offset_t)bi + sizeof(struct boot_info) + kern_sym_size,
		     boot_start,
		     boot_size + load_info_size);

	ovbcopy_ints((vm_offset_t)bi + sizeof(struct boot_info),
		     kern_sym_start,
		     kern_sym_size);

	return boot_start + boot_size + load_info_size;
}
/*
 * Copy boot_data (executable) to the user portion of this task.
 */
boolean_t	load_protect_text = TRUE;
#if MACH_KDB
		/* if set, fault in the text segment if desired*/
boolean_t	load_fault_in_text = FALSE;
#endif

vm_offset_t
boot_map(
	void		*data,	/* private data */
	vm_offset_t	offset)	/* offset to map */
{
	vm_offset_t	start_offset = (vm_offset_t) data;

	return pmap_extract(kernel_pmap, start_offset + offset);
}


#if BOOTSTRAP_SYMBOLS
boolean_t load_bootstrap_symbols = TRUE;
#else
boolean_t load_bootstrap_symbols = FALSE;
#endif

void
copy_bootstrap(
	int		*entry)
{
	struct loader_info	*lp;
	vm_offset_t		text_page_start,
				text_page_end,
				data_page_start,
				data_page_end,
				bss_start,
				bss_page_start,
				bss_page_end;

	register vm_map_t	user_map = current_task()->map;
	vm_object_t		boot_object;
	vm_size_t		bss_size;

	/*
	 * Point to bootstrap load information.
	 */
	lp = (struct loader_info *)load_info_start;

	/*
	 * We assume that makeboot has aligned the various
	 * pieces of the bootstrap image on page boundaries.
	 */
	assert(lp->text_start == trunc_page(lp->text_start));
	assert(lp->data_start == trunc_page(lp->data_start));
	assert(lp->text_offset == trunc_page(lp->text_offset));
	assert(lp->data_offset == trunc_page(lp->data_offset)
	    || lp->data_offset == lp->text_offset + lp->text_size);

	/*
	 * Find how much virtual space we have to allocate.
	 */
	text_page_start = trunc_page(lp->text_start);
	text_page_end   = round_page(lp->text_start + lp->text_size);
	data_page_start = trunc_page(lp->data_start);
	data_page_end   = round_page(lp->data_start + lp->data_size);
	bss_start	= lp->data_start + lp->data_size;
	bss_page_start  = trunc_page(bss_start);
	bss_page_end    = round_page(bss_start + lp->bss_size);
	bss_size	= bss_page_end - bss_page_start;


	/*
	 * Create an object that maps the pages in the
	 * bootstrap image.  Map only until the end of the last
	 * whole page.
	 */
	boot_size = lp->data_offset + bss_page_start - lp->data_start;
	boot_object = vm_object_allocate(boot_size);

	vm_object_page_map_internal(boot_object,
			   (vm_offset_t) 0,	/* from boot_start */
			   boot_size,
			   boot_map,
			   (void *) boot_start,
			   FALSE, 		/* wired */
			   FALSE);		/* private */

	/*
	 * Map the text and data from the boot image into
	 * the user task.  Map the data area only through
	 * the last whole page of data - the next page of
	 * data is split between data and bss, and must be
	 * partially cleared.
	 */

	if (text_page_end >= data_page_start) {
	    /*
	     * One contiguous area for text and data.
	     */
	    (void) vm_map_enter(user_map,
			&text_page_start,
			(vm_size_t) (bss_page_start - text_page_start),
			(vm_offset_t) 0, FALSE,
			boot_object,
			lp->text_offset,
			FALSE,
			VM_PROT_READ | VM_PROT_WRITE,
			VM_PROT_ALL,
			VM_INHERIT_DEFAULT);

	}
	else {
	    /*
	     * Separated text and data areas.
	     */
	    (void) vm_map_enter(user_map,
			&text_page_start,
			(vm_size_t)(text_page_end - text_page_start),
			(vm_offset_t) 0, FALSE,
			boot_object,
			lp->text_offset,
			FALSE,
			VM_PROT_READ | VM_PROT_WRITE,
			VM_PROT_ALL,
			VM_INHERIT_DEFAULT);

	    if (data_page_start < bss_page_start) {
		    (void) vm_map_enter(user_map,
				&data_page_start,
				(vm_size_t)(bss_page_start - data_page_start),
				(vm_offset_t) 0, FALSE,
				boot_object,
				lp->data_offset,
				FALSE,
				VM_PROT_READ | VM_PROT_WRITE,
				VM_PROT_ALL,
				VM_INHERIT_DEFAULT);
	    boot_object->ref_count++;
	    }
	}


	/*
	 * Allocate the remainder of the data segment and all
	 * of the BSS. Protect against zero size bss.
	 */
	if (bss_size)
	  {
	    (void) vm_allocate(user_map, &bss_page_start, 
			       (vm_size_t)(bss_size), FALSE);

	    /*
	     * If the data segment does not end on a VM page boundary,
	     * we copy the end of the data segment onto a new page
	     * so that the bss segment will be zero, and so that
	     * we do not overwrite the bootstrap symbol table.
	     */
	    if (bss_start > bss_page_start) {
	      (void) copyout((const char *) boot_start +
			     lp->data_offset +
			     lp->data_size -
			     (bss_start - bss_page_start),
			     (char *) bss_page_start,
			     bss_start - bss_page_start);
	    }
	  } 

	/*
	 * Protect the text.
	 */
	if (load_protect_text)
	    if (text_page_end >= data_page_start)
		(void) vm_protect(user_map,
			text_page_start,
			(vm_size_t)(trunc_page(lp->text_start + lp->text_size)
				    - text_page_start),
			FALSE,
			VM_PROT_READ|VM_PROT_EXECUTE);
	    else
		(void) vm_protect(user_map,
			text_page_start,
			(vm_size_t)(text_page_end - text_page_start),
			FALSE,
			VM_PROT_READ|VM_PROT_EXECUTE);
#if	MACH_KDB
	/*
	 * Enter the bootstrap symbol table.
	 */

	if (load_bootstrap_symbols)
	(void) X_db_sym_init(
		(char *)(boot_start + lp->sym_offset[0]),
		(char *)(boot_start + lp->sym_offset[0] + lp->sym_size[0]),
		"bootstrap",
		(char *) user_map);

	if (load_fault_in_text)
	  {
	    vm_offset_t lenp = round_page(lp->text_start+lp->text_size) -
	      		     trunc_page(lp->text_start);
	    vm_offset_t i = 0;

	    while (i < lenp)
	      {
		vm_fault(user_map, text_page_start +i, 
		         load_protect_text ?  
			 VM_PROT_READ|VM_PROT_EXECUTE :
			 VM_PROT_READ|VM_PROT_EXECUTE | VM_PROT_WRITE,
			 FALSE);
		i = round_page (i+1);
	      }
	  }


#endif	/* MACH_KDB */
	/*
	 * Return the entry points.
	 */
	entry[0] = lp->entry_1;
	entry[1] = lp->entry_2;
}

/*
 * Allocate the stack, and build the argument list.
 */
void
build_args_and_stack(int *entry, ...)
{
	vm_offset_t	stack_base;
	vm_size_t	stack_size;
	va_list		argv_ptr;
	register
	char *		arg_ptr;
	int		arg_len;
	int		arg_count;
	register
	char *		arg_pos;
	int		arg_item_len;
	char *		string_pos;
	char *		zero = (char *)0;

#define	STACK_SIZE	(64*1024)

	/*
	 * Calculate the size of the argument list.
	 */
	va_start(argv_ptr, entry);
	arg_len = 0;
	arg_count = 0;
	for (;;) {
	    arg_ptr = va_arg(argv_ptr, char *);
	    if (arg_ptr == 0)
		break;
	    arg_count++;
	    arg_len += strlen(arg_ptr) + 1;
	}
	va_end(argv_ptr);

	/*
	 * Add space for:
	 *	arg count
	 *	pointers to arguments
	 *	trailing 0 pointer
	 *	dummy 0 pointer to environment variables
	 *	and align to integer boundary
	 */
	arg_len += sizeof(int)
		 + (2 + arg_count) * sizeof(char *);
	arg_len = (arg_len + sizeof(int) - 1) & ~(sizeof(int)-1);

	/*
	 * Allocate the stack.
	 */
	stack_size = round_page(STACK_SIZE);
	stack_base = user_stack_low(stack_size);
	(void) vm_allocate(current_task()->map,
			&stack_base,
			stack_size,
			FALSE);

	arg_pos = (char *)
		set_user_regs(stack_base, stack_size, entry, arg_len);

	/*
	 * Start the strings after the arg-count and pointers
	 */
	string_pos = arg_pos
		+ sizeof(int)
		+ arg_count * sizeof(char *)
		+ 2 * sizeof(char *);

	/*
	 * first the argument count
	 */
	(void) copyout((const char *) &arg_count, (char *) arg_pos,
			sizeof(int));
	arg_pos += sizeof(int);

	/*
	 * Then the strings and string pointers for each argument
	 */
	va_start(argv_ptr, entry);
	while (--arg_count >= 0) {
	    arg_ptr = va_arg(argv_ptr, char *);
	    arg_item_len = strlen(arg_ptr) + 1; /* include trailing 0 */

	    /* set string pointer */
	    (void) copyout((const char *) &string_pos, (char * ) arg_pos,
			sizeof (char *));
	    arg_pos += sizeof(char *);

	    /* copy string */
	    (void) copyout((const char *) arg_ptr, (char *) string_pos, 
			   arg_item_len);
	    string_pos += arg_item_len;
	}
	va_end(argv_ptr);

	/*
	 * last, the trailing 0 argument and a null environment pointer.
	 */
	(void) copyout((const char *) &zero, (char *) arg_pos, sizeof(char *));
	arg_pos += sizeof(char *);
	(void) copyout((const char *) &zero, (char *) arg_pos, sizeof(char *));
}

mach_port_t
task_insert_send_right(
	task_t		task,
	ipc_port_t	port)
{
	mach_port_t	name;

	for (name = 1; ; ++name) {
		kern_return_t kr;

		kr = mach_port_insert_right(task->itk_space, name,
					    port, MACH_MSG_TYPE_PORT_SEND);
		if (kr == KERN_SUCCESS)
			break;
		assert(kr == KERN_NAME_EXISTS);
	}

	return name;
}

kern_return_t
do_bootstrap_privileged_ports(
	ipc_port_t	bootstrap,
	ipc_port_t	*priv_hostp,
	ipc_port_t	*priv_devicep)
{
    *priv_hostp = ipc_port_copy_send(realhost.host_priv_self);
    *priv_devicep = ipc_port_copy_send(master_device_port);
    return KERN_SUCCESS;
}

void
bootstrap_create(void)
{
    ipc_port_t		root_device_port;
    kern_return_t	kr;
    thread_act_t	bootstrap_thread;

#if	NORMA_IPC
    if (no_bootstrap_task())
		return;
#endif	/* NORMA_IPC */

    /*
     * Allocate the master bootstrap port;
     */
    master_bootstrap_port = ipc_port_alloc_kernel();
    if (master_bootstrap_port == IP_NULL)
	panic("can't allocate master bootstrap port");

    /*
     * Create the bootstrap task.
     */
    if (boot_size == 0) {
	printf("Not starting bootstrap task.\n");
	return;
    }

    kr = task_create_local(TASK_NULL, FALSE, FALSE, &bootstrap_task);
    assert( kr == KERN_SUCCESS );
	kr = thread_create(bootstrap_task, &bootstrap_thread);
    assert( kr == KERN_SUCCESS );


    kr = task_set_special_port(bootstrap_task, TASK_BOOTSTRAP_PORT,
				 ipc_port_make_send(master_bootstrap_port));
    assert( kr == KERN_SUCCESS );

    /*
     * Insert send rights to the master host and device ports.
     */
    bootstrap_master_host_port =
	task_insert_send_right(bootstrap_task,
			       ipc_port_make_send(realhost.host_priv_self));

    bootstrap_master_device_port =
	task_insert_send_right(bootstrap_task,
			       ipc_port_make_send(master_device_port));

#if	NORMA_IPC
    /*
     * Insert send right to the potential non local root master device port.
     */

    if ((root_device_port = get_root_master_device_port()) != IP_NULL)
    	root_master_device_port = task_insert_send_right(bootstrap_task,
							 root_device_port);
    else
#endif	/* NORMA_IPC */
	root_master_device_port = bootstrap_master_device_port;

    /*
     * Start the bootstrap thread.
     */

    thread_start(bootstrap_thread->thread, user_bootstrap);
    kr = thread_resume(bootstrap_thread);
    assert( kr == KERN_SUCCESS );
}

void
user_bootstrap(void)
{
	int	entry[2];

	char	host_string[12];
	char	device_string[12];
	char	flag_string[12];
	char	root_master_device_string[12];

	/*
	 * Copy the bootstrap code from boot_data to the user task.
	 */
	copy_bootstrap(entry);

	/*
	 * Convert the host and device ports to strings,
	 * to put in the argument list.
	 */
	(void) itoa(bootstrap_master_host_port, host_string);
	(void) itoa(bootstrap_master_device_port, device_string);
	(void) itoa(root_master_device_port, root_master_device_string);

	/*
	 * Get the boot flags, also
	 */
	get_boot_flags(flag_string);

	/*
	 * Build the argument list and insert in the user task.
	 * Argument list is
	 * "bootstrap -<boothowto> <host_port> <device_port> <root_name>"
	 */
	build_args_and_stack(entry,
			"bootstrap",
			flag_string,
			host_string,
			device_string,
			root_name,
			boot_string,     
			root_master_device_string,
			(char *)0);

	/*
	 * Exit to user thread.
	 */
	thread_bootstrap_return();
	/*NOTREACHED*/
}

/*
 * Parse the boot flags into an argument string.
 * Format as a standard flag argument: '-asd...'
 */

static void
get_boot_flags(
	char	str[])	/* OUT */
{
	register char *cp;
	register int	bflag;
#ifdef	mips
	extern char *machine_get_boot_flags();
	cp = machine_get_boot_flags(str);
#else	/* mips */
	cp = str;
	*cp++ = '-';
#endif	/* mips */

	bflag = boothowto;

	if (bflag & RB_ASKNAME)
	    *cp++ = 'q';
	if (bflag & RB_SINGLE)
	    *cp++ = 's';
#if	MACH_KDB
	if (bflag & RB_KDB)
	    *cp++ = 'd';
#endif	/* MACH_KDB */
	if (bflag & RB_HALT)
	    *cp++ = 'h';
	if (bflag & RB_INITNAME)
	    *cp++ = 'n';
	if (bflag & RB_ALTBOOT)
	    *cp++ = 'v';

	if (cp == &str[1])	/* no flags */
	    *cp++ = 'x';
	*cp = '\0';
}
