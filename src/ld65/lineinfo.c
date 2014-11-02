/*****************************************************************************/
/*                                                                           */
/*                                lineinfo.h                                 */
/*                                                                           */
/*                      Source file line info structure                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2012, Ullrich von Bassewitz                                      */
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
#include "check.h"
#include "lidefs.h"
#include "xmalloc.h"

/* ld65 */
#include "error.h"
#include "fileinfo.h"
#include "fileio.h"
#include "lineinfo.h"
#include "objdata.h"
#include "segments.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static LineInfo* NewLineInfo (void)
/* Create and return a new LineInfo struct with mostly empty fields */
{
    /* Allocate memory */
    LineInfo* LI = xmalloc (sizeof (LineInfo));

    /* Initialize the fields */
    LI->Id         = ~0U;
    LI->File       = 0;
    LI->Type       = LI_MAKE_TYPE (LI_TYPE_ASM, 0);
    LI->Pos.Name   = INVALID_STRING_ID;
    LI->Pos.Line   = 0;
    LI->Pos.Col    = 0;
    LI->Spans      = 0;

    /* Return the new struct */
    return LI;
}



void FreeLineInfo (LineInfo* LI)
/* Free a LineInfo structure. */
{
    /* Free the span list */
    xfree (LI->Spans);

    /* Free the structure itself */
    xfree (LI);
}



LineInfo* DupLineInfo (const LineInfo* LI)
/* Creates a duplicate of a line info structure */
{
    /* Allocate memory */
    LineInfo* New = xmalloc (sizeof (LineInfo));

    /* Copy the fields (leave id invalid) */
    New->Id     = LI->Id;
    New->File   = LI->File;
    New->Type   = LI->Type;
    New->Pos    = LI->Pos;
    New->Spans  = DupSpanList (LI->Spans);

    /* Return the copy */
    return New;
}



LineInfo* GenLineInfo (const FilePos* Pos)
/* Generate a new (internally used) line info with the given information */
{
    /* Create a new LineInfo struct */
    LineInfo* LI = NewLineInfo ();

    /* Initialize the fields in the new LineInfo */
    LI->Pos = *Pos;

    /* Return the struct read */
    return LI;
}



LineInfo* ReadLineInfo (FILE* F, ObjData* O)
/* Read a line info from a file and return it */
{
    /* Create a new LineInfo struct */
    LineInfo* LI = NewLineInfo ();

    /* Read/fill the fields in the new LineInfo */
    LI->Pos.Line = ReadVar (F);
    LI->Pos.Col  = ReadVar (F);
    LI->File     = CollAt (&O->Files, ReadVar (F));
    LI->Pos.Name = LI->File->Name;
    LI->Type     = ReadVar (F);
    LI->Spans    = ReadSpanList (F);

    /* Return the struct read */
    return LI;
}



void ReadLineInfoList (FILE* F, ObjData* O, Collection* LineInfos)
/* Read a list of line infos stored as a list of indices in the object file,
** make real line infos from them and place them into the passed collection.
*/
{
    /* Read the number of line info indices that follow */
    unsigned LineInfoCount = ReadVar (F);

    /* Grow the collection as needed */
    CollGrow (LineInfos, LineInfoCount);

    /* Read the line infos and resolve them */
    while (LineInfoCount--) {

        /* Read an index */
        unsigned LineInfoIndex = ReadVar (F);

        /* The line info index was written by the assembler and must
        ** therefore be part of the line infos read from the object file.
        */
        if (LineInfoIndex >= CollCount (&O->LineInfos)) {
            Internal ("Invalid line info index %u in module `%s' - max is %u",
                      LineInfoIndex,
                      GetObjFileName (O),
                      CollCount (&O->LineInfos));
        }

        /* Add the line info to the collection */
        CollAppend (LineInfos, CollAt (&O->LineInfos, LineInfoIndex));
    }
}



const LineInfo* GetAsmLineInfo (const Collection* LineInfos)
/* Find a line info of type LI_TYPE_ASM and count zero in the given collection
** and return it. Return NULL if no such line info was found.
*/
{
    unsigned I;

    /* Search for a line info of LI_TYPE_ASM */
    for (I = 0; I < CollCount (LineInfos); ++I) {
        const LineInfo* LI = CollConstAt (LineInfos, I);
        if (LI->Type == LI_MAKE_TYPE (LI_TYPE_ASM, 0)) {
            return LI;
        }
    }

    /* Not found */
    return 0;
}



unsigned LineInfoCount (void)
/* Return the total number of line infos */
{
    /* Walk over all object files */
    unsigned I;
    unsigned Count = 0;
    for (I = 0; I < CollCount (&ObjDataList); ++I) {

        /* Get this object file */
        const ObjData* O = CollAtUnchecked (&ObjDataList, I);

        /* Count spans */
        Count += CollCount (&O->LineInfos);
    }

    return Count;
}



void AssignLineInfoIds (void)
/* Assign the ids to the line infos */
{
    unsigned I, J;

    /* Walk over all line infos */
    unsigned Id = 0;
    for (I = 0; I < CollCount (&ObjDataList); ++I) {

        /* Get the object file */
        ObjData* O = CollAtUnchecked (&ObjDataList, I);

        /* Output the line infos */
        for (J = 0; J < CollCount (&O->LineInfos); ++J) {

            /* Get this line info */
            LineInfo* LI = CollAtUnchecked (&O->LineInfos, J);

            /* Assign the id */
            LI->Id = Id++;
        }
    }
}



void PrintDbgLineInfo (FILE* F)
/* Output the line infos to a debug info file */
{
    unsigned I, J;

    /* Print line infos from all modules we have linked into the output file */
    for (I = 0; I < CollCount (&ObjDataList); ++I) {

        /* Get the object file */
        const ObjData* O = CollAtUnchecked (&ObjDataList, I);

        /* Output the line infos */
        for (J = 0; J < CollCount (&O->LineInfos); ++J) {

            /* Get this line info */
            const LineInfo* LI = CollConstAt (&O->LineInfos, J);

            /* Get the line info type and count */
            unsigned Type  = LI_GET_TYPE (LI->Type);
            unsigned Count = LI_GET_COUNT (LI->Type);

            /* Print the start of the line */
            fprintf (F,
                     "line\tid=%u,file=%u,line=%u",
                     LI->Id, LI->File->Id, GetSourceLine (LI));

            /* Print type if not LI_TYPE_ASM and count if not zero */
            if (Type != LI_TYPE_ASM) {
                fprintf (F, ",type=%u", Type);
            }
            if (Count != 0) {
                fprintf (F, ",count=%u", Count);
            }

            /* Add spans if the line info has it */
            PrintDbgSpanList (F, O, LI->Spans);

            /* Terminate line */
            fputc ('\n', F);
        }
    }
}
