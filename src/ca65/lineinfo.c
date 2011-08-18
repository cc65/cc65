/*****************************************************************************/
/*                                                                           */
/*				  lineinfo.c                                 */
/*                                                                           */
/*			Source file line info structure                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2011, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                70794 Filderstadt                                          */
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



#include <string.h>

/* common */
#include "coll.h"
#include "hashfunc.h"
#include "xmalloc.h"

/* ca65 */
#include "global.h"
#include "lineinfo.h"
#include "objfile.h"
#include "scanner.h"
#include "span.h"



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



static unsigned HT_GenHash (const void* Key);
/* Generate the hash over a key. */

static const void* HT_GetKey (const void* Entry);
/* Given a pointer to the user entry data, return a pointer to the key */

static int HT_Compare (const void* Key1, const void* Key2);
/* Compare two keys. The function must return a value less than zero if
 * Key1 is smaller than Key2, zero if both are equal, and a value greater
 * than zero if Key1 is greater then Key2.
 */



/*****************************************************************************/
/*  		       		     Data                                    */
/*****************************************************************************/



/* Structure that holds the key for a line info */
typedef struct LineInfoKey LineInfoKey;
struct LineInfoKey {
    FilePos         Pos;                /* File position */
    unsigned        Type;               /* Type/count of line info */
};

/* Structure that holds line info */
struct LineInfo {
    HashNode        Node;               /* Hash table node */
    unsigned        Id;                 /* Index */
    LineInfoKey     Key;                /* Key for this line info */
    unsigned char   Hashed;             /* True if in hash list */
    unsigned char   Referenced;         /* Force reference even if no spans */
    Collection      Spans;              /* Segment spans for this line info */
    Collection      OpenSpans;          /* List of currently open spans */
};



/* Collection containing all line infos */
static Collection LineInfoList = STATIC_COLLECTION_INITIALIZER;

/* Collection with currently active line infos */
static Collection CurLineInfo = STATIC_COLLECTION_INITIALIZER;

/* Hash table functions */
static const HashFunctions HashFunc = {
    HT_GenHash,
    HT_GetKey,
    HT_Compare
};

/* Line info hash table */
static HashTable LineInfoTab = STATIC_HASHTABLE_INITIALIZER (1051, &HashFunc);

/* The current assembler input line */
static LineInfo* AsmLineInfo = 0;



/*****************************************************************************/
/*                           Hash table functions                            */
/*****************************************************************************/



static unsigned HT_GenHash (const void* Key)
/* Generate the hash over a key. */
{
    /* Key is a LineInfoKey pointer */
    const LineInfoKey* K = Key;

    /* Hash over a combination of type, file and line */
    return HashInt ((K->Type << 18) ^ (K->Pos.Name << 14) ^ K->Pos.Line);
}



static const void* HT_GetKey (const void* Entry)
/* Given a pointer to the user entry data, return a pointer to the key */
{
    return &((const LineInfo*)Entry)->Key;
}



static int HT_Compare (const void* Key1, const void* Key2)
/* Compare two keys. The function must return a value less than zero if
 * Key1 is smaller than Key2, zero if both are equal, and a value greater
 * than zero if Key1 is greater then Key2.
 */
{
    /* Convert both parameters to FileInfoKey pointers */
    const LineInfoKey* K1 = Key1;
    const LineInfoKey* K2 = Key2;

    /* Compare line number, then file and type */
    int Res = (int)K2->Pos.Line - (int)K1->Pos.Line;
    if (Res == 0) {
        Res = (int)K2->Pos.Name - (int)K1->Pos.Name;
        if (Res == 0) {
            Res = (int)K2->Type - (int)K1->Type;
        }
    }

    /* Done */
    return Res;
}



/*****************************************************************************/
/*                              struct LineInfo                              */
/*****************************************************************************/



