/*****************************************************************************/
/*                                                                           */
/*                                  list.c                                   */
/*                                                                           */
/*                   Module listing for the ar65 archiver                    */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2011, Ullrich von Bassewitz                                      */
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

/* ar65 */
#include "error.h"
#include "library.h"
#include "list.h"
#include "objdata.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void ListObjFiles (int argc, char* argv [])
/* List modules in a library */
{
    unsigned I;
    const ObjData* O;

    /* Check the argument count */
    if (argc <= 0) {
        Error ("No library name given");
    }
    if (argc > 2) {
        Error ("Too many arguments");
    }

    /* Open the library, read the index */
    LibOpen (argv [0], 1, 0);

    /* List the modules */
    for (I = 0; I < CollCount (&ObjPool); ++I) {

        /* Get the entry */
        O = CollConstAt (&ObjPool, I);

        /* Print the name */
        printf ("%s\n", O->Name);

    }

    /* Create a new library file and close the old one */
    LibClose ();

    /* Successful end */
    exit (EXIT_SUCCESS);
}
