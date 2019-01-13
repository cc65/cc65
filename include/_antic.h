/*****************************************************************************/
/*                                                                           */
/*                                _antic.h                                   */
/*                                                                           */
/*                  Internal include file, do not use directly               */
/*                                                                           */
/*                                                                           */
/* "ANTIC is responsible for the generation of playfield graphics which is   */
/* delivered as a datastream to the related CTIA/GTIA chip. The CTIA/GTIA    */
/* provides the coloring of the playfield graphics, and is responsible for   */
/* adding overlaid sprites referred to as "Player/Missile graphics" by       */
/* Atari.  Atari advertised it as a true microprocessor, in that it has an   */
/* instruction set to run programs (called display lists) to process data.   */
/* ANTIC has no capacity for writing back computed values to memory, it      */
/* merely reads data from memory and processes it for output to the screen,  */
/* therefore it is not Turing complete." - Wikipedia article on "ANTIC"      */
/*                                                                           */
/* (C) 2000 Freddy Offenga <taf_offenga@yahoo.com>                           */
/* 24-Jan-2011: Christian Krueger: Added defines for Antic instruction set   */
/* 2019-01-12: Bill Kendrick <nbs@sonic.net>: More defines for registers     */
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


#ifndef __ANTIC_H
#define __ANTIC_H

/* Define a structure with the ANTIC coprocessor's register offsets */
struct __antic {
    unsigned char   dmactl; /* (W) direct memory access control */

/* Playfield modes: */
#define DMACTL_PLAYFIELD_NONE     0x00
#define DMACTL_PLAYFIELD_NARROW   0x01 /* e.g., 32 bytes per scanline with thick borders */
#define DMACTL_PLAYFIELD_NORMAL   0x02 /* e.g., 40 bytes per scanline with normal borders */
#define DMACTL_PLAYFIELD_WIDE     0x03 /* e.g., 48 bytes per scanline with no borders (overscan) */
/* Other options: */
#define DMACTL_DMA_MISSILES    0x04 /* if not set, GTIA's GRAFP0 thru GRAFP3 used for player shapes; if set, ANTIC's PMBASE will be used to fetch shapes via DMA */
#define DMACTL_DMA_PLAYERS     0x08 /* (ditto, using GTIA's GRAFM for missile shapes...) */
#define DMACTL_PMG_SINGLELINE  0x10 /* if not set, default is double-scanline resolution PMGs */
#define DMACTL_DMA_FETCH       0x20 /* if not set, disables ANTIC operation since it cannot fetch Display List instructions */
/* Initialized to 0x22 (DMA fetch, normal playfield, no PMG DMA, double-line PMGs) */


    unsigned char   chactl; /* (W) character mode control */

/* Inverted (upside-down) characters */
#define CHACTL_CHAR_NORMAL    0x00
#define CHACTL_CHAR_INVERTED  0x04
/* Inverse (reverse-video) characters */
#define CHACTL_INV_TRANS      0x00 /* chars with high-bit shown */
#define CHACTL_INV_OPAQUE     0x01 /* chars with high-bit appear as space */
#define CHACTL_INV_PRESENT    0x02 /* chars with high-bit are reverse-video */
/* N.B. Default is "CHACTL_CHAR_NORMAL | CHACTL_INV_PRESENT", aka decimal 2 */


    unsigned char   dlistl; /* display list pointer low-byte */
    unsigned char   dlisth; /* display list pointer high-byte */
    unsigned char   hscrol; /* (W) horizontal scroll enable */
    unsigned char   vscrol; /* (W) vertical scroll enable */
    unsigned char   unuse0; /* unused */
    unsigned char   pmbase; /* (W) msb of p/m base address (for when DMACTL has player and/or missile DMA enabled) */
    unsigned char   unuse1; /* unused */
    unsigned char   chbase; /* (W) character set base address */
    unsigned char   wsync;  /* (W) wait for horizontal synchronization */
    unsigned char   vcount; /* (R) vertical line counter */
    unsigned char   penh;   /* (R) light pen horizontal position */
    unsigned char   penv;   /* (R) light pen vertical position */

    unsigned char   nmien;  /* (W) non-maskable interrupt enable */

/* NMIEN settings: */
#define NMIEN_DLI   0x80 /* see also: DL_DLI */
#define NMIEN_VBI   0x40
#define NMIEN_RESET 0x20

    unsigned char   nmires; /* (W) ("NMIRES") nmi reset; (R) ("NMIST") nmi status */
};


/* ANTIC instruction set */

/* Absolute instructions (non mode lines) */
#define DL_JMP  ((unsigned char) 1)
#define DL_JVB  ((unsigned char) 65)

#define DL_BLK1 ((unsigned char) 0)   /* 1 blank scanline */
#define DL_BLK2 ((unsigned char) 16)  /* 2 blank scanlines */
#define DL_BLK3 ((unsigned char) 32)  /* ...etc. */
#define DL_BLK4 ((unsigned char) 48)
#define DL_BLK5 ((unsigned char) 64)
#define DL_BLK6 ((unsigned char) 80)
#define DL_BLK7 ((unsigned char) 96)
#define DL_BLK8 ((unsigned char) 112)

