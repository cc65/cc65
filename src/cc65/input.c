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

/* common */
#include "check.h"
#include "coll.h"
#include "xmalloc.h"

/* cc65 */
#include "asmcode.h"
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
    unsigned	Index;	 	/* File index 				*/
    unsigned	Usage;		/* Usage counter 		        */
    char       	Name[1]; 	/* Name of file (dynamically allocated) */
};

/* Struct that describes an active input file */
typedef struct AFile AFile;
struct AFile {
    unsigned	Line; 	 	/* Line number for this file 		*/
    FILE*   	F;    	 	/* Input file stream 			*/
    const char*	Name;		/* Points to corresponding IFile name	*/
};

/* List of all input files */
static Collection IFiles = STATIC_COLLECTION_INITIALIZER;

/* List of all active files */
static Collection AFiles = STATIC_COLLECTION_INITIALIZER;



/*****************************************************************************/
/*	       	      		 struct IFile				     */
/*****************************************************************************/



static IFile* NewIFile (const char* Name)
/* Create and return a new IFile */
{
    /* Get the length of the name */
    unsigned Len = strlen (Name);

    /* Allocate a IFile structure */
    IFile* IF = (IFile*) xmalloc (sizeof (IFile) + Len);

    /* Initialize the fields */
    IF->Index = CollCount (&IFiles) + 1;
    IF->Usage = 0;
    memcpy (IF->Name, Name, Len+1);

    /* Insert the new structure into the IFile collection */
    CollAppend (&IFiles, IF);

    /* Return the new struct */
    return IF;
}



/*****************************************************************************/
/*	       	      		 struct AFile				     */
/*****************************************************************************/



static AFile* NewAFile (IFile* IF, FILE* F)
/* Create and return a new AFile */
{
    /* Allocate a AFile structure */
    AFile* AF = (AFile*) xmalloc (sizeof (AFile));

    /* Initialize the fields */
    AF->Line  = 0;
    AF->F     = F;
    AF->Name  = IF->Name;

    /* Increment the usage counter of the corresponding IFile */
    ++IF->Usage;

    /* Insert the new structure into the AFile collection */
    CollAppend (&AFiles, AF);

    /* Return the new struct */
    return AF;
}



static void FreeAFile (AFile* AF)
/* Free an AFile structure */
{
    xfree (AF);
}



/*****************************************************************************/
/*	       	     	     	     Code		     		     */
/*****************************************************************************/



static IFile* FindFile (const char* Name)
/* Find the file with the given name in the list of all files. Since the list
 * is not large (usually less than 10), I don't care about using hashes or
 * similar things and do a linear search.
 */
{
    unsigned I;
    for (I = 0; I < CollCount (&IFiles); ++I) {
	/* Get the file struct */
	IFile* IF = (IFile*) CollAt (&IFiles, I);
	/* Check the name */
	if (strcmp (Name, IF->Name) == 0) {
	    /* Found, return the struct */
	    return IF;
	}
    }

    /* Not found */
    return 0;
}



void OpenMainFile (const char* Name)
/* Open the main file. Will call Fatal() in case of failures. */
{
    /* Setup a new IFile structure for the main file */
    IFile* IF = NewIFile (Name);

    /* Open the file for reading */
    FILE* F = fopen (Name, "r");
    if (F == 0) {
       	/* Cannot open */
       	Fatal ("Cannot open input file `%s': %s", Name, strerror (errno));
    }

    /* Allocate a new AFile structure for the file */
    (void) NewAFile (IF, F);
}



