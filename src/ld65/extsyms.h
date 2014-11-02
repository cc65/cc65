/*****************************************************************************/
/*                                                                           */
/*                                 extsyms.h                                 */
/*                                                                           */
/*      Handle program external symbols for relocatable output formats       */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1999-2001 Ullrich von Bassewitz                                       */
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



#ifndef EXTSYMS_H
#define EXTSYMS_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Forward decl for structure holding an external symbol */
typedef struct ExtSym ExtSym;

/* External symbol table structure */
typedef struct ExtSymTab ExtSymTab;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



ExtSym* NewExtSym (ExtSymTab* Tab, unsigned Name);
/* Create a new external symbol and insert it into the list */

ExtSymTab* NewExtSymTab (void);
/* Create a new external symbol table */

void FreeExtSymTab (ExtSymTab* Tab);
/* Free an external symbol structure */

ExtSym* GetExtSym (const ExtSymTab* Tab, unsigned Name);
/* Return the entry for the external symbol with the given name. Return NULL
** if there is no such symbol.
*/

unsigned ExtSymCount (const ExtSymTab* Tab);
/* Return the number of symbols in the table */

const ExtSym* ExtSymList (const ExtSymTab* Tab);
/* Return the start of the symbol list sorted by symbol number. Call
** ExtSymNext for the next symbol.
*/

unsigned ExtSymNum (const ExtSym* E);
/* Return the number of an external symbol */

unsigned ExtSymName (const ExtSym* E);
/* Return the symbol name index */

const ExtSym* ExtSymNext (const ExtSym* E);
/* Return the next symbol in the list */



/* End of extsyms.h */

#endif
