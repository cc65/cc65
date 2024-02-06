/*****************************************************************************/
/*                                                                           */
/*                                   nes.h                                   */
/*                                                                           */
/*                      NES system specific definitions                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2003 Groepaz/Hitmen                                              */
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



#ifndef _NES_H
#define _NES_H



/* Check for errors */
#if !defined(__NES__)
#  error This module may only be used when compiling for the NES!
#endif



/* Key and character defines */
#define CH_ENTER                '\n'
#define CH_CURS_UP              0x01
#define CH_CURS_DOWN            0x02
#define CH_CURS_LEFT            0x03
#define CH_CURS_RIGHT           0x04
#define CH_ESC                  8
#define CH_DEL                  20

#define CH_CROSS                0x10
#define CH_RTEE                 0x17
#define CH_LTEE                 0x0f
#define CH_TTEE                 0x16
#define CH_BTEE                 0x15
#define CH_HLINE                0x0b
#define CH_VLINE                0x0e
#define CH_ULCORNER             0x14
#define CH_URCORNER             0x12
#define CH_LLCORNER             0x11
#define CH_LRCORNER             0x08
#define CH_PI                   0x05

/* Color defines */
#define COLOR_BLACK             0x00
#define COLOR_WHITE             0x01
#define COLOR_RED               0x02
#define COLOR_CYAN              0x03
#define COLOR_PURPLE            0x04
#define COLOR_GREEN             0x05
#define COLOR_BLUE              0x06
#define COLOR_YELLOW            0x07
#define COLOR_ORANGE            0x08
#define COLOR_BROWN             0x09
#define COLOR_LIGHTRED          0x0A
#define COLOR_GRAY1             0x0B
#define COLOR_GRAY2             0x0C
#define COLOR_LIGHTGREEN        0x0D
#define COLOR_LIGHTBLUE         0x0E
#define COLOR_GRAY3             0x0F

/* TGI color defines (default palette) */
#define TGI_COLOR_BLACK         COLOR_BLACK
#define TGI_COLOR_RED           COLOR_RED
#define TGI_COLOR_PINK          COLOR_LIGHTRED
#define TGI_COLOR_LIGHTGREY     COLOR_GRAY1
#define TGI_COLOR_GREY          COLOR_GRAY2
#define TGI_COLOR_DARKGREY      COLOR_GRAY3
#define TGI_COLOR_BROWN         COLOR_BROWN
#define TGI_COLOR_PEACH         COLOR_CYAN
#define TGI_COLOR_YELLOW        COLOR_YELLOW
#define TGI_COLOR_LIGHTGREEN    COLOR_LIGHTGREEN
#define TGI_COLOR_GREEN         COLOR_GREEN
#define TGI_COLOR_DARKBROWN     COLOR_ORANGE
#define TGI_COLOR_PURPLE        COLOR_PURPLE
#define TGI_COLOR_BLUE          COLOR_BLUE
#define TGI_COLOR_LIGHTBLUE     COLOR_LIGHTBLUE
#define TGI_COLOR_WHITE         COLOR_WHITE

/* Masks for joy_read */
#define JOY_UP_MASK     0x10
#define JOY_DOWN_MASK   0x20
#define JOY_LEFT_MASK   0x40
#define JOY_RIGHT_MASK  0x80
#define JOY_BTN_1_MASK  0x01
#define JOY_BTN_2_MASK  0x02
#define JOY_BTN_3_MASK  0x04
#define JOY_BTN_4_MASK  0x08

#define JOY_BTN_A_MASK  JOY_BTN_1_MASK
#define JOY_BTN_B_MASK  JOY_BTN_2_MASK
#define JOY_SELECT_MASK JOY_BTN_3_MASK
#define JOY_START_MASK  JOY_BTN_4_MASK

#define JOY_BTN_A(v)    ((v) & JOY_BTN_A_MASK)
#define JOY_BTN_B(v)    ((v) & JOY_BTN_B_MASK)
#define JOY_SELECT(v)   ((v) & JOY_SELECT_MASK)
#define JOY_START(v)    ((v) & JOY_START_MASK)

/* Return codes of get_tv */
#define TV_NTSC         0
#define TV_PAL          1
#define TV_OTHER        2

/* No support for dynamically loadable drivers */
#define DYN_DRV         0

/* Define hardware */

/* Picture Processing Unit */
struct __ppu {
    unsigned char control;
    unsigned char mask;                 /* color; show sprites, background */
      signed char volatile const status;
    struct {
        unsigned char address;
        unsigned char data;
    } sprite;
    unsigned char scroll;
    struct {
        unsigned char address;
        unsigned char data;
    } vram;
};
#define PPU             (*(struct __ppu*)0x2000)
#define SPRITE_DMA      (APU.sprite.dma)

/* Audio Processing Unit */
struct __apu {
    struct {
        unsigned char control;          /* duty, counter halt, volume/envelope */
        unsigned char ramp;
        unsigned char period_low;       /* timing */
        unsigned char len_period_high;  /* length, timing */
    } pulse[2];
    struct {
        unsigned char counter;          /* counter halt, linear counter */
        unsigned char unused;
        unsigned char period_low;       /* timing */
        unsigned char len_period_high;  /* length, timing */
    } triangle;
    struct {
        unsigned char control;          /* counter halt, volume/envelope */
        unsigned char unused;
        unsigned char period;           /* loop, timing */
        unsigned char len;              /* length */
    } noise;
    struct {
        unsigned char control;          /* IRQ, loop, rate */
        unsigned char output;           /* output value */
        unsigned char address;
        unsigned char length;
    } delta_mod;                        /* delta pulse-code modulation */
    struct {
        unsigned char dma;
    } sprite;
      signed char volatile status;
    unsigned char unused;
    unsigned char fcontrol;
};
#define APU             (*(struct __apu*)0x4000)

#define JOYPAD          ((unsigned char volatile[2])0x4016)

/* The addresses of the static drivers */
extern void nes_stdjoy_joy[];       /* Referred to by joy_static_stddrv[] */
extern void nes_64_56_2_tgi[];      /* Referred to by tgi_static_stddrv[] */



void waitvsync (void);
/* Wait for start of the next frame */

unsigned char get_tv (void);
/* Return the video mode the machine is using. */



/* End of nes.h */
#endif
