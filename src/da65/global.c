/*****************************************************************************/
/*                                                                           */
/*				   global.c				     */
/*                                                                           */
/*		  Global variables for the da65 disassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2006 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
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
/*     	      	    		     Data				     */
/*****************************************************************************/



/* File names */
const char* InFile     	      = 0;      /* Name of input file */
const char* OutFile    	      = 0;      /* Name of output file */

/* Default extensions */
const char OutExt[]	      = ".dis";	/* Output file extension */
const char CfgExt[]    	      = ".cfg";	/* Config file extension */

/* Flags and other command line stuff */
unsigned char DebugInfo       = 0;      /* Add debug info to the object file */
unsigned char FormFeeds	      = 0;  	/* Add form feeds to the output? */
unsigned char UseHexOffs      = 0;      /* Use hexadecimal label offsets */
unsigned char PassCount	      = 2;  	/* How many passed do we do? */
long 	      StartAddr	      = -1L; 	/* Start/load address of the program */
long          InputOffs       = -1L;    /* Offset into input file */
long          InputSize       = -1L;    /* Number of bytes to read from input */

/* Stuff needed by many routines */
unsigned      Pass	      = 0;	/* Disassembler pass */
char          Now[128];                 /* Current time as string */

/* Comments */
unsigned      Comments        = 0;      /* Add which comments to the output? */

/* Page formatting */
unsigned PageLength    	      = 0;	/* Length of a listing page */
unsigned LBreak               = 7;      /* Linefeed if labels exceed this limit */
unsigned MIndent	      = 9;	/* Mnemonic indent */
unsigned AIndent	      = 17;	/* Argument indent */
unsigned CIndent	      = 49;	/* Comment indent */
unsigned TIndent	      = 81;	/* Text bytes indent */
unsigned BytesPerLine	      = 8;	/* Max. number of data bytes per line */



