/*****************************************************************************/
/*                                                                           */
/*				    input.h				     */
/*                                                                           */
/*			      Input file handling			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



/*****************************************************************************/
/*	 			     data				     */
/*****************************************************************************/



/* Maximum length of an input line and the corresponding char array */
#define LINEMAX		4095
#define LINESIZE 	LINEMAX+1

/* Input line stuff */
extern char* line;
extern const char* lptr;		/* ### Remove this */

/* Current and next input character */
extern char CurC;
extern char NextC;

/* Struct that describes an input file */
typedef struct IFile IFile;
struct IFile {
    unsigned	    Index;     	/* File index */
    unsigned	    Usage;	/* Usage counter */
    unsigned long   Size;       /* File size */
    unsigned long   MTime;      /* Time of last modification */
    char       	    Name[1];  	/* Name of file (dynamically allocated) */
};



/*****************************************************************************/
/*	       	     	     	     Code		     		     */
/*****************************************************************************/



void OpenMainFile (const char* Name);
/* Open the main file. Will call Fatal() in case of failures. */

void OpenIncludeFile (const char* Name, unsigned DirSpec);
/* Open an include file and insert it into the tables. */

void ClearLine (void);
/* Clear the current input line */
							     
void InitLine (const char* Buf);
/* Initialize lptr from Buf and read CurC and NextC from the new input line */

void NextChar (void);
/* Read the next character from the input stream and make CurC and NextC
 * valid. If end of line is reached, both are set to NUL, no more lines
 * are read by this function.
 */

int NextLine (void);
/* Get a line from the current input. Returns 0 on end of file. */

const char* GetCurrentFile (void);
/* Return the name of the current input file */

unsigned GetCurrentLine (void);
/* Return the line number in the current input file */

void WriteDependencies (FILE* F, const char* OutputFile);
/* Write a makefile dependency list to the given file */



/* End of input.h */
#endif



