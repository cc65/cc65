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

/* ca65 */
#include "error.h"
#include "nexttok.h"
#include "scanner.h"
#include "symbol.h"
#include "symtab.h"



/*****************************************************************************/
/*     	      	       		     Data				     */
/*****************************************************************************/



/*****************************************************************************/
/*     	       	       	  	     Code	  		   	     */
/*****************************************************************************/



SymEntry* ParseScopedSymName (int AllocNew)
/* Parse a (possibly scoped) symbol name, search for it in the symbol table
 * and return the symbol table entry.
 */
{
    /* Get the starting table */
    SymTable* Scope;
    if (Tok == TOK_NAMESPACE) {
        Scope = RootScope;
        NextTok ();
    } else {
        Scope = CurrentScope;
        /* ### Need to walk up the tree */
    }

    /* Resolve scopes */
    while (1) {

        /* An identifier must follow. Remember and skip it. */
        char Name[sizeof (SVal)];
        if (Tok != TOK_IDENT) {
            Error ("Identifier expected");
            return 0;
        }
        strcpy (Name, SVal);
        NextTok ();

        /* If the next token is a namespace token, handle the name as the
         * name of a scope, otherwise it's the name of a symbol in that
         * scope.
         */

        if (Tok == TOK_NAMESPACE) {

            /* Search for the child scope */
            Scope = SymFindScope (Scope, Name, AllocNew);

	    /* Skip the namespace token */
	    NextTok ();

            /* If we didn't find the scope, bail out */
            if (Scope == 0) {
                return 0;
            }

        } else {

            /* Search for the symbol and return it */
            return SymFind (Scope, Name, AllocNew);

        }
    }
}



SymTable* ParseScopedSymTable (int AllocNew)
/* Parse a (possibly scoped) symbol table (scope) name, search for it in the
 * symbol space and return the symbol table struct.
 */
{
    /* Get the starting table */
    SymTable* Scope;
    if (Tok == TOK_NAMESPACE) {
        Scope = RootScope;
        NextTok ();
    } else {
        Scope = CurrentScope;
        if (Tok != TOK_IDENT) {
            Error ("Identifier expected");
            return Scope;
        }

        /* If no new scope should be allocated, the scope may specify any
         * scope in any of the parent scopes, so search for it.
         */
        if (!AllocNew) {
            Scope = SymFindAnyScope (Scope, SVal);
            NextTok ();
            if (Tok != TOK_NAMESPACE) {
                return Scope;
            }
            NextTok ();
        }
    }

    /* Resolve scopes. */
    while (Tok == TOK_IDENT) {

        /* Search for the child scope if we have a valid parent */
        if (Scope) {
            Scope = SymFindScope (Scope, SVal, AllocNew);
        }

        /* Skip the name token */
        NextTok ();

        /* If a namespace token follows, read on, otherwise bail out */
        if (Tok == TOK_NAMESPACE) {
            NextTok ();
            if (Tok != TOK_IDENT) {
                Error ("Identifier expected");
            }
        } else {
            break;
        }
    }

    /* Return the scope we found or created */
    return Scope;
}



