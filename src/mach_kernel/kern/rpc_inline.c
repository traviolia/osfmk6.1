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
 *	Custom Inline Functions for RPC
 *
 *	Inline version of functions used by RPC for performance.
 */


#if defined(__GNUC__)


#include <cpus.h>
#include <mach_debug.h>
#include <mach_ldebug.h>

#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/thread_status.h>
#include <mach/vm_param.h>
#include <mach/port.h>
#include <mach/message.h>
#include <mach/alert.h>

#include <kern/assert.h>
#include <kern/misc_protos.h>
#include <kern/counters.h>
#include <kern/ipc_subsystem.h>
#include <kern/ipc_mig.h>
#include <kern/ipc_tt.h>
#include <kern/mach_param.h>
#include <kern/machine.h>
#include <kern/spl.h>
#include <kern/processor.h>
#include <kern/queue.h>
#include <kern/sched.h>
#include <kern/sched_prim.h>
#include <kern/task.h>
#include <kern/thread_act.h>
#include <kern/thread_pool.h>
#include <kern/thread.h>
#include <kern/zalloc.h>
#include <kern/misc_protos.h>
#include <kern/sched_prim.h>
#include <kern/syscall_subr.h>
#include <kern/sync_lock.h>
#include <kern/profile.h>

#include <ipc/port.h>
#include <ipc/ipc_entry.h>
#include <ipc/ipc_space.h>
#include <ipc/ipc_object.h>
#include <ipc/ipc_hash.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_pset.h>
#include <ipc/ipc_right.h>
#include <ipc/ipc_notify.h>
#include <ipc/ipc_table.h>
#include <ipc/mach_msg.h>

#include <vm/vm_kern.h>
#include <vm/pmap.h>

#include <device/device_typedefs.h>

#include <i386/thread.h>			/* XXX machine dependent */
#include <i386/eflags.h>			/* use i386/rpc_inline.c */
#include <i386/proc_reg.h>
#include <i386/seg.h>
#include <i386/tss.h>
#include <i386/user_ldt.h>
#include <i386/fpu.h>
#include <i386/iopb_entries.h>


/*
 *	Change inlined function names so they will be unique.
 */

#define act_attach              act_attach_inline
#define act_detach              act_detach_inline
#define ipc_right_copyin	ipc_right_copyin_inline
#define ipc_right_copyout	ipc_right_copyout_inline
#define ipc_object_copyin       ipc_object_copyin_inline
#define ipc_object_copyout      ipc_object_copyout_inline
#define thread_pool_get_act     thread_pool_get_act_inline
#define act_machine_switch_pcb	act_machine_switch_pcb_inline


extern void             act_attach(thread_act_t, thread_t, unsigned);
extern void            	act_detach(thread_act_t, boolean_t);
extern void            	act_machine_switch_pcb(thread_act_t);
extern thread_act_t 	thread_pool_get_act( ipc_port_t );
extern kern_return_t ipc_right_copyin(
        ipc_space_t             space,
        mach_port_t             name,
        ipc_entry_t             entry,
        mach_msg_type_name_t    msgt_name,
        boolean_t               deadok,
        ipc_object_t            *objectp,
        ipc_port_t              *sorightp);
extern kern_return_t ipc_right_copyout(
        ipc_space_t             space,
        mach_port_t             name,
        ipc_entry_t             entry,
        mach_msg_type_name_t    msgt_name,
        boolean_t               overflow,
        ipc_object_t            object);
extern kern_return_t ipc_object_copyin(
        ipc_space_t             space,
        mach_port_t             name,
        mach_msg_type_name_t    msgt_name,
        ipc_object_t            *objectp);
extern kern_return_t ipc_object_copyout(
        ipc_space_t             space,
        ipc_object_t            object,
        mach_msg_type_name_t    msgt_name,
        boolean_t               overflow,
        mach_port_t             *namep);




