/*****************************************************************************/
/*                                                                           */
/*                                 struct.c                                  */
/*                                                                           */
/*                          .STRUCT/.UNION commands                          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
/*               Römerstraße 52                                              */
/*               D-70794 Filderstadt                                         */
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
#include "addrsize.h"

/* ca65 */
#include "error.h"
#include "expr.h"
#include "nexttok.h"
#include "scanner.h"
#include "symbol.h"
#include "symtab.h"
#include "struct.h"



/*****************************************************************************/
/*     	       	    		     Data				     */
/*****************************************************************************/



enum {
    STRUCT,
    UNION
};



/*****************************************************************************/
/*     	       	    		     Code	   			     */
/*****************************************************************************/



static long Member (long AllocSize)
/* Read one struct member and return its size */
{
    long Multiplicator;

    /* A comma and a multiplicator may follow */
    if (Tok == TOK_COMMA) {
        NextTok ();
        Multiplicator = ConstExpression ();
        if (Multiplicator <= 0) {
            Error ("Range error");
            Multiplicator = 1;
        }
        AllocSize *= Multiplicator;
    }

    /* Return the size */
    return AllocSize;
}



static long DoStructInternal (long Offs, unsigned Type)
/* Handle the .STRUCT command */
{
    long Size = 0;

    /* Outside of other structs, we need a name. Inside another struct or
     * union, the struct may be anonymous, in which case no new lexical level
     * is started.
     */
    int Anon = (Tok != TOK_IDENT);
    if (Anon) {
        unsigned char T = GetCurrentSymTabType ();
        if (T != ST_STRUCT) {
            ErrorSkip ("Struct/union needs a name");
            return 0;
        }
    } else {
        /* Enter a new scope, then skip the name */
        SymEnterLevel (SVal, ST_STRUCT, ADDR_SIZE_ABS);
        NextTok ();
        /* Start at zero offset in the new scope */
        Offs = 0;
    }

    /* Test for end of line */
    ConsumeSep ();

    /* Read until end of struct */
    while (Tok != TOK_ENDSTRUCT && Tok != TOK_ENDUNION && Tok != TOK_EOF) {

        long      MemberSize;
        SymEntry* Sym;
        SymTable* Struct;

        /* The format is "[identifier] storage-allocator [, multiplicator]" */
        if (Tok == TOK_IDENT) {
            /* We have an identifier, generate a symbol */
            Sym = SymFind (CurrentScope, SVal, SYM_ALLOC_NEW);

            /* Assign the symbol the offset of the current member */
            SymDef (Sym, GenLiteralExpr (Offs), ADDR_SIZE_DEFAULT, SF_NONE);

            /* Skip the member name */
            NextTok ();
        }

        /* Read storage allocators */
        MemberSize = 0;                 /* In case of errors, use zero */
        switch (Tok) {

            case TOK_BYTE:
                NextTok ();
                MemberSize = Member (1);
                break;

            case TOK_DBYT:
            case TOK_WORD:
            case TOK_ADDR:
                NextTok ();
                MemberSize = Member (2);
                break;

            case TOK_FARADDR:
                NextTok ();
                MemberSize = Member (3);
                break;

            case TOK_DWORD:
                NextTok ();
                MemberSize = Member (4);
                break;

            case TOK_RES:
                Error ("Not implemented");
                break;

            case TOK_TAG:
                NextTok ();
                Struct = ParseScopedSymTable (SYM_FIND_EXISTING);
                if (Struct == 0) {
                    Error ("Unknown struct/union");
                } else if (GetSymTabType (Struct) != ST_STRUCT) {
                    Error ("Not a struct/union");
                } else {
                    MemberSize = Member (GetStructSize (Struct));
                }
                break;

            case TOK_STRUCT:
                NextTok ();
                MemberSize = DoStructInternal (Offs, STRUCT);
                break;

            case TOK_UNION:
                NextTok ();
                MemberSize = DoStructInternal (Offs, UNION);
                break;

            default:
                Error ("Invalid storage allocator in struct/union");
                SkipUntilSep ();
        }

        /* Next member */
        if (Type == STRUCT) {
            /* Struct */
            Offs += MemberSize;
            Size += MemberSize;
        } else {
            /* Union */
            if (MemberSize > Size) {
                Size = MemberSize;
            }
        }

        /* Expect end of line */
        ConsumeSep ();
    }

    /* If this is not a anon struct, enter a special symbol named ".size"
     * into the symbol table of the struct that holds the size of the
     * struct. Since the symbol starts with a dot, it cannot be accessed
     * by user code.
     * Leave the struct scope level.
     */
    if (!Anon) {
        /* Add a symbol */
        SymEntry* SizeSym = SymFind (CurrentScope, ".size", SYM_ALLOC_NEW);
        SymDef (SizeSym, GenLiteralExpr (Size), ADDR_SIZE_DEFAULT, SF_NONE);

        /* Close the struct scope */
        SymLeaveLevel ();
    }

    /* End of struct/union definition */
    if (Type == STRUCT) {
        Consume (TOK_ENDSTRUCT, "`.ENDSTRUCT' expected");
    } else {
        Consume (TOK_ENDUNION, "`.ENDUNION' expected");
    }

    /* Return the size of the struct */
    return Size;
}



long GetStructSize (SymTable* Struct)
/* Get the size of a struct or union */
{
    SymEntry* Sym = SymFind (Struct, ".size", SYM_FIND_EXISTING);
    if (Sym == 0) {
        Error ("Size of struct/union is unknown");
        return 0;
    } else {
        return GetSymVal (Sym);
    }
}



void DoStruct (void)
/* Handle the .STRUCT command */
{
    DoStructInternal (0, STRUCT);
}



void DoUnion (void)
/* Handle the .UNION command */
{
    DoStructInternal (0, UNION);
}



