/*****************************************************************************/
/*                                                                           */
/*                                 atari.h                                   */
/*                                                                           */
/*                      Atari system specific definitions                    */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2021 Mark Keates <markk@dendrite.co.uk>                          */
/*               Freddy Offenga <taf_offenga@yahoo.com>                      */
/*               Christian Groessler <chris@groessler.org>                   */
/*               Bill Kendrick <nbs@sonic.net>                               */
/*               et al.                                                      */
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



#ifndef _ATARI_H
#define _ATARI_H


/* Check for errors */
#if !defined(__ATARI__)
#  error This module may only be used when compiling for the Atari!
#endif


/*****************************************************************************/
/* Character codes                                                           */
/*****************************************************************************/

#define CH_DELCHR       0xFE   /* delete char under the cursor */
#define CH_ENTER        0x9B
#define CH_ESC          0x1B
#define CH_CURS_UP      28
#define CH_CURS_DOWN    29
#define CH_CURS_LEFT    30
#define CH_CURS_RIGHT   31

#define CH_TAB          0x7F   /* tabulator */
#define CH_EOL          0x9B   /* end-of-line marker */
#define CH_CLR          0x7D   /* clear screen */
#define CH_BEL          0xFD   /* bell */
#define CH_DEL          0x7E   /* back space (delete char to the left) */
#define CH_RUBOUT       0x7E   /* back space (old, deprecated) */
#define CH_DELLINE      0x9C   /* delete line */
#define CH_INSLINE      0x9D   /* insert line */

/* These are defined to be Atari + NumberKey */
#define CH_F1           177
#define CH_F2           178
#define CH_F3           179
#define CH_F4           180
#define CH_F5           181
#define CH_F6           182
#define CH_F7           183
#define CH_F8           184
#define CH_F9           185
#define CH_F10          176

#define CH_ULCORNER     0x11
#define CH_URCORNER     0x05
#define CH_LLCORNER     0x1A
#define CH_LRCORNER     0x03
#define CH_TTEE         0x17
#define CH_BTEE         0x18
#define CH_LTEE         0x01
#define CH_RTEE         0x04
#define CH_CROSS        0x13
#define CH_HLINE        0x12
#define CH_VLINE        0x7C


/*****************************************************************************/
/* Masks for joy_read                                                        */
/*****************************************************************************/

#define JOY_UP_MASK     0x01
#define JOY_DOWN_MASK   0x02
#define JOY_LEFT_MASK   0x04
#define JOY_RIGHT_MASK  0x08
#define JOY_BTN_1_MASK  0x10

#define JOY_FIRE_MASK   JOY_BTN_1_MASK
#define JOY_FIRE(v)     ((v) & JOY_FIRE_MASK)


/*****************************************************************************/
/* Keyboard values returned by kbcode / CH                                   */
/*****************************************************************************/

#define KEY_NONE        ((unsigned char) 0xFF)

#define KEY_0           ((unsigned char) 0x32)
#define KEY_1           ((unsigned char) 0x1F)
#define KEY_2           ((unsigned char) 0x1E)
#define KEY_3           ((unsigned char) 0x1A)
#define KEY_4           ((unsigned char) 0x18)
#define KEY_5           ((unsigned char) 0x1D)
#define KEY_6           ((unsigned char) 0x1B)
#define KEY_7           ((unsigned char) 0x33)
#define KEY_8           ((unsigned char) 0x35)
#define KEY_9           ((unsigned char) 0x30)

#define KEY_A           ((unsigned char) 0x3F)
#define KEY_B           ((unsigned char) 0x15)
#define KEY_C           ((unsigned char) 0x12)
#define KEY_D           ((unsigned char) 0x3A)
#define KEY_E           ((unsigned char) 0x2A)
#define KEY_F           ((unsigned char) 0x38)
#define KEY_G           ((unsigned char) 0x3D)
#define KEY_H           ((unsigned char) 0x39)
#define KEY_I           ((unsigned char) 0x0D)
#define KEY_J           ((unsigned char) 0x01)
#define KEY_K           ((unsigned char) 0x05)
#define KEY_L           ((unsigned char) 0x00)
#define KEY_M           ((unsigned char) 0x25)
#define KEY_N           ((unsigned char) 0x23)
#define KEY_O           ((unsigned char) 0x08)
#define KEY_P           ((unsigned char) 0x0A)
#define KEY_Q           ((unsigned char) 0x2F)
#define KEY_R           ((unsigned char) 0x28)
#define KEY_S           ((unsigned char) 0x3E)
#define KEY_T           ((unsigned char) 0x2D)
#define KEY_U           ((unsigned char) 0x0B)
#define KEY_V           ((unsigned char) 0x10)
#define KEY_W           ((unsigned char) 0x2E)
#define KEY_X           ((unsigned char) 0x16)
#define KEY_Y           ((unsigned char) 0x2B)
#define KEY_Z           ((unsigned char) 0x17)

