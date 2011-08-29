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
#include "coll.h"
#include "hlldbgsym.h"
#include "strbuf.h"

/* ca65 */
#include "dbginfo.h"
#include "error.h"
#include "expr.h"
#include "filetab.h"
#include "global.h"
#include "lineinfo.h"
#include "objfile.h"
#include "nexttok.h"
#include "symtab.h"



/*****************************************************************************/
/*     	       	    		     Data                                    */
/*****************************************************************************/



/* The current line info */
static LineInfo* CurLineInfo = 0;

/* List of high level language debug symbols */
static Collection HLLDbgSyms = STATIC_COLLECTION_INITIALIZER;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static HLLDbgSym* NewHLLDbgSym (unsigned Flags, unsigned Name, unsigned Type)
/* Allocate and return a new HLLDbgSym structure */
{
    /* Allocate memory */
    HLLDbgSym* S = xmalloc (sizeof (*S));

    /* Initialize the fields as necessary */
    S->Flags    = Flags;
    S->Name     = Name;
    S->AsmName  = EMPTY_STRING_ID;
    S->Offs     = 0;
    S->Type     = Type;
    S->ScopeId  = CurrentScope->Id;

    /* Return the result */
    return S;
}



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

    unsigned    Name;
    unsigned    Type;
    unsigned    AsmName;
    unsigned    Flags;
    HLLDbgSym*  S;


    /* Parameters are separated by a comma */
    ConsumeComma ();

    /* Name */
    if (CurTok.Tok != TOK_STRCON) {
       	ErrorSkip ("String constant expected");
       	return;
    }
    Name = GetStrBufId (&CurTok.SVal);
    NextTok ();

    /* Comma expected */
    ConsumeComma ();

    /* Type */
    if (CurTok.Tok != TOK_STRCON) {
       	ErrorSkip ("String constant expected");
       	return;
    }
    Type = GetStrBufId (&CurTok.SVal);
    NextTok ();

    /* Comma expected */
    ConsumeComma ();

    /* The storage class follows */
    if (CurTok.Tok != TOK_IDENT) {
       	ErrorSkip ("Storage class specifier expected");
       	return;
    }
    switch (GetSubKey (StorageKeys, sizeof (StorageKeys)/sizeof (StorageKeys[0]))) {
        case 0:   Flags = HLL_TYPE_FUNC | HLL_SC_EXTERN;            break;
        case 1:   Flags = HLL_TYPE_FUNC | HLL_SC_STATIC;            break;
        default:  ErrorSkip ("Storage class specifier expected");   return;
    }
    NextTok ();

    /* Comma expected */
    ConsumeComma ();

    /* Assembler name follows */
    if (CurTok.Tok != TOK_STRCON) {
        ErrorSkip ("String constant expected");
        return;
    }
    AsmName = GetStrBufId (&CurTok.SVal);
    NextTok ();

    /* There may only be one function per scope */
    if (CurrentScope == RootScope) {
        ErrorSkip ("Functions may not be used in the root scope");
        return;
    } else if (CurrentScope->Flags & ST_EXTFUNC) {
        ErrorSkip ("Only one function per scope allowed");
        return;
    }
    CurrentScope->Flags |= ST_EXTFUNC;

    /* Add the function */
    S = NewHLLDbgSym (Flags, Name, Type);
    S->AsmName = AsmName;
    CollAppend (&HLLDbgSyms, S);
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

    unsigned    Name;
    unsigned    Type;
    unsigned    AsmName = EMPTY_STRING_ID;
    unsigned    Flags;
    int         Offs;
    HLLDbgSym*  S;


    /* Parameters are separated by a comma */
    ConsumeComma ();

    /* Name */
    if (CurTok.Tok != TOK_STRCON) {
       	ErrorSkip ("String constant expected");
       	return;
    }
    Name = GetStrBufId (&CurTok.SVal);
    NextTok ();

    /* Comma expected */
    ConsumeComma ();

    /* Type */
    if (CurTok.Tok != TOK_STRCON) {
       	ErrorSkip ("String constant expected");
       	return;
    }
    Type = GetStrBufId (&CurTok.SVal);
    NextTok ();

    /* Comma expected */
    ConsumeComma ();

    /* The storage class follows */
    if (CurTok.Tok != TOK_IDENT) {
       	ErrorSkip ("Storage class specifier expected");
       	return;
    }
    switch (GetSubKey (StorageKeys, sizeof (StorageKeys)/sizeof (StorageKeys[0]))) {
        case 0:   Flags = HLL_SC_AUTO;                              break;
        case 1:   Flags = HLL_SC_EXTERN;                            break;
        case 2:   Flags = HLL_SC_REG;                               break;
        case 3:   Flags = HLL_SC_STATIC;                            break;
        default:  ErrorSkip ("Storage class specifier expected");   return;
    }

    /* Skip the storage class token and the following comma */
    NextTok ();
    ConsumeComma ();

    /* The next tokens depend on the storage class */
    if (Flags == HLL_SC_AUTO) {
        /* Auto: Stack offset follows */
        Offs = ConstExpression ();
    } else {
        /* Register, extern or static: Assembler name follows */
        if (CurTok.Tok != TOK_STRCON) {
            ErrorSkip ("String constant expected");
            return;
        }
        AsmName = GetStrBufId (&CurTok.SVal);
        NextTok ();

        /* For register, an offset follows */
        if (Flags == HLL_SC_REG) {
            ConsumeComma ();
            Offs = ConstExpression ();
        }
    }

    /* Add the function */
    S = NewHLLDbgSym (Flags | HLL_TYPE_SYM, Name, Type);
    S->AsmName = AsmName;
    S->Offs    = Offs;
    CollAppend (&HLLDbgSyms, S);
}



void WriteHLLDbgSyms (void)
/* Write a list of all high level language symbols to the object file. */
{
    unsigned I;

    /* Only if debug info is enabled */
    if (DbgSyms) {

        /* Write the symbol count to the list */
        ObjWriteVar (CollCount (&HLLDbgSyms));

        /* Walk through list and write all symbols to the file. */
        for (I = 0; I < CollCount (&HLLDbgSyms); ++I) {

            /* Get the next symbol */
            const HLLDbgSym* S = CollAtUnchecked (&HLLDbgSyms, I);

            /* Write the symbol data */
            ObjWriteVar (S->Flags);
            ObjWriteVar (S->Name);
            ObjWriteVar (S->AsmName);
            ObjWriteVar (S->Offs);
            ObjWriteVar (S->Type);
            ObjWriteVar (S->ScopeId);
        }

    } else {

        /* Write a count of zero */
        ObjWriteVar (0);

    }
}



