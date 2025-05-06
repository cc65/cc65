/*****************************************************************************/
/*                                                                           */
/*                                  cx16.h                                   */
/*                                                                           */
/*                      CX16 system-specific definitions                     */
/*                             For prerelease 43                             */
/*                                                                           */
/*                                                                           */
/* This software is provided "as-is", without any expressed or implied       */
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
/*    appreciated, but is not required.                                      */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/



#ifndef _CX16_H
#define _CX16_H



/* Check for errors */
#ifndef __CX16__
#  error This module may be used only when compiling for the CX16!
#endif



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Additional output character codes */
#define CH_COLOR_SWAP           0x01
#define CH_UNDERLINE            0x04
#define CH_WHITE                0x05
#define CH_BOLD                 0x06
#define CH_BACKSPACE            0x08
#define CH_ITALIC               0x0B
#define CH_OUTLINE              0x0C
#define CH_FONT_ISO             0x0F
#define CH_RED                  0x1C
#define CH_GREEN                0x1E
#define CH_BLUE                 0x1F
#define CH_ORANGE               0x81
#define CH_FONT_PET             0x8F
#define CH_BLACK                0x90
#define CH_ATTR_CLEAR           0x92
#define CH_BROWN                0x95
#define CH_PINK                 0x96
#define CH_LIGHTRED             CH_PINK
#define CH_GRAY1                0x97
#define CH_GRAY2                0x98
#define CH_LIGHTGREEN           0x99
#define CH_LIGHTBLUE            0x9A
#define CH_GRAY3                0x9B
#define CH_PURPLE               0x9C
#define CH_YELLOW               0x9E
#define CH_CYAN                 0x9F
#define CH_SHIFT_SPACE          0xA0

/* Additional key defines */
#define CH_SHIFT_TAB            0x18
#define CH_HELP                 0x84
#define CH_F1                   0x85
#define CH_F2                   0x89
#define CH_F3                   0x86
#define CH_F4                   0x8A
#define CH_F5                   0x87
#define CH_F6                   0x8B
#define CH_F7                   0x88
#define CH_F8                   0x8C
#define CH_F9                   0x10
#define CH_F10                  0x15
#define CH_F11                  0x16
#define CH_F12                  0x17

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
#define COLOR_PINK              0x0A
#define COLOR_LIGHTRED          COLOR_PINK
#define COLOR_GRAY1             0x0B
#define COLOR_GRAY2             0x0C
#define COLOR_LIGHTGREEN        0x0D
#define COLOR_LIGHTBLUE         0x0E
#define COLOR_GRAY3             0x0F

/* TGI color defines */
#define TGI_COLOR_BLACK         COLOR_BLACK
#define TGI_COLOR_WHITE         COLOR_WHITE
#define TGI_COLOR_RED           COLOR_RED
#define TGI_COLOR_CYAN          COLOR_CYAN
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

/* NES controller masks for joy_read() */

#define JOY_BTN_1_MASK          0x80
#define JOY_BTN_2_MASK          0x40
#define JOY_BTN_3_MASK          0x20
#define JOY_BTN_4_MASK          0x10
#define JOY_UP_MASK             0x08
#define JOY_DOWN_MASK           0x04
#define JOY_LEFT_MASK           0x02
#define JOY_RIGHT_MASK          0x01

#define JOY_BTN_A_MASK          JOY_BTN_1_MASK
#define JOY_BTN_B_MASK          JOY_BTN_2_MASK
#define JOY_SELECT_MASK         JOY_BTN_3_MASK
#define JOY_START_MASK          JOY_BTN_4_MASK

#define JOY_BTN_A(v)            ((v) & JOY_BTN_A_MASK)
#define JOY_BTN_B(v)            ((v) & JOY_BTN_B_MASK)
#define JOY_SELECT(v)           ((v) & JOY_SELECT_MASK)
#define JOY_START(v)            ((v) & JOY_START_MASK)

#define JOY_FIRE2_MASK          JOY_BTN_2_MASK
#define JOY_FIRE2(v)            ((v) & JOY_FIRE2_MASK)

/* Additional mouse button mask */
#define MOUSE_BTN_MIDDLE        0x02

/* get_tv() return codes
** set_tv() argument codes
*/
enum {
    TV_NONE                     = 0x00,
    TV_VGA,
    TV_NTSC_COLOR,
    TV_RGB,
    TV_NONE2,
    TV_VGA2,
    TV_NTSC_MONO,
    TV_RGB2
};

