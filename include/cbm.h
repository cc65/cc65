/*
 * cbm.h
 *
 * Ullrich von Bassewitz, 07.08.1998
 */



#ifndef _CBM_H
#define _CBM_H



/* Load the system specific files here, if needed */
#ifdef __C64__
#ifndef _C64_H
#include <c64.h>
#endif
#endif

#ifdef __C128__
#ifndef _C128_H
#include <c128.h>
#endif
#endif

#ifdef __PLUS4__
#ifndef _PLUS4_H
#include <plus4.h>
#endif
#endif

#ifdef __CBM610__
#ifndef _CBM610_H
#include <cbm610.h>
#endif
#endif

#ifdef __PET__
#ifndef _PET_H
#include <pet.h>
#endif
#endif



/* Characters codes (CBM charset) */
#define CH_HLINE    		 96
#define CH_VLINE    		125
#define	CH_ULCORNER 		176
#define CH_URCORNER 		174
#define CH_LLCORNER 		173
#define CH_LRCORNER 		189
#define CH_TTEE	    		178
#define CH_RTEE	    		179
#define CH_BTEE	    		177
#define CH_LTEE	    		171
#define CH_CROSS    		123
#define CH_CURS_UP		145
#define CH_CURS_DOWN		 17
#define CH_CURS_LEFT		157
#define CH_CURS_RIGHT		 29
#define CH_PI			126
#define CH_DEL			 20
#define CH_INS			148
#define CH_ESC			 95



/* End of cbm.h */
#endif



