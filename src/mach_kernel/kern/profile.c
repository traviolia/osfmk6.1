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
 * Revision 2.1.6.2  92/09/15  17:21:43  jeffreyh
 * 	Changes to support kernel task & threads profiling.
 * 	Fixed kernel interface (now uses mig and uses
 * 	a single port). The size of buffers is now
 * 	invisible to users. Detection of last sample is
 * 	possible using no_more_senders notification.
 * 	Some locks are still missing for MP.
 * 	[92/07/17            bernadat]
 * 
 * Revision 2.1.6.1  92/02/18  19:09:45  jeffreyh
 * 	Fixed Profiling code. [emcmanus@gr.osf.org]
 * 
 * Revision 2.1.1.1  91/09/26  04:48:14  bernadat
 * 	Profiling support
 * 	(Bernard Tabib & Andrei Danes @ gr.osf.org)
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

#include	<kern/thread.h>
#include	<kern/queue.h>
#include	<kern/profile.h>
#include	<kern/sched_prim.h>
#include	<kern/spl.h>
#include	<kern/misc_protos.h>
#include	<ipc/ipc_space.h>

#include	<mach/prof.h>
#include	<mach/mach_host_server.h>

extern vm_map_t kernel_map; /* can be discarded, defined in <vm/vm_kern.h> */

thread_t profile_thread_id = THREAD_NULL;
int profile_sample_count = 0;	/* Provided for looking at from kdb. */

/* Forwards */
prof_data_t	pbuf_alloc(void);
void		pbuf_free(
			prof_data_t	pbuf);
void		profile_thread(void);
void		send_last_sample_buf(
			prof_data_t	pbuf);

void
profile_thread(void)
{
    spl_t	    s;
    buffer_t	    buf_entry;
    queue_entry_t   prof_queue_entry;
    prof_data_t	    pbuf;
    kern_return_t   kr;
    int		    j;



    /* Initialise the queue header for the prof_queue */
    mpqueue_init(&prof_queue);

    while (TRUE) {

	/* Dequeue the first buffer. */
	s = splsched();
	mpdequeue_head(&prof_queue, &prof_queue_entry);
	splx(s);

	if ((buf_entry = (buffer_t) prof_queue_entry) == NULLPBUF) { 
	    assert_wait((int) profile_thread, FALSE);
	    thread_block((void (*)(void)) 0);
	    if (current_thread()->wait_result != THREAD_AWAKENED)
		break;
	} else {
	    register int    *sample;
	    int		    imax, dropped;

	    pbuf = buf_entry->p_prof;
	    kr = send_samples(pbuf->prof_port, (void *)buf_entry->p_zone,
			(mach_msg_type_number_t)buf_entry->p_index);
	    profile_sample_count += buf_entry->p_index;
	    if (kr != KERN_SUCCESS)
	      printf("send_samples(%x, %x, %d) error %x\n",
			pbuf->prof_port, buf_entry->p_zone, buf_entry->p_index, kr); 
	    dropped = buf_entry->p_dropped;
	    if (dropped > 0) {
		printf("kernel: profile dropped %d sample%s\n", dropped,
		       dropped == 1 ? "" : "s");
		buf_entry->p_dropped = 0;
	    }
	    /* Indicate you've finished the dirty job */
	    buf_entry->p_full = FALSE;
	    if (buf_entry->p_wakeme)
	      thread_wakeup((int) &buf_entry->p_wakeme);
	}

    }
    /* The profile thread has been signalled to exit.  Any threads waiting
       for the last buffer of samples to be acknowledged should be woken
       up now.  */
    profile_thread_id = THREAD_NULL;
    while (1) {
	s = splsched();
	mpdequeue_head(&prof_queue, &prof_queue_entry);
	splx(s);
	if ((buf_entry = (buffer_t) prof_queue_entry) == NULLPBUF)
	    break;
	if (buf_entry->p_wakeme)
	    thread_wakeup((int) &buf_entry->p_wakeme);
    }
#if 0	/* XXXXX */
    thread_halt_self();
#else
	panic("profile_thread(): halt_self");
#endif	/* XXXXX */
}