/* Video modes for videomode() */
#define VIDEOMODE_80x60         0x00
#define VIDEOMODE_80x30         0x01
#define VIDEOMODE_40x60         0x02
#define VIDEOMODE_40x30         0x03
#define VIDEOMODE_40x15         0x04
#define VIDEOMODE_20x30         0x05
#define VIDEOMODE_20x15         0x06
#define VIDEOMODE_22x23         0x07
#define VIDEOMODE_64x50         0x08
#define VIDEOMODE_64x25         0x09
#define VIDEOMODE_32x50         0x0A
#define VIDEOMODE_32x25         0x0B
#define VIDEOMODE_80COL         VIDEOMODE_80x60
#define VIDEOMODE_40COL         VIDEOMODE_40x30
#define VIDEOMODE_320x240       0x80
#define VIDEOMODE_SWAP          (-1)

/* VERA's address increment/decrement numbers */
enum {
    VERA_DEC_0                  = ((0 << 1) | 1) << 3,
    VERA_DEC_1                  = ((1 << 1) | 1) << 3,
    VERA_DEC_2                  = ((2 << 1) | 1) << 3,
    VERA_DEC_4                  = ((3 << 1) | 1) << 3,
    VERA_DEC_8                  = ((4 << 1) | 1) << 3,
    VERA_DEC_16                 = ((5 << 1) | 1) << 3,
    VERA_DEC_32                 = ((6 << 1) | 1) << 3,
    VERA_DEC_64                 = ((7 << 1) | 1) << 3,
    VERA_DEC_128                = ((8 << 1) | 1) << 3,
    VERA_DEC_256                = ((9 << 1) | 1) << 3,
    VERA_DEC_512                = ((10 << 1) | 1) << 3,
    VERA_DEC_40                 = ((11 << 1) | 1) << 3,
    VERA_DEC_80                 = ((12 << 1) | 1) << 3,
    VERA_DEC_160                = ((13 << 1) | 1) << 3,
    VERA_DEC_320                = ((14 << 1) | 1) << 3,
    VERA_DEC_640                = ((15 << 1) | 1) << 3,
    VERA_INC_0                  = ((0 << 1) | 0) << 3,
    VERA_INC_1                  = ((1 << 1) | 0) << 3,
    VERA_INC_2                  = ((2 << 1) | 0) << 3,
    VERA_INC_4                  = ((3 << 1) | 0) << 3,
    VERA_INC_8                  = ((4 << 1) | 0) << 3,
    VERA_INC_16                 = ((5 << 1) | 0) << 3,
    VERA_INC_32                 = ((6 << 1) | 0) << 3,
    VERA_INC_64                 = ((7 << 1) | 0) << 3,
    VERA_INC_128                = ((8 << 1) | 0) << 3,
    VERA_INC_256                = ((9 << 1) | 0) << 3,
    VERA_INC_512                = ((10 << 1) | 0) << 3,
    VERA_INC_40                 = ((11 << 1) | 0) << 3,
    VERA_INC_80                 = ((12 << 1) | 0) << 3,
    VERA_INC_160                = ((13 << 1) | 0) << 3,
    VERA_INC_320                = ((14 << 1) | 0) << 3,
    VERA_INC_640                = ((15 << 1) | 0) << 3
};

/* VERA's interrupt flags */
#define VERA_IRQ_VSYNC          0b00000001
#define VERA_IRQ_RASTER         0b00000010
#define VERA_IRQ_SPR_COLL       0b00000100
#define VERA_IRQ_AUDIO_LOW      0b00001000


/* Define hardware. */

#define RAM_BANK        (*(unsigned char *)0x00)
#define ROM_BANK        (*(unsigned char *)0x01)

#include <_6522.h>
#define VIA1    (*(volatile struct __6522 *)0x9F00)
#define VIA2    (*(volatile struct __6522 *)0x9F10)

