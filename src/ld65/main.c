/*****************************************************************************/
/*									     */
/*				    main.c				     */
/*									     */
/*		       Main program for the ld65 linker			     */
/*									     */
/*									     */
/*									     */
/* (C) 1998-2000 Ullrich von Bassewitz					     */
/*		 Wacholderweg 14					     */
/*		 D-70597 Stuttgart					     */
/* EMail:	 uz@musoftware.de					     */
/*									     */
/*									     */
/* This software is provided 'as-is', without any expressed or implied	     */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.				     */
/*									     */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:			     */
/*									     */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.					     */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.			     */
/* 3. This notice may not be removed or altered from any source		     */
/*    distribution.							     */
/*									     */
/*****************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* common */
#include "cmdline.h"
#include "libdefs.h"
#include "objdefs.h"
#include "target.h"
#include "version.h"
#include "xmalloc.h"

/* ld65 */
#include "binfmt.h"
#include "config.h"
#include "error.h"
#include "exports.h"
#include "fileio.h"
#include "global.h"
#include "library.h"
#include "mapfile.h"
#include "objfile.h"
#include "scanner.h"
#include "segments.h"
#include "tgtcfg.h"



/*****************************************************************************/
/*				     Data				     */
/*****************************************************************************/



static unsigned		ObjFiles   = 0; /* Count of object files linked */
static unsigned		LibFiles   = 0; /* Count of library files linked */
static const char*	LibPath    = 0; /* Search path for modules */
static unsigned		LibPathLen = 0; /* Length of LibPath */



/*****************************************************************************/
/*				     Code				     */
/*****************************************************************************/



static void Usage (void)
/* Print usage information and exit */
{
    fprintf (stderr,
	     "Usage: %s [options] module ...\n"
    	     "Short options:\n"
       	     "  -h\t\t\tHelp (this text)\n"
       	     "  -m name\t\tCreate a map file\n"
       	     "  -o name\t\tName the default output file\n"
       	     "  -t sys\t\tSet the target system\n"
       	     "  -v\t\t\tVerbose mode\n"
       	     "  -vm\t\t\tVerbose map file\n"
       	     "  -C name\t\tUse linker config file\n"
       	     "  -Ln name\t\tCreate a VICE label file\n"
       	     "  -Lp\t\t\tMark write protected segments as such (VICE)\n"
       	     "  -S addr\t\tSet the default start address\n"
       	     "  -V\t\t\tPrint the linker version\n"
	     "\n"
	     "Long options:\n"
	     "  --help\t\tHelp (this text)\n"
	     "  --mapfile name\tCreate a map file\n"
       	     "  --start-addr addr\tSet the default start address\n"
       	     "  --target sys\t\tSet the target system\n"
       	     "  --version\t\tPrint the linker version\n",
	     ProgName);
}



static unsigned long CvtNumber (const char* Arg, const char* Number)
/* Convert a number from a string. Allow '$' and '0x' prefixes for hex
 * numbers.
 */
{
    unsigned long Val;
    int 	  Converted;

    /* Convert */
    if (*Number == '$') {
	++Number;
	Converted = sscanf (Number, "%lx", &Val);
    } else {
	Converted = sscanf (Number, "%li", (long*)&Val);
    }

    /* Check if we do really have a number */
    if (Converted != 1) {
       	Error ("Invalid number given in argument: %s\n", Arg);
    }

    /* Return the result */
    return Val;
}



static int HasPath (const char* Name)
/* Check if the given Name has a path component */
{
    return strchr (Name, '/') != 0 || strchr (Name, '\\') != 0;
}



static void LinkFile (const char* Name)
/* Handle one file */
{
    unsigned long Magic;
    unsigned Len;
    char* NewName = 0;

    /* Try to open the file */
    FILE* F = fopen (Name, "rb");
    if (F == 0) {
	/* We couldn't open the file. If the name doesn't have a path, and we
	 * have a search path given, try the name with the search path
	 * prepended.
	 */
	if (LibPathLen > 0 && !HasPath (Name)) {
	    /* Allocate memory. Account for the trailing zero, and for a
	     * path separator character eventually needed.
	     */
	    Len = LibPathLen;
	    NewName = xmalloc (strlen (Name) + Len + 2);
	    /* Build the new name */
	    memcpy (NewName, LibPath, Len);
	    if (NewName [Len-1] != '/' && NewName [Len-1] != '\\') {
		/* We need an additional path separator */
		NewName [Len++] = '/';
	    }
    	    strcpy (NewName + Len, Name);

	    /* Now try to open the new file */
	    F = fopen (NewName, "rb");
	}

	if (F == 0) {
	    Error ("Cannot open `%s': %s", Name, strerror (errno));
	}
    }

    /* Read the magic word */
    Magic = Read32 (F);

    /* Do we know this type of file? */
    switch (Magic) {

	case OBJ_MAGIC:
	    ObjAdd (F, Name);
	    ++ObjFiles;
	    break;

	case LIB_MAGIC:
	    LibAdd (F, Name);
	    ++LibFiles;
	    break;

	default:
	    fclose (F);
	    Error ("File `%s' has unknown type", Name);

    }

    /* If we have allocated memory, free it here. Note: Memory will not always
     * be freed if we run into an error, but that's no problem. Adding more
     * code to work around it will use more memory than the chunk that's lost.
     */
    xfree (NewName);
}