/* from ipc/ipc_right.c */
__inline__
kern_return_t
ipc_right_copyin(
	ipc_space_t		space,
	mach_port_t		name,
	ipc_entry_t		entry,
	mach_msg_type_name_t	msgt_name,
	boolean_t		deadok,
	ipc_object_t		*objectp,
	ipc_port_t		*sorightp)
{
	ipc_entry_bits_t bits = entry->ie_bits;

	assert(space->is_active);

	switch (msgt_name) {
	    case MACH_MSG_TYPE_MAKE_SEND: {
		ipc_port_t port;

		if ((bits & MACH_PORT_TYPE_RECEIVE) == 0)
			goto invalid_right;

		port = (ipc_port_t) entry->ie_object;
		assert(port != IP_NULL);

		ip_lock(port);
		assert(ip_active(port));
		assert(port->ip_receiver_name == name);
		assert(port->ip_receiver == space);

		port->ip_mscount++;
		port->ip_srights++;
		ip_reference(port);
		ip_unlock(port);

		*objectp = (ipc_object_t) port;
		*sorightp = IP_NULL;
		break;
	    }

	    case MACH_MSG_TYPE_MAKE_SEND_ONCE: {
		ipc_port_t port;

		if ((bits & MACH_PORT_TYPE_RECEIVE) == 0)
			goto invalid_right;

		port = (ipc_port_t) entry->ie_object;
		assert(port != IP_NULL);

		ip_lock(port);
		assert(ip_active(port));
		assert(port->ip_receiver_name == name);
		assert(port->ip_receiver == space);

		port->ip_sorights++;
		ip_reference(port);
		ip_unlock(port);

		*objectp = (ipc_object_t) port;
		*sorightp = IP_NULL;
		break;
	    }

	    case MACH_MSG_TYPE_MOVE_RECEIVE: {
		ipc_port_t port;
		ipc_port_t dnrequest = IP_NULL;

		if ((bits & MACH_PORT_TYPE_RECEIVE) == 0)
			goto invalid_right;

		port = (ipc_port_t) entry->ie_object;
		assert(port != IP_NULL);

		ip_lock(port);
		assert(ip_active(port));
		assert(port->ip_receiver_name == name);
		assert(port->ip_receiver == space);

		if (bits & MACH_PORT_TYPE_SEND) {
			assert(IE_BITS_TYPE(bits) ==
					MACH_PORT_TYPE_SEND_RECEIVE);
			assert(IE_BITS_UREFS(bits) > 0);
			assert(port->ip_srights > 0);

			ipc_hash_insert(space, (ipc_object_t) port,
					name, entry);

			ip_reference(port);
		} else {
			assert(IE_BITS_TYPE(bits) == MACH_PORT_TYPE_RECEIVE);
			assert(IE_BITS_UREFS(bits) == 0);

			dnrequest = ipc_right_dncancel_macro(space, port,
							     name, entry);

			entry->ie_object = IO_NULL;
		}
		entry->ie_bits = bits &~ MACH_PORT_TYPE_RECEIVE;

		ipc_port_clear_receiver(port);

		port->ip_receiver_name = MACH_PORT_NULL;
		port->ip_destination = IP_NULL;
		ip_unlock(port);

		*objectp = (ipc_object_t) port;
		*sorightp = dnrequest;
		break;
	    }

	    case MACH_MSG_TYPE_COPY_SEND: {
		ipc_port_t port;

		if (bits & MACH_PORT_TYPE_DEAD_NAME)
			goto copy_dead;

		/* allow for dead send-once rights */

		if ((bits & MACH_PORT_TYPE_SEND_RIGHTS) == 0)
			goto invalid_right;

		assert(IE_BITS_UREFS(bits) > 0);

		port = (ipc_port_t) entry->ie_object;
		assert(port != IP_NULL);

		if (ipc_right_check(space, port, name, entry)) {
			bits = entry->ie_bits;
			goto copy_dead;
		}
		/* port is locked and active */

		if ((bits & MACH_PORT_TYPE_SEND) == 0) {
			assert(IE_BITS_TYPE(bits) == MACH_PORT_TYPE_SEND_ONCE);
			assert(port->ip_sorights > 0);

			ip_unlock(port);
			goto invalid_right;
		}

		assert(port->ip_srights > 0);

		port->ip_srights++;
		ip_reference(port);
		ip_unlock(port);

		*objectp = (ipc_object_t) port;
		*sorightp = IP_NULL;
		break;
	    }

	    case MACH_MSG_TYPE_MOVE_SEND: {
		ipc_port_t port;
		ipc_port_t dnrequest = IP_NULL;

		if (bits & MACH_PORT_TYPE_DEAD_NAME)
			goto move_dead;

		/* allow for dead send-once rights */

		if ((bits & MACH_PORT_TYPE_SEND_RIGHTS) == 0)
			goto invalid_right;

		assert(IE_BITS_UREFS(bits) > 0);

		port = (ipc_port_t) entry->ie_object;
		assert(port != IP_NULL);

		if (ipc_right_check(space, port, name, entry)) {
			bits = entry->ie_bits;
			goto move_dead;
		}
		/* port is locked and active */

		if ((bits & MACH_PORT_TYPE_SEND) == 0) {
			assert(IE_BITS_TYPE(bits) == MACH_PORT_TYPE_SEND_ONCE);
			assert(port->ip_sorights > 0);

			ip_unlock(port);
			goto invalid_right;
		}

		assert(port->ip_srights > 0);

		if (IE_BITS_UREFS(bits) == 1) {
			if (bits & MACH_PORT_TYPE_RECEIVE) {
				assert(port->ip_receiver_name == name);
				assert(port->ip_receiver == space);
				assert(IE_BITS_TYPE(bits) ==
						MACH_PORT_TYPE_SEND_RECEIVE);

				ip_reference(port);
			} else {
				assert(IE_BITS_TYPE(bits) ==
						MACH_PORT_TYPE_SEND);

				dnrequest = ipc_right_dncancel_macro(
						space, port, name, entry);

				ipc_hash_delete(space, (ipc_object_t) port,
						name, entry);

				entry->ie_object = IO_NULL;
			}
			entry->ie_bits = bits &~
				(IE_BITS_UREFS_MASK|MACH_PORT_TYPE_SEND);
		} else {
			port->ip_srights++;
			ip_reference(port);
			entry->ie_bits = bits-1; /* decrement urefs */
		}

		ip_unlock(port);

		*objectp = (ipc_object_t) port;
		*sorightp = dnrequest;
		break;
	    }

	    case MACH_MSG_TYPE_MOVE_SEND_ONCE: {
		ipc_port_t port;
		ipc_port_t dnrequest;

		if (bits & MACH_PORT_TYPE_DEAD_NAME)
			goto move_dead;

		/* allow for dead send rights */

		if ((bits & MACH_PORT_TYPE_SEND_RIGHTS) == 0)
			goto invalid_right;

		assert(IE_BITS_UREFS(bits) > 0);

		port = (ipc_port_t) entry->ie_object;
		assert(port != IP_NULL);

		if (ipc_right_check(space, port, name, entry)) {
			bits = entry->ie_bits;
			goto move_dead;
		}
		/* port is locked and active */

		if ((bits & MACH_PORT_TYPE_SEND_ONCE) == 0) {
			assert(bits & MACH_PORT_TYPE_SEND);
			assert(port->ip_srights > 0);

			ip_unlock(port);
			goto invalid_right;
		}

		assert(IE_BITS_TYPE(bits) == MACH_PORT_TYPE_SEND_ONCE);
		assert(IE_BITS_UREFS(bits) == 1);
		assert(port->ip_sorights > 0);

		dnrequest = ipc_right_dncancel_macro(space, port, name, entry);
		ip_unlock(port);

		entry->ie_object = IO_NULL;
		entry->ie_bits = bits &~ MACH_PORT_TYPE_SEND_ONCE;

		*objectp = (ipc_object_t) port;
		*sorightp = dnrequest;
		break;
	    }

	    default:
		panic("ipc_right_copyin: strange rights");
	}

	return KERN_SUCCESS;

    copy_dead:
	assert(IE_BITS_TYPE(bits) == MACH_PORT_TYPE_DEAD_NAME);
	assert(IE_BITS_UREFS(bits) > 0);
	assert(entry->ie_request == 0);
	assert(entry->ie_object == 0);

	if (!deadok)
		goto invalid_right;

	*objectp = IO_DEAD;
	*sorightp = IP_NULL;
	return KERN_SUCCESS;

    move_dead:
	assert(IE_BITS_TYPE(bits) == MACH_PORT_TYPE_DEAD_NAME);
	assert(IE_BITS_UREFS(bits) > 0);
	assert(entry->ie_request == 0);
	assert(entry->ie_object == 0);

	if (!deadok)
		goto invalid_right;

	if (IE_BITS_UREFS(bits) == 1)
		entry->ie_bits = bits &~ MACH_PORT_TYPE_DEAD_NAME;
	else
		entry->ie_bits = bits-1; /* decrement urefs */

	*objectp = IO_DEAD;
	*sorightp = IP_NULL;
	return KERN_SUCCESS;

    invalid_right:
	return KERN_INVALID_RIGHT;
}


