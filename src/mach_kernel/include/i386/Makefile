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

VPATH	= ../../i386:../../i386/${TARGET_MACHINE}

EXPINC_SUBDIRS	= mp

DATAFILES		= ast.h ast_types.h \
			  gdb_defs.h kgdb_defs.h kgdb_setjmp.h \
			  cpu_number.h db_machdep.h lock.h mach_param.h \
			  machine_routines.h machparam.h ntoh.h pmap.h \
			  setjmp.h syscall_subr.h task.h thread.h \
			  vm_tuning.h xpr.h spl.h thread_act.h \
			  ${${TARGET_MACHINE}_DATAFILES}

INCLUDES		= ${DATAFILES}

EXPDIR			= /mach_kernel/i386/

.if	exists(${TARGET_MACHINE}/machdep.mk)
.include "${TARGET_MACHINE}/machdep.mk"
.endif
.include <${RULES_MK}>
