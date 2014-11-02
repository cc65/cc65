/*****************************************************************************/
/*                                                                           */
/*                                 textseg.h                                 */
/*                                                                           */
/*                          Text segment structure                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2009, Ullrich von Bassewitz                                      */
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



/* Note: This is NOT some sort of code segment, it is used to store lines of
** output that are textual (not real code) instead.
*/



#ifndef TEXTSEG_H
#define TEXTSEG_H



#include <stdarg.h>

/* common */
#include "attrib.h"
#include "coll.h"

/* cc65 */
#include "symentry.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



typedef struct TextSeg TextSeg;
struct TextSeg {
    SymEntry*           Func;           /* Owner function */
    Collection          Lines;          /* List of text lines */
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



TextSeg* NewTextSeg (SymEntry* Func);
/* Create a new text segment, initialize and return it */

void TS_AddVLine (TextSeg* S, const char* Format, va_list ap) attribute ((format(printf,2,0)));
/* Add a line to the given text segment */

void TS_AddLine (TextSeg* S, const char* Format, ...) attribute ((format(printf,2,3)));
/* Add a line to the given text segment */

void TS_Output (const TextSeg* S);
/* Output the text segment data to the output file */



/* End of textseg.h */

#endif
