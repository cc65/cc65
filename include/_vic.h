/*****************************************************************************/
/*                                                                           */
/*                                  _vic.h                                   */
/*                                                                           */
/*                Internal include file, do not use directly                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



#ifndef __VIC_H
#define __VIC_H



/* Define a structure with the vic register offsets */
struct __vic {
    unsigned char       leftborder;
    unsigned char       upperborder;
    unsigned char       charsperline;   /* Characters per line */
    unsigned char       linecount;      /* Number of lines */
    unsigned char       rasterline;     /* Current raster line */
    unsigned char       addr;           /* Address of chargen and video ram */
    unsigned char       strobe_x;       /* Light pen, X position */
    unsigned char       strobe_y;       /* Light pen, Y position */
    unsigned char       analog_x;       /* Analog input X */
    unsigned char       analog_y;       /* Analog input Y */
    unsigned char       voice1;         /* Sound generator #1 */
    unsigned char       voice2;         /* Sound generator #2 */
    unsigned char       voice3;         /* Sound generator #3 */
    unsigned char       noise;          /* Noise generator */
    unsigned char       volume_color;   /* Bits 0..3: volume, 4..7: color */
    unsigned char       bg_border_color;/* Background and border color */
};



/* End of _vic.h */
#endif



