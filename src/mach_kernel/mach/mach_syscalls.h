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

#ifndef MACH_SYSCALLS_H
#define	MACH_SYSCALLS_H

#include <mach/kern_return.h>
#include <mach/boolean.h>
#include <mach/port.h>
#include <mach/std_types.h>
#include <mach/mach_types.h>
#include <mach/exception.h>
#include <kern/clock.h>
#include <device/device_types.h>

extern kern_return_t syscall_task_create(
				mach_port_t	parent_task,
				boolean_t	inherit_memory,
				mach_port_t	*child_task);

extern kern_return_t syscall_kernel_task_create(
				mach_port_t	parent_task,
				vm_offset_t	map_base,
				vm_size_t	map_size,
				mach_port_t	*child_task);

extern kern_return_t syscall_task_terminate(
				mach_port_t	task);

extern kern_return_t syscall_vm_allocate(
				mach_port_t	target_map,
				vm_offset_t	*address,
				vm_size_t	size,
				boolean_t	anywhere);

extern kern_return_t syscall_vm_deallocate(
				mach_port_t	target_map,
				vm_offset_t	start,
				vm_size_t	size);

extern kern_return_t syscall_vm_write(
				mach_port_t		target_map,
				vm_address_t		address,
				vm_offset_t		data,
				mach_msg_type_number_t	size);

extern kern_return_t syscall_task_suspend(
				mach_port_t	task);

extern kern_return_t syscall_task_set_special_port(
				mach_port_t	task,
				int		which_port,
				mach_port_t	port_name);

extern kern_return_t syscall_vm_map(
				mach_port_t	target_map,
				vm_offset_t	*address,
				vm_size_t	size,
				vm_offset_t	mask,
				boolean_t	anywhere,
				mach_port_t	memory_object,
				vm_offset_t	offset,
				boolean_t	copy,
				vm_prot_t	cur_protection,
				vm_prot_t	max_protection,
				vm_inherit_t	inheritance);

extern kern_return_t syscall_thread_create_running(
				mach_port_t	parent_task,
				int		flavor,
				thread_state_t	new_state,
				unsigned int	new_state_count,
				mach_port_t	*child_thread);

extern kern_return_t syscall_vm_read_overwrite(
				mach_port_t	target_map,
				vm_address_t	address,
				vm_size_t	size,
				vm_address_t	data,
				vm_size_t	*data_size);

extern kern_return_t syscall_mach_port_allocate(
				mach_port_t		task,
				mach_port_right_t	right,
				mach_port_t		*namep);

extern kern_return_t syscall_mach_port_allocate_name(
				mach_port_t		task,
				mach_port_right_t	right,
				mach_port_t		name);

extern kern_return_t syscall_mach_port_deallocate(
				mach_port_t	task,
				mach_port_t	name);

extern kern_return_t syscall_mach_port_insert_right(
				mach_port_t		task,
				mach_port_t		name,
				mach_port_t		right,
				mach_msg_type_name_t	rightType);

extern kern_return_t syscall_mach_port_move_member(
				mach_port_t		task,
				mach_port_t		name,
				mach_port_t		after);

extern kern_return_t syscall_thread_depress_abort(
				mach_port_t	thread);

extern kern_return_t	syscall_clock_get_time(
				mach_port_t	clock_name,
				tvalspec_t	*cur_time);	/* OUT */

extern kern_return_t	clock_sleep_trap(
				mach_port_t	clock_name,
				sleep_type_t	sleep_type,
				int		sleep_sec,
				int		sleep_nsec,
				tvalspec_t	*wakeup_time);

extern kern_return_t syscall_device_read(
				mach_port_t		device,
				dev_mode_t		mode,
				recnum_t		recnum,
				int			bytes_wanted,
				io_buf_ptr_t		*data_ptr,
				unsigned int		*data_count_ptr);

extern kern_return_t syscall_device_read_overwrite(
				mach_port_t		device,
				dev_mode_t		mode,
				recnum_t		recnum,
				int			bytes_wanted,
				vm_address_t		buffer,
				unsigned int		*data_count_ptr);

extern kern_return_t syscall_device_read_request(
				mach_port_t		device,
				mach_port_t		reply_port,
				dev_mode_t		mode,
				recnum_t		recnum,
				int			bytes_wanted);

extern kern_return_t syscall_device_read_overwrite_request(
				mach_port_t		device,
				mach_port_t		reply_port,
				dev_mode_t		mode,
				recnum_t		recnum,
				int			bytes_wanted,
				vm_address_t		buffer);

extern kern_return_t device_read_async(
				mach_port_t		device,
				mach_port_t		queue,
				mach_port_t		request_id,
				dev_mode_t		mode,
				recnum_t		recnum,
				int			bytes_wanted);

