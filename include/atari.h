/*****************************************************************************/
/*                                                                           */
/*				   atari.h			       	     */
/*                                                                           */
/*		        Atari system specific definitions		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000 Mark Keates <markk@dendrite.co.uk>                               */
/*          Freddy Offenga <taf_offenga@yahoo.com>                           */
/*          Christian Groessler <cpg@aladdin.de>                             */
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

#ifndef _ATARI_H
#define _ATARI_H

/* Character codes */
#define CH_DEL 	    	0xFE
#define CH_ESC 	    	0x1B
#define CH_CURS_UP  	28
#define CH_CURS_DOWN 	29
#define CH_CURS_LEFT    30
#define CH_CURS_RIGHT   31

#define CH_TAB          0x7F   /* tabulator */
#define CH_EOL          0x9B   /* end-of-line marker */
#define CH_CLR          0x7D   /* clear screen */
#define CH_BEL          0xFD   /* bell */
#define CH_RUBOUT       0x7E   /* back space (rubout) */
#define CH_DELLINE      0x9C   /* delete line */
#define CH_INSLINE      0x9D   /* insert line */

/* These are defined to be Atari + NumberKey */
#define CH_F1 	    	177
#define CH_F2 	    	178
#define CH_F3 	    	179
#define CH_F4 	    	180
#define CH_F5 	    	181
#define CH_F6 	    	182
#define CH_F7 	    	183
#define CH_F8 	    	184
#define CH_F9 	    	185
#define CH_F10 	    	176

#define CH_ULCORNER 	0x11
#define CH_URCORNER 	0x05
#define CH_LLCORNER 	0x1A
#define CH_LRCORNER 	0x03
#define CH_TTEE     	0x17
#define CH_BTEE     	0x18
#define CH_LTEE     	0x01
#define CH_RTEE     	0x04
#define CH_CROSS    	0x19
#define CH_HLINE        0x12
#define CH_VLINE        0x16

/* color defines */

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
#define COLOR_BLACK  	       	_gtia_mkcolor(HUE_GREY,0)
#define COLOR_WHITE  	       	_gtia_mkcolor(HUE_GREY,7)
#define COLOR_RED    	       	_gtia_mkcolor(HUE_REDORANGE,1)
#define COLOR_CYAN      	_gtia_mkcolor(HUE_CYAN,3)
#define COLOR_VIOLET 	       	_gtia_mkcolor(HUE_PURPLE,4)
#define COLOR_GREEN  	        _gtia_mkcolor(HUE_GREEN,2)
#define COLOR_BLUE   	       	_gtia_mkcolor(HUE_BLUE,2)
#define COLOR_YELLOW 	       	_gtia_mkcolor(HUE_YELLOW,7)
#define COLOR_ORANGE 	       	_gtia_mkcolor(HUE_ORANGE,5)
#define COLOR_BROWN  	       	_gtia_mkcolor(HUE_YELLOW,2)
#define COLOR_LIGHTRED       	_gtia_mkcolor(HUE_REDORANGE,6)
#define COLOR_GRAY1  	       	_gtia_mkcolor(HUE_GREY,2)
#define COLOR_GRAY2  	       	_gtia_mkcolor(HUE_GREY,3)
#define COLOR_LIGHTGREEN     	_gtia_mkcolor(HUE_GREEN,6)
#define COLOR_LIGHTBLUE      	_gtia_mkcolor(HUE_BLUE,6)
#define COLOR_GRAY3  	       	_gtia_mkcolor(HUE_GREY,5)

/* color register functions */
extern void __fastcall__ setcolor     (unsigned char color_reg, unsigned char hue, unsigned char luminace);
extern void __fastcall__ setcolor_low (unsigned char color_reg, unsigned char color_value);
extern unsigned char __fastcall__ getcolor (unsigned char color_reg);

/* other screen functions */
extern void __fastcall__ scroll (signed char numlines);
                                          /* numlines > 0  scrolls up */
                                          /* numlines < 0  scrolls down */

/* misc. functions */
extern void save_vecs(void);          /* save system vectors */
extern void rest_vecs(void);          /* restore system vectors */
extern unsigned int get_ostype(void); /* get ROM version */

/* get_ostype return value defines (for explanation, see ostype.s) */
/* masks */
#define AT_OS_TYPE_MAIN  7
#define AT_OS_PALNTSC    (3 << 3)
#define AT_OS_TYPE_MINOR (7 << 5)
/* AT_OS_TYPE_MAIN values */
#define AT_OS_UNKNOWN  0
#define AT_OS_400800   1
#define AT_OS_1200XL   2
#define AT_OS_XLXE     3
/* AT_OS_PALNTSC values */
#define AT_OS_PAL      1
#define AT_OS_NTSC     2
/* AS_OS_TYPE_MINOR values */
#define AT_OS_400800_A 1
#define AT_OS_400800_B 2
#define AT_OS_1200_10  1
#define AT_OS_1200_11  2
#define AT_OS_XLXE_1   1
#define AT_OS_XLXE_2   2
#define AT_OS_XLXE_3   3
#define AT_OS_XLXE_4   4

/* Define hardware */
#include <_gtia.h>
#define GTIA (*(struct __gtia_write*)0xD000)
#define GTIA (*(struct __gtia_read*)0xD000)

#include <_pbi.h>

#include <_pokey.h>
#define POKEY (*(struct __pokey_write*)0xD200)
#define POKEY (*(struct __pokey_read*)0xD200)

#include <_pia.h>
#define PIA (*(struct __pia*)0xD300)

#include <_antic.h>
#define ANTIC (*(struct __antic*)0xD400)

/* End of atari.h */
#endif /* #ifndef _ATARI_H */
