/*****************************************************************************/
/*                                                                           */
/*                                  segnames.h                               */
/*                                                                           */
/*                           Default segment names                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
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



#include <string.h>

/* common */
#include "chartype.h"
#include "segnames.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int ValidSegName (const char* Name)
/* Return true if the given segment name is valid, return false otherwise */
{
    /* Must start with '_' or a letter */
    if ((*Name != '_' && !IsAlpha(*Name)) || strlen(Name) > 80) {
        return 0;
    }

    /* Can have letters, digits or the underline */
    while (*++Name) {
        if (*Name != '_' && !IsAlNum(*Name)) {
            return 0;
        }
    }

    /* Name is ok */
    return 1;
}
