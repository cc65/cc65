/*****************************************************************************/
/*                                                                           */
/*				     nes.h			       	     */
/*                                                                           */
/*		        NES system specific definitions			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2003 Groepaz/Hitmen                                              */
/*                                                                           */
/*                                                                           */
/* This software is provided 'as-is', without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/



#ifndef _NES_H
#define _NES_H



/* Check for errors */
#if !defined(__NES__)
#  error This module may only be used when compiling for the NES!
#endif



/* Key and character defines */
#define CH_ENTER	        '\n'
#define CH_CURS_UP		0x01
#define CH_CURS_DOWN	        0x02
#define CH_CURS_LEFT	        0x03
#define CH_CURS_RIGHT	        0x04
#define CH_ESC		        8
#define CH_DEL		        20

#define CH_CROSS	        0x10
#define CH_RTEE                 0x17
#define CH_LTEE                 0x0f
#define CH_TTEE                 0x16
#define CH_BTEE                 0x15
#define CH_HLINE                0x0b
#define CH_VLINE                0x0e
#define CH_ULCORNER             0x14
#define CH_URCORNER             0x12
#define CH_LLCORNER             0x11
#define CH_LRCORNER             0x08
#define CH_PI                   0x05


/* Color defines */
#define COLOR_BLACK  	       	0x00
#define COLOR_WHITE  	       	0x01
#define COLOR_RED    	       	0x02
#define COLOR_CYAN      	0x03
#define COLOR_VIOLET 	       	0x04
#define COLOR_GREEN  	       	0x05
#define COLOR_BLUE   	       	0x06
#define COLOR_YELLOW 	       	0x07
#define COLOR_ORANGE 	       	0x08
#define COLOR_BROWN  	       	0x09
#define COLOR_LIGHTRED       	0x0A
#define COLOR_GRAY1  	       	0x0B
#define COLOR_GRAY2  	       	0x0C
#define COLOR_LIGHTGREEN     	0x0D
#define COLOR_LIGHTBLUE      	0x0E
#define COLOR_GRAY3  	       	0x0F

/* Return codes of get_tv */
#define TV_NTSC         0
#define TV_PAL          1
#define TV_OTHER        2



void __fastcall__ waitvblank (void);
/* Wait for the vertical blanking */

unsigned char __fastcall__ get_tv (void);
/* Return the video mode the machine is using. */



/* End of nes.h */
#endif