/* Absolute instructions (mode lines) */
/* Note: Actual width varies (e.g., 40 vs 32 vs 48) depending on normal vs narrow vs wide (overscan) playfield setting; see DMACTL */

/* Character modes (text, tile graphics, etc.) */
#define DL_CHR40x8x1    ((unsigned char) 2)      /* monochrome, 40 character & 8 scanlines per mode line (aka Atari BASIC GRAPHICS 0 via OS's CIO routines) */
#define DL_CHR40x10x1   ((unsigned char) 3)      /* monochrome, 40 character & 10 scanlines per mode line (like GR. 0, with descenders) */
#define DL_CHR40x8x4    ((unsigned char) 4)      /* colour, 40 character & 8 scanlines per mode line (GR. 12) */
#define DL_CHR40x16x4   ((unsigned char) 5)      /* colour, 40 character & 16 scanlines per mode line (GR. 13) */
#define DL_CHR20x8x2    ((unsigned char) 6)      /* colour (duochrome per character), 20 character & 8 scanlines per mode line (GR. 1) */
#define DL_CHR20x16x2   ((unsigned char) 7)      /* colour (duochrome per character), 20 character & 16 scanlines per mode line (GR. 2) */

/* Bitmap modes */
#define DL_MAP40x8x4    ((unsigned char) 8)      /* colour, 40 pixel & 8 scanlines per mode line (GR. 3) */
#define DL_MAP80x4x2    ((unsigned char) 9)      /* 'duochrome', 80 pixel & 4 scanlines per mode line (GR.4) */
#define DL_MAP80x4x4    ((unsigned char) 10)     /* colour, 80 pixel & 4 scanlines per mode line (GR.5) */
#define DL_MAP160x2x2   ((unsigned char) 11)     /* 'duochrome', 160 pixel & 2 scanlines per mode line (GR.6) */
#define DL_MAP160x1x2   ((unsigned char) 12)     /* 'duochrome', 160 pixel & 1 scanline per mode line (GR.14) */
#define DL_MAP160x2x4   ((unsigned char) 13)     /* 4 colours, 160 pixel & 2 scanlines per mode line (GR.7) */
#define DL_MAP160x1x4   ((unsigned char) 14)     /* 4 colours, 160 pixel & 1 scanline per mode line (GR.15) */
#define DL_MAP320x1x1   ((unsigned char) 15)     /* monochrome, 320 pixel & 1 scanline per mode line (GR.8) */

/* Equivalents, for people familiar with Atari 8-bit OS */
#define DL_GRAPHICS0    DL_CHR40x8x1
#define DL_GRAPHICS1    DL_CHR20x8x2
#define DL_GRAPHICS2    DL_CHR20x16x2
#define DL_GRAPHICS3    DL_MAP40x8x4
#define DL_GRAPHICS4    DL_MAP80x4x2
#define DL_GRAPHICS5    DL_MAP80x4x4
#define DL_GRAPHICS6    DL_MAP160x2x2
#define DL_GRAPHICS7    DL_MAP160x2x4
#define DL_GRAPHICS8    DL_MAP320x1x1
#define DL_GRAPHICS9    DL_MAP320x1x1  /* N.B.: GRAPHICS 9, 10, and 11 also involve GTIA's PRIOR register */
#define DL_GRAPHICS10   DL_MAP320x1x1
#define DL_GRAPHICS11   DL_MAP320x1x1
#define DL_GRAPHICS12   DL_CHR40x8x4   /* N.B.: Atari 400/800 OS didn't have GRAPHICS 12 or 13 */
#define DL_GRAPHICS13   DL_CHR40x16x4
#define DL_GRAPHICS14   DL_MAP160x1x2
#define DL_GRAPHICS15   DL_MAP160x1x4

/* Atari 400/800 OS didn't have GRAPHICS 14 or 15, so they were known by "6+" and "7+" */
#define DL_GRAPHICS6PLUS DL_GRAPHICS14
#define DL_GRAPHICS7PLUS DL_GRAPHICS15

/* Neither Atari 400/800 nor XL OS supported 10-scanline (descenders) text mode via CIO */
#define DL_GRAPHICS0_DESCENDERS  DL_CHR40x10x1

/* Modifiers to mode lines */
#define DL_HSCROL(x)    ((unsigned char)((x) | 16)) /* enable smooth horizontal scrolling on this line; see HSCROL */
#define DL_VSCROL(x)    ((unsigned char)((x) | 32)) /* enable smooth vertical scrolling on this line; see VSCROL */
#define DL_LMS(x)       ((unsigned char)((x) | 64)) /* Load Memory Scan (next two bytes must be the LSB/MSB of the data to load */

/* General modifier */
#define DL_DLI(x)       ((unsigned char)((x) | 128)) /* enable Display List Interrupt on this mode line; requires NMIEN set to enable DLIs */


/* Macros for the beginning and end of functions used as Display List Interrupts */
#define DLI_START asm("pha"); asm("txa"); asm("pha"); asm("tya"); asm("pha");
#define DLI_END asm("pla"); asm("tay"); asm("pla"); asm("tax"); asm("pla"); asm("rti");

/* End of _antic.h */
#endif /* #ifndef __ANTIC_H */
