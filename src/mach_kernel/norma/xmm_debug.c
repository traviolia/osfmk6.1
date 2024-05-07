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
 * Revision 2.3.3.5  92/06/24  18:02:25  jeffreyh
 * 	Minor changes for XMM Framework cleanup.  %M and %R are probably
 * 	broken, ditto for the sample calls.
 * 	[92/06/24            dlb]
 * 
 * 	Add debug print support for use_routine values.
 * 	[92/06/04            dlb]
 * 
 * Revision 2.3.3.4  92/03/28  10:12:39  jeffreyh
 * 	Changed data_write to data_write_return and deleted data_return.
 * 	[92/03/20            sjs]
 * 
 * Revision 2.3.3.3  92/02/21  11:25:46  jsb
 * 	No longer an xmm layer. Will eventually be used by xmm_invoke.c.
 * 	[92/02/20  15:45:23  jsb]
 * 
 * Revision 2.3.3.1  92/01/21  21:53:54  jsb
 * 	De-linted. Supports new (dlb) memory object routines.
 * 	Supports arbitrary reply ports to lock_request, etc.
 * 	Converted mach_port_t (and port_t) to ipc_port_t.
 * 	Use db_printf instead of printf.
 * 	[92/01/20  17:20:11  jsb]
 * 
 * 	Fixes from OSF.
 * 	[92/01/17  14:14:30  jsb]
 * 
 * Revision 2.3.1.1  92/01/15  12:14:56  jeffreyh
 * 	Return a value from the debug layer terminate routine.
 * 	It was losing it before. (sjs)
 * 
 * Revision 2.3  91/07/01  08:26:00  jsb
 * 	Collect garbage. Return valid return values.
 * 	[91/06/29  15:25:16  jsb]
 * 
 * Revision 2.2  91/06/17  15:48:13  jsb
 * 	First checkin.
 * 	[91/06/17  11:05:55  jsb]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
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
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 */
/*
 *	File:	norma/xmm_debug.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Xmm layer providing debugging output.
 */

#ifdef	MACH_KERNEL
#include <norma/xmm_obj.h>
#include <sys/varargs.h>
#else	/* MACH_KERNEL */
#include <xmm_obj.h>
#include <varargs.h>
#endif	/* MACH_KERNEL */

int xmm_debug = 0;

