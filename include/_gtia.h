/*****************************************************************************/
/*                                                                           */
/*                                 _gtia.h                                   */
/*                                                                           */
/*                  Internal include file, do not use directly               */
/*                                                                           */
/* "GTIA, Graphic Television Interface Adaptor, is a custom chip used in the */
/* Atari 8-bit family of computers and in the Atari 5200 console. In these   */
/* systems, GTIA chip works together with ANTIC to produce video display.    */
/* ANTIC generates the playfield graphics (text and bitmap) while GTIA       */
/* provides the color for the playfield and adds overlay objects known as    */
/* player/missile graphics (sprites)" - Wikipedia article on "GTIA"          */
/*                                                                           */
/*                                                                           */
/* (C) 2000 Freddy Offenga <taf_offenga@yahoo.com>                           */
/* 2019-01-16: Bill Kendrick <nbs@sonic.net>: More defines for registers     */
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


#ifndef __GTIA_H
#define __GTIA_H

/*****************************************************************************/
/* Define a structure with the GTIA register offsets for write (W)           */
/*****************************************************************************/

struct __gtia_write {
    unsigned char   hposp0; /* 0x00: horizontal position of player 0 */
    unsigned char   hposp1; /* 0x01: horizontal position of player 1 */
    unsigned char   hposp2; /* 0x02: horizontal position of player 2 */
    unsigned char   hposp3; /* 0x03: horizontal position of player 3 */
    unsigned char   hposm0; /* 0x04: horizontal position of missile 0 */
    unsigned char   hposm1; /* 0x05: horizontal position of missile 1 */
    unsigned char   hposm2; /* 0x06: horizontal position of missile 2 */
    unsigned char   hposm3; /* 0x07: horizontal position of missile 3 */

    unsigned char   sizep0; /* 0x08: size of player 0 */
    unsigned char   sizep1; /* 0x09: size of player 1 */
    unsigned char   sizep2; /* 0x0A: size of player 2 */
    unsigned char   sizep3; /* 0x0B: size of player 3 */
    unsigned char   sizem;  /* 0x0C: size of missiles */

    unsigned char   grafp0; /* 0x0D: graphics shape player 0 (used when ANTIC is not instructed to use DMA; see DMACTL) */
    unsigned char   grafp1; /* 0x0E: graphics shape player 1 */
    unsigned char   grafp2; /* 0x0F: graphics shape player 2 */
    unsigned char   grafp3; /* 0x10: graphics shape player 3 */
    unsigned char   grafm;  /* 0x11: graphics shape missiles */

    unsigned char   colpm0; /* 0x12: color player and missile 0 */
    unsigned char   colpm1; /* 0x13: color player and missile 1 */
    unsigned char   colpm2; /* 0x14: color player and missile 2 */
    unsigned char   colpm3; /* 0x15: color player and missile 3 */
    unsigned char   colpf0; /* 0x16: color playfield 0 */
    unsigned char   colpf1; /* 0x17: color playfield 1 */
    unsigned char   colpf2; /* 0x18: color playfield 2 */
    unsigned char   colpf3; /* 0x19: color playfield 3 */
    unsigned char   colbk;  /* 0x1A: color background */

    unsigned char   prior;  /* 0x1B: priority selection */

    unsigned char   vdelay;
    /* 0x1C: vertical delay -- one-line resolution movement of
    ** vertical position of an object when two line resolution display is enabled
    */

    unsigned char   gractl; /* 0x1D: stick/paddle latch, p/m control */

    unsigned char   hitclr; /* 0x1E: clear p/m collision */
    unsigned char   consol; /* 0x1F: builtin speaker */
};


/*****************************************************************************/
/* (W) Values for SIZEP0-SIZEP3 and SIZEM registers:                         */
/*****************************************************************************/

#define PMG_SIZE_NORMAL 0x0 /* one color clock per pixel */
#define PMG_SIZE_DOUBLE 0x1 /* two color clocks per pixel */
#define PMG_SIZE_QUAD   0x3 /* four color clocks per pixel */


