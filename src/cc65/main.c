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

/* common */
#include "abend.h"
#include "cmdline.h"
#include "fname.h"
#include "target.h"
#include "tgttrans.h"
#include "version.h"
#include "xmalloc.h"

/* cc65 */
#include "asmcode.h"
#include "compile.h"
#include "cpu.h"
#include "error.h"
#include "global.h"
#include "incpath.h"
#include "input.h"
#include "macrotab.h"
#include "optimize.h"
#include "scanner.h"
#include "segname.h"



/*****************************************************************************/
/*				     Code				     */
/*****************************************************************************/



static void Usage (void)
{
    fprintf (stderr,
	     "Usage: %s [options] file\n"
	     "Short options:\n"
       	     "  -d\t\t\tDebug mode\n"
       	     "  -g\t\t\tAdd debug info to object file\n"
       	     "  -h\t\t\tHelp (this text)\n"
       	     "  -j\t\t\tDefault characters are signed\n"
       	     "  -o name\t\tName the output file\n"
       	     "  -t sys\t\tSet the target system\n"
       	     "  -v\t\t\tIncrease verbosity\n"
       	     "  -A\t\t\tStrict ANSI mode\n"
       	     "  -Cl\t\t\tMake local variables static\n"
       	     "  -Dsym[=defn]\t\tDefine a symbol\n"
       	     "  -I dir\t\tSet an include directory search path\n"
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
	     "  --bss-name seg\tSet the name of the BSS segment\n"
       	     "  --code-name seg\tSet the name of the CODE segment\n"
       	     "  --cpu type\t\tSet cpu type\n"
       	     "  --data-name seg\tSet the name of the DATA segment\n"
       	     "  --debug\t\tDebug mode\n"
       	     "  --debug-info\t\tAdd debug info to object file\n"
	     "  --help\t\tHelp (this text)\n"
       	     "  --include-dir dir\tSet an include directory search path\n"
       	     "  --rodata-name seg\tSet the name of the RODATA segment\n"
       	     "  --signed-chars\tDefault characters are signed\n"
       	     "  --static-locals\tMake local variables static\n"
       	     "  --target sys\t\tSet the target system\n"
       	     "  --verbose\t\tIncrease verbosity\n"
       	     "  --version\t\tPrint the compiler version number\n",
	     ProgName);
}



static void cbmsys (const char* sys)
/* Define a CBM system */
{
    AddNumericMacro ("__CBM__", 1);
    AddNumericMacro (sys, 1);
}



static void SetSys (const char* Sys)
/* Define a target system */
{
    switch (Target = FindTarget (Sys)) {

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

     	case TGT_BBC:
     	    AddNumericMacro ("__BBC__", 1);
     	    break;

     	case TGT_APPLE2:
     	    AddNumericMacro ("__APPLE2__", 1);
     	    break;

     	case TGT_GEOS:
     	    /* Do not handle as a CBM system */
     	    AddNumericMacro ("__GEOS__", 1);
     	    break;

     	default:
       	    AbEnd ("Unknown target system type");
    }

    /* Initialize the translation tables for the target system */
    TgtTranslateInit ();
}



