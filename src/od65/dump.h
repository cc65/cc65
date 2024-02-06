/*****************************************************************************/
/*                                                                           */
/*                                  dump.h                                   */
/*                                                                           */
/*          Dump subroutines for the od65 object file dump utility           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2011, Ullrich von Bassewitz                                      */
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



#include <stdio.h>



#ifndef DUMP_H
#define DUMP_H



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void DumpObjHeader (FILE* F, unsigned long Offset);
/* Dump the header of the given object file */

void DumpObjOptions (FILE* F, unsigned long Offset);
/* Dump the file options */

void DumpObjFiles (FILE* F, unsigned long Offset);
/* Dump the source files */

void DumpObjSegments (FILE* F, unsigned long Offset);
/* Dump the segments in the object file */

void DumpObjImports (FILE* F, unsigned long Offset);
/* Dump the imports in the object file */

void DumpObjExports (FILE* F, unsigned long Offset);
/* Dump the exports in the object file */

void DumpObjDbgSyms (FILE* F, unsigned long Offset);
/* Dump the debug symbols from an object file */

void DumpObjLineInfo (FILE* F, unsigned long Offset);
/* Dump the line infos from an object file */

void DumpObjScopes (FILE* F, unsigned long Offset);
/* Dump the scopes from an object file */

void DumpObjSegSize (FILE* F, unsigned long Offset);
/* Dump the sizes of the segment in the object file */



/* End of dump.h */

#endif
