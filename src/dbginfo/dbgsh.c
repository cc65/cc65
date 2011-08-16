/*****************************************************************************/
/*                                                                           */
/*                                  dbgsh.c                                  */
/*                                                                           */
/*                           debug info test shell                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2011,      Ullrich von Bassewitz                                      */
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
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>

/* common */
#include "attrib.h"
#include "chartype.h"
#include "coll.h"

/* dbginfo */
#include "dbginfo.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Terminate flag - end program when set to true */
static int Terminate = 0;

/* The debug file data */
static cc65_dbginfo     Info = 0;

/* Error and warning counters */
static unsigned FileErrors   = 0;
static unsigned FileWarnings = 0;

/* Structure that contains a command description */
typedef struct CmdEntry CmdEntry;
struct CmdEntry {
    char    Cmd[12];
    int     ArgCount;
    void    (*Func) (Collection*);
};



/*****************************************************************************/
/*                            Debug file handling                            */
/*****************************************************************************/



static void CloseFile (void)
/* Close the debug info file */
{
    if (Info) {
        cc65_free_dbginfo (Info);
        Info = 0;
    }
}



static int FileIsOpen (void)
/* Return true if the file is open and has loaded without errors: If not,
 * print an error message and return false.
 */
{
    /* File open? */
    if (Info == 0) {
        printf ("No debug info file\n");
        return 0;
    }

    /* Errors on load? */
    if (FileErrors > 0) {
        printf ("File had load errors!\n");
        return 0;
    }

    /* Warnings on load? */
    if (FileWarnings > 0) {
        printf ("Beware - file had load warnings!\n");
    }

    /* Ok */
    return 1;
}



/*****************************************************************************/
/*                                  Helpers                                  */
/*****************************************************************************/



static void FileError (const cc65_parseerror* Info)
/* Callback function - is called in case of errors */
{
    /* Output a message */
    printf ("%s:%s(%lu): %s\n",
            Info->type? "Error" : "Warning",
            Info->name,
            (unsigned long) Info->line,
            Info->errormsg);

    /* Bump the counters */
    switch (Info->type) {
        case CC65_WARNING:      ++FileWarnings;         break;
        default:                ++FileErrors;           break;
    }
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void CmdOpen (Collection* Args)
/* Open a debug info file */
{
    /* Argument is file name */
    if (CollCount (Args) != 2) {
        printf ("Command requires exactly one argument\n");
        return;
    }

    /* Close an open file */
    CloseFile ();

    /* Clear the counters */
    FileErrors   = 0;
    FileWarnings = 0;

    /* Open the debug info file */
    Info = cc65_read_dbginfo (CollAt (Args, 1), FileError);
}



static void CmdClose (Collection* Args attribute ((unused)))
/* Close a debug info file */
{
    CloseFile ();
}



static void CmdQuit (Collection* Args attribute ((unused)))
/* Terminate the application */
{
    Terminate = 1;
}



static int Parse (char* CmdLine, Collection* Args)
/* Parse the command line and store the arguments in Args. Return true if ok,
 * false on error.
 */
{
    char* End;

    /* Clear the collection */
    CollDeleteAll (Args);

    /* Parse the command line */
    while (1) {

        /* Break out on end of line */
        if (*CmdLine == '\0') {
            break;
        }

        /* Search for start of next command */
        if (IsSpace (*CmdLine)) {
            ++CmdLine;
            continue;
        }

        /* Allow double quotes to terminate a command */
        if (*CmdLine == '\"' || *CmdLine == '\'') {
            char Term = *CmdLine++;
            End = CmdLine;
            while (*End != Term) {
                if (*End == '\0') {
                    fputs ("Unterminated argument\n", stdout);
                    return 0;
                }
                ++End;
            }
            *End++ = '\0';
        } else {
            End = CmdLine;
            while (!IsSpace (*End)) {
                if (*End == '\0') {
                    fputs ("Unterminated argument\n", stdout);
                    return 0;
                }
                ++End;
            }
            *End++ = '\0';
        }
        CollAppend (Args, CmdLine);
        CmdLine = End;
    }

    /* Ok */
    return 1;
}



static const CmdEntry* FindCmd (const char* Cmd)
/* Search for a command */
{
    static const CmdEntry CmdTab[] = {
        { "close",      0,      CmdClose        },
        { "open",       1,      CmdOpen         },
        { "quit",       0,      CmdQuit         },
    };

    unsigned I;
    for (I = 0; I < sizeof (CmdTab) / sizeof (CmdTab[0]); ++I) {
        if (strcmp (Cmd, CmdTab[I].Cmd) == 0) {
            return CmdTab + I;
        }
    }
    return 0;
}



int main (void)
/* Main program */
{
    char Input[256];
    Collection Args = STATIC_COLLECTION_INITIALIZER;

    const char* Cmd;
    const CmdEntry* E;
    while (!Terminate) {

        /* Output a prompt, then read the input */
        fputs ("dbgsh> ", stdout);
        fflush (stdout);
        if (fgets (Input, sizeof (Input), stdin) == 0) {
            fputs ("(EOF)\n", stdout);
            break;
        }

        /* Parse the command line */
        if (Parse (Input, &Args) == 0 || CollCount (&Args) == 0) {
            continue;
        }

        /* Search for the command, then execute it */
        Cmd = CollAt (&Args, 0);
        E = FindCmd (Cmd);
        if (E == 0) {
            printf ("No such command: %s\n", Cmd);
        } else {
            E->Func (&Args);
        }
    }

    /* Free arguments */
    DoneCollection (&Args);
    return 0;
}



