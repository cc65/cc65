/*****************************************************************************/
/*                                                                           */
/*                                 dbginfo.h                                 */
/*                                                                           */
/*                         Handle the .dbg commands                          */
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



#ifndef DBGINFO_H
#define DBGINFO_H



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void DbgInfoFile (void);
/* Parse and handle FILE subcommand of the .dbg pseudo instruction */

void DbgInfoFunc (void);
/* Parse and handle FUNC subcommand of the .dbg pseudo instruction */

void DbgInfoLine (void);
/* Parse and handle LINE subcommand of the .dbg pseudo instruction */

void DbgInfoSym (void);
/* Parse and handle SYM subcommand of the .dbg pseudo instruction */

void DbgInfoCheck (void);
/* Do checks on all hll debug info symbols when assembly is complete */

void WriteHLLDbgSyms (void);
/* Write a list of all high level language symbols to the object file. */



/* End of dbginfo.h */

#endif
