/*****************************************************************************/
/*                                                                           */
/*                                 textseg.c                                 */
/*                                                                           */
/*                          Text segment structure                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2009, Ullrich von Bassewitz                                      */
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



/* Note: This is NOT some sort of code segment, it is used to store lines of
** output that are textual (not real code) instead.
*/



/* common */
#include "xmalloc.h"
#include "xsprintf.h"

/* cc65 */
#include "output.h"
#include "textseg.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



TextSeg* NewTextSeg (SymEntry* Func)
/* Create a new text segment, initialize and return it */
{
    /* Allocate memory for the structure */
    TextSeg* S  = xmalloc (sizeof (TextSeg));

    /* Initialize the fields */
    S->Func     = Func;
    InitCollection (&S->Lines);

    /* Return the new struct */
    return S;
}



void TS_AddVLine (TextSeg* S, const char* Format, va_list ap)
/* Add a line to the given text segment */
{
    /* Format the line */
    char Buf [256];
    xvsprintf (Buf, sizeof (Buf), Format, ap);

    /* Add a copy to the data segment */
    CollAppend (&S->Lines, xstrdup (Buf));
}



void TS_AddLine (TextSeg* S, const char* Format, ...)
/* Add a line to the given text segment */
{
    va_list ap;
    va_start (ap, Format);
    TS_AddVLine (S, Format, ap);
    va_end (ap);
}



void TS_Output (const TextSeg* S)
/* Output the text segment data to the output file */
{
    unsigned I;

    /* Get the number of entries in this segment */
    unsigned Count = CollCount (&S->Lines);

    /* If the segment is actually empty, bail out */
    if (Count == 0) {
        return;
    }

    /* Output all entries */
    for (I = 0; I < Count; ++I) {
        WriteOutput ("%s\n", (const char*) CollConstAt (&S->Lines, I));
    }

    /* Add an additional newline after the segment output */
    WriteOutput ("\n");
}
