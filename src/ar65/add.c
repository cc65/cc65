/*****************************************************************************/
/*                                                                           */
/*                                   add.c                                   */
/*                                                                           */
/*                 Object file adding for the ar65 archiver                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998     Ullrich von Bassewitz                                        */
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



#include <stdlib.h>

#include "error.h"
#include "objfile.h"
#include "library.h"
#include "add.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void AddObjFiles (int argc, char* argv [])
/* Add object files to a library */
{
    int I;

    /* Check the argument count */
    if (argc <= 0) {
        Error ("No library name given");
    }
    if (argc <= 1) {
        Error ("No object files to add");
    }

    /* Open the library, read the index */
    LibOpen (argv [0], 0, 1);

    /* Add the object files */
    I = 1;
    while (I < argc) {
        ObjAdd (argv [I]);
        ++I;
    }

    /* Create a new library file and close the old one */
    LibClose ();

    /* Successful end */
    exit (EXIT_SUCCESS);
}
