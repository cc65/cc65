/*****************************************************************************/
/*                                                                           */
/*                                  _ted.h                                   */
/*                                                                           */
/*                Internal include file, do not use directly                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
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



#ifndef __TED_H
#define __TED_H



/* Define a structure with the ted register offsets */
struct __ted {
    unsigned char       t1_lo;          /* Timer #1 low */
    unsigned char       t1_hi;          /* Timer #1 high */
    unsigned char       t2_lo;          /* Timer #2 low */
    unsigned char       t2_hi;          /* Timer #2 high */
    unsigned char       t3_lo;          /* Timer #3 low */
    unsigned char       t3_hi;          /* Timer #3 high */
    unsigned char       vscroll;        /* Vertical scroll control */
    unsigned char       hscroll;        /* Horizontal scroll control */
    unsigned char       kbdlatch;       /* Keyboard latch */
    unsigned char       irr;            /* Interrupt request register */
    unsigned char       imr;            /* Interrupt mask register */
    unsigned char       irq_rasterline; /* Interrupt rasterline */
    unsigned char       cursor_hi;      /* Cursor position high */
    unsigned char       cursor_lo;      /* Cursor position low */
    unsigned char       snd1_freq_lo;   /* Channel #1 frequency */
    unsigned char       snd2_freq_lo;   /* Channel #2 frequency low */
    unsigned char       snd2_freq_hi;   /* Channel #2 frequency high */
    unsigned char       snd_ctrl;       /* Sound control */
    unsigned char       misc;           /* Channel #1 frequency high and more */
    unsigned char       char_addr;      /* Character data base address */
    unsigned char       video_addr;     /* Video memory base address */
    unsigned char       bgcolor;        /* Background color */
    unsigned char       color1;         /* Color register #1 */
    unsigned char       color2;         /* Color register #2 */
    unsigned char       color3;         /* Color register #3 */
    unsigned char       bordercolor;    /* Border color */
    unsigned char       bmap_reload_hi; /* Bitmap reload bits 8+9 */
    unsigned char       bmap_reload_lo; /* Bitmap reload bits 0-7 */
    unsigned char       rasterline_hi;  /* Current rasterline bit 8 */
    unsigned char       rasterline_lo;  /* Current rasterline bits 0-7 */
    unsigned char       rastercolumn;   /* Current rastercolumn */
    unsigned char       cursor_blink;   /* Cursor blink attribute */
    unsigned char       unused[30];     /* Unused */
    unsigned char       enable_rom;     /* Write enables ROM */
    unsigned char       enable_ram;     /* Write enables RAM */
};



/* End of _ted.h */
#endif



