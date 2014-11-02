/*****************************************************************************/
/*                                                                           */
/*                                  main.c                                   */
/*                                                                           */
/*                    Main program for the ar65 archiver                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2013, Ullrich von Bassewitz                                      */
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
#include <string.h>
#include <time.h>

/* common */
#include "cmdline.h"
#include "print.h"
#include "version.h"

#include "global.h"
#include "add.h"
#include "del.h"
#include "list.h"
#include "extract.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void Usage (void)
/* Print usage information and exit */
{
    fprintf (stderr, "Usage: %s <operation ...> lib file|module ...\n"
            "Operations are some of:\n"
            "\ta\tAdd modules\n"
            "\td\tDelete modules\n"
            "\tl\tList library contents\n"
            "\tv\tIncrease verbosity (put before other operation)\n"
            "\tx\tExtract modules\n"
            "\tV\tPrint the archiver version\n",
            ProgName);
    exit (EXIT_FAILURE);
}



int main (int argc, char* argv [])
/* Assembler main program */
{
    unsigned I;

    /* Initialize the cmdline module */
    InitCmdLine (&argc, &argv, "ar65");

    /* We must have a file name */
    if (ArgCount < 2) {
        Usage ();
    }

    /* Check the parameters */
    I = 1;
    while (I < ArgCount) {

        /* Get the argument */
        const char* Arg = ArgVec [I];

        /* Check for an option */
        if (strlen (Arg) != 1) {
            Usage ();
        }
        switch (Arg [0]) {

            case 'a':
                AddObjFiles (ArgCount - I - 1, &ArgVec[I+1]);
                break;

            case 'd':
                DelObjFiles (ArgCount - I - 1, &ArgVec [I+1]);
                break;

            case 'l':
                ListObjFiles (ArgCount - I - 1, &ArgVec [I+1]);
                break;

            case 'v':
                ++Verbosity;
                break;

            case 'x':
                ExtractObjFiles (ArgCount - I - 1, &ArgVec [I+1]);
                break;

            case 'V':
                fprintf (stderr, "ar65 V%s\n", GetVersionAsString ());
                break;

            default:
                fprintf (stderr, "Unknown option: %s\n", Arg);
                Usage ();

        }

        /* Next argument */
        ++I;
    }

    /* Return an apropriate exit code */
    return EXIT_SUCCESS;
}
