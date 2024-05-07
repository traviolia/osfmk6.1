#
# Copyright (c) 1995, 1994, 1993, 1992, 1991, 1990  
# Open Software Foundation, Inc. 
#  
# Permission to use, copy, modify, and distribute this software and 
# its documentation for any purpose and without fee is hereby granted, 
# provided that the above copyright notice appears in all copies and 
# that both the copyright notice and this permission notice appear in 
# supporting documentation, and that the name of ("OSF") or Open Software 
# Foundation not be used in advertising or publicity pertaining to 
# distribution of the software without specific, written prior permission. 
#  
# OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE 
# INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
# FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL OSF BE LIABLE FOR ANY 
# SPECIAL, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN 
# ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING 
# OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE 
#
#
# OSF Research Institute MK6.1 (unencumbered) 1/31/1995

SETUP_SUBDIRS		= machine ${MACHINE}

EXPINC_SUBDIRS		= machine ${MACHINE}

SUBDIRS			= machine ${MACHINE}

MACH_MIG_DEFS		= bootstrap.defs clock.defs clock_reply.defs \
			  mach_host.defs mach_port.defs \
			  memory_object_default.defs
NORMA_MIG_DEFS		= mach_norma.defs
MIG_DEFS		= ${MACH_MIG_DEFS} ${NORMA_MIG_DEFS}

MACH_DATAFILES		= boolean.h boot_info.h \
			  clock_types.defs clock_types.h \
			  error.h exception.h \
			  host_info.h kern_return.h mach.defs \
			  mach_param.h mach_traps.h \
			  mach_types.defs mach_types.h machine.h \
			  exc.defs \
			  memory_object.defs memory_object.h \
			  message.h mig_errors.h mig_log.h msg_type.h \
			  ndr.h norma_special_ports.h norma_task.defs \
			  notify.defs notify.h policy.h port.h \
			  processor_info.h rpc.h ipc_common.h \
			  std_types.defs std_types.h sync.h \
			  syscall_sw.h task_info.h task_special_ports.h \
			  thread_info.h thread_special_ports.h \
			  thread_status.h thread_switch.h time_value.h \
			  vm_attributes.h vm_inherit.h vm_param.h \
			  vm_prot.h vm_statistics.h vm_sync.h \
			  events_info.h task_ledger.h vm_behavior.h \
			  vm_region.h  \
			  ${MACH_MIG_DEFS}
# prof.defs ?
NORMA_DATAFILES		= prof_types.h ${NORMA_MIG_DEFS}
DATAFILES		= ${MACH_DATAFILES} ${NORMA_DATAFILES}

MIGINCLUDES		= mach_interface.h notify_server.h exc_server.h \
			  bootstrap_server.h
INCLUDES		= ${DATAFILES} ${_MIG_HDRS_} ${MIGINCLUDES}
SETUP_INCLUDES		= boolean.h kern_return.h message.h ndr.h port.h

ILIST			= ${INCLUDES}
IDIR			= ${MACH3_INCLUDE_IDIR}mach/
IMODE			= 644

INCFLAGS		= -I..

.include <${RULES_MK}>

${_MIG_HDRS_} ${MIGINCLUDES}: std_types.defs mach_types.defs

mach_interface.h: mach.defs
	${_MIG_} ${_MIGFLAGS_}					\
		-server /dev/null				\
		-user /dev/null					\
		-header	${.TARGET}				\
		${mach.defs:P}

notify_server.h exc_server.h bootstrap_server.h: $${.TARGET:_server.h=.defs}
	${_MIG_} ${_MIGFLAGS_}					\
		-server /dev/null				\
		-user /dev/null					\
		-header /dev/null				\
		-sheader ${.TARGET}				\
		${${.TARGET:_server.h=.defs}:P}
