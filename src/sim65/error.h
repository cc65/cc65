/*****************************************************************************/
/*                                                                           */
/*                                 error.h                                   */
/*                                                                           */
/*                    Error handling for the sim65 simulator                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2003 Ullrich von Bassewitz                                       */
/*               R�merstrasse 52                                             */
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



#ifndef ERROR_H
#define ERROR_H



/* common */
#include "attrib.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



#define SIM65_ERROR         256
/* Does not use EXIT_FAILURE because it may overlap with test results. */

#define SIM65_ERROR_TIMEOUT 257
/* An error result for max CPU instructions exceeded. */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void Warning (const char* Format, ...) attribute((format(printf,1,2)));
/* Print a warning message */

void Error (const char* Format, ...) attribute((noreturn, format(printf,1,2)));
/* Print an error message and die */

void ErrorCode (int Code, const char* Format, ...) attribute((noreturn, format(printf,2,3)));
/* Print an error message and die with the given exit code */

void Internal (const char* Format, ...) attribute((noreturn, format(printf,1,2)));
/* Print an internal error message and die */



/* End of error.h */

#endif
