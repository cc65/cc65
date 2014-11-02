/*****************************************************************************/
/*                                                                           */
/*                                  input.c                                  */
/*                                                                           */
/*                            Input file handling                            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2012, Ullrich von Bassewitz                                      */
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



#include <stdio.h>
#include <string.h>
#include <errno.h>

/* common */
#include "check.h"
#include "coll.h"
#include "filestat.h"
#include "fname.h"
#include "print.h"
#include "strbuf.h"
#include "xmalloc.h"

/* cc65 */
#include "codegen.h"
#include "error.h"
#include "global.h"
#include "incpath.h"
#include "input.h"
#include "lineinfo.h"
#include "output.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* The current input line */
StrBuf* Line;

/* Current and next input character */
char CurC  = '\0';
char NextC = '\0';

/* Maximum count of nested includes */
#define MAX_INC_NESTING         16

/* Struct that describes an input file */
typedef struct IFile IFile;
struct IFile {
    unsigned        Index;      /* File index */
    unsigned        Usage;      /* Usage counter */
    unsigned long   Size;       /* File size */
    unsigned long   MTime;      /* Time of last modification */
    InputType       Type;       /* Type of input file */
    char            Name[1];    /* Name of file (dynamically allocated) */
};

/* Struct that describes an active input file */
typedef struct AFile AFile;
struct AFile {
    unsigned    Line;           /* Line number for this file */
    FILE*       F;              /* Input file stream */
    IFile*      Input;          /* Points to corresponding IFile */
    int         SearchPath;     /* True if we've added a path for this file */
};

/* List of all input files */
static Collection IFiles = STATIC_COLLECTION_INITIALIZER;

/* List of all active files */
static Collection AFiles = STATIC_COLLECTION_INITIALIZER;

/* Input stack used when preprocessing. */
static Collection InputStack = STATIC_COLLECTION_INITIALIZER;



/*****************************************************************************/
/*                               struct IFile                                */
/*****************************************************************************/



static IFile* NewIFile (const char* Name, InputType Type)
/* Create and return a new IFile */
{
    /* Get the length of the name */
    unsigned Len = strlen (Name);

    /* Allocate a IFile structure */
    IFile* IF = (IFile*) xmalloc (sizeof (IFile) + Len);

    /* Initialize the fields */
    IF->Index = CollCount (&IFiles) + 1;
    IF->Usage = 0;
    IF->Size  = 0;
    IF->MTime = 0;
    IF->Type  = Type;
    memcpy (IF->Name, Name, Len+1);

    /* Insert the new structure into the IFile collection */
    CollAppend (&IFiles, IF);

    /* Return the new struct */
    return IF;
}



/*****************************************************************************/
/*                               struct AFile                                */
/*****************************************************************************/



static AFile* NewAFile (IFile* IF, FILE* F)
/* Create a new AFile, push it onto the stack, add the path of the file to
** the path search list, and finally return a pointer to the new AFile struct.
*/
{
    StrBuf Path = AUTO_STRBUF_INITIALIZER;

    /* Allocate a AFile structure */
    AFile* AF = (AFile*) xmalloc (sizeof (AFile));

    /* Initialize the fields */
    AF->Line  = 0;
    AF->F     = F;
    AF->Input = IF;

    /* Increment the usage counter of the corresponding IFile. If this
    ** is the first use, set the file data and output debug info if
    ** requested.
    */
    if (IF->Usage++ == 0) {

        /* Get file size and modification time. There a race condition here,
        ** since we cannot use fileno() (non standard identifier in standard
        ** header file), and therefore not fstat. When using stat with the
        ** file name, there's a risk that the file was deleted and recreated
        ** while it was open. Since mtime and size are only used to check
        ** if a file has changed in the debugger, we will ignore this problem
        ** here.
        */
        struct stat Buf;
        if (FileStat (IF->Name, &Buf) != 0) {
            /* Error */
            Fatal ("Cannot stat `%s': %s", IF->Name, strerror (errno));
        }
        IF->Size  = (unsigned long) Buf.st_size;
        IF->MTime = (unsigned long) Buf.st_mtime;

        /* Set the debug data */
        g_fileinfo (IF->Name, IF->Size, IF->MTime);
    }

    /* Insert the new structure into the AFile collection */
    CollAppend (&AFiles, AF);

    /* Get the path of this file and add it as an extra search path.
    ** To avoid file search overhead, we will add one path only once.
    ** This is checked by the PushSearchPath function.
    */
    SB_CopyBuf (&Path, IF->Name, FindName (IF->Name) - IF->Name);
    SB_Terminate (&Path);
    AF->SearchPath = PushSearchPath (UsrIncSearchPath, SB_GetConstBuf (&Path));
    SB_Done (&Path);

    /* Return the new struct */
    return AF;
}