extern kern_return_t device_read_async_inband(
				mach_port_t		device,
				mach_port_t		queue,
				mach_port_t		request_id,
				dev_mode_t		mode,
				recnum_t		recnum,
				int			bytes_wanted);

extern kern_return_t device_read_overwrite_async(
				mach_port_t		device,
				mach_port_t		queue,
				mach_port_t		request_id,
				dev_mode_t		mode,
				recnum_t		recnum,
				int			bytes_wanted,
				vm_address_t		buffer);

extern kern_return_t syscall_device_write(
				mach_port_t		device,
				dev_mode_t		mode,
				recnum_t		recnum,
				io_buf_ptr_t		data,
				unsigned int		data_count,
				int			*bytes_written_ptr);

extern kern_return_t syscall_device_write_request(
				mach_port_t		device,
				mach_port_t		reply_port,
				dev_mode_t		mode,
				recnum_t		recnum,
				io_buf_ptr_t		data,
				unsigned int		data_count);

extern kern_return_t device_write_async(
				mach_port_t		device,
				mach_port_t		queue,
				mach_port_t		request_id,
				dev_mode_t		mode,
				recnum_t		recnum,
				io_buf_ptr_t		data,
				unsigned int		data_count);

extern kern_return_t device_write_async_inband(
				mach_port_t		device,
				mach_port_t		queue,
				mach_port_t		request_id,
				dev_mode_t		mode,
				recnum_t		recnum,
				io_buf_ptr_t		data,
				unsigned int		data_count);

extern kern_return_t io_done_queue_wait(
				mach_port_t		queue,
				io_done_result_t	*result);

extern	kern_return_t	evc_wait(
				unsigned	ev_id);

extern	kern_return_t	syscall_vm_msync(
				mach_port_t             target_map,
				vm_address_t            address,
				vm_size_t               size,
				vm_sync_t               sync_flags);

extern	kern_return_t	syscall_thread_set_exception_ports(
				mach_port_t		thread,
				exception_mask_t	new_mask,
				mach_port_t		new_port,
				exception_behavior_t	new_behavior,
				thread_state_flavor_t	new_flavor);

extern	kern_return_t	syscall_mach_port_destroy(
				mach_port_t		task,
				mach_port_t		name);

extern	kern_return_t	syscall_task_set_exception_ports(
				mach_port_t		task,
				exception_mask_t	new_mask,
				mach_port_t		new_port,
				exception_behavior_t	new_behavior,
				thread_state_flavor_t	new_flavor);

extern	kern_return_t	syscall_mach_port_rename(
				mach_port_t		task,
				mach_port_t		old_name,
				mach_port_t		new_name);

extern kern_return_t syscall_vm_protect(
				mach_port_t	target_map,
				vm_offset_t	address,
				vm_size_t	size,
				boolean_t	set_maximum,
				vm_prot_t	new_protection);

extern kern_return_t syscall_vm_remap(
				mach_port_t	target_map,
				vm_offset_t	*address,
				vm_size_t	size,
				vm_offset_t	mask,
				boolean_t	anywhere,
				mach_port_t	src_map,
				vm_offset_t	memory_address,
				boolean_t	copy,
				vm_prot_t	*cur_protection,
				vm_prot_t	*max_protection,
				vm_inherit_t	inheritance);

extern kern_return_t syscall_vm_region(
				mach_port_t		target_map,
				vm_address_t		*address,
				vm_size_t		*size,
				vm_region_flavor_t	flavor,
				vm_region_info_t	info,
				mach_msg_type_number_t	*count,
				mach_port_t		*object_name);

extern kern_return_t syscall_task_info(
				mach_port_t		task,
				int			flavor,
				task_info_t		info_out,
				unsigned int		*info_cnt);

extern	kern_return_t	syscall_mach_port_allocate_subsystem(
				mach_port_t		task,
				mach_port_t		subsystem_name,
				mach_port_t		*namep);

extern kern_return_t syscall_mach_port_allocate_qos(
				mach_port_t		task,
				mach_port_right_t	right,
				mach_port_qos_t		*qosp,
				mach_port_t		*namep);

extern kern_return_t syscall_mach_port_allocate_full(
				mach_port_t		task,
				mach_port_right_t	right,
				mach_port_t		subsystem_name,
				mach_port_qos_t		*qos,
				mach_port_t		*namep);

extern kern_return_t syscall_task_set_port_space(
				mach_port_t	task,
				int		table_entries);

/*
 * NB: definition of below function is still in kern/syscall_subr.c --
 * it's very messy to move it elsewhere.
 */
extern kern_return_t syscall_thread_switch(
				mach_port_t	thread_name,
				int		option,
				int		option_time);

#endif	/* MACH_SYSCALLS_H */
