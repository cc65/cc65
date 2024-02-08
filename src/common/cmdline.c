/*****************************************************************************/
/*                                                                           */
/*                                 cmdline.c                                 */
/*                                                                           */
/*                 Helper functions for command line parsing                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2009, Ullrich von Bassewitz                                      */
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
#include "abend.h"
#include "chartype.h"
#include "fname.h"
#include "xmalloc.h"
#include "cmdline.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Program name - is set after call to InitCmdLine */
const char* ProgName;

/* The program argument vector */
char** ArgVec     = 0;
unsigned ArgCount = 0;

/* Struct to pass the command line */
typedef struct {
    char**      Vec;            /* The argument vector */
    unsigned    Count;          /* Actual number of arguments */
    unsigned    Size;           /* Number of argument allocated */
} CmdLine;



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



static void NewCmdLine (CmdLine* L)
/* Initialize a CmdLine struct */
{
    /* Initialize the struct */
    L->Size    = 8;
    L->Count   = 0;
    L->Vec     = xmalloc (L->Size * sizeof (L->Vec[0]));
}



static void AddArg (CmdLine* L, char* Arg)
/* Add one argument to the list */
{
    if (L->Size <= L->Count) {
        /* No space left, reallocate */
        unsigned NewSize = L->Size * 2;
        char**   NewVec  = xmalloc (NewSize * sizeof (L->Vec[0]));
        memcpy (NewVec, L->Vec, L->Count * sizeof (L->Vec[0]));
        xfree (L->Vec);
        L->Vec  = NewVec;
        L->Size = NewSize;
    }

    /* We have space left, add a copy of the argument */
    L->Vec[L->Count++] = Arg;
}



static void ExpandFile (CmdLine* L, const char* Name)
/* Add the contents of a file to the command line. Each line is a separate
** argument with leading and trailing whitespace removed.
*/
{
    char Buf [256];

    /* Try to open the file for reading */
    FILE* F = fopen (Name, "r");
    if (F == 0) {
        AbEnd ("Cannot open \"%s\": %s", Name, strerror (errno));
    }

    /* File is open, read all lines */
    while (fgets (Buf, sizeof (Buf), F) != 0) {

        /* Get a pointer to the buffer */
        const char* B = Buf;

        /* Skip trailing whitespace (this will also kill the newline that is
        ** appended by fgets().
        */
        unsigned Len = strlen (Buf);
        while (Len > 0 && IsSpace (Buf [Len-1])) {
            --Len;
        }
        Buf [Len] = '\0';

        /* Skip leading spaces */
        while (IsSpace (*B)) {
            ++B;
        }

        /* Skip empty lines to work around problems with some editors */
        if (*B == '\0') {
            continue;
        }

        /* Add anything not empty to the command line */
        AddArg (L, xstrdup (B));

    }

    /* Close the file, ignore errors here since we had the file open for
    ** reading only.
    */
    (void) fclose (F);
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void InitCmdLine (int* aArgCount, char*** aArgVec, const char* aProgName)
/* Initialize command line parsing. aArgVec is the argument array terminated by
** a NULL pointer (as usual), ArgCount is the number of valid arguments in the
** array. Both arguments are remembered in static storage.
*/
{
    CmdLine     L;
    int         I;

    /* Get the program name from argv[0] but strip a path */
    if ((*aArgVec)[0] == 0) {
        /* Use the default name given */
        ProgName = aProgName;
    } else {
        /* Strip a path */
        ProgName = FindName ((*aArgVec)[0]);
        if (ProgName[0] == '\0') {
            /* Use the default */
            ProgName = aProgName;
        }
        else {
            /* remove .exe extension, if there is any
            **
            ** Note: This creates a new string that is
            ** never free()d.
            ** As this is exactly only string, and it
            ** lives for the whole lifetime of the tool,
            ** this is not an issue.
            */
            ProgName = MakeFilename (ProgName, "");
        }
    }

    /* Make a CmdLine struct */
    NewCmdLine (&L);

    /* Walk over the parameters and add them to the CmdLine struct. Add a
    ** special handling for arguments preceeded by the '@' sign - these are
    ** actually files containing arguments.
    */
    for (I = 0; I <= *aArgCount; ++I) {

        /* Get the next argument */
        char* Arg = (*aArgVec)[I];

        /* Is this a file argument? */
        if (Arg && Arg[0] == '@') {

            /* Expand the file */
            ExpandFile (&L, Arg+1);

        } else {

            /* No file, just add a copy */
            AddArg (&L, Arg);

        }
    }

    /* Store the new argument list in a safe place... */
    ArgCount = L.Count - 1;
    ArgVec   = L.Vec;

    /* ...and pass back the changed data also */
    *aArgCount = L.Count - 1;
    *aArgVec   = L.Vec;
}



void UnknownOption (const char* Opt)
/* Print an error about an unknown option and die. */
{
    AbEnd ("Unknown option: %s", Opt);
}



void NeedArg (const char* Opt)
/* Print an error about a missing option argument and exit. */
{
    AbEnd ("Option requires an argument: %s", Opt);
}



void InvArg (const char* Opt, const char* Arg)
/* Print an error about an invalid option argument and exit. */
{
    AbEnd ("Invalid argument for %s: '%s'", Opt, Arg);
}



void InvDef (const char* Def)
/* Print an error about an invalid definition and die */
{
    AbEnd ("Invalid definition: '%s'", Def);
}



const char* GetArg (unsigned* ArgNum, unsigned Len)
/* Get an argument for a short option. The argument may be appended to the
** option itself or may be separate. Len is the length of the option string.
*/
{
    const char* Arg = ArgVec[*ArgNum];
    if (Arg[Len] != '\0') {
        /* Argument appended */
        return Arg + Len;
    } else {
        /* Separate argument */
        Arg = ArgVec[*ArgNum + 1];
        if (Arg == 0) {
            /* End of arguments */
            NeedArg (ArgVec[*ArgNum]);
        }
        ++(*ArgNum);
        return Arg;
    }
}



void LongOption (unsigned* ArgNum, const LongOpt* OptTab, unsigned OptCount)
/* Handle a long command line option */
{
    /* Get the option and the argument (which may be zero) */
    const char* Opt = ArgVec[*ArgNum];

    /* Search the table for a match */
    while (OptCount) {
        if (strcmp (Opt, OptTab->Option) == 0) {
            /* Found, call the function */
            if (OptTab->ArgCount > 0) {
                /* We need an argument, check if we have one */
                const char* Arg = ArgVec[++(*ArgNum)];
                if (Arg == 0) {
                    NeedArg (Opt);
                }
                OptTab->Func (Opt, Arg);
            } else {
                OptTab->Func (Opt, 0);
            }
            /* Done */
            return;
        }

        /* Next table entry */
        --OptCount;
        ++OptTab;
    }

    /* Invalid option */
    UnknownOption (Opt);
}
