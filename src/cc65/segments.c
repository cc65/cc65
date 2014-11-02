/*****************************************************************************/
/*                                                                           */
/*                                segments.c                                 */
/*                                                                           */
/*                   Lightweight segment management stuff                    */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2009, Ullrich von Bassewitz                                      */
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



#include <stdarg.h>
#include <string.h>

/* common */
#include "chartype.h"
#include "check.h"
#include "coll.h"
#include "scanner.h"
#include "segnames.h"
#include "strstack.h"
#include "xmalloc.h"

/* cc65 */
#include "codeent.h"
#include "codeseg.h"
#include "dataseg.h"
#include "error.h"
#include "textseg.h"
#include "segments.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Pointer to the current segment list. Output goes here. */
Segments* CS = 0;

/* Pointer to the global segment list */
Segments* GS = 0;

/* Actual names for the segments */
static StrStack SegmentNames[SEG_COUNT];

/* We're using a collection for the stack instead of a linked list. Since
** functions may not be nested (at least in the current implementation), the
** maximum stack depth is 2, so there is not really a need for a better
** implementation.
*/
static Collection SegmentStack = STATIC_COLLECTION_INITIALIZER;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void InitSegNames (void)
/* Initialize the segment names */
{
    SS_Push (&SegmentNames[SEG_BSS], SEGNAME_BSS);
    SS_Push (&SegmentNames[SEG_CODE], SEGNAME_CODE);
    SS_Push (&SegmentNames[SEG_DATA], SEGNAME_DATA);
    SS_Push (&SegmentNames[SEG_RODATA], SEGNAME_RODATA);
}



void SetSegName (segment_t Seg, const char* Name)
/* Set a new name for a segment */
{
    SS_Set (&SegmentNames[Seg], Name);
}



void PushSegName (segment_t Seg, const char* Name)
/* Push the current segment name and set a new name for a segment */
{
    if (SS_IsFull (&SegmentNames[Seg])) {
        Error ("Segment name stack overflow");
    } else {
        SS_Push (&SegmentNames[Seg], Name);
    }
}



void PopSegName (segment_t Seg)
/* Restore a segment name from the segment name stack */
{
    if (SS_GetCount (&SegmentNames[Seg]) < 2) {
        Error ("Segment name stack is empty");
    } else {
        SS_Drop (&SegmentNames[Seg]);
    }
}



const char* GetSegName (segment_t Seg)
/* Get the name of the given segment */
{
    return SS_Get (&SegmentNames[Seg]);
}



static Segments* NewSegments (SymEntry* Func)
/* Initialize a Segments structure (set all fields to NULL) */
{
    /* Allocate memory */
    Segments* S = xmalloc (sizeof (Segments));

    /* Initialize the fields */
    S->Text     = NewTextSeg (Func);
    S->Code     = NewCodeSeg (GetSegName (SEG_CODE), Func);
    S->Data     = NewDataSeg (GetSegName (SEG_DATA), Func);
    S->ROData   = NewDataSeg (GetSegName (SEG_RODATA), Func);
    S->BSS      = NewDataSeg (GetSegName (SEG_BSS), Func);
    S->CurDSeg  = SEG_DATA;

    /* Return the new struct */
    return S;
}



Segments* PushSegments (SymEntry* Func)
/* Make the new segment list current but remember the old one */
{
    /* Push the current pointer onto the stack */
    CollAppend (&SegmentStack, CS);

    /* Create a new Segments structure */
    CS = NewSegments (Func);

    /* Return the new struct */
    return CS;
}



void PopSegments (void)
/* Pop the old segment list (make it current) */
{
    /* Must have something on the stack */
    PRECONDITION (CollCount (&SegmentStack) > 0);

    /* Pop the last segment and set it as current */
    CS = CollPop (&SegmentStack);
}



void CreateGlobalSegments (void)
/* Create the global segments and remember them in GS */
{
    GS = PushSegments (0);
}



void UseDataSeg (segment_t DSeg)
/* For the current segment list, use the data segment DSeg */
{
    /* Check the input */
    PRECONDITION (CS && DSeg != SEG_CODE);

    /* Set the new segment to use */
    CS->CurDSeg = DSeg;
}



struct DataSeg* GetDataSeg (void)
/* Return the current data segment */
{
    PRECONDITION (CS != 0);
    switch (CS->CurDSeg) {
        case SEG_BSS:     return CS->BSS;
        case SEG_DATA:    return CS->Data;
        case SEG_RODATA:  return CS->ROData;
        default:
            FAIL ("Invalid data segment");
            return 0;
    }
}



void AddTextLine (const char* Format, ...)
/* Add a line of code to the current text segment */
{
    va_list ap;
    va_start (ap, Format);
    CHECK (CS != 0);
    TS_AddVLine (CS->Text, Format, ap);
    va_end (ap);
}



void AddCodeLine (const char* Format, ...)
/* Add a line of code to the current code segment */
{
    va_list ap;
    va_start (ap, Format);
    CHECK (CS != 0);
    CS_AddVLine (CS->Code, CurTok.LI, Format, ap);
    va_end (ap);
}



void AddCode (opc_t OPC, am_t AM, const char* Arg, struct CodeLabel* JumpTo)
/* Add a code entry to the current code segment */
{
    CHECK (CS != 0);
    CS_AddEntry (CS->Code, NewCodeEntry (OPC, AM, Arg, JumpTo, CurTok.LI));
}



void AddDataLine (const char* Format, ...)
/* Add a line of data to the current data segment */
{
    va_list ap;
    va_start (ap, Format);
    CHECK (CS != 0);
    DS_AddVLine (GetDataSeg(), Format, ap);
    va_end (ap);
}



int HaveGlobalCode (void)
/* Return true if the global code segment contains entries (which is an error) */
{
    return (CS_GetEntryCount (GS->Code) > 0);
}



void RemoveGlobalCode (void)
/* Remove all code from the global code segment. Used for error recovery. */
{
    CS_DelEntries (GS->Code, 0, CS_GetEntryCount (GS->Code));
}



void OutputSegments (const Segments* S)
/* Output the given segments to the output file */
{
    /* Output the function prologue if the segments came from a function */
    CS_OutputPrologue (S->Code);

    /* Output the text segment */
    TS_Output (S->Text);

    /* Output the three data segments */
    DS_Output (S->Data);
    DS_Output (S->ROData);
    DS_Output (S->BSS);

    /* Output the code segment */
    CS_Output (S->Code);

    /* Output the code segment epiloque */
    CS_OutputEpilogue (S->Code);
}
