/*****************************************************************************/
/*                                                                           */
/*                                 nexttok.h                                 */
/*                                                                           */
/*              Get next token and handle token level functions              */
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



#ifndef NEXTTOK_H
#define NEXTTOK_H



#include "scanner.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void NextTok (void);
/* Get next token and handle token level functions */

int Consume (token_t Expected, const char* ErrMsg);
/* Consume Token, print an error if we don't find it. Return true if the token
** was found and false otherwise.
*/

int ConsumeSep (void);
/* Consume a separator token. Return true if the token was found and false
 * otherwise.
 */

int ConsumeLParen (void);
/* Consume a left paren. Return true if the token was found and false
** otherwise.
*/

int ConsumeRParen (void);
/* Consume a right paren. Return true if the token was found and false
** otherwise.
*/

int ConsumeComma (void);
/* Consume a comma. Return true if the token was found and false
** otherwise.
*/

void SkipUntilSep (void);
/* Skip tokens until we reach a line separator or end of file */

void EnterRawTokenMode (void);
/* Enter raw token mode. In raw mode, token handling functions are not
** executed, but the function tokens are passed untouched to the upper
** layer. Raw token mode is used when storing macro tokens for later
** use.
** Calls to EnterRawTokenMode and LeaveRawTokenMode may be nested.
*/

void LeaveRawTokenMode (void);
/* Leave raw token mode. */



/* End of nexttok.h */

#endif
