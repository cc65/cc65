/*****************************************************************************/
/*                                                                           */
/*				   global.h				     */
/*                                                                           */
/*		  Global variables for the da65 disassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000      Ullrich von Bassewitz                                       */
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



/* File names */
extern const char*	InFile;		/* Name of input file */
extern const char*	OutFile;	/* Name of output file */

/* Default extensions */
extern const char      	ObjExt[];      	/* Default object extension */
extern const char	ListExt[]; 	/* Default listing extension */

/* Flags and other command line stuff */
extern unsigned char	Verbose;	/* Verbosity of the output file */

/* Stuff needed by many routines */
extern unsigned 	Pass;		/* Disassembler pass */

/* Page formatting */
#define MIN_PAGE_LEN	32
#define MAX_PAGE_LEN	127
extern int	     	PageLength;	/* Length of a listing page */
extern unsigned		MIndent;	/* Mnemonic indent */
extern unsigned		AIndent;	/* Argument indent */
extern unsigned 	CIndent;	/* Comment indent */



/* End of global.h */

#endif