/* from ipc/ipc_right.c */
__inline__
kern_return_t
ipc_right_copyout(
	ipc_space_t		space,
	mach_port_t		name,
	ipc_entry_t		entry,
	mach_msg_type_name_t	msgt_name,
	boolean_t		overflow,
	ipc_object_t		object)
{
	ipc_entry_bits_t bits = entry->ie_bits;
	ipc_port_t port;

	assert(IO_VALID(object));
	assert(io_otype(object) == IOT_PORT);
	assert(io_active(object));
	assert(entry->ie_object == object);

	port = (ipc_port_t) object;

	switch (msgt_name) {
	    case MACH_MSG_TYPE_PORT_SEND_ONCE:
		assert(IE_BITS_TYPE(bits) == MACH_PORT_TYPE_NONE);
		assert(port->ip_sorights > 0);

		/* transfer send-once right and ref to entry */
		ip_unlock(port);

		entry->ie_bits = bits | (MACH_PORT_TYPE_SEND_ONCE | 1);
		break;

	    case MACH_MSG_TYPE_PORT_SEND:
		assert(port->ip_srights > 0);

		if (bits & MACH_PORT_TYPE_SEND) {
			mach_port_urefs_t urefs = IE_BITS_UREFS(bits);

			assert(port->ip_srights > 1);
			assert(urefs > 0);
			assert(urefs < MACH_PORT_UREFS_MAX);

			if (urefs+1 == MACH_PORT_UREFS_MAX) {
				if (overflow) {
					/* leave urefs pegged to maximum */

					port->ip_srights--;
					ip_release(port);
					ip_unlock(port);
					return KERN_SUCCESS;
				}

				ip_unlock(port);
				return KERN_UREFS_OVERFLOW;
			}

			port->ip_srights--;
			ip_release(port);
			ip_unlock(port);
		} else if (bits & MACH_PORT_TYPE_RECEIVE) {
			assert(IE_BITS_TYPE(bits) == MACH_PORT_TYPE_RECEIVE);
			assert(IE_BITS_UREFS(bits) == 0);

			/* transfer send right to entry */
			ip_release(port);
			ip_unlock(port);
		} else {
			assert(IE_BITS_TYPE(bits) == MACH_PORT_TYPE_NONE);
			assert(IE_BITS_UREFS(bits) == 0);

			/* transfer send right and ref to entry */
			ip_unlock(port);

			/* entry is locked holding ref, so can use port */

			ipc_hash_insert(space, (ipc_object_t) port,
					name, entry);
		}

		entry->ie_bits = (bits | MACH_PORT_TYPE_SEND) + 1;
		break;

	    case MACH_MSG_TYPE_PORT_RECEIVE: {
		ipc_port_t dest;

		assert(port->ip_mscount == 0);
		assert(port->ip_receiver_name == MACH_PORT_NULL);
		dest = port->ip_destination;

		port->ip_receiver_name = name;
		port->ip_receiver = space;

		assert((bits & MACH_PORT_TYPE_RECEIVE) == 0);

		if (bits & MACH_PORT_TYPE_SEND) {
			assert(IE_BITS_TYPE(bits) == MACH_PORT_TYPE_SEND);
			assert(IE_BITS_UREFS(bits) > 0);
			assert(port->ip_srights > 0);

			ip_release(port);
			ip_unlock(port);

			/* entry is locked holding ref, so can use port */

			ipc_hash_delete(space, (ipc_object_t) port,
					name, entry);
		} else {
			assert(IE_BITS_TYPE(bits) == MACH_PORT_TYPE_NONE);
			assert(IE_BITS_UREFS(bits) == 0);

			/* transfer ref to entry */
			ip_unlock(port);
		}

		entry->ie_bits = bits | MACH_PORT_TYPE_RECEIVE;

		if (dest != IP_NULL)
			ipc_port_release(dest);
		break;
	    }

	    default:
		panic("ipc_right_copyout: strange rights");
	}

	return KERN_SUCCESS;
}