/* COLPM0-COLPM3, COLPF0-COLPF3, COLBK color registers */

/*****************************************************************************/
/* Color definitions                                                         */
/*****************************************************************************/

/* Make a GTIA color value */
#define _gtia_mkcolor(hue,lum) (((hue) << 4) | ((lum) << 1))

/* Luminance values go from 0 (black) to 7 (white) */

/* Hue values */
/* (These can vary depending on TV standard (NTSC vs PAL),
** tint potentiometer settings, TV tint settings, emulator palette, etc.)
*/
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
/* Hardware palette values (for GTIA colxxx registers) */
#define GTIA_COLOR_BLACK             _gtia_mkcolor(HUE_GREY,0)
#define GTIA_COLOR_WHITE             _gtia_mkcolor(HUE_GREY,7)
#define GTIA_COLOR_RED               _gtia_mkcolor(HUE_REDORANGE,1)
#define GTIA_COLOR_CYAN              _gtia_mkcolor(HUE_CYAN,3)
#define GTIA_COLOR_VIOLET            _gtia_mkcolor(HUE_PURPLE,4)
#define GTIA_COLOR_GREEN             _gtia_mkcolor(HUE_GREEN,2)
#define GTIA_COLOR_BLUE              _gtia_mkcolor(HUE_BLUE,2)
#define GTIA_COLOR_YELLOW            _gtia_mkcolor(HUE_YELLOW,7)
#define GTIA_COLOR_ORANGE            _gtia_mkcolor(HUE_ORANGE,5)
#define GTIA_COLOR_BROWN             _gtia_mkcolor(HUE_YELLOW,2)
#define GTIA_COLOR_LIGHTRED          _gtia_mkcolor(HUE_REDORANGE,6)
#define GTIA_COLOR_GRAY1             _gtia_mkcolor(HUE_GREY,2)
#define GTIA_COLOR_GRAY2             _gtia_mkcolor(HUE_GREY,3)
#define GTIA_COLOR_LIGHTGREEN        _gtia_mkcolor(HUE_GREEN,6)
#define GTIA_COLOR_LIGHTBLUE         _gtia_mkcolor(HUE_BLUE,6)
#define GTIA_COLOR_GRAY3             _gtia_mkcolor(HUE_GREY,5)


/*****************************************************************************/
/* (W) PRIOR register values                                                 */
/*****************************************************************************/

#define PRIOR_P03_PF03          0x01 /* Players 0-3, then Playfields 0-3, then background */
#define PRIOR_P01_PF03_P23      0x02 /* Players 0-1, then Playfields 0-3, then Players 2-3, then background */
#define PRIOR_PF03_P03          0x04 /* Playfields 0-3, then Players 0-3, then background */
#define PRIOR_PF01_P03_PF23     0x08 /* Playfields 0-1, then Players 0-3, then Playfields 2-3, then background */

#define PRIOR_5TH_PLAYER        0x10 /* Four missiles combine to be a 5th player (uses COLPF3) */

/* Causes overlap of players 0 & 1 and of players 2 & 3 to result in a third color,
** the logical OR of the two players' colors, and other overlaps (e.g., players 0 and 2)
** to result in black (0x00).
*/
#define PRIOR_OVERLAP_3RD_COLOR 0x20


/*****************************************************************************/
/* (W) GTIA special graphics mode options for GPRIOR                         */
/*****************************************************************************/

/* Pixels are 2 color clocks wide, and one scanline tall
** (so 80x192 in normal playfield width).
** May be used with both bitmap and character modelines.
*/

/* 16 shade shades of the background (COLBK) hue;
** Note: brightnesses other than 0 (darkest) in COLBK cause additional effects
*/
#define PRIOR_GFX_MODE_9        0x40

/* 9 color palette mode;
** COLPM0 (acts as background) thru COLPM3, followed by COLPF0 thru COLPF3, and COLBK
*/
#define PRIOR_GFX_MODE_10       0x80

