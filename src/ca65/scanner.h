/*****************************************************************************/
/*                                                                           */
/*                                 scanner.h                                 */
/*                                                                           */
/*                  The scanner for the ca65 macroassembler                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2011, Ullrich von Bassewitz                                      */
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



#ifndef SCANNER_H
#define SCANNER_H



/* ca65 */
#include "token.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Scanner variables */
extern Token CurTok;            /* Current input token incl. attributes */
extern int   ForcedEnd;         /* Force end of assembly */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int IsIdChar (int C);
/* Return true if the character is a valid character for an identifier */

int IsIdStart (int C);
/* Return true if the character may start an identifier */

int NewInputFile (const char* Name);
/* Open a new input file. Returns true if the file could be successfully opened
** and false otherwise.
*/

void NewInputData (char* Text, int Malloced);
/* Add a chunk of input data to the input stream */

void LocaseSVal (void);
/* Make SVal lower case */

void UpcaseSVal (void);
/* Make SVal upper case */

void NextRawTok (void);
/* Read the next raw token from the input stream */

int GetSubKey (const char** Keys, unsigned Count);
/* Search for a subkey in a table of keywords. The current token must be an
** identifier and all keys must be in upper case. The identifier will be
** uppercased in the process. The function returns the index of the keyword,
** or -1 if the keyword was not found.
*/

unsigned char ParseAddrSize (void);
/* Check if the next token is a keyword that denotes an address size specifier.
** If so, return the corresponding address size constant, otherwise output an
** error message and return ADDR_SIZE_DEFAULT.
*/

void InitScanner (const char* InFile);
/* Initialize the scanner, open the given input file */

void DoneScanner (void);
/* Release scanner resources */



/* End of scanner.h */

#endif
