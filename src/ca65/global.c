/*****************************************************************************/
/*                                                                           */
/*				   global.c				     */
/*                                                                           */
/*		 Global variables for the ca65 macroassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
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



#include "global.h"



/*****************************************************************************/
/*     	      	    		     Data				     */
/*****************************************************************************/



const char* ProgName   	      = "ca65"; /* Program name */

/* File names */
const char* InFile     	      = 0;      /* Name of input file */
const char* OutFile    	      = 0;      /* Name of output file */
const char* ListFile   	      = 0;     	/* Name of listing file */

/* Default extensions */
const char ObjExt[]    	      = ".o";  	/* Default object extension */
const char ListExt[]   	      = ".lst"; /* Default listing extension */

char LocalStart	              = '@';  	/* This char starts local symbols */

unsigned char IgnoreCase      = 0;    	/* Ignore case on identifiers? */
unsigned char AutoImport      = 0;    	/* Mark unresolveds as import */
unsigned char Verbose         = 0;      /* Verbose operation flag */
unsigned char SmartMode	      = 0;    	/* Smart mode */
unsigned char DbgSyms	      = 0;    	/* Add debug symbols */
unsigned char Listing  	      = 0;    	/* Create listing file */
unsigned char LineCont	      = 0;    	/* Allow line continuation */

/* Emulation features */
unsigned char DollarIsPC      = 0;      /* Allow the $ symbol as current PC */
unsigned char NoColonLabels   = 0;     	/* Allow labels without a colon */
unsigned char LooseStringTerm = 0;	/* Allow ' as string terminator */
unsigned char AtInIdents      = 0;	/* Allow '@' in identifiers */
unsigned char DollarInIdents  = 0;  	/* Allow '$' in identifiers */
									  



