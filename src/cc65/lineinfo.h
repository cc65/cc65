/*****************************************************************************/
/*                                                                           */
/*                                lineinfo.h                                 */
/*                                                                           */
/*                      Source file line info structure                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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
#include "strbuf.h"



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



/* Input file structure */
struct IFile;



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Struct that describes an input file for line info */
typedef struct LineInfoFile LineInfoFile;
struct LineInfoFile {
    struct IFile*       InputFile;      /* Points to corresponding IFile */
    unsigned            LineNum;        /* Presumed line number for this file */
    char                Name[1];        /* Presumed name of the file */
};

/* The text for the actual line is allocated at the end of the structure, so
** the size of the structure varies.
*/
typedef struct LineInfo LineInfo;
struct LineInfo {
    unsigned            RefCount;       /* Reference counter */
    LineInfoFile*       File;           /* Presumed input files for this line */
    unsigned            ActualLineNum;  /* Actual line number for this file */
    struct Collection*  IncFiles;       /* Presumed inclusion input files */
    char                Line[1];        /* Text of source code line */
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



LineInfo* UseLineInfo (LineInfo* LI);
/* Increase the reference count of the given line info and return it. */

void ReleaseLineInfo (LineInfo* LI);
/* Release a reference to the given line info, free the structure if the
** reference count drops to zero.
*/

LineInfo* GetCurLineInfo (void);
/* Return a pointer to the current line info. The reference count is NOT
** increased, use UseLineInfo for that purpose.
*/

void UpdateCurrentLineInfo (const StrBuf* Line);
/* Update the current line info - called if a new line is read */

void RememberCheckedLI (struct LineInfo* LI);
/* Remember the latest checked line info struct */

LineInfo* GetPrevCheckedLI (void);
/* Get the latest checked line info struct */

const char* GetPresumedFileName (const LineInfo* LI);
/* Return the presumed file name from a line info */

unsigned GetPresumedLineNum (const LineInfo* LI);
/* Return the presumed line number from a line info */

const char* GetActualFileName (const struct LineInfo* LI);
/* Return the actual name of the source file from a line info struct */

unsigned GetActualLineNum (const struct LineInfo* LI);
/* Return the actual line number of the source file from a line info struct */



/* End of lineinfo.h */

#endif
