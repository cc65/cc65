/*****************************************************************************/
/*                                                                           */
/*				    check.h				     */
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



#ifndef CHECK_H
#define CHECK_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



extern const char* _MsgInternalError;           /* "Internal error: "        */
extern const char* _MsgPrecondition;            /* "Precondition violated: " */
extern const char* _MsgCheckFailed;             /* "Check failed: "          */
extern const char* _MsgProgramAborted;		/* "Program aborted: "       */



extern void (*CheckFailed) (const char* Msg, const char* Cond,
       	       	            int Code, const char* File, unsigned Line);
/* Function pointer that is called from check if the condition code is true. */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void Check (const char* Msg, const char* Cond, int Code,
            const char* File, unsigned Line);
/* This function is called from all check macros (see below). It checks,
 * wether the given Code is true (!= 0). If so, it calls the CheckFailed
 * vector with the given strings. If not, it simply returns.
 */



#define FAIL(s) CheckFailed (_MsgInternalError, s, 0, __FILE__, __LINE__)
/* Fail macro. Is used if something evil happens, calls checkfailed directly. */

#define ABORT(s) CheckFailed (_MsgProgramAborted, s, 0, __FILE__, __LINE__)
/* Use this one instead of FAIL if there is no internal program error but an
 * error condition that is caused by the user or operating system (FAIL and
 * ABORT are essentially the same but the message differs).
 */

#define PRECONDITION(c) Check (_MsgPrecondition, #c, !(c), __FILE__, __LINE__)

#define CHECK(c)        Check (_MsgCheckFailed, #c, !(c), __FILE__, __LINE__)

#define ZCHECK(c)       Check (_MsgCheckFailed, #c, c, __FILE__, __LINE__)



/* End of check.h */
#endif



