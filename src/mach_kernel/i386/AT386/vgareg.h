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

#define	VGA_TEXT_SIZE			(80*25*2)
#define	VGA_FONT_SIZE			8192
#define	VGA_GRAPHICS_SIZE		(VGA_FONT_SIZE*4)

#define	VGA_FB_ADDR			0xa0000

#define VGA_MAXCLOCKS			32

#define MiscOutReg General[0]
typedef struct {
  unsigned char General[4];     	/* General Registers */
  unsigned char CRTC[25];       	/* Crtc Controller */
  unsigned char Sequencer[5];   	/* Video Sequencer */
  unsigned char Graphics[9];    	/* Video Graphics */
  unsigned char Attribute[21];  	/* Video Atribute */
  unsigned char DAC[768];       	/* Internal Colorlookuptable */
  unsigned char NoClock;        	/* number of selected clock */
  union {
    struct {
      unsigned char FontArea[VGA_FONT_SIZE];
					/* save area for fonts */
      unsigned char TextArea[VGA_TEXT_SIZE];
					/* save area for screen text */
    } text;
    unsigned char GraphicsArea[VGA_GRAPHICS_SIZE];
					/* save area for graphics */
  } fb;
} vgaHWRec, *vgaHWPtr;

#define	FontInfo	fb.text.FontArea
#define	TextInfo	fb.text.TextArea
#define	GraphicsInfo	fb.GraphicsArea