/* 16 hues of the background (COLBK) brightness;
** Note: hues other than 0 (greys) in COLBK caus additional effects
*/
#define PRIOR_GFX_MODE_11       0xC0


/*****************************************************************************/
/* (W) VDELAY register values                                                */
/*****************************************************************************/

#define VDELAY_MISSILE0 0x01
#define VDELAY_MISSILE1 0x02
#define VDELAY_MISSILE2 0x04
#define VDELAY_MISSILE3 0x08
#define VDELAY_PLAYER0  0x10
#define VDELAY_PLAYER1  0x20
#define VDELAY_PLAYER2  0x40
#define VDELAY_PLAYER3  0x80


/*****************************************************************************/
/* (W) GRACTL register values                                                */
/*****************************************************************************/

#define GRACTL_MISSLES              0x01 /* enable missiles */
#define GRACTL_PLAYERS              0x02 /* enable players */

/* "Latch" triggers; once pressed, will give a continuous
** pressed input until this bit is cleared
*/
#define GRACTL_LATCH_TRIGGER_INPUTS 0x04


/*****************************************************************************/
/* Define a structure with the GTIA register offsets for read (R)            */
/*****************************************************************************/

struct __gtia_read {
    unsigned char   m0pf;       /* 0x00: missile 0 to playfield collision */
    unsigned char   m1pf;       /* 0x01: missile 1 to playfield collision */
    unsigned char   m2pf;       /* 0x02: missile 2 to playfield collision */
    unsigned char   m3pf;       /* 0x03: missile 3 to playfield collision */
    unsigned char   p0pf;       /* 0x04: player 0 to playfield collision */
    unsigned char   p1pf;       /* 0x05: player 1 to playfield collision */
    unsigned char   p2pf;       /* 0x06: player 2 to playfield collision */
    unsigned char   p3pf;       /* 0x07: player 3 to playfield collision */
    unsigned char   m0pl;       /* 0x08: missile 0 to player collision */
    unsigned char   m1pl;       /* 0x09: missile 1 to player collision */
    unsigned char   m2pl;       /* 0x0A: missile 2 to player collision */
    unsigned char   m3pl;       /* 0x0B: missile 3 to player collision */
    unsigned char   p0pl;       /* 0x0C: player 0 to player collision */
    unsigned char   p1pl;       /* 0x0D: player 1 to player collision */
    unsigned char   p2pl;       /* 0x0E: player 2 to player collision */
    unsigned char   p3pl;       /* 0x0F: player 3 to player collision */

    unsigned char   trig0;      /* 0x10: joystick trigger 0 (0=pressed, 1=released) */
    unsigned char   trig1;      /* 0x11: joystick trigger 1 */
    unsigned char   trig2;      /* 0x12: joystick trigger 2 */
    unsigned char   trig3;      /* 0x13: joystick trigger 3 */

    unsigned char   pal;        /* 0x14: pal/ntsc flag */

    unsigned char   unused[10];

    unsigned char   consol;     /* 0x1F: console buttons */
};


/*****************************************************************************/
/* (R) PAL register possible values                                          */
/*****************************************************************************/

/* Note: This only tells you whether the GTIA is PAL or NTSC; some NTSC
** systems are modded with PAL ANTIC chips; testing VCOUNT limits can be
** done to check for that.  Seems like it's not possible to test for SECAM
*/

#define TV_STD_PAL  0x1
#define TV_STD_NTSC 0xE


/*****************************************************************************/
/* Macros for reading console keys (Start/Select/Option) via CONSOL register */
/*****************************************************************************/

#define CONSOL_START(x)     !((unsigned char)((x) & 1)) /* true if Start pressed */
#define CONSOL_SELECT(x)    !((unsigned char)((x) & 2)) /* true if Select pressed */
#define CONSOL_OPTION(x)    !((unsigned char)((x) & 4)) /* true if Option pressed */


/* End of _gtia.h */
#endif /* #ifndef __GTIA_H */
