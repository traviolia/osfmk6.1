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
 * Revision 2.1.2.1  92/09/15  17:35:14  jeffreyh
 * 	Created.
 * 	[92/09/15  15:27:42  jeffreyh]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1992 Carnegie Mellon University
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
/*
 *	File:		norma/tr.h
 *	Author:		Alan Langerman, Jeffrey Heller
 *	Date:		1992
 *
 *	Internal trace routines.  Like old-style XPRs but
 *	less formatting.
 */

#include <mach_assert.h>

#define	TRACE_BUFFER	MACH_ASSERT

/*
 *	Log events in a circular trace buffer for future debugging.
 *	Events are unsigned integers.  Each event has a descriptive
 *	message.
 *
 *	TR_DECL must be used at the beginning of a routine using
 *	one of the tr calls.  The macro should be passed the name
 *	of the function surrounded by quotation marks, e.g.,
 *		TR_DECL("netipc_recv_intr");
 *	and should be terminated with a semi-colon.  The TR_DECL
 *	must be the *last* declaration in the variable declaration
 *	list, or syntax errors will be introduced when TRACE_BUFFER
 *	is turned off.
 */
#ifndef	_NORMA_TR_H_
#define	_NORMA_TR_H_

#if	TRACE_BUFFER
#define	TR_INIT()		tr_init()

#define	TR_DECL(funcname)	char	*__ntr_func_name__ = funcname

#define	tr1(msg)				\
	tr(__ntr_func_name__,			\
	   __FILE__,				\
	   __LINE__,				\
	   (msg),				\
	   0,0,0,0)

#define	tr2(msg, tag1)				\
	tr(__ntr_func_name__,			\
	   __FILE__,				\
	   __LINE__,				\
	   (msg),				\
	   (unsigned int)(tag1),		\
	   0,0,0)

#define	tr3(msg, tag1, tag2)			\
	tr(__ntr_func_name__,			\
	   __FILE__,				\
	   __LINE__,				\
	   (msg),				\
	   (unsigned int)(tag1),		\
	   (unsigned int)(tag2),		\
	   0,0)

#define	tr4(msg, tag1, tag2, tag3)		\
	tr(__ntr_func_name__,			\
	   __FILE__,				\
	   __LINE__,				\
	   (msg),				\
	   (unsigned int)(tag1),		\
	   (unsigned int)(tag2),		\
	   (unsigned int)(tag3),		\
	   0)

#define	tr5(msg, tag1, tag2, tag3, tag4)	\
	tr(__ntr_func_name__,			\
	   __FILE__,				\
	   __LINE__,				\
	   (msg),				\
	   (unsigned int)(tag1),		\
	   (unsigned int)(tag2),		\
	   (unsigned int)(tag3),		\
	   (unsigned int)(tag4))

extern void	tr_init(void);

extern void	tr(
		char		*funcname,
		char		*file,
		unsigned int	lineno,
		char		*fmt,
		unsigned int	tag1,
		unsigned int	tag2,
		unsigned int	tag3,
		unsigned int	tag4);

#else /* TRACE_BUFFER */

#define	TR_INIT()
#define	TR_DECL(funcname)
#define tr1(msg)
#define tr2(msg, tag1)
#define tr3(msg, tag1, tag2)
#define tr4(msg, tag1, tag2, tag3)
#define tr5(msg, tag1, tag2, tag3, tag4)

#endif /* TRACE_BUFFER */

#endif	/* _NORMA_TR_H_ */
