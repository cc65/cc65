/*****************************************************************************/
/*                                                                           */
/*                                 error.c                                   */
/*                                                                           */
/*                    Error handling for the sim65 simulator                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2003 Ullrich von Bassewitz                                       */
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

#include "error.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* flag to print cycles at program termination */
int PrintCycles = 0;

/* cycles are counted by main.c */
extern unsigned long long TotalCycles;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void Warning (const char* Format, ...)
/* Print a warning message */
{
    va_list ap;
    va_start (ap, Format);
    fprintf (stderr, "Warning: ");
    vfprintf (stderr, Format, ap);
    putc ('\n', stderr);
    va_end (ap);
}



void Error (const char* Format, ...)
/* Print an error message and die */
{
    va_list ap;
    va_start (ap, Format);
    fprintf (stderr, "Error: ");
    vfprintf (stderr, Format, ap);
    putc ('\n', stderr);
    va_end (ap);
    exit (SIM65_ERROR);
}



void ErrorCode (int Code, const char* Format, ...)
/* Print an error message and die with the given exit code */
{
    va_list ap;
    va_start (ap, Format);
    fprintf (stderr, "Error: ");
    vfprintf (stderr, Format, ap);
    putc ('\n', stderr);
    va_end (ap);
    exit (Code);
}



void Internal (const char* Format, ...)
/* Print an internal error message and die */
{
    va_list ap;
    va_start (ap, Format);
    fprintf (stderr, "Internal error: ");
    vfprintf (stderr, Format, ap);
    putc ('\n', stderr);
    va_end (ap);
    exit (SIM65_ERROR);
}



void SimExit (int Code)
/* Exit the simulation with an exit code */
{
    if (PrintCycles) {
        fprintf (stdout, "%llu cycles\n", TotalCycles);
    }
    exit (Code);
}