/* A structure with the Video Enhanced Retro Adapter's external registers */
struct __vera {
    unsigned short      address;        /* Address for data ports */
    unsigned char       address_hi;
    unsigned char       data0;          /* Data port 0 */
    unsigned char       data1;          /* Data port 1 */
    unsigned char       control;        /* Control register */
    unsigned char       irq_enable;     /* Interrupt enable bits */
    unsigned char       irq_flags;      /* Interrupt flags */
    unsigned char       irq_raster;     /* Line where IRQ will occur */
    union {
        struct {                        /* Visible when DCSEL flag = 0 */
            unsigned char video;        /* Flags to enable video layers */
            unsigned char hscale;       /* Horizontal scale factor */
            unsigned char vscale;       /* Vertical scale factor */
            unsigned char border;       /* Border color (NTSC mode) */
        };
        struct {                        /* Visible when DCSEL flag = 1 */
            unsigned char hstart;       /* Horizontal start position */
            unsigned char hstop;        /* Horizontal stop position */
            unsigned char vstart;       /* Vertical start position */
            unsigned char vstop;        /* Vertical stop position */
        };
        struct {                        /* Visible when DCSEL flag = 2 */
            unsigned char fxctrl;
            unsigned char fxtilebase;
            unsigned char fxmapbase;
            unsigned char fxmult;
        };
        struct {                        /* Visible when DCSEL flag = 3 */
            unsigned char fxxincrl;
            unsigned char fxxincrh;
            unsigned char fxyincrl;
            unsigned char fxyincrh;
        };
        struct {                        /* Visible when DCSEL flag = 4 */
            unsigned char fxxposl;
            unsigned char fxxposh;
            unsigned char fxyposl;
            unsigned char fxyposh;
        };
        struct {                        /* Visible when DCSEL flag = 5 */
            unsigned char fxxposs;
            unsigned char fxyposs;
            unsigned char fxpolyfilll;
            unsigned char fxpolyfillh;
        };
        struct {                        /* Visible when DCSEL flag = 6 */
            unsigned char fxcachel;
            unsigned char fxcachem;
            unsigned char fxcacheh;
            unsigned char fxcacheu;
        };
        struct {                        /* Visible when DCSEL flag = 63 */
            unsigned char dcver0;
            unsigned char dcver1;
            unsigned char dcver2;
            unsigned char dcver3;
        };
    } display;
    struct {
        unsigned char   config;         /* Layer map geometry */
        unsigned char   mapbase;        /* Map data address */
        unsigned char   tilebase;       /* Tile address and geometry */
        unsigned int    hscroll;        /* Smooth scroll horizontal offset */
        unsigned int    vscroll;        /* Smooth scroll vertical offset */
    } layer0;
    struct {
        unsigned char   config;
        unsigned char   mapbase;
        unsigned char   tilebase;
        unsigned int    hscroll;
        unsigned int    vscroll;
    } layer1;
    struct {
        unsigned char   control;        /* PCM format */
        unsigned char   rate;           /* Sample rate */
        unsigned char   data;           /* PCM output queue */
    } audio;                            /* Pulse-Code Modulation registers */
    struct {
        unsigned char   data;
        unsigned char   control;
    } spi;                              /* SD card interface */
};
#define VERA    (*(volatile struct __vera *)0x9F20)

/* Audio chip */
struct __ym2151 {
    unsigned char       reg;            /* Register number for data */
    union {
        unsigned char   data;
        unsigned char   status;         /* Busy flag */
    };
};
#define YM2151  (*(volatile struct __ym2151 *)0x9F40)

/* A structure with the x16emu's settings registers */
struct __emul {
    unsigned char       debug;          /* Boolean: debugging enabled */
    unsigned char       vera_action;    /* Boolean: displaying VERA activity */
    unsigned char       keyboard;       /* Boolean: displaying typed keys */
    unsigned char       echo;           /* How to send Kernal output to host */
    unsigned char       save_on_exit;   /* Boolean: save machine state on exit */
    unsigned char       gif_method;     /* How GIF movie is being recorded */
    unsigned char const unused1[2];
    unsigned long const cycle_count;    /* Running total of CPU cycles (8 MHz.) */
    unsigned char const unused2[1];
    unsigned char const keymap;         /* Keyboard layout number */
             char const detect[2];      /* "16" if running on x16emu */
};
#define EMULATOR        (*(volatile struct __emul *)0x9FB0)

/* An array window into the half Mebibyte or two Mebibytes of banked RAM */
#define BANK_RAM        ((unsigned char *)0xA000)



/* The addresses of the static drivers */

extern void cx16_std_joy[];             /* Referenced by joy_static_stddrv[] */
extern void cx16_std_mou[];             /* Referenced by mouse_static_stddrv[] */
extern void cx320p1_tgi[];              /* Referenced by tgi_static_stddrv[] */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned short get_numbanks (void);
/* Return the number of RAM banks that the machine has. */

signed char get_ostype (void);
/* Get the ROM build version.
** -1 -- custom build
** Negative -- prerelease build
** Positive -- release build
*/

unsigned char get_tv (void);
/* Return the video signal type that the machine is using.
** Return a TV_xx constant.
*/

void __fastcall__ set_tv (unsigned char type);
/* Set the video signal type that the machine will use.
** Call with a TV_xx constant.
*/

unsigned char __fastcall__ vera_layer_enable (unsigned char layers);
/* Display the layers that are "named" by the bit flags in layers.
** A value of 0b01 shows layer 0, a value of 0b10 shows layer 1,
** a value of 0b11 shows both layers.  Return the previous value.
*/

unsigned char __fastcall__ vera_sprites_enable (unsigned char mode);
/* Enable the sprite engine when mode is non-zero (true);
** disable sprites when mode is zero.  Return the previous mode.
*/

signed char __fastcall__ videomode (signed char mode);
/* Set the video mode, return the old mode.
** Return -1 if Mode isn't valid.
** Call with one of the VIDEOMODE_xx constants.
*/

unsigned char __fastcall__ vpeek (unsigned long addr);
/* Get a byte from a location in VERA's internal address space. */

void __fastcall__ vpoke (unsigned char data, unsigned long addr);
/* Put a byte into a location in VERA's internal address space.
** (addr is second instead of first for the sake of code efficiency.)
*/



/* End of cX16.h */
#endif