/* from ipc/ipc_object.c */
__inline__ 
kern_return_t
ipc_object_copyin(
	ipc_space_t		space,
	mach_port_t		name,
	mach_msg_type_name_t	msgt_name,
	ipc_object_t		*objectp)
{
	ipc_entry_t entry;
	ipc_port_t soright;
	kern_return_t kr;

	/*
	 *	Could first try a read lock when doing
	 *	MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND,
	 *	and MACH_MSG_TYPE_MAKE_SEND_ONCE.
	 */

        /*                                                              
         *      Inline expansion of:                                    
         *      ipc_right_lookup_write(space, name, &entry);       
         */                                                             
        assert(space != IS_NULL);                                       
                                                                        
        is_write_lock(space);                                           
                                                                        
        if (!space->is_active) {                                        
                is_write_unlock(space);                                 
                return KERN_INVALID_TASK;                               
        }                                                               
                                                                        
        if ((entry = ipc_entry_lookup(space, name)) == IE_NULL) {  
                is_write_unlock(space);                                 
                return KERN_INVALID_NAME;                               
        }                                                               
        /* end of inline expansion of ipc_right_lookup_write */         
                               
	/* space is write-locked and active */

	kr = ipc_right_copyin(space, name, entry,
			      msgt_name, TRUE,
			      objectp, &soright);
	if (IE_BITS_TYPE(entry->ie_bits) == MACH_PORT_TYPE_NONE)
		ipc_entry_dealloc(space, name, entry);
	is_write_unlock(space);

	if ((kr == KERN_SUCCESS) && (soright != IP_NULL))
		ipc_notify_port_deleted(soright, name);

	return kr;
}


