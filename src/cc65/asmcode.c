/*****************************************************************************/
/*                                                                           */
/*				   asmcode.c				     */
/*                                                                           */
/*	    Assembler output code handling for the cc65 C compiler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2001 Ullrich von Bassewitz                                       */
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



/* common */
#include "check.h"

/* cc65 */
#include "codeopt.h"
#include "codeseg.h"
#include "dataseg.h"
#include "segments.h"
#include "symtab.h"
#include "asmcode.h"



/*****************************************************************************/
/*  	       		    	     Code   				     */
/*****************************************************************************/



CodeMark GetCodePos (void)
/* Get a marker pointing to the current output position */
{
    return CS_GetEntryCount (CS->Code);
}



void RemoveCode (CodeMark M)
/* Remove all code after the given code marker */
{
    CS_DelCodeAfter (CS->Code, M);
}



void MoveCode (CodeMark Start, CodeMark End, CodeMark Target)
/* Move the code between Start (inclusive) and End (exclusive) to
 * (before) Target.
 */
{
    CS_MoveEntries (CS->Code, Start, End - Start, Target);
}



void WriteOutput (FILE* F)
/* Write the final output to a file */
{
    SymTable* SymTab;
    SymEntry* Entry;

    /* Output the global data segment */
    CHECK (!HaveGlobalCode ());
    OutputSegments (CS, F);

    /* Output all global or referenced functions */
    SymTab = GetGlobalSymTab ();
    Entry  = SymTab->SymHead;
    while (Entry) {
       	if (IsTypeFunc (Entry->Type) 	  	&&
       	    SymIsDef (Entry)   	                &&
       	    (Entry->Flags & (SC_REF | SC_EXTERN)) != 0) {
       	    /* Function which is defined and referenced or extern */
       	    CS_MergeLabels (Entry->V.F.Seg->Code);
       	    RunOpt (Entry->V.F.Seg->Code);
       	    OutputSegments (Entry->V.F.Seg, F);
       	}
       	Entry = Entry->NextSym;
    }
}



