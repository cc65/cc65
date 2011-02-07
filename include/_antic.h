/*****************************************************************************/
/*                                                                           */
/*                                _antic.h                                   */
/*                                                                           */
/*                  Internal include file, do not use directly               */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000 Freddy Offenga <taf_offenga@yahoo.com>                           */
/* 24-Jan-2011: Christian Krueger: Added defines for Antic instruction set   */
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

/* Define a structure with the antic register offsets */
struct __antic {
    unsigned char   dmactl; /* direct memory access control */
    unsigned char   chactl; /* character mode control */
    unsigned char   dlistl; /* display list pointer low-byte */
    unsigned char   dlisth; /* display list pointer high-byte */
    unsigned char   hscrol; /* horizontal scroll enable */
    unsigned char   vscrol; /* vertical scroll enable */
    unsigned char   unuse0; /* unused */
    unsigned char   pmbase; /* msb of p/m base address */
    unsigned char   unuse1; /* unused */
    unsigned char   chbase; /* character base address */
    unsigned char   wsync;  /* wait for horizontal synchronization */
    unsigned char   vcount; /* vertical line counter */
    unsigned char   penh;   /* light pen horizontal position */
    unsigned char   penv;   /* light pen vertical position */
    unsigned char   nmien;  /* non-maskable interrupt enable */
    unsigned char   nmires; /* nmi reset/status */
};


/* antic instruction set */

/* absolute instructions (non mode lines) */
#define DL_JMP  1
#define DL_JVB  65

#define DL_BLK1 0
#define DL_BLK2 16
#define DL_BLK3 32
#define DL_BLK4 48
#define DL_BLK5 64
#define DL_BLK6 80
#define DL_BLK7 96
#define DL_BLK8 112

/* absolute instructions (mode lines) */
#define DL_CHR40x8x1    2       /* monochrome, 40 character & 8 scanlines per mode line (GR. 0) */
#define DL_CHR40x10x1   3       /* monochrome, 40 character & 10 scanlines per mode line */
#define DL_CHR40x8x4    4       /* colour, 40 character & 8 scanlines per mode line (GR. 12) */
#define DL_CHR40x16x4   5       /* colour, 40 character & 16 scanlines per mode line (GR. 13) */
#define DL_CHR20x8x2    6       /* colour (duochrome per character), 20 character & 8 scanlines per mode line (GR. 1) */
#define DL_CHR20x16x2   7       /* colour (duochrome per character), 20 character & 16 scanlines per mode line (GR. 2) */

#define DL_MAP40x8x4    8       /* colour, 40 pixel & 8 scanlines per mode line (GR. 3) */
#define DL_MAP80x4x2    9       /* 'duochrome', 80 pixel & 4 scanlines per mode line (GR.4) */
#define DL_MAP80x4x4    10      /* colour, 80 pixel & 4 scanlines per mode line (GR.5) */
#define DL_MAP160x2x2   11      /* 'duochrome', 160 pixel & 2 scanlines per mode line (GR.6) */
#define DL_MAP160x1x2   12      /* 'duochrome', 160 pixel & 1 scanline per mode line (GR.14) */
#define DL_MAP160x2x4   13      /* 4 colours, 160 pixel & 2 scanlines per mode line (GR.7) */
#define DL_MAP160x1x4   14      /* 4 colours, 160 pixel & 1 scanline per mode line (GR.15) */
#define DL_MAP320x1x1   15      /* monochrome, 320 pixel & 1 scanline per mode line (GR.8) */

/* modifiers on mode lines */
#define DL_HSCROL       16
#define DL_VSCROL       32
#define DL_LMS          64

/* general modifier */
#define DL_DLI          128

/* End of _antic.h */
#endif /* #ifndef __ANTIC_H */
