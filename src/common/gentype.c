/*****************************************************************************/
/*                                                                           */
/*                                 gentype.c                                 */
/*                                                                           */
/*                        Generic data type encoding                         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2011,      Ullrich von Bassewitz                                      */
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



/* common */
#include "gentype.h"
#include "strbuf.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void GT_AddArray (StrBuf* Type, unsigned ArraySize)
/* Add an array with the given size to the type string in Type. This will
 * NOT add the element type!
 */
{
    unsigned I;

    /* Add the array token */
    SB_AppendChar (Type, GT_TYPE_ARRAY);

    /* Add the size. */
    for (I = 0; I < 4; ++I) {
        SB_AppendChar (Type, ArraySize & 0xFF);
        ArraySize >>= 8;
    }
}



unsigned GT_GetArraySize (StrBuf* Type)
/* Retrieve the size of an array stored in Type at the current index position.
 * The index position will get moved past the array size.
 */
{
    unsigned Size;
    Size  = (unsigned)SB_Get (Type);
    Size |= (unsigned)SB_Get (Type) << 8;
    Size |= (unsigned)SB_Get (Type) << 16;
    Size |= (unsigned)SB_Get (Type) << 24;
    return Size;
}






