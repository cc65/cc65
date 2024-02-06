/*****************************************************************************/
/*                                                                           */
/*                                fileinfo.c                                 */
/*                                                                           */
/*                        Source file info structure                         */
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
#include "coll.h"
#include "xmalloc.h"

/* ld65 */
#include "fileio.h"
#include "fileinfo.h"
#include "objdata.h"
#include "spool.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* A list of all file infos without duplicates */
static Collection FileInfos = STATIC_COLLECTION_INITIALIZER;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static int FindFileInfo (unsigned Name, unsigned* Index)
/* Find the FileInfo for a given file name. The function returns true if the
** name was found. In this case, Index contains the index of the first item
** that matches. If the item wasn't found, the function returns false and
** Index contains the insert position for FileName.
*/
{
    /* Do a binary search */
    int Lo = 0;
    int Hi = (int) CollCount (&FileInfos) - 1;
    int Found = 0;
    while (Lo <= Hi) {

        /* Mid of range */
        int Cur = (Lo + Hi) / 2;

        /* Get item */
        FileInfo* CurItem = CollAt (&FileInfos, Cur);

        /* Found? */
        if (CurItem->Name < Name) {
            Lo = Cur + 1;
        } else {
            Hi = Cur - 1;
            /* Since we may have duplicates, repeat the search until we've
            ** the first item that has a match.
            */
            if (CurItem->Name == Name) {
                Found = 1;
            }
        }
    }

    /* Pass back the index. This is also the insert position */
    *Index = Lo;
    return Found;
}



static FileInfo* NewFileInfo (unsigned Name, unsigned long MTime, unsigned long Size)
/* Allocate and initialize a new FileInfo struct and return it */
{
    /* Allocate memory */
    FileInfo* FI = xmalloc (sizeof (FileInfo));

    /* Initialize stuff */
    FI->Id      = ~0U;
    FI->Name    = Name;
    FI->MTime   = MTime;
    FI->Size    = Size;
    FI->Modules = EmptyCollection;

    /* Return the new struct */
    return FI;
}



static void FreeFileInfo (FileInfo* FI)
/* Free a file info structure */
{
    /* Free the collection */
    DoneCollection (&FI->Modules);

    /* Free memory for the structure */
    xfree (FI);
}



FileInfo* ReadFileInfo (FILE* F, ObjData* O)
/* Read a file info from a file and return it */
{
    FileInfo* FI;

    /* Read the fields from the file */
    unsigned      Name  = MakeGlobalStringId (O, ReadVar (F));
    unsigned long MTime = Read32 (F);
    unsigned long Size  = ReadVar (F);

    /* Search for the first entry with this name */
    unsigned Index;
    if (FindFileInfo (Name, &Index)) {

        /* We have at least one such entry. Try all of them and, if size and
        ** modification time matches, return the first match. When the loop
        ** is terminated without finding an entry, Index points one behind
        ** the last entry with the name, which is the perfect insert position.
        */
        FI = CollAt (&FileInfos, Index);
        while (1) {

            /* Check size and modification time stamp */
            if (FI->Size == Size && FI->MTime == MTime) {
                /* Remember that the modules uses this file info, then return it */
                CollAppend (&FI->Modules, O);
                return FI;
            }

            /* Check the next one */
            if (++Index >= CollCount (&FileInfos)) {
                /* Nothing left */
                break;
            }
            FI = CollAt (&FileInfos, Index);

            /* Done if the name differs */
            if (FI->Name != Name) {
                break;
            }
        }
    }

    /* Not found. Allocate a new FileInfo structure */
    FI = NewFileInfo (Name, MTime, Size);

    /* Remember that this module uses the file info */
    CollAppend (&FI->Modules, O);

    /* Insert the file info in our global list. Index points to the insert
    ** position.
    */
    CollInsert (&FileInfos, FI, Index);

    /* Return the new struct */
    return FI;
}



unsigned FileInfoCount (void)
/* Return the total number of file infos */
{
    return CollCount (&FileInfos);
}



void AssignFileInfoIds (void)
/* Remove unused file infos and assign the ids to the remaining ones */
{
    unsigned I, J;

    /* Print all file infos */
    for (I = 0, J = 0; I < CollCount (&FileInfos); ++I) {

        /* Get the next file info */
        FileInfo* FI = CollAtUnchecked (&FileInfos, I);

        /* If it's unused, free it, otherwise assign the id and keep it */
        if (CollCount (&FI->Modules) == 0) {
            FreeFileInfo (FI);
        } else {
            FI->Id = J;
            CollReplace (&FileInfos, FI, J++);
        }
    }

    /* The new count is now in J */
    FileInfos.Count = J;
}



void PrintDbgFileInfo (FILE* F)
/* Output the file info to a debug info file */
{
    unsigned I, J;

    /* Print all file infos */
    for (I = 0; I < CollCount (&FileInfos); ++I) {

        /* Get the file info */
        const FileInfo* FI = CollAtUnchecked (&FileInfos, I);

        /* Base info */
        fprintf (F,
                 "file\tid=%u,name=\"%s\",size=%lu,mtime=0x%08lX,mod=",
                 FI->Id, GetString (FI->Name), FI->Size, FI->MTime);

        /* Modules that use the file */
        for (J = 0; J < CollCount (&FI->Modules); ++J) {

            /* Get the module */
            const ObjData* O = CollConstAt (&FI->Modules, J);

            /* Output its id */
            if (J > 0) {
                fprintf (F, "+%u", O->Id);
            } else {
                fprintf (F, "%u", O->Id);
            }
        }

        /* Terminate the output line */
        fputc ('\n', F);
    }
}
