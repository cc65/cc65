/*****************************************************************************/
/*                                                                           */
/*                                 preproc.h                                 */
/*                                                                           */
/*                              C preprocessor                               */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2004 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
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



#ifndef PREPROC_H
#define PREPROC_H

#include "macrotab.h"

/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Maximum #if depth per file */
#define MAX_PP_IFS      256

/* Data struct used for per-file-directive handling */
typedef struct PPIfStack PPIfStack;
struct PPIfStack {
    unsigned char   Stack[MAX_PP_IFS];
    int             Index;
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void Preprocess (void);
/* Preprocess a line */

void SetPPIfStack (PPIfStack* Stack);
/* Specify which PP #if stack to use */

void ContinueLine (void);
/* Continue the current line ended with a '\\' */

void PreprocessBegin (void);
/* Initialize preprocessor with current file */

void PreprocessEnd (void);
/* Preprocessor done with current file */

void InitPreprocess (void);
/* Init preprocessor */

void DonePreprocess (void);
/* Done with preprocessor */

void HandleSpecialMacro (Macro* M, const char* Name);
/* Handle special "magic" macros that may change */



/* End of preproc.h */

#endif
