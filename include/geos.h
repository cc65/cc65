/*
   Supreme GEOS header file
   includes all other headers

   Maciej 'YTM/Elysium' Witkowiak, 27.10.1999
*/



#ifndef _GEOS_H
#define _GEOS_H



/* Check for errors */
#if !defined(__GEOS__)
#  error This module may only be used when compiling for GEOS!
#endif



#include <geos/gconst.h>
#include <geos/gstruct.h>
#include <geos/gsym.h>
#include <geos/gdisk.h>
#include <geos/gfile.h>
#include <geos/gprocess.h>
#include <geos/ggraph.h>
#include <geos/gmenu.h>
#include <geos/gsprite.h>
#include <geos/gmemory.h>
#include <geos/gsys.h>
#include <geos/gdlgbox.h>



#define CH_ULCORNER             '+'
#define CH_URCORNER             '+'
#define CH_LLCORNER             '+'
#define CH_LRCORNER             '+'
#define CH_TTEE                 '+'
#define CH_RTEE                 '+'
#define CH_BTEE                 '+'
#define CH_LTEE                 '+'
#define CH_CROSS                '+'

#define CH_F1                   KEY_F1
#define CH_F2                   KEY_F2
#define CH_F3                   KEY_F3
#define CH_F4                   KEY_F4
#define CH_F5                   KEY_F5
#define CH_F6                   KEY_F6
#define CH_F7                   KEY_F7
#define CH_F8                   KEY_F8

#define CH_CURS_UP              KEY_UP
#define CH_CURS_DOWN            KEY_DOWN
#define CH_CURS_LEFT            KEY_LEFT
#define CH_CURS_RIGHT           KEY_RIGHT
#define CH_DEL                  KEY_DELETE
#define CH_INS                  KEY_INSERT
#define CH_ENTER                KEY_ENTER
#define CH_STOP                 KEY_STOP
#define CH_ESC                  KEY_ESC

#define COLOR_BLACK             BLACK
#define COLOR_WHITE             WHITE
#define COLOR_RED               RED
#define COLOR_CYAN              CYAN
#define COLOR_VIOLET            PURPLE
#define COLOR_PURPLE            PURPLE
#define COLOR_GREEN             GREEN
#define COLOR_BLUE              BLUE
#define COLOR_YELLOW            YELLOW
#define COLOR_ORANGE            ORANGE
#define COLOR_BROWN             BROWN
#define COLOR_LIGHTRED          LTRED
#define COLOR_GRAY1             DKGREY
#define COLOR_GRAY2             MEDGREY
#define COLOR_LIGHTGREEN        LTGREEN
#define COLOR_LIGHTBLUE         LTBLUE
#define COLOR_GRAY3             LTGREY

#define TGI_COLOR_BLACK         COLOR_BLACK
#define TGI_COLOR_WHITE         COLOR_WHITE
#define TGI_COLOR_RED           COLOR_RED
#define TGI_COLOR_CYAN          COLOR_CYAN
#define TGI_COLOR_VIOLET        COLOR_VIOLET
#define TGI_COLOR_PURPLE        COLOR_PURPLE
#define TGI_COLOR_GREEN         COLOR_GREEN
#define TGI_COLOR_BLUE          COLOR_BLUE
#define TGI_COLOR_YELLOW        COLOR_YELLOW
#define TGI_COLOR_ORANGE        COLOR_ORANGE
#define TGI_COLOR_BROWN         COLOR_BROWN
#define TGI_COLOR_LIGHTRED      COLOR_LIGHTRED
#define TGI_COLOR_GRAY1         COLOR_GRAY1
#define TGI_COLOR_GRAY2         COLOR_GRAY2
#define TGI_COLOR_LIGHTGREEN    COLOR_LIGHTGREEN
#define TGI_COLOR_LIGHTBLUE     COLOR_LIGHTBLUE
#define TGI_COLOR_GRAY3         COLOR_GRAY3

#define JOY_UP_MASK             0x01
#define JOY_DOWN_MASK           0x02
#define JOY_LEFT_MASK           0x04
#define JOY_RIGHT_MASK          0x08
#define JOY_BTN_1_MASK          0x10



/* End of geos.h */
#endif
