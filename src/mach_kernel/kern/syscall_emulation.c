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
 * Revision 2.13.4.3  92/03/28  10:10:25  jeffreyh
 * 	Use actual vector size instead of allocated memory size for
 * 	vm_map_copyin in task_get_emulation_vector.  This was breaking
 * 	remote task creation.
 * 	[92/03/24            dlb]
 * 	Pick up changes from MK71
 * 	[92/03/20  13:18:25  jeffreyh]
 * 
 * Revision 2.16  92/03/03  12:28:57  rpd
 * 	Added syscall_emulation_sync.
 * 	[92/03/03            rpd]
 * 
 * Revision 2.15  92/02/23  19:50:06  elf
 * 	Eliminate keep_wired argument from vm_map_copyin()
 * 	[92/02/23            danner]
 * 
 * Revision 2.14  92/01/03  20:15:18  dbg
 * 	Change new versions of calls to pass dispatch table out-of-line.
 * 	Old calls (xxx_*) use inline data.
 * 	[92/01/03            dbg]
 * 
 * 	Don't round up allocated vector size.
 * 	[92/01/02            dbg]
 * 
 * 	Remove fixed lower and upper bounds on emulated system call
 * 	table.  Now everyone can get as much as the MiG interface will
 * 	allow.
 * 	[91/12/18            dbg]
 * 
 * Revision 2.13.4.2  92/03/04  16:07:23  jeffreyh
 * 	Use vector_size instead of size_used in vm_map_copyin.
 * 	[92/03/04  14:54:49  jsb]
 * 
 * Revision 2.13.4.1  92/03/03  16:20:33  jeffreyh
 * 	Eliminate keep_wired argument from vm_map_copyin().
 * 	[92/02/26            jeffreyh]
 * 	Changes from TRUNK
 * 	[92/02/26  11:56:15  jeffreyh]
 * 
 * Revision 2.13  91/12/13  13:43:31  jsb
 * 	Increased eml_max_emulate_count for OSF/1+ server support.
 * 
 * Revision 2.12  91/11/15  14:08:35  rpd
 * 	Rewrote task_set_emulation_vector for greater clarity.
 * 	Fixed bcopy bug in task_get_emulation_vector.
 * 	Simplified and removed debugging printf from task_set_emulation.
 * 	[91/09/24  14:15:28  jsb]
 * 
 * Revision 2.11  91/10/09  16:11:47  af
 * 	Everyone gets as many syscalls as mips, needed
 * 	on e.g. vax and sun for AFS support.
 * 	[91/10/07            af]
 * 
 * Revision 2.10  91/08/24  12:00:20  af
 * 	Cast tags for bcopy
 * 	[91/08/14            rvb]
 * 
 * Revision 2.9  91/06/25  10:29:13  rpd
 * 	Fixed the includes.
 * 	[91/06/24            rpd]
 * 
 * Revision 2.8  91/06/06  17:07:33  jsb
 * 	Added task_get_emulation_vector, task_set_emulation_vector.
 * 	Task_set_emulation is now a call to task_set_emulation_vector.
 * 	[91/05/24  18:30:16  jsb]
 * 
 * Revision 2.7  91/05/18  14:33:39  rpd
 * 	Fixed eml_task_deallocate to always unlock.
 * 	[91/05/02            rpd]
 * 
 * Revision 2.6  91/05/14  16:47:09  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/02/05  17:29:26  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:17:54  mrt]
 * 
 * Revision 2.3  90/12/05  20:42:15  af
 * 	I beg your pardon, Ultrix uses up to syscall #257 inclusive.
 * 	[90/12/03  22:57:35  af]
 * 
 * Revision 2.2  89/11/29  14:09:18  af
 * 	For mips, grow the max syscall limit, as Ultrix uses up to 256.
 * 	Rcs-ed.
 * 	[89/11/16            af]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and ditribute this software and its
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

#include <mach/error.h>
#include <mach/vm_param.h>
#include <mach/boolean.h>
#include <kern/misc_protos.h>
#include <kern/syscall_emulation.h>
#include <kern/task.h>
#include <kern/kalloc.h>
#include <vm/vm_kern.h>
#include <machine/thread.h>	/* for syscall_emulation_sync */

/*
 * Exported interface
 */
#include <mach/mach_server.h>

