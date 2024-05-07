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
 * Revision 2.1.2.1.2.2  92/05/26  11:19:19  jeffreyh
 * 		Since some code tests locks not using simple_lock/simple_unlock
 * 	routines (intel pmap TLB routines) but just testing the lock
 * 	word, used xchgl instead of bts (bit set).
 * 	[92/05/18            bernadat]
 * 
 * Revision 2.1.2.1.2.1  92/02/18  18:46:21  jeffreyh
 * 	Moved from cbus/i386_lock.s and adapted for MACH_LOCK_MON
 * 	[91/12/23            bernadat]
 * 
 * Revision 2.1.4.1  91/09/26  04:27:29  bernadat
 * 	Created
 * 	[91/06/27  05:02:08  bernadat]
 * 
*/
/* CMU_ENDHIST */


#include <cpus.h>
#include <i386/asm.h>
#include <mach_ldebug.h>
#include <assym.s>

#if	MACH_LDEBUG
#define	L_PC		4(%ebp)
#define	L_ARG0		8(%ebp)
#define	L_ARG1		12(%ebp)
#else
#undef	FRAME
#undef	EMARF
#define	FRAME
#define	EMARF
#define	L_PC		(%esp)
#define	L_ARG0		4(%esp)
#define	L_ARG1		8(%esp)
#endif

#define	S_ILK		(%edx)
#if	MACH_LDEBUG
#define	S_TYPE		4(%edx)
#define	S_PC		8(%edx)
#define	S_THREAD	12(%edx)
#define	S_DURATIONH	16(%edx)
#define	S_DURATIONL	20(%edx)
#endif

#define	M_ILK		(%edx)
#define	M_LOCKED	1(%edx)
#define	M_WAITERS	2(%edx)
#if	MACH_LDEBUG
#define	M_TYPE		4(%edx)
#define	M_PC		8(%edx)
#define	M_THREAD	12(%edx)
#endif

#if	(NCPUS > 1)
#include <i386/AT386/mp/mp.h>
#define	CX(addr,reg)	addr(,reg,4)
#else
#define	CPU_NUMBER(reg)
#define	CX(addr,reg)	addr
#endif

#if	MACH_RT
#define	DISABLE_PREEMPTION()					\
	pushf						;	\
	cli						;	\
	CPU_NUMBER(%eax)				;	\
	lock						;	\
	incl	CX(EXT(preemption_level),%eax)		;	\
	popf

#define	ENABLE_PREEMPTION()					\
	pushf						;	\
	cli						;	\
	CPU_NUMBER(%eax)				;	\
	lock						;	\
	decl	CX(EXT(preemption_level),%eax)		;	\
	popf						;	\
	cmpl	$0,CX(EXT(preemption_level),%eax)	;	\
	jne	9f					;	\
	movl	CX(EXT(need_ast),%eax),%eax		;	\
	testl	$AST_URGENT,%eax			;	\
	je	9f					;	\
	pushl	%edx					;	\
	call	EXT(kernel_preempt_check)		;	\
	popl	%edx					;	\
9:

#else	/* MACH_RT */
#define	DISABLE_PREEMPTION()
#define	ENABLE_PREEMPTION()
#endif	/* MACH_RT */

#if	MACH_LDEBUG
#define	CHECK_TYPE(type,type_field)				\
	cmpl	$type,type_field			;	\
	je	1f					;	\
	movl	$type,%eax				;	\
	pushl	CX(lock_type_names,%eax)		;	\
	pushl	$2f					;	\
	call	EXT(panic)				;	\
	hlt						;	\
	.data						;	\
2:	String	"not a %s!"				;	\
	.text						;	\
1:

#define	CHECK_NO_SPINLOCKS()					\
	CPU_NUMBER(%eax)				;	\
	cmpl	$0,CX(EXT(spinlock_count),%eax)		;	\
	jne	3f					;	\
	cmpl	$0,CX(EXT(preemption_level),%eax)	;	\
	je	1f					;	\
3:	pushl	$2f					;	\
	call	EXT(panic)				;	\
	hlt						;	\
	.data						;	\
