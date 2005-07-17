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



#ifndef _GCONST_H
#include <geos/gconst.h>
#endif

#ifndef _GSTRUCT_H
#include <geos/gstruct.h>
#endif

#ifndef _GSYM_H
#include <geos/gsym.h>
#endif

#ifndef _GDISK_H
#include <geos/gdisk.h>
#endif

#ifndef _GFILE_H
#include <geos/gfile.h>
#endif

#ifndef _GPROCESS_H
#include <geos/gprocess.h>
#endif

#ifndef _GGRAPH_H
#include <geos/ggraph.h>
#endif

#ifndef _GMENU_H
#include <geos/gmenu.h>
#endif

#ifndef _GSPRITE_H
#include <geos/gsprite.h>
#endif

#ifndef _GMEMORY_H
#include <geos/gmemory.h>
#endif

#ifndef _GSYS_H
#include <geos/gsys.h>
#endif

#ifndef _GDLGBOX_H
#include <geos/gdlgbox.h>
#endif

/* Character codes $80-$8F */
/*  1  */
/* 2 4 */
/*  8  */
#define CH_HLINE		128|0|2|4|0
#define CH_VLINE		128|1|0|0|8
#define CH_ULCORNER		128|1|2|0|0
#define CH_URCORNER		128|1|0|4|0
#define CH_LLCORNER		128|0|2|0|8
#define CH_LRCORNER		128|0|0|4|8
#define CH_TTEE			128|1|2|4|0
#define CH_RTEE			128|1|0|4|8
#define CH_BTEE			128|0|2|4|8
#define CH_LTEE			128|1|2|0|8
#define CH_CROSS		128|1|2|4|8

/* Additional key defines */
#define CH_F1			133
#define CH_F2			137
#define CH_F3			134
#define CH_F4			138
#define CH_F5			135
#define CH_F6			139
#define CH_F7			136
#define CH_F8			140

#define CH_CURS_UP		145
#define CH_CURS_DOWN		 17
#define CH_CURS_LEFT		157
#define CH_CURS_RIGHT		 29
#define CH_DEL			 20
#define CH_INS			148
#define CH_ENTER		'\n'
#define CH_STOP			  3
#define CH_ESC			 27

/* End of geos.h */
#endif