/*
 * WARNING:
 * This code knows that kalloc() allocates memory most efficiently
 * in sizes that are powers of 2, and asks for those sizes.
 */

/*
 * Go from number of entries to size of struct eml_dispatch and back.
 */
#define	base_size	(sizeof(struct eml_dispatch) - sizeof(eml_routine_t))
#define	count_to_size(count) \
	(base_size + sizeof(vm_offset_t) * (count))

#define	size_to_count(size) \
	( ((size) - base_size) / sizeof(vm_offset_t) )

/* Forwards */
kern_return_t
task_set_emulation_vector_internal(
	task_t 			task,
	int			vector_start,
	emulation_vector_t	emulation_vector,
	unsigned int		emulation_vector_count);

/*
 *  eml_init:	initialize user space emulation code
 */
void
eml_init(void)
{
}

/*
 * eml_task_reference() [Exported]
 *
 *	Bumps the reference count on the common emulation
 *	vector.
 */

void
eml_task_reference(
	task_t	task,
	task_t	parent)
{
	register eml_dispatch_t	eml;

	if (parent == TASK_NULL)
	    eml = EML_DISPATCH_NULL;
	else
	    eml = parent->eml_dispatch;

	if (eml != EML_DISPATCH_NULL) {
	    mutex_lock(&eml->lock);
	    eml->ref_count++;
	    mutex_unlock(&eml->lock);
	}
	task->eml_dispatch = eml;
}


/*
 * eml_task_deallocate() [Exported]
 *
 *	Cleans up after the emulation code when a process exits.
 */
 
void
eml_task_deallocate(
	task_t task)
{
	register eml_dispatch_t	eml;

	eml = task->eml_dispatch;
	if (eml != EML_DISPATCH_NULL) {
	    int count;

	    mutex_lock(&eml->lock);
	    count = --eml->ref_count;
	    mutex_unlock(&eml->lock);

	    if (count == 0)
		kfree((vm_offset_t)eml, count_to_size(eml->disp_count));

	    task->eml_dispatch = EML_DISPATCH_NULL;
	}
}

/*
 *   task_set_emulation_vector:  [Server Entry]
 *   set a list of emulated system calls for this task.
 */
