/*****************************************************************************/
/*                                                                           */
/*                                 symbol.c                                  */
/*                                                                           */
/*                   Parse a symbol name and search for it                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
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



#include <string.h>

/* common */
#include "strbuf.h"

/* ca65 */
#include "error.h"
#include "nexttok.h"
#include "scanner.h"
#include "symbol.h"
#include "symtab.h"



/*****************************************************************************/
/*     	       	       	  	     Code	       		   	     */
/*****************************************************************************/



SymTable* ParseScopedIdent (char* Name, StrBuf* ScopeName)
/* Parse a (possibly scoped) identifer. Name must point to a buffer big enough
 * to hold such an identifier. The scope of the name must exist and is returned
 * as function result, while the last part (the identifier) which may be either
 * a symbol or a scope depending on the context is returned in Name. ScopeName
 * is a string buffer that is used to store the name of the scope, the
 * identifier lives in. It does contain anything but the identifier itself, so
 * if ScopeName is empty on return, no explicit scope was specified. The full
 * name of the identifier (including the scope) is ScopeName+Name.
 */
{
    /* Get the starting table */
    SymTable* Scope;
    if (Tok == TOK_NAMESPACE) {

        /* Start from the root scope */
        Scope = RootScope;

    } else if (Tok == TOK_IDENT) {

        /* Remember the name and skip it */
        strcpy (Name, SVal);
        NextTok ();

        /* If no namespace symbol follows, we're already done */
        if (Tok != TOK_NAMESPACE) {
            SB_Terminate (ScopeName);
            return CurrentScope;
        }

        /* Pass the scope back to the caller */
        SB_AppendStr (ScopeName, Name);

        /* The scope must exist, so search for it starting with the current
         * scope.
         */
        Scope = SymFindAnyScope (CurrentScope, Name);
        if (Scope == 0) {
            /* Scope not found */
            SB_Terminate (ScopeName);
            Error ("No such scope: `%s'", SB_GetConstBuf (ScopeName));
            return 0;
        }

    } else {

        /* Invalid token */
        Error ("Identifier expected");
        SB_Terminate (ScopeName);
        Name[0] = '\0';
        return 0;

    }

    /* Skip the namespace token that follows */
    SB_AppendStr (ScopeName, "::");
    NextTok ();

    /* Resolve scopes. */
    while (1) {

        /* Next token must be an identifier. */
        if (Tok != TOK_IDENT) {
            Error ("Identifier expected");
            SB_Terminate (ScopeName);
            Name[0] = '\0';
            return 0;
        }

        /* Remember and skip the identifier */
        strcpy (Name, SVal);
        NextTok ();

        /* If a namespace token follows, we search for another scope, otherwise
         * the name is a symbol and we're done.
         */
        if (Tok != TOK_NAMESPACE) {
            /* Symbol */
            SB_Terminate (ScopeName);
            return Scope;
        }

        /* Pass the scope back to the caller */
        SB_AppendStr (ScopeName, Name);

        /* Search for the child scope */
        Scope = SymFindScope (Scope, Name, SYM_FIND_EXISTING);
        if (Scope == 0) {
            /* Scope not found */
            SB_Terminate (ScopeName);
            Error ("No such scope: `%s'", SB_GetConstBuf (ScopeName));
            return 0;
        }

        /* Skip the namespace token that follows */
        SB_AppendStr (ScopeName, "::");
        NextTok ();
    }
}



SymEntry* ParseScopedSymName (int AllocNew)
/* Parse a (possibly scoped) symbol name, search for it in the symbol table
 * and return the symbol table entry.
 */
{
    StrBuf    ScopeName = AUTO_STRBUF_INITIALIZER;
    char      Ident[sizeof (SVal)];
    int       NoScope;
    SymEntry* Sym;

    /* Parse the scoped symbol name */
    SymTable* Scope = ParseScopedIdent (Ident, &ScopeName);

    /* If ScopeName is empty, no scope was specified */
    NoScope = SB_IsEmpty (&ScopeName);

    /* We don't need ScopeName any longer */
    DoneStrBuf (&ScopeName);

    /* Check if the scope is valid. Errors have already been diagnosed by
     * the routine, so just exit.
     */
    if (Scope) {
        /* Search for the symbol and return it. If no scope was specified,
         * search also in the upper levels.
         */
        if (NoScope && !AllocNew) {
            Sym = SymFindAny (Scope, Ident);
        } else {
            Sym = SymFind (Scope, Ident, AllocNew);
        }
    } else {
        /* No scope ==> no symbol. To avoid errors in the calling routine that
         * may not expect NULL to be returned if AllocNew is true, create a new
         * symbol.
         */
        if (AllocNew) {
            Sym = NewSymEntry (Ident, SF_NONE);
        } else {
            Sym = 0;
        }
    }

    /* Return the symbol found */
    return Sym;
}



SymTable* ParseScopedSymTable (void)
/* Parse a (possibly scoped) symbol table (scope) name, search for it in the
 * symbol space and return the symbol table struct.
 */
{
    StrBuf    ScopeName = AUTO_STRBUF_INITIALIZER;
    char      Name[sizeof (SVal)];
    int       NoScope;


    /* Parse the scoped symbol name */
    SymTable* Scope = ParseScopedIdent (Name, &ScopeName);

    /* If ScopeName is empty, no scope was specified */
    NoScope = SB_IsEmpty (&ScopeName);

    /* We don't need FullName any longer */
    DoneStrBuf (&ScopeName);

    /* If we got no error, search for the child scope withint the enclosing one.
     * Beware: If no explicit parent scope was specified, search in all upper
     * levels.
     */
    if (Scope) {
        /* Search for the last scope */
        if (NoScope) {
            Scope = SymFindAnyScope (Scope, Name);
        } else {
            Scope = SymFindScope (Scope, Name, SYM_FIND_EXISTING);
        }
    }
    return Scope;
}