2:	String	"spinlocks_held!"			;	\
	.text						;	\
1:

#define	CHECK_THREAD(thd)					\
	CPU_NUMBER(%eax)				;	\
	movl	CX(EXT(active_threads),%eax),%ecx	;	\
	testl	%ecx,%ecx				;	\
	je	1f					;	\
	cmpl	%ecx,thd				;	\
	je	1f					;	\
	pushl	$2f					;	\
	call	EXT(panic)				;	\
	hlt						;	\
	.data						;	\
2:	String	"wrong thread!"				;	\
	.text						;	\
1:

#define	CHECK_MYLOCK(thd)					\
	CPU_NUMBER(%eax)				;	\
	movl	CX(EXT(active_threads),%eax),%ecx	;	\
	testl	%ecx,%ecx				;	\
	je	1f					;	\
	cmpl	%ecx,thd				;	\
	jne	1f					;	\
	pushl	$2f					;	\
	call	EXT(panic)				;	\
	hlt						;	\
	.data						;	\
2:	String	"mylock attempt!"			;	\
	.text						;	\
1:

#define	METER_SPINLOCK_LOCK(reg)				\
	pushl	reg					;	\
	call	EXT(meter_spinlock_lock)		;	\
	popl	reg

#define	METER_SPINLOCK_UNLOCK(reg)				\
	pushl	reg					;	\
	call	EXT(meter_spinlock_unlock)		;	\
	popl	reg

spinlock_name:
	String	"spinlock"
mutex_name:
	String	"mutex"

lock_type_names:
	.long	spinlock_name
	.long	mutex_name

#else
#define	CHECK_TYPE(type,type_field)
#define	CHECK_THREAD(thd)
#define	CHECK_NO_SPINLOCKS()
#define	CHECK_MYLOCK(thd)
#define	METER_SPINLOCK_LOCK(reg)
#define	METER_SPINLOCK_UNLOCK(reg)
#endif

ENTRY(spinlock_init)
	FRAME
	movl	L_ARG0,%edx		/ fetch lock pointer
	xorl	%eax,%eax
	movb	%al,S_ILK		/ unlock the HW lock
#if	MACH_LDEBUG
	movl	$SPINLOCK_TAG,S_TYPE	/ set lock type
	movl	%eax,S_PC		/ init caller pc
	movl	%eax,S_THREAD		/ and owning thread
	movl	%eax,S_DURATIONH
	movl	%eax,S_DURATIONL
#endif

	EMARF
	ret

ENTRY(_spinlock_lock)
	FRAME
	movl	L_ARG0,%edx		/ fetch lock pointer

	CHECK_TYPE(SPINLOCK_TAG,S_TYPE)

	DISABLE_PREEMPTION()

sl_get_hw:
	movb	$1,%cl
	xchgb	S_ILK,%cl		/ try to acquire the HW lock
	testb	%cl,%cl			/ did we succeed?

#if	MACH_LDEBUG
	je	5f
	CHECK_MYLOCK(S_THREAD)
	jmp	sl_get_hw
5:
#else	/* MACH_LDEBUG */
	jne	sl_get_hw		/ no, try again
#endif	/* MACH_LDEBUG */

#if	MACH_LDEBUG
	CPU_NUMBER(%eax)
	movl	L_PC,%ecx
	movl	%ecx,S_PC
	movl	CX(EXT(active_threads),%eax),%ecx
	movl	%ecx,S_THREAD
	incl	CX(EXT(spinlock_count),%eax)
#if 0
	METER_SPINLOCK_LOCK(%edx)
#endif
#if	NCPUS == 1
	pushf
	pushl	%edx
	cli
	call	EXT(lock_stack_push)
	popl	%edx
	popfl
#endif	/* NCPUS == 1 */
#endif	/* MACH_LDEBUG */

	EMARF
	ret