static void FreeAFile (AFile* AF)
/* Free an AFile structure */
{
    xfree (AF);
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static IFile* FindFile (const char* Name)
/* Find the file with the given name in the list of all files. Since the list
** is not large (usually less than 10), I don't care about using hashes or
** similar things and do a linear search.
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
    AFile* MainFile;


    /* Setup a new IFile structure for the main file */
    IFile* IF = NewIFile (Name, IT_MAIN);

    /* Open the file for reading */
    FILE* F = fopen (Name, "r");
    if (F == 0) {
        /* Cannot open */
        Fatal ("Cannot open input file `%s': %s", Name, strerror (errno));
    }

    /* Allocate a new AFile structure for the file */
    MainFile = NewAFile (IF, F);

    /* Allocate the input line buffer */
    Line = NewStrBuf ();

    /* Update the line infos, so we have a valid line info even at start of
    ** the main file before the first line is read.
    */
    UpdateLineInfo (MainFile->Input, MainFile->Line, Line);
}



void OpenIncludeFile (const char* Name, InputType IT)
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
    N = SearchFile ((IT == IT_SYSINC)? SysIncSearchPath : UsrIncSearchPath, Name);
    if (N == 0) {
        PPError ("Include file `%s' not found", Name);
        return;
    }

    /* Search the list of all input files for this file. If we don't find
    ** it, create a new IFile object.
    */
    IF = FindFile (N);
    if (IF == 0) {
        IF = NewIFile (N, IT);
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

    /* Debugging output */
    Print (stdout, 1, "Opened include file `%s'\n", IF->Name);

    /* Allocate a new AFile structure */
    (void) NewAFile (IF, F);
}



static void CloseIncludeFile (void)
/* Close an include file and switch to the higher level file. Set Input to
** NULL if this was the main file.
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

    /* If we had added an extra search path for this AFile, remove it */
    if (Input->SearchPath) {
        PopSearchPath (UsrIncSearchPath);
    }

    /* Delete the active file structure */
    FreeAFile (Input);
}



static void GetInputChar (void)
/* Read the next character from the input stream and make CurC and NextC
** valid. If end of line is reached, both are set to NUL, no more lines
** are read by this function.
*/
{
    /* Drop all pushed fragments that don't have data left */
    while (SB_GetIndex (Line) >= SB_GetLen (Line)) {
        /* Cannot read more from this line, check next line on stack if any */
        if (CollCount (&InputStack) == 0) {
            /* This is THE line */
            break;
        }
        FreeStrBuf (Line);
        Line = CollPop (&InputStack);
    }

    /* Now get the next characters from the line */
    if (SB_GetIndex (Line) >= SB_GetLen (Line)) {
        CurC = NextC = '\0';
    } else {
        CurC = SB_AtUnchecked (Line, SB_GetIndex (Line));
        if (SB_GetIndex (Line) + 1 < SB_GetLen (Line)) {
            /* NextC comes from this fragment */
            NextC = SB_AtUnchecked (Line, SB_GetIndex (Line) + 1);
        } else {
            /* NextC comes from next fragment */
            if (CollCount (&InputStack) > 0) {
                NextC = ' ';
            } else {
                NextC = '\0';
            }
        }
    }
}



void NextChar (void)
/* Skip the current input character and read the next one from the input
** stream. CurC and NextC are valid after the call. If end of line is
** reached, both are set to NUL, no more lines are read by this function.
*/
{
    /* Skip the last character read */
    SB_Skip (Line);

    /* Read the next one */
    GetInputChar ();
}



void ClearLine (void)
/* Clear the current input line */
{
    unsigned I;

    /* Remove all pushed fragments from the input stack */
    for (I = 0; I < CollCount (&InputStack); ++I) {
        FreeStrBuf (CollAtUnchecked (&InputStack, I));
    }
    CollDeleteAll (&InputStack);

    /* Clear the contents of Line */
    SB_Clear (Line);
    CurC    = '\0';
    NextC   = '\0';
}



StrBuf* InitLine (StrBuf* Buf)
/* Initialize Line from Buf and read CurC and NextC from the new input line.
** The function returns the old input line.
*/
{
    StrBuf* OldLine = Line;
    Line  = Buf;
    CurC  = SB_LookAt (Buf, SB_GetIndex (Buf));
    NextC = SB_LookAt (Buf, SB_GetIndex (Buf) + 1);
    return OldLine;
}



