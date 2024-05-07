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
# Mach Operating System
# Copyright (c) 1991,1990,1989,1988,1987,1986 Carnegie Mellon University
# All Rights Reserved.
#
# Permission to use, copy, modify and distribute this software and its
# documentation is hereby granted, provided that both the copyright
# notice and this permission notice appear in all copies of the
# software, derivative works or modified versions, and any portions
# thereof, and that both notices appear in supporting documentation.
#
# CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
# CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
# ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
#
# Carnegie Mellon requests users of this software to return to
#
#  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
#  School of Computer Science
#  Carnegie Mellon University
#  Pittsburgh PA 15213-3890
#
# any improvements or extensions that they make and grant Carnegie Mellon
# the rights to redistribute these changes.
#
#
# OSF Research Institute MK6.1 (unencumbered) 1/31/1995

#
#  This must be here before any rules are possibly defined by the
#  machine dependent makefile fragment so that a plain "make" command
#  always works.  The config program will emit an appropriate rule to
#  cause "all" to depend on every kernel configuration it generates.
#

VPATH=..

SUBDIRS=device mach mach_debug norma
OTHERS=${LOAD} ${DYNAMIC}

KCC=${${.TARGET}_DYNAMIC:D${DCC}:U${_CC_}}
DEPENDENCIES=
USE_STATIC_LIBRARIES=
CCTYPE=ansi
GLINE=
#
# microkernel should be compiled using ANSI rules (doens't work properly
# with traditional rules) but don't enforce strict checking for now

DYNAMIC_MODULE=_kmod
LOAD_IDIR=/
DYNAMIC_IDIR=/sbin/subsys/
ILIST=${LOAD} ${DYNAMIC:S;$;${DYNAMIC_MODULE};g}

NORMAL_GCC_WARNINGS=-Wtrigraphs -Wcomment -Wstrict-prototypes -Wimplicit -Wreturn-type -Wmissing-prototypes
EXTRA_GCC_WARNINGS=

PGFLAG=${PROFILING:D-pg:U}
NCFLAGS=${IDENT} -D_ANSI_C_SOURCE ${NORMAL_GCC_WARNINGS} ${WARNINGS:D${EXTRA_GCC_WARNINGS}}
CFLAGS=${PGFLAG} ${NCFLAGS} ${MACHINE_CFLAGS}
DRIVER_CFLAGS=${CFLAGS}
PROFILING_CFLAGS=${NCFLAGS}

INCFLAGS=-I..
MDINCFLAGS=-I.. -I${EXPORTBASE}/mach_kernel

MDFLAGS=-K${EXPORTBASE}/mach_kernel

INCDIRS:=-I${EXPORTBASE}/mach_kernel ${INCDIRS}

LDOBJS=${LDOBJS_PREFIX} ${OBJS} ${LDOBJS_SUFFIX}

LDDEPS=${LDDEPS_PREFIX} ${SYSDEPS} conf/newvers.sh conf/copyright \
	conf/version.major conf/version.minor conf/version.variant \
	conf/version.edit conf/version.patch

#
# Make internal symbols in the kernel servers visible for profiling
# and debugging. Don't enable paranoid type checking in the MiG stubs.
#
MIG_CFLAGS=-Dmig_internal= -DTypeCheck=0
mach_host_server.o_CFLAGS+=${CFLAGS} ${MIG_CFLAGS}
monitor_server.o_CFLAGS+=${CFLAGS} ${MIG_CFLAGS}
mach_debug_server.o_CFLAGS+=${CFLAGS} ${MIG_CFLAGS}
mach_server.o_CFLAGS+=${CFLAGS} ${MIG_CFLAGS}
exc_server.o_CFLAGS+=${CFLAGS} ${MIG_CFLAGS}
mach_user_internal.o_CFLAGS+=${CFLAGS} ${MIG_CFLAGS}

#
#  These macros are filled in by the config program depending on the
#  current configuration.  The MACHDEP macro is replaced by the
#  contents of the machine dependent makefile template and the others
#  are replaced by the corresponding symbol definitions for the
#  configuration.
#
%CFLAGS

%OBJS

%CFILES

%SFILES

%BFILES

%ORDERED

%LOAD

%DYNAMIC

%MACHDEP

.include <${RULES_MK}>

.if	defined(DYNAMIC) && !defined(TOSTAGE)

${DYNAMIC}: $${.TARGET:S/$$/${DYNAMIC_MODULE}/}

${DYNAMIC:S/$/${DYNAMIC_MODULE}/g}: $${$${.TARGET}_OBJECTS} ${VMUNIX_LIB}
	${DLD} ${DLD_FLAGS} -e ${${.TARGET}_ENTRY} \
		-o ${.TARGET} ${${.TARGET}_OBJECTS} ${VMUNIX_LIB}
.endif

_CC_GENINC_=-I.
_CCDEFS_=${TARGET_MACHINE_CFLAGS}
_host_INCDIRS_=${INCDIRS}

.PRECIOUS: Makefile

