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
/*
 *	File:	kern/sync_sema.c
 *	Author:	Joseph CaraDonna
 *
 *	Contains RT distributed semaphore synchronization services.
 */
/*
 *	Semaphores: Rule of Thumb
 *
 *  	A non-negative semaphore count means that the blocked threads queue is
 *  	empty.  A semaphore count of negative n means that the blocked threads
 *  	queue contains n blocked threads.
 */

#include <kern/misc_protos.h>
#include <kern/sync_sema.h>
#include <kern/sync_policy.h>
#include <kern/spl.h>
#include <kern/ipc_kobject.h>
#include <kern/ipc_sync.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_space.h>

unsigned int	active_semaphores = 0;


/*
 *	Routine:	semaphore_create
 *
 *	Creates a semaphore.
 *	The port representing the semaphore is returned as a parameter.
 */

kern_return_t
semaphore_create (
	task_t		task,
	semaphore_t	*new_semaphore,
	int		policy,
	int		value)
{
	semaphore_t s  = SEMAPHORE_NULL;
	*new_semaphore = SEMAPHORE_NULL;


	if (task == TASK_NULL || value < 0 || policy > SYNC_POLICY_MAX)
		return KERN_INVALID_ARGUMENT;

	s = (semaphore_t) kalloc (sizeof(struct semaphore));

	if (s == SEMAPHORE_NULL)
		return KERN_RESOURCE_SHORTAGE; 

	semaphore_lock_init(s);
	queue_init(&s->blocked_threads);
	s->count  = value;
	s->policy = policy;

	/*
	 *  Create and initialize the semaphore port
	 */

	s->port	= ipc_port_alloc_kernel();
	if (s->port == IP_NULL) {	
		kfree ((vm_offset_t) s, sizeof(struct semaphore));
		return KERN_RESOURCE_SHORTAGE; 
	}

	ipc_kobject_set (s->port,
			(ipc_kobject_t) s,
			IKOT_SEMAPHORE);

	/*
	 *  Associate the new semaphore with the task by adding
	 *  the new semaphore to the task's semaphore list.
	 *
	 *  Associate the task with the new semaphore by having the
	 *  semaphores task pointer point to the owning task's structure.
	 */

	task_lock(task);
	enqueue_head(&task->semaphore_list, (queue_entry_t) s);
	task->semaphores_owned++;
	task_unlock(task);
	s->owner = task;

	/*
	 *  Activate semaphore
	 */

	s->active = TRUE;
	active_semaphores++;

	*new_semaphore = s;

	return KERN_SUCCESS;
}		  

/*
 *	Routine:	semaphore_destroy
 *
 *	Destroys a semaphore.  This call will only succeed if the
 *	specified task is the SAME task name specified at the semaphore's
 *	creation.
 *
 *	All threads currently blocked on the semaphore are awoken.  These
 *	threads will return with the KERN_TERMINATED error.
 */

kern_return_t
semaphore_destroy (task_t task, semaphore_t semaphore)
{
	thread_t	thread;
	spl_t		spl_level;
	ipc_port_t	sport;


	/*
	 *  INSERT: access check
	 *
	 *  If this request is comming from a remote node
	 *  return with failure.  
	 *
	 *  Semaphores are always created and destroyed locally.
	 */

	if (task == TASK_NULL || semaphore == SEMAPHORE_NULL)
		return KERN_INVALID_ARGUMENT;

	if (semaphore->owner != task)
		return KERN_INVALID_RIGHT;

	
	spl_level = splsched();
	semaphore_lock(semaphore);

	if (!semaphore->active) {
		semaphore_unlock(semaphore);
		splx(spl_level);
		return KERN_TERMINATED;
	}

	/*
	 *  Deactivate semaphore
	 */

	sport = semaphore->port;
	ipc_kobject_set(sport, IKO_NULL, IKOT_NONE);
	semaphore->port = IP_NULL;
	semaphore->active = FALSE;

	/* Don't need semaphore lock anymore, since it's deactivated */
	semaphore_unlock(semaphore);

	/*
	 *  Wakeup blocked threads  
	 *  Blocked threads will receive a KERN_TERMINATED error
	 */

	if (semaphore->count < 0)
		while (semaphore->count++ < 0) {
			thread = sync_policy_dequeue(semaphore->policy,
						   &semaphore->blocked_threads);

			if (thread == THREAD_NULL)
				panic("semaphore_destroy: null thread dq");

			SEMAPHORE_OPERATION_COMPLETE(thread);
			clear_wait(thread, KERN_SEMAPHORE_DESTROYED, TRUE);
		}

	/*
	 *  Disown semaphore
	 */

	task_lock(task);
	remqueue(&task->semaphore_list, (queue_entry_t) semaphore);
	task->semaphores_owned--;
	task_unlock(task);

	splx(spl_level);

	/*
	 *  Destroy semaphore
	 */

	ipc_port_dealloc_kernel(sport);
	kfree((vm_offset_t) semaphore, sizeof(struct semaphore));

	active_semaphores--;

	return KERN_SUCCESS;
}

