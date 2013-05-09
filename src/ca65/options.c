/*****************************************************************************/
/*                                                                           */
/*                                 options.c                                 */
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



#include <string.h>

/* common */
#include "optdefs.h"
#include "xmalloc.h"

/* ca65 */
#include "error.h"
#include "objfile.h"
#include "options.h"
#include "spool.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Option list */
static Option*          OptRoot = 0;
static Option*          OptLast = 0;
static unsigned         OptCount = 0;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static Option* NewOption (unsigned char Type, unsigned long Val)
/* Create a new option, insert it into the list and return it */
{
    Option* Opt;

    /* Allocate memory */
    Opt = xmalloc (sizeof (*Opt));

    /* Initialize fields */
    Opt->Next  = 0;
    Opt->Type  = Type;
    Opt->Val   = Val;

    /* Insert it into the list */
    if (OptRoot == 0) {
        OptRoot = Opt;
    } else {
        OptLast->Next = Opt;
    }
    OptLast = Opt;

    /* One more option now */
    ++OptCount;

    /* Return the new struct */
    return Opt;
}



void OptStr (unsigned char Type, const StrBuf* Text)
/* Add a string option */
{
    NewOption (Type, GetStrBufId (Text));
}



void OptComment (const StrBuf* Comment)
/* Add a comment */
{
    NewOption (OPT_COMMENT, GetStrBufId (Comment));
}



void OptAuthor (const StrBuf* Author)
/* Add an author statement */
{
    NewOption (OPT_AUTHOR, GetStrBufId (Author));
}



void OptTranslator (const StrBuf* Translator)
/* Add a translator option */
{
    NewOption (OPT_TRANSLATOR, GetStrBufId (Translator));
}



void OptCompiler (const StrBuf* Compiler)
/* Add a compiler option */
{
    NewOption (OPT_COMPILER, GetStrBufId (Compiler));
}



void OptOS (const StrBuf* OS)
/* Add an operating system option */
{
    NewOption (OPT_OS, GetStrBufId (OS));
}



void OptDateTime (unsigned long DateTime)
/* Add a date/time option */
{
    NewOption (OPT_DATETIME, DateTime);
}



void WriteOptions (void)
/* Write the options to the object file */
{
    Option* O;

    /* Tell the object file module that we're about to start the options */
    ObjStartOptions ();

    /* Write the option count */
    ObjWriteVar (OptCount);

    /* Walk through the list and write the options */
    O = OptRoot;
    while (O) {

        /* Write the type of the option, then the value */
        ObjWrite8 (O->Type);
        ObjWriteVar (O->Val);

        /* Next option */
        O = O->Next;

    }

    /* Done writing options */
    ObjEndOptions ();
}



                        
