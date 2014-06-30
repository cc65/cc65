/*****************************************************************************/
/*                                                                           */
/*                                  util.c                                   */
/*                                                                           */
/*                 Utility functions for the cc65 C compiler                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2004 Ullrich von Bassewitz                                       */
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



#include "util.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int PowerOf2 (unsigned long Val)
/* Return the exponent if val is a power of two. Return -1 if val is not a
** power of two.
*/
{
    int I;
    unsigned long Mask = 0x0001;

    for (I = 0; I < 32; ++I) {
        if (Val == Mask) {
            return I;
        }
        Mask <<= 1;
    }
    return -1;
}