/*
 *	Routine:	semaphore_signal
 *
 *	Increments the semaphore count by one.  If any threads are blocked
 *	on the semaphore ONE is woken up.
 */

kern_return_t
semaphore_signal (semaphore_t  semaphore)
{
	thread_t	thread;
	spl_t		spl_level;


	if (semaphore == SEMAPHORE_NULL)
		return KERN_INVALID_ARGUMENT;

	spl_level = splsched();
	semaphore_lock(semaphore);

	if (!semaphore->active) {
		semaphore_unlock(semaphore);
		splx(spl_level);
		return KERN_TERMINATED;
	}

	if (++semaphore->count <= 0) {
		thread = sync_policy_dequeue(semaphore->policy,
					     &semaphore->blocked_threads);

		if (thread == THREAD_NULL)
			panic("semaphore_signal: null thread dq");

		SEMAPHORE_OPERATION_COMPLETE(thread);
		clear_wait(thread, KERN_SUCCESS, TRUE);
	}

	semaphore_unlock(semaphore);
	splx(spl_level);

	return KERN_SUCCESS;
}

/*
 *	Routine:	semaphore_signal_all
 *
 *	Awakens ALL threads currently blocked on the semaphore.
 *	The semaphore count returns to zero.
 */
 
kern_return_t
semaphore_signal_all (semaphore_t semaphore)
{
	thread_t	thread;
	spl_t		spl_level;


	if (semaphore == SEMAPHORE_NULL)
		return KERN_INVALID_ARGUMENT;

	spl_level = splsched();
	semaphore_lock (semaphore);

	if (!semaphore->active) {
		semaphore_unlock(semaphore);
		splx(spl_level);
		return KERN_TERMINATED;
	}

	if (semaphore->count < 0)
		while (semaphore->count++ < 0) {
			thread = sync_policy_dequeue(semaphore->policy,
						   &semaphore->blocked_threads);

			if (thread == THREAD_NULL)
				panic("semaphore_signal_all: null thread dq\n");
		
			SEMAPHORE_OPERATION_COMPLETE(thread);
			clear_wait(thread, KERN_SUCCESS, TRUE);
		}

	semaphore_unlock(semaphore);
	splx(spl_level);

	return KERN_SUCCESS;
}

/*
 *	Routine:	semaphore_wait
 *
 *	Decrements the semaphore count by one.  If the count is negative
 *	after the decrement, the calling thread blocks.
 *
 *	Assumes: Never called from interrupt context.
 */
	
kern_return_t
semaphore_wait (semaphore_t semaphore)
{
	thread_t	thread;
	spl_t		spl_level;


	if (semaphore == SEMAPHORE_NULL)
		return KERN_INVALID_ARGUMENT;

	spl_level = splsched();
	semaphore_lock(semaphore);

	if (!semaphore->active) {
		semaphore_unlock(semaphore);
		splx(spl_level);
		return KERN_TERMINATED;
	}

	if (--semaphore->count < 0) {

		thread = current_thread();

		sync_policy_enqueue(semaphore->policy,
				    &semaphore->blocked_threads,
				    thread);

		assert_wait(0, TRUE);
		semaphore_unlock(semaphore);
		splx(spl_level);

		thread_block((void (*)(void)) 0);

		if (SEMAPHORE_OPERATION_ABORTED(thread)) {
			spl_level = splsched();
			semaphore_lock(semaphore);
			sync_policy_remqueue(semaphore->policy,
					     &semaphore->blocked_threads,
					     thread);
			semaphore_unlock(semaphore);
			splx(spl_level);
		}

		return(thread->wait_result);
	}

	semaphore_unlock(semaphore);
	splx(spl_level);

	return KERN_SUCCESS;
}