${LOAD}: ${PRELDDEPS} ${LDOBJS} ${LDDEPS}
	/bin/sh ${conf/newvers.sh:P} \
		${conf/copyright:P} \
		`cat ${conf/version.major:P} \
		     ${conf/version.minor:P} \
		     ${conf/version.variant:P} \
		     ${conf/version.edit:P} \
		     ${conf/version.patch:P}`
	${KCC} -c ${_CCFLAGS_} vers.c
	rm -f ${.TARGET}
	@echo [ loading ${.TARGET} ]
	@${_LD_} ${SHLDSTRIP} ${_LDFLAGS_} ${LDOBJS} vers.o ${_LIBS_}
	@chmod 755 a.out
	@-mv a.out ${.TARGET}
	@eval `awk 'NR==1{S=$$1;next;}\
	END {\
	  if (S != "") {\
	    C = "ln ${.TARGET} ${.TARGET}" S; \
	    print "rm -f ${.TARGET}" S "; echo \"" C "\"; " C; \
	  } else {\
	    print "echo \"No vers.suffix for ${.TARGET}\""; \
	  }\
	}' vers.suffix`
.if defined(SYMBOLIC_DEBUG_INFO)
	rm -f ${.TARGET}.debug
	@echo [ loading ${.TARGET}.debug ]
	@${_LD_} ${_LDFLAGS_} ${LDOBJS} vers.o ${_LIBS_}
	@chmod 755 a.out
	@-mv a.out ${.TARGET}.debug
	@eval `awk 'NR==1{S=$$1;next;}\
	END {\
	  if (S != "") {\
	    C = "ln ${.TARGET}.debug ${.TARGET}.debug" S; \
	    print "rm -f ${.TARGET}.debug" S "; echo \"" C "\"; " C; \
	  } else {\
	    print "echo \"No vers.suffix for ${.TARGET}.debug\""; \
	  }\
	}' vers.suffix`
.endif

.if defined(SYS_RULE_1)
	${SYS_RULE_1}
.endif
.if defined(SYS_RULE_2)
	${SYS_RULE_2}
.endif

relink: ${LOAD:=.relink}

${LOAD:=.relink}: ${LDDEPS}
	/bin/sh ${conf/newvers.sh:P} \
		${conf/copyright:P} \
		`cat ${conf/version.major:P} \
		     ${conf/version.minor:P} \
		     ${conf/version.variant:P} \
		     ${conf/version.edit:P} \
		     ${conf/version.patch:P}`
	${KCC} -c ${_CCFLAGS_} vers.c
	rm -f ${.TARGET:.relink=}
	@echo [ loading ${.TARGET:.relink=} ]
	@${_LD_} ${SHLDSTRIP} ${_LDFLAGS_} ${LDOBJS} vers.o ${_LIBS_}
	@chmod 755 a.out
	@-mv a.out ${.TARGET:.relink=}
	@eval `awk 'NR==1{S=$$1;next;}\
	END {\
	  if (S != "") {\
	    C = "ln ${.TARGET:.relink=} ${.TARGET:.relink=}" S; \
	    print "rm -f ${.TARGET:.relink=}" S "; echo \"" C "\"; " C; \
	  } else {\
	    print "echo \"No vers.suffix for ${.TARGET:.relink=}\""; \
	  }\
	}' vers.suffix`
.if defined(SYMBOLIC_DEBUG_INFO)
	rm -f ${.TARGET:.relink=.debug}
	@echo [ loading ${.TARGET:.relink=.debug} ]
	@${_LD_} ${_LDFLAGS_} ${LDOBJS} vers.o ${_LIBS_}
	@chmod 755 a.out
	@-mv a.out ${.TARGET:.relink=.debug}
	@eval `awk 'NR==1{S=$$1;next;}\
	END {\
	  if (S != "") {\
	    C = "ln ${.TARGET:.relink=.debug} ${.TARGET:.relink=.debug}" S; \
	    print "rm -f ${.TARGET:.relink=.debug}" S "; echo \"" C "\"; " C; \
	  } else {\
	    print "echo \"No vers.suffix for ${.TARGET:.relink=.debug}\""; \
	  }\
	}' vers.suffix`
.endif

.if defined(SYS_RULE_1)
	${SYS_RULE_1}
.endif
.if defined(SYS_RULE_2)
	${SYS_RULE_2}
.endif

.ORDER: ${LOAD}

${OBJS}: ${OBJSDEPS}

.if !defined(MACHINEDEP_RULES)
${COBJS}: $${$${.TARGET}_SOURCE}
	${KCC} -c ${_CCFLAGS_} ${${${.TARGET}_SOURCE}:P}
.endif

${DECODE_OFILES}: $${$${.TARGET}_SOURCE}
	${RM} ${_RMFLAGS_} ${.TARGET}
	${UUDECODE} ${${${.TARGET}_SOURCE}:P}

.if exists(depend.mk) && !defined(TOSTAGE)
.include "depend.mk"
.endif
