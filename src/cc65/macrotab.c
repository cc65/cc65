/*****************************************************************************/
/*                                                                           */
/*                                macrotab.h                                 */
/*                                                                           */
/*             Preprocessor macro table for the cc65 C compiler              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2011, Ullrich von Bassewitz                                      */
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

/* common */
#include "hashfunc.h"
#include "xmalloc.h"

/* cc65 */
#include "error.h"
#include "macrotab.h"



/*****************************************************************************/
/*                                   data                                    */
/*****************************************************************************/



/* The macro hash table */
#define MACRO_TAB_SIZE  211
static Macro* MacroTab[MACRO_TAB_SIZE];



/*****************************************************************************/
/*                                   code                                    */
/*****************************************************************************/



Macro* NewMacro (const char* Name)
/* Allocate a macro structure with the given name. The structure is not
** inserted into the macro table.
*/
{
    /* Get the length of the macro name */
    unsigned Len = strlen(Name);

    /* Allocate the structure */
    Macro* M = (Macro*) xmalloc (sizeof(Macro) + Len);

    /* Initialize the data */
    M->Next        = 0;
    M->Expanding   = 0;
    M->ArgCount    = -1;        /* Flag: Not a function like macro */
    M->MaxArgs     = 0;
    InitCollection (&M->FormalArgs);
    SB_Init (&M->Replacement);
    M->Variadic    = 0;
    memcpy (M->Name, Name, Len+1);

    /* Return the new macro */
    return M;
}



void FreeMacro (Macro* M)
/* Delete a macro definition. The function will NOT remove the macro from the
** table, use UndefineMacro for that.
*/
{
    unsigned I;

    for (I = 0; I < CollCount (&M->FormalArgs); ++I) {
        xfree (CollAtUnchecked (&M->FormalArgs, I));
    }
    DoneCollection (&M->FormalArgs);
    SB_Done (&M->Replacement);
    xfree (M);
}



void DefineNumericMacro (const char* Name, long Val)
/* Define a macro for a numeric constant */
{
    char Buf[64];

    /* Make a string from the number */
    sprintf (Buf, "%ld", Val);

    /* Handle as text macro */
    DefineTextMacro (Name, Buf);
}



void DefineTextMacro (const char* Name, const char* Val)
/* Define a macro for a textual constant */
{
    /* Create a new macro */
    Macro* M = NewMacro (Name);

    /* Set the value as replacement text */
    SB_CopyStr (&M->Replacement, Val);

    /* Insert the macro into the macro table */
    InsertMacro (M);
}



void InsertMacro (Macro* M)
/* Insert the given macro into the macro table. */
{
    /* Get the hash value of the macro name */
    unsigned Hash = HashStr (M->Name) % MACRO_TAB_SIZE;

    /* Insert the macro */
    M->Next = MacroTab[Hash];
    MacroTab[Hash] = M;
}



int UndefineMacro (const char* Name)
/* Search for the macro with the given name and remove it from the macro
** table if it exists. Return 1 if a macro was found and deleted, return
** 0 otherwise.
*/
{
    /* Get the hash value of the macro name */
    unsigned Hash = HashStr (Name) % MACRO_TAB_SIZE;

    /* Search the hash chain */
    Macro* L = 0;
    Macro* M = MacroTab[Hash];
    while (M) {
        if (strcmp (M->Name, Name) == 0) {

            /* Found it */
            if (L == 0) {
                /* First in chain */
                MacroTab[Hash] = M->Next;
            } else {
                L->Next = M->Next;
            }

            /* Delete the macro */
            FreeMacro (M);

            /* Done */
            return 1;
        }

        /* Next macro */
        L = M;
        M = M->Next;
    }

    /* Not found */
    return 0;
}



Macro* FindMacro (const char* Name)
/* Find a macro with the given name. Return the macro definition or NULL */
{
    /* Get the hash value of the macro name */
    unsigned Hash = HashStr (Name) % MACRO_TAB_SIZE;

    /* Search the hash chain */
    Macro* M = MacroTab[Hash];
    while (M) {
        if (strcmp (M->Name, Name) == 0) {
            /* Found it */
            return M;
        }

        /* Next macro */
        M = M->Next;
    }

    /* Not found */
    return 0;
}



int FindMacroArg (Macro* M, const char* Arg)
/* Search for a formal macro argument. If found, return the index of the
** argument. If the argument was not found, return -1.
*/
{
    unsigned I;
    for (I = 0; I < CollCount (&M->FormalArgs); ++I) {
        if (strcmp (CollAtUnchecked (&M->FormalArgs, I), Arg) == 0) {
            /* Found */
            return I;
        }
    }

    /* Not found */
    return -1;
}



void AddMacroArg (Macro* M, const char* Arg)
/* Add a formal macro argument. */
{
    /* Check if we have a duplicate macro argument, but add it anyway.
    ** Beware: Don't use FindMacroArg here, since the actual argument array
    ** may not be initialized.
    */
    unsigned I;
    for (I = 0; I < CollCount (&M->FormalArgs); ++I) {
        if (strcmp (CollAtUnchecked (&M->FormalArgs, I), Arg) == 0) {
            /* Found */
            Error ("Duplicate macro parameter: `%s'", Arg);
            break;
        }
    }

    /* Add the new argument */
    CollAppend (&M->FormalArgs, xstrdup (Arg));
    ++M->ArgCount;
}



int MacroCmp (const Macro* M1, const Macro* M2)
/* Compare two macros and return zero if both are identical. */
{
    int I;

    /* Argument count must be identical */
    if (M1->ArgCount != M2->ArgCount) {
        return 1;
    }

    /* Compare the arguments */
    for (I = 0; I < M1->ArgCount; ++I) {
        if (strcmp (CollConstAt (&M1->FormalArgs, I),
                    CollConstAt (&M2->FormalArgs, I)) != 0) {
            return 1;
        }
    }

    /* Compare the replacement */
    return SB_Compare (&M1->Replacement, &M2->Replacement);
}



void PrintMacroStats (FILE* F)
/* Print macro statistics to the given text file. */
{
    unsigned I;
    Macro* M;

    fprintf (F, "\n\nMacro Hash Table Summary\n");
    for (I = 0; I < MACRO_TAB_SIZE; ++I) {
        fprintf (F, "%3u : ", I);
        M = MacroTab [I];
        if (M) {
            while (M) {
                fprintf (F, "%s ", M->Name);
                M = M->Next;
            }
            fprintf (F, "\n");
        } else {
            fprintf (F, "empty\n");
        }
    }
}
