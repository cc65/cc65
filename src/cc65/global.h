/*****************************************************************************/
/*                                                                           */
/*				   global.h				     */
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



#ifndef GLOBAL_H
#define GLOBAL_H



/* common */
#include "intstack.h"



/*****************************************************************************/
/*				     Data				     */
/*****************************************************************************/



/* Options */
extern unsigned char	AddSource;		/* Add source lines as comments */
extern unsigned char	DebugInfo;		/* Add debug info to the obj */
extern unsigned char	CreateDep;		/* Create a dependency file */
extern unsigned char    PreprocessOnly;         /* Just preprocess the input */
extern unsigned         RegisterSpace;          /* Space available for register vars */

/* Stackable options */
extern IntStack         WarnDisable;            /* Suppress warnings */
extern IntStack         WritableStrings;	/* Literal strings are r/w */
extern IntStack         InlineStdFuncs;	       	/* Inline some known functions */
extern IntStack         EnableRegVars;		/* Enable register variables */
extern IntStack         AllowRegVarAddr;	/* Allow taking addresses of register vars */
extern IntStack         RegVarsToCallStack;	/* Save reg variables on call stack */
extern IntStack         StaticLocals;		/* Make local variables static */
extern IntStack         SignedChars;		/* Make characters signed by default */
extern IntStack         CheckStack;		/* Generate stack overflow checks */
extern IntStack         Optimize;		/* Optimize flag */
extern IntStack         CodeSizeFactor;		/* Size factor for generated code */



/* End of global.h */

#endif




