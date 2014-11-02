/*****************************************************************************/
/*                                                                           */
/*                                lineinfo.h                                 */
/*                                                                           */
/*                     Source file line info management                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2011, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                70794 Filderstadt                                          */
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



#ifndef LINEINFO_H
#define LINEINFO_H



/* common */
#include "coll.h"
#include "filepos.h"
#include "hashtab.h"
#include "lidefs.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Opaque structure used to handle line information */
typedef struct LineInfo LineInfo;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void InitLineInfo (void);
/* Initialize the line infos */

void DoneLineInfo (void);
/* Close down line infos */

void EndLine (LineInfo* LI);
/* End a line that is tracked by the given LineInfo structure */

LineInfo* StartLine (const FilePos* Pos, unsigned Type, unsigned Count);
/* Start line info for a new line */

void NewAsmLine (void);
/* Start a new assembler input line. Use this function when generating new
** line of LI_TYPE_ASM. It will check if line and/or file have actually
** changed, end the old and start the new line as necessary.
*/

LineInfo* GetAsmLineInfo (void);
/* Return the line info for the current assembler file. The function will
** bump the reference counter before returning the line info.
*/

void ReleaseLineInfo (LineInfo* LI);
/* Decrease the reference count for a line info */

void GetFullLineInfo (Collection* LineInfos);
/* Return full line infos, that is line infos for currently active Slots. The
** infos will be added to the given collection, existing entries will be left
** intact. The reference count of all added entries will be increased.
*/

void ReleaseFullLineInfo (Collection* LineInfos);
/* Decrease the reference count for a collection full of LineInfos, then clear
** the collection.
*/

const FilePos* GetSourcePos (const LineInfo* LI);
/* Return the source file position from the given line info */

unsigned GetLineInfoType (const LineInfo* LI);
/* Return the type of a line info */

void WriteLineInfo (const Collection* LineInfos);
/* Write a list of line infos to the object file. */

void WriteLineInfos (void);
/* Write a list of all line infos to the object file. */



/* End of lineinfo.h */

#endif