#define KEY_COMMA       ((unsigned char) 0x20)
#define KEY_PERIOD      ((unsigned char) 0x22)
#define KEY_SLASH       ((unsigned char) 0x26)
#define KEY_SEMICOLON   ((unsigned char) 0x02)
#define KEY_PLUS        ((unsigned char) 0x06)
#define KEY_ASTERISK    ((unsigned char) 0x07)
#define KEY_DASH        ((unsigned char) 0x0E)
#define KEY_EQUALS      ((unsigned char) 0x0F)
#define KEY_LESSTHAN    ((unsigned char) 0x36)
#define KEY_GREATERTHAN ((unsigned char) 0x37)

#define KEY_ESC         ((unsigned char) 0x1C)
#define KEY_TAB         ((unsigned char) 0x2C)
#define KEY_SPACE       ((unsigned char) 0x21)
#define KEY_RETURN      ((unsigned char) 0x0C)
#define KEY_DELETE      ((unsigned char) 0x34)
#define KEY_CAPS        ((unsigned char) 0x3C)
#define KEY_INVERSE     ((unsigned char) 0x27)
#define KEY_HELP        ((unsigned char) 0x11)

/* Function keys only exist on the 1200XL model. */
#define KEY_F1          ((unsigned char) 0x03)
#define KEY_F2          ((unsigned char) 0x04)
#define KEY_F3          ((unsigned char) 0x13)
#define KEY_F4          ((unsigned char) 0x14)

/* N.B. Cannot read Ctrl key alone */
#define KEY_CTRL        ((unsigned char) 0x80)

/* N.B. Cannot read Shift key alone via KBCODE;
** instead, check "Shfit key press" bit of SKSTAT register.
** Also, no way to tell left Shift from right Shift.
*/
#define KEY_SHIFT       ((unsigned char) 0x40)


/* Composed keys
** (Other combinations are possible, including Shift+Ctrl+key,
** though not all such combinations are available.)
*/

#define KEY_EXCLAMATIONMARK     (KEY_1 | KEY_SHIFT)
#define KEY_QUOTE               (KEY_2 | KEY_SHIFT)
#define KEY_HASH                (KEY_3 | KEY_SHIFT)
#define KEY_DOLLAR              (KEY_4 | KEY_SHIFT)
#define KEY_PERCENT             (KEY_5 | KEY_SHIFT)
#define KEY_AMPERSAND           (KEY_6 | KEY_SHIFT)
#define KEY_APOSTROPHE          (KEY_7 | KEY_SHIFT)
#define KEY_AT                  (KEY_8 | KEY_SHIFT)
#define KEY_OPENINGPARAN        (KEY_9 | KEY_SHIFT)
#define KEY_CLOSINGPARAN        (KEY_0 | KEY_SHIFT)
#define KEY_UNDERLINE           (KEY_DASH | KEY_SHIFT)
#define KEY_BAR                 (KEY_EQUALS | KEY_SHIFT)
#define KEY_COLON               (KEY_SEMICOLON | KEY_SHIFT)
#define KEY_BACKSLASH           (KEY_PLUS | KEY_SHIFT)
#define KEY_CIRCUMFLEX          (KEY_ASTERISK | KEY_SHIFT)
#define KEY_OPENINGBRACKET      (KEY_COMMA | KEY_SHIFT)
#define KEY_CLOSINGBRACKET      (KEY_PERIOD | KEY_SHIFT)
#define KEY_QUESTIONMARK        (KEY_SLASH | KEY_SHIFT)
#define KEY_CLEAR               (KEY_LESSTHAN | KEY_SHIFT)
#define KEY_INSERT              (KEY_GREATERTHAN | KEY_SHIFT)

#define KEY_UP      (KEY_DASH | KEY_CTRL)
#define KEY_DOWN    (KEY_EQUALS | KEY_CTRL)
#define KEY_LEFT    (KEY_PLUS | KEY_CTRL)
#define KEY_RIGHT   (KEY_ASTERISK | KEY_CTRL)


