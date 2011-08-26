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
/*     	       	    		     Data                                    */
/*****************************************************************************/



/* The current line info */
static LineInfo* CurLineInfo = 0;



/*****************************************************************************/
/*                                   Code                                    */
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



void DbgInfoFunc (void)
/* Parse and handle func subcommand of the .dbg pseudo instruction */
{
    static const char* StorageKeys[] = {
       	"EXTERN",
     	"STATIC",
    };

    StrBuf        Name = STATIC_STRBUF_INITIALIZER;
    StrBuf        AsmName = STATIC_STRBUF_INITIALIZER;
    int           StorageClass;


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

    /* The storage class follows */
    if (CurTok.Tok != TOK_IDENT) {
       	ErrorSkip ("Storage class specifier expected");
       	return;
    }
    StorageClass = GetSubKey (StorageKeys, sizeof (StorageKeys)/sizeof (StorageKeys[0]));
    if (StorageClass < 0) {
        ErrorSkip ("Storage class specifier expected");
        return;
    }
    NextTok ();

    /* Comma expected */
    ConsumeComma ();

    /* Assembler name follows */
    if (CurTok.Tok != TOK_STRCON) {
        ErrorSkip ("String constant expected");
        return;
    }
    SB_Copy (&AsmName, &CurTok.SVal);
    NextTok ();

    /* Free memory used for the strings */
    SB_Done (&AsmName);
    SB_Done (&Name);
}



void DbgInfoLine (void)
/* Parse and handle LINE subcommand of the .dbg pseudo instruction */
{
    long Line;
    FilePos Pos = STATIC_FILEPOS_INITIALIZER;

    /* Any new line info terminates the last one */
    if (CurLineInfo) {
        EndLine (CurLineInfo);
        CurLineInfo = 0;
    }

    /* If a parameters follow, this is actual line info. If no parameters
     * follow, the last line info is terminated.
     */
    if (CurTok.Tok == TOK_SEP) {
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
    Pos.Name = GetFileIndex (&CurTok.SVal);

    /* Skip the name */
    NextTok ();

    /* Comma expected */
    ConsumeComma ();

    /* Line number */
    Line = ConstExpression ();
    if (Line < 0) {
	ErrorSkip ("Line number is out of valid range");
	return;
    }
    Pos.Line = Line;

    /* Generate a new external line info */
    CurLineInfo = StartLine (&Pos, LI_TYPE_EXT, 0);
}



void DbgInfoSym (void)
/* Parse and handle SYM subcommand of the .dbg pseudo instruction */
{
    static const char* StorageKeys[] = {
        "AUTO",
       	"EXTERN",
        "REGISTER",
     	"STATIC",
    };

    StrBuf        Name = STATIC_STRBUF_INITIALIZER;
    StrBuf        AsmName = STATIC_STRBUF_INITIALIZER;
    int           StorageClass;
    int           Offs;


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

    /* The storage class follows */
    if (CurTok.Tok != TOK_IDENT) {
       	ErrorSkip ("Storage class specifier expected");
       	return;
    }
    StorageClass = GetSubKey (StorageKeys, sizeof (StorageKeys)/sizeof (StorageKeys[0]));
    if (StorageClass < 0) {
        ErrorSkip ("Storage class specifier expected");
        return;
    }

    /* Skip the storage class token and the following comma */
    NextTok ();
    ConsumeComma ();

    /* The next tokens depend on the storage class */
    if (StorageClass == 0) {
        /* Auto: Stack offset follows */
        Offs = ConstExpression ();
    } else if (StorageClass == 2) {
        /* Register: Register bank offset follows */
        Offs = ConstExpression ();
    } else {
        /* Extern or static: Assembler name follows */
        if (CurTok.Tok != TOK_STRCON) {
            ErrorSkip ("String constant expected");
            return;
        }
        SB_Copy (&AsmName, &CurTok.SVal);
        NextTok ();
    }

    /* Free memory used for the strings */
    SB_Done (&AsmName);
    SB_Done (&Name);
}



