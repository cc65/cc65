/*****************************************************************************/
/*                                                                           */
/*                                osic1p.h                                   */
/*                                                                           */
/*                Challenger 1P system specific definitions                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2015 Stephan Muehlstrasser                                            */
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

#ifndef _OSIC1P_H
#define _OSIC1P_H

/* Check for errors */
#if !defined(__OSIC1P__)
#  error "This module may only be used when compiling for the Challenger 1P!"
#endif

/* Colors are not functional, display is black and white only. */
#define COLOR_BLACK     0x00
#define COLOR_WHITE     0x01

#define CH_ULCORNER     0xCC
#define CH_URCORNER     0xCD
#define CH_LLCORNER     0xCB
#define CH_LRCORNER     0xCE
#define CH_TTEE         0xD9
#define CH_BTEE         0xD7
#define CH_LTEE         0xD8
#define CH_RTEE         0xDA
#define CH_CROSS        0xDB
#define CH_HLINE        0x94
#define CH_VLINE        0x95

#define CH_ENTER        0x0D

/* The following #defines will cause the matching functions calls in conio.h
** to be overlaid by macros with the same names, saving the function call
** overhead.
*/
#define _textcolor(color)       COLOR_WHITE
#define _bgcolor(color)         COLOR_BLACK
#define _bordercolor(color)     COLOR_BLACK
#define _cpeekcolor(color)      COLOR_WHITE

#endif
