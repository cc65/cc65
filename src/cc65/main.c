/*****************************************************************************/
/*                                                                           */
/*				    main.c				     */
/*                                                                           */
/*			       cc65 main program			     */
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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#include "../common/cmdline.h"
#include "../common/version.h"

#include "asmcode.h"
#include "compile.h"
#include "cpu.h"
#include "error.h"
#include "global.h"
#include "incpath.h"
#include "io.h"
#include "macrotab.h"
#include "mem.h"
#include "optimize.h"
#include "scanner.h"



/*****************************************************************************/
/*		  		     data				     */
/*****************************************************************************/



/* Names of the target systems sorted by target name */
static const char* TargetNames [] = {
    "none",
    "atari",
    "c64",
    "c128",
    "ace",
    "plus4",
    "cbm610",
    "pet",
    "nes",
    "apple2",
    "geos",
};



/*****************************************************************************/
/*				     Code				     */
/*****************************************************************************/



static void Usage (void)
{
    fprintf (stderr,
	     "Usage: cc65 [options] file\n"
	     "Short options:\n"
       	     "  -d\t\t\tDebug mode\n"
       	     "  -g\t\t\tAdd debug info to object file\n"
       	     "  -h\t\t\tPrint this help\n"
       	     "  -j\t\t\tDefault characters are signed\n"
       	     "  -o name\t\tName the output file\n"
       	     "  -t sys\t\tSet the target system\n"
       	     "  -v\t\t\tIncrease verbosity\n"
       	     "  -A\t\t\tStrict ANSI mode\n"
       	     "  -Cl\t\t\tMake local variables static\n"
       	     "  -Dsym[=defn]\t\tDefine a symbol\n"
       	     "  -I path\t\tSet an include directory search path\n"
       	     "  -O\t\t\tOptimize code\n"
       	     "  -Oi\t\t\tOptimize code, inline more code\n"
       	     "  -Or\t\t\tEnable register variables\n"
       	     "  -Os\t\t\tInline some known functions\n"
       	     "  -T\t\t\tInclude source as comment\n"
       	     "  -V\t\t\tPrint the compiler version number\n"
       	     "  -W\t\t\tSuppress warnings\n"
	     "\n"
	     "Long options:\n"
       	     "  --ansi\t\tStrict ANSI mode\n"
       	     "  --cpu type\t\tSet cpu type\n"
       	     "  --debug-info\t\tAdd debug info to object file\n"
	     "  --help\t\tHelp (this text)\n"
       	     "  --include-dir dir\tSet an include directory search path\n"
       	     "  --signed-chars\tDefault characters are signed\n"
       	     "  --target sys\t\tSet the target system\n"
       	     "  --verbose\t\tIncrease verbosity\n"
       	     "  --version\t\tPrint the compiler version number\n");
}



static void cbmsys (const char* sys)
/* Define a CBM system */
{
    AddNumericMacro ("__CBM__", 1);
    AddNumericMacro (sys, 1);
}



static int MapSys (const char* Name)
/* Map a target name to a system code. Return -1 in case of an error */
{
    unsigned I;

    /* Check for a numeric target */
    if (isdigit (*Name)) {
	int Target = atoi (Name);
	if (Target >= 0 && Target < TGT_COUNT) {
	    return Target;
	}
    }

    /* Check for a target string */
    for (I = 0; I < TGT_COUNT; ++I) {
	if (strcmp (TargetNames [I], Name) == 0) {
	    return I;
	}
    }
    /* Not found */
    return -1;
}



static void SetSys (const char* Sys)
/* Define a target system */
{
    switch (Target = MapSys (Sys)) {

	case TGT_NONE:
	    break;

	case TGT_ATARI:
    	    AddNumericMacro ("__ATARI__", 1);
	    break;

	case TGT_C64:
	    cbmsys ("__C64__");
	    break;

	case TGT_C128:
	    cbmsys ("__C128__");
	    break;

	case TGT_ACE:
	    cbmsys ("__ACE__");
	    break;

	case TGT_PLUS4:
	    cbmsys ("__PLUS4__");
	    break;

	case TGT_CBM610:
	    cbmsys ("__CBM610__");
	    break;

	case TGT_PET:
	    cbmsys ("__PET__");
	    break;

	case TGT_NES:
	    AddNumericMacro ("__NES__", 1);
	    break;

	case TGT_APPLE2:
	    AddNumericMacro ("__APPLE2__", 1);
	    break;

	case TGT_GEOS:
	    /* Do not handle as a CBM system */
	    AddNumericMacro ("__GEOS__", 1);
	    break;

	default:
	    fputs ("Unknown system type\n", stderr);
	    exit (EXIT_FAILURE);
    }
}



