/*****************************************************************************/
/*                                                                           */
/*                              tgi_pieslice.c                               */
/*                                                                           */
/*                         Draw an ellipic pie slice                         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2009,      Ullrich von Bassewitz                                      */
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



void __fastcall__ tgi_pieslice (int x, int y, unsigned char rx, unsigned char ry,
                                unsigned sa, unsigned ea)
/* Draw an ellipse pie slice with center at x/y and radii rx/ry using the
** current drawing color. The pie slice covers the angle between sa and ea
** (startangle and endangle), which must be in the range 0..360 (otherwise the
** function may behave unexpectedly).
*/
{
    /* Draw an arc ... */
    tgi_arc (x, y, rx, ry, sa, ea);

    /* ... and close it */
    tgi_line (x, y, x + tgi_imulround (rx, cc65_cos (sa)), y - tgi_imulround (ry, cc65_sin (sa)));
    tgi_line (x, y, x + tgi_imulround (rx, cc65_cos (ea)), y - tgi_imulround (ry, cc65_sin (ea)));
}



