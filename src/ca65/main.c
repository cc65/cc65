/*****************************************************************************/
/*                                                                           */
/*				    main.c				     */
/*                                                                           */
/*		   Main program for the ca65 macroassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
/*               D-70794 Filderstadt                                         */
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
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* common */
#include "chartype.h"
#include "cmdline.h"
#include "print.h"
#include "target.h"
#include "tgttrans.h"
#include "version.h"

/* ca65 */
#include "abend.h"
#include "asserts.h"
#include "error.h"
#include "expr.h"
#include "feature.h"
#include "filetab.h"
#include "global.h"
#include "incpath.h"
#include "instr.h"
#include "istack.h"
#include "lineinfo.h"
#include "listing.h"
#include "macro.h"
#include "nexttok.h"
#include "objfile.h"
#include "options.h"
#include "pseudo.h"
#include "scanner.h"
#include "segment.h"
#include "spool.h"
#include "symtab.h"
#include "ulabel.h"



/*****************************************************************************/
/*     	       	     	       	     Code				     */
/*****************************************************************************/



static void Usage (void)
/* Print usage information and exit */
{
    fprintf (stderr,
    	     "Usage: %s [options] file\n"
    	     "Short options:\n"
       	     "  -D name[=value]\tDefine a symbol\n"
       	     "  -I dir\t\tSet an include directory search path\n"
       	     "  -U\t\t\tMark unresolved symbols as import\n"
       	     "  -V\t\t\tPrint the assembler version\n"
       	     "  -W n\t\t\tSet warning level n\n"
       	     "  -g\t\t\tAdd debug info to object file\n"
       	     "  -h\t\t\tHelp (this text)\n"
       	     "  -i\t\t\tIgnore case of symbols\n"
       	     "  -l\t\t\tCreate a listing if assembly was ok\n"
       	     "  -o name\t\tName the output file\n"
       	     "  -s\t\t\tEnable smart mode\n"
       	     "  -t sys\t\tSet the target system\n"
       	     "  -v\t\t\tIncrease verbosity\n"
	     "\n"
	     "Long options:\n"
       	     "  --auto-import\t\tMark unresolved symbols as import\n"
       	     "  --cpu type\t\tSet cpu type\n"
       	     "  --debug-info\t\tAdd debug info to object file\n"
	     "  --feature name\tSet an emulation feature\n"
	     "  --help\t\tHelp (this text)\n"
	     "  --ignore-case\t\tIgnore case of symbols\n"
       	     "  --include-dir dir\tSet an include directory search path\n"
       	     "  --listing\t\tCreate a listing if assembly was ok\n"
       	     "  --pagelength n\tSet the page length for the listing\n"
       	     "  --smart\t\tEnable smart mode\n"
       	     "  --target sys\t\tSet the target system\n"
       	     "  --verbose\t\tIncrease verbosity\n"
       	     "  --version\t\tPrint the assembler version\n",
    	     ProgName);
}



static void SetOptions (void)
/* Set the option for the translator */
{
    char Buf [256];

    /* Set the translator */
    sprintf (Buf, "ca65 V%u.%u.%u", VER_MAJOR, VER_MINOR, VER_PATCH);
    OptTranslator (Buf);

    /* Set date and time */
    OptDateTime ((unsigned long) time(0));
}



static void DefineSymbol (const char* Def)
/* Define a symbol from the command line */
{
    const char* P;
    unsigned I;
    long Val;
    char SymName [MAX_STR_LEN+1];

    /* The symbol must start with a character or underline */
    if (Def [0] != '_' && !IsAlpha (Def [0])) {
	InvDef (Def);
    }
    P = Def;

    /* Copy the symbol, checking the rest */
    I = 0;
    while (IsAlNum (*P) || *P == '_') {
	if (I <= MAX_STR_LEN) {
	    SymName [I++] = *P;
	}
	++P;
    }
    SymName [I] = '\0';

    /* Do we have a value given? */
    if (*P != '=') {
	if (*P != '\0') {
	    InvDef (Def);
	}
	Val = 0;
    } else {
	/* We have a value */
	++P;
	if (*P == '$') {
	    ++P;
	    if (sscanf (P, "%lx", &Val) != 1) {
	 	InvDef (Def);
	    }
	} else {
	    if (sscanf (P, "%li", &Val) != 1) {
     		InvDef (Def);
	    }
       	}
    }

    /* Check if have already a symbol with this name */
    if (SymIsDef (SymName, SCOPE_ANY)) {
	AbEnd ("`%s' is already defined", SymName);
    }

    /* Define the symbol */
    SymDef (SymName, GenLiteralExpr (Val), SYM_DEFAULT);
}



