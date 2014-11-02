/*****************************************************************************/
/*                                                                           */
/*                                macrotab.h                                 */
/*                                                                           */
/*             Preprocessor macro table for the cc65 C compiler              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2005, Ullrich von Bassewitz                                      */
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



#ifndef MACROTAB_H
#define MACROTAB_H



/* common */
#include "coll.h"
#include "inline.h"
#include "strbuf.h"



/*****************************************************************************/
/*                                   data                                    */
/*****************************************************************************/



/* Structure describing a macro */
typedef struct Macro Macro;
struct Macro {
    Macro*        Next;         /* Next macro with same hash value */
    int           Expanding;    /* Are we currently expanding this macro? */
    int           ArgCount;     /* Number of parameters, -1 = no parens */
    unsigned      MaxArgs;      /* Size of formal argument list */
    Collection    FormalArgs;   /* Formal argument list (char*) */
    StrBuf        Replacement;  /* Replacement text */
    unsigned char Variadic;     /* C99 variadic macro */
    char          Name[1];      /* Name, dynamically allocated */
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



Macro* NewMacro (const char* Name);
/* Allocate a macro structure with the given name. The structure is not
** inserted into the macro table.
*/

void FreeMacro (Macro* M);
/* Delete a macro definition. The function will NOT remove the macro from the
** table, use UndefineMacro for that.
*/

void DefineNumericMacro (const char* Name, long Val);
/* Define a macro for a numeric constant */

void DefineTextMacro (const char* Name, const char* Val);
/* Define a macro for a textual constant */

void InsertMacro (Macro* M);
/* Insert the given macro into the macro table. */

int UndefineMacro (const char* Name);
/* Search for the macro with the given name and remove it from the macro
** table if it exists. Return 1 if a macro was found and deleted, return
** 0 otherwise.
*/

Macro* FindMacro (const char* Name);
/* Find a macro with the given name. Return the macro definition or NULL */

#if defined(HAVE_INLINE)
INLINE int IsMacro (const char* Name)
/* Return true if the given name is the name of a macro, return false otherwise */
{
    return FindMacro (Name) != 0;
}
#else
#  define IsMacro(Name)         (FindMacro (Name) != 0)
#endif

int FindMacroArg (Macro* M, const char* Arg);
/* Search for a formal macro argument. If found, return the index of the
** argument. If the argument was not found, return -1.
*/

void AddMacroArg (Macro* M, const char* Arg);
/* Add a formal macro argument. */

int MacroCmp (const Macro* M1, const Macro* M2);
/* Compare two macros and return zero if both are identical. */

void PrintMacroStats (FILE* F);
/* Print macro statistics to the given text file. */



/* End of macrotab.h */

#endif
