/*****************************************************************************/
/*                                                                           */
/*				   global.c				     */
/*                                                                           */
/*		   Global variables for the cc65 C compiler		     */
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



#include "global.h"



/*****************************************************************************/
/*     	      	    	       	     Data				     */
/*****************************************************************************/



unsigned char ANSI   	       	= 0;	/* Strict ANSI flag */
unsigned char WriteableStrings	= 0;   	/* Literal strings are r/w */
unsigned char NoWarn		= 0;   	/* Suppress warnings */
unsigned char Optimize		= 0;   	/* Optimize flag */
unsigned long OptDisable	= 0;	/* Optimizer passes to disable */
unsigned char FavourSize	= 1;   	/* Favour size over speed */
unsigned      CodeSizeFactor	= 100;	/* Size factor for generated code */
unsigned char InlineStdFuncs	= 0;   	/* Inline some known functions */
unsigned char EnableRegVars	= 0;   	/* Enable register variables */
unsigned char AllowRegVarAddr	= 0;	/* Allow taking addresses of register vars */
unsigned char RegVarsToCallStack= 0;   	/* Save reg variables on call stack */
unsigned char StaticLocals	= 0;   	/* Make local variables static */
unsigned char SignedChars	= 0;	/* Make characters signed by default */
unsigned char AddSource		= 0; 	/* Add source lines as comments */
unsigned char DebugInfo		= 0;	/* Add debug info to the obj */
unsigned char Debug		= 0;	/* Debug mode */
unsigned char CreateDep		= 0;	/* Create a dependency file */
unsigned char CheckStack	= 0;	/* Generate stack overflow checks */