/*****************************************************************************/
/* Color register functions                                                  */
/*****************************************************************************/

void __fastcall__ _setcolor     (unsigned char color_reg, unsigned char hue, unsigned char luminance);
void __fastcall__ _setcolor_low (unsigned char color_reg, unsigned char color_value);
unsigned char __fastcall__ _getcolor (unsigned char color_reg);

/*****************************************************************************/
/* Other screen functions                                                    */
/*****************************************************************************/

void waitvsync (void);                            /* wait for start of next frame */
int  __fastcall__ _graphics (unsigned char mode); /* mode value same as in BASIC */
void __fastcall__ _scroll (signed char numlines);
                                          /* numlines > 0  scrolls up */
                                          /* numlines < 0  scrolls down */


/*****************************************************************************/
/*    Sound function                                                         */
/*****************************************************************************/

void  __fastcall__ _sound (unsigned char voice, unsigned char frequency, unsigned char distortion, unsigned char volume);

/*****************************************************************************/
/* Misc. functions                                                           */
/*****************************************************************************/

unsigned char get_ostype(void);       /* get ROM version */
unsigned char get_tv(void);           /* get TV system */
void _save_vecs(void);                /* save system vectors */
void _rest_vecs(void);                /* restore system vectors */
char *_getdefdev(void);               /* get default floppy device */
unsigned char _is_cmdline_dos(void);  /* does DOS support command lines */


/*****************************************************************************/
/* Global variables                                                          */
/*****************************************************************************/

extern unsigned char _dos_type;         /* the DOS flavour */
#ifndef __ATARIXL__
extern void atr130_emd[];
extern void atrstd_joy[];               /* referred to by joy_static_stddrv[] */
extern void atrmj8_joy[];
extern void atrjoy_mou[];
extern void atrst_mou[];                /* referred to by mouse_static_stddrv[] */
extern void atrami_mou[];
extern void atrtrk_mou[];
extern void atrtt_mou[];
extern void atrrdev_ser[];              /* referred to by ser_static_stddrv[] */
extern void atr3_tgi[];
extern void atr4_tgi[];
extern void atr5_tgi[];
extern void atr6_tgi[];
extern void atr7_tgi[];
extern void atr8_tgi[];                 /* referred to by tgi_static_stddrv[] */
extern void atr8p2_tgi[];
extern void atr9_tgi[];
extern void atr9p2_tgi[];
extern void atr10_tgi[];
extern void atr10p2_tgi[];
extern void atr11_tgi[];
extern void atr14_tgi[];
extern void atr15_tgi[];
extern void atr15p2_tgi[];
#else
extern void atrx130_emd[];
extern void atrxstd_joy[];              /* referred to by joy_static_stddrv[] */
extern void atrxmj8_joy[];
extern void atrxjoy_mou[];
extern void atrxst_mou[];               /* referred to by mouse_static_stddrv[] */
extern void atrxami_mou[];
extern void atrxtrk_mou[];
extern void atrxtt_mou[];
extern void atrxrdev_ser[];             /* referred to by ser_static_stddrv[] */
extern void atrx3_tgi[];
extern void atrx4_tgi[];
extern void atrx5_tgi[];
extern void atrx6_tgi[];
extern void atrx7_tgi[];
extern void atrx8_tgi[];                /* referred to by tgi_static_stddrv[] */
extern void atrx8p2_tgi[];
extern void atrx9_tgi[];
extern void atrx9p2_tgi[];
extern void atrx10_tgi[];
extern void atrx10p2_tgi[];
extern void atrx11_tgi[];
extern void atrx14_tgi[];
extern void atrx15_tgi[];
extern void atrx15p2_tgi[];
#endif


/*****************************************************************************/
/* get_ostype return value defines (for explanation, see ostype.s)           */
/*****************************************************************************/

/* masks */
#define AT_OS_TYPE_MAIN  7
#define AT_OS_TYPE_MINOR (7 << 3)
/* AT_OS_TYPE_MAIN values */
#define AT_OS_UNKNOWN  0
#define AT_OS_400800   1
#define AT_OS_1200XL   2
#define AT_OS_XLXE     3
/* AS_OS_TYPE_MINOR values */
/* for 400/800 remember this are the ROM versions */
/* to check whether the hw is PAL or NTSC, use get_tv() */
#define AT_OS_400800PAL_A  1
#define AT_OS_400800PAL_B  2
#define AT_OS_400800NTSC_A 1
#define AT_OS_400800NTSC_B 2
#define AT_OS_1200_10  1
#define AT_OS_1200_11  2
#define AT_OS_XLXE_1   1
#define AT_OS_XLXE_2   2
#define AT_OS_XLXE_3   3
#define AT_OS_XLXE_4   4


