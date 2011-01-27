/*****************************************************************************/
/*                                                                           */
/*				  fileinfo.c                                 */
/*                                                                           */
/*			  sOURCE FILE INFO STRUCTURE                         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2011, Ullrich von Bassewitz                                      */
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



/* common */
#include "xmalloc.h"

/* ld65 */
#include "fileio.h"
#include "fileinfo.h"



/*****************************************************************************/
/*     	       	      	       	     Code			     	     */
/*****************************************************************************/



static FileInfo* NewFileInfo (void)
/* Allocate and initialize a new FileInfo struct and return it */
{
    /* We will assign file info ids in increasing order of creation */
    static unsigned Id = 0;

    /* Allocate memory */
    FileInfo* FI = xmalloc (sizeof (FileInfo));

    /* Initialize stuff */
    FI->Id = Id++;

    /* Return the new struct */
    return FI;
}



FileInfo* ReadFileInfo (FILE* F, ObjData* O)
/* Read a file info from a file and return it */
{
    /* Allocate a new FileInfo structure */
    FileInfo* FI = NewFileInfo ();

    /* Read the fields from the file */
    FI->Name  = MakeGlobalStringId (O, ReadVar (F));
    FI->MTime = Read32 (F);
    FI->Size  = ReadVar (F);

    /* Return the new struct */
    return FI;
}



