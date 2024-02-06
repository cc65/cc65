/*****************************************************************************/
/*                                                                           */
/*                                  error.h                                  */
/*                                                                           */
/*                Error handling for the ca65 macroassembler                 */
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



#ifndef ERROR_H
#define ERROR_H



#if defined( __MINGW32__)
#  pragma GCC diagnostic ignored "-Wformat"
#endif



/* common */
#include "attrib.h"
#include "coll.h"
#include "filepos.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Warning levels */
extern unsigned WarnLevel;

/* Statistics */
extern unsigned ErrorCount;
extern unsigned WarningCount;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void Warning (unsigned Level, const char* Format, ...) attribute ((format (printf, 2, 3)));
/* Print warning message. */

void PWarning (const FilePos* Pos, unsigned Level, const char* Format, ...) attribute ((format (printf, 3, 4)));
/* Print warning message giving an explicit file and position. */

void LIWarning (const Collection* LineInfos, unsigned Level, const char* Format, ...) attribute ((format (printf, 3, 4)));
/* Print warning message using the given line infos */

void Error (const char* Format, ...) attribute ((format (printf, 1, 2)));
/* Print an error message */

void PError (const FilePos* Pos, const char* Format, ...) attribute ((format (printf, 2, 3)));
/* Print an error message giving an explicit file and position. */

void LIError (const Collection* LineInfos, const char* Format, ...) attribute ((format (printf, 2, 3)));
/* Print an error message using the given line infos. */

void ErrorSkip (const char* Format, ...) attribute ((format (printf, 1, 2)));
/* Print an error message and skip the rest of the line */

void Fatal (const char* Format, ...) attribute((noreturn, format(printf,1,2)));
/* Print a message about a fatal error and die */

void Internal (const char* Format, ...) attribute((noreturn, format(printf,1,2)));
/* Print a message about an internal assembler error and die. */



/* End of error.h */

#endif
