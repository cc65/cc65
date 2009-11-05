/*****************************************************************************/
/*                                                                           */
/*                               tgi_ellipse.c                               */
/*                                                                           */
/*                            Draw a full ellipse                            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2009, Ullrich von Bassewitz                                      */
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



#include <tgi.h>
#include <cc65.h>



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static int RoundMul (int rhs, int lhs)
{
    long res = cc65_imul16x16r32 (rhs, lhs);
    if ((unsigned char)res & 0x80) {
        return (int)(res >> 8) + 1;
    } else {
        return (int)(res >> 8);
    }
}



void __fastcall__ tgi_ellipse (int x, int y, unsigned char rx, unsigned char ry)
/* Draw a full ellipse with center at x/y and radii rx/ry using the current
 * drawing color.
 */
{
    int x1, y1, x2, y2;
    unsigned angle;
    unsigned char inc;
    unsigned size = rx + ry;

    if (size >= 128) {
        inc = 12;
    } else if (size >= 32) {
        inc = 15;
    } else if (size >= 12) {
        inc = 20;
    } else {
        inc = 45;
    }

    x1 = x + rx;
    y1 = y;
    angle = 0;
    for (angle = 0; angle <= 360; angle += inc) {
        x2 = x + RoundMul (rx, cc65_cos (angle));
        y2 = y + RoundMul (ry, cc65_sin (angle));
        tgi_line (x1, y1, x2, y2);
        x1 = x2;
        y1 = y2;
    }
}