/* from ipc/ipc_object.c */
__inline__
kern_return_t
ipc_object_copyout(
	ipc_space_t		space,
	ipc_object_t		object,
	mach_msg_type_name_t	msgt_name,
	boolean_t		overflow,
	mach_port_t		*namep)
{
	mach_port_t name;
	ipc_entry_t entry;
	kern_return_t kr;

	assert(IO_VALID(object));
	assert(io_otype(object) == IOT_PORT);

	is_write_lock(space);

	for (;;) {
		if (!space->is_active) {
			is_write_unlock(space);
			return KERN_INVALID_TASK;
		}

		if ((msgt_name != MACH_MSG_TYPE_PORT_SEND_ONCE) &&
		    ipc_right_reverse(space, object, &name, &entry)) {
			/* object is locked and active */

			assert(entry->ie_bits & MACH_PORT_TYPE_SEND_RECEIVE);
			break;
		}

		name = (mach_port_t)object;
		kr = ipc_entry_get(space, 
			msgt_name == MACH_MSG_TYPE_PORT_SEND_ONCE,
			&name, &entry);
		if (kr != KERN_SUCCESS) {
			/* unlocks/locks space, so must start again */

			kr = ipc_entry_grow_table(space, ITS_SIZE_NONE);
			if (kr != KERN_SUCCESS)
				return kr; /* space is unlocked */

			continue;
		}

		assert(IE_BITS_TYPE(entry->ie_bits) == MACH_PORT_TYPE_NONE);
		assert(entry->ie_object == IO_NULL);

		io_lock(object);
		if (!io_active(object)) {
			io_unlock(object);
			ipc_entry_dealloc(space, name, entry);
			is_write_unlock(space);
			return KERN_INVALID_CAPABILITY;
		}

		entry->ie_object = object;
		break;
	}

	/* space is write-locked and active, object is locked and active */

	kr = ipc_right_copyout(space, name, entry,
			       msgt_name, overflow, object);
	/* object is unlocked */
	is_write_unlock(space);

	if (kr == KERN_SUCCESS)
		*namep = name;
	return kr;
}


/* from kern/thread_pool.c */
__inline__
thread_act_t
thread_pool_get_act(ipc_port_t pool_port)
{
	thread_pool_t thread_pool = &pool_port->ip_thread_pool;
	thread_act_t thr_act;

#if	MACH_ASSERT
	assert(thread_pool != THREAD_POOL_NULL);
	if (watchacts & WA_ACT_LNK)
		printf("thread_pool_block: %x, waiting=%d\n",
		       thread_pool, thread_pool->waiting);
#endif

	while ((thr_act = thread_pool->thr_acts) == THR_ACT_NULL) {
		if (!ip_active(pool_port))
			return THR_ACT_NULL;
		thread_pool->waiting = 1;
		assert_wait((int)thread_pool, FALSE);
		ip_unlock(pool_port);
		thread_block((void (*)(void)) 0);       /* block self */
		ip_lock(pool_port);
	}
	assert(thr_act->thread == THREAD_NULL);
	assert(thr_act->suspend_count == 0);
	thread_pool->thr_acts = thr_act->thread_pool_next;
	act_lock(thr_act);

#if	MACH_ASSERT
	if (watchacts & WA_ACT_LNK)
		printf("thread_pool_block: return %x, next=%x\n",
		       thr_act, thread_pool->thr_acts);
#endif
	return thr_act;
}


