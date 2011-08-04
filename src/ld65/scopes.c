/*****************************************************************************/
/*                                                                           */
/*                                 scopes.c                                  */
/*                                                                           */
/*                    Scope handling for the ld65 linker                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2011,      Ullrich von Bassewitz                                      */
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
#include "xmalloc.h"

/* ld65 */
#include "error.h"
#include "fileio.h"
#include "scopes.h"
#include "span.h"



/*****************************************************************************/
/*     	      	    		     Code			       	     */
/*****************************************************************************/



static Scope* NewScope (ObjData* Obj, unsigned Id)
/* Create a new Scope and return it */
{
    /* Allocate memory */
    Scope* S     = xmalloc (sizeof (Scope));

    /* Initialize the fields where necessary */
    S->Id       = Id;
    S->Obj      = Obj;
    S->Size     = 0;
    S->Spans    = EmptyCollection;

    /* Return the new entry */
    return S;
}



Scope* ReadScope (FILE* F, ObjData* Obj, unsigned Id)
/* Read a scope from a file and return it */
{
    /* Create a new scope */
    Scope* S = NewScope (Obj, Id);

    /* Read the data from file */
    S->Parent.Id    = ReadVar (F);
    S->LexicalLevel = ReadVar (F);
    S->Flags        = ReadVar (F);
    S->Type         = ReadVar (F);
    S->Name         = MakeGlobalStringId (Obj, ReadVar (F));
    if (SCOPE_HAS_SIZE (S->Flags)) {
        S->Size     = ReadVar (F);
    }

    /* Read the segment ranges for this scope */
    ReadSpans (&S->Spans, F, Obj);

    /* Return the new Scope */
    return S;
}



void ResolveScopes (ObjData* Obj)
/* Resolve a scope list. */
{
    unsigned I;

    /* Walk over the list and resolve the parent ids. */
    for (I = 0; I < CollCount (&Obj->Scopes); ++I) {

        /* Get the scope */
        Scope* S = CollAtUnchecked (&Obj->Scopes, I);

        /* Resolve the parent id. The root scope doesn't have a parent */
        if (S->Id == 0) {
            /* Root scope */
            S->Parent.Scope = 0;
        } else {
            S->Parent.Scope = GetObjScope (Obj, S->Parent.Id);
        }
    }
}



