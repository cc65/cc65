/*****************************************************************************/
/*                                                                           */
/*				   asmcode.c				     */
/*                                                                           */
/*	    Assembler output code handling for the cc65 C compiler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000     Ullrich von Bassewitz                                        */
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



/* common */
#include "check.h"

/* b6502 */
#include "codeseg.h"
#include "dataseg.h"

/* cc65 */
#include "codegen.h"
#include "global.h"
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



void WriteOutput (FILE* F)
/* Write the final output to a file */
{
    OutputDataSeg (F, DS);
    MergeCodeLabels (CS);
    OutputCodeSeg (F, CS);
}



