/*****************************************************************************/
/*                                                                           */
/*                                   vic20.h                                 */
/*                                                                           */
/*                     VIC-20 system-specific definitions                    */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2004 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
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
/* Only the background and multi-color characters can have these colors. */
#define COLOR_ORANGE            0x08
#define COLOR_LIGHTORANGE       0x09
#define COLOR_PINK              0x0A
#define COLOR_LIGHTCYAN         0x0B
#define COLOR_LIGHTVIOLET       0x0C
#define COLOR_LIGHTGREEN        0x0D
#define COLOR_LIGHTBLUE         0x0E
#define COLOR_LIGHTYELLOW       0x0F

/* TGI color defines */
#define TGI_COLOR_BLACK         COLOR_BLACK
#define TGI_COLOR_WHITE         COLOR_WHITE
#define TGI_COLOR_RED           COLOR_RED
#define TGI_COLOR_CYAN          COLOR_CYAN
#define TGI_COLOR_VIOLET        COLOR_VIOLET
#define TGI_COLOR_GREEN         COLOR_GREEN
#define TGI_COLOR_BLUE          COLOR_BLUE
#define TGI_COLOR_YELLOW        COLOR_YELLOW
/* Only the background and multi-color graphics can have these colors. */
#define TGI_COLOR_ORANGE        COLOR_ORANGE
#define TGI_COLOR_LIGHTORANGE   COLOR_LIGHTORANGE
#define TGI_COLOR_PINK          COLOR_PINK
#define TGI_COLOR_LIGHTCYAN     COLOR_LIGHTCYAN
#define TGI_COLOR_LIGHTVIOLET   COLOR_LIGHTVIOLET
#define TGI_COLOR_LIGHTGREEN    COLOR_LIGHTGREEN
#define TGI_COLOR_LIGHTBLUE     COLOR_LIGHTBLUE
#define TGI_COLOR_LIGHTYELLOW   COLOR_LIGHTYELLOW



/* tgi_ioctl() commands */
#define TGI_IOCTL_VIC20_SET_PATTERN     0x01    /* Set 8-byte pattern for tgi_bar(). */



/* Masks for joy_read */
#define JOY_UP_MASK             0x01
#define JOY_DOWN_MASK           0x02
#define JOY_LEFT_MASK           0x04
#define JOY_RIGHT_MASK          0x08
#define JOY_BTN_1_MASK          0x10



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

extern void vic20_rama_emd[];
extern void vic20_georam_emd[];

extern void vic20_hi_tgi[];             /* Referred to by tgi_static_stddrv[] */



/* End of vic20.h */
#endif
