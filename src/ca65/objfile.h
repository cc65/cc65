/*****************************************************************************/
/*                                                                           */
/*                                 objfile.h                                 */
/*                                                                           */
/*         Object file writing routines for the ca65 macroassembler          */
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



/* common */
#include "filepos.h"
#include "strbuf.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void ObjOpen (void);
/* Open the object file for writing, write a dummy header */

void ObjClose (void);
/* Write an update header and close the object file. */

unsigned long ObjGetFilePos (void);
/* Get the current file position */

void ObjSetFilePos (unsigned long Pos);
/* Set the file position */

void ObjWrite8 (unsigned V);
/* Write an 8 bit value to the file */

void ObjWrite16 (unsigned V);
/* Write a 16 bit value to the file */

void ObjWrite24 (unsigned long V);
/* Write a 24 bit value to the file */

void ObjWrite32 (unsigned long V);
/* Write a 32 bit value to the file */

void ObjWriteVar (unsigned long V);
/* Write a variable sized value to the file in special encoding */

void ObjWriteStr (const char* S);
/* Write a string to the object file */

void ObjWriteBuf (const StrBuf* S);
/* Write a string to the object file */

void ObjWriteData (const void* Data, unsigned Size);
/* Write literal data to the file */

void ObjWritePos (const FilePos* Pos);
/* Write a file position to the object file */

void ObjStartOptions (void);
/* Mark the start of the option section */

void ObjEndOptions (void);
/* Mark the end of the option section */

void ObjStartFiles (void);
/* Mark the start of the files section */

void ObjEndFiles (void);
/* Mark the end of the files section */

void ObjStartSegments (void);
/* Mark the start of the segment section */

void ObjEndSegments (void);
/* Mark the end of the segment section */

void ObjStartImports (void);
/* Mark the start of the import section */

void ObjEndImports (void);
/* Mark the end of the import section */

void ObjStartExports (void);
/* Mark the start of the export section */

void ObjEndExports (void);
/* Mark the end of the export section */

void ObjStartDbgSyms (void);
/* Mark the start of the debug symbol section */

void ObjEndDbgSyms (void);
/* Mark the end of the debug symbol section */

void ObjStartLineInfos (void);
/* Mark the start of the line info section */

void ObjEndLineInfos (void);
/* Mark the end of the line info section */

void ObjStartStrPool (void);
/* Mark the start of the string pool section */

void ObjEndStrPool (void);
/* Mark the end of the string pool section */

void ObjStartAssertions (void);
/* Mark the start of the assertion table */

void ObjEndAssertions (void);
/* Mark the end of the assertion table */

void ObjStartScopes (void);
/* Mark the start of the scope table */

void ObjEndScopes (void);
/* Mark the end of the scope table */

void ObjStartSpans (void);
/* Mark the start of the span table */

void ObjEndSpans (void);
/* Mark the end of the span table */



/* End of objfile.h */

#endif
