/*****************************************************************************/
/*                                                                           */
/*				  macrotab.h				     */
/*                                                                           */
/*	       Preprocessor macro table for the cc65 C compiler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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
#include "hashstr.h"
#include "xmalloc.h"

/* cc65 */
#include "error.h"
#include "macrotab.h"



/*****************************************************************************/
/*				     data				     */
/*****************************************************************************/



/* The macro hash table */
#define MACRO_TAB_SIZE	211
static Macro* MacroTab[MACRO_TAB_SIZE];

/* A table that holds the count of macros that start with a specific character.
 * It is used to determine quickly, if an identifier may be a macro or not
 * without calculating the hash over the name.
 */
static unsigned short MacroFlagTab[256];



/*****************************************************************************/
/*	   		   	     code				     */
/*****************************************************************************/



Macro* NewMacro (const char* Name)
/* Allocate a macro structure with the given name. The structure is not
 * inserted into the macro table.
 */
{
    /* Get the length of the macro name */
    unsigned Len = strlen(Name);

    /* Allocate the structure */
    Macro* M = (Macro*) xmalloc (sizeof(Macro) + Len);

    /* Initialize the data */
    M->Next   	   = 0;
    M->ArgCount    = -1;	/* Flag: Not a function like macro */
    M->MaxArgs	   = 0;
    M->FormalArgs  = 0;
    M->ActualArgs  = 0;
    M->Replacement = 0;
    memcpy (M->Name, Name, Len+1);

    /* Return the new macro */
    return M;
}



void FreeMacro (Macro* M)
/* Delete a macro definition. The function will NOT remove the macro from the
 * table, use UndefineMacro for that.
 */
{
    int I;

    for (I = 0; I < M->ArgCount; ++I) {
	xfree (M->FormalArgs[I]);
    }
    xfree (M->FormalArgs);
    xfree (M->ActualArgs);
    xfree (M->Replacement);
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
    M->Replacement = xstrdup (Val);

    /* Insert the macro into the macro table */
    InsertMacro (M);
}



void InsertMacro (Macro* M)
/* Insert the given macro into the macro table. This call will also allocate
 * the ActualArgs parameter array.
 */
{
    unsigned Hash;

    /* Allocate the ActualArgs parameter array */
    if (M->ArgCount > 0) {
    	M->ActualArgs = (char const**) xmalloc (M->ArgCount * sizeof(char*));
    }

    /* Get the hash value of the macro name */
    Hash = HashStr (M->Name) % MACRO_TAB_SIZE;

    /* Insert the macro */
    M->Next = MacroTab[Hash];
    MacroTab[Hash] = M;

    /* Increment the number of macros starting with this char */
    MacroFlagTab[(unsigned)(unsigned char)M->Name[0]]++;
}
    


int UndefineMacro (const char* Name)
/* Search for the macro with the given name and remove it from the macro
 * table if it exists. Return 1 if a macro was found and deleted, return
 * 0 otherwise.
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

	    /* Decrement the number of macros starting with this char */
	    MacroFlagTab[(unsigned)(unsigned char)M->Name[0]]--;

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



int IsMacro (const char* Name)
/* Return true if the given name is the name of a macro, return false otherwise */
{
    return FindMacro(Name) != 0;
}



int MaybeMacro (unsigned char C)
/* Return true if the given character may be the start of the name of an
 * existing macro, return false if not.
 */
{
    return (MacroFlagTab[C] > 0);
}



const char* FindMacroArg (Macro* M, const char* Arg)
/* Search for a formal macro argument. If found, return the actual
 * (replacement) argument. If the argument was not found, return NULL.
 */
{
    int I;
    for (I = 0; I < M->ArgCount; ++I) {
	if (strcmp (M->FormalArgs[I], Arg) == 0) {
	    /* Found */
	    return M->ActualArgs[I];
	}
    }
    /* Not found */
    return 0;
}



void AddMacroArg (Macro* M, const char* Arg)
/* Add a formal macro argument. */
{
    /* Check if we have a duplicate macro argument, but add it anyway.
     * Beware: Don't use FindMacroArg here, since the actual argument array
     * may not be initialized.
     */
    int I;
    for (I = 0; I < M->ArgCount; ++I) {
	if (strcmp (M->FormalArgs[I], Arg) == 0) {
	    /* Found */
	    Error ("Duplicate macro parameter: `%s'", Arg);
	    break;
	}
    }

    /* Check if we have enough room available, otherwise expand the array
     * that holds the formal argument list.
     */
    if (M->ArgCount >= (int) M->MaxArgs) {
	/* We must expand the array */
	char** OldArgs = M->FormalArgs;
	M->MaxArgs += 10;
	M->FormalArgs = (char**) xmalloc (M->MaxArgs * sizeof(char*));
	memcpy (M->FormalArgs, OldArgs, M->ArgCount * sizeof (char*));
	xfree (OldArgs);
    }

    /* Add the new argument */
    M->FormalArgs[M->ArgCount++] = xstrdup (Arg);
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
	if (strcmp (M1->FormalArgs[I], M2->FormalArgs[I]) != 0) {
	    return 1;
	}
    }

    /* Compare the replacement */
    return strcmp (M1->Replacement, M2->Replacement);
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



