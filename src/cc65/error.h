/*****************************************************************************/
/*                                                                           */
/*				    error.h				     */
/*                                                                           */
/*		    Error handling for the cc65 C compiler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
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



#ifndef ERROR_H
#define ERROR_H



/* common */
#include "attrib.h"

/* cc65 */
#include "lineinfo.h"



/*****************************************************************************/
/*	  			     Data		     		     */
/*****************************************************************************/



/* Count of errors/warnings */
extern unsigned ErrorCount;
extern unsigned WarningCount;



/*****************************************************************************/
/* 	       	      	 	     code				     */
/*****************************************************************************/



void Warning (const char* Format, ...) attribute ((format (printf, 1, 2)));
/* Print warning message. */

void LIWarning (const LineInfo* LI, const char* Format, ...) attribute ((format (printf, 2, 3)));
/* Print a warning message with the line info given explicitly */

void PPWarning (const char* Format, ...) attribute ((format (printf, 1, 2)));
/* Print warning message. For use within the preprocessor. */

void Error (const char* Format, ...) attribute ((format (printf, 1, 2)));
/* Print an error message */

void LIError (const LineInfo* LI, const char* Format, ...) attribute ((format (printf, 2, 3)));
/* Print an error message with the line info given explicitly */

void PPError (const char* Format, ...) attribute ((format (printf, 1, 2)));
/* Print an error message. For use within the preprocessor.  */

void Fatal (const char* Format, ...) attribute ((noreturn, format (printf, 1, 2)));
/* Print a message about a fatal error and die */

void Internal (const char* Format, ...) attribute ((noreturn, format (printf, 1, 2)));
/* Print a message about an internal compiler error and die. */

void ErrorReport (void);
/* Report errors (called at end of compile) */



/* End of error.h */
#endif





