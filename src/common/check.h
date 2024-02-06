/*****************************************************************************/
/*                                                                           */
/*                                  check.h                                  */
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



#ifndef CHECK_H
#define CHECK_H



#include "attrib.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



extern const char* MsgInternalError;            /* "Internal error: "        */
extern const char* MsgPrecondition;             /* "Precondition violated: " */
extern const char* MsgCheckFailed;              /* "Check failed: "          */
extern const char* MsgProgramAborted;           /* "Program aborted: "       */



extern void (*CheckFailed) (const char* Msg, const char* Cond,
                            const char* File, unsigned Line)
                            attribute ((noreturn));
/* Function pointer that is called from check if the condition code is true. */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



#define FAIL(s) CheckFailed (MsgInternalError, s, __FILE__, __LINE__)
/* Fail macro. Is used if something evil happens, calls checkfailed directly. */

#define ABORT(s) CheckFailed (MsgProgramAborted, s, __FILE__, __LINE__)
/* Use this one instead of FAIL if there is no internal program error but an
** error condition that is caused by the user or operating system (FAIL and
** ABORT are essentially the same but the message differs).
*/

#define PRECONDITION(c) \
    ((void) ((c)? 0 : (CheckFailed (MsgPrecondition, #c, __FILE__, __LINE__), 0)))

#define CHECK(c)        \
    ((void) ((c)? 0 : (CheckFailed (MsgCheckFailed, #c, __FILE__, __LINE__), 0)))



/* End of check.h */

#endif
