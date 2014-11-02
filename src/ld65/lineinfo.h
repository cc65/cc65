/*****************************************************************************/
/*                                                                           */
/*                                lineinfo.h                                 */
/*                                                                           */
/*                      Source file line info structure                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2012, Ullrich von Bassewitz                                      */
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



#ifndef LINEINFO_H
#define LINEINFO_H



#include <stdio.h>

/* common */
#include "coll.h"
#include "filepos.h"

/* ld65 */
#include "span.h"
#include "spool.h"



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



struct ObjData;
struct Segment;



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Structure holding line information. The Pos.Name field is always the
** global string id of the file name. If the line info was read from the
** object file, the File pointer is valid, otherwise it is NULL.
*/
typedef struct LineInfo LineInfo;
struct LineInfo {
    unsigned            Id;             /* Line info id */
    struct FileInfo*    File;           /* File struct for this line if any */
    unsigned            Type;           /* Type of line info */
    FilePos             Pos;            /* Position in file */
    unsigned*           Spans;          /* Spans for this line */
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



LineInfo* GenLineInfo (const FilePos* Pos);
/* Generate a new (internally used) line info with the given information */

LineInfo* ReadLineInfo (FILE* F, struct ObjData* O);
/* Read a line info from a file and return it */

void FreeLineInfo (LineInfo* LI);
/* Free a LineInfo structure. */

LineInfo* DupLineInfo (const LineInfo* LI);
/* Creates a duplicate of a line info structure */

void ReadLineInfoList (FILE* F, struct ObjData* O, Collection* LineInfos);
/* Read a list of line infos stored as a list of indices in the object file,
** make real line infos from them and place them into the passed collection.
*/

const LineInfo* GetAsmLineInfo (const Collection* LineInfos);
/* Find a line info of type LI_TYPE_ASM and count zero in the given collection
** and return it. Return NULL if no such line info was found.
*/

#if defined(HAVE_INLINE)
INLINE const FilePos* GetSourcePos (const LineInfo* LI)
/* Return the source file position from the given line info */
{
    return &LI->Pos;
}
#else
#  define GetSourcePos(LI)      (&(LI)->Pos)
#endif

#if defined(HAVE_INLINE)
INLINE const char* GetSourceName (const LineInfo* LI)
/* Return the name of a source file from the given line info */
{
    return GetString (LI->Pos.Name);
}
#else
#  define GetSourceName(LI)     (GetString ((LI)->Pos.Name))
#endif

#if defined(HAVE_INLINE)
INLINE unsigned GetSourceLine (const LineInfo* LI)
/* Return the source file line from the given line info */
{
    return LI->Pos.Line;
}
#else
#  define GetSourceLine(LI)     ((LI)->Pos.Line)
#endif

#if defined(HAVE_INLINE)
INLINE unsigned GetSourceCol (const LineInfo* LI)
/* Return the source file column from the given line info */
{
    return LI->Pos.Col;
}
#else
#  define GetSourceCol(LI)      ((LI)->Pos.Col)
#endif

#if defined(HAVE_INLINE)
INLINE const char* GetSourceNameFromList (const Collection* LineInfos)
/* Return the name of a source file from a list of line infos */
{
    /* The relevant entry is in slot zero */
    return GetSourceName (CollConstAt (LineInfos, 0));
}
#else
#  define GetSourceNameFromList(LineInfos)      \
        GetSourceName ((const LineInfo*) CollConstAt ((LineInfos), 0))
#endif

#if defined(HAVE_INLINE)
INLINE unsigned GetSourceLineFromList (const Collection* LineInfos)
/* Return the source file line from a list of line infos */
{
    /* The relevant entry is in slot zero */
    return GetSourceLine (CollConstAt (LineInfos, 0));
}
#else
#  define GetSourceLineFromList(LineInfos)      \
        GetSourceLine ((const LineInfo*) CollConstAt ((LineInfos), 0))
#endif

unsigned LineInfoCount (void);
/* Return the total number of line infos */

void AssignLineInfoIds (void);
/* Assign the ids to the line infos */

void PrintDbgLineInfo (FILE* F);
/* Output the line infos to a debug info file */



/* End of lineinfo.h */

#endif
