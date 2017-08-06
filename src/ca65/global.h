/*****************************************************************************/
/*                                                                           */
/*                                 global.h                                  */
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



#ifndef GLOBAL_H
#define GLOBAL_H



/* common */
#include "strbuf.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* File names */
extern const char*      InFile;             /* Name of input file */
extern const char*      OutFile;            /* Name of output file */
extern StrBuf           ListingName;        /* Name of listing file */
extern StrBuf           DepName;            /* Name of dependencies file */
extern StrBuf           FullDepName;        /* Name of full dependencies file */

/* Default extensions */
extern const char       ObjExt[];           /* Default object extension */

extern char             LocalStart;         /* This char starts local symbols */

extern unsigned char    IgnoreCase;         /* Ignore case on identifiers? */
extern unsigned char    AutoImport;         /* Mark unresolveds as import */
extern unsigned char    SmartMode;          /* Smart mode */
extern unsigned char    DbgSyms;            /* Add debug symbols */
extern unsigned char    LineCont;           /* Allow line continuation */
extern unsigned char    LargeAlignment;     /* Don't warn about large alignments */
extern unsigned char    RelaxChecks;        /* Relax a few assembler checks */

/* Emulation features */
extern unsigned char    DollarIsPC;         /* Allow the $ symbol as current PC */
extern unsigned char    NoColonLabels;      /* Allow labels without a colon */
extern unsigned char    LooseStringTerm;    /* Allow ' as string terminator */
extern unsigned char    LooseCharTerm;      /* Allow " for char constants */
extern unsigned char    AtInIdents;         /* Allow '@' in identifiers */
extern unsigned char    DollarInIdents;     /* Allow '$' in identifiers */
extern unsigned char    LeadingDotInIdents; /* Allow '.' to start an identifier */
extern unsigned char    PCAssignment;       /* Allow "* = $XXX" or "$ = $XXX" */
extern unsigned char    MissingCharTerm;    /* Allow lda #'a (no closing term) */
extern unsigned char    UbiquitousIdents;   /* Allow ubiquitous identifiers */
extern unsigned char    OrgPerSeg;          /* Make .org local to current seg */
extern unsigned char    CComments;          /* Allow C like comments */
extern unsigned char    ForceRange;         /* Force values into expected range */
extern unsigned char    UnderlineInNumbers; /* Allow underlines in numbers */
extern unsigned char    AddrSize;           /* Allow .ADDRSIZE function */
extern unsigned char    BracketAsIndirect;  /* Use '[]' not '()' for indirection */




/* End of global.h */

#endif
