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

/* b6502 */
#include "codeopt.h"
#include "codeseg.h"
#include "dataseg.h"

/* cc65 */
#include "symtab.h"
#include "asmcode.h"



/*****************************************************************************/
/*	       		    	     Code   				     */
/*****************************************************************************/



void AddCodeHint (const char* Hint)
/* Add an optimizer hint */
{
    /* ### AddCodeLine ("+%s", Hint); */
}



CodeMark GetCodePos (void)
/* Get a marker pointing to the current output position */
{
    return GetCodeSegEntries (CS);
}



void RemoveCode (CodeMark M)
/* Remove all code after the given code marker */
{
    DelCodeSegAfter (CS, M);
}



static void PrintFunctionHeader (FILE* F, SymEntry* Entry)
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



void WriteOutput (FILE* F)
/* Write the final output to a file */
{
    SymTable* SymTab;
    SymEntry* Entry;

    /* Output the data segment (the global code segment should be empty) */
    OutputDataSeg (F, DS);
    CHECK (GetCodeSegEntries (CS) == 0);

    /* Output all global or referenced functions */
    SymTab = GetGlobalSymTab ();
    Entry  = SymTab->SymHead;
    while (Entry) {
       	if (IsTypeFunc (Entry->Type) 	  	&&
	    (Entry->Flags & SC_DEF) != 0  	&&
	    (Entry->Flags & (SC_REF | SC_EXTERN)) != 0) {
	    /* Function which is defined and referenced or extern */
	    PrintFunctionHeader (F, Entry);
	    MergeCodeLabels (Entry->V.F.CS);
	    RunOpt (Entry->V.F.CS);
	    fprintf (F, "; Data segment for function %s:\n", Entry->Name);
	    OutputDataSeg (F, Entry->V.F.DS);
	    fprintf (F, "; Code segment for function %s:\n", Entry->Name);
	    OutputCodeSeg (F, Entry->V.F.CS);
     	}
	Entry = Entry->NextSym;
    }
}



