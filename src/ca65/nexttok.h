/*****************************************************************************/
/*                                                                           */
/*			 	   nexttok.h				     */
/*                                                                           */
/*		Get next token and handle token level functions		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000     Ullrich von Bassewitz                                        */
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



#ifndef NEXTTOK_H
#define NEXTTOK_H



#include "scanner.h"



/*****************************************************************************/
/*     	       	    		     Code			   	     */
/*****************************************************************************/



void NextTok (void);
/* Get next token and handle token level functions */

void Consume (enum Token Expected, unsigned ErrMsg);
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
/* Skip tokens until we reach a line separator */



/* End of nexttok.h */

#endif




