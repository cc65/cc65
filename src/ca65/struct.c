/*****************************************************************************/
/*                                                                           */
/*                                 struct.c                                  */
/*                                                                           */
/*                          .STRUCT/.UNION commands                          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2011, Ullrich von Bassewitz                                      */
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



/* common */
#include "addrsize.h"
#include "scopedefs.h"

/* ca65 */
#include "condasm.h"
#include "error.h"
#include "expr.h"
#include "macro.h"
#include "nexttok.h"
#include "scanner.h"
#include "sizeof.h"
#include "symbol.h"
#include "symtab.h"
#include "struct.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



enum {
    STRUCT,
    UNION
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static long Member (long AllocSize)
/* Read one struct member and return its size */
{
    long Multiplicator;

    /* A multiplicator may follow */
    if (CurTok.Tok != TOK_SEP) {
        Multiplicator = ConstExpression ();
        if (Multiplicator <= 0) {
            ErrorSkip ("Range error");
            Multiplicator = 1;
        }
        AllocSize *= Multiplicator;
    }

    /* Check the size for a reasonable value */
    if (AllocSize >= 0x10000) {
        ErrorSkip ("Range error");
    }

    /* Return the size */
    return AllocSize;
}



static long DoStructInternal (long Offs, unsigned Type)
/* Handle the .STRUCT command */
{
    long Size = 0;

    /* Outside of other structs, we need a name. Inside another struct or
    ** union, the struct may be anonymous, in which case no new lexical level
    ** is started.
    */
    int Anon = (CurTok.Tok != TOK_IDENT);
    if (!Anon) {
        /* Enter a new scope, then skip the name */
        SymEnterLevel (&CurTok.SVal, SCOPE_STRUCT, ADDR_SIZE_ABS, 0);
        NextTok ();
        /* Start at zero offset in the new scope */
        Offs = 0;
    }

    /* Test for end of line */
    ConsumeSep ();

    /* Read until end of struct */
    while (CurTok.Tok != TOK_ENDSTRUCT &&
           CurTok.Tok != TOK_ENDUNION  &&
           CurTok.Tok != TOK_EOF) {

        long      MemberSize;
        SymTable* Struct;
        SymEntry* Sym;

        /* Allow empty and comment lines */
        if (CurTok.Tok == TOK_SEP) {
            NextTok ();
            continue;
        }

        /* The format is "[identifier] storage-allocator [, multiplicator]" */
        Sym = 0;
        if (CurTok.Tok == TOK_IDENT) {

            /* Beware: An identifier may also be a macro, in which case we have
            ** to start over.
            */
            Macro* M = FindMacro (&CurTok.SVal);
            if (M) {
                MacExpandStart (M);
                continue;
            }

            /* We have an identifier, generate a symbol */
            Sym = SymFind (CurrentScope, &CurTok.SVal, SYM_ALLOC_NEW);

            /* Assign the symbol the offset of the current member */
            SymDef (Sym, GenLiteralExpr (Offs), ADDR_SIZE_DEFAULT, SF_NONE);

            /* Skip the member name */
            NextTok ();
        }

        /* Read storage allocators */
        MemberSize = 0;                 /* In case of errors, use zero */
        switch (CurTok.Tok) {

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
                NextTok ();
                if (CurTok.Tok == TOK_SEP) {
                    ErrorSkip ("Size is missing");
                } else {
                    MemberSize = Member (1);
                }
                break;

            case TOK_TAG:
                NextTok ();
                Struct = ParseScopedSymTable ();
                if (Struct == 0) {
                    ErrorSkip ("Unknown struct/union");
                } else if (GetSymTabType (Struct) != SCOPE_STRUCT) {
                    ErrorSkip ("Not a struct/union");
                } else {
                    SymEntry* SizeSym = GetSizeOfScope (Struct);
                    if (!SymIsDef (SizeSym) || !SymIsConst (SizeSym, &MemberSize)) {
                        ErrorSkip ("Size of struct/union is unknown");
                    }
                }
                MemberSize = Member (MemberSize);
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
                if (!CheckConditionals ()) {
                    /* Not a conditional directive */
                    ErrorSkip ("Invalid storage allocator in struct/union");
                }
        }

        /* Assign the size to the member if it has a name */
        if (Sym) {
            DefSizeOfSymbol (Sym, MemberSize);
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
    ** into the symbol table of the struct that holds the size of the
    ** struct. Since the symbol starts with a dot, it cannot be accessed
    ** by user code.
    ** Leave the struct scope level.
    */
    if (!Anon) {
        /* Add a symbol */
        SymEntry* SizeSym = GetSizeOfScope (CurrentScope);
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
    SymEntry* SizeSym = FindSizeOfScope (Struct);
    if (SizeSym == 0) {
        Error ("Size of struct/union is unknown");
        return 0;
    } else {
        return GetSymVal (SizeSym);
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
