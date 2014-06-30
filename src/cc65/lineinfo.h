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



/* The text for the actual line is allocated at the end of the structure, so
** the size of the structure varies.
*/
typedef struct LineInfo LineInfo;
struct LineInfo {
    unsigned        RefCount;             /* Reference counter */
    struct IFile*   InputFile;            /* Input file for this line */
    unsigned        LineNum;              /* Line number */
    char            Line[1];              /* Source code line */
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

void UpdateLineInfo (struct IFile* F, unsigned LineNum, const StrBuf* Line);
/* Update the line info - called if a new line is read */

const char* GetInputName (const LineInfo* LI);
/* Return the file name from a line info */

unsigned GetInputLine (const LineInfo* LI);
/* Return the line number from a line info */



/* End of lineinfo.h */

#endif
