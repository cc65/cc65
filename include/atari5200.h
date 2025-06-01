/*****************************************************************************/
/*                                                                           */
/*                               atari5200.h                                 */
/*                                                                           */
/*                   Atari 5200 system specific definitions                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2014      Christian Groessler <chris@groessler.org>                   */
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



#ifndef _ATARI5200_H
#define _ATARI5200_H



/* check for errors */
#if !defined(__ATARI5200__)
#  error This module may only be used when compiling for the Atari 5200!
#endif

/* no support for dynamically loadable drivers */
#define DYN_DRV 0

/* the addresses of the static drivers */
extern void atr5200std_joy[];        /* referred to by joy_static_stddrv[] */

/* masks for joy_read */
#define JOY_UP_MASK     0x01
#define JOY_DOWN_MASK   0x02
#define JOY_LEFT_MASK   0x04
#define JOY_RIGHT_MASK  0x08
#define JOY_BTN_1_MASK  0x10

/* character codes */
#define CH_ULCORNER     0x0B         /* '+' sign */
#define CH_URCORNER     0x0B
#define CH_LLCORNER     0x0B
#define CH_LRCORNER     0x0B
#define CH_HLINE        0x0D         /* dash */
#define CH_VLINE        0x01         /* exclamation mark */

/* get_tv return values */
#define AT_NTSC     0
#define AT_PAL      1

/* Define variables used by the OS*/
#include <_atari5200os.h>
#define OS (*(struct __os*)0x0000)

/* define hardware */
#include <_gtia.h>
#define GTIA_READ  (*(struct __gtia_read*)0xC000)
#define GTIA_WRITE (*(struct __gtia_write*)0xC000)

#include <_pokey.h>
#define POKEY_READ  (*(struct __pokey_read*)0xE800)
#define POKEY_WRITE (*(struct __pokey_write*)0xE800)

#include <_antic.h>
#define ANTIC (*(struct __antic*)0xD400)

/* conio color defines */
#define COLOR_WHITE 0x00
#define COLOR_RED   0x01
#define COLOR_GREEN 0x02
#define COLOR_BLACK 0x03

/* The following #define will cause the matching function calls in conio.h
** to be overlaid by macros with the same names, saving the function call
** overhead.
*/
#define _bordercolor(color) 0

/* wait for start of next frame */
void waitvsync (void);

/* end of atari5200.h */
#endif
