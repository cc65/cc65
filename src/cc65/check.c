/*****************************************************************************/
/*                                                                           */
/*				    check.c				     */
/*                                                                           */
/*		     Assert macros for the cc65 C compiler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998     Ullrich von Bassewitz                                        */
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



#include <stdlib.h>

#include "error.h"
#include "check.h"



/*****************************************************************************/
/*				     Data				     */
/*****************************************************************************/



/* Predefined messages */
const char* _MsgInternalError	= "Internal error: ";
const char* _MsgAbstractCall	= "Call to abstract method";
const char* _MsgPrecondition	= "Precondition violated: ";
const char* _MsgCheckFailed	= "Check failed: ";
const char* _MsgProgramAborted	= "Program aborted: ";



static void _CheckFailed (const char* msg, const char* cond,
       	       	          int code, const char* file, unsigned line);

void (*CheckFailed) (const char* Msg, const char* Cond, int Code,
       	       	     const char* File, unsigned Line) = _CheckFailed;
/* Function pointer that is called from check if the condition code is true. */



/*****************************************************************************/
/*		   	  	     Code			   	     */
/*****************************************************************************/



static void _CheckFailed (const char* Msg, const char* Cond,
         		  int Code, const char* File, unsigned Line)
{
    /* Log the error */
    if (Code) {
	Internal ("%s%s (= %d), file `%s', line %u", Msg, Cond, Code, File, Line);
    } else {
       	Internal ("%s%s, file `%s', line %u", Msg, Cond, File, Line);
    }

    /* Use abort() to create a core for debugging */
    abort ();
}



void Check (const char* Msg, const char* Cond, int Code,
	    const char* File, unsigned Line)
/* This function is called from all check macros (see below). It checks,
 * wether the given Code is true (!= 0). If so, it calls the CheckFailed
 * vector with the given strings. If not, it simply returns.
 */
{
    if (Code != 0) {
	CheckFailed (Msg, Cond, Code, File, Line);
    }
}



