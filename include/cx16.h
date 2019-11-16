/*****************************************************************************/
/*                                                                           */
/*                                  cx16.h                                   */
/*                                                                           */
/*                     CX16 system-specific definitions                      */
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



/* Additional key defines */
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
#define COLOR_VIOLET            0x04
#define COLOR_PURPLE            COLOR_VIOLET
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

/* Masks for joy_read() */
#define JOY_BTN_1_MASK  0x80
#define JOY_BTN_2_MASK  0x40
#define JOY_BTN_3_MASK  0x20
#define JOY_BTN_4_MASK  0x10
#define JOY_UP_MASK     0x08
#define JOY_DOWN_MASK   0x04
#define JOY_LEFT_MASK   0x02
#define JOY_RIGHT_MASK  0x01

#define JOY_BTN_A_MASK  JOY_BTN_1_MASK
#define JOY_BTN_B_MASK  JOY_BTN_2_MASK
#define JOY_SELECT_MASK JOY_BTN_3_MASK
#define JOY_START_MASK  JOY_BTN_4_MASK

#define JOY_BTN_A(v)    ((v) & JOY_BTN_A_MASK)
#define JOY_BTN_B(v)    ((v) & JOY_BTN_B_MASK)
#define JOY_SELECT(v)   ((v) & JOY_SELECT_MASK)
#define JOY_START(v)    ((v) & JOY_START_MASK)

#define JOY_FIRE2_MASK  JOY_BTN_2_MASK
#define JOY_FIRE2(v)    ((v) & JOY_FIRE2_MASK)

/* Additional mouse button mask */
#define MOUSE_BTN_MIDDLE     0x02

/* get_tv() return codes
** set_tv() argument codes
*/
#define TV_NONE         0
#define TV_VGA          1
#define TV_NTSC_COLOR   2
#define TV_RGB          3
#define TV_NONE2        4
#define TV_VGA2         5
#define TV_NTSC_MONO    6
#define TV_RGB2         7

/* Video modes */
#define VIDEOMODE_40x30         0x00
#define VIDEOMODE_80x60         0x02
#define VIDEOMODE_40COL         VIDEOMODE_40x30
#define VIDEOMODE_80COL         VIDEOMODE_80x60
#define VIDEOMODE_320x240       0x80
#define VIDEOMODE_SWAP          (-1)

/* VERA's interrupt flags */
#define VERA_IRQ_VSYNC          0b00000001
#define VERA_IRQ_RASTER         0b00000010
#define VERA_IRQ_SPR_COLL       0b00000100
#define VERA_IRQ_UART           0b00001000


/* Define hardware */

/* Define a structure with the Video Enhanced Retro Adapter's
** external registers.
*/
struct __vera {
    unsigned short      address;        /* Address for data ports */
    unsigned char       address_hi;
    unsigned char       data0;          /* Data port 0 */
    unsigned char       data1;          /* Data port 1 */
    unsigned char       control;        /* Control register */
    unsigned char       irq_enable;     /* Interrupt enable bits */
    unsigned char       irq_flags;      /* Interrupt flags */
};
#define VERA    (*(volatile struct __vera *)0x9F20)

#include <_6522.h>
#define VIA1    (*(volatile struct __6522 *)0x9F60)
#define VIA2    (*(volatile struct __6522 *)0x9F70)

/* Define a structure with the x16emu's settings registers. */
struct __emul {
    unsigned char       debug;          /* Boolean: debugging enabled */
    unsigned char       vera_action;    /* Boolean: displaying VERA activity */
    unsigned char       keyboard;       /* Boolean: displaying typed keys */
    unsigned char       echo;           /* How Kernal output should be echoed to host */
    unsigned char       save_on_exit;   /* Boolean: save SD card when quitting */
    unsigned char       gif_method;     /* How GIF movie is being recorded */
    unsigned char       unused[0xD - 0x6];
    unsigned char       keymap;         /* Keyboard layout number */
       const char       detect[2];      /* "16" if running on x16emu */
};
#define EMULATOR (*(volatile struct __emul)0x9FB0)



/* The addresses of the static drivers */

extern void cx16_std_joy[];             /* Referred to by joy_static_stddrv[] */
extern void cx16_std_mou[];             /* Referred to by mouse_static_stddrv[] */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



signed char get_ostype (void);
/* Get the ROM build version.
** -1 -- custom build
** Negative -- prerelease build
** Positive -- release build
*/

unsigned char get_tv (void);
/* Return the video type that the machine is using.
** Return a TV_xx constant.
*/

void __fastcall__ set_tv (unsigned char type);
/* Set the video type that the machine will use.
** Call with a TV_xx constant.
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
