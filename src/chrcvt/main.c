/*****************************************************************************/
/*                                                                           */
/*				    main.c				     */
/*                                                                           */
/*             Main program of the chrcvt vector font converter              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2009, Ullrich von Bassewitz                                      */
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
#include "version.h"

/* chrcvt */
#include "error.h"



/*****************************************************************************/
/*     	       	     	       	     Data			  	     */
/*****************************************************************************/



static unsigned FilesProcessed = 0;



/*****************************************************************************/
/*     	       	     	       	     Code			  	     */
/*****************************************************************************/



static void Usage (void)
/* Print usage information and exit */
{
    fprintf (stderr,
    	     "Usage: %s [options] file [options] [file]\n"
    	     "Short options:\n"
       	     "  -h\t\t\tHelp (this text)\n"
       	     "  -H\t\t\tDump the object file header\n"
       	     "  -S\t\t\tDump segments sizes\n"
       	     "  -V\t\t\tPrint the version number and exit\n"
	     "\n"
	     "Long options:\n"
	     "  --dump-all\t\tDump all object file information\n"
	     "  --dump-dbgsyms\tDump debug symbols\n"
       	     "  --dump-exports\tDump exported symbols\n"
	     "  --dump-files\t\tDump the source files\n"
	     "  --dump-header\t\tDump the object file header\n"
	     "  --dump-imports\tDump imported symbols\n"
	     "  --dump-lineinfo\tDump line information\n"
	     "  --dump-options\tDump object file options\n"
	     "  --dump-segments\tDump the segments in the file\n"
       	     "  --dump-segsize\tDump segments sizes\n"
	     "  --help\t\tHelp (this text)\n"
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



static void OptVersion (const char* Opt attribute ((unused)),
			const char* Arg attribute ((unused)))
/* Print the assembler version */
{
    fprintf (stderr,
       	     "%s V%s - (C) Copyright 2009, Ullrich von Bassewitz\n",
       	     ProgName, GetVersionAsString ());
}



static void ConvertFile (const char* Name)
/* Convert one vector font file */
{
    /* Try to open the file */
    FILE* F = fopen (Name, "rb");
    if (F == 0) {
	Error ("Cannot open `%s': %s", Name, strerror (errno));
    }

    /* Close the file */
    fclose (F);
}



int main (int argc, char* argv [])
/* Assembler main program */
{
    /* Program long options */
    static const LongOpt OptTab[] = {
	{ "--help",    		0,	OptHelp			},
	{ "--version", 	       	0,	OptVersion		},
    };

    unsigned I;

    /* Initialize the cmdline module */
    InitCmdLine (&argc, &argv, "chrcvt");

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

	 	case 'h':
	 	    OptHelp (Arg, 0);
	 	    break;

       	        case 'V':
    		    OptVersion (Arg, 0);
       		    break;

       	       	default:
       	       	    UnknownOption (Arg);
		    break;

     	    }
       	} else {
    	    /* Filename. Dump it. */
	    ConvertFile (Arg);
	    ++FilesProcessed;
     	}

	/* Next argument */
	++I;
    }

    /* Print a message if we did not process any files */
    if (FilesProcessed == 0) {
	fprintf (stderr, "%s: No input files\n", ProgName);
    }

    /* Success */
    return EXIT_SUCCESS;
}



