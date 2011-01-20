/*****************************************************************************/
/*                                                                           */
/*		     		   dbginfo.c				     */
/*                                                                           */
/*  		     	   Handle the .dbg commands			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2011, Ullrich von Bassewitz                                      */
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



#include <string.h>

/* common */
#include "strbuf.h"

/* ca65 */
#include "error.h"
#include "expr.h"
#include "filetab.h"
#include "lineinfo.h"
#include "nexttok.h"
#include "dbginfo.h"



/*****************************************************************************/
/*     	       	    		     Code			   	     */
/*****************************************************************************/



void DbgInfoFile (void)
/* Parse and handle FILE subcommand of the .dbg pseudo instruction */
{
    StrBuf Name = STATIC_STRBUF_INITIALIZER;
    unsigned long Size;
    unsigned long MTime;

    /* Parameters are separated by a comma */
    ConsumeComma ();

    /* Name */
    if (CurTok.Tok != TOK_STRCON) {
       	ErrorSkip ("String constant expected");
       	return;
    }
    SB_Copy (&Name, &CurTok.SVal);
    NextTok ();

    /* Comma expected */
    ConsumeComma ();

    /* Size */
    Size = ConstExpression ();

    /* Comma expected */
    ConsumeComma ();

    /* MTime */
    MTime = ConstExpression ();

    /* Insert the file into the table */
    AddFile (&Name, FT_DBGINFO, Size, MTime);

    /* Free memory used for Name */
    SB_Done (&Name);
}



void DbgInfoLine (void)
/* Parse and handle LINE subcommand of the .dbg pseudo instruction */
{
    unsigned Index;
    long LineNum;

    /* If a parameters follow, this is actual line info. If no parameters
     * follow, the last line info is terminated.
     */
    if (CurTok.Tok == TOK_SEP) {
	ClearLineInfo ();
	return;
    }

    /* Parameters are separated by a comma */
    ConsumeComma ();

    /* The name of the file follows */
    if (CurTok.Tok != TOK_STRCON) {
     	ErrorSkip ("String constant expected");
     	return;
    }

    /* Get the index in the file table for the name */
    Index = GetFileIndex (&CurTok.SVal);

    /* Skip the name */
    NextTok ();

    /* Comma expected */
    ConsumeComma ();

    /* Line number */
    LineNum = ConstExpression ();
    if (LineNum < 0) {
	ErrorSkip ("Line number is out of valid range");
	return;
    }

    /* Remember the line info */
    GenLineInfo (Index, LineNum, 0);
}



void DbgInfoSym (void)
/* Parse and handle SYM subcommand of the .dbg pseudo instruction */
{
    ErrorSkip ("Not implemented");
}