static void DoCreateDep (const char* OutputName)
/* Create the dependency file */
{
    /* Make the dependency file name from the output file name */
    char* DepName = MakeFilename (OutputName, ".u");

    /* Open the file */
    FILE* F = fopen (DepName, "w");
    if (F == 0) {
    	Fatal ("Cannot open dependency file `%s': %s", DepName, strerror (errno));
    }

    /* Write the dependencies to the file */
    WriteDependencies (F, OutputName);

    /* Close the file, check for errors */
    if (fclose (F) != 0) {
    	remove (DepName);
    	Fatal ("Cannot write to dependeny file (disk full?)");
    }

    /* Free the name */
    xfree (DepName);
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



static void CheckSegName (const char* Seg)
/* Abort if the given name is not a valid segment name */
{
    /* Print an error and abort if the name is not ok */
    if (!ValidSegName (Seg)) {
	AbEnd ("Segment name `%s' is invalid", Seg);
    }
}



static void OptAddSource (const char* Opt, const char* Arg)
/* Add source lines as comments in generated assembler file */
{
    AddSource = 1;
}



static void OptAnsi (const char* Opt, const char* Arg)
/* Compile in strict ANSI mode */
{
    ANSI = 1;
}



static void OptBssName (const char* Opt, const char* Arg)
/* Handle the --bss-name option */
{
    /* Check for a valid name */
    CheckSegName (Arg);

    /* Set the name */
    NewSegName (SEG_BSS, Arg);
}



static void OptCodeName (const char* Opt, const char* Arg)
/* Handle the --code-name option */
{
    /* Check for a valid name */
    CheckSegName (Arg);

    /* Set the name */
    NewSegName (SEG_CODE, Arg);
}



static void OptCreateDep (const char* Opt, const char* Arg)
/* Handle the --create-dep option */
{
    CreateDep = 1;
}



static void OptCPU (const char* Opt, const char* Arg)
/* Handle the --cpu option */
{
    if (strcmp (Arg, "6502") == 0) {
       	CPU = CPU_6502;
    } else if (strcmp (Arg, "65C02") == 0) {
	CPU = CPU_65C02;
    } else {
	AbEnd ("Invalid CPU: `%s'", Arg);
    }
}



static void OptDataName (const char* Opt, const char* Arg)
/* Handle the --code-name option */
{
    /* Check for a valid name */
    CheckSegName (Arg);

    /* Set the name */
    NewSegName (SEG_DATA, Arg);
}



static void OptDebug (const char* Opt, const char* Arg)
/* Compiler debug mode */
{
    Debug = 1;
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
    AddIncludePath (Arg, INC_SYS | INC_USER);
}



static void OptRodataName (const char* Opt, const char* Arg)
/* Handle the --rodata-name option */
{
    /* Check for a valid name */
    CheckSegName (Arg);

    /* Set the name */
    NewSegName (SEG_RODATA, Arg);
}



static void OptSignedChars (const char* Opt, const char* Arg)
/* Make default characters signed */
{
    SignedChars = 1;
}



static void OptStaticLocals (const char* Opt, const char* Arg)
/* Place local variables in static storage */
{
    StaticLocals = 1;
}



static void OptTarget (const char* Opt, const char* Arg)
/* Set the target system */
{
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
	{ "--add-source",	0,    	OptAddSource		},
	{ "--ansi",   	 	0,	OptAnsi			},
	{ "--bss-name",		1, 	OptBssName		},
	{ "--code-name",	1, 	OptCodeName		},
	{ "--create-dep",	0,	OptCreateDep		},
        { "--cpu",     	       	1, 	OptCPU 			},
	{ "--data-name",	1, 	OptDataName		},
       	{ "--debug",           	0,     	OptDebug		},
	{ "--debug-info",      	0, 	OptDebugInfo		},
	{ "--help",	 	0, 	OptHelp			},
	{ "--include-dir",     	1,   	OptIncludeDir		},
	{ "--rodata-name",	1, 	OptRodataName		},
	{ "--signed-chars",	0, 	OptSignedChars		},
       	{ "--static-locals",   	0, 	OptStaticLocals		},
	{ "--target",	 	1,  	OptTarget		},
	{ "--verbose",	       	0, 	OptVerbose		},
	{ "--version",	       	0,	OptVersion		},
    };

    int I;

    /* Initialize the output file name */
    const char* OutputFile = 0;
    const char* InputFile  = 0;

    /* Initialize the cmdline module */
    InitCmdLine (argc, argv, "cc65");

    /* Initialize the default segment names */
    InitSegNames ();

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

		case 'd':
		    OptDebug (Arg, 0);
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
		    OutputFile = GetArg (&I, 2);
		    break;

		case 't':
		    OptTarget (Arg, GetArg (&I, 2));
		    break;

		case 'u':
		    OptCreateDep (Arg, 0);
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
		    	     	OptStaticLocals (Arg, 0);
		    	     	break;
		  	    default:
		  		UnknownOption (Arg);
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
       	       	    OptAddSource (Arg, 0);
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
       	    if (InputFile) {
       	       	fprintf (stderr, "additional file specs ignored\n");
       	    } else {
       	       	InputFile = Arg;
	    }
	}

	/* Next argument */
	++I;
    }

    /* Did we have a file spec on the command line? */
    if (InputFile == 0) {
	AbEnd ("No input files");
    }

    /* Open the input file */
    OpenMainFile (InputFile);

    /* Create the output file name if it was not explicitly given */
    if (OutputFile == 0) {
	OutputFile = MakeFilename (InputFile, ".s");
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
	F = fopen (OutputFile, "w");
	if (F == 0) {
	    Fatal ("Cannot open output file `%s': %s", OutputFile, strerror (errno));
	}

	/* Write the output to the file */
	WriteOutput (F);

	/* Close the file, check for errors */
	if (fclose (F) != 0) {
	    remove (OutputFile);
	    Fatal ("Cannot write to output file (disk full?)");
	}

	/* Create dependencies if requested */
	if (CreateDep) {
	    DoCreateDep (OutputFile);
	}

    }

    /* Return an apropriate exit code */
    return (ErrorCount > 0)? EXIT_FAILURE : EXIT_SUCCESS;
}



