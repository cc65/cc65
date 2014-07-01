/*****************************************************************************/
/*                                                                           */
/*                                 tgi_arc.c                                 */
/*                                                                           */
/*                            Draw an ellipse arc                            */
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
#include <tgi/tgi-kernel.h>
#include <cc65.h>



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void __fastcall__ tgi_arc (int x, int y, unsigned char rx, unsigned char ry,
                           unsigned sa, unsigned ea)
/* Draw an ellipse arc with center at x/y and radii rx/ry using the current
** drawing color. The arc covers the angle between sa and ea (startangle and
** endangle), which must be in the range 0..360 (otherwise the function may
** bevave unextectedly).
*/
{
    int x1, y1, x2, y2;
    unsigned char inc;
    unsigned char done = 0;

    /* Bail out if there's nothing to do */
    if (sa > ea) {
        return;
    }

    /* Determine the number of segments to use. This may be refined ... */
    if (rx + ry >= 25) {
        inc = 12;
    } else {
        inc = 24;
    }

    /* Calculate the start coords */
    x1 = x + tgi_imulround (rx, cc65_cos (sa));
    y1 = y - tgi_imulround (ry, cc65_sin (sa));
    do {
        sa += inc;
        if (sa >= ea) {
            sa = ea;
            done = 1;
        }
        x2 = x + tgi_imulround (rx, cc65_cos (sa));
        y2 = y - tgi_imulround (ry, cc65_sin (sa));
        tgi_line (x1, y1, x2, y2);
        x1 = x2;
        y1 = y2;
    } while (!done);
}