/*****************************************************************************/
/* get_tv return values                                                      */
/*****************************************************************************/

#define AT_NTSC     0
#define AT_PAL      1


/*****************************************************************************/
/* valid _dos_type values                                                    */
/*****************************************************************************/

#define SPARTADOS   0
#define REALDOS     1
#define BWDOS       2
#define OSADOS      3
#define XDOS        4
#define ATARIDOS    5
#define MYDOS       6
#define NODOS       255


/*****************************************************************************/
/* Define hardware and where they're mapped in memory                        */
/*****************************************************************************/

#include <_atarios.h>
#define OS (*(struct __os*)0x0000)
#define BASIC (*(struct __basic*)0x0080)

#include <_gtia.h>
#define GTIA_READ  (*(struct __gtia_read*)0xD000)
#define GTIA_WRITE (*(struct __gtia_write*)0xD000)

#include <_pbi.h>

#include <_pokey.h>
#define POKEY_READ  (*(struct __pokey_read*)0xD200)
#define POKEY_WRITE (*(struct __pokey_write*)0xD200)

#include <_pia.h>
#define PIA (*(struct __pia*)0xD300)

#include <_antic.h>
#define ANTIC (*(struct __antic*)0xD400)


/*****************************************************************************/
/* conio and TGI color defines                                               */
/*****************************************************************************/

/* Note that the conio color implementation is monochrome
** (textcolor just sets text brightness low or high, depending on background
** color)
** These values can be used with bordercolor(), bgcolor(), and _setcolor_low()
*/
#define COLOR_BLACK      GTIA_COLOR_BLACK
#define COLOR_WHITE      GTIA_COLOR_WHITE
#define COLOR_RED        GTIA_COLOR_RED
#define COLOR_CYAN       GTIA_COLOR_CYAN
#define COLOR_PURPLE     GTIA_COLOR_VIOLET
#define COLOR_GREEN      GTIA_COLOR_GREEN
#define COLOR_BLUE       GTIA_COLOR_BLUE
#define COLOR_YELLOW     GTIA_COLOR_YELLOW
#define COLOR_ORANGE     GTIA_COLOR_ORANGE
#define COLOR_BROWN      GTIA_COLOR_BROWN
#define COLOR_LIGHTRED   GTIA_COLOR_LIGHTRED
#define COLOR_GRAY1      GTIA_COLOR_GRAY1
#define COLOR_GRAY2      GTIA_COLOR_GRAY2
#define COLOR_LIGHTGREEN GTIA_COLOR_LIGHTGREEN
#define COLOR_LIGHTBLUE  GTIA_COLOR_LIGHTBLUE
#define COLOR_GRAY3      GTIA_COLOR_GRAY3

/* TGI color defines */
#define TGI_COLOR_BLACK      COLOR_BLACK
#define TGI_COLOR_WHITE      COLOR_WHITE
#define TGI_COLOR_RED        COLOR_RED
#define TGI_COLOR_CYAN       COLOR_CYAN
#define TGI_COLOR_PURPLE     COLOR_PURPLE
#define TGI_COLOR_GREEN      COLOR_GREEN
#define TGI_COLOR_BLUE       COLOR_BLUE
#define TGI_COLOR_YELLOW     COLOR_YELLOW
#define TGI_COLOR_ORANGE     COLOR_ORANGE
#define TGI_COLOR_BROWN      COLOR_BROWN
#define TGI_COLOR_LIGHTRED   COLOR_LIGHTRED
#define TGI_COLOR_GRAY1      COLOR_GRAY1
#define TGI_COLOR_GRAY2      COLOR_GRAY2
#define TGI_COLOR_LIGHTGREEN COLOR_LIGHTGREEN
#define TGI_COLOR_LIGHTBLUE  COLOR_LIGHTBLUE
#define TGI_COLOR_GRAY3      COLOR_GRAY3


/*****************************************************************************/
/* PIA PORTA and PORTB register bits                                         */
/*****************************************************************************/

