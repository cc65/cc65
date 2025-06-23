/*****************************************************************************/
/*                                                                           */
/*                                 error.c                                   */
/*                                                                           */
/*                    Error handling for the ld65 linker                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2008 Ullrich von Bassewitz                                       */
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
#include <stdlib.h>
#include <stdarg.h>

/* common */
#include "cmdline.h"
#include "strbuf.h"

/* ld65 */
#include "error.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Statistics */
unsigned WarningCount   = 0;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void Warning (const char* Format, ...)
/* Print a warning message */
{
    StrBuf S = STATIC_STRBUF_INITIALIZER;
    va_list ap;

    va_start (ap, Format);
    SB_VPrintf (&S, Format, ap);
    va_end (ap);
    SB_Terminate (&S);

    fprintf (stderr, "%s: Warning: %s\n", ProgName, SB_GetConstBuf (&S));

    SB_Done (&S);

    /* Count warnings */
    ++WarningCount;
}



void Error (const char* Format, ...)
/* Print an error message and die */
{
    StrBuf S = STATIC_STRBUF_INITIALIZER;
    va_list ap;

    va_start (ap, Format);
    SB_VPrintf (&S, Format, ap);
    va_end (ap);
    SB_Terminate (&S);

    fprintf (stderr, "%s: Error: %s\n", ProgName, SB_GetConstBuf (&S));

    SB_Done (&S);

    exit (EXIT_FAILURE);
}



void Internal (const char* Format, ...)
/* Print an internal error message and die */
{
    StrBuf S = STATIC_STRBUF_INITIALIZER;
    va_list ap;

    va_start (ap, Format);
    SB_VPrintf (&S, Format, ap);
    va_end (ap);
    SB_Terminate (&S);

    fprintf (stderr, "%s: Internal Error: %s\n", ProgName, SB_GetConstBuf (&S));

    SB_Done (&S);

    exit (EXIT_FAILURE);
}
