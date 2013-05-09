/*****************************************************************************/
/*                                                                           */
/*                                   vic20.h                                 */
/*                                                                           */
/*                      vic20 system specific definitions                    */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2004 Ullrich von Bassewitz                                       */
/*               Römerstraße 52                                              */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
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



#ifndef _VIC20_H
#define _VIC20_H



/* Check for errors */
#if !defined(__VIC20__)
#  error This module may only be used when compiling for the Vic20!
#endif



/* Additional key defines */
#define CH_F1                   133
#define CH_F2                   137
#define CH_F3                   134
#define CH_F4                   138
#define CH_F5                   135
#define CH_F6                   139
#define CH_F7                   136
#define CH_F8                   140



/* Color defines */
#define COLOR_BLACK             0x00
#define COLOR_WHITE             0x01
#define COLOR_RED               0x02
#define COLOR_CYAN              0x03
#define COLOR_VIOLET            0x04
#define COLOR_GREEN             0x05
#define COLOR_BLUE              0x06
#define COLOR_YELLOW            0x07
#define COLOR_ORANGE            0x08
#define COLOR_BROWN             0x09
#define COLOR_LIGHTRED          0x0A
#define COLOR_GRAY1             0x0B
#define COLOR_GRAY2             0x0C
#define COLOR_LIGHTGREEN        0x0D
#define COLOR_LIGHTBLUE         0x0E
#define COLOR_GRAY3             0x0F



/* Define hardware */
#include <_vic.h>
#define VIC     (*(struct __vic*)0x9000)

#include <_6522.h>
#define VIA1    (*(struct __6522*)0x9110)
#define VIA2    (*(struct __6522*)0x9120)



/* Define special memory areas */
#define COLOR_RAM       ((unsigned char*)0x9600)



/* The addresses of the static drivers */
extern void vic20_ptvjoy_joy[];
extern void vic20_stdjoy_joy[];         /* Referred to by joy_static_stddrv[] */



/* End of vic20.h */
#endif
