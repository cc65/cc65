/*****************************************************************************/
/*                                                                           */
/*				     cbm.h			       	     */
/*                                                                           */
/*		        CBM system specific definitions			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



