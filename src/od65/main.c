/*****************************************************************************/
/*                                                                           */
/*				    main.c				     */
/*                                                                           */
/*	       Main program of the od65 object file dump utility	     */
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* common */
#include "cmdline.h"
#include "objdefs.h"
#include "version.h"

/* od65 */
#include "dump.h"
#include "error.h"
#include "fileio.h"
#include "global.h"



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
    	     "Usage: %s [options] file\n"
    	     "Short options:\n"
       	     "  -h\t\t\tHelp (this text)\n"
       	     "  -V\t\t\tPrint the version number and exit\n"
	     "\n"
	     "Long options:\n"
	     "  --dump-files\t\tDump the source files\n"
	     "  --dump-header\t\tDump the object file header\n"
	     "  --dump-options\t\tDump object file options\n"
	     "  --help\t\tHelp (this text)\n"
       	     "  --version\t\tPrint the version number and exit\n",
    	     ProgName);
}



static void OptDumpFiles (const char* Opt, const char* Arg)
/* Dump the source files */
{
    What |= D_FILES;
}



static void OptDumpHeader (const char* Opt, const char* Arg)
/* Dump the object file header */
{
    What |= D_HEADER;
}



static void OptDumpOptions (const char* Opt, const char* Arg)
/* Dump the object file options */
{
    What |= D_OPTIONS;
}



static void OptHelp (const char* Opt, const char* Arg)
/* Print usage information and exit */
{
    Usage ();
    exit (EXIT_SUCCESS);
}



static void OptVersion (const char* Opt, const char* Arg)
/* Print the assembler version */
{
    fprintf (stderr,
       	     "%s V%u.%u.%u - (C) Copyright 2000 Ullrich von Bassewitz\n",
       	     ProgName, VER_MAJOR, VER_MINOR, VER_PATCH);
}



static void DumpFile (const char* Name)
/* Dump information from the named file */
{
    unsigned long Magic;

    /* Try to open the file */
    FILE* F = fopen (Name, "rb");
    if (F == 0) {
	Warning ("Cannot open `%s': %s", Name, strerror (errno));
    }

    /* Read the magic word */
    Magic = Read32 (F);

    /* Do we know this type of file? */
    if (Magic != OBJ_MAGIC) {

	/* Unknown format */
       	printf ("%s: (no x65 object file)\n", Name);

    } else if (What == 0) {

	/* Special handling if no info was requested */
     	printf ("%s: (no information requested)\n", Name);

    } else {

     	/* Print the filename */
     	printf ("%s:\n", Name);

     	/* Check what to dump */
     	if (What & D_HEADER) {
     	    DumpObjHeader (F, 0);
     	}
	if (What & D_OPTIONS) {
	    DumpObjOptions (F, 0);
	}
	if (What & D_FILES) {
	    DumpObjFiles (F, 0);
	}
    }

    /* Close the file */
    fclose (F);
}



int main (int argc, char* argv [])
/* Assembler main program */
{
    /* Program long options */
    static const LongOpt OptTab[] = {
	{ "--dump-files",	0,	OptDumpFiles		},
	{ "--dump-header",	0,	OptDumpHeader		},
	{ "--dump-options",	0,	OptDumpOptions		},
	{ "--help",		0,	OptHelp			},
	{ "--version",	       	0,	OptVersion		},
    };

    int I;

    /* Initialize the cmdline module */
    InitCmdLine (argc, argv, "od65");

    /* Check the parameters */
    I = 1;
    while (I < argc) {

       	/* Get the argument */
       	const char* Arg = argv [I];

       	/* Check for an option */
       	if (Arg [0] == '-') {
       	    switch (Arg [1]) {

		case '-':
		    LongOption (&I, OptTab, sizeof(OptTab)/sizeof(OptTab[0]));
		    break;

		case 'h':
		    OptHelp (Arg, 0);
		    break;

		case 'H':
		    OptDumpHeader (Arg, 0);
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
	    DumpFile (Arg);
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



