/*****************************************************************************/
/*                                                                           */
/*				   global.h				     */
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



#ifndef GLOBAL_H
#define GLOBAL_H



/*****************************************************************************/
/*     	      	    		     Data			    	     */
/*****************************************************************************/



extern const char*	ProgName;	/* Program name */

/* File names */
extern const char*	InFile;		/* Name of input file */
extern const char*	OutFile;	/* Name of output file */
extern const char*	ListFile;	/* Name of listing file */

/* Default extensions */
extern const char      	ObjExt[];      	/* Default object extension */
extern const char	ListExt[]; 	/* Default listing extension */

extern char		LocalStart;	/* This char starts local symbols */

extern unsigned char	IgnoreCase;	/* Ignore case on identifiers? */
extern unsigned char	AutoImport;	/* Mark unresolveds as import */
extern unsigned char	Verbose;    	/* Verbose operation flag */
extern unsigned char	SmartMode;	/* Smart mode */
extern unsigned char   	DbgSyms;	/* Add debug symbols */
extern unsigned char	Listing;        /* Create listing file */
extern unsigned char	LineCont;	/* Allow line continuation */

/* Emulation features */
extern unsigned char   	DollarIsPC;	/* Allow the $ symbol as current PC */
extern unsigned char	NoColonLabels;	/* Allow labels without a colon */
extern unsigned char	LooseStringTerm;/* Allow ' as string terminator */
extern unsigned char    AtInIdents;	/* Allow '@' in identifiers */
extern unsigned char	DollarInIdents;	/* Allow '$' in identifiers */



/* End of global.h */

#endif



