/*****************************************************************************/
/*                                                                           */
/*				   apple2.h                                  */
/*                                                                           */
/*		     Apple ][ system specific definitions                    */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000  Kevin Ruland, <kevin@rodin.wustl.edu>                           */
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



#ifndef _APPLE2_H
#define _APPLE2_H



/* Check for errors */
#if !defined(__APPLE2__)
#  error This module may only be used when compiling for the Apple ][!
#endif



/* Color Defines
 * Since Apple2 does not support color text these defines are only
 * used to get the library to compile correctly.  They should not be used
 * in user code
 */
#define COLOR_BLACK 	0x00
#define COLOR_WHITE 	0x01



/* Characters codes */
#define CH_DEL 	    	0x7F
#define CH_ESC 	    	0x1B
#define CH_CURS_UP  	0x0B
#define CH_CURS_DOWN 	0x0A

/* These are defined to be OpenApple + NumberKey */
#define CH_F1 	    	0xB1
#define CH_F2 	    	0xB2
#define CH_F3 	    	0xB3
#define CH_F4 	    	0xB4
#define CH_F5 	    	0xB5
#define CH_F6 	    	0xB6
#define CH_F7 	    	0xB7
#define CH_F8 	    	0xB8
#define CH_F9 	    	0xB9
#define CH_F10 	    	0xB0

#define CH_ULCORNER 	'+'
#define CH_URCORNER 	'+'
#define CH_LLCORNER 	'+'
#define CH_LRCORNER 	'+'
#define CH_TTEE     	'+'
#define CH_BTEE     	'+'
#define CH_LTEE     	'+'
#define CH_RTEE     	'+'
#define CH_CROSS    	'+'



/* End of apple2.h */
#endif



