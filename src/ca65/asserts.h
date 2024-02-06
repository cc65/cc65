/*****************************************************************************/
/*                                                                           */
/*                                 asserts.h                                 */
/*                                                                           */
/*               Linker assertions for the ca65 crossassembler               */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2009, Ullrich von Bassewitz                                      */
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



#ifndef ASSERTS_H
#define ASSERTS_H



/* common */
#include "assertion.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Forwards */
struct ExprNode;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void AddAssertion (struct ExprNode* Expr, AssertAction Action, unsigned Msg);
/* Add an assertion to the assertion table */

void CheckAssertions (void);
/* Check all assertions and evaluate the ones we can evaluate here. */

void WriteAssertions (void);
/* Write the assertion table to the object file */



/* End of asserts.h */

#endif
