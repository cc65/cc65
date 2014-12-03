/*****************************************************************************/
/*                                                                           */
/*                                 output.h                                  */
/*                                                                           */
/*                       Disassembler output routines                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2014, Ullrich von Bassewitz                                      */
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



#ifndef OUTPUT_H
#define OUTPUT_H



/* common */
#include "attrib.h"



/*****************************************************************************/
/*                                   Code                                    */
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

void DefForward (const char* Name, const char* Comment, unsigned Offs);
/* Define a label as "* + x", where x is the offset relative to the
** current PC.
*/

void DefConst (const char* Name, const char* Comment, unsigned Addr);
/* Define an address constant */

void OneDataByte (void);
/* Output a .byte line with the current code byte */

void DataByteLine (unsigned ByteCount);
/* Output a line with bytes */

void DataDByteLine (unsigned ByteCount);
/* Output a line with dbytes */

void DataWordLine (unsigned ByteCount);
/* Output a line with words */

void DataDWordLine (unsigned ByteCount);
/* Output a line with dwords */

void SeparatorLine (void);
/* Print a separator line */

void StartSegment (const char* Name, unsigned AddrSize);
/* Start a segment */

void EndSegment (void);
/* End a segment */

void UserComment (const char* Comment);
/* Output a comment line */

void LineComment (unsigned PC, unsigned Count);
/* Add a line comment with the PC and data bytes */

void OutputSettings (void);
/* Output CPU and other settings */



/* End of output.h */
#endif
