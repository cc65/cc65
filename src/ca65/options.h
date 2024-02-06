/*****************************************************************************/
/*                                                                           */
/*                                 options.h                                 */
/*                                                                           */
/*              Object file options for the ca65 macroassembler              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2008, Ullrich von Bassewitz                                      */
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



#ifndef OPTIONS_H
#define OPTIONS_H



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void OptStr (unsigned char Type, const StrBuf* Text);
/* Add a string option */

void OptComment (const StrBuf* Comment);
/* Add a comment */

void OptAuthor (const StrBuf* Author);
/* Add an author statement */

void OptTranslator (const StrBuf* Translator);
/* Add a translator option */

void OptCompiler (const StrBuf* Compiler);
/* Add a compiler option */

void OptOS (const StrBuf* OS);
/* Add an operating system option */

void OptDateTime (unsigned long DateTime);
/* Add a date/time option */

void WriteOptions (void);
/* Write the options to the object file */



/* End of options.h */

#endif
