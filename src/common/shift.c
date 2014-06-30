/*****************************************************************************/
/*                                                                           */
/*                                  shift.c                                  */
/*                                                                           */
/*                            Safe shift routines                            */
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



/* According to the C standard, shifting a data type by the number of bits it
** has causes undefined behaviour. So
**
**      unsigned long l = 1;
**      unsigned u =32;
**      l <<= u;
**
** may be illegal. The functions in this module behave safely in that respect,
** and they use proper casting to distinguish signed from unsigned shifts.
** They are not a general purpose replacement for the shift operator!
*/



#include <limits.h>

/* common */
#include "shift.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



long asl_l (long l, unsigned count)
/* Arithmetic shift left l by count. */
{
    while (1) {
        if (count >= CHAR_BIT * sizeof (l)) {
            l <<= (CHAR_BIT * sizeof (l) - 1);
            count -= (CHAR_BIT * sizeof (l) - 1);
        } else {
            l <<= count;
            break;
        }
    }
    return l;
}



long asr_l (long l, unsigned count)
/* Arithmetic shift right l by count */
{
    while (1) {
        if (count >= CHAR_BIT * sizeof (l)) {
            l >>= (CHAR_BIT * sizeof (l) - 1);
            count -= (CHAR_BIT * sizeof (l) - 1);
        } else {
            l >>= count;
            break;
        }
    }
    return l;
}



unsigned long shl_l (unsigned long l, unsigned count)
/* Logical shift left l by count */
{
    while (1) {
        if (count >= CHAR_BIT * sizeof (l)) {
            l <<= (CHAR_BIT * sizeof (l) - 1);
            count -= (CHAR_BIT * sizeof (l) - 1);
        } else {
            l <<= count;
            break;
        }
    }
    return l;
}



unsigned long shr_l (unsigned long l, unsigned count)
/* Logical shift right l by count */
{
    while (1) {
        if (count >= CHAR_BIT * sizeof (l)) {
            l >>= (CHAR_BIT * sizeof (l) - 1);
            count -= (CHAR_BIT * sizeof (l) - 1);
        } else {
            l >>= count;
            break;
        }
    }
    return l;
}
