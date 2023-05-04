/*****************************************************************************/
/*                                                                           */
/*                                 global.c                                  */
/*                                                                           */
/*               Global variables for the ca65 macroassembler                */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2013, Ullrich von Bassewitz                                      */
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



/* common */
#include "addrsize.h"

/* ca65 */
#include "global.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* File names */
const char* InFile               = 0;   /* Name of input file */
const char* OutFile              = 0;   /* Name of output file */
StrBuf ListingName = STATIC_STRBUF_INITIALIZER; /* Name of listing file */
StrBuf DepName     = STATIC_STRBUF_INITIALIZER; /* Dependency file */
StrBuf FullDepName = STATIC_STRBUF_INITIALIZER; /* Full dependency file */

/* Default extensions */
const char ObjExt[]              = ".o";/* Default object extension */

char LocalStart                  = '@'; /* This char starts local symbols */

unsigned char IgnoreCase         = 0;   /* Ignore case on identifiers? */
unsigned char AutoImport         = 0;   /* Mark unresolveds as import */
unsigned char SmartMode          = 0;   /* Smart mode */
unsigned char DbgSyms            = 0;   /* Add debug symbols */
unsigned char LineCont           = 0;   /* Allow line continuation */
unsigned char LargeAlignment     = 0;   /* Don't warn about large alignments */
unsigned char RelaxChecks        = 0;   /* Relax a few assembler checks */
unsigned char StringEscapes      = 0;   /* Allow C-style escapes in strings */
unsigned char LongJsrJmpRts      = 0;   /* Allow JSR/JMP/RTS as alias for JSL/JML/RTL */
unsigned char WarningsAsErrors   = 0;   /* Error if any warnings */

/* Emulation features */
unsigned char DollarIsPC         = 0;   /* Allow the $ symbol as current PC */
unsigned char NoColonLabels      = 0;   /* Allow labels without a colon */
unsigned char LooseStringTerm    = 0;   /* Allow ' as string terminator */
unsigned char LooseCharTerm      = 0;   /* Allow " for char constants */
unsigned char AtInIdents         = 0;   /* Allow '@' in identifiers */
unsigned char DollarInIdents     = 0;   /* Allow '$' in identifiers */
unsigned char LeadingDotInIdents = 0;   /* Allow '.' to start an identifier */
unsigned char PCAssignment       = 0;   /* Allow "* = $XXX" or "$ = $XXX" */
unsigned char MissingCharTerm    = 0;   /* Allow lda #'a (no closing term) */
unsigned char UbiquitousIdents   = 0;   /* Allow ubiquitous identifiers */
unsigned char OrgPerSeg          = 0;   /* Make .org local to current seg */
unsigned char CComments          = 0;   /* Allow C like comments */
unsigned char ForceRange         = 0;   /* Force values into expected range */
unsigned char UnderlineInNumbers = 0;   /* Allow underlines in numbers */
unsigned char AddrSize           = 0;   /* Allow .ADDRSIZE function */
unsigned char BracketAsIndirect  = 0;   /* Use '[]' not '()' for indirection */
