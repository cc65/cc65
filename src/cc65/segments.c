/*****************************************************************************/
/*                                                                           */
/*				  segments.c				     */
/*                                                                           */
/*		     Lightweight segment management stuff		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@cc65.org                                                 */
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
#include "xmalloc.h"

/* cc65 */
#include "codeseg.h"
#include "dataseg.h"
#include "textseg.h"
#include "segments.h"



/*****************************************************************************/
/*  		    		     Data				     */
/*****************************************************************************/



/* Pointer to the current segment list. Output goes here. */
Segments* CS = 0;

/* Actual names for the segments */
static char* SegmentNames[SEG_COUNT];

/* We're using a collection for the stack instead of a linked list. Since
 * functions may not be nested (at least in the current implementation), the
 * maximum stack depth is 2, so there is not really a need for a better
 * implementation.
 */
static Collection SegmentStack = STATIC_COLLECTION_INITIALIZER;



/*****************************************************************************/
/*  		    		     Code				     */
/*****************************************************************************/



void InitSegNames (void)
/* Initialize the segment names */
{
    SegmentNames [SEG_BSS]	= xstrdup ("BSS");
    SegmentNames [SEG_CODE] 	= xstrdup ("CODE");
    SegmentNames [SEG_DATA]	= xstrdup ("DATA");
    SegmentNames [SEG_RODATA]	= xstrdup ("RODATA");
}



void NewSegName (segment_t Seg, const char* Name)
/* Set a new name for a segment */
{
    /* Free the old name and set a new one */
    xfree (SegmentNames [Seg]);
    SegmentNames [Seg] = xstrdup (Name);
}



int ValidSegName (const char* Name)
/* Return true if the given segment name is valid, return false otherwise */
{
    /* Must start with '_' or a letter */
    if ((*Name != '_' && !IsAlpha(*Name)) || strlen(Name) > 80) {
       	return 0;
    }

    /* Can have letters, digits or the underline */
    while (*++Name) {
       	if (*Name != '_' && !IsAlNum(*Name)) {
       	    return 0;
       	}
    }

    /* Name is ok */
    return 1;
}



static Segments* NewSegments (SymEntry* Func)
/* Initialize a Segments structure (set all fields to NULL) */
{
    /* Allocate memory */
    Segments* S = xmalloc (sizeof (Segments));

    /* Initialize the fields */
    S->Text	= NewTextSeg (Func);
    S->Code	= NewCodeSeg (SegmentNames[SEG_CODE], Func);
    S->Data	= NewDataSeg (SegmentNames[SEG_DATA], Func);
    S->ROData	= NewDataSeg (SegmentNames[SEG_RODATA], Func);
    S->BSS	= NewDataSeg (SegmentNames[SEG_BSS], Func);
    S->CurDSeg 	= SEG_DATA;

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
	case SEG_BSS:	  return CS->BSS;
	case SEG_DATA:	  return CS->Data;
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
    AddTextEntry (CS->Text, Format, ap);
    va_end (ap);
}



void AddCodeLine (const char* Format, ...)
/* Add a line of code to the current code segment */
{
    va_list ap;
    va_start (ap, Format);
    CHECK (CS != 0);
    CS_AddEntryLine (CS->Code, CurTok.LI, Format, ap);
    va_end (ap);
}



void AddCode (struct CodeEntry* E)
/* Add a code entry to the current code segment */
{
    CHECK (CS != 0);
    CS_AddEntry (CS->Code, E, CurTok.LI);
}



void AddDataLine (const char* Format, ...)
/* Add a line of data to the current data segment */
{
    va_list ap;
    va_start (ap, Format);
    CHECK (CS != 0);
    AddDataEntry (GetDataSeg(), Format, ap);
    va_end (ap);
}



static void PrintFunctionHeader (const SymEntry* Entry, FILE* F)
{
    /* Print a comment with the function signature */
    fprintf (F,
    	     "; ---------------------------------------------------------------\n"
    	     "; ");
    PrintFuncSig (F, Entry->Name, Entry->Type);
    fprintf (F,
       	     "\n"
	     "; ---------------------------------------------------------------\n"
 	     "\n");
}



void OutputSegments (const Segments* S, FILE* F)
/* Output the given segments to the file */
{
    /* If the code segment is associated with a function, print a function header */
    if (S->Code->Func) {
       	PrintFunctionHeader (S->Code->Func, F);
    }

    /* Output the text segment */
    OutputTextSeg (S->Text, F);

    /* Output the three data segments */
    OutputDataSeg (S->Data, F);
    OutputDataSeg (S->ROData, F);
    OutputDataSeg (S->BSS, F);

    /* Output the code segment */
    CS_Output (S->Code, F);
}



