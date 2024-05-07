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

AT386_CONFIG=STD+WS
PMAX_CONFIG=STD+ANY
SQT_CONFIG=STD+SQT+32
INTEL860_CONFIG=STD+PARAGON

CONFIG=${MACH_KERNEL_${TARGET_MACHINE}_CONFIG:U${MACH_KERNEL_CONFIG:U${${TARGET_MACHINE}_CONFIG}}}

OTHERS		= ../${CONFIG}/Makefile \
		  ../${CONFIG}/cputypes.h \
		  ../${CONFIG}/platforms.h \
		  ../${CONFIG}/vers.config \
		  ../${CONFIG}/device/Makefile \
		  ../${CONFIG}/mach/Makefile \
		  ../${CONFIG}/mach_debug/Makefile \
		  ../${CONFIG}/norma/Makefile

DEPENDENCIES	=

.include <${RULES_MK}>

.ORDER: ../${CONFIG}/Makefile ../${CONFIG}/cputypes.h ../${CONFIG}/platforms.h

../${CONFIG}/Makefile:	${TARGET_MACHINE}/${CONFIG}
	@echo "[ configuring ${CONFIG} ]"
	@-if [ -d ../${CONFIG} ];					\
	then								\
		true;							\
	else								\
		mkdir ../${CONFIG}; fi
	@-rm -f ${CONFIG}
	@cp ${${TARGET_MACHINE}/${CONFIG}:P} ${CONFIG}
	${_CONFIG_} -MD `genpath -I.` -c . ${CONFIG_FLAGS} ${CONFIG}

../${CONFIG}/cputypes.h:
	-rm -f ../${CONFIG}/cputypes.h
	-ln ../${CONFIG}/platforms.h ../${CONFIG}/cputypes.h

../${CONFIG}/platforms.h:
	-rm -f ../${CONFIG}/platforms.h
	-ln ../${CONFIG}/cputypes.h ../${CONFIG}/platforms.h

../${CONFIG}/vers.config: ../${CONFIG}/Makefile
	echo ${CONFIG} >../${CONFIG}/vers.config

../${CONFIG}/device/Makefile: ../${CONFIG}/Makefile ../device/template.mk
	${MAKEPATH} ../${CONFIG}/device/.
	sed -n -e '/^IDENT=/p' ../${CONFIG}/Makefile > ${.TARGET}.X
	cat ${../device/template.mk:P} >> ${.TARGET}.X
	${MV} ${.TARGET}.X ${.TARGET}

../${CONFIG}/mach/Makefile: ../${CONFIG}/Makefile ../mach/template.mk
	${MAKEPATH} ../${CONFIG}/mach/.
	sed -n -e '/^IDENT=/p' ../${CONFIG}/Makefile > ${.TARGET}.X
	cat ${../mach/template.mk:P} >> ${.TARGET}.X
	${MV} ${.TARGET}.X ${.TARGET}

../${CONFIG}/mach_debug/Makefile: ../${CONFIG}/Makefile \
				  ../mach_debug/template.mk
	${MAKEPATH} ../${CONFIG}/mach_debug/.
	sed -n -e '/^IDENT=/p' ../${CONFIG}/Makefile > ${.TARGET}.X
	cat ${../mach_debug/template.mk:P} >> ${.TARGET}.X
	${MV} ${.TARGET}.X ${.TARGET}

../${CONFIG}/norma/Makefile: ../${CONFIG}/Makefile \
			     ../norma/template.mk
	${MAKEPATH} ../${CONFIG}/norma/.
	sed -n -e '/^IDENT=/p' ../${CONFIG}/Makefile > ${.TARGET}.X
	cat ${../norma/template.mk:P} >> ${.TARGET}.X
	${MV} ${.TARGET}.X ${.TARGET}

.if exists(config.local)
../${CONFIG}/Makefile:	config.local
.else
config.local:
	@true
.endif

.if exists(${TARGET_MACHINE}/config.local)
../${CONFIG}/Makefile:	${TARGET_MACHINE}/config.local
.else
${TARGET_MACHINE}/config.local:
	@true
.endif

.if exists(depend.mk)
.include "depend.mk"
.endif
