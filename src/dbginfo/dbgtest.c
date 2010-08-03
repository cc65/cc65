/*****************************************************************************/
/*                                                                           */
/*                                 dbgtest.c                                 */
/*                                                                           */
/*                   Test file for the cc65 dbginfo module                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2010,      Ullrich von Bassewitz                                      */
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



#include <stdio.h>
#include <stdlib.h>
#include "dbginfo.h"



static void ErrorFunc (const struct cc65_parseerror* E)
/* Handle errors or warnings that occur while parsing a debug file */
{
    fprintf (stderr,
             "%s:%s(%lu): %s\n",
             E->type? "Error" : "Warning",
             E->name,
             E->line,
             E->errormsg);
}



static void Usage (void)
/* Print usage information and exit */
{
    printf ("Usage: dbgtest debug-file\n");
    exit (1);
}



int main (int argc, char** argv)
{
    const char* Input;
    cc65_dbginfo Handle;


    /* Input file is argument */
    if (argc != 2) {
        Usage ();
    }
    Input = argv[1];


    Handle = cc65_read_dbginfo (Input, ErrorFunc);
    if (Handle == 0) {
        fprintf (stderr, "No handle\n");
    }
    return 0;
}



