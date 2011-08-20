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
#include "xmalloc.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



gt_string GT_FromStrBuf (const struct StrBuf* S)
/* Create a dynamically allocated type string from a string buffer. */
{
    /* To avoid silly mistakes, check if the last character in S is a
     * terminator. If not, don't rely on S being terminated.
     */
    unsigned Len = SB_GetLen (S);
    if (Len > 0 && SB_LookAtLast (S) == '\0') {
        /* String is terminated - allocate memory */
        gt_string Type = xmalloc (Len);
        /* Copy the data and return the result */
        return memcpy (Type, SB_GetConstBuf (S), Len);
    } else {
        /* String not terminated - allocate memory */
        gt_string Type = xmalloc (Len + 1);
        /* Copy the data */
        memcpy (Type, SB_GetConstBuf (S), Len);
        /* Terminate the string */
        Type[Len] = GT_END;
        /* Return the copy */
        return Type;
    }
}



