/*****************************************************************************/
/*                                                                           */
/*				     ea.h				     */
/*                                                                           */
/*	     Effective address parsing for the ca65 macroassembler	     */
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



#ifndef EA_H
#define EA_H



#include "expr.h"



/*****************************************************************************/
/*     	      	    		     Data				     */
/*****************************************************************************/



/*****************************************************************************/
/*     	      	    		     Code				     */
/*****************************************************************************/



void GetEA (unsigned long* AddrMode, ExprNode** Expr, ExprNode** Bank);
/* Parse an effective address, return the possible modes in AddrMode, and the
 * expression involved (if any) in Expr.
 */



/* End of ea.h */

#endif



