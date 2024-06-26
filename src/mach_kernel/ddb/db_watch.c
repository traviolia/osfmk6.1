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
 * Revision 2.7  91/10/09  16:04:32  af
 * 	 Revision 2.6.3.1  91/10/05  13:08:50  jeffreyh
 * 	 	Added user space watch point support including non current task.
 * 	 	Changed "map" field of db_watchpoint structure to "task"
 * 	 	  for a user to easily understand the target space.
 * 	 	[91/08/29            tak]
 * 
 * Revision 2.6.3.1  91/10/05  13:08:50  jeffreyh
 * 	Added user space watch point support including non current task.
 * 	Changed "map" field of db_watchpoint structure to "task"
 * 	  for a user to easily understand the target space.
 * 	[91/08/29            tak]
 * 
 * Revision 2.6  91/05/14  15:37:30  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/02/05  17:07:27  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  16:20:02  mrt]
 * 
 * Revision 2.4  91/01/08  15:09:24  rpd
 * 	Use db_map_equal, db_map_current, db_map_addr.
 * 	[90/11/10            rpd]
 * 
 * Revision 2.3  90/11/05  14:26:39  rpd
 * 	Initialize db_watchpoints_inserted to TRUE.
 * 	[90/11/04            rpd]
 * 
 * Revision 2.2  90/10/25  14:44:16  rwd
 * 	Made db_watchpoint_cmd parse a size argument.
 * 	[90/10/17            rpd]
 * 	Generalized the watchpoint support.
 * 	[90/10/16            rwd]
 * 	Created.
 * 	[90/10/16            rpd]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
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
 * 	Author: Richard P. Draves, Carnegie Mellon University
 *	Date:	10/90
 */

#include <mach/boolean.h>
#include <mach/vm_param.h>
#include <mach/machine/vm_types.h>
#include <mach/machine/vm_param.h>
#include <vm/vm_map.h>

#include <machine/db_machdep.h>
#include <ddb/db_lex.h>
#include <ddb/db_watch.h>
#include <ddb/db_access.h>
#include <ddb/db_sym.h>
#include <ddb/db_task_thread.h>
#include <ddb/db_command.h>
#include <ddb/db_expr.h>
#include <ddb/db_output.h>		/* For db_printf() */
#include <ddb/db_run.h>			/* For db_single_step() */

/*
 * Watchpoints.
 */

boolean_t	db_watchpoints_inserted = TRUE;

#define	NWATCHPOINTS	100
struct db_watchpoint	db_watch_table[NWATCHPOINTS];
db_watchpoint_t		db_next_free_watchpoint = &db_watch_table[0];
db_watchpoint_t		db_free_watchpoints = 0;
db_watchpoint_t		db_watchpoint_list = 0;

extern vm_map_t		kernel_map;



/* Prototypes for functions local to this file.  XXX -- should be static.
 */

db_watchpoint_t db_watchpoint_alloc(void);

void db_watchpoint_free(register db_watchpoint_t watch);

void db_set_watchpoint(
	task_t		task,
	db_addr_t	addr,
	vm_size_t	size);

void db_delete_watchpoint(
	task_t		task,
	db_addr_t	addr);

static int db_get_task(
	char		*modif,
	task_t		*taskp,
	db_addr_t	addr);

void db_list_watchpoints(void);



db_watchpoint_t
db_watchpoint_alloc(void)
{
	register db_watchpoint_t	watch;

	if ((watch = db_free_watchpoints) != 0) {
	    db_free_watchpoints = watch->link;
	    return (watch);
	}
	if (db_next_free_watchpoint == &db_watch_table[NWATCHPOINTS]) {
	    db_printf("All watchpoints used.\n");
	    return (0);
	}
	watch = db_next_free_watchpoint;
	db_next_free_watchpoint++;

	return (watch);
}

void
db_watchpoint_free(register db_watchpoint_t watch)
{
	watch->link = db_free_watchpoints;
	db_free_watchpoints = watch;
}

void
db_set_watchpoint(
	task_t		task,
	db_addr_t	addr,
	vm_size_t	size)
{
	register db_watchpoint_t	watch;

	/*
	 *	Should we do anything fancy with overlapping regions?
	 */

	for (watch = db_watchpoint_list; watch != 0; watch = watch->link) {
	    if (watch->task == task &&
		(watch->loaddr == addr) &&
		(watch->hiaddr == addr+size)) {
		db_printf("Already set.\n");
		return;
	    }
	}

	watch = db_watchpoint_alloc();
	if (watch == 0) {
	    db_printf("Too many watchpoints.\n");
	    return;
	}

	watch->task = task;
	watch->loaddr = addr;
	watch->hiaddr = addr+size;

	watch->link = db_watchpoint_list;
	db_watchpoint_list = watch;

	db_watchpoints_inserted = FALSE;
}