static void DefineSym (const char* Def)
/* Define a symbol on the command line */
{
    const char* P = Def;

    /* The symbol must start with a character or underline */
    if (Def [0] != '_' && !isalpha (Def [0])) {
	InvDef (Def);
    }

    /* Check the symbol name */
    while (isalnum (*P) || *P == '_') {
	++P;
    }

    /* Do we have a value given? */
    if (*P != '=') {
	if (*P != '\0') {
	    InvDef (Def);
	}
	/* No value given. Define the macro with the value 1 */
	AddNumericMacro (Def, 1);
    } else {
	/* We have a value, P points to the '=' character. Since the argument
	 * is const, create a copy and replace the '=' in the copy by a zero
	 * terminator.
       	 */
	char* Q;
	unsigned Len = strlen (Def)+1;
	char* S = xmalloc (Len);
	memcpy (S, Def, Len);
	Q = S + (P - Def);
	*Q++ = '\0';

    	/* Define this as a macro */
    	AddTextMacro (S, Q);

    	/* Release the allocated memory */
    	xfree (S);
    }
}



static void OptAnsi (const char* Opt, const char* Arg)
/* Compile in strict ANSI mode */
{
    ANSI = 1;
}



static void OptCPU (const char* Opt, const char* Arg)
/* Handle the --cpu option */
{
    if (Arg == 0) {
	NeedArg (Opt);
    }
    if (strcmp (Arg, "6502") == 0) {
       	CPU = CPU_6502;
    } else if (strcmp (Arg, "65C02") == 0) {
	CPU = CPU_65C02;
    } else {
	fprintf (stderr, "Invalid CPU: `%s'\n", Arg);
	exit (EXIT_FAILURE);
    }
}



static void OptDebugInfo (const char* Opt, const char* Arg)
/* Add debug info to the object file */
{
    DebugInfo = 1;
}



static void OptHelp (const char* Opt, const char* Arg)
/* Print usage information and exit */
{
    Usage ();
    exit (EXIT_SUCCESS);
}



static void OptIncludeDir (const char* Opt, const char* Arg)
/* Add an include search path */
{
    if (Arg == 0) {
	NeedArg (Opt);
    }
    AddIncludePath (Arg, INC_SYS | INC_USER);
}



static void OptSignedChars (const char* Opt, const char* Arg)
/* Make default characters signed */
{
    SignedChars = 1;
}



static void OptTarget (const char* Opt, const char* Arg)
/* Set the target system */
{
    if (Arg == 0) {
	NeedArg (Opt);
    }
    SetSys (Arg);
}



static void OptVerbose (const char* Opt, const char* Arg)
/* Increase verbosity */
{
    ++Verbose;
}



static void OptVersion (const char* Opt, const char* Arg)
/* Print the assembler version */
{
    fprintf (stderr,
       	     "cc65 V%u.%u.%u\n",
       	     VER_MAJOR, VER_MINOR, VER_PATCH);
}