/* VARARGS */
m_printf(fmt, va_alist)
	char *fmt;
	va_dcl
{
	va_list adx;
	char c, buf[1024], fmt0[3], *bp;
	int i;
	xmm_obj_t mobj;
	xmm_obj_t kobj;
	xmm_reply_t reply;

	if (! xmm_debug) {
		return;
	}
	va_start(adx);
	fmt0[0] = '%';
	fmt0[2] = '\0';
	for (;;) {
		bp = buf;
		while ((c = *fmt++) && c != '%') {
			*bp++ = c;
		}
		*bp++ = '\0';
		db_printf("%s", buf);
		if (c == '\0') {
			break;
		}
		switch (c = *fmt++) {
		case 'M':
			mobj = va_arg(adx, xmm_obj_t);
			kobj = va_arg(adx, xmm_obj_t);
			db_printf("k_%s.%x -> m_%s.%x",
				  kobj->k_kobj->class->c_name, kobj,
				  mobj->m_mobj->class->c_name, mobj);
			break;
			
		case 'K':
			kobj = va_arg(adx, xmm_obj_t);
			mobj = kobj;	/* XXX no */
			db_printf("m_%s.%x -> k_%s.%x",
				  mobj->m_mobj->class->c_name, mobj,
				  kobj->k_kobj->class->c_name, kobj);
			break;

		case 'Z':
			i = va_arg(adx, int);
			db_printf("%d", i / PAGE_SIZE);
			if (i % PAGE_SIZE) {
				db_printf(".%d", i % PAGE_SIZE);
			}
			break;

		case 'P':
			i = va_arg(adx, int);
			db_printf("%c%c%c",
				  ((i & VM_PROT_READ) ? 'r' : '-'),
				  ((i & VM_PROT_WRITE) ? 'w' : '-'),
				  ((i & VM_PROT_EXECUTE)? 'x' : '-'));
			break;

		case 'N':
			if (! va_arg(adx, int)) {
				db_printf("!");
			}
			break;

		case 'C':
			i = va_arg(adx, int);
			if (i == MEMORY_OBJECT_COPY_NONE) {
				bp = "copy_none";
			} else if (i == MEMORY_OBJECT_COPY_CALL) {
				bp = "copy_call";
			} else if (i == MEMORY_OBJECT_COPY_DELAY) {
				bp = "copy_delay";
			} else {
				bp = "copy_???";
			}
			db_printf("%s", bp);
			break;

		case 'U':
			i = va_arg(adx, int);
			if (i == XMM_USE_DATA_WRITE) {
				bp = "data_write";
			} else if (i == XMM_USE_DATA_RETURN) {
				bp = "data_return";
			} else if (i == XMM_USE_DATA_INITIALIZE) {
				bp = "data_initialize";
			} else {
				bp = "data_???";
			}
			db_printf("%s", bp);
			break;

		case 'R':
			reply = va_arg(adx, xmm_reply_t);
			/* XXX mobj = reply->mobj; ??? */
			db_printf("k_%s.%x -> r_%s.%x",
				  mobj->k_kobj->class->c_name, mobj,
				  mobj->m_mobj->class->c_name, reply);
			break;

		default:
			fmt0[1] = c;
			db_printf(fmt0, va_arg(adx, long));
			break;
		}
	}
	va_end(adx);
}

#if 0
sample_calls()
{
	m_printf("m_init            (%M, 0x%x, %d)\n",
		 mobj, memory_object_name, pagesize);
	m_printf("m_terminate       (%M, 0x%x)\n",
		 mobj, kobj, memory_object_name);
	m_printf("m_copy            (%M, %Z, %Z, 0x%x)\n",
		 mobj, kobj, offset, length, new_mobj);
	m_printf("m_data_request    (%M, %Z, %Z, %P)\n",
		 mobj, kobj, offset, length, desired_access);
	m_printf("m_data_unlock     (%M, %Z, %Z, %P)\n",
		 mobj, kobj, offset, length, desired_access);
	m_printf("m_data_write_return [%U]      (%M, %Z, 0x%x, %Z)\n",
		 use_routine, mobj, kobj, offset, data, length);
	m_printf("m_lock_completed  (%R, %Z, %Z)\n",
		 reply, offset, length);
	m_printf("m_supply_completed(%R, %Z, %Z, 0x%x, %Z)\n",
		 reply, offset, length, result, error_offset);
	m_printf("m_change_completed(%R, %Nmay_cache, %C)\n",
		 reply, may_cache, copy_strategy);
	m_printf("k_data_unavailable(%K, %Z, %Z)\n",
		 kobj, offset, length);
	m_printf("k_get_attributes  (%K)\n",
		 kobj);
	m_printf("k_lock_request    (%K, %Z, %Z, %Nclean, %Nflush, %P, 0x%x)\n",
		 kobj, offset, length, should_clean, should_flush, lock_value,
		 reply);
	m_printf("k_data_error      (%K, %Z, %Z, 0x%x)\n",
		 kobj, offset, length, error_value);
	m_printf("k_set_ready       (%K, %Nready, %Nmay_cache, %C, %U, 0x%x)\n",
		 kobj, object_ready, may_cache, copy_strategy,
		 use_routine, reply);
	m_printf("k_destroy         (%K, 0x%x)\n",
		 kobj, reason);
	m_printf("k_data_supply     (%K, %Z, 0x%x, %Z, %P, %Npr, rply=0x%x)\n",
		 kobj, offset, data, length, lock_value, precious, reply);
}
#endif
