/*****************************************************************************/
/*                                                                           */
/*				    input.c				     */
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



#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "../common/xmalloc.h"

#include "asmcode.h"
#include "check.h"
#include "error.h"
#include "global.h"
#include "incpath.h"
#include "input.h"



/*****************************************************************************/
/*	       	      	     	     Data		     		     */
/*****************************************************************************/



/* Input line stuff */
static char LineBuf [LINESIZE];
char* line = LineBuf;
const char* lptr = LineBuf;

/* Current and next input character */
char CurC  = '\0';
char NextC = '\0';

/* Maximum count of nested includes */
#define MAX_INC_NESTING 	16

/* Struct that describes an input file */
typedef struct IFile IFile;
struct IFile {
    IFile*	Next; 	 	/* Next file in single linked list 	*/
    IFile*	Active;		/* Next file in list of active includes */
    unsigned	Index;	 	/* File index 				*/
    unsigned	Line; 	 	/* Line number for this file 		*/
    FILE*	F;    	 	/* Input file stream 			*/
    char       	Name[1]; 	/* Name of file (dynamically allocated) */
};

/* Main file input data */
static const IFile* MainFile = 0;

/* List of input files */
static unsigned IFileTotal = 0;	/* Total number of files 		*/
static IFile*  	IFileList  = 0;	/* Single linked list of all files 	*/
static unsigned IFileCount = 0; /* Number of active input files 	*/
static IFile*   Input 	   = 0; /* Single linked list of active files	*/



/*****************************************************************************/
/*	       	      		 struct IFile				     */
/*****************************************************************************/



static IFile* NewIFile (const char* Name, FILE* F)
/* Create and return a new IFile */
{
    /* Get the length of the name */
    unsigned Len = strlen (Name);

    /* Allocate a IFile structure */
    IFile* IF = xmalloc (sizeof (IFile) + Len);

    /* Initialize the fields */
    IF->Index	= ++IFileTotal;
    IF->Line	= 0;
    IF->F	= F;
    memcpy (IF->Name, Name, Len+1);

    /* Insert the structure into both lists */
    IF->Next 	= IFileList;
    IFileList	= IF;
    IF->Active	= Input;
    Input	= IF;
    ++IFileCount;
    ++IFileTotal;

    /* Return the new struct */
    return IF;
}



/*****************************************************************************/
/*	       	     	     	     Code		     		     */
/*****************************************************************************/



void OpenMainFile (const char* Name)
/* Open the main file. Will call Fatal() in case of failures. */
{
    /* Open the file for reading */
    FILE* F = fopen (Name, "r");
    if (F == 0) {
	/* Cannot open */
	Fatal (FAT_CANNOT_OPEN_INPUT, strerror (errno));
    }

    /* Setup a new IFile structure */
    MainFile = NewIFile (Name, F);
}



void OpenIncludeFile (const char* Name, unsigned DirSpec)
/* Open an include file and insert it into the tables. */
{
    char* N;
    FILE* F;

    /* Check for the maximum include nesting */
    if (IFileCount > MAX_INC_NESTING) {
     	PPError (ERR_INCLUDE_NESTING);
      	return;
    }

    /* Search for the file */
    N = FindInclude (Name, DirSpec);
    if (N == 0) {
	PPError (ERR_INCLUDE_NOT_FOUND, Name);
     	return;
    }

    /* Open the file */
    F = fopen (N, "r");
    if (F == 0) {
	/* Error opening the file */
	PPError (ERR_INCLUDE_OPEN_FAILURE, N, strerror (errno));
	xfree (N);
	return;
    }

    /* Allocate a new IFile structure */
    NewIFile (N, F);

    /* We don't need the full name any longer */
    xfree (N);
}



static void CloseIncludeFile (void)
/* Close an include file and switch to the higher level file. Set Input to
 * NULL if this was the main file.
 */
{
    /* Must have an input file when called */
    PRECONDITION (Input != 0);

    /* Close the current input file (we're just reading so no error check) */
    fclose (Input->F);

    /* Make this file inactive and the last one active again */
    Input = Input->Active;

    /* Adjust the counter */
    --IFileCount;
}



void ClearLine (void)
/* Clear the current input line */
{
    line[0] = '\0';
    lptr    = line;
    CurC    = '\0';
    NextC   = '\0';
}



void InitLine (const char* Buf)
/* Initialize lptr from Buf and read CurC and NextC from the new input line */
{
    lptr = Buf;
    CurC = lptr[0];
    if (CurC != '\0') {
	NextC = lptr[1];
    } else {
	NextC = '\0';
    }
}



void NextChar (void)
/* Read the next character from the input stream and make CurC and NextC
 * valid. If end of line is reached, both are set to NUL, no more lines
 * are read by this function.
 */
{
    if (lptr[0] != '\0') {
	++lptr;
       	CurC = lptr[0];
	if (CurC != '\0') {
	    NextC = lptr[1];
	} else {
	    NextC = '\0';
	}
    } else {
	CurC = NextC = '\0';
    }
}



int NextLine (void)
/* Get a line from the current input. Returns 0 on end of file. */
{
    unsigned   	Len;
    unsigned   	Part;
    unsigned   	Start;
    int	       	Done;

    /* Setup the line */
    ClearLine ();

    /* If there is no file open, bail out */
    if (Input == 0) {
	return 0;
    }

    /* Read lines until we get one with real contents */
    Len = 0;
    Done = 0;
    while (!Done && Len < LINESIZE) {

       	while (fgets (line + Len, LINESIZE - Len, Input->F) == 0) {

      	    /* Assume EOF */
      	    ClearLine ();

      	    /* Leave the current file */
      	    CloseIncludeFile ();

      	    /* If this was the last file, bail out */
	    if (Input == 0) {
	       	return 0;
	    }
       	}

	/* We got a new line */
	++Input->Line;

	/* Remove the trailing newline if we have one */
	Part = strlen (line + Len);
	Start = Len;
	Len += Part;
	while (Len > 0 && line [Len-1] == '\n') {
	    --Len;
	}
      	line [Len] = '\0';

	/* Output the source line in the generated assembler file
	 * if requested.
	 */
	if (AddSource && line[Start] != '\0') {
	    AddCodeLine ("; %s", line+Start);
	}

	/* Check if we have a line continuation character at the end. If not,
	 * we're done.
	 */
	if (Len > 0 && line[Len-1] == '\\') {
	    line[Len-1] = '\n';		/* Replace by newline */
      	} else {
      	    Done = 1;
      	}
    }

    /* Got a line. Initialize the current and next characters. */
    InitLine (line);

    /* Done */
    return 1;
}



const char* GetCurrentFile (void)
/* Return the name of the current input file */
{
    if (Input == 0) {
	if (MainFile) {
	    return MainFile->Name;
	} else {
      	    return "(outside file scope)";
	}
    } else {
      	return Input->Name;
    }
}



unsigned GetCurrentLine (void)
/* Return the line number in the current input file */
{
    return Input? Input->Line : 0;
}



