/*****************************************************************************/
/*                                                                           */
/*				   options.c				     */
/*                                                                           */
/*		Object file options for the ca65 macroassembler		     */
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



#include <string.h>

/* common */
#include "optdefs.h"
#include "xmalloc.h"

/* ca65 */
#include "error.h"
#include "objfile.h"
#include "options.h"



/*****************************************************************************/
/*     	      	    		     Data				     */
/*****************************************************************************/



/* Option list */
static Option*	     	OptRoot = 0;
static Option*	     	OptLast = 0;
static unsigned		OptCount = 0;



/*****************************************************************************/
/*     	      	     		     Code				     */
/*****************************************************************************/



static Option* NewOption (unsigned char Type)
/* Create a new option, insert it into the list and return it */
{
    Option* Opt;

    /* Allocate memory */
    Opt = xmalloc (sizeof (*Opt));

    /* Initialize fields */
    Opt->Next  = 0;
    Opt->Type  = Type;
    Opt->V.Str = 0;

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



void OptStr (unsigned char Type, const char* Text)
/* Add a string option */
{
    Option* O;

    /* String must have less than 255 bytes */
    if (strlen (Text) > 255) {
	Fatal (FAT_STRING_TOO_LONG);
    }
    O        = NewOption (Type);
    O->V.Str = xstrdup (Text);
}



void OptComment (const char* Comment)
/* Add a comment */
{
    OptStr (OPT_COMMENT, Comment);
}



void OptAuthor (const char* Author)
/* Add an author statement */
{
    OptStr (OPT_AUTHOR, Author);
}



void OptTranslator (const char* Translator)
/* Add a translator option */
{
    OptStr (OPT_TRANSLATOR, Translator);
}



void OptCompiler (const char* Compiler)
/* Add a compiler option */
{
    OptStr (OPT_COMPILER, Compiler);
}



void OptOS (const char* OS)
/* Add an operating system option */
{
    OptStr (OPT_OS, OS);
}



void OptDateTime (unsigned long DateTime)
/* Add a date/time option */
{
    Option* O = NewOption (OPT_DATETIME);
    O->V.Val = DateTime;
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

	/* Write the type of the option */
	ObjWrite8 (O->Type);

	/* Write the argument */
	switch (O->Type & OPT_ARGMASK) {

	    case OPT_ARGSTR:
	       	ObjWriteStr (O->V.Str);
	       	break;

	    case OPT_ARGNUM:
	       	ObjWrite32 (O->V.Val);
	       	break;

	    default:
	       	Internal ("Invalid option type: $%02X", O->Type & 0xFF);

	}

	/* Next option */
	O = O->Next;

    }

    /* Done writing options */
    ObjEndOptions ();
}



