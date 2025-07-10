/*****************************************************************************/
/*                                                                           */
/*                                  _vic3.h                                  */
/*                                                                           */
/*                Internal include file, do not use directly                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2012, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
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



#ifndef __VIC3_H
#define __VIC3_H

/* FIXME: only VIC2 registers right now */

/* Define a structure with the vic register offsets. In cc65 mode, there
** are aliases for the field accessible as arrays.
*/
#if __CC65_STD__ == __CC65_STD_CC65__
struct __vic3 {
    union {
        struct {
            unsigned char   spr0_x;         /* Sprite 0, X coordinate */
            unsigned char   spr0_y;         /* Sprite 0, Y coordinate */
            unsigned char   spr1_x;         /* Sprite 1, X coordinate */
            unsigned char   spr1_y;         /* Sprite 1, Y coordinate */
            unsigned char   spr2_x;         /* Sprite 2, X coordinate */
            unsigned char   spr2_y;         /* Sprite 2, Y coordinate */
            unsigned char   spr3_x;         /* Sprite 3, X coordinate */
            unsigned char   spr3_y;         /* Sprite 3, Y coordinate */
            unsigned char   spr4_x;         /* Sprite 4, X coordinate */
            unsigned char   spr4_y;         /* Sprite 4, Y coordinate */
            unsigned char   spr5_x;         /* Sprite 5, X coordinate */
            unsigned char   spr5_y;         /* Sprite 5, Y coordinate */
            unsigned char   spr6_x;         /* Sprite 6, X coordinate */
            unsigned char   spr6_y;         /* Sprite 6, Y coordinate */
            unsigned char   spr7_x;         /* Sprite 7, X coordinate */
            unsigned char   spr7_y;         /* Sprite 7, Y coordinate */
        };
        struct {
            unsigned char   x;              /* X coordinate */
            unsigned char   y;              /* Y coordinate */
        } spr_pos[8];
    };
    unsigned char           spr_hi_x;       /* High bits of X coordinate */
    unsigned char           ctrl1;          /* Control register 1 */
    unsigned char           rasterline;     /* Current raster line */
    union {
        struct {
            unsigned char   strobe_x;       /* Light pen, X position */
            unsigned char   strobe_y;       /* Light pen, Y position */
        };
        struct {
            unsigned char   x;              /* Light pen, X position */
            unsigned char   y;              /* Light pen, Y position */
        } strobe;
    };
    unsigned char           spr_ena;        /* Enable sprites */
    unsigned char           ctrl2;          /* Control register 2 */
    unsigned char           spr_exp_y;      /* Expand sprites in Y dir */
    unsigned char           addr;           /* Address of chargen and video ram */
    unsigned char           irr;            /* Interrupt request register */
    unsigned char           imr;            /* Interrupt mask register */
    unsigned char           spr_bg_prio;    /* Priority to background */
    unsigned char           spr_mcolor;     /* Sprite multicolor bits */
    unsigned char           spr_exp_x;      /* Expand sprites in X dir */
    unsigned char           spr_coll;       /* Sprite/sprite collision reg */
    unsigned char           spr_bg_coll;    /* Sprite/background collision reg */
    unsigned char           bordercolor;    /* Border color */
    union {
        struct {
            unsigned char   bgcolor0;       /* Background color 0 */
            unsigned char   bgcolor1;       /* Background color 1 */
            unsigned char   bgcolor2;       /* Background color 2 */
            unsigned char   bgcolor3;       /* Background color 3 */
        };
        unsigned char       bgcolor[4];     /* Background colors */
    };
    union {
        struct {
            unsigned char   spr_mcolor0;    /* Color 0 for multicolor sprites */
            unsigned char   spr_mcolor1;    /* Color 1 for multicolor sprites */
        };
        /* spr_color is already used ... */
        unsigned char       spr_mcolors[2]; /* Color for multicolor sprites */
    };
    union {
        struct {
            unsigned char   spr0_color;     /* Color sprite 0 */
            unsigned char   spr1_color;     /* Color sprite 1 */
            unsigned char   spr2_color;     /* Color sprite 2 */
            unsigned char   spr3_color;     /* Color sprite 3 */
            unsigned char   spr4_color;     /* Color sprite 4 */
            unsigned char   spr5_color;     /* Color sprite 5 */
            unsigned char   spr6_color;     /* Color sprite 6 */
            unsigned char   spr7_color;     /* Color sprite 7 */
        };
        unsigned char       spr_color[8];   /* Colors for the sprites */
    };

