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

VPATH		= ..:../..

MIGFLAGS	= -MD ${IDENT} -X
MIGKSFLAGS	= -DKERNEL_SERVER
MIGKUFLAGS	= -DKERNEL_USER

NORMA_INTERNAL_FILES =				\
	norma_internal_server.h			\
	norma_internal_server.c 

NORMA_INTERNAL_USER_FILES =			\
	norma_internal.h			\
	norma_internal_user.c

PROXY_FILES = 					\
	proxy_server.h				\
	proxy_server.c

PROXY_USER_FILES = 				\
	proxy_user.h				\
	proxy_user.c

NORMA_DEVICE_FILES =				\
	dev_forward.h				\
	dev_forward.c

OTHERS		= ${NORMA_INTERNAL_FILES} ${NORMA_INTERNAL_USER_FILES} \
		  ${PROXY_FILES} ${PROXY_USER_FILES} \
		  ${NORMA_DEVICE_FILES} ${T_M_FILES}

INCFLAGS	= -I.. -I../..
MDINCFLAGS	= -I.. -I../..

DEPENDENCIES	=

.if exists(../../norma/${TARGET_MACHINE}/machdep.mk)
.include "../../norma/${TARGET_MACHINE}/machdep.mk"
.endif
.include <${RULES_MK}>

.ORDER: ${NORMA_INTERNAL_FILES}

${NORMA_INTERNAL_FILES}:  norma/norma_internal.defs
	${_MIG_} ${_MIGFLAGS_} ${MIGKSFLAGS}		\
		-header /dev/null			\
		-user /dev/null				\
		-sheader norma_internal_server.h	\
                -server norma_internal_server.c		\
                ${norma/norma_internal.defs:P}

.ORDER: ${NORMA_INTERNAL_USER_FILES}

${NORMA_INTERNAL_USER_FILES}:  norma/norma_internal.defs
	${_MIG_} ${_MIGFLAGS_} ${MIGKUFLAGS}		\
		-header norma_internal.h		\
		-user norma_internal_user.c		\
		-server /dev/null			\
                ${norma/norma_internal.defs:P}

.ORDER: ${PROXY_FILES}

${PROXY_FILES}: norma/xmm_proxy.defs
	${_MIG_} ${_MIGFLAGS_} ${MIGKSFLAGS}		\
		-header /dev/null			\
		-user /dev/null				\
		-sheader proxy_server.h			\
		-server proxy_server.c			\
                ${norma/xmm_proxy.defs:P}

.ORDER: ${PROXY_USER_FILES}

${PROXY_USER_FILES}: norma/xmm_proxy.defs
	${_MIG_} ${_MIGFLAGS_} ${MIGKUFLAGS}		\
		-header proxy_user.h			\
		-user proxy_user.c			\
		-server /dev/null			\
		${norma/xmm_proxy.defs:P}

.ORDER: ${NORMA_DEVICE_FILES}

${NORMA_DEVICE_FILES}: device/dev_forward.defs
	${_MIG_} ${_MIGFLAGS_} ${MIGKUFLAGS}		\
		-header dev_forward.h			\
		-user dev_forward.c			\
		-server /dev/null			\
                ${device/dev_forward.defs:P}

.if exists(depend.mk)
.include "depend.mk"
.endif
