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
#include "spool.h"



/*****************************************************************************/
/*                                   Code                                    */
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
    S->LabelId  = ~0U;
    S->Spans    = 0;

    /* Return the new entry */
    return S;
}



Scope* ReadScope (FILE* F, ObjData* Obj, unsigned Id)
/* Read a scope from a file and return it */
{
    /* Create a new scope */
    Scope* S = NewScope (Obj, Id);

    /* Read the data from file */
    S->ParentId     = ReadVar (F);
    S->LexicalLevel = ReadVar (F);
    S->Flags        = ReadVar (F);
    S->Type         = ReadVar (F);
    S->Name         = MakeGlobalStringId (Obj, ReadVar (F));
    if (SCOPE_HAS_SIZE (S->Flags)) {
        S->Size     = ReadVar (F);
    }
    if (SCOPE_HAS_LABEL (S->Flags)) {
        S->LabelId  = ReadVar (F);
    }
    S->Spans        = ReadSpanList (F);

    /* Return the new Scope */
    return S;
}



unsigned ScopeCount (void)
/* Return the total number of scopes */
{

    /* Count scopes from all modules we have linked into the output file */
    unsigned I;
    unsigned Count = 0;
    for (I = 0; I < CollCount (&ObjDataList); ++I) {
        /* Get the object file */
        const ObjData* O = CollAtUnchecked (&ObjDataList, I);

        /* Account for the scopes in this file */
        Count += CollCount (&O->Scopes);
    }
    return Count;
}



void PrintDbgScopes (FILE* F)
/* Output the scopes to a debug info file */
{
    unsigned I, J;

    /* Print scopes from all modules we have linked into the output file */
    for (I = 0; I < CollCount (&ObjDataList); ++I) {

        /* Get the object file */
        ObjData* O = CollAtUnchecked (&ObjDataList, I);

        /* Output the scopes for this object file */
        for (J = 0; J < CollCount (&O->Scopes); ++J) {
            const Scope* S = CollConstAt (&O->Scopes, J);

            /* Output the first chunk of data */
            fprintf (F,
                     "scope\tid=%u,name=\"%s\",mod=%u",
                     O->ScopeBaseId + S->Id,
                     GetString (S->Name),
                     I);

            /* Print the type if not module */
            switch (S->Type) {

                case SCOPE_GLOBAL:      fputs (",type=global", F);      break;
                case SCOPE_FILE:        /* default */                   break;
                case SCOPE_SCOPE:       fputs (",type=scope", F);       break;
                case SCOPE_STRUCT:      fputs (",type=struct", F);      break;
                case SCOPE_ENUM:        fputs (",type=enum", F);        break;

                default:
                    Error ("Module `%s': Unknown scope type %u",
                           GetObjFileName (O), S->Type);
            }

            /* Print the size if available */
            if (S->Size != 0) {
                fprintf (F, ",size=%lu", S->Size);
            }
            /* Print parent if available */
            if (S->Id != S->ParentId) {
                fprintf (F, ",parent=%u", O->ScopeBaseId + S->ParentId);
            }
            /* Print the label id if the scope is labeled */
            if (SCOPE_HAS_LABEL (S->Flags)) {
                fprintf (F, ",sym=%u", O->SymBaseId + S->LabelId);
            }
            /* Print the list of spans for this scope */
            PrintDbgSpanList (F, O, S->Spans);

            /* Terminate the output line */
            fputc ('\n', F);
        }
    }
}
