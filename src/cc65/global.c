/*****************************************************************************/
/*                                                                           */
/*				   global.c				     */
/*                                                                           */
/*		   Global variables for the cc65 C compiler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2004 Ullrich von Bassewitz                                       */
/*               Römerstraße 52                                              */
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



#include "global.h"



/*****************************************************************************/
/*     	      	    	       	     Data				     */
/*****************************************************************************/



unsigned char AddSource		= 0; 	/* Add source lines as comments */
unsigned char DebugInfo		= 0;	/* Add debug info to the obj */
unsigned char CreateDep		= 0;	/* Create a dependency file */
unsigned char ANSI   	       	= 0;	/* Strict ANSI flag */
unsigned      RegisterSpace     = 6;    /* Space available for register vars */

/* Stackable options */
IntStack WarnDisable        = INTSTACK(0);  /* Suppress warnings */
IntStack WritableStrings    = INTSTACK(0);  /* Literal strings are r/w */
IntStack InlineStdFuncs     = INTSTACK(0);  /* Inline some known functions */
IntStack EnableRegVars      = INTSTACK(0);  /* Enable register variables */
IntStack AllowRegVarAddr    = INTSTACK(0);  /* Allow taking addresses of register vars */
IntStack RegVarsToCallStack = INTSTACK(0);  /* Save reg variables on call stack */
IntStack StaticLocals       = INTSTACK(0);  /* Make local variables static */
IntStack SignedChars        = INTSTACK(0);  /* Make characters signed by default */
IntStack CheckStack         = INTSTACK(0);  /* Generate stack overflow checks */
IntStack Optimize      	    = INTSTACK(0);  /* Optimize flag */
IntStack CodeSizeFactor	    = INTSTACK(100);/* Size factor for generated code */