/* from i386/pcb.c */				/* XXX machine dependent */
						/* use i386/rpc_inline.c */

#if     NCPUS > 1
#define curr_gdt(mycpu)         (mp_gdt[mycpu])
#define curr_ktss(mycpu)        (mp_ktss[mycpu])
#else
#define curr_gdt(mycpu)         (gdt)
#define curr_ktss(mycpu)        (&ktss)
#endif

#define gdt_desc_p(mycpu,sel) \
        ((struct real_descriptor *)&curr_gdt(mycpu)[sel_idx(sel)])

__inline__
void
act_machine_switch_pcb( thread_act_t new_act )
{
	register pcb_t		pcb = new_act->mact.pcb;
	register int		mycpu = cpu_number();
#if 0
	register iopb_tss_t	tss = pcb->ims.io_tss;
	vm_offset_t		pcb_stack_top;
#endif

	/*
	 *	Save a pointer to the top of the "kernel" stack -
	 *	actually the place in the PCB where a trap into
	 *	kernel mode will push the registers.
	 *	The location depends on V8086 mode.  If we are
	 *	not in V8086 mode, then a trap into the kernel
	 *	won`t save the v86 segments, so we leave room.
	 */
#if 0
	pcb_stack_top = (pcb->iss.efl & EFL_VM)
			? (int) (&pcb->iss + 1)
			: (int) (&pcb->iss.v86_segs);
	if (tss == 0) {
	    /*
	     *	No per-thread IO permissions.
	     *	Use standard kernel TSS.
	     */
#endif
	    if (!(gdt_desc_p(mycpu,KERNEL_TSS)->access & ACC_TSS_BUSY))
		set_tr(KERNEL_TSS);
	    curr_ktss(mycpu)->esp0 = (pcb->iss.efl & EFL_VM)
				     ? (int) (&pcb->iss + 1)
				     : (int) (&pcb->iss.v86_segs);
#if 0
	}
	else {
	    /*
	     * Set the IO permissions.  Use this thread`s TSS.
	     */
	    *gdt_desc_p(mycpu,USER_TSS)
	    	= *(struct real_descriptor *)tss->iopb_desc;
	    tss->tss.esp0 = pcb_stack_top;
	    set_tr(USER_TSS);
	    gdt_desc_p(mycpu,KERNEL_TSS)->access &= ~ ACC_TSS_BUSY;
	}

    {
	register user_ldt_t	ldt = pcb->ims.ldt;
	/*
	 * Set the thread`s LDT.
	 */
	if (ldt == 0) {
	    /*
	     * Use system LDT.
	     */
#endif
	    set_ldt(KERNEL_LDT);
#if 0
	}
	else {
	    /*
	     * Thread has its own LDT.
	     */
	    *gdt_desc_p(mycpu,USER_LDT) = ldt->desc;
	    set_ldt(USER_LDT);
	}
    }
	/*
	 * Load the floating-point context, if necessary.
	 */
	fpu_load_context(pcb);
#endif
}