static void OptConfig (const char* Opt, const char* Arg)
/* Define the config file */
{
    if (CfgAvail ()) {
	Error ("Cannot use -C/-t twice");
    }
    CfgSetName (Arg);
}



static void OptHelp (const char* Opt, const char* Arg)
/* Print usage information and exit */
{
    Usage ();
    exit (EXIT_SUCCESS);
}



static void OptMapFile (const char* Opt, const char* Arg)
/* Give the name of the map file */
{
    MapFileName = Arg;
}



static void OptStartAddr (const char* Opt, const char* Arg)
/* Set the default start address */
{
    StartAddr = CvtNumber (Opt, Arg);
}



static void OptTarget (const char* Opt, const char* Arg)
/* Set the target system */
{
    const TargetDesc* D;

    /* Map the target name to a target id */
    Target = FindTarget (Arg);
    if (Target == TGT_UNKNOWN) {
       	Error ("Invalid target name: `%s'", Arg);
    }

    /* Get the target description record */
    D = &Targets[Target];

    /* Set the target data */
    DefaultBinFmt = D->BinFmt;
    CfgSetBuf (D->Cfg);
}



static void OptVersion (const char* Opt, const char* Arg)
/* Print the assembler version */
{
    fprintf (stderr,
       	     "ld65 V%u.%u.%u - (C) Copyright 1998-2000 Ullrich von Bassewitz\n",
	     VER_MAJOR, VER_MINOR, VER_PATCH);
}



int main (int argc, char* argv [])
/* Assembler main program */
{
    /* Program long options */
    static const LongOpt OptTab[] = {
       	{ "--config",  	       	1,     	OptConfig    		},
       	{ "--help",	       	0,     	OptHelp	     		},
	{ "--mapfile",		1,	OptMapFile		},
	{ "--start-addr",	1,	OptStartAddr		},
	{ "--target",		1,	OptTarget    		},
	{ "--version",	       	0,  	OptVersion   		},
    };

    int I;

    /* Initialize the cmdline module */
    InitCmdLine (argc, argv, "ld65");

    /* Evaluate the CC65_LIB environment variable */
    LibPath = getenv ("CC65_LIB");
    if (LibPath == 0) {
	/* Use some default path */
#ifdef CC65_LIB
	LibPath = CC65_LIB;
#else
     	LibPath = "/usr/lib/cc65/lib/";
#endif
    }
    LibPathLen = strlen (LibPath);

    /* Check the parameters */
    I = 1;
    while (I < argc) {

	/* Get the argument */
	const char* Arg = argv [I];

	/* Check for an option */
	if (Arg [0] == '-') {

	    /* An option */
	    switch (Arg [1]) {

		case '-':
	       	    LongOption (&I, OptTab, sizeof(OptTab)/sizeof(OptTab[0]));
	       	    break;

	       	case 'm':
	       	    OptMapFile (Arg, GetArg (&I, 2));
	       	    break;

	       	case 'o':
	       	    OutputName = GetArg (&I, 2);
	       	    break;

	       	case 't':
	       	    if (CfgAvail ()) {
	       		Error ("Cannot use -C/-t twice");
	       	    }
	       	    OptTarget (Arg, GetArg (&I, 2));
	       	    break;

	       	case 'v':
	       	    switch (Arg [2]) {
	       	      	case 'm':   VerboseMap = 1; 	break;
    		      	case '\0':  ++Verbose;	    	break;
		      	default:    UnknownOption (Arg);
		    }
		    break;

		case 'C':
		    OptConfig (Arg, GetArg (&I, 2));
		    break;

		case 'L':
		    switch (Arg [2]) {
		      	case 'n': LabelFileName = GetArg (&I, 3); break;
		      	case 'p': WProtSegs = 1;    	      	  break;
		      	default:  UnknownOption (Arg);		  break;
		    }
		    break;

		case 'S':
		    OptStartAddr (Arg, GetArg (&I, 2));
		    break;

		case 'V':
		    OptVersion (Arg, 0);
		    break;

	       	default:
	       	    UnknownOption (Arg);
	       	    break;
	    }

	} else {

	    /* A filename */
	    LinkFile (Arg);

	}

	/* Next argument */
	++I;
    }

    /* Check if we had any object files */
    if (ObjFiles == 0) {
	Error ("No object files to link");
    }

    /* Check if we have a valid configuration */
    if (!CfgAvail ()) {
       	Error ("Memory configuration missing");
    }

    /* Read the config file */
    CfgRead ();

    /* Assign start addresses for the segments, define linker symbols */
    CfgAssignSegments ();

    /* Create the output file */
    CfgWriteTarget ();

    /* Check for segments not written to the output file */
    CheckSegments ();

    /* If requested, create a map file and a label file for VICE */
    if (MapFileName) {
	CreateMapFile ();
    }
    if (LabelFileName) {
	CreateLabelFile ();
    }

    /* Dump the data for debugging */
    if (Verbose > 1) {
	SegDump ();
    }

    /* Return an apropriate exit code */
    return EXIT_SUCCESS;
}




