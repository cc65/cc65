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



/* Check for errors */
#if !defined(__ATARI5200__)
#  error This module may only be used when compiling for the Atari 5200!
#endif

/* no support for dynamically loadable drivers */
#define DYN_DRV 0

/* the addresses of the static drivers */
extern void atr5200std_joy[];        /* referred to by joy_static_stddrv[] */

/* Masks for joy_read */
#define JOY_UP_MASK     0x01
#define JOY_DOWN_MASK   0x02
#define JOY_LEFT_MASK   0x04
#define JOY_RIGHT_MASK  0x08
#define JOY_BTN_1_MASK  0x10

/* get_tv return values */
#define AT_NTSC     0
#define AT_PAL      1

/* Define hardware */
#include <_gtia.h>
#define GTIA_READ  (*(struct __gtia_read*)0xC000)
#define GTIA_WRITE (*(struct __gtia_write*)0xC000)

#include <_pokey.h>
#define POKEY_READ  (*(struct __pokey_read*)0xE800)
#define POKEY_WRITE (*(struct __pokey_write*)0xE800)

#include <_antic.h>
#define ANTIC (*(struct __antic*)0xD400)

/* End of atari5200.h */
#endif