int NextLine (void)
/* Get a line from the current input. Returns 0 on end of file. */
{
    AFile*      Input;

    /* Clear the current line */
    ClearLine ();

    /* If there is no file open, bail out, otherwise get the current input file */
    if (CollCount (&AFiles) == 0) {
        return 0;
    }
    Input = CollLast (&AFiles);

    /* Read characters until we have one complete line */
    while (1) {

        /* Read the next character */
        int C = fgetc (Input->F);

        /* Check for EOF */
        if (C == EOF) {

            /* Accept files without a newline at the end */
            if (SB_NotEmpty (Line)) {
                ++Input->Line;
                break;
            }

            /* Leave the current file */
            CloseIncludeFile ();

            /* If there is no file open, bail out, otherwise get the
            ** previous input file and start over.
            */
            if (CollCount (&AFiles) == 0) {
                return 0;
            }
            Input = CollLast (&AFiles);
            continue;
        }

        /* Check for end of line */
        if (C == '\n') {

            /* We got a new line */
            ++Input->Line;

            /* If the \n is preceeded by a \r, remove the \r, so we can read
            ** DOS/Windows files under *nix.
            */
            if (SB_LookAtLast (Line) == '\r') {
                SB_Drop (Line, 1);
            }

            /* If we don't have a line continuation character at the end,
            ** we're done with this line. Otherwise replace the character
            ** by a newline and continue reading.
            */
            if (SB_LookAtLast (Line) == '\\') {
                Line->Buf[Line->Len-1] = '\n';
            } else {
                break;
            }

        } else if (C != '\0') {         /* Ignore embedded NULs */

            /* Just some character, add it to the line */
            SB_AppendChar (Line, C);

        }
    }

    /* Add a termination character to the string buffer */
    SB_Terminate (Line);

    /* Initialize the current and next characters. */
    InitLine (Line);

    /* Create line information for this line */
    UpdateLineInfo (Input->Input, Input->Line, Line);

    /* Done */
    return 1;
}



const char* GetInputFile (const struct IFile* IF)
/* Return a filename from an IFile struct */
{
    return IF->Name;
}



const char* GetCurrentFile (void)
/* Return the name of the current input file */
{
    unsigned AFileCount = CollCount (&AFiles);
    if (AFileCount > 0) {
        const AFile* AF = (const AFile*) CollAt (&AFiles, AFileCount-1);
        return AF->Input->Name;
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



static void WriteEscaped (FILE* F, const char* Name)
/* Write a file name to a dependency file escaping spaces */
{
    while (*Name) {
        if (*Name == ' ') {
            /* Escape spaces */
            fputc ('\\', F);
        }
        fputc (*Name, F);
        ++Name;
    }
}



static void WriteDep (FILE* F, InputType Types)
/* Helper function. Writes all file names that match Types to the output */
{
    unsigned I;

    /* Loop over all files */
    unsigned FileCount = CollCount (&IFiles);
    for (I = 0; I < FileCount; ++I) {

        /* Get the next input file */
        const IFile* IF = (const IFile*) CollAt (&IFiles, I);

        /* Ignore it if it is not of the correct type */
        if ((IF->Type & Types) == 0) {
            continue;
        }

        /* If this is not the first file, add a space */
        if (I > 0) {
            fputc (' ', F);
        }

        /* Print the dependency escaping spaces */
        WriteEscaped (F, IF->Name);
    }
}



static void CreateDepFile (const char* Name, InputType Types)
/* Create a dependency file with the given name and place dependencies for
** all files with the given types there.
*/
{
    /* Open the file */
    FILE* F = fopen (Name, "w");
    if (F == 0) {
        Fatal ("Cannot open dependency file `%s': %s", Name, strerror (errno));
    }

    /* If a dependency target was given, use it, otherwise use the output
    ** file name as target, followed by a tab character.
    */
    if (SB_IsEmpty (&DepTarget)) {
        WriteEscaped (F, OutputFilename);
    } else {
        WriteEscaped (F, SB_GetConstBuf (&DepTarget));
    }
    fputs (":\t", F);

    /* Write out the dependencies for the output file */
    WriteDep (F, Types);
    fputs ("\n\n", F);

    /* Write out a phony dependency for the included files */
    WriteDep (F, Types);
    fputs (":\n\n", F);

    /* Close the file, check for errors */
    if (fclose (F) != 0) {
        remove (Name);
        Fatal ("Cannot write to dependeny file (disk full?)");
    }
}



void CreateDependencies (void)
/* Create dependency files requested by the user */
{
    if (SB_NotEmpty (&DepName)) {
        CreateDepFile (SB_GetConstBuf (&DepName),
                       IT_MAIN | IT_USRINC);
    }
    if (SB_NotEmpty (&FullDepName)) {
        CreateDepFile (SB_GetConstBuf (&FullDepName),
                       IT_MAIN | IT_SYSINC | IT_USRINC);
    }
}
