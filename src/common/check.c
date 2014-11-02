/*****************************************************************************/
/*                                                                           */
/*                                  check.c                                  */
/*                                                                           */
/*                            Assert like macros                             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2001 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
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



#include <stdlib.h>

#include "abend.h"
#include "check.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Predefined messages */
const char* MsgInternalError    = "Internal error: ";
const char* MsgPrecondition     = "Precondition violated: ";
const char* MsgCheckFailed      = "Check failed: ";
const char* MsgProgramAborted   = "Program aborted: ";



static void DefaultCheckFailed (const char* msg, const char* cond,
                                const char* file, unsigned line)
                                attribute ((noreturn));

void (*CheckFailed) (const char* Msg, const char* Cond,
                     const char* File, unsigned Line) attribute ((noreturn))
                = DefaultCheckFailed;
/* Function pointer that is called from check if the condition code is true. */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void DefaultCheckFailed (const char* Msg, const char* Cond,
                                const char* File, unsigned Line)
{
    /* Output a diagnostic and abort */
    AbEnd ("%s%s, file `%s', line %u", Msg, Cond, File, Line);
}
