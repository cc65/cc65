/*****************************************************************************/
/*                                                                           */
/*                                asmlabel.c                                 */
/*                                                                           */
/*                      Generate assembler code labels                       */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2009 Ullrich von Bassewitz                                       */
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



#include <stdio.h>
#include <string.h>

/* common */
#include "chartype.h"

/* cc65 */
#include "asmlabel.h"
#include "error.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned GetLocalLabel (void)
/* Get an unused label. Will never return zero. */
{
    /* Number to generate unique labels */
    static unsigned NextLabel = 0;

    /* Check for an overflow */
    if (NextLabel >= 0xFFFF) {
        Internal ("Local label overflow");
    }

    /* Return the next label */
    return ++NextLabel;
}



const char* LocalLabelName (unsigned L)
/* Make a label name from the given label number. The label name will be
** created in static storage and overwritten when calling the function
** again.
*/
{
    static char Buf[64];
    sprintf (Buf, "L%04X", L);
    return Buf;
}



int IsLocalLabelName (const char* Name)
/* Return true if Name is the name of a local label */
{
    unsigned I;

    if (Name[0] != 'L' || strlen (Name) != 5) {
        return 0;
    }
    for (I = 1; I <= 4; ++I) {
        if (!IsXDigit (Name[I])) {
            return 0;
        }
    }

    /* Local label name */
    return 1;
}
