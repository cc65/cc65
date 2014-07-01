/*****************************************************************************/
/*                                                                           */
/*                                 symbol.c                                  */
/*                                                                           */
/*                   Parse a symbol name and search for it                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2012, Ullrich von Bassewitz                                      */
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
#include "nexttok.h"
#include "scanner.h"
#include "symbol.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



SymTable* ParseScopedIdent (StrBuf* Name, StrBuf* FullName)
/* Parse a (possibly scoped) identifer. The scope of the name must exist and
** is returned as function result, while the last part (the identifier) which
** may be either a symbol or a scope depending on the context is returned in
** Name. FullName is a string buffer that is used to store the full name of
** the identifier including the scope. It is used internally and may be used
** by the caller for error messages or similar.
*/
{
    SymTable* Scope;

    /* Clear both passed string buffers */
    SB_Clear (Name);
    SB_Clear (FullName);

    /* Get the starting table */
    if (CurTok.Tok == TOK_NAMESPACE) {

        /* Start from the root scope */
        Scope = RootScope;

    } else if (CurTok.Tok == TOK_IDENT) {

        /* Remember the name and skip it */
        SB_Copy (Name, &CurTok.SVal);
        NextTok ();

        /* If no namespace symbol follows, we're already done */
        if (CurTok.Tok != TOK_NAMESPACE) {
            SB_Terminate (FullName);
            return CurrentScope;
        }

        /* Pass the scope back to the caller */
        SB_Append (FullName, Name);

        /* The scope must exist, so search for it starting with the current
        ** scope.
        */
        Scope = SymFindAnyScope (CurrentScope, Name);
        if (Scope == 0) {
            /* Scope not found */
            SB_Terminate (FullName);
            Error ("No such scope: `%m%p'", FullName);
            return 0;
        }

    } else {

        /* Invalid token */
        Error ("Identifier expected");
        return 0;

    }

    /* Skip the namespace token that follows */
    SB_AppendStr (FullName, "::");
    NextTok ();

    /* Resolve scopes. */
    while (1) {

        /* Next token must be an identifier. */
        if (CurTok.Tok != TOK_IDENT) {
            Error ("Identifier expected");
            return 0;
        }

        /* Remember and skip the identifier */
        SB_Copy (Name, &CurTok.SVal);
        NextTok ();

        /* If a namespace token follows, we search for another scope, otherwise
        ** the name is a symbol and we're done.
        */
        if (CurTok.Tok != TOK_NAMESPACE) {
            /* Symbol */
            return Scope;
        }

        /* Pass the scope back to the caller */
        SB_Append (FullName, Name);

        /* Search for the child scope */
        Scope = SymFindScope (Scope, Name, SYM_FIND_EXISTING);
        if (Scope == 0) {
            /* Scope not found */
            Error ("No such scope: `%m%p'", FullName);
            return 0;
        }

        /* Skip the namespace token that follows */
        SB_AppendStr (FullName, "::");
        NextTok ();
    }
}



SymEntry* ParseScopedSymName (SymFindAction Action)
/* Parse a (possibly scoped) symbol name, search for it in the symbol table
** and return the symbol table entry.
*/
{
    StrBuf    ScopeName = STATIC_STRBUF_INITIALIZER;
    StrBuf    Ident = STATIC_STRBUF_INITIALIZER;
    int       NoScope;
    SymEntry* Sym;

    /* Parse the scoped symbol name */
    SymTable* Scope = ParseScopedIdent (&Ident, &ScopeName);

    /* If ScopeName is empty, no scope was specified */
    NoScope = SB_IsEmpty (&ScopeName);

    /* We don't need ScopeName any longer */
    SB_Done (&ScopeName);

    /* Check if the scope is valid. Errors have already been diagnosed by
    ** the routine, so just exit.
    */
    if (Scope) {
        /* Search for the symbol and return it. If no scope was specified,
        ** search also in the upper levels.
        */
        if (NoScope && (Action & SYM_ALLOC_NEW) == 0) {
            Sym = SymFindAny (Scope, &Ident);
        } else {
            Sym = SymFind (Scope, &Ident, Action);
        }
    } else {
        /* No scope ==> no symbol. To avoid errors in the calling routine that
        ** may not expect NULL to be returned if Action contains SYM_ALLOC_NEW,
        ** create a new symbol.
        */
        if (Action & SYM_ALLOC_NEW) { 
            Sym = NewSymEntry (&Ident, SF_NONE);
        } else {
            Sym = 0;
        }
    }

    /* Deallocate memory for ident */
    SB_Done (&Ident);

    /* Return the symbol found */
    return Sym;
}



SymTable* ParseScopedSymTable (void)
/* Parse a (possibly scoped) symbol table (scope) name, search for it in the
** symbol space and return the symbol table struct.
*/
{
    StrBuf    ScopeName = STATIC_STRBUF_INITIALIZER;
    StrBuf    Name = STATIC_STRBUF_INITIALIZER;
    int       NoScope;


    /* Parse the scoped symbol name */
    SymTable* Scope = ParseScopedIdent (&Name, &ScopeName);

    /* If ScopeName is empty, no scope was specified */
    NoScope = SB_IsEmpty (&ScopeName);

    /* We don't need FullName any longer */
    SB_Done (&ScopeName);

    /* If we got no error, search for the child scope withint the enclosing one.
    ** Beware: If no explicit parent scope was specified, search in all upper
    ** levels.
    */
    if (Scope) {
        /* Search for the last scope */
        if (NoScope) {
            Scope = SymFindAnyScope (Scope, &Name);
        } else {
            Scope = SymFindScope (Scope, &Name, SYM_FIND_EXISTING);
        }
    }

    /* Free memory for name */
    SB_Done (&Name);

    /* Return the scope found */
    return Scope;
}



SymEntry* ParseAnySymName (SymFindAction Action)
/* Parse a cheap local symbol or a a (possibly scoped) symbol name, search
** for it in the symbol table and return the symbol table entry.
*/
{
    SymEntry* Sym;

    /* Distinguish cheap locals and other symbols */
    if (CurTok.Tok == TOK_LOCAL_IDENT) {
        Sym = SymFindLocal (SymLast, &CurTok.SVal, Action);
        NextTok ();
    } else {
        Sym = ParseScopedSymName (Action);
    }

    /* Return the symbol found */
    return Sym;
}
