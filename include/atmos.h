/*****************************************************************************/
/*                                                                           */
/*				   atmos.h                                   */
/*                                                                           */
/*		     Oric Atmos system specific definitions                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002       Debrune Jérome, <jede@oric.org>                            */
/* (C) 2003-2004  Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
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



#ifndef _ATMOS_H
#define _ATMOS_H



/* Check for errors */
#if !defined(__ATMOS__)
#  error This module may only be used when compiling for the Oric Atmos!
#endif



/* Color defines */
#define COLOR_BLACK 	0x00
#define COLOR_RED 	0x01
#define COLOR_GREEN 	0x02
#define COLOR_YELLOW 	0x03
#define COLOR_BLUE 	0x04
#define COLOR_MAGENTA 	0x05
#define COLOR_CYAN 	0x06
#define COLOR_WHITE 	0x07



/* Define hardware */
#include <_6522.h>
#define VIA    	(*(struct __6522*)0x300)



/* These are defined to be FUNCT + NumberKey */
#define CH_F1		0xB1
#define CH_F2		0xB2
#define CH_F3		0xB3
#define CH_F4		0xB4
#define CH_F5		0xB5
#define CH_F6		0xB6
#define CH_F7		0xB7
#define CH_F8		0xB8
#define CH_F9		0xB9
#define CH_F10		0xB0



/* Character codes */
#define CH_ULCORNER 	'+'
#define CH_URCORNER 	'+'
#define CH_LLCORNER 	'+'
#define CH_LRCORNER 	'+'
#define CH_TTEE     	'+'
#define CH_BTEE     	'+'
#define CH_LTEE     	'+'
#define CH_RTEE     	'+'
#define CH_CROSS    	'+'



/* End of atmos.h */
#endif