static void OptAutoImport (const char* Opt attribute ((unused)),
			   const char* Arg attribute ((unused)))
/* Mark unresolved symbols as imported */
{
    AutoImport = 1;
}



static void OptCPU (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --cpu option */
{
    cpu_t CPU = FindCPU (Arg);
    if (CPU == CPU_UNKNOWN) {
	AbEnd ("Invalid CPU: `%s'", Arg);
    } else {
	SetCPU (CPU);
    }
}



static void OptDebugInfo (const char* Opt attribute ((unused)),
			  const char* Arg attribute ((unused)))
/* Add debug info to the object file */
{
    DbgSyms = 1;
}



static void OptFeature (const char* Opt attribute ((unused)), const char* Arg)
/* Set an emulation feature */
{
    /* Set the feature, check for errors */
    if (SetFeature (Arg) == FEAT_UNKNOWN) {
      	AbEnd ("Illegal emulation feature: `%s'", Arg);
    }
}



static void OptHelp (const char* Opt attribute ((unused)),
		     const char* Arg attribute ((unused)))
/* Print usage information and exit */
{
    Usage ();
    exit (EXIT_SUCCESS);
}



static void OptIgnoreCase (const char* Opt attribute ((unused)),
			   const char* Arg attribute ((unused)))
/* Ignore case on symbols */
{
    IgnoreCase = 1;
}



static void OptIncludeDir (const char* Opt attribute ((unused)), const char* Arg)
/* Add an include search path */
{
    AddIncludePath (Arg);
}



static void OptListing (const char* Opt attribute ((unused)),
			const char* Arg attribute ((unused)))
/* Create a listing file */
{
    Listing = 1;
}



static void OptPageLength (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --pagelength option */
{
    int Len = atoi (Arg);
    if (Len != -1 && (Len < MIN_PAGE_LEN || Len > MAX_PAGE_LEN)) {
	AbEnd ("Invalid page length: %d", Len);
    }
    PageLength = Len;
}



static void OptSmart (const char* Opt attribute ((unused)),
		      const char* Arg attribute ((unused)))
/* Handle the -s/--smart options */
{
    SmartMode = 1;
}



static void OptTarget (const char* Opt attribute ((unused)), const char* Arg)
/* Set the target system */
{
    /* Map the target name to a target id */
    Target = FindTarget (Arg);
    if (Target == TGT_UNKNOWN) {
     	AbEnd ("Invalid target name: `%s'", Arg);
    } else if (Target == TGT_MODULE) {
        AbEnd ("Cannot use `module' as a target for the assembler");
    }
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
       	     "ca65 V%u.%u.%u - %s\n",
       	     VER_MAJOR, VER_MINOR, VER_PATCH, Copyright);
}



static void DoPCAssign (void)
/* Start absolute code */
{
    long PC = ConstExpression ();
    if (PC < 0 || PC > 0xFFFFFF) {
	Error (ERR_RANGE);
    } else {
	SetAbsPC (PC);
    }
}



static void OneLine (void)
/* Assemble one line */
{
    char Ident [MAX_STR_LEN+1];
    int Done = 0;

    /* Initialize the new listing line if we are actually reading from file
     * and not from internally pushed input.
     */
    if (!HavePushedInput ()) {
     	InitListingLine ();
    }

    if (Tok == TOK_COLON) {
     	/* An unnamed label */
     	ULabDef ();
     	NextTok ();
    }

    /* Assemble the line */
    if (Tok == TOK_IDENT) {

     	/* Is it a macro? */
     	if (IsMacro (SVal)) {

     	    /* Yes, start a macro expansion */
     	    MacExpandStart ();
     	    Done = 1;

     	} else {

     	    /* No, label. Remember the identifier, then skip it */
     	    int HadWS = WS;	/* Did we have whitespace before the ident? */
     	    strcpy (Ident, SVal);
     	    NextTok ();

     	    /* If a colon follows, this is a label definition. If there
     	     * is no colon, it's an assignment.
     	     */
       	    if (Tok == TOK_EQ || Tok == TOK_ASSIGN) {
                /* If it's an assign token, we have a label */
                unsigned Flags = (Tok == TOK_ASSIGN)? SYM_LABEL : SYM_DEFAULT;
     	    	/* Skip the '=' */
     	    	NextTok ();
     	    	/* Define the symbol with the expression following the '=' */
     	    	SymDef (Ident, Expression(), Flags);
     	    	/* Don't allow anything after a symbol definition */
     	    	Done = 1;
     	    } else {
                /* Define the symbol flags */
                unsigned Flags = IsZPSeg ()? SYM_ZP | SYM_LABEL : SYM_LABEL;
     	    	/* Define a label */
     	    	SymDef (Ident, GenCurrentPC (), Flags);
     	    	/* Skip the colon. If NoColonLabels is enabled, allow labels
     	    	 * without a colon if there is no whitespace before the
     	    	 * identifier.
     	    	 */
     	    	if (Tok != TOK_COLON) {
     	    	    if (HadWS || !NoColonLabels) {
     	    	     	Error (ERR_COLON_EXPECTED);
     	    	    }
     	    	    if (Tok == TOK_NAMESPACE) {
     	    	       	/* Smart :: handling */
     	    	       	NextTok ();
     	    	    }
     	    	} else {
     	     	    /* Skip the colon */
     	    	    NextTok ();
     	    	}
     	    }
     	}
    }

    if (!Done) {

     	if (TokIsPseudo (Tok)) {
     	    /* A control command, IVal is index into table */
     	    HandlePseudo ();
     	} else if (Tok == TOK_MNEMO) {
     	    /* A mnemonic - assemble one instruction */
     	    HandleInstruction (IVal);
     	} else if (Tok == TOK_IDENT && IsMacro (SVal)) {
     	    /* A macro expansion */
     	    MacExpandStart ();
     	} else if (PCAssignment && (Tok == TOK_STAR || Tok == TOK_PC)) {
	    NextTok ();
	    if (Tok != TOK_EQ) {
		Error (ERR_EQ_EXPECTED);
		SkipUntilSep ();
	    } else {
		/* Skip the equal sign */
		NextTok ();
		/* Enter absolute mode */
		DoPCAssign ();
	    }
	}
    }

    /* Line separator must come here */
    ConsumeSep ();
}



static void Assemble (void)
/* Start the ball rolling ... */
{
    /* Prime the pump */
    NextTok ();

    /* Assemble lines until end of file */
    while (Tok != TOK_EOF) {
     	OneLine ();
    }
}



static void CreateObjFile (void)
/* Create the object file */
{
    /* Open the object, write the header */
    ObjOpen ();

    /* Write the object file options */
    WriteOptions ();

    /* Write the list of input files */
    WriteFiles ();

    /* Write the segment data to the file */
    WriteSegments ();

    /* Write the import list */
    WriteImports ();

    /* Write the export list */
    WriteExports ();

    /* Write debug symbols if requested */
    WriteDbgSyms ();

    /* Write line infos if requested */
    WriteLineInfo ();

    /* Write the string pool */
    WriteStrPool ();

    /* Write the assertions */
    WriteAssertions ();

    /* Write an updated header and close the file */
    ObjClose ();
}



int main (int argc, char* argv [])
/* Assembler main program */
{
    /* Program long options */
    static const LongOpt OptTab[] = {
        { "--auto-import",     	0,	OptAutoImport		},
        { "--cpu",     	       	1,	OptCPU 			},
	{ "--debug-info",      	0,	OptDebugInfo		},
	{ "--feature",		1,	OptFeature		},
	{ "--help",    		0,	OptHelp			},
	{ "--ignore-case",     	0,	OptIgnoreCase 		},
	{ "--include-dir",     	1,	OptIncludeDir		},
	{ "--listing", 	       	0,	OptListing		},
	{ "--pagelength",      	1,	OptPageLength		},
	{ "--smart",   	       	0,	OptSmart		},
	{ "--target",  		1,	OptTarget		},
	{ "--verbose", 	       	0,	OptVerbose		},
	{ "--version", 	       	0,	OptVersion		},
    };

    unsigned I;

    /* Initialize the cmdline module */
    InitCmdLine (&argc, &argv, "ca65");

    /* Enter the base lexical level. We must do that here, since we may
     * define symbols using -D.
     */
    SymEnterLevel (0);

    /* Check the parameters */
    I = 1;
    while (I < ArgCount) {

       	/* Get the argument */
       	const char* Arg = ArgVec [I];

       	/* Check for an option */
       	if (Arg [0] == '-') {
       	    switch (Arg [1]) {

		case '-':
		    LongOption (&I, OptTab, sizeof(OptTab)/sizeof(OptTab[0]));
		    break;

       	    	case 'g':
       		    OptDebugInfo (Arg, 0);
       		    break;

		case 'h':
		    OptHelp (Arg, 0);
		    break;

       	        case 'i':
       		    OptIgnoreCase (Arg, 0);
       		    break;

       		case 'l':
		    OptListing (Arg, 0);
       		    break;

       	        case 'o':
       		    OutFile = GetArg (&I, 2);
       	     	    break;

       		case 's':
       		    OptSmart (Arg, 0);
       		    break;

		case 't':
		    OptTarget (Arg, GetArg (&I, 2));
		    break;

       	       	case 'v':
		    OptVerbose (Arg, 0);
       	       	    break;

    	        case 'D':
    		    DefineSymbol (GetArg (&I, 2));
    		    break;

    		case 'I':
    		    OptIncludeDir (Arg, GetArg (&I, 2));
    		    break;

       	        case 'U':
		    OptAutoImport (Arg, 0);
       		    break;

       	        case 'V':
    	    	    OptVersion (Arg, 0);
       		    break;

       	        case 'W':
       		    WarnLevel = atoi (GetArg (&I, 2));
       		    break;

       	       	default:
       	       	    UnknownOption (Arg);
		    break;

     	    }
       	} else {
    	    /* Filename. Check if we already had one */
    	    if (InFile) {
    	       	fprintf (stderr, "%s: Don't know what to do with `%s'\n",
	     		 ProgName, Arg);
	     	exit (EXIT_FAILURE);
    	    } else {
	     	InFile = Arg;
	    }
     	}

	/* Next argument */
	++I;
    }

    /* Do we have an input file? */
    if (InFile == 0) {
	fprintf (stderr, "%s: No input files\n", ProgName);
	exit (EXIT_FAILURE);
    }

    /* If no CPU given, use the default CPU for the target */
    if (GetCPU () == CPU_UNKNOWN) {
        if (Target != TGT_UNKNOWN) {
            SetCPU (DefaultCPU[Target]);
        } else {
            SetCPU (CPU_6502);
        }
    }

    /* Intialize the target translation tables */
    TgtTranslateInit ();

    /* Initialize the scanner, open the input file */
    InitScanner (InFile);

    /* Define the default options */
    SetOptions ();

    /* Assemble the input */
    Assemble ();

    /* If we didn't have any errors, check the segment stack */
    if (ErrorCount == 0) {
        SegStackCheck ();
    }

    /* If we didn't have any errors, check the unnamed labels */
    if (ErrorCount == 0) {
        ULabCheck ();
    }

    /* If we didn't have any errors, check the symbol table */
    if (ErrorCount == 0) {
        SymCheck ();
    }

    /* If we didn't have any errors, check and resolve the segment data */
    if (ErrorCount == 0) {
        SegCheck ();
    }

    /* If we didn't have an errors, index the line infos */
    MakeLineInfoIndex ();

    /* Dump the data */
    if (Verbosity >= 2) {
        SymDump (stdout);
        SegDump ();
    }

    /* If we didn't have any errors, create the object and listing files */
    if (ErrorCount == 0) {
	CreateObjFile ();
	if (Listing) {
	    CreateListing ();
	}
    }

    /* Close the input file */
    DoneScanner ();

    /* Return an apropriate exit code */
    return (ErrorCount == 0)? EXIT_SUCCESS : EXIT_FAILURE;
}



