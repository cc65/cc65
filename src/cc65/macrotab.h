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



#ifndef MACROTAB_H
#define MACROTAB_H



/*****************************************************************************/
/*				     data				     */
/*****************************************************************************/



typedef struct Macro_ Macro;
struct Macro_ {
    Macro*	 Next;		/* Next macro with same hash value */
    int		 ArgCount;	/* Number of parameters, -1 = no parens */
    unsigned	 MaxArgs;	/* Size of formal argument list */
    char**     	 FormalArgs;	/* Formal argument list */
    char const** ActualArgs;	/* Actual argument list */
    char*      	 Replacement;   /* Replacement text */
    char	 Name[1];   	/* Name, dynamically allocated */
};



/*****************************************************************************/
/*	   		   	     code	    			     */
/*****************************************************************************/



Macro* NewMacro (const char* Name);
/* Allocate a macro structure with the given name. The structure is not
 * inserted into the macro table.
 */

void FreeMacro (Macro* M);
/* Delete a macro definition. The function will NOT remove the macro from the
 * table, use UndefineMacro for that.
 */

void AddNumericMacro (const char* Name, long Val);
/* Add a macro for a numeric constant */

void AddTextMacro (const char* Name, const char* Val);
/* Add a macro for a textual constant */

void InsertMacro (Macro* M);
/* Insert the given macro into the macro table. This call will also allocate
 * the ActualArgs parameter array.
 */

int UndefineMacro (const char* Name);
/* Search for the macro with the given name and remove it from the macro
 * table if it exists. Return 1 if a macro was found and deleted, return
 * 0 otherwise.
 */

Macro* FindMacro (const char* Name);
/* Find a macro with the given name. Return the macro definition or NULL */

int IsMacro (const char* Name);
/* Return true if the given name is the name of a macro, return false otherwise */

int MaybeMacro (unsigned char C);
/* Return true if the given character may be the start of the name of an
 * existing macro, return false if not.
 */

const char* FindMacroArg (Macro* M, const char* Arg);
/* Search for a formal macro argument. If found, return the actual
 * (replacement) argument. If the argument was not found, return NULL.
 */

void AddMacroArg (Macro* M, const char* Arg);
/* Add a formal macro argument. */

void PrintMacroStats (FILE* F);
/* Print macro statistics to the given text file. */



/* End of macrotab.h */
#endif




