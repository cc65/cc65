/*****************************************************************************/
/*                                                                           */
/*                                 library.h                                 */
/*                                                                           */
/*          Library data structures and helpers for the ld65 linker          */
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



#ifndef LIBRARY_H
#define LIBRARY_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Opaque structure */
struct Library;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void LibAdd (FILE* F, const char* Name);
/* Add files from the library to the list if there are references that could
** be satisfied.
*/

void LibStartGroup (void);
/* Start a library group. Objects within a library group may reference each
** other, and libraries are searched repeatedly until all references are
** satisfied.
*/

void LibEndGroup (void);
/* End a library group and resolve all open references. Objects within a
** library group may reference each other, and libraries are searched
** repeatedly until all references are satisfied.
*/

void LibCheckGroup (void);
/* Check if there are open library groups */

const char* GetLibFileName (const struct Library* L);
/* Get the name of a library */

unsigned GetLibId (const struct Library* L);
/* Get the id of a library file. */

unsigned LibraryCount (void);
/* Return the total number of libraries */

void PrintDbgLibraries (FILE* F);
/* Output the libraries to a debug info file */



/* End of library.h */

#endif
