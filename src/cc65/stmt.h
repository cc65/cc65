/*****************************************************************************/
/*                                                                           */
/*                                  stmt.h                                   */
/*                                                                           */
/*                             Parse a statement                             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2008 Ullrich von Bassewitz                                       */
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



#ifndef STMT_H
#define STMT_H



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int Statement (int* PendingToken);
/* Statement parser. Returns 1 if the statement does a return/break, returns
** 0 otherwise. If the PendingToken pointer is not NULL, the function will
** not skip the terminating token of the statement (closing brace or
** semicolon), but store true if there is a pending token, and false if there
** is none. The token is always checked, so there is no need for the caller to
** check this token, it must be skipped, however. If the argument pointer is
** NULL, the function will skip the token.
*/



/* End of stmt.h */

#endif