kern_return_t
task_set_emulation_vector_internal(
	task_t 			task,
	int			vector_start,
	emulation_vector_t	emulation_vector,
	unsigned int		emulation_vector_count)
{
	eml_dispatch_t	cur_eml, new_eml, old_eml;
	vm_size_t	new_size;
	int		cur_start, cur_end;
	int		new_start, new_end;
	int		vector_end;

	if (task == TASK_NULL)
		return EML_BAD_TASK;

	vector_end = vector_start + emulation_vector_count;

	/*
	 * We try to re-use the existing emulation vetor
	 * if possible.  We can reuse the vector if it
	 * is not shared with another task and if it is
	 * large enough to contain the entries we are
	 * supplying.
	 *
	 * We must grab the lock on the task to check whether
	 * there is an emulation vector.
	 * If the vector is shared or not large enough, we
	 * need to drop the lock and allocate a new emulation
	 * vector.
	 *
	 * While the lock is dropped, the emulation vector
	 * may be released by all other tasks (giving us
	 * exclusive use), or may be enlarged by another
	 * task_set_emulation_vector call.  Therefore,
	 * after allocating the new emulation vector, we
	 * must grab the lock again to check whether we
	 * really need the new vector we just allocated.
	 *
	 * Since an emulation vector cannot be altered
	 * if it is in use by more than one task, the
	 * task lock is sufficient to protect the vector`s
	 * start, count, and contents.  The lock in the
	 * vector protects only the reference count.
	 */

	old_eml = EML_DISPATCH_NULL;	/* vector to discard */
	new_eml = EML_DISPATCH_NULL;	/* new vector */

	for (;;) {
	    /*
	     * Find the current emulation vector.
	     * See whether we can overwrite it.
	     */
	    task_lock(task);
	    cur_eml = task->eml_dispatch;
	    if (cur_eml != EML_DISPATCH_NULL) {
		cur_start = cur_eml->disp_min;
		cur_end   = cur_eml->disp_count + cur_start;

		mutex_lock(&cur_eml->lock);
		if (cur_eml->ref_count == 1 &&
		    cur_start <= vector_start &&
		    cur_end >= vector_end)
		{
		    /*
		     * Can use the existing emulation vector.
		     * Discard any new one we allocated.
		     */
		    mutex_unlock(&cur_eml->lock);
		    old_eml = new_eml;
		    break;
		}

		if (new_eml != EML_DISPATCH_NULL &&
		    new_start <= cur_start &&
		    new_end >= cur_end)
		{
		    /*
		     * A new vector was allocated, and it is large enough
		     * to hold all the entries from the current vector.
		     * Copy the entries to the new emulation vector,
		     * deallocate the current one, and use the new one.
		     */
		    
		    bcopy((char *)&cur_eml->disp_vector[0],
			  (char *)&new_eml->disp_vector[cur_start-new_start],
			  cur_eml->disp_count * sizeof(vm_offset_t));


		    if (--cur_eml->ref_count == 0)
			old_eml = cur_eml;	/* discard old vector */
		    mutex_unlock(&cur_eml->lock);

		    task->eml_dispatch = new_eml;
		    syscall_emulation_sync(task);
		    cur_eml = new_eml;
		    break;
		}
		mutex_unlock(&cur_eml->lock);

		/*
		 * Need a new emulation vector.
		 * Ensure it will hold all the entries from
		 * both the old and new emulation vectors.
		 */
		new_start = vector_start;
		if (new_start > cur_start)
		    new_start = cur_start;
		new_end = vector_end;
		if (new_end < cur_end)
		    new_end = cur_end;
	    }
	    else {
		/*
		 * There is no curren emulation vector.
		 * If a new one was allocated, use it.
		 */
		if (new_eml != EML_DISPATCH_NULL) {
		    task->eml_dispatch = new_eml;
		    cur_eml = new_eml;
		    break;
		}

		/*
		 * Compute the size needed for the new vector.
		 */
		new_start = vector_start;
		new_end = vector_end;
	    }

	    /*
	     * Have no vector (or one that is no longer large enough).
	     * Drop all the locks and allocate a new vector.
	     * Repeat the loop to check whether the old vector was
	     * changed while we didn`t hold the locks.
	     */

	    task_unlock(task);

	    if (new_eml != EML_DISPATCH_NULL)
		kfree((vm_offset_t)new_eml, count_to_size(new_eml->disp_count));

	    new_size = count_to_size(new_end - new_start);
	    new_eml = (eml_dispatch_t) kalloc(new_size);

	    bzero((char *)new_eml, new_size);
	    mutex_init(&new_eml->lock);
	    new_eml->ref_count = 1;
	    new_eml->disp_min   = new_start;
	    new_eml->disp_count = new_end - new_start;

	    continue;
	}

	/*
	 * We have the emulation vector.
	 * Install the new emulation entries.
	 */
	bcopy((char *)&emulation_vector[0],
	      (char *)&cur_eml->disp_vector[vector_start - cur_eml->disp_min],
	      emulation_vector_count * sizeof(vm_offset_t));

	task_unlock(task);

	/*
	 * Discard any old emulation vector we don`t need.
	 */
	if (old_eml)
	    kfree((vm_offset_t) old_eml, count_to_size(old_eml->disp_count));

	return KERN_SUCCESS;
}

/*
 *	task_set_emulation_vector:  [Server Entry]
 *
 *	Set the list of emulated system calls for this task.
 *	The list is out-of-line.
 */