static LineInfo* NewLineInfo (const LineInfoKey* Key)
/* Create and return a new line info. Usage will be zero. */
{
    /* Allocate memory */
    LineInfo* LI = xmalloc (sizeof (LineInfo));

    /* Initialize the fields */
    InitHashNode (&LI->Node);
    LI->Id        = ~0U;
    LI->Key       = *Key;
    LI->Hashed    = 0;
    LI->Referenced= 0;
    InitCollection (&LI->Spans);
    InitCollection (&LI->OpenSpans);

    /* Return the new struct */
    return LI;
}



static void FreeLineInfo (LineInfo* LI)
/* Free a LineInfo structure */
{
    /* Free the Spans collection. It is supposed to be empty */
    CHECK (CollCount (&LI->Spans) == 0);
    DoneCollection (&LI->Spans);
    DoneCollection (&LI->OpenSpans);

    /* Free the structure itself */
    xfree (LI);
}



static void RememberLineInfo (LineInfo* LI)
/* Remember a LineInfo by adding it to the hash table and the global list.
 * This will also assign the id which is actually the list position.
 */
{
    /* Assign the id */
    LI->Id = CollCount (&LineInfoList);

    /* Remember it in the global list */
    CollAppend (&LineInfoList, LI);

    /* Add it to the hash table, so we will find it if necessary */
    HT_InsertEntry (&LineInfoTab, LI);

    /* Remember that we have it */
    LI->Hashed = 1;
}



/*****************************************************************************/
/*     	       	       	      	     Code			     	     */
/*****************************************************************************/



void InitLineInfo (void)
/* Initialize the line infos */
{
    static const FilePos DefaultPos = STATIC_FILEPOS_INITIALIZER;

    /* Increase the initial count of the line info collection */
    CollGrow (&LineInfoList, 200);

    /* Create a LineInfo for the default source. This is necessary to allow
     * error message to be generated without any input file open.
     */
    AsmLineInfo = StartLine (&DefaultPos, LI_TYPE_ASM, 0);
}



void DoneLineInfo (void)
/* Close down line infos */
{
    /* Close all current line infos */
    unsigned Count = CollCount (&CurLineInfo);
    while (Count) {
        EndLine (CollAt (&CurLineInfo, --Count));
    }
}



void EndLine (LineInfo* LI)
/* End a line that is tracked by the given LineInfo structure */
{
    unsigned I;

    /* Close the spans for the line */
    CloseSpans (&LI->OpenSpans);

    /* Move the spans to the list of all spans for this line, then clear the
     * list of open spans.
     */
    for (I = 0; I < CollCount (&LI->OpenSpans); ++I) {
        CollAppend (&LI->Spans, CollAtUnchecked (&LI->OpenSpans, I));
    }
    CollDeleteAll (&LI->OpenSpans);

    /* Line info is no longer active - remove it from the list of current
     * line infos.
     */
    CollDeleteItem (&CurLineInfo, LI);

    /* If this line info is already hashed, we're done. Otherwise, if it is
     * marked as referenced or has non empty spans, remember it. It it is not
     * referenced or doesn't have open spans, delete it.
     */
    if (!LI->Hashed) {
        if (LI->Referenced || CollCount (&LI->Spans) > 0) {
            RememberLineInfo (LI);
        } else {
            FreeLineInfo (LI);
        }
    }
}



LineInfo* StartLine (const FilePos* Pos, unsigned Type, unsigned Count)
/* Start line info for a new line */
{
    LineInfoKey Key;
    LineInfo* LI;

    /* Prepare the key struct */
    Key.Pos   = *Pos;
    Key.Type  = LI_MAKE_TYPE (Type, Count);

    /* Try to find a line info with this position and type in the hash table.
     * If so, reuse it. Otherwise create a new one.
     */
    LI = HT_FindEntry (&LineInfoTab, &Key);
    if (LI == 0) {
        /* Allocate a new LineInfo */
        LI = NewLineInfo (&Key);
    }

    /* Open the spans for this line info */
    OpenSpans (&LI->OpenSpans);

    /* Add the line info to the list of current line infos */
    CollAppend (&CurLineInfo, LI);

    /* Return the new info */
    return LI;
}



