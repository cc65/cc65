/*****************************************************************************/
/*                                                                           */
/*				   macpack.c				     */
/*                                                                           */
/*	     Predefined macro packages for the ca65 macroassembler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2008, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                            */
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



/* common */
#include "check.h"
#include "strbuf.h"
#include "strutil.h"

/* ca65 */
#include "error.h"
#include "scanner.h"
#include "macpack.h"



/*****************************************************************************/
/*     	       	    	       	     Data				     */
/*****************************************************************************/



/* Predefined macro packages converted into C strings by a perl script */
#include "atari.inc"
#include "cbm.inc"
#include "cpu.inc"
#include "generic.inc"
#include "longbranch.inc"

/* Table with pointers to the different packages */
static struct {
    const char* Name;
    char*       Package;
} MacPackages[MAC_COUNT] = {
    /* Packages sorted by id */
    { "atari",          MacAtari        },
    { "cbm",            MacCBM          },
    { "cpu",            MacCPU          },
    { "generic",        MacGeneric      },
    { "longbranch",     MacLongBranch   },
};

/* Directory that contains standard macro package files */
static StrBuf MacPackDir = STATIC_STRBUF_INITIALIZER;



/*****************************************************************************/
/*     	       	       	       	     Code    				     */
/*****************************************************************************/



int MacPackFind (const StrBuf* Name)
/* Find a macro package by name. The function will either return the id or
 * -1 if the package name was not found.
 */
{
    int I;

    for (I = 0; I < MAC_COUNT; ++I) {
        if (SB_CompareStr (Name, MacPackages[I].Name) == 0) {
            /* Found */
            return I;
        }
    }

    /* Not found */
    return -1;
}



void MacPackInsert (int Id)
/* Insert the macro package with the given id in the input stream */
{
    /* Check the parameter */
    CHECK (Id >= 0 && Id < MAC_COUNT);

    /* If we have a macro package directory given, load a file from the
     * directory, otherwise use the builtin stuff.
     */
    if (SB_IsEmpty (&MacPackDir)) {

        /* Insert the builtin package */
        NewInputData (MacPackages[Id].Package, 0);

    } else {

        StrBuf Filename = AUTO_STRBUF_INITIALIZER;

        /* Build the complete file name */
        SB_Copy (&Filename, &MacPackDir);
        SB_AppendStr (&Filename, MacPackages[Id].Name);
        SB_AppendStr (&Filename, ".mac");
        SB_Terminate (&Filename);

        /* Open the macro package as include file */
        NewInputFile (SB_GetConstBuf (&Filename));

        /* Destroy the contents of Filename */
        SB_Done (&Filename);

    }
}



void MacPackSetDir (const StrBuf* Dir)
/* Set a directory where files for macro packages can be found. Standard is
 * to use the builtin packages. For debugging macro packages, external files
 * can be used.
 */
{
    /* Copy the directory name to the buffer */
    SB_Copy (&MacPackDir, Dir);

    /* Make sure that the last character is a path delimiter */
    if (SB_NotEmpty (&MacPackDir)) {
        char C = SB_LookAtLast (&MacPackDir);
        if (C != '\\' && C != '/') {
            SB_AppendChar (&MacPackDir, '/');
        }
    }

    /* Terminate the buffer so it's usable as a C string */
    SB_Terminate (&MacPackDir);
}



