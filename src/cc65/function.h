/*****************************************************************************/
/*                                                                           */
/*				  function.h				     */
/*                                                                           */
/*			      Function management			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2001 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



#ifndef FUNCTION_H
#define FUNCTION_H



/*****************************************************************************/
/*			    	     data				     */
/*****************************************************************************/



/* Structure that holds all data needed for function activation */
typedef struct Function Function;

/* Function activation data for current function (or NULL) */
extern Function* CurrentFunc;



/*****************************************************************************/
/*	      		    	     Code		     		     */
/*****************************************************************************/



const char* GetFuncName (const Function* F);
/* Return the name of the current function */

unsigned GetParamCount (const Function* F);
/* Return the parameter count for the current function */

unsigned GetParamSize (const Function* F);
/* Return the parameter size for the current function */

type* GetReturnType (Function* F);
/* Get the return type for the function */

int HasVoidReturn (const Function* F);
/* Return true if the function does not have a return value */

int IsVariadic (const Function* F);
/* Return true if this is a variadic function */

void RememberEntry (Function* F);
/* Remember the current output position for local space creation later */

unsigned GetRetLab (const Function* F);
/* Return the return jump label */

int ReserveLocalSpace (Function* F, unsigned Size);
/* Reserve (but don't allocate) the given local space and return the stack
 * offset.
 */

void AllocLocalSpace (Function* F);
/* Allocate any local space previously reserved. The function will do
 * nothing if there is no reserved local space.
 */

void NewFunc (struct SymEntry* Func);
/* Parse argument declarations and function body. */



/* End of function.h */
#endif



