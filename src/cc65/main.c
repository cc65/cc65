/*****************************************************************************/
/*                                                                           */
/*				    main.c				     */
/*                                                                           */
/*			       cc65 main program			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2002 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@cc65.org                                                 */
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
#include <errno.h>

/* common */
#include "abend.h"
#include "chartype.h"
#include "cmdline.h"
#include "fname.h"
#include "print.h"
#include "segdefs.h"
#include "target.h"
#include "tgttrans.h"
#include "version.h"
#include "xmalloc.h"

/* cc65 */
#include "asmcode.h"
#include "compile.h"
#include "codeopt.h"
#include "cpu.h"
#include "error.h"
#include "global.h"
#include "incpath.h"
#include "input.h"
#include "macrotab.h"
#include "scanner.h"
#include "segments.h"



/*****************************************************************************/
/*				     Code				     */
/*****************************************************************************/



static void Usage (void)
{
    fprintf (stderr,
	     "Usage: %s [options] file\n"
	     "Short options:\n"
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
       	     "  -d\t\t\tDebug mode\n"
       	     "  -g\t\t\tAdd debug info to object file\n"
       	     "  -h\t\t\tHelp (this text)\n"
       	     "  -j\t\t\tDefault characters are signed\n"
       	     "  -o name\t\tName the output file\n"
             "  -r\t\t\tEnable register variables\n"
       	     "  -t sys\t\tSet the target system\n"
       	     "  -v\t\t\tIncrease verbosity\n"
	     "\n"
	     "Long options:\n"
       	     "  --add-source\t\tInclude source as comment\n"
       	     "  --ansi\t\tStrict ANSI mode\n"
	     "  --bss-name seg\tSet the name of the BSS segment\n"
       	     "  --check-stack\t\tGenerate stack overflow checks\n"
       	     "  --code-name seg\tSet the name of the CODE segment\n"
	     "  --codesize x\t\tAccept larger code by factor x\n"
       	     "  --cpu type\t\tSet cpu type\n"
	     "  --create-dep\t\tCreate a make dependency file\n"
       	     "  --data-name seg\tSet the name of the DATA segment\n"
       	     "  --debug\t\tDebug mode\n"
       	     "  --debug-info\t\tAdd debug info to object file\n"
	     "  --debug-opt name\tDebug optimization steps\n"
	     "  --disable-opt name\tDisable an optimization step\n"
       	     "  --enable-opt name\tEnable an optimization step\n"
	     "  --help\t\tHelp (this text)\n"
       	     "  --include-dir dir\tSet an include directory search path\n"
	     "  --list-opt-steps\tList all optimizer steps and exit\n"
             "  --register-space b\tSet space available for register variables\n"
             "  --register-vars\tEnable register variables\n"
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
    DefineNumericMacro ("__CBM__", 1);
    DefineNumericMacro (sys, 1);
}



static void SetSys (const char* Sys)
/* Define a target system */
{
    switch (Target = FindTarget (Sys)) {

	case TGT_NONE:
	    break;

        case TGT_MODULE:
            AbEnd ("Cannot use `module' as a target for the compiler");
            break;

	case TGT_ATARI:
    	    DefineNumericMacro ("__ATARI__", 1);
	    break;

	case TGT_C16:
	    cbmsys ("__C16__");
	    break;

	case TGT_C64:
	    cbmsys ("__C64__");
	    break;

	case TGT_VIC20:
	    cbmsys ("__VIC20__");
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

	case TGT_CBM510:
	    cbmsys ("__CBM510__");
	    break;

	case TGT_CBM610:
	    cbmsys ("__CBM610__");
	    break;

	case TGT_PET:
     	    cbmsys ("__PET__");
     	    break;

     	case TGT_BBC:
     	    DefineNumericMacro ("__BBC__", 1);
     	    break;

     	case TGT_APPLE2:
     	    DefineNumericMacro ("__APPLE2__", 1);
     	    break;

     	case TGT_GEOS:
     	    /* Do not handle as a CBM system */
     	    DefineNumericMacro ("__GEOS__", 1);
     	    break;

	case TGT_LUNIX:
	    DefineNumericMacro ("__LUNIX__", 1);
	    break;

        case TGT_ATMOS:
            DefineNumericMacro ("__ATMOS__", 1);
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
    if (Def [0] != '_' && !IsAlpha (Def [0])) {
	InvDef (Def);
    }

    /* Check the symbol name */
    while (IsAlNum (*P) || *P == '_') {
	++P;
    }

    /* Do we have a value given? */
    if (*P != '=') {
	if (*P != '\0') {
	    InvDef (Def);
	}
	/* No value given. Define the macro with the value 1 */
     	DefineNumericMacro (Def, 1);
    } else {
	/* We have a value, P points to the '=' character. Since the argument
	 * is const, create a copy and replace the '=' in the copy by a zero
	 * terminator.
       	 */
	char* Q;
	unsigned Len = strlen (Def)+1;
	char* S = (char*) xmalloc (Len);
	memcpy (S, Def, Len);
	Q = S + (P - Def);
	*Q++ = '\0';

    	/* Define this as a macro */
    	DefineTextMacro (S, Q);

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



static void OptAddSource (const char* Opt attribute ((unused)),
			  const char* Arg attribute ((unused)))
/* Add source lines as comments in generated assembler file */
{
    AddSource = 1;
}



static void OptAnsi (const char* Opt attribute ((unused)),
		     const char* Arg attribute ((unused)))
/* Compile in strict ANSI mode */
{
    ANSI = 1;
}



static void OptBssName (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --bss-name option */
{
    /* Check for a valid name */
    CheckSegName (Arg);

    /* Set the name */
    NewSegName (SEG_BSS, Arg);
}



static void OptCheckStack (const char* Opt attribute ((unused)),
			   const char* Arg attribute ((unused)))
/* Handle the --check-stack option */
{
    CheckStack = 1;
}



static void OptCodeName (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --code-name option */
{
    /* Check for a valid name */
    CheckSegName (Arg);

    /* Set the name */
    NewSegName (SEG_CODE, Arg);
}



static void OptCodeSize (const char* Opt, const char* Arg)
/* Handle the --codesize option */
{
    /* Numeric argument expected */
    if (sscanf (Arg, "%u", &CodeSizeFactor) != 1 ||
	CodeSizeFactor < 100 ||
       	CodeSizeFactor > 1000) {
	AbEnd ("Argument for %s is invalid", Opt);
    }
}



static void OptCreateDep (const char* Opt attribute ((unused)),
			  const char* Arg attribute ((unused)))
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
       	AbEnd ("Invalid argument for %s: `%s'", Opt, Arg);
    }
}



static void OptDataName (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --data-name option */
{
    /* Check for a valid name */
    CheckSegName (Arg);

    /* Set the name */
    NewSegName (SEG_DATA, Arg);
}



static void OptDebug (const char* Opt attribute ((unused)),
		      const char* Arg attribute ((unused)))
/* Compiler debug mode */
{
    Debug = 1;
}



static void OptDebugInfo (const char* Opt attribute ((unused)),
		    	  const char* Arg attribute ((unused)))
/* Add debug info to the object file */
{
    DebugInfo = 1;
}



static void OptDebugOpt (const char* Opt attribute ((unused)), const char* Arg)
/* Debug optimization steps */
{
    char Buf [128];
    char* Line;

    /* Open the file */
    FILE* F = fopen (Arg, "r");
    if (F == 0) {
       	AbEnd ("Cannot open `%s': %s", Arg, strerror (errno));
    }

    /* Read line by line, ignore empty lines and switch optimization
     * steps on/off.
     */
    while (fgets (Buf, sizeof (Buf), F) != 0) {

	/* Remove trailing control chars. This will also remove the
	 * trailing newline.
	 */
	unsigned Len = strlen (Buf);
	while (Len > 0 && IsControl (Buf[Len-1])) {
	    --Len;
	}
	Buf[Len] = '\0';

	/* Get a pointer to the buffer and remove leading white space */
	Line = Buf;
	while (IsBlank (*Line)) {
	    ++Line;
	}

	/* Check the first character and enable/disable the step or
	 * ignore the line
	 */
	switch (*Line) {

	    case '\0':
	    case '#':
	    case ';':
	        /* Empty or comment line */
	        continue;

	    case '-':
	        DisableOpt (Line+1);
	        break;

	    case '+':
	        ++Line;
	        /* FALLTHROUGH */

	    default:
	       	EnableOpt (Line);
	        break;

	}

    }

    /* Close the file, no error check here since we were just reading and
     * this is only a debug function.
     */
    (void) fclose (F);
}



static void OptDisableOpt (const char* Opt attribute ((unused)), const char* Arg)
/* Disable an optimization step */
{
    DisableOpt (Arg);
}



static void OptEnableOpt (const char* Opt attribute ((unused)), const char* Arg)
/* Enable an optimization step */
{
    EnableOpt (Arg);
}



static void OptHelp (const char* Opt attribute ((unused)),
		     const char* Arg attribute ((unused)))
/* Print usage information and exit */
{
    Usage ();
    exit (EXIT_SUCCESS);
}



static void OptIncludeDir (const char* Opt attribute ((unused)), const char* Arg)
/* Add an include search path */
{
    AddIncludePath (Arg, INC_SYS | INC_USER);
}



static void OptListOptSteps (const char* Opt attribute ((unused)),
			     const char* Arg attribute ((unused)))
/* List all optimizer steps */
{
    /* List the optimizer steps */
    ListOptSteps (stdout);

    /* Terminate */
    exit (EXIT_SUCCESS);
}



static void OptRegisterSpace (const char* Opt, const char* Arg)
/* Handle the --register-space option */
{
    /* Numeric argument expected */
    if (sscanf (Arg, "%u", &RegisterSpace) != 1 || RegisterSpace > 256) {
       	AbEnd ("Argument for option %s is invalid", Opt);
    }
}



static void OptRegisterVars (const char* Opt attribute ((unused)),
                             const char* Arg attribute ((unused)))
/* Handle the --register-vars option */
{
    EnableRegVars = 1;
}



static void OptRodataName (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --rodata-name option */
{
    /* Check for a valid name */
    CheckSegName (Arg);

    /* Set the name */
    NewSegName (SEG_RODATA, Arg);
}



static void OptSignedChars (const char* Opt attribute ((unused)),
			    const char* Arg attribute ((unused)))
/* Make default characters signed */
{
    SignedChars = 1;
}



static void OptStaticLocals (const char* Opt attribute ((unused)),
			     const char* Arg attribute ((unused)))
/* Place local variables in static storage */
{
    StaticLocals = 1;
}



static void OptTarget (const char* Opt attribute ((unused)), const char* Arg)
/* Set the target system */
{
    SetSys (Arg);
}



static void OptVerbose (const char* Opt attribute ((unused)),
			const char* Arg attribute ((unused)))
/* Increase verbosity */
{
    ++Verbosity;
}



static void OptVersion (const char* Opt attribute ((unused)),
			const char* Arg attribute ((unused)))
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
	{ "--add-source",	0,    	OptAddSource 		},
	{ "--ansi",   	 	0,	OptAnsi	     		},
	{ "--bss-name",		1, 	OptBssName   		},
       	{ "--check-stack",	0,     	OptCheckStack		},
	{ "--code-name",	1, 	OptCodeName  		},
	{ "--codesize",		1,	OptCodeSize		},
        { "--cpu",     	       	1, 	OptCPU 	     		},
	{ "--create-dep",	0,	OptCreateDep 		},
	{ "--data-name",	1, 	OptDataName  		},
       	{ "--debug",           	0,     	OptDebug     		},
	{ "--debug-info",      	0, 	OptDebugInfo 		},
        { "--debug-opt",        1,      OptDebugOpt             },
	{ "--disable-opt",	1,	OptDisableOpt		},
	{ "--enable-opt",	1,	OptEnableOpt,		},
	{ "--help",	 	0, 	OptHelp	     		},
	{ "--include-dir",     	1,   	OptIncludeDir		},
	{ "--list-opt-steps",   0,      OptListOptSteps         },
        { "--register-space",   1,      OptRegisterSpace        },
        { "--register-vars",    0,      OptRegisterVars         },
	{ "--rodata-name",	1, 	OptRodataName		},
	{ "--signed-chars",	0, 	OptSignedChars	       	},
       	{ "--static-locals",   	0, 	OptStaticLocals	       	},
	{ "--target",	  	1,  	OptTarget    	       	},
	{ "--verbose",	       	0, 	OptVerbose   	       	},
	{ "--version",	       	0,	OptVersion   	       	},
    };

    unsigned I;

    /* Initialize the output file name */
    const char* OutputFile = 0;
    const char* InputFile  = 0;

    /* Initialize the cmdline module */
    InitCmdLine (&argc, &argv, "cc65");

    /* Initialize the default segment names */
    InitSegNames ();

    /* Parse the command line */
    I = 1;
    while (I < ArgCount) {

	const char* P;

       	/* Get the argument */
       	const char* Arg = ArgVec[I];

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

                case 'r':
                    OptRegisterVars (Arg, 0);
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
			        CodeSizeFactor = 200;
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

    /* Create the output file name if it was not explicitly given */
    if (OutputFile == 0) {
	OutputFile = MakeFilename (InputFile, ".s");
    }

    /* Go! */
    Compile (InputFile);

    /* Create the output file if we didn't had any errors */
    if (ErrorCount == 0 || Debug) {

	FILE* F;

#if 0
     	/* Optimize the output if requested */
     	if (Optimize) {
     	    OptDoOpt ();
     	}
#endif

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