#include <mach/message.h>

void
send_last_sample_buf(
	prof_data_t	pbuf)
{
    spl_t    s;
    buffer_t buf_entry;

    if (pbuf == NULLPROFDATA)
	return;

    /* Ask for the sending of the last PC buffer.
     * Make a request to the profile_thread by inserting
     * the buffer in the send queue, and wake it up. 
     * The last buffer must be inserted at the head of the
     * send queue, so the profile_thread handles it immediatly. 
     */ 
    buf_entry = pbuf->prof_area + pbuf->prof_index;
    buf_entry->p_prof = pbuf;

    /* Watch out in case profile thread exits while we are about to
       queue data for it.  */
    s = splsched();
    if (profile_thread_id == THREAD_NULL)
	splx(s);
    else {
	buf_entry->p_wakeme = 1;
	mpenqueue_tail(&prof_queue, &buf_entry->p_list);
	thread_wakeup((int) profile_thread);
	assert_wait((int) &buf_entry->p_wakeme, TRUE);
	splx(s); 
	thread_block((void (*)(void)) 0);
    }
}


/*
 * Add a profile sample for the current pbuf.
 */

void
profile(
	int	pc,
	prof_data_t	pbuf)
{
    int inout_val = pc; 
    buffer_t buf_entry;
    int *val;

    if (pbuf == NULLPROFDATA)
	return;
    
    /* Inserts the PC value in the buffer of the thread */
    set_pbuf_value(pbuf, &inout_val); 
    switch(inout_val) {
    case 0: 
	if (profile_thread_id == THREAD_NULL) {
	  reset_pbuf_area(pbuf);
	}
	break;
    case 1: 
	/* Normal case, value successfully inserted */
	break;
    case 2 : 
	/*
	 * The value we have just inserted caused the
	 * buffer to be full, and ready to be sent.
	 * If profile_thread_id is null, the profile
	 * thread has been killed.  Since this generally
	 * happens only when the O/S server task of which
	 * it is a part is killed, it is not a great loss
	 * to throw away the data.
	 */
	if (profile_thread_id == THREAD_NULL) {
	  reset_pbuf_area(pbuf);
	  break;
	}

	buf_entry = (buffer_t) &pbuf->prof_area[pbuf->prof_index];
	buf_entry->p_prof = pbuf;
	mpenqueue_tail(&prof_queue, &buf_entry->p_list);
	
	/* Switch to another buffer */
	reset_pbuf_area(pbuf);
	
	/* Wake up the profile thread */
	if (profile_thread_id != THREAD_NULL)
	  thread_wakeup((int) profile_thread);
	break;
      
      default: 
	printf("profile : unexpected case\n"); 
    }
}


kern_return_t
thread_sample(
	thread_act_t	thr_act,
	ipc_port_t	reply)
{
    /* 
     * This routine is called every time that a new thread has made
     * a request for the sampling service. We must keep track of the 
     * correspondance between its identity (thread) and the port
     * we are going to use as a reply port to send out the samples resulting 
     * from its execution. 
     */
    prof_data_t	    pbuf;
    vm_offset_t	    vmpbuf;

    if (reply != MACH_PORT_NULL) {
	if (thr_act->act_profiled) 	/* yuck! */
		return KERN_INVALID_ARGUMENT;
	/* Start profiling this activation, do the initialization. */
	pbuf = pbuf_alloc();
	if ((thr_act->profil_buffer = pbuf) == NULLPROFDATA) {
	    printf("thread_sample: cannot allocate pbuf\n");
	    return KERN_RESOURCE_SHORTAGE;
	} 
	else {
	    if (!set_pbuf_nb(pbuf, NB_PROF_BUFFER-1)) {
		printf("mach_sample_thread: cannot set pbuf_nb\n");
		return KERN_FAILURE;
	    }
	    reset_pbuf_area(pbuf);
	}
	pbuf->prof_port = reply;
	thr_act->act_profiled = TRUE;
	thr_act->act_profiled_own = TRUE;
	if (profile_thread_id == THREAD_NULL)
	    profile_thread_id = kernel_thread(kernel_task, profile_thread, 0);
    } else {
	if (!thr_act->act_profiled)
		return(KERN_INVALID_ARGUMENT);

	thr_act->act_profiled = FALSE;
	/* do not stop sampling if thread is not profiled by its own */

	if (!thr_act->act_profiled_own)
	    return KERN_SUCCESS;
	else
	    thr_act->act_profiled_own = FALSE;

	send_last_sample_buf(thr_act->profil_buffer);
	pbuf_free(thr_act->profil_buffer);
	thr_act->profil_buffer = NULLPROFDATA;
    }
    return KERN_SUCCESS;
}

