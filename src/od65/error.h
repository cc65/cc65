/*****************************************************************************/
/*                                                                           */
/*				   global.h				     */
/*                                                                           */
/*	     Error handling for the od65 object file dump utility	     */
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



/*****************************************************************************/
/*	  		   	     Data		     		     */
/*****************************************************************************/



/* Messages for internal compiler errors */
extern const char _MsgCheckFailed [];
extern const char _MsgPrecondition [];
extern const char _MsgFail [];



/*****************************************************************************/
/*     	      	    		     Code				     */
/*****************************************************************************/



void Warning (const char* Format, ...) attribute((format(printf,1,2)));
/* Print a warning message */

void Error (const char* Format, ...) attribute((format(printf,1,2)));
/* Print an error message and die */

void Internal (const char* Format, ...) attribute((format(printf,1,2)));
/* Print an internal error message and die */

#define CHECK(c)     							\
    if (!(c)) 	       	       	       	       	       	     		\
 	Internal (_MsgCheckFailed, #c, c, __FILE__, __LINE__)

#define PRECONDITION(c)							\
    if (!(c)) 	       	       	       	       	       	     		\
       	Internal (_MsgPrecondition, #c, c, __FILE__, __LINE__)

#define FAIL(s)								\
    Internal (_MsgFail, s, __FILE__, __LINE__)



/* End of error.h */

#endif



