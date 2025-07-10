/*****************************************************************************/
/*                                                                           */
/*                                 cbm610.h                                  */
/*                                                                           */
/*                    CBM610 system specific definitions                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2009, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
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



#ifndef _CBM610_H
#define _CBM610_H



/* Check for errors */
#if !defined(__CBM610__)
#  error This module may only be used when compiling for the CBM 610!
#endif



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Additional key defines */
#define CH_F1                   224
#define CH_F2                   225
#define CH_F3                   226
#define CH_F4                   227
#define CH_F5                   228
#define CH_F6                   229
#define CH_F7                   230
#define CH_F8                   231
#define CH_F9                   232
#define CH_F10                  233
#define CH_F11                  234
#define CH_F12                  235
#define CH_F13                  236
#define CH_F14                  237
#define CH_F15                  238
#define CH_F16                  239
#define CH_F17                  240
#define CH_F18                  241
#define CH_F19                  242
#define CH_F20                  243

/* Color defines */
#define COLOR_BLACK             0x00
#define COLOR_WHITE             0x01

/* Define hardware */
#include <_6545.h>
#define CRTC    (*(struct __6545)0xD800)

#include <_sid.h>
#define SID     (*(struct __sid*)0xDA00)

#include <_6526.h>
#define CIA1    (*(struct __6526*)0xDB00)
#define CIA2    (*(struct __6526*)0xDC00)

#include <_6551.h>
#define ACIA    (*(struct __6551*)0xDD00)

#include <_6525.h>
#define TPI1    (*(struct __6525*)0xDE00)
#define TPI2    (*(struct __6525*)0xDF00)



/*****************************************************************************/
/*                                 Variables                                 */
/*****************************************************************************/



/* The addresses of the static drivers */
extern void cbm610_ram_emd[];
extern void cbm610_std_ser[];   /* Referred to by ser_static_stddrv[] */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



/* Special routines to read/write bytes and words in the system bank */
unsigned char __fastcall__ peekbsys (unsigned addr);
unsigned __fastcall__ peekwsys (unsigned addr);
void __fastcall__ pokebsys (unsigned addr, unsigned char val);
void __fastcall__ pokewsys (unsigned addr, unsigned val);

#if defined(__OPT_i__) && (__OPT_i__ >= 600)
#define peekbsys(addr)                  \
        __AX__ = (addr),                \
        __asm__ ("sta ptr1"),           \
        __asm__ ("stx ptr1+1"),         \
        __asm__ ("ldx $01"),            \
        __asm__ ("lda #$0F"),           \
        __asm__ ("sta $01"),            \
        __asm__ ("ldy #$00"),           \
        __asm__ ("lda (ptr1),y"),       \
        __asm__ ("stx $01"),            \
        __asm__ ("ldx #$00"),           \
        __AX__
#endif



/* The following #defines will cause the matching functions calls in conio.h
** to be overlaid by macros with the same names, saving the function call
** overhead.
*/
#define _textcolor(color)       COLOR_WHITE
#define _bgcolor(color)         COLOR_BLACK
#define _bordercolor(color)     COLOR_BLACK
#define _cpeekcolor(color)      COLOR_WHITE



/* End of cbm610.h */
#endif
