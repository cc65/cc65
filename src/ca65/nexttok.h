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

void Consume (token_t Expected, const char* ErrMsg);
/* Consume Token, print an error if we don't find it */

void ConsumeSep (void);
/* Consume a separator token */

void ConsumeLParen (void);
/* Consume a left paren */

void ConsumeRParen (void);
/* Consume a right paren */

void ConsumeComma (void);
/* Consume a comma */

void SkipUntilSep (void);
/* Skip tokens until we reach a line separator or end of file */

void ExpectSep (void);
/* Check if we've reached a line separator, and output an error if not. Do
** not skip the line separator.
*/

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
