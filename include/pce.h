/*****************************************************************************/
/*                                                                           */
/*                                   pce.h                                   */
/*                                                                           */
/*                   PC-Engine system-specific definitions                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2015, Groepaz/Hitmen                                                  */
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

#ifndef _PCE_H
#define _PCE_H

/* Check for errors */
#if !defined(__PCE__)
#  error This module may only be used when compiling for the PCE!
#endif

#define CH_HLINE                1
#define CH_VLINE                2
#define CH_CROSS                3
#define CH_ULCORNER             4
#define CH_URCORNER             5
#define CH_LLCORNER             6
#define CH_LRCORNER             7
#define CH_TTEE                 8
#define CH_BTEE                 9
#define CH_LTEE                 10
#define CH_RTEE                 11

#define CH_ENTER                13
#define CH_PI                   18

/* Color defines (CBM-compatible, for conio) */
#define COLOR_BLACK             0x00
#define COLOR_WHITE             0x01
#define COLOR_RED               0x02
#define COLOR_CYAN              0x03
#define COLOR_PURPLE            0x04
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

#define TV_NTSC                 0
#define TV_PAL                  1
#define TV_OTHER                2

/* Masks for joy_read */
#define JOY_UP_MASK             0x10
#define JOY_DOWN_MASK           0x40
#define JOY_LEFT_MASK           0x80
#define JOY_RIGHT_MASK          0x20
#define JOY_BTN_1_MASK          0x01
#define JOY_BTN_2_MASK          0x02
#define JOY_BTN_3_MASK          0x04
#define JOY_BTN_4_MASK          0x08

#define JOY_BTN_I_MASK          JOY_BTN_1_MASK
#define JOY_BTN_II_MASK         JOY_BTN_2_MASK
#define JOY_SELECT_MASK         JOY_BTN_3_MASK
#define JOY_RUN_MASK            JOY_BTN_4_MASK

#define JOY_BTN_I(v)            ((v) & JOY_BTN_I_MASK)
#define JOY_BTN_II(v)           ((v) & JOY_BTN_II_MASK)
#define JOY_SELECT(v)           ((v) & JOY_SELECT_MASK)
#define JOY_RUN(v)              ((v) & JOY_RUN_MASK)

/* No support for dynamically loadable drivers */
#define DYN_DRV                 0

/* The addresses of the static drivers */
extern void pce_stdjoy_joy[];   /* Referred to by joy_static_stddrv[] */

void waitvsync (void);
/* Wait for start of the next frame */

/* NOTE: all PCEs are NTSC. */
#define get_tv()                TV_NTSC
/* Return the video mode the machine is using. */

/* End of pce.h */
#endif
