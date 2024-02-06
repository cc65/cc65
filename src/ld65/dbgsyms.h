/*****************************************************************************/
/*                                                                           */
/*                                 dbgsyms.h                                 */
/*                                                                           */
/*                 Debug symbol handling for the ld65 linker                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2011, Ullrich von Bassewitz                                      */
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



#ifndef DBGSYMS_H
#define DBGSYMS_H



#include <stdio.h>

/* common */
#include "coll.h"
#include "exprdefs.h"

/* ld65 */
#include "objdata.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Forwards */
struct Scope;
struct HLLDbgSym;

/* Opaque debug symbol structure */
typedef struct DbgSym DbgSym;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



DbgSym* ReadDbgSym (FILE* F, ObjData* Obj, unsigned Id);
/* Read a debug symbol from a file, insert and return it */

struct HLLDbgSym* ReadHLLDbgSym (FILE* F, ObjData* Obj, unsigned Id);
/* Read a hll debug symbol from a file, insert and return it */

void PrintDbgSyms (FILE* F);
/* Print the debug symbols in a debug file */

unsigned DbgSymCount (void);
/* Return the total number of debug symbols */

unsigned HLLDbgSymCount (void);
/* Return the total number of high level language debug symbols */

void PrintHLLDbgSyms (FILE* F);
/* Print the high level language debug symbols in a debug file */

void PrintDbgSymLabels (FILE* F);
/* Print the debug symbols in a VICE label file */



/* End of dbgsyms.h */

#endif