void
db_delete_watchpoint(
	task_t		task,
	db_addr_t	addr)
{
	register db_watchpoint_t	watch;
	register db_watchpoint_t	*prev;

	for (prev = &db_watchpoint_list; (watch = *prev) != 0;
	     prev = &watch->link) {
	    if (watch->task == task &&
		(watch->loaddr <= addr) &&
		(addr < watch->hiaddr)) {
		*prev = watch->link;
		db_watchpoint_free(watch);
		return;
	    }
	}

	db_printf("Not set.\n");
}

void
db_list_watchpoints(void)
{
	register db_watchpoint_t watch;
	int	 task_id;

	if (db_watchpoint_list == 0) {
	    db_printf("No watchpoints set\n");
	    return;
	}

	db_printf("Space      Address  Size\n");
	for (watch = db_watchpoint_list; watch != 0; watch = watch->link)  {
	    if (watch->task == TASK_NULL)
		db_printf("kernel  ");
	    else {
		task_id = db_lookup_task(watch->task);
		if (task_id < 0)
		    db_printf("%08x", watch->task);
		else
		    db_printf("task%-3d ", task_id);
	    }
	    db_printf("  %8x  %x\n", watch->loaddr,
		      watch->hiaddr - watch->loaddr);
	}
}

static int
db_get_task(
	char		*modif,
	task_t		*taskp,
	db_addr_t	addr)
{
	task_t		task = TASK_NULL;
	db_expr_t	value;
	boolean_t	user_space;

	user_space = db_option(modif, 'T');
	if (user_space) {
	    if (db_expression(&value)) {
		task = (task_t)value;
		if (db_lookup_task(task) < 0) {
		    db_printf("bad task address %x\n", task);
		    return(-1);
		}
	    } else {
		task = db_default_task;
		if (task == TASK_NULL) {
		    if ((task = db_current_task()) == TASK_NULL) {
			db_printf("no task\n");
			return(-1);
		    }
		}
	    }
	}
	if (!DB_VALID_ADDRESS(addr, user_space)) {
	    db_printf("Address 0x%x is not in %s space\n", addr, 
			(user_space)? "user": "kernel");
	    return(-1);
	}
	*taskp = task;
	return(0);
}

/* Delete watchpoint */
void
db_deletewatch_cmd(
	db_expr_t	addr,
	int		have_addr,
	db_expr_t	count,
	char *		modif)
{
	task_t		task;

	if (db_get_task(modif, &task, addr) < 0)
	    return;
	db_delete_watchpoint(task, addr);
}

/* Set watchpoint */
void
db_watchpoint_cmd(
	db_expr_t	addr,
	int		have_addr,
	db_expr_t	count,
	char *		modif)
{
	vm_size_t	size;
	db_expr_t	value;
	task_t		task;

	if (db_get_task(modif, &task, addr) < 0)
	    return;
	if (db_expression(&value))
	    size = (vm_size_t) value;
	else
	    size = 4;
	db_set_watchpoint(task, addr, size);
}

/* list watchpoints */
void
db_listwatch_cmd(void)
{
	db_list_watchpoints();
}

void
db_set_watchpoints(void)
{
	register db_watchpoint_t	watch;
	vm_map_t			map;

	if (!db_watchpoints_inserted) {
	    for (watch = db_watchpoint_list; watch != 0; watch = watch->link) {
		map = (watch->task)? watch->task->map: kernel_map;
		pmap_protect(map->pmap,
			     trunc_page(watch->loaddr),
			     round_page(watch->hiaddr),
			     VM_PROT_READ);
	    }
	    db_watchpoints_inserted = TRUE;
	}
}

void
db_clear_watchpoints(void)
{
	db_watchpoints_inserted = FALSE;
}

boolean_t
db_find_watchpoint(
	vm_map_t	map,
	db_addr_t	addr,
	db_regs_t	*regs)
{
	register db_watchpoint_t watch;
	db_watchpoint_t found = 0;
	register task_t	task_space;

	task_space = (vm_map_pmap(map) == kernel_pmap)?
		TASK_NULL: db_current_space();
	for (watch = db_watchpoint_list; watch != 0; watch = watch->link) {
	    if (watch->task == task_space) {
		if ((watch->loaddr <= addr) && (addr < watch->hiaddr))
		    return (TRUE);
		else if ((trunc_page(watch->loaddr) <= addr) &&
			 (addr < round_page(watch->hiaddr)))
		    found = watch;
	    }
	}

	/*
	 *	We didn't hit exactly on a watchpoint, but we are
	 *	in a protected region.  We want to single-step
	 *	and then re-protect.
	 */

	if (found) {
	    db_watchpoints_inserted = FALSE;
	    db_single_step(regs, task_space);
	}

	return (FALSE);
}
