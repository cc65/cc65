/*****************************************************************************/
/*                                                                           */
/*                                  abend.c                                  */
/*                                                                           */
/*                           Abnormal program end                            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000     Ullrich von Bassewitz                                        */
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



#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "cmdline.h"
#include "abend.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void AbEnd (const char* Format, ...)
/* Print a message preceeded by the program name and terminate the program
** with an error exit code.
*/
{
    va_list ap;

    /* Print the program name */
    fprintf (stderr, "%s: ", ProgName);

    /* Format the given message and print it */
    va_start (ap, Format);
    vfprintf (stderr, Format, ap);
    va_end (ap);

    /* Add a newline */
    fprintf (stderr, "\n");

    /* Terminate the program */
    exit (EXIT_FAILURE);
}