int main (int argc, char* argv[])
{
    /* Program long options */
    static const LongOpt OptTab[] = {
	{ "--ansi",   	 	0,	OptAnsi			},
        { "--cpu",     	       	1,	OptCPU 			},
	{ "--debug-info",      	0,	OptDebugInfo		},
	{ "--help",	 	0,	OptHelp			},
	{ "--include-dir",     	1,   	OptIncludeDir		},
	{ "--signed-chars",	0,	OptSignedChars		},
	{ "--target",	 	1,  	OptTarget		},
	{ "--verbose",	       	0,	OptVerbose		},
	{ "--version",	       	0,	OptVersion		},
    };

    int I;
    char out_name [256];

    /* Initialize the output file name */
    out_name [0] = '\0';

    fin = NULL;

    /* Initialize the cmdline module */
    InitCmdLine (argc, argv);

    /* Parse the command line */
    I = 1;
    while (I < argc) {

	const char* P;

       	/* Get the argument */
       	const char* Arg = argv [I];

       	/* Check for an option */
       	if (Arg [0] == '-') {

       	    switch (Arg [1]) {

		case '-':
		    LongOption (&I, OptTab, sizeof(OptTab)/sizeof(OptTab[0]));
		    break;

		case 'd':	/* debug mode */
		    Debug = 1;
		    break;

		case 'h':
		case '?':
		    OptHelp (Arg, 0);
		    break;

	    	case 'g':
	    	    OptDebugInfo (Arg, 0);
	    	    break;

		case 'j':
		    OptSignedChars (Arg, 0);
		    break;

    		case 'o':
		    strcpy (out_name, GetArg (&I, 2));
		    break;

		case 't':
		    OptTarget (Arg, GetArg (&I, 2));
		    break;

		case 'v':
		    OptVerbose (Arg, 0);
		    break;

		case 'A':
		    OptAnsi (Arg, 0);
		    break;

		case 'C':
       	       	    P = Arg + 2;
		    while (*P) {
		    	switch (*P++) {
		    	    case 'l':
		    	    	LocalsAreStatic = 1;
		    		break;
		    	}
		    }
		    break;

		case 'D':
		    DefineSym (GetArg (&I, 2));
		    break;

		case 'I':
		    OptIncludeDir (Arg, GetArg (&I, 2));
		    break;

		case 'O':
		    Optimize = 1;
		    P = Arg + 2;
		    while (*P) {
		    	switch (*P++) {
		    	    case 'f':
		    	     	sscanf (P, "%lx", (long*) &OptDisable);
		    	     	break;
		    	    case 'i':
		    	     	FavourSize = 0;
    		    	     	break;
		    	    case 'r':
		    	 	EnableRegVars = 1;
				break;
			    case 's':
			       	InlineStdFuncs = 1;
				break;
			}
		    }
		    break;

		case 'T':
		    IncSource = 1;
		    break;

		case 'V':
		    OptVersion (Arg, 0);
		    break;

		case 'W':
		    NoWarn = 1;
		    break;

		default:
       	       	    UnknownOption (Arg);
		    break;
	    }
	} else {
	    if (fin) {
		fprintf (stderr, "additional file specs ignored\n");
	    } else {
		fin = xstrdup (Arg);
		inp = fopen (fin, "r");
		if (inp == 0) {
		    Fatal (FAT_CANNOT_OPEN_INPUT, strerror (errno));
		}
	    }
	}

	/* Next argument */
	++I;
    }

    /* Did we have a file spec on the command line? */
    if (!fin) {
	fprintf (stderr, "%s: No input files\n", argv [0]);
	exit (EXIT_FAILURE);
    }

    /* Create the output file name. We should really have
     * some checks for string overflow, but I'll drop this because of the
     * additional code size it would need (as in other places). Sigh.
     * #### To be removed
     */
    if (out_name [0] == '\0') {
	char* p;
	/* No output name given, create default */
     	strcpy (out_name, fin);
     	if ((p = strrchr (out_name, '.'))) {
     	    *p = '\0';
     	}
     	strcat (out_name, ".s");
    }

    /* Go! */
    Compile ();

    /* Create the output file if we didn't had any errors */
    if (ErrorCount == 0 || Debug) {

	FILE* F;

	/* Optimize the output if requested */
	if (Optimize) {
	    OptDoOpt ();
	}

	/* Open the file */
	F = fopen (out_name, "w");
	if (F == 0) {
	    Fatal (FAT_CANNOT_OPEN_OUTPUT, strerror (errno));
	}

	/* Write the output to the file */
	WriteOutput (F);

	/* Close the file, check for errors */
	if (fclose (F) != 0) {
	    remove (out_name);
	    Fatal (FAT_CANNOT_WRITE_OUTPUT);
	}
    }

    /* Return an apropriate exit code */
    return (ErrorCount > 0)? EXIT_FAILURE : EXIT_SUCCESS;
}