ENTRY(_spinlock_try)
	FRAME
	movl	L_ARG0,%edx		/ fetch lock pointer

	CHECK_TYPE(SPINLOCK_TAG,S_TYPE)

	DISABLE_PREEMPTION()

	movb	$1,%cl
	xchgb	S_ILK,%cl		/ try to acquire the HW lock
	testb	%cl,%cl			/ did we succeed?
	jne	1f			/ no, return failure

#if	MACH_LDEBUG
	CPU_NUMBER(%eax)
	movl	L_PC,%ecx
	movl	%ecx,S_PC
	movl	CX(EXT(active_threads),%eax),%ecx
	movl	%ecx,S_THREAD
	incl	CX(EXT(spinlock_count),%eax)
#if 0
	METER_SPINLOCK_LOCK(%edx)
#endif
#if	NCPUS == 1
	pushf
	pushl	%edx
	cli
	call	EXT(lock_stack_push)
	popl	%edx
	popfl
#endif	/* NCPUS == 1 */
#endif	/* MACH_LDEBUG */

	movl	$1,%eax			/ return success

	EMARF
	ret

1:
	ENABLE_PREEMPTION()

	xorl	%eax,%eax		/ and return failure

	EMARF
	ret

ENTRY(_spinlock_unlock)
	FRAME
	movl	L_ARG0,%edx		/ fetch lock pointer

	CHECK_TYPE(SPINLOCK_TAG,S_TYPE)
	CHECK_THREAD(S_THREAD)

#if	MACH_LDEBUG
	xorl	%eax,%eax
	movl	%eax,S_THREAD		/ disown thread
	CPU_NUMBER(%eax)
	decl	CX(EXT(spinlock_count),%eax)
#if 0
	METER_SPINLOCK_UNLOCK(%edx)
#endif
#if	NCPUS == 1
	pushf
	pushl	%edx
	cli
	call	EXT(lock_stack_pop)
	popl	%edx
	popfl
#endif	/* NCPUS == 1 */
#endif	/* MACH_LDEBUG */

	xorb	%cl,%cl
	xchgb	S_ILK,%cl		/ unlock the HW lock

	ENABLE_PREEMPTION()

	EMARF
	ret

ENTRY(mutex_init)
	FRAME
	movl	L_ARG0,%edx		/ fetch lock pointer
	xorl	%eax,%eax
	movb	%al,M_ILK		/ clear interlock
	movb	%al,M_LOCKED		/ clear locked flag
	movw	%ax,M_WAITERS		/ init waiter count

#if	MACH_LDEBUG
	movl	$MUTEX_TAG,M_TYPE	/ set lock type
	movl	%eax,M_PC		/ init caller pc
	movl	%eax,M_THREAD		/ and owning thread
#endif

	EMARF
	ret

ENTRY(_mutex_lock)
	FRAME
	movl	L_ARG0,%edx		/ fetch lock pointer

	CHECK_TYPE(MUTEX_TAG,M_TYPE)
	CHECK_NO_SPINLOCKS()

ml_retry:
	DISABLE_PREEMPTION()

ml_get_hw:
	movb	$1,%cl
	xchgb	%cl,M_ILK
	testb	%cl,%cl			/ did we succeed?
	jne	ml_get_hw		/ no, try again

/
/ Beware of a race between this code path and the inline ASM fast-path locking
/ sequence which attempts to lock a mutex by directly setting the locked flag
/ 

	movb	$1,%cl
	xchgb	%cl,M_LOCKED		/ try to set locked flag
	testb	%cl,%cl			/ is the mutex locked?
	jne	ml_fail			/ yes, we lose

#if	MACH_LDEBUG
	CPU_NUMBER(%eax)
	movl	L_PC,%ecx
	movl	%ecx,M_PC
	movl	CX(EXT(active_threads),%eax),%ecx
	movl	%ecx,M_THREAD
	testl	%ecx,%ecx
	je	3f
	incl	TH_MUTEX_COUNT(%ecx)
3:
#endif

	xorb	%cl,%cl
	xchgb	%cl,M_ILK

	ENABLE_PREEMPTION()

	EMARF
	ret