void NewAsmLine (void)
/* Start a new assembler input line. Use this function when generating new
 * line of LI_TYPE_ASM. It will check if line and/or file have actually
 * changed, end the old and start the new line as necessary.
 */
{
    /* Check if we can reuse the old line */
    if (AsmLineInfo) {
        if (AsmLineInfo->Key.Pos.Line == CurTok.Pos.Line &&
            AsmLineInfo->Key.Pos.Name == CurTok.Pos.Name) {
            /* We do already have line info for this line */
            return;
        }

        /* Line has changed -> end the old line */
        EndLine (AsmLineInfo);
    }

    /* Start a new line using the current line info */
    AsmLineInfo = StartLine (&CurTok.Pos, LI_TYPE_ASM, 0);
}



void GetFullLineInfo (Collection* LineInfos, int ForceRef)
/* Return full line infos, that is line infos for currently active Slots. The
 * function will clear LineInfos before usage. If ForceRef is not zero, a
 * forced reference will be added to all line infos, with the consequence that
 * they won't get deleted, even if there is no code or data generated for these
 * lines.
 */
{
    unsigned I;

    /* Clear the collection */
    CollDeleteAll (LineInfos);

    /* Grow the collection as necessary */
    CollGrow (LineInfos, CollCount (&CurLineInfo));

    /* Copy all valid line infos to the collection */
    for (I = 0; I < CollCount (&CurLineInfo); ++I) {

        /* Get the line info from the slot */
        LineInfo* LI = CollAt (&CurLineInfo, I);

        /* Mark it as referenced */
        if (ForceRef) {
            LI->Referenced = 1;
        }

        /* Return it to the caller */
        CollAppend (LineInfos, LI);
    }
}



const FilePos* GetSourcePos (const LineInfo* LI)
/* Return the source file position from the given line info */
{
    return &LI->Key.Pos;
}



unsigned GetLineInfoType (const LineInfo* LI)
/* Return the type of a line info */
{
    return LI_GET_TYPE (LI->Key.Type);
}



void WriteLineInfo (const Collection* LineInfos)
/* Write a list of line infos to the object file. */
{
    unsigned I;

    /* Write the count */
    ObjWriteVar (CollCount (LineInfos));

    /* Write the line info indices */
    for (I = 0; I < CollCount (LineInfos); ++I) {

        /* Get a pointer to the line info */
        const LineInfo* LI = CollConstAt (LineInfos, I);

        CHECK (LI->Id != ~0U);

        /* Write the index to the file */
        ObjWriteVar (LI->Id);
    }
}



void WriteLineInfos (void)
/* Write a list of all line infos to the object file. */
{
    unsigned I;

    Collection EmptySpans = STATIC_COLLECTION_INITIALIZER;

    /* Tell the object file module that we're about to write line infos */
    ObjStartLineInfos ();

    /* Write the line info count to the list */
    ObjWriteVar (CollCount (&LineInfoList));

    /* Walk over the list and write all line infos */
    for (I = 0; I < CollCount (&LineInfoList); ++I) {

        /* Get a pointer to this line info */
        LineInfo* LI = CollAt (&LineInfoList, I);

        /* Write the source file position */
        ObjWritePos (&LI->Key.Pos);

        /* Write the type and count of the line info */
        ObjWriteVar (LI->Key.Type);

        /* Spans are only added to the debug file if debug information is
         * requested. Otherwise we write an empty list.
         */
        if (DbgSyms) {
            WriteSpans (&LI->Spans);
        } else {
            /* Write out an empty list */
            WriteSpans (&EmptySpans);
        }
    }

    /* End of line infos */
    ObjEndLineInfos ();

    /* For the sake of completeness, but not really necessary */
    DoneCollection (&EmptySpans);
}



