/*****************************************************************************/
/*                                                                           */
/*				    macro.h				     */
/*                                                                           */
/*		      Macros for the ca65 macroassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2011, Ullrich von Bassewitz                                      */
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



#ifndef MACRO_H
#define MACRO_H



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



struct StrBuf;



/*****************************************************************************/
/*			    	     Data				     */
/*****************************************************************************/



/* Macro styles */
#define MAC_STYLE_CLASSIC   	0
#define MAC_STYLE_DEFINE    	1



/*****************************************************************************/
/*     	       	    	    	     Code				     */
/*****************************************************************************/



void MacDef (unsigned Style);
/* Parse a macro definition */

void MacUndef (const StrBuf* Name, unsigned char Style);
/* Undefine the macro with the given name and style. A style mismatch is
 * treated as if the macro didn't exist.
 */

void MacExpandStart (void);
/* Start expanding the macro in SVal */

void MacAbort (void);
/* Abort the current macro expansion */

int IsMacro (const StrBuf* Name);
/* Return true if the given name is the name of a macro */

int IsDefine (const StrBuf* Name);
/* Return true if the given name is the name of a define style macro */

int InMacExpansion (void);
/* Return true if we're currently expanding a macro */

void DisableDefineStyleMacros (void);
/* Disable define style macros until EnableDefineStyleMacros is called */

void EnableDefineStyleMacros (void);
/* Re-enable define style macros previously disabled with
 * DisableDefineStyleMacros.
 */



/* End of macro.h */

#endif