kern_return_t
task_set_emulation_vector(
	task_t 			task,
	int			vector_start,
	emulation_vector_t	emulation_vector,
	mach_msg_type_number_t	emulation_vector_count)
{
	kern_return_t		kr;
	vm_offset_t		emul_vector_addr;

	if (task == TASK_NULL)
	    return EML_BAD_TASK;	/* XXX sb KERN_INVALID_ARGUMENT */

	/*
	 * XXX - barbou@gr.osf.org.
	 * If emulation_vector_count is NULL, discard the emulation
	 * vectors.
	 * We need a way to do that for emulator-less servers started
	 * from a classic server. There seems to be no way to get rid
	 * of or to avoid inheriting the emulation vector !?
	 */
	if (emulation_vector_count == 0) {
		eml_task_deallocate(task);
		return KERN_SUCCESS;
	}


	/*
	 *	The emulation vector is really a vm_map_copy_t.
	 */
	kr = vm_map_copyout(ipc_kernel_map, &emul_vector_addr,
			(vm_map_copy_t) emulation_vector);
	if (kr != KERN_SUCCESS)
	    return kr;

	/*
	 *	Can't fault while we hold locks.
	 */
	kr = vm_map_wire(ipc_kernel_map,
			     trunc_page(emul_vector_addr),
			     round_page(emul_vector_addr +
					emulation_vector_count *
					sizeof(eml_dispatch_t)),
			     VM_PROT_READ|VM_PROT_WRITE, FALSE);
	assert(kr == KERN_SUCCESS);

	/*
	 *	Do the work.
	 */
	kr = task_set_emulation_vector_internal(
			task,
			vector_start,
			(emulation_vector_t) emul_vector_addr,
			emulation_vector_count);
	assert(kr == KERN_SUCCESS);

	/*
	 *	Discard the memory
	 */
	(void) kmem_free(ipc_kernel_map,
			 emul_vector_addr,
			 emulation_vector_count * sizeof(eml_dispatch_t));

	return KERN_SUCCESS;
}

/*
 *	task_get_emulation_vector: [Server Entry]
 *
 *	Get the list of emulated system calls for this task.
 *	List is returned out-of-line.
 */
kern_return_t
task_get_emulation_vector(
	task_t			task,
	int			*vector_start,			/* out */
	emulation_vector_t	*emulation_vector,		/* out */
	mach_msg_type_number_t	*emulation_vector_count)	/* out */
{
	eml_dispatch_t		eml;
	vm_size_t		vector_size, size;
	vm_offset_t		addr;

	if (task == TASK_NULL)
	    return EML_BAD_TASK;

	addr = 0;
	size = 0;

	for(;;) {
	    vm_size_t	size_needed;

	    task_lock(task);
	    eml = task->eml_dispatch;
	    if (eml == EML_DISPATCH_NULL) {
		task_unlock(task);
		if (addr)
		    (void) kmem_free(ipc_kernel_map, addr, size);
		*vector_start = 0;
		*emulation_vector = 0;
		*emulation_vector_count = 0;
		return KERN_SUCCESS;
	    }

	    /*
	     * Do we have the memory we need?
	     */
	    vector_size = eml->disp_count * sizeof(vm_offset_t);

	    size_needed = round_page(vector_size);
	    if (size_needed <= size)
		break;

	    /*
	     * If not, unlock the task and allocate more memory.
	     */
	    task_unlock(task);

	    if (size != 0)
		kmem_free(ipc_kernel_map, addr, size);

	    size = size_needed;
	    if (kmem_alloc(ipc_kernel_map, &addr, size) != KERN_SUCCESS)
		return KERN_RESOURCE_SHORTAGE;
	}

	/*
	 * Copy out the dispatch addresses
	 */
	*vector_start = eml->disp_min;
	*emulation_vector_count = eml->disp_count;
	bcopy((char *)eml->disp_vector,
	      (char *)addr,
	      vector_size);

	/*
	 * Unlock the task and free any memory we did not need
	 */
	task_unlock(task);
    {
	vm_size_t	size_used, size_left;
	vm_map_copy_t	memory;

	/*
	 * Free any unused memory beyond the end of the last page used
	 */
	size_used = round_page(vector_size);
	if (size_used != size)
	    (void) kmem_free(ipc_kernel_map,
			     addr + size_used,
			     size - size_used);

	/*
	 * Zero the remainder of the page being returned.
	 */
	size_left = size_used - vector_size;
	if (size_left > 0)
	    bzero((char *)addr + vector_size, size_left);

	/*
	 * Unwire and make memory into copyin form.
	 */
	(void) vm_map_unwire(ipc_kernel_map, addr, addr + size_used, FALSE);
	(void) vm_map_copyin(ipc_kernel_map, addr, vector_size,
			     TRUE, &memory);

	*emulation_vector = (emulation_vector_t) memory;
    }

	return KERN_SUCCESS;
}

/*
 *   task_set_emulation:  [Server Entry]
 *   set up for user space emulation of syscalls within this task.
 */
kern_return_t
task_set_emulation(
	task_t		task,
	vm_offset_t 	routine_entry_pt,
	int		routine_number)
{
	return task_set_emulation_vector_internal(task, routine_number,
					 &routine_entry_pt, 1);
}




