/*****************************************************************************/
/*                                                                           */
/*				   global.h				     */
/*                                                                           */
/*		   Global variables for the cc65 C compiler		     */
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



#ifndef GLOBAL_H
#define GLOBAL_H



/*****************************************************************************/
/*				     Data				     */
/*****************************************************************************/



extern unsigned char	ANSI;			/* Strict ANSI flag */
extern unsigned char	WriteableStrings;	/* Literal strings are r/w */
extern unsigned char	NoWarn;			/* Suppress warnings */
extern unsigned char	Optimize;		/* Optimize flag */
extern unsigned char	FavourSize;		/* Favour size over speed */
extern unsigned char    InlineStdFuncs;	       	/* Inline some known functions */
extern unsigned char	EnableRegVars;		/* Enable register variables */
extern unsigned char	AllowRegVarAddr;	/* Allow taking addresses of register vars */
extern unsigned char	RegVarsToCallStack;	/* Save reg variables on call stack */
extern unsigned char   	StaticLocals;		/* Make local variables static */
extern unsigned char	SignedChars;		/* Make characters signed by default */
extern unsigned char	AddSource;		/* Add source lines as comments */
extern unsigned char	DebugInfo;		/* Add debug info to the obj */
extern unsigned char	Debug;			/* Debug mode */
extern unsigned char	CreateDep;		/* Create a dependency file */
extern unsigned char	CheckStack;		/* Generate stack overflow checks */


/* End of global.h */

#endif




