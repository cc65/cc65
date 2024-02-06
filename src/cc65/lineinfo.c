/*****************************************************************************/
/*                                                                           */
/*                                lineinfo.c                                 */
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



#include <string.h>

/* common */
#include "chartype.h"
#include "check.h"
#include "xmalloc.h"

/* cc65 */
#include "global.h"
#include "input.h"
#include "lineinfo.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Global pointer to line information for the current line */
static LineInfo* CurLineInfo = 0;

/* Global pointer to previously checked line information about file inclusion hierarchy */
static LineInfo* PrevCheckedLI = 0;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static LineInfo* NewLineInfo (const StrBuf* Line)
/* Create and return a new line info. Ref count will be 1. */
{
    unsigned    Len;
    LineInfo*   LI;
    const char* S;
    char*       T;

    /* Get the length of the line and a pointer to the line buffer */
    Len = SB_GetLen (Line);
    S   = SB_GetConstBuf (Line);

    /* Skip leading spaces in Line */
    while (Len > 0 && IsBlank (*S)) {
        ++S;
        --Len;
    }

    /* Allocate memory for the line info and the input line */
    LI = xmalloc (sizeof (LineInfo) + Len);

    /* Initialize the fields */
    LI->RefCount  = 1;
    LI->File      = 0;
    LI->IncFiles  = 0;
    GetFileInclusionInfo (LI);

    /* Copy the line, replacing tabs by spaces in the given line since tabs
    ** will give rather arbitrary results when used in the output later, and
    ** if we do it here, we won't need another copy later.
    */
    T = LI->Line;
    while (Len--) {
        if (*S == '\t') {
            *T = ' ';
        } else {
            *T = *S;
        }
        ++S;
        ++T;
    }

    /* Add the terminator */
    *T = '\0';

    /* Return the new struct */
    return LI;
}



static void FreeLineInfo (LineInfo* LI)
/* Free a LineInfo structure */
{
    FreeFileInclusionInfo (LI);
    xfree (LI);
}



LineInfo* UseLineInfo (LineInfo* LI)
/* Increase the reference count of the given line info and return it. */
{
    CHECK (LI != 0);
    ++LI->RefCount;
    return LI;
}



void ReleaseLineInfo (LineInfo* LI)
/* Release a reference to the given line info, free the structure if the
** reference count drops to zero.
*/
{
    CHECK (LI && LI->RefCount > 0);
    if (--LI->RefCount == 0) {
        /* No more references, free it */
        FreeLineInfo (LI);
    }
}



LineInfo* GetCurLineInfo (void)
/* Return a pointer to the current line info. The reference count is NOT
** increased, use UseLineInfo for that purpose.
*/
{
    return CurLineInfo;
}



void UpdateCurrentLineInfo (const StrBuf* Line)
/* Update the current line info - called if a new line is read */
{
    /* If a current line info exists, release it */
    if (CurLineInfo) {
        ReleaseLineInfo (CurLineInfo);
    }

    /* If we have intermixed assembly switched off, use an empty line instead
    ** of the supplied one to save some memory.
    */
    if (!AddSource) {
        Line = &EmptyStrBuf;
    }

    /* Create a new line info */
    CurLineInfo = NewLineInfo (Line);
}



void RememberCheckedLI (LineInfo* LI)
/* Remember the latest checked line info struct */
{
    if (PrevCheckedLI != LI) {
        if (PrevCheckedLI != 0) {
            ReleaseLineInfo (PrevCheckedLI);
        }
        PrevCheckedLI = UseLineInfo (LI);
    }
}



LineInfo* GetPrevCheckedLI (void)
/* Get the latest checked line info struct */
{
    return PrevCheckedLI;
}



const char* GetPresumedFileName (const LineInfo* LI)
/* Return the presumed file name from a line info */
{
    PRECONDITION (LI != 0);
    return LI->File->Name;
}



unsigned GetPresumedLineNum (const LineInfo* LI)
/* Return the presumed line number from a line info */
{
    PRECONDITION (LI != 0);
    return LI->File->LineNum;
}



const char* GetActualFileName (const struct LineInfo* LI)
/* Return the actual name of the source file from a line info struct */
{
    return LI->File != 0 ? GetInputFileName (LI->File->InputFile) : "<out of filescope>";
}



unsigned GetActualLineNum (const struct LineInfo* LI)
/* Return the actual line number of the source file from a line info struct */
{
    return LI->ActualLineNum;
}
