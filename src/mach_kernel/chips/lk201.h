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
 * Revision 2.7  91/06/19  11:53:50  rvb
 * 	File moved here from mips/PMAX since it tries to be generic;
 * 	it is used on the PMAX and the Vax3100.
 * 	[91/06/04            rvb]
 * 
 * Revision 2.6  91/05/14  17:23:50  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/03/16  14:54:12  rpd
 * 	Fixed ioctl definitions for ANSI C.
 * 	[91/02/20            rpd]
 * 
 * Revision 2.4  91/02/05  17:42:19  mrt
 * 	Added author notices
 * 	[91/02/04  11:14:42  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:13:17  mrt]
 * 
 * Revision 2.3  90/12/05  23:32:09  af
 * 	Cleanups.
 * 	[90/12/03  23:26:51  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:42:54  af
 * 	Created, from DEC specs:
 * 	"VCB02 Video Subsystem Technical Manual"
 * 	DEC Educational Services, AZ-GLGAB-MN Feb 1986
 * 	[90/10/10            af]
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
 *	File: lk201.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	9/90
 *
 *	Definitions for the LK201 Keyboard Driver
 */

/*
 *	(labels as in the lk201-AA USA model)
 */

#define	LK_MINCODE	0x56
							/* Function keys */
#define	LK_F1		0x56		/* div 10 */
#define	LK_F2		0x57
#define	LK_F3		0x58
#define	LK_F4		0x59
#define	LK_F5		0x5a
#define	LK_F6		0x64		/* div 11 */
#define	LK_F7		0x65
#define	LK_F8		0x66
#define	LK_F9		0x67
#define	LK_F10		0x68
#define	LK_F11		0x71		/* div 12 */
#define	LK_ESC		LK_F11
#define	LK_F12		0x72
#define	LK_BS		LK_F12
#define	LK_F13		0x73
#define	LK_LF		LK_F13
#define	LK_F14		0x74
#define	LK_HELP		0x7c		/* div 13 */
#define	LK_DO		0x7d
#define	LK_F17		0x80		/* div 14 */
#define	LK_F18		0x81
#define	LK_F19		0x82
#define	LK_F20		0x83
							/* Editing keys */
#define	LK_FIND		0x8a		/* div 9 */
#define	LK_INSERT	0x8b
#define	LK_REMOVE	0x8c
#define	LK_SELECT	0x8d
#define	LK_PREV_SCREEN	0x8e
#define	LK_NEXT_SCREEN	0x8f
							/* Numeric keypad */
#define	LK_KP_0		0x92		/* div 2 */
#define	LK_KP_DOT	0x94
#define	LK_KP_ENTER	0x95
#define	LK_KP_1		0x96
#define	LK_KP_2		0x97
#define	LK_KP_3		0x98
#define	LK_KP_4		0x99
#define	LK_KP_5		0x9a
#define	LK_KP_6		0x9b
#define	LK_KP_COMMA	0x9c
#define	LK_KP_7		0x9d
#define	LK_KP_8		0x9e
#define	LK_KP_9		0x9f
#define	LK_KP_MINUS	0xa0
#define	LK_KP_PF1	0xa1
#define	LK_KP_PF2	0xa2
#define	LK_KP_PF3	0xa3
#define	LK_KP_PF4	0xa4
							/* Cursor keys */
#define	LK_LEFT		0xa7		/* div 7 */
#define	LK_RIGHT	0xa8
#define	LK_DOWN		0xa9		/* div 8 */
#define	LK_UP		0xaa
							/* Shift & Co. */
#define	LK_R_SHIFT	0xab		/* lk401 */
#define	LK_L_ALT	0xac		/* lk401 */
#define	LK_R_COMPOSE	0xad		/* lk401 */
#define	LK_SHIFT	0xae		/* div 6 */
#define	LK_CNTRL	0xaf
#define	LK_LOCK		0xb0		/* div 5 */
#define	LK_ALT		0xb1
#define	LK_R_ALT	0xb2		/* lk401 */
							/* Special codes */
#define	LK_ALLUP	0xb3
#define	LK_REPEAT	0xb4
#define	LK_OUTPUT_ERR	0xb5
#define	LK_INPUT_ERR	0xb6
#define	LK_KDBLOCK_ACK	0xb7
#define	LK_TESTMODE_ACK	0xb8
#define	LK_PFX_KEYDOWN	0xb9
#define	LK_MODECHG_ACK	0xba
							/* Delete & Co. */
#define	LK_DEL		0xbc		/* div 3 */
#define	LK_RETURN	0xbd		/* div 4 */
#define	LK_TAB		0xbe
							/* Graphic keys */

#define	LK_TILDE	0xbf		/* div 2 */
#define	LK_1		0xc0
#define	LK_Q		0xc1
#define	LK_A		0xc2
#define	LK_Z		0xc3
#define	LK_2		0xc5
#define	LK_W		0xc6
#define	LK_S		0xc7
#define	LK_X		0xc8
#define	LK_LESSGRT	0xc9
#define	LK_3		0xcb
#define	LK_E		0xcc
#define	LK_D		0xcd
#define	LK_C		0xce
#define	LK_4		0xd0
#define	LK_R		0xd1
#define	LK_F		0xd2
#define	LK_V		0xd3
#define	LK_SP		0xd4
#define	LK_5		0xd6
#define	LK_T		0xd7
#define	LK_G		0xd8
#define	LK_B		0xd9
#define	LK_6		0xdb
#define	LK_Y		0xdc
#define	LK_H		0xdd
#define	LK_N		0xde
#define	LK_7		0xe0		/* div 1 */
#define	LK_U		0xe1
#define	LK_J		0xe2
#define	LK_M		0xe3
#define	LK_8		0xe5
#define	LK_I		0xe6
#define	LK_K		0xe7
#define	LK_DQUOTE	0xe8
#define	LK_9		0xea
#define	LK_O		0xeb
#define	LK_L		0xec
#define	LK_DOT		0xed
#define	LK_0		0xef
#define	LK_P		0xf0
#define	LK_COLON	0xf2
#define	LK_QMARK	0xf3
#define	LK_PLUS		0xf5
#define	LK_RBRACKET	0xf6
#define	LK_VBAR		0xf7
#define	LK_MINUS	0xf9
#define	LK_LBRACKET	0xfa
#define	LK_COMMA	0xfb


/*
 *	Commands to the keyboard processor
 */

#define	LK_PARAM		0x80	/* start/end parameter list */

#define	LK_CMD_RESUME		0x8b
#define	LK_CMD_INHIBIT		0xb9
#define	LK_CMD_LEDS_ON		0x13	/* 1 param: led bitmask */
#define	LK_CMD_LEDS_OFF		0x11	/* 1 param: led bitmask */
#define	LK_CMD_DIS_KEYCLK	0x99
#define	LK_CMD_ENB_KEYCLK	0x1b	/* 1 param: volume */
#define	LK_CMD_DIS_CTLCLK	0xb9
#define	LK_CMD_ENB_CTLCLK	0xbb
#define	LK_CMD_SOUND_CLK	0x9f
#define	LK_CMD_DIS_BELL		0xa1
#define	LK_CMD_ENB_BELL		0x23	/* 1 param: volume */
#define	LK_CMD_BELL		0xa7
#define	LK_CMD_TMP_NORPT	0xc1
#define	LK_CMD_ENB_RPT		0xe3
#define	LK_CMD_DIS_RPT		0xe1
#define	LK_CMD_RPT_TO_DOWN	0xd9
#define	LK_CMD_REQ_ID		0xab
#define	LK_CMD_POWER_UP		0xfd
#define	LK_CMD_TEST_MODE	0xcb
#define	LK_CMD_SET_DEFAULTS	0xd3

/* there are 4 leds, represent them in the low 4 bits of a byte */
#define	LK_PARAM_LED_MASK(ledbmap)	(LK_PARAM|(ledbmap))

/* max volume is 0, lowest is 0x7 */
#define	LK_PARAM_VOLUME(v)		(LK_PARAM|((v)&0x7))

/* mode set command(s) details */
#define	LK_MODE_DOWN		0x0
#define	LK_MODE_RPT_DOWN	0x2
#define	LK_MODE_DOWN_UP		0x6
#define	LK_CMD_MODE(m,div)	(LK_PARAM|(div<<3)|m)


/*
 *	Keyboard set_status codes and arguments
 */

/* Send a command packet to the lk201 */
typedef struct {
	unsigned char	len;				/* how many params */
	unsigned char	command;			/* cmd to lk201 */
	unsigned char	params[2];			/* extra params */
} lk201_cmd_t;
#define LK201_SEND_CMD		_IOW('q', 5, lk201_cmd_t)/* keybd. per. cmd */

/* Control rotation of lk201 leds */
#define LK201_LED_PATTERN	_IOW('q', 119, int)	/* cylon, ... */
#	define LED_COUNT	1			/* led counter */
#	define LED_ROTATE	2			/* led rotate */
#	define LED_CYLON	3			/* led cylon mode */

#define LK201_LED_INTERVAL	_IOW('q', 120, int)	/* refresh interval */
#	define LED_OFF		0			/* no led movement */

/* Map the caps-lock key to act as the control key (skinny-fingers) */
#define	LK201_mapLOCKtoCNTRL	_IOW('q', 121, int)	/* 1 or 0 */

/* Remap a lk201 keycode to a different (ASCII) translation */
typedef struct {
	unsigned char in_keyval;
	unsigned char shifted;
	unsigned char out_keyval;
	unsigned char meta;
} KeyMap;
#define	LK201_REMAP_KEY		 _IOW('q', 122, KeyMap)	/* 1 or 0 */
