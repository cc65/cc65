/*****************************************************************************/
/*									     */
/*				    main.c				     */
/*									     */
/*		       Main program for the ld65 linker			     */
/*									     */
/*									     */
/*									     */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
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
#include "filetype.h"
#include "libdefs.h"
#include "objdefs.h"
#include "print.h"
#include "target.h"
#include "version.h"
#include "xmalloc.h"

/* ld65 */
#include "asserts.h"
#include "binfmt.h"
#include "condes.h"
#include "config.h"
#include "error.h"
#include "exports.h"
#include "fileio.h"
#include "filepath.h"
#include "global.h"
#include "library.h"
#include "mapfile.h"
#include "objfile.h"
#include "scanner.h"
#include "segments.h"
#include "spool.h"
#include "tgtcfg.h"



/*****************************************************************************/
/*				     Data				     */
/*****************************************************************************/



static unsigned		ObjFiles   = 0; /* Count of object files linked */
static unsigned		LibFiles   = 0; /* Count of library files linked */



/*****************************************************************************/
/*				     Code				     */
/*****************************************************************************/



static void Usage (void)
/* Print usage information and exit */
{
    fprintf (stderr,
	     "Usage: %s [options] module ...\n"
    	     "Short options:\n"
       	     "  -C name\t\tUse linker config file\n"
             "  -L path\t\tSpecify a library search path\n"
       	     "  -Ln name\t\tCreate a VICE label file\n"
       	     "  -Lp\t\t\tMark write protected segments as such (VICE)\n"
       	     "  -S addr\t\tSet the default start address\n"
       	     "  -V\t\t\tPrint the linker version\n"
       	     "  -h\t\t\tHelp (this text)\n"
       	     "  -m name\t\tCreate a map file\n"
       	     "  -o name\t\tName the default output file\n"
       	     "  -t sys\t\tSet the target system\n"
       	     "  -v\t\t\tVerbose mode\n"
       	     "  -vm\t\t\tVerbose map file\n"
	     "\n"
	     "Long options:\n"
             "  --cfg-path path\tSpecify a config file search path\n"
       	     "  --config name\t\tUse linker config file\n"
             "  --dump-config name\tDump a builtin configuration\n"
	     "  --help\t\tHelp (this text)\n"
             "  --lib file\t\tLink this library\n"
             "  --lib-path path\tSpecify a library search path\n"
	     "  --mapfile name\tCreate a map file\n"
             "  --module-id id\tSpecify a module id\n"
             "  --obj file\t\tLink this object file\n"
             "  --obj-path path\tSpecify an object file search path\n"
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



static void LinkFile (const char* Name, FILETYPE Type)
/* Handle one file */
{
    char*         PathName;
    FILE*         F;
    unsigned long Magic;


    /* If we don't know the file type, determine it from the extension */
    if (Type == FILETYPE_UNKNOWN) {
        Type = GetFileType (Name);
    }

    /* For known file types, search the file in the directory list */
    switch (Type) {

        case FILETYPE_LIB:
            PathName = SearchFile (Name, SEARCH_LIB);
            break;

        case FILETYPE_OBJ:
            PathName = SearchFile (Name, SEARCH_OBJ);
            break;

        default:
            PathName = xstrdup (Name);   /* Use the name as is */
            break;
    }

    /* We must have a valid name now */
    if (PathName == 0) {
        Error ("Input file `%s' not found", Name);
    }

    /* Try to open the file */
    F = fopen (PathName, "rb");
    if (F == 0) {
        Error ("Cannot open `%s': %s", PathName, strerror (errno));
    }

    /* Read the magic word */
    Magic = Read32 (F);

    /* Check the magic for known file types. The handling is somewhat weird
     * since we may have given a file with a ".lib" extension, which was
     * searched and found in a directory for library files, but we now find
     * out (by looking at the magic) that it's indeed an object file. We just
     * ignore the problem and hope no one will notice...
     */
    switch (Magic) {

       	case OBJ_MAGIC:
       	    ObjAdd (F, PathName);
       	    ++ObjFiles;
       	    break;

       	case LIB_MAGIC:
       	    LibAdd (F, PathName);
       	    ++LibFiles;
       	    break;

       	default:
	    fclose (F);
	    Error ("File `%s' has unknown type", PathName);

    }

    /* Free allocated memory. */
    xfree (PathName);
}



static void OptCfgPath (const char* Opt attribute ((unused)), const char* Arg)
/* Specify a config file search path */
{
    AddSearchPath (Arg, SEARCH_CFG);
}



static void OptConfig (const char* Opt attribute ((unused)), const char* Arg)
/* Define the config file */
{
    char* PathName;

    if (CfgAvail ()) {
	Error ("Cannot use -C/-t twice");
    }
    /* Search for the file */
    PathName = SearchFile (Arg, SEARCH_CFG);
    if (PathName == 0) {
        Error ("Cannot find config file `%s'", Arg);
    } else {
        CfgSetName (PathName);
        xfree (PathName);
    }
}



static void OptDbgFile (const char* Opt attribute ((unused)), const char* Arg)
/* Give the name of the debug file */
{
    DbgFileName = Arg;
}



static void OptDumpConfig (const char* Opt attribute ((unused)), const char* Arg)
/* Dump a builtin linker configuration */
{
    /* Map the given target name to its id */
    target_t T = FindTarget (Arg);
    if (T == TGT_UNKNOWN) {
        Error ("Target system `%s' is unknown", Arg);
    }

    /* Dump the builtin configuration */
    DumpBuiltinConfig (stdout, T);
}



static void OptHelp (const char* Opt attribute ((unused)),
		     const char* Arg attribute ((unused)))
/* Print usage information and exit */
{
    Usage ();
    exit (EXIT_SUCCESS);
}



static void OptLib (const char* Opt attribute ((unused)), const char* Arg)
/* Link a library */
{
    LinkFile (Arg, FILETYPE_LIB);
}



static void OptLibPath (const char* Opt attribute ((unused)), const char* Arg)
/* Specify a library file search path */
{
    AddSearchPath (Arg, SEARCH_LIB);
}



static void OptMapFile (const char* Opt attribute ((unused)), const char* Arg)
/* Give the name of the map file */
{
    MapFileName = Arg;
}



static void OptModuleId (const char* Opt, const char* Arg)
/* Specify a module id */
{
    unsigned long Id = CvtNumber (Opt, Arg);
    if (Id > 0xFFFFUL) {
        Error ("Range error in module id");
    }
    ModuleId = (unsigned) Id;
}



static void OptObj (const char* Opt attribute ((unused)), const char* Arg)
/* Link an object file */
{
    LinkFile (Arg, FILETYPE_OBJ);
}



static void OptObjPath (const char* Opt attribute ((unused)), const char* Arg)
/* Specify an object file search path */
{
    AddSearchPath (Arg, SEARCH_OBJ);
}



static void OptStartAddr (const char* Opt, const char* Arg)
/* Set the default start address */
{
    StartAddr = CvtNumber (Opt, Arg);
    HaveStartAddr = 1;
}



static void OptTarget (const char* Opt attribute ((unused)), const char* Arg)
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



static void OptVersion (const char* Opt attribute ((unused)),
			const char* Arg attribute ((unused)))
/* Print the assembler version */
{
    fprintf (stderr,
       	     "ld65 V%u.%u.%u - (C) Copyright 1998-2002 Ullrich von Bassewitz\n",
	     VER_MAJOR, VER_MINOR, VER_PATCH);
}



int main (int argc, char* argv [])
/* Assembler main program */
{
    /* Program long options */
    static const LongOpt OptTab[] = {
       	{ "--cfg-path",         1,     	OptCfgPath              },
       	{ "--config",  	       	1,     	OptConfig    	    	},
     	{ "--dbgfile",          1,      OptDbgFile              },
       	{ "--dump-config",     	1,     	OptDumpConfig           },
     	{ "--help",	       	0,     	OptHelp	     	    	},
        { "--lib",              1,      OptLib                  },
       	{ "--lib-path",         1,     	OptLibPath              },
     	{ "--mapfile",		1,	OptMapFile	    	},
       	{ "--module-id",        1,     	OptModuleId             },
        { "--obj",              1,      OptObj                  },
       	{ "--obj-path",         1,     	OptObjPath              },
	{ "--start-addr",	1,	OptStartAddr	    	},
	{ "--target",		1,	OptTarget    	    	},
	{ "--version",	       	0,  	OptVersion   	    	},
    };

    unsigned I;

    /* Initialize the cmdline module */
    InitCmdLine (&argc, &argv, "ld65");

    /* Initialize the input file search paths */
    InitSearchPaths ();

    /* Initialize the string pool */
    InitStrPool ();

    /* Check the parameters */
    I = 1;
    while (I < ArgCount) {

	/* Get the argument */
	const char* Arg = ArgVec[I];

	/* Check for an option */
	if (Arg [0] == '-') {

	    /* An option */
	    switch (Arg [1]) {

		case '-':
	       	    LongOption (&I, OptTab, sizeof(OptTab)/sizeof(OptTab[0]));
	       	    break;

		case 'h':
		case '?':
	       	    OptHelp (Arg, 0);
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
    		      	case '\0':  ++Verbosity;    	break;
		      	default:    UnknownOption (Arg);
		    }
		    break;

		case 'C':
		    OptConfig (Arg, GetArg (&I, 2));
		    break;

		case 'L':
		    switch (Arg [2]) {
                        /* ## The first two are obsolete and will go */
		      	case 'n': LabelFileName = GetArg (&I, 3);   break;
		      	case 'p': WProtSegs = 1;    	      	    break;
		      	default:  OptLibPath (Arg, GetArg (&I, 2)); break;
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
	    LinkFile (Arg, FILETYPE_UNKNOWN);

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

    /* Create the condes tables if requested */
    ConDesCreate ();

    /* Assign start addresses for the segments, define linker symbols */
    CfgAssignSegments ();

    /* Check module assertions */
    CheckAssertions ();

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
    if (DbgFileName) {
	CreateDbgFile ();
    }

    /* Dump the data for debugging */
    if (Verbosity > 1) {
	SegDump ();
	ConDesDump ();
    }

    /* Return an apropriate exit code */
    return EXIT_SUCCESS;
}




