/*****************************************************************************/
/*                                                                           */
/*                                 scopes.h                                  */
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



#ifndef SCOPES_H
#define SCOPES_H



#include <stdio.h>

/* common */
#include "coll.h"
#include "scopedefs.h"

/* ld65 */
#include "objdata.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Debug symbol structure */
typedef struct Scope Scope;
struct Scope {
    unsigned            Id;             /* Id of scope */
    ObjData*            Obj;            /* Object file that contains the scope */
    unsigned            ParentId;       /* Id of parent scope */
    unsigned            LabelId;        /* Id of the scope label if any */
    unsigned            LexicalLevel;   /* Lexical level */
    unsigned            Flags;
    unsigned            Type;           /* Type of scope */
    unsigned            Name;           /* Name of scope */
    unsigned long       Size;           /* Size of scope */
    unsigned*           Spans;          /* Spans for this scope */
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



Scope* ReadScope (FILE* F, ObjData* Obj, unsigned Id);
/* Read a scope from a file, insert and return it */

unsigned ScopeCount (void);
/* Return the total number of scopes */

void PrintDbgScopes (FILE* F);
/* Output the scopes to a debug info file */



/* End of scopes.h */

#endif
