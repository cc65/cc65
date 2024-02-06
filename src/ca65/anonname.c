/*****************************************************************************/
/*                                                                           */
/*                                anonname.c                                 */
/*                                                                           */
/*             Create names for anonymous scopes/variables/types             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2008 Ullrich von Bassewitz                                       */
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

/* ca65 */
#include "anonname.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



static const char AnonTag[] = "$anon";



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



StrBuf* AnonName (StrBuf* Buf, const char* Spec)
/* Get a name for an anonymous scope, variable or type. Size is the size of
** the buffer passed to the function, Spec will be used as part of the
** identifier if given. A pointer to the buffer is returned.
*/
{
    static unsigned ACount = 0;
    SB_Printf (Buf, "%s-%s-%04X", AnonTag, Spec, ++ACount);
    return Buf;
}



int IsAnonName (const StrBuf* Name)
/* Check if the given symbol name is that of an anonymous symbol */
{
    if (SB_GetLen (Name) < sizeof (AnonTag) - 1) {
        /* Too short */
        return 0;
    }
    return (strncmp (SB_GetConstBuf (Name), AnonTag, sizeof (AnonTag) - 1) == 0);
}