ml_fail:
	CHECK_MYLOCK(M_THREAD)
	pushl	%edx			/ push mutex address
	call	EXT(mutex_lock_wait)	/ yes, wait for the lock
	addl	$4,%esp
	movl	L_ARG0,%edx		/ refetch lock pointer
	jmp	ml_retry		/ and try again

ENTRY(_mutex_try)
	FRAME
	movl	L_ARG0,%edx		/ fetch lock pointer

	CHECK_TYPE(MUTEX_TAG,M_TYPE)
	CHECK_NO_SPINLOCKS()

	xorl	%eax,%eax
	movb	$1,%al			/ locked value for mutex
	xchgb	%al,M_LOCKED		/ swap locked values
	xorb	$1,%al			/ generate return value

#if	MACH_LDEBUG
	testl	%eax,%eax		/ did we succeed?
	je	1f			/ no, skip
	movl	L_PC,%ecx
	movl	%ecx,M_PC
	CPU_NUMBER(%ecx)
	movl	CX(EXT(active_threads),%ecx),%ecx
	movl	%ecx,M_THREAD
	testl	%ecx,%ecx
	je	1f
	incl	TH_MUTEX_COUNT(%ecx)
1:
#endif

	EMARF
	ret

ENTRY(mutex_unlock)
	FRAME
	movl	L_ARG0,%edx		/ fetch lock pointer

	CHECK_TYPE(MUTEX_TAG,M_TYPE)
	CHECK_THREAD(M_THREAD)

	DISABLE_PREEMPTION()

mu_get_hw:
	movb	$1,%cl
	xchgb	%cl,M_ILK
	testb	%cl,%cl			/ did we succeed?
	jne	mu_get_hw		/ no, try again

	cmpw	$0,M_WAITERS		/ are there any waiters?
	jne	mu_wakeup		/ yes, more work to do

mu_doit:
#if	MACH_LDEBUG
	xorl	%eax,%eax
	movl	%eax,M_THREAD		/ disown thread
	CPU_NUMBER(%eax)
	movl	CX(EXT(active_threads),%eax),%ecx
	testl	%ecx,%ecx
	je	0f
	decl	TH_MUTEX_COUNT(%ecx)
0:
#endif

	xorb	%cl,%cl
	xchgb	%cl,M_LOCKED		/ unlock the mutex

	xorb	%cl,%cl
	xchgb	%cl,M_ILK

	ENABLE_PREEMPTION()

	EMARF
	ret

mu_wakeup:
	pushl	%edx			/ push mutex address
	call	EXT(mutex_unlock_wakeup)/ yes, wake a thread
	addl	$4,%esp
	movl	L_ARG0,%edx		/ refetch lock pointer
	jmp	mu_doit

ENTRY(interlock_unlock)
	FRAME
	movl	L_ARG0,%edx

	xorb	%cl,%cl
	xchgb	%cl,M_ILK

	ENABLE_PREEMPTION()

	EMARF
	ret

ENTRY(_disable_preemption)
	DISABLE_PREEMPTION()

	ret

ENTRY(_enable_preemption)
	ENABLE_PREEMPTION()

	ret

ENTRY(i_bit_set)
	movl	S_ARG0,%edx
	movl	S_ARG1,%eax
	lock
	bts	%dl,(%eax)
	ret

ENTRY(i_bit_clear)
	movl	S_ARG0,%edx
	movl	S_ARG1,%eax
	lock
	btr	%dl,(%eax)
	ret

ENTRY(bit_lock)
	movl	S_ARG0,%ecx
	movl	S_ARG1,%eax
1:
	lock
	bts	%ecx,(%eax)
	jb	1b
	ret

ENTRY(bit_lock_try)
	movl	S_ARG0,%ecx
	movl	S_ARG1,%eax
	lock
	bts	%ecx,(%eax)
	jb	bit_lock_failed
	ret			/ %eax better not be null !
bit_lock_failed:
	xorl	%eax,%eax
	ret

ENTRY(bit_unlock)
	movl	S_ARG0,%ecx
	movl	S_ARG1,%eax
	lock
	btr	%ecx,(%eax)
	ret