/* from kern/thread_act.c */
__inline__
void 
act_attach(
	thread_act_t	thr_act,
	thread_t	thread,
	unsigned	init_alert_mask)
{
	thread_act_t	lower;
	vm_offset_t 	stack;
	int		s;


#if	MACH_ASSERT
	assert(thread == current_thread() || thread->top_act == THR_ACT_NULL);
	if (watchacts & WA_ACT_LNK)
		printf("act_attach(thr_act %x(%d) thread %x(%d) mask %d)\n",
		       thr_act, thr_act->ref_count, thread, thread->ref_count,
		       init_alert_mask);
#endif	/* MACH_ASSERT */

	/* Chain the thr_act onto the thread's thr_act stack.  */
	/* Set mask and auto-propagate alerts from below */
	thr_act->thread = thread;
	thr_act->higher = THR_ACT_NULL;  /*safety*/
	thr_act->alerts = 0;
	thr_act->alert_mask = 0; /* init_alert_mask */
	lower = thr_act->lower = thread->top_act;
	/*
	 * Distinguish here between initial (base) activation and
	 * later ones.  For later ones, we manipulate kernel-cached
	 * state relative to the current kernel stack, and update
	 * the kernel stack stored in the thread.
	 */
#if 0
	if (lower != THR_ACT_NULL) {
		vm_offset_t stack;
#endif
		lower->higher = thr_act;
		thr_act->alerts = 0; /* (lower->alerts & init_alert_mask) */
		stack = thr_act->kernel_stack;
		assert(stack != 0);
		/*
		 * We're changing the current activation, so need to
		 * update the kernel's cached values for it.  Ideally,
		 * the change and update would be atomic, but we can't
		 * arrange that.  WARNING: this code really can't be
		 * single-stepped through (XXX).
		 */
		assert(active_threads[cpu_number()] == thread);
		active_kloaded[cpu_number()] = (thr_act->kernel_loaded)
								? thr_act : 0;
		MACHINE_STACK_STASH(stack);
		ast_context(thr_act, cpu_number());
#if 1
		PMAP_DEACTIVATE_USER(lower, cpu_number());
		PMAP_ACTIVATE_USER(thr_act, cpu_number());
		act_machine_switch_pcb(thr_act);
#endif
#if 0
	}
#endif
	/*
	 * Probably need check only user_stop_count -- no one's going to
	 * thread_hold() an empty thread_act.
	 */
	if (thr_act->user_stop_count || thr_act->suspend_count) {
		s = splsched();
		thread_lock(thread);
		thread->state |= TH_SUSP;
		thread_unlock(thread);
		splx(s);
	}
	thread->top_act = thr_act;
}


/* from kern/thread_act.c */
/* 
 * Same as original except we know the activation is current and 
 * there is a lower activation on the stack. And no asserts.
 */
__inline__
void 
act_detach(
	thread_act_t	cur_act,
	boolean_t	is_current)
{
	thread_t	cur_thread = cur_act->thread;
	thread_act_t	old_act;
	vm_offset_t	old_stack;
	thread_pool_t	cur_pool;
	int 		s;

	cur_pool = &cur_act->pool_port->ip_thread_pool;

	/* Unlink the thr_act from the thread's thr_act stack */
	old_act = cur_thread->top_act = cur_act->lower;
	old_stack = old_act->kernel_stack;
	cur_act->thread = 0;

	assert(old_stack != 0);

	/*
	 * We're changing the current activation, so need to
	 * update the kernel's cached value for it.  Ideally,
	 * the change and update would be atomic, but we can't
	 * arrange that.  WARNING: this code really can't be
	 * single-stepped through (XXX).
	 */
	assert(active_threads[cpu_number()] == cur_thread);
	active_kloaded[cpu_number()] = (old_act->kernel_loaded) ? old_act : 0;
	MACHINE_STACK_STASH(old_stack);
	ast_context(old_act, cpu_number());
	PMAP_DEACTIVATE_USER(cur_act, cpu_number());
	PMAP_ACTIVATE_USER(old_act, cpu_number());
	act_machine_switch_pcb(old_act);

	if (old_act->user_stop_count || old_act->suspend_count) {
       		s = splsched();
               	thread_lock(cur_thread);
               	cur_thread->state |= TH_SUSP;
                thread_unlock(cur_thread);
		splx(s);
	}


	/* Return it to the thread_pool's list, if it is still alive: */
	if (cur_act->active) {
		assert(cur_pool);
		cur_act->thread_pool_next = cur_pool->thr_acts;
		cur_pool->thr_acts = cur_act;
		if (cur_pool->waiting)
			thread_pool_wakeup(cur_pool);
	} else if (cur_pool) {
		/* Remove it from its thread_pool, which will
		 * deallocate it and destroy it.  Lock the pool_port
		 * only if we're not already using it (in RPC, in
		 * which case it will be locked).
		 */
		assert(cur_act->pool_port);
		if (!is_current)
			ip_lock(cur_act->pool_port);
		act_set_thread_pool(cur_act, IP_NULL);
		if (!is_current)
			ip_unlock(cur_act->pool_port);
	}
}


#endif	/* __GNUC__ */
