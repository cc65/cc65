/*****************************************************************************/
/*                                                                           */
/*                                 atari.h                                   */
/*                                                                           */
/*                      Atari system specific definitions                    */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2018 Mark Keates <markk@dendrite.co.uk>                          */
/*               Freddy Offenga <taf_offenga@yahoo.com>                      */
/*               Christian Groessler <chris@groessler.org>                   */
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



/* Character codes */
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

/* color defines */

/* make GTIA color value */
#define _gtia_mkcolor(hue,lum) (((hue) << 4) | ((lum) << 1))

/* luminance values go from 0 (black) to 7 (white) */

/* hue values */
#define HUE_GREY        0
#define HUE_GOLD        1
#define HUE_GOLDORANGE  2
#define HUE_REDORANGE   3
#define HUE_ORANGE      4
#define HUE_MAGENTA     5
#define HUE_PURPLE      6
#define HUE_BLUE        7
#define HUE_BLUE2       8
#define HUE_CYAN        9
#define HUE_BLUEGREEN   10
#define HUE_BLUEGREEN2  11
#define HUE_GREEN       12
#define HUE_YELLOWGREEN 13
#define HUE_YELLOW      14
#define HUE_YELLOWRED   15

/* Color defines, similar to c64 colors (untested) */
/* Note that the conio color implementation is monochrome (bgcolor and textcolor are only placeholders) */
/* Use the defines with the setcolor() or _atari_xxxcolor() functions */
#define COLOR_BLACK             _gtia_mkcolor(HUE_GREY,0)
#define COLOR_WHITE             _gtia_mkcolor(HUE_GREY,7)
#define COLOR_RED               _gtia_mkcolor(HUE_REDORANGE,1)
#define COLOR_CYAN              _gtia_mkcolor(HUE_CYAN,3)
#define COLOR_VIOLET            _gtia_mkcolor(HUE_PURPLE,4)
#define COLOR_GREEN             _gtia_mkcolor(HUE_GREEN,2)
#define COLOR_BLUE              _gtia_mkcolor(HUE_BLUE,2)
#define COLOR_YELLOW            _gtia_mkcolor(HUE_YELLOW,7)
#define COLOR_ORANGE            _gtia_mkcolor(HUE_ORANGE,5)
#define COLOR_BROWN             _gtia_mkcolor(HUE_YELLOW,2)
#define COLOR_LIGHTRED          _gtia_mkcolor(HUE_REDORANGE,6)
#define COLOR_GRAY1             _gtia_mkcolor(HUE_GREY,2)
#define COLOR_GRAY2             _gtia_mkcolor(HUE_GREY,3)
#define COLOR_LIGHTGREEN        _gtia_mkcolor(HUE_GREEN,6)
#define COLOR_LIGHTBLUE         _gtia_mkcolor(HUE_BLUE,6)
#define COLOR_GRAY3             _gtia_mkcolor(HUE_GREY,5)

/* TGI color defines */
#define TGI_COLOR_BLACK         COLOR_BLACK
#define TGI_COLOR_WHITE         COLOR_WHITE
#define TGI_COLOR_RED           COLOR_RED
#define TGI_COLOR_CYAN          COLOR_CYAN
#define TGI_COLOR_VIOLET        COLOR_VIOLET
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

/* Masks for joy_read */
#define JOY_UP_MASK     0x01
#define JOY_DOWN_MASK   0x02
#define JOY_LEFT_MASK   0x04
#define JOY_RIGHT_MASK  0x08
#define JOY_BTN_1_MASK  0x10

#define JOY_FIRE_MASK   JOY_BTN_1_MASK
#define JOY_FIRE(v)     ((v) & JOY_FIRE_MASK)

/* Keyboard values returned by kbcode / CH */
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

#define KEY_F1          ((unsigned char) 0x03)
#define KEY_F2          ((unsigned char) 0x04)
#define KEY_F3          ((unsigned char) 0x13)
#define KEY_F4          ((unsigned char) 0x14)

#define KEY_CTRL        ((unsigned char) 0x80)
#define KEY_SHIFT       ((unsigned char) 0x40)

/* Composed keys */

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

#define KEY_UP      (KEY_UNDERLINE | KEY_CTRL)
#define KEY_DOWN    (KEY_EQUALS | KEY_CTRL)
#define KEY_LEFT    (KEY_PLUS | KEY_CTRL)
#define KEY_RIGHT   (KEY_ASTERISK | KEY_CTRL)

