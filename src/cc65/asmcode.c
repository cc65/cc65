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



#include "asmline.h"
#include "check.h"
#include "global.h"
#include "asmcode.h"



/*****************************************************************************/
/*	       		   	     Code   				     */
/*****************************************************************************/



void AddCodeLine (const char* Format, ...)
/* Add a new line of code to the output */
{
    va_list ap;
    va_start (ap, Format);
    NewCodeLine (Format, ap);
    va_end (ap);
}



void AddCodeHint (const char* Hint)
/* Add an optimizer hint */
{
    AddCodeLine ("+%s", Hint);
}



void AddEmptyLine (void)
/* Add an empty line for formatting purposes */
{
    AddCodeLine ("");
}



CodeMark GetCodePos (void)
/* Get a marker pointing to the current output position */
{
    /* This function should never be called without any code output */
    CHECK (LastLine != 0);

    return LastLine;
}



void RemoveCode (CodeMark M)
/* Remove all code after the given code marker */
{
    while (LastLine != M) {
	FreeCodeLine (LastLine);
    }
}



void WriteOutput (FILE* F)
/* Write the final output to a file */
{
    Line* L = FirstLine;
    while (L) {
     	/* Don't write optimizer hints if not requested to do so */
     	if (L->Line[0] == '+') {
     	    if (Debug) {
     	        fprintf (F, ";%s\n", L->Line);
     	    }
     	} else {
     	    fprintf (F, "%s\n", L->Line);
     	}
     	L = L->Next;
    }
}



