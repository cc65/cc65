/*****************************************************************************/
/*                                                                           */
/*                                 objfile.h                                 */
/*                                                                           */
/*                 Object file handling for the ld65 linker                  */
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



#ifndef OBJFILE_H
#define OBJFILE_H



#include <stdio.h>

/* common */
#include "objdefs.h"

/* ld65 */
#include "objdata.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void ObjReadFiles (FILE* F, unsigned long Pos, ObjData* O);
/* Read the files list from a file at the given position */

void ObjReadSections (FILE* F, unsigned long Pos, ObjData* O);
/* Read the section data from a file at the given position */

void ObjReadImports (FILE* F, unsigned long Pos, ObjData* O);
/* Read the imports from a file at the given position */

void ObjReadExports (FILE* F, unsigned long Pos, ObjData* O);
/* Read the exports from a file at the given position */

void ObjReadDbgSyms (FILE* F, unsigned long Pos, ObjData* O);
/* Read the debug symbols from a file at the given position */

void ObjReadLineInfos (FILE* F, unsigned long Pos, ObjData* O);
/* Read the line infos from a file at the given position */

void ObjReadStrPool (FILE* F, unsigned long Pos, ObjData* O);
/* Read the string pool from a file at the given position */

void ObjReadAssertions (FILE* F, unsigned long Pos, ObjData* O);
/* Read the assertions from a file at the given offset */

void ObjReadScopes (FILE* F, unsigned long Pos, ObjData* O);
/* Read the scope table from a file at the given offset */

void ObjReadSpans (FILE* F, unsigned long Pos, ObjData* O);
/* Read the span table from a file at the given offset */

void ObjAdd (FILE* F, const char* Name);
/* Add an object file to the module list */



/* End of objfile.h */

#endif
