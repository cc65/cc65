/*****************************************************************************/
/*                                                                           */
/*                               lynxpalette.c                               */
/*                                                                           */
/*    Lynx palette backend for the sp65 sprite and bitmap utility            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2022,      Karri Kaksonen                                             */
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



#include <stdlib.h>

/* common */
#include "attrib.h"
#include "print.h"

/* sp65 */
#include "attr.h"
#include "error.h"
#include "palette.h"
#include "lynxpalette.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/


/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/


StrBuf* GenLynxPalette (const Bitmap* B, const Collection* A)
/* Generate binary output in Lynx palette format for the bitmap B. The output
** is stored in a string buffer (which is actually a dynamic char array) and
** returned.
**
*/
{
    StrBuf* D;
    const Palette* P = GetBitmapPalette (B);
    const char* Format = GetAttrVal(A, "format");
    unsigned I;

    if (Format == 0) {
        /* No format specified */
    }
    D = NewStrBuf ();
    for (I = 0; I < 16; ++I) {

        /* Get the color entry */
        const Color* C = P->Entries + I;

        /* Add the green component */
        SB_AppendChar (D, C->G >> 4);
    }
    for (I = 0; I < 16; ++I) {

        /* Get the color entry */
        const Color* C = P->Entries + I;

        /* Add the blue,red component */
        SB_AppendChar (D, (C->B & 0xF0) | (C->R >> 4));
    }

    /* Return the converted palette */
    return D;
}

