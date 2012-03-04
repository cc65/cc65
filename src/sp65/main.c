/*****************************************************************************/
/*                                                                           */
/*				    main.c				     */
/*                                                                           */
/*            Main program of the sp65 sprite and bitmap utility             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2012,      Ullrich von Bassewitz                                      */
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* common */
#include "cmdline.h"
#include "print.h"
#include "version.h"

/* sp65 */
#include "attr.h"
#include "error.h"
#include "input.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Bitmap first read */
static Bitmap* B;

/* Bitmap last processed */
static Bitmap* C;



/*****************************************************************************/
/*     	       	     	       	     Code			  	     */
/*****************************************************************************/



static void Usage (void)
/* Print usage information and exit */
{
    printf (
    	    "Usage: %s [options] file [options] [file]\n"
    	    "Short options:\n"
       	    "  -V\t\t\tPrint the version number and exit\n"
       	    "  -h\t\t\tHelp (this text)\n"
            "  -v\t\t\tIncrease verbosity\n"
	    "\n"
	    "Long options:\n"
    	    "  --help\t\tHelp (this text)\n"
            "  --verbose\t\tIncrease verbosity\n"
       	    "  --version\t\tPrint the version number and exit\n",
    	    ProgName);
}



static void OptHelp (const char* Opt attribute ((unused)),
		     const char* Arg attribute ((unused)))
/* Print usage information and exit */
{
    Usage ();
    exit (EXIT_SUCCESS);
}



static void OptRead (const char* Opt, const char* Arg)
/* Read an input file */
{
    static const char* NameList[] = {
        "name", "format"
    };


    /* Parse the argument */
    Collection* C = ParseAttrList (Arg, NameList, 2);

    /* Must have a file name given */
    const char* FileName = NeedAttrVal (C, "name", Opt);

    /* Determine the format of the input file */
    int IF = ifAuto;
    const char* Format = GetAttrVal (C, "format");
    if (Format != 0) {
        IF = FindInputFormat (Format);
        if (IF <= 0) {
            Error ("Unknown input format `%s'", Format);
        }
    }

    /* Read the file */
    B = ReadInputFile (FileName, IF);
}



static void OptVerbose (const char* Opt attribute ((unused)),
		       	const char* Arg attribute ((unused)))
/* Increase versbosity */
{
    ++Verbosity;
}



static void OptVersion (const char* Opt attribute ((unused)),
			const char* Arg attribute ((unused)))
/* Print the assembler version */
{
    fprintf (stderr,
       	     "%s V%s - (C) Copyright 2012, Ullrich von Bassewitz\n",
       	     ProgName, GetVersionAsString ());
}



int main (int argc, char* argv [])
/* sp65 main program */
{
    /* Program long options */
    static const LongOpt OptTab[] = {
	{ "--help",    		0,	OptHelp			},
        { "--read",             1,      OptRead                 },
       	{ "--verbose",          0,      OptVerbose              },
	{ "--version", 	       	0,	OptVersion		},
    };

    unsigned I;

    /* Initialize the cmdline module */
    InitCmdLine (&argc, &argv, "sp65");

    /* Check the parameters */
    I = 1;
    while (I < ArgCount) {

       	/* Get the argument */
       	const char* Arg = ArgVec[I];

       	/* Check for an option */
       	if (Arg [0] == '-') {
       	    switch (Arg [1]) {

	 	case '-':
	 	    LongOption (&I, OptTab, sizeof(OptTab)/sizeof(OptTab[0]));
	 	    break;

       	        case 'V':
    		    OptVersion (Arg, 0);
       		    break;

	 	case 'h':
	 	    OptHelp (Arg, 0);
	 	    break;

                case 'r':
                    OptRead (Arg, GetArg (&I, 2));
                    break;

	 	case 'v':
	 	    OptVerbose (Arg, 0);
	 	    break;

       	       	default:
       	       	    UnknownOption (Arg);
		    break;

     	    }
       	} else {
    	    /* #### Testing */
       	    ReadInputFile (Arg, ifAuto);
     	}

	/* Next argument */
	++I;
    }

    /* Success */
    return EXIT_SUCCESS;
}



