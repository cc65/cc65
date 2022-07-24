/*****************************************************************************/
/*                                                                           */
/*                             creativision.h                                */
/*                                                                           */
/*                  Creativision system specific definitions                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2013      cvemu                                                       */
/* (C) 2017      Christian Groessler <chris@groessler.org>                   */
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

#ifndef _CVISION_H
#define _CVISION_H

/* Character codes */
#define CH_VLINE 33
#define CH_HLINE 34
#define CH_ULCORNER 35
#define CH_URCORNER 36
#define CH_LLCORNER 37
#define CH_LRCORNER 38

/* Masks for joy_read */
#define JOY_UP_MASK     0x10
#define JOY_DOWN_MASK   0x04
#define JOY_LEFT_MASK   0x20
#define JOY_RIGHT_MASK  0x08
#define JOY_BTN_1_MASK  0x01
#define JOY_BTN_2_MASK  0x02

/* no support for dynamically loadable drivers */
#define DYN_DRV 0

/* Colours - from TMS9918 */
#define COLOR_TRANSPARENT   0
#define COLOR_BLACK         1
#define COLOR_MED_GREEN     2
#define COLOR_LIGHT_GREEN   3
#define COLOR_DARK_BLUE     4
#define COLOR_LIGHT_BLUE    5
#define COLOR_DARK_RED      6
#define COLOR_CYAN          7
#define COLOR_MED_RED       8
#define COLOR_LIGHT_RED     9
#define COLOR_DARK_YELLOW   10
#define COLOR_LIGHT_YELLOW  11
#define COLOR_DARK_GREEN    12
#define COLOR_MAGENTA       13
#define COLOR_GREY          14
#define COLOR_WHITE         15

/* Protos */
void __fastcall__ psg_outb (unsigned char b);
void __fastcall__ psg_delay (unsigned char b);
void psg_silence (void);
void __fastcall__ bios_playsound (const void *a, unsigned char b);

#endif  /* #ifndef _CVISION_H */
