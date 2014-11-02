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

/* make GTIA color value */
#define _gtia_mkcolor(hue,lum) (((hue) << 4) | ((lum) << 1))

/* luminance values go from 0 (black) to 7 (white) */

/* hue values */
#define HUE_GREY        0
#define HUE_GOLD        1
#define HUE_GOLDORANGE  2
#define HUE_REDORANGE   3
#define HUE_ORANGE      4
#define HUE_MAGENTA     5
#define HUE_PURPLE      6
#define HUE_BLUE        7
#define HUE_BLUE2       8
#define HUE_CYAN        9
#define HUE_BLUEGREEN   10
#define HUE_BLUEGREEN2  11
#define HUE_GREEN       12
#define HUE_YELLOWGREEN 13
#define HUE_YELLOW      14
#define HUE_YELLOWRED   15

/* Color defines, similar to c64 colors (untested) */
#define COLOR_BLACK             _gtia_mkcolor(HUE_GREY,0)
#define COLOR_WHITE             _gtia_mkcolor(HUE_GREY,7)
#define COLOR_RED               _gtia_mkcolor(HUE_REDORANGE,1)
#define COLOR_CYAN              _gtia_mkcolor(HUE_CYAN,3)
#define COLOR_VIOLET            _gtia_mkcolor(HUE_PURPLE,4)
#define COLOR_GREEN             _gtia_mkcolor(HUE_GREEN,2)
#define COLOR_BLUE              _gtia_mkcolor(HUE_BLUE,2)
#define COLOR_YELLOW            _gtia_mkcolor(HUE_YELLOW,7)
#define COLOR_ORANGE            _gtia_mkcolor(HUE_ORANGE,5)
#define COLOR_BROWN             _gtia_mkcolor(HUE_YELLOW,2)
#define COLOR_LIGHTRED          _gtia_mkcolor(HUE_REDORANGE,6)
#define COLOR_GRAY1             _gtia_mkcolor(HUE_GREY,2)
#define COLOR_GRAY2             _gtia_mkcolor(HUE_GREY,3)
#define COLOR_LIGHTGREEN        _gtia_mkcolor(HUE_GREEN,6)
#define COLOR_LIGHTBLUE         _gtia_mkcolor(HUE_BLUE,6)
#define COLOR_GRAY3             _gtia_mkcolor(HUE_GREY,5)

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
#endif /* #ifndef _ATARI5200_H */