/* See also: "JOY_xxx_MASK" in "atari.h" */

/* Paddle 0-3 triggers (per PORTA bits) */
#define PORTA_PTRIG3 0x80
#define PORTA_PTRIG2 0x40
#define PORTA_PTRIG1 0x08
#define PORTA_PTRIG0 0x04


/* On the Atari 400/800, PORTB is the same as PORTA, but for controller ports 3 & 4. */

/* Paddle 4-7 triggers (per PORTB bits); only 400/800 had four controller ports */
#define PORTB_PTRIG7 0x80
#define PORTB_PTRIG6 0x40
#define PORTB_PTRIG5 0x08
#define PORTB_PTRIG4 0x04


/* On the XL series of computers, PORTB has been changed to a memory and
** LED control (1200XL model only) register (read/write):
*/

/* If set, the built-in OS is enabled, and occupies the address range $C000-$FFFF
** (except that the area $D000-$D7FF will only access the hardware registers.)
** If clear, RAM is enabled in this area (again, save for the hole.)
*/
#define PORTB_OSROM            0x01

/* If set, RAM is enabled for the address range $A000-$BFFF.
** If clear, the built-in BASIC ROM is enabled at this address.
** And if there is a cartridge installed in the computer, it makes no difference.
*/
#define PORTB_BASICROM         0x02

/* If set, the corresponding LED is turned off. If clear, the LED will be on.
** (1200XL only)
*/
#define PORTB_LED1             0x04
#define PORTB_LED2             0x08


/* On the XE series of computers, PORTB is a bank-selected memory control register (read/write): */

/* These bits determine which memory bank is visible to the CPU and/or ANTIC chip
** when their Bank Switch bit is set. There are four possible banks of 16KB each.
*/
#define PORTB_BANKSELECT1      0x00
#define PORTB_BANKSELECT2      0x04
#define PORTB_BANKSELECT3      0x08
#define PORTB_BANKSELECT4      0x0C

/* If set, the CPU and/or ANTIC chip will access bank-switched memory mapped to the
** address range $4000-$7FFF.
** If clear, the CPU and/or ANTIC will see normal memory in this region.
*/
#define PORTB_BANKSWITCH_CPU   0x10
#define PORTB_BANKSWITCH_ANTIC 0x20

/* If set, RAM is enabled for the address range $5000-$57FF.
** If clear, the self-test ROM (physically located at $D000-$D7FF, under the hardware registers)
** is remapped to this memory area.
*/
#define PORTB_SELFTEST         0x80


/*****************************************************************************/
/* PACTL and PBCTL register bits                                             */
/*****************************************************************************/

/* (W) Peripheral PA1/PB1 interrupt (IRQ) ("peripheral proceed line available") enable.
** One equals enable. Set by the OS but available to the user; reset on powerup.
** (PxCTL_IRQ_STATUS (R) bit will get set upon interrupt occurance)
*/
#define PxCTL_IRQ_ENABLE         0x01 /* bit 0 */

/* Note: Bit 1 is always set to */

/* (W) Controls PORTA/PORTB addressing
** 1 = PORTA/PORTB register; read/write to controller port
** 0 = direction control register; write to direction controls
**     (allows setting data flow; write 0s & 1s to PORTA/PORTB bits
**     to set which port's pins are read (input), or write (output),
**     respectively)
*/
#define PxCTL_ADDRESSING         0x04 /* bit 2 */

/* (W) Peripheral motor control line; Turn the cassette on or off
** (PACTL-specific register bit)
** 0 = on
** 1 = off
*/
#define PACTL_MOTOR_CONTROL      0x08 /* bit 3 */

/* Peripheral command identification (serial bus command line)
** (PBCTL-specific register bit)
*/
#define PBCTL_PERIPH_CMD_IDENT   0x08 /* bit 3 */

/* Note: Bits 4 & 5 are always set to 1 */

/* Note: Bit 6 is always set to 0 */

/* (R) Peripheral interrupt (IRQ) status bit.
** Set by Peripherals (PORTA / PORTB).  Reset by reading from PORTA / PORTB.
** PACTL's is interrupt status of PROCEED
** PBCTL's is interrupt status of SIO
*/
#define PxCTL_IRQ_STATUS         0x80


/* The following #define will cause the matching function calls in conio.h
** to be overlaid by macros with the same names, saving the function call
** overhead.
*/
#define _textcolor(color)        COLOR_WHITE

/* End of atari.h */
#endif
