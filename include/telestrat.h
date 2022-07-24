/*****************************************************************************/
/*                                                                           */
/*                             telestrat.h                                   */
/*                                                                           */
/*                   Oric Telestrat system-specific definitions              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2017       Debrune Jérome, <jede@oric.org>                            */
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




/* Color defines */
#define COLOR_BLACK     0x00
#define COLOR_RED       0x01
#define COLOR_GREEN     0x02
#define COLOR_YELLOW    0x03
#define COLOR_BLUE      0x04
#define COLOR_MAGENTA   0x05
#define COLOR_CYAN      0x06
#define COLOR_WHITE     0x07

/* TGI color defines */
/* White and red are swapped, so that the pallete
** driver is compatible with black-and-white drivers.
*/
#define TGI_COLOR_BLACK         COLOR_BLACK
#define TGI_COLOR_WHITE         1
#define TGI_COLOR_GREEN         COLOR_GREEN
#define TGI_COLOR_YELLOW        COLOR_YELLOW
#define TGI_COLOR_BLUE          COLOR_BLUE
#define TGI_COLOR_MAGENTA       COLOR_MAGENTA
#define TGI_COLOR_CYAN          COLOR_CYAN
#define TGI_COLOR_RED           7


/* Define hardware */
#include <_6522.h>
#define VIA     (*(struct __6522*)0x300)



/* These are defined to be FUNCT + NumberKey */
#define CH_F1           0xB1
#define CH_F2           0xB2
#define CH_F3           0xB3
#define CH_F4           0xB4
#define CH_F5           0xB5
#define CH_F6           0xB6
#define CH_F7           0xB7
#define CH_F8           0xB8
#define CH_F9           0xB9
#define CH_F10          0xB0



/* Character codes */
#define CH_ULCORNER     '+'
#define CH_URCORNER     '+'
#define CH_LLCORNER     '+'
#define CH_LRCORNER     '+'
#define CH_TTEE         '+'
#define CH_BTEE         '+'
#define CH_LTEE         '+'
#define CH_RTEE         '+'
#define CH_CROSS        '+'
#define CH_HLINE        '-'
#define CH_VLINE        '|'
#define CH_CURS_UP       11
#define CH_CURS_DOWN     10
#define CH_CURS_LEFT      8
#define CH_CURS_RIGHT     9
#define CH_DEL          127
#define CH_ENTER         13
#define CH_STOP           3
#define CH_LIRA          95
#define CH_ESC           27

/* Masks for joy_read */
#define JOY_UP_MASK     0x10
#define JOY_DOWN_MASK   0x08
#define JOY_LEFT_MASK   0x02
#define JOY_RIGHT_MASK  0x01
#define JOY_BTN_1_MASK  0x04

#define JOY_FIRE_MASK   JOY_BTN_1_MASK
#define JOY_FIRE(v)     ((v) & JOY_FIRE_MASK)


/* The addresses of the static drivers */
extern void telestrat_joy[];            /* Referred to by joy_static_stddrv[] */
extern void telestrat_228_200_3_tgi[];
extern void telestrat_240_200_2_tgi[];  /* Referred to by tgi_static_stddrv[] */


void oups();
void ping();
void zap();
void shoot();
void explode();

void kbdclick1();

/* The following #defines will cause the matching functions calls in conio.h
** to be overlaid by macros with the same names, saving the function call
** overhead.
*/
#define _bordercolor(color)     COLOR_BLACK
