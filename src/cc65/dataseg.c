/*****************************************************************************/
/*                                                                           */
/*				   dataseg.c				     */
/*                                                                           */
/*			    Data segment structure			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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
#include "xmalloc.h"
#include "xsprintf.h"

/* cc65 */
#include "dataseg.h"



/*****************************************************************************/
/*     	       	       	  	     Code				     */
/*****************************************************************************/



DataSeg* NewDataSeg (void)
/* Create a new data segment, initialize and return it */
{
    /* Allocate memory */
    DataSeg* S = xmalloc (sizeof (DataSeg));

    /* Initialize the fields */
    InitCollection (&S->Lines);

    /* Return the new struct */
    return S;
}



void FreeDataSeg (DataSeg* S)
/* Free a data segment including all line entries */
{
    unsigned I, Count;

    /* Free the lines */
    Count = CollCount (&S->Lines);
    for (I = 0; I < Count; ++I) {
	xfree (CollAt (&S->Lines, I));
    }

    /* Free the collection */
    DoneCollection (&S->Lines);

    /* Free the struct */
    xfree (S);
}



void AppendDataSeg (DataSeg* Target, const DataSeg* Source)
/* Append the data from Source to Target */
{
    unsigned I;

    /* Append all lines from Source to Target */
    unsigned Count = CollCount (&Source->Lines);
    for (I = 0; I < Count; ++I) {
	CollAppend (&Target->Lines, xstrdup (CollConstAt (&Source->Lines, I)));
    }
}



void AddDataSegLine (DataSeg* S, const char* Format, ...)
/* Add a line to the given data segment */
{
    va_list ap;
    char Buf [256];

    /* Format the line */
    va_start (ap, Format);
    xvsprintf (Buf, sizeof (Buf), Format, ap);
    va_end (ap);

    /* Add a copy to the data segment */
    CollAppend (&S->Lines, xstrdup (Buf));
}



void OutputDataSeg (FILE* F, const DataSeg* S)
/* Output the data segment data to a file */
{
    unsigned I;

    /* Get the number of entries in this segment */
    unsigned Count = CollCount (&S->Lines);

    /* Output all entries */
    for (I = 0; I < Count; ++I) {
	fprintf (F, "%s\n", (const char*) CollConstAt (&S->Lines, I));
    }
}