/* color register functions */
extern void __fastcall__ _setcolor     (unsigned char color_reg, unsigned char hue, unsigned char luminace);
extern void __fastcall__ _setcolor_low (unsigned char color_reg, unsigned char color_value);
extern unsigned char __fastcall__ _getcolor (unsigned char color_reg);

/* other screen functions */
extern int  __fastcall__ _graphics (unsigned char mode); /* mode value same as in BASIC */
extern void __fastcall__ _scroll (signed char numlines);
                                          /* numlines > 0  scrolls up */
                                          /* numlines < 0  scrolls down */

/* misc. functions */
extern unsigned char get_ostype(void);       /* get ROM version */
extern unsigned char get_tv(void);           /* get TV system */
extern void _save_vecs(void);                /* save system vectors */
extern void _rest_vecs(void);                /* restore system vectors */
extern char *_getdefdev(void);               /* get default floppy device */
extern unsigned char _is_cmdline_dos(void);  /* does DOS support command lines */

/* global variables */
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
extern void atrrdev_ser[];
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
extern void atrxrdev_ser[];
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

/* get_ostype return value defines (for explanation, see ostype.s) */
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

/* get_tv return values */
#define AT_NTSC     0
#define AT_PAL      1

/* valid _dos_type values */
#define SPARTADOS   0
#define REALDOS     1
#define BWDOS       2
#define OSADOS      3
#define XDOS        4
#define ATARIDOS    5
#define MYDOS       6
#define NODOS       255

/* Define hardware */
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

/* device control block */
struct __dcb {
    unsigned char device;     /* device id */
    unsigned char unit;       /* unit number */
    unsigned char command;    /* command */
    unsigned char status;     /* command type / status return */
    void          *buffer;    /* pointer to buffer */
    unsigned char timeout;    /* device timeout in seconds */
    unsigned char unused;
    unsigned int  xfersize;   /* # of bytes to transfer */
    unsigned char aux1;       /* 1st command auxiliary byte */
    unsigned char aux2;       /* 2nd command auxiliary byte */
};
#define DCB (*(struct __dcb *)0x300)

/* I/O control block */
struct __iocb {
    unsigned char handler;    /* handler index number (0xff free) */
    unsigned char drive;      /* device number (drive) */
    unsigned char command;    /* command */
    unsigned char status;     /* status of last operation */
    void          *buffer;    /* pointer to buffer */
    void          *put_byte;  /* pointer to device's PUT BYTE routine */
    unsigned int  buflen;     /* length of buffer */
    unsigned char aux1;       /* 1st auxiliary byte */
    unsigned char aux2;       /* 2nd auxiliary byte */
    unsigned char aux3;       /* 3rd auxiliary byte */
    unsigned char aux4;       /* 4th auxiliary byte */
    unsigned char aux5;       /* 5th auxiliary byte */
    unsigned char spare;      /* spare byte */
};
#define ZIOCB (*(struct __iocb *)0x20)  /* zero page IOCB */
#define IOCB (*(struct __iocb *)0x340)  /* system IOCB buffers */

/* IOCB Command Codes */
#define IOCB_OPEN        0x03  /* open */
#define IOCB_GETREC      0x05  /* get record */
#define IOCB_GETCHR      0x07  /* get character(s) */
#define IOCB_PUTREC      0x09  /* put record */
#define IOCB_PUTCHR      0x0B  /* put character(s) */
#define IOCB_CLOSE       0x0C  /* close */
#define IOCB_STATIS      0x0D  /* status */
#define IOCB_SPECIL      0x0E  /* special */
#define IOCB_DRAWLN      0x11  /* draw line */
#define IOCB_FILLIN      0x12  /* draw line with right fill */
#define IOCB_RENAME      0x20  /* rename disk file */
#define IOCB_DELETE      0x21  /* delete disk file */
#define IOCB_LOCKFL      0x23  /* lock file (set to read-only) */
#define IOCB_UNLOCK      0x24  /* unlock file */
#define IOCB_POINT       0x25  /* point sector */
#define IOCB_NOTE        0x26  /* note sector */
#define IOCB_GETFL       0x27  /* get file length */
#define IOCB_CHDIR_MYDOS 0x29  /* change directory (MyDOS) */
#define IOCB_MKDIR       0x2A  /* make directory (MyDOS/SpartaDOS) */
#define IOCB_RMDIR       0x2B  /* remove directory (SpartaDOS) */
#define IOCB_CHDIR_SPDOS 0x2C  /* change directory (SpartaDOS) */
#define IOCB_GETCWD      0x30  /* get current directory (MyDOS/SpartaDOS) */
#define IOCB_FORMAT      0xFE  /* format */



/* End of atari.h */
#endif
