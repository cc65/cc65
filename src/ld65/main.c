/*****************************************************************************/
/*									     */
/*				    main.c				     */
/*									     */
/*		       Main program for the ld65 linker			     */
/*									     */
/*									     */
/*									     */
/* (C) 1998	Ullrich von Bassewitz					     */
/*		Wacholderweg 14						     */
/*		D-70597 Stuttgart					     */
/* EMail:	uz@musoftware.de					     */
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

#include "../common/libdefs.h"
#include "../common/objdefs.h"
#include "../common/version.h"

#include "global.h"
#include "error.h"
#include "mem.h"
#include "target.h"
#include "fileio.h"
#include "scanner.h"
#include "config.h"
#include "objfile.h"
#include "library.h"
#include "exports.h"
#include "segments.h"
#include "mapfile.h"



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
	     "Options are:\n"
	     "\t-m name\t\tCreate a map file\n"
	     "\t-o name\t\tName the default output file\n"
	     "\t-t type\t\tType of target system\n"
	     "\t-v\t\tVerbose mode\n"
	     "\t-vm\t\tVerbose map file\n"
	     "\t-C name\t\tUse linker config file\n"
       	     "\t-Ln name\tCreate a VICE label file\n"
	     "\t-Lp\t\tMark write protected segments as such (VICE)\n"
	     "\t-S addr\t\tSet the default start address\n"
	     "\t-V\t\tPrint linker version\n",
	     ProgName);
    exit (EXIT_FAILURE);
}



static void UnknownOption (const char* Arg)
/* Print an error about an unknown option. Print usage information and exit */
{
    fprintf (stderr, "Unknown option: %s\n", Arg);
    Usage ();
}



static void InvNumber (const char* Arg)
/* Print an error about an unknown option. Print usage information and exit */
{
    fprintf (stderr, "Invalid number given in argument: %s\n", Arg);
    Usage ();
}



static unsigned long CvtNumber (const char* Arg, const char* Number)
/* Convert a number from a string. Allow '$' and '0x' prefixes for hex
 * numbers.
 */
{
    unsigned long Val;

    /* Convert */
    if (*Number == '$') {
	++Number;
	if (sscanf (Number, "%lx", &Val) != 1) {
	    InvNumber (Arg);
	}
    } else {
	if (sscanf (Number, "%li", (long*)&Val) != 1) {
	    InvNumber (Arg);
	}
    }

    /* Return the result */
    return Val;
}



static const char* GetArg (int* ArgNum, char* argv [], unsigned Len)
/* Get an option argument */
{
    const char* Arg = argv [*ArgNum];
    if (Arg [Len] != '\0') {
	/* Argument appended */
	return Arg + Len;
    } else {
	/* Separate argument */
	Arg = argv [*ArgNum + 1];
	if (Arg == 0) {
	    /* End of arguments */
	    fprintf (stderr, "Option requires an argument: %s\n", argv [*ArgNum]);
	    exit (EXIT_FAILURE);
	}
	++(*ArgNum);
	return Arg;
    }
}



static void LongOption (int* Arg, char* argv [])
/* Handle a long command line option */
{
    /* For now ... */
    UnknownOption (argv [*Arg]);
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
	    NewName = Xmalloc (strlen (Name) + Len + 2);
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
    Xfree (NewName);
}



int main (int argc, char* argv [])
/* Assembler main program */
{
    int I;

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
		    LongOption (&I, argv);
		    break;

		case 'm':
		    MapFileName = GetArg (&I, argv, 2);
		    break;

		case 'o':
		    OutputName = GetArg (&I, argv, 2);
		    break;

		case 't':
		    if (CfgAvail ()) {
			Error ("Cannot use -C/-t twice");
		    }
		    TgtSet (GetArg (&I, argv, 2));
		    break;

		case 'v':
		    switch (Arg [2]) {
		      	case 'm':   VerboseMap = 1;	break;
    		      	case '\0':  ++Verbose;		break;
		      	default:    UnknownOption (Arg);
		    }
		    break;

		case 'C':
		    if (CfgAvail ()) {
		      	Error ("Cannot use -C/-t twice");
		    }
		    CfgSetName (GetArg (&I, argv, 2));
		    break;

		case 'L':
		    switch (Arg [2]) {
		      	case 'n': LabelFileName = GetArg (&I, argv, 3); break;
		      	case 'p': WProtSegs = 1;			break;
		      	default:  UnknownOption (Arg);
		    }
		    break;

		case 'S':
		    StartAddr = CvtNumber (Arg, GetArg (&I, argv, 2));
		    break;

		case 'V':
		    fprintf (stderr,
			     "ld65 V%u.%u.%u - (C) Copyright 1998-2000 Ullrich von Bassewitz\n",
			     VER_MAJOR, VER_MINOR, VER_PATCH);
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
	fprintf (stderr, "No object files to link\n");
	Usage ();
    }

    /* Check if we have a valid configuration */
    if (!CfgAvail ()) {
	fprintf (stderr, "Memory configuration missing\n");
	Usage ();
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



