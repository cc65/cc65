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
    int           ParamCount;   /* Number of parameters, -1 = no parens */
    Collection    Params;       /* Parameter list (char*) */
    StrBuf        Replacement;  /* Replacement text */
    unsigned char Predefined;   /* True if this is a predefined macro */
    unsigned char Variadic;     /* C99 variadic macro */
    char          Name[1];      /* Name, dynamically allocated */
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



Macro* NewMacro (const char* Name, unsigned char Predefined);
/* Allocate a macro structure with the given name. The structure is not
** inserted into the macro table.
*/

void FreeMacro (Macro* M);
/* Delete a macro definition. The function will NOT remove the macro from the
** table, use UndefineMacro for that.
*/

Macro* CloneMacro (const Macro* M);
/* Clone a macro definition. The function is not insert the macro into the
** macro table, thus the cloned instance cannot be freed with UndefineMacro.
** Use FreeMacro for that.
*/

void DefineNumericMacro (const char* Name, long Val);
/* Define a predefined macro for a numeric constant */

void DefineTextMacro (const char* Name, const char* Val);
/* Define a predefined macro for a textual constant */

void InsertMacro (Macro* M);
/* Insert the given macro into the macro table. */

Macro* UndefineMacro (const char* Name);
/* Search for the macro with the given name, if it exists, remove it from
** the defined macro table and insert it to a list for pending deletion.
** Return the macro if it was found and removed, return 0 otherwise.
** To safely free the removed macro, use FreeUndefinedMacros().
*/

void FreeUndefinedMacros (void);
/* Free all undefined macros */

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

int FindMacroParam (const Macro* M, const char* Param);
/* Search for a macro parameter. If found, return the index of the parameter.
** If the parameter was not found, return -1.
*/

void AddMacroParam (Macro* M, const char* Param);
/* Add a macro parameter. */

int MacroCmp (const Macro* M1, const Macro* M2);
/* Compare two macros and return zero if both are identical. */

void PrintMacroStats (FILE* F);
/* Print macro statistics to the given text file. */

void OutputAllMacrosFull (void);
/* Output all macros to the output file */

void OutputUserMacros (void);
/* Output the names of all user defined macros to the output file */

void OutputUserMacrosFull (void);
/* Output all user defined macros to the output file */



/* End of macrotab.h */

#endif