    /* The following ones are only valid in the C128: */
    unsigned char           x_kbd;          /* Additional keyboard lines */
    unsigned char           clock;          /* Clock switch bit */
};
#else
struct __vic3 {
    unsigned char           spr0_x;         /* Sprite 0, X coordinate */
    unsigned char           spr0_y;         /* Sprite 0, Y coordinate */
    unsigned char           spr1_x;         /* Sprite 1, X coordinate */
    unsigned char           spr1_y;         /* Sprite 1, Y coordinate */
    unsigned char           spr2_x;         /* Sprite 2, X coordinate */
    unsigned char           spr2_y;         /* Sprite 2, Y coordinate */
    unsigned char           spr3_x;         /* Sprite 3, X coordinate */
    unsigned char           spr3_y;         /* Sprite 3, Y coordinate */
    unsigned char           spr4_x;         /* Sprite 4, X coordinate */
    unsigned char           spr4_y;         /* Sprite 4, Y coordinate */
    unsigned char           spr5_x;         /* Sprite 5, X coordinate */
    unsigned char           spr5_y;         /* Sprite 5, Y coordinate */
    unsigned char           spr6_x;         /* Sprite 6, X coordinate */
    unsigned char           spr6_y;         /* Sprite 6, Y coordinate */
    unsigned char           spr7_x;         /* Sprite 7, X coordinate */
    unsigned char           spr7_y;         /* Sprite 7, Y coordinate */
    unsigned char           spr_hi_x;       /* High bits of X coordinate */
    unsigned char           ctrl1;          /* Control register 1 */
    unsigned char           rasterline;     /* Current raster line */
    unsigned char           strobe_x;       /* Light pen, X position */
    unsigned char           strobe_y;       /* Light pen, Y position */
    unsigned char           spr_ena;        /* Enable sprites */
    unsigned char           ctrl2;          /* Control register 2 */
    unsigned char           spr_exp_y;      /* Expand sprites in Y dir */
    unsigned char           addr;           /* Address of chargen and video ram */
    unsigned char           irr;            /* Interrupt request register */
    unsigned char           imr;            /* Interrupt mask register */
    unsigned char           spr_bg_prio;    /* Priority to background */
    unsigned char           spr_mcolor;     /* Sprite multicolor bits */
    unsigned char           spr_exp_x;      /* Expand sprites in X dir */
    unsigned char           spr_coll;       /* Sprite/sprite collision reg */
    unsigned char           spr_bg_coll;    /* Sprite/background collision reg */
    unsigned char           bordercolor;    /* Border color */
    unsigned char           bgcolor0;       /* Background color 0 */
    unsigned char           bgcolor1;       /* Background color 1 */
    unsigned char           bgcolor2;       /* Background color 2 */
    unsigned char           bgcolor3;       /* Background color 3 */
    unsigned char           spr_mcolor0;    /* Color 0 for multicolor sprites */
    unsigned char           spr_mcolor1;    /* Color 1 for multicolor sprites */
    unsigned char           spr0_color;     /* Color sprite 0 */
    unsigned char           spr1_color;     /* Color sprite 1 */
    unsigned char           spr2_color;     /* Color sprite 2 */
    unsigned char           spr3_color;     /* Color sprite 3 */
    unsigned char           spr4_color;     /* Color sprite 4 */
    unsigned char           spr5_color;     /* Color sprite 5 */
    unsigned char           spr6_color;     /* Color sprite 6 */
    unsigned char           spr7_color;     /* Color sprite 7 */
};
#endif



/* End of _vic3.h */
#endif



