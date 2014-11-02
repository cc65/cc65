/*****************************************************************************/
/*                                                                           */
/*                                 filetab.h                                 */
/*                                                                           */
/*                         Input file table for ca65                         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2008 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
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



#ifndef FILETAB_H
#define FILETAB_H



/* common */
#include "strbuf.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* An enum that describes different types of input files. The members are
** choosen so that it is possible to combine them to bitsets
*/
typedef enum {
    FT_MAIN     = 0x01,         /* Main input file */
    FT_INCLUDE  = 0x02,         /* Normal include file */
    FT_BINARY   = 0x04,         /* Binary include file */
    FT_DBGINFO  = 0x08,         /* File from debug info */
} FileType;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



const StrBuf* GetFileName (unsigned Name);
/* Get the name of a file where the name index is known */

unsigned GetFileIndex (const StrBuf* Name);
/* Return the file index for the given file name. */

unsigned AddFile (const StrBuf* Name, FileType Type,
                  unsigned long Size, unsigned long MTime);
/* Add a new file to the list of input files. Return the index of the file in
** the table.
*/

void WriteFiles (void);
/* Write the list of input files to the object file */

void CreateDependencies (void);
/* Create dependency files requested by the user */



/* End of filetab.h */

#endif