kern_return_t
task_sample(
	task_t		task,
	ipc_port_t	reply)
{
    prof_data_t	    pbuf=task->profil_buffer;
    vm_offset_t	    vmpbuf;
    int		    turnon = (reply != MACH_PORT_NULL);

    if (turnon) {
	if (task->task_profiled)
		return(KERN_INVALID_ARGUMENT);
	pbuf = pbuf_alloc();
	if (pbuf == NULLPROFDATA)
	  return KERN_RESOURCE_SHORTAGE;
	task->profil_buffer = pbuf;
	
	if (!set_pbuf_nb(pbuf, NB_PROF_BUFFER-1))
	    return KERN_FAILURE;
	reset_pbuf_area(pbuf);
	pbuf->prof_port = reply;
    } else
	if (!task->task_profiled)
		return(KERN_INVALID_ARGUMENT);


    if (turnon != task->task_profiled) {
	int actual, i;
	thread_act_t thr_act;

	if (turnon && profile_thread_id == THREAD_NULL)
	    profile_thread_id =
		kernel_thread(kernel_task, profile_thread, 0);
	task->task_profiled = turnon;  
	actual = task->thr_act_count; 
	for (i = 0, thr_act = (thread_act_t)queue_first(&task->thr_acts);
	     i < actual;
	     i++, thr_act = (thread_act_t)queue_next(&thr_act->thr_acts)) {
		  if (!thr_act->act_profiled_own) {
		    thr_act->act_profiled = turnon;
		    if (turnon) {
			thr_act->profil_buffer = task->profil_buffer;
		  	thr_act->act_profiled = TRUE;
		    } else {
			thr_act->act_profiled = FALSE;
		        thr_act->profil_buffer = NULLPROFDATA;
		    }
		  }
	}
	if (!turnon) {
	    send_last_sample_buf(task->profil_buffer);
	    pbuf_free(task->profil_buffer);
	    task->profil_buffer = NULLPROFDATA;
	}
    }

    return KERN_SUCCESS;
}

prof_data_t
pbuf_alloc(void)
{
	register prof_data_t pbuf;
	register i;
	register int *zone;

	pbuf = (prof_data_t)kalloc(sizeof(struct prof_data));
	if (!pbuf)
		return(NULLPROFDATA);
	pbuf->prof_port = MACH_PORT_NULL;
	for (i=0; i< NB_PROF_BUFFER; i++) {
		zone = (int *)kalloc(SIZE_PROF_BUFFER*sizeof(int));
		if (!zone) {
			i--;
			while (i--)
				kfree((vm_offset_t)pbuf->prof_area[i].p_zone, SIZE_PROF_BUFFER*sizeof(int));
			kfree((vm_offset_t)pbuf, sizeof(struct prof_data));
			return(NULLPROFDATA);
		}
		pbuf->prof_area[i].p_zone = zone;
		pbuf->prof_area[i].p_full = FALSE;
	}
	pbuf->prof_port = MACH_PORT_NULL;
	return(pbuf);
}

void
pbuf_free(
	prof_data_t	pbuf)
{
	register i;

	if (pbuf->prof_port)
		ipc_port_release_send(pbuf->prof_port);
	
	for(i=0; i < NB_PROF_BUFFER ; i++)
		kfree((vm_offset_t)pbuf->prof_area[i].p_zone, SIZE_PROF_BUFFER*sizeof(int));
	kfree((vm_offset_t)pbuf, sizeof(struct prof_data));
}