void OpenIncludeFile (const char* Name, unsigned DirSpec)
/* Open an include file and insert it into the tables. */
{
    char*  N;
    FILE*  F;
    IFile* IF;

    /* Check for the maximum include nesting */
    if (CollCount (&AFiles) > MAX_INC_NESTING) {
     	PPError ("Include nesting too deep");
      	return;
    }

    /* Search for the file */
    N = FindInclude (Name, DirSpec);
    if (N == 0) {
	PPError ("Include file `%s' not found", Name);
     	return;
    }

    /* Search the list of all input files for this file. If we don't find
     * it, create a new IFile object.
     */
    IF = FindFile (N);
    if (IF == 0) {
	IF = NewIFile (N);
    }

    /* We don't need N any longer, since we may now use IF->Name */
    xfree (N);

    /* Open the file */
    F = fopen (IF->Name, "r");
    if (F == 0) {
	/* Error opening the file */
	PPError ("Cannot open include file `%s': %s", IF->Name, strerror (errno));
	return;
    }

    /* Allocate a new AFile structure */
    (void) NewAFile (IF, F);
}



static void CloseIncludeFile (void)
/* Close an include file and switch to the higher level file. Set Input to
 * NULL if this was the main file.
 */
{
    AFile* Input;

    /* Get the number of active input files */
    unsigned AFileCount = CollCount (&AFiles);

    /* Must have an input file when called */
    PRECONDITION (AFileCount > 0);

    /* Get the current active input file */
    Input = (AFile*) CollLast (&AFiles);

    /* Close the current input file (we're just reading so no error check) */
    fclose (Input->F);

    /* Delete the last active file from the active file collection */
    CollDelete (&AFiles, AFileCount-1);

    /* Delete the active file structure */
    FreeAFile (Input);
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
    AFile*	Input;
    unsigned   	Len;
    unsigned   	Part;
    unsigned   	Start;
    int	       	Done;

    /* Setup the line */
    ClearLine ();

    /* If there is no file open, bail out, otherwise get the current input file */
    if (CollCount (&AFiles) == 0) {
	return 0;
    }
    Input = (AFile*) CollLast (&AFiles);

    /* Read lines until we get one with real contents */
    Len = 0;
    Done = 0;
    while (!Done && Len < LINESIZE) {

       	while (fgets (line + Len, LINESIZE - Len, Input->F) == 0) {

      	    /* Assume EOF */
      	    ClearLine ();

      	    /* Leave the current file */
      	    CloseIncludeFile ();

	    /* If there is no file open, bail out, otherwise get the
	     * current input file
	     */
	    if (CollCount (&AFiles) == 0) {
		return 0;
	    }
	    Input = (AFile*) CollLast (&AFiles);

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
	    
#if 0 
	/* ######### */
      	/* Output the source line in the generated assembler file
      	 * if requested.
      	 */
      	if (AddSource && line[Start] != '\0') {
      	    AddCodeLine ("; %s", line+Start);
      	}
#endif

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
    unsigned AFileCount = CollCount (&AFiles);
    if (AFileCount > 0) {
	const AFile* AF = (const AFile*) CollAt (&AFiles, AFileCount-1);
	return AF->Name;
    } else {
	/* No open file. Use the main file if we have one. */
	unsigned IFileCount = CollCount (&IFiles);
	if (IFileCount > 0) {
	    const IFile* IF = (const IFile*) CollAt (&IFiles, 0);
	    return IF->Name;
	} else {
      	    return "(outside file scope)";
	}
    }
}



unsigned GetCurrentLine (void)
/* Return the line number in the current input file */
{
    unsigned AFileCount = CollCount (&AFiles);
    if (AFileCount > 0) {
	const AFile* AF = (const AFile*) CollAt (&AFiles, AFileCount-1);
	return AF->Line;
    } else {
	/* No open file */
	return 0;
    }
}



void WriteDependencies (FILE* F, const char* OutputFile)
/* Write a makefile dependency list to the given file */
{
    unsigned I;

    /* Get the number of input files */
    unsigned IFileCount = CollCount (&IFiles);

    /* Print the output file followed by a tab char */
    fprintf (F, "%s:\t", OutputFile);

    /* Loop over all files */
    for (I = 0; I < IFileCount; ++I) {
	/* Get the next input file */
	const IFile* IF = (const IFile*) CollAt (&IFiles, I);
	/* If this is not the first file, add a space */
	const char* Format = (I == 0)? "%s" : " %s";
	/* Print the dependency */
	fprintf (F, Format, IF->Name);
    }

    /* End the line */
    fprintf (F, "\n\n");
}



