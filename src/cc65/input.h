/*****************************************************************************/
/*                                                                           */
/*                                  input.h                                  */
/*                                                                           */
/*                            Input file handling                            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2010, Ullrich von Bassewitz                                      */
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



#ifndef INPUT_H
#define INPUT_H



#include <stdio.h>

/* common */
#include "coll.h"
#include "strbuf.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Forwards */
struct IFile;
struct LineInfo;

/* An enum that describes different types of input files. The members are
** choosen so that it is possible to combine them to bitsets
*/
typedef enum {
    IT_MAIN   = 0x01,           /* Main input file */
    IT_SYSINC = 0x02,           /* System include file (using <>) */
    IT_USRINC = 0x04,           /* User include file (using "") */
} InputType;

/* The current input line */
extern StrBuf* Line;

/* Current and next input character */
extern char CurC;
extern char NextC;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void OpenMainFile (const char* Name);
/* Open the main file. Will call Fatal() in case of failures. */

void OpenIncludeFile (const char* Name, InputType IT);
/* Open an include file and insert it into the tables. */

void CloseIncludeFile (void);
/* Close an include file and switch to the higher level file. Set Input to
** NULL if this was the main file.
*/

void NextChar (void);
/* Read the next character from the input stream and make CurC and NextC
** valid. If end of line is reached, both are set to NUL, no more lines
** are read by this function.
*/

Collection* UseInputStack (Collection* InputStack);
/* Use the provided input stack for incoming input. Return the previously used
** InputStack.
*/

void PushLine (StrBuf* L);
/* Save the current input line and use a new one */

void ReuseInputLine (void);
/* Save and reuse the current line as the next line */

void ClearLine (void);
/* Clear the current input line */

StrBuf* InitLine (StrBuf* Buf);
/* Initialize Line from Buf and read CurC and NextC from the new input line.
** The function returns the old input line.
*/

int NextLine (void);
/* Get a line from the current input. Returns 0 on end of file with no new
** input bytes.
*/

int PreprocessNextLine (void);
/* Get a line from opened input files and do preprocess. Returns 0 on end of
** main file.
*/

void GetFileInclusionInfo (struct LineInfo* LI);
/* Get info about source file inclusion for LineInfo struct */

void FreeFileInclusionInfo (struct LineInfo* LI);
/* Free info about source file inclusion for LineInfo struct */

int HasFileInclusionChanged (const struct LineInfo* LI);
/* Return true if file inclusion has changed from last time */

const char* GetInputFileName (const struct IFile* IF);
/* Return the name of the file from an IFile struct */

const char* GetCurrentFileName (void);
/* Return the name of the current input file */

unsigned GetCurrentLineNum (void);
/* Return the line number in the current input file */

void SetCurrentLineNum (unsigned LineNum);
/* Set the line number in the current input file */

void SetCurrentFileName (const char* Name);
/* Set the presumed name of the current input file */

unsigned GetCurrentCounter (void);
/* Return the counter number in the current input file */

void CreateDependencies (void);
/* Create dependency files requested by the user */



/* End of input.h */

#endif
