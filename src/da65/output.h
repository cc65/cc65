/*****************************************************************************/
/*                                                                           */
/*				   output.h				     */
/*                                                                           */
/*			 Disassembler output routines			     */
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



#ifndef OUTPUT_H
#define OUTPUT_H



/* common */
#include "attrib.h"



/*****************************************************************************/
/*	      			     Code				     */
/*****************************************************************************/



void OpenOutput (const char* Name);
/* Open the given file for output */

void CloseOutput (void);
/* Close the output file */

void Output (const char* Format, ...) attribute ((format(printf, 1, 2)));
/* Write to the output file */

void Indent (unsigned N);
/* Make sure the current line column is at position N (zero based) */

void LineFeed (void);
/* Add a linefeed to the output file */

void DefLabel (const char* Name);
/* Define a label with the given name */

void OneDataByte (void);
/* Output a .byte line with the current code byte */

void SeparatorLine (void);
/* Print a separator line */



/* End of output.h */
#endif




