/*****************************************************************************/
/*                                                                           */
/*				    main.c				     */
/*                                                                           */
/*		   Main program for the ca65 macroassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
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
#include <ctype.h>
#include <time.h>

#include "../common/version.h"

#include "error.h"
#include "expr.h"
#include "global.h"
#include "instr.h"
#include "listing.h"
#include "macro.h"
#include "mem.h"
#include "objcode.h"
#include "objfile.h"
#include "options.h"
#include "pseudo.h"
#include "scanner.h"
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
    	     "Options:\n"
	     "\t-g\t\tAdd debug info to object file\n"
	     "\t-i\t\tIgnore case of symbols\n"
	     "\t-l\t\tCreate a listing if assembly was ok\n"
       	     "\t-o name\t\tName the output file\n"
	     "\t-s\t\tEnable smart mode\n"
    	     "\t-v\t\tIncrease verbosity\n"
	     "\t-D name[=value]\tDefine a symbol\n"
	     "\t-U\t\tMark unresolved symbols as import\n"
	     "\t-V\t\tPrint the assembler version\n"
	     "\t-W n\t\tSet warning level n\n"
	     "\t--cpu type\tSet cpu type\n"
	     "\t--pagelength n\tSet the page length for the listing\n"
       	     "\t--smart\t\tEnable smart mode\n",
    	     ProgName);
    exit (EXIT_FAILURE);
}



static void UnknownOption (const char* Arg)
/* Print an error about an unknown option. Print usage information and exit */
{
    fprintf (stderr, "Unknown option: %s\n", Arg);
    Usage ();
}



static void NeedArg (const char* Arg)
/* Print an error about a missing option argument and exit. */
{
    fprintf (stderr, "Option requires an argument: %s\n", Arg);
    exit (EXIT_FAILURE);
}



static void InvSym (const char* Def)
/* Print an error about an invalid symbol definition and die */
{
    fprintf (stderr, "Invalid symbol definition: `%s'\n", Def);
    exit (EXIT_FAILURE);
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
	    NeedArg (argv [*ArgNum]);
	}
	++(*ArgNum);
	return Arg;
    }
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
    if (Def [0] != '_' && !isalpha (Def [0])) {
	InvSym (Def);
    }
    P = Def;

    /* Copy the symbol, checking the rest */
    I = 0;
    while (isalnum (*P) || *P == '_') {
	if (I <= MAX_STR_LEN) {
	    SymName [I++] = *P;
	}
	++P;
    }
    SymName [I] = '\0';

    /* Do we have a value given? */
    if (*P != '=') {
	if (*P != '\0') {
	    InvSym (Def);
	}
	Val = 0;
    } else {
	/* We have a value */
	++P;
	if (*P == '$') {
	    ++P;
	    if (sscanf (P, "%lx", &Val) != 1) {
	 	InvSym (Def);
	    }
	} else {
	    if (sscanf (P, "%li", &Val) != 1) {
     		InvSym (Def);
	    }
       	}
    }

    /* Check if have already a symbol with this name */
    if (SymIsDef (SymName)) {
	fprintf (stderr, "`%s' is already defined\n", SymName);
	exit (EXIT_FAILURE);
    }

    /* Define the symbol */
    SymDef (SymName, LiteralExpr (Val), 0);
}



static void OptCPU (const char* Opt, const char* Arg)
/* Handle the --cpu option */
{
    if (Arg == 0) {
	NeedArg (Opt);
    }
    if (strcmp (Arg, "6502") == 0) {
	SetCPU (CPU_6502);
    } else if (strcmp (Arg, "65C02") == 0) {
	SetCPU (CPU_65C02);
    } else if (strcmp (Arg, "65816") == 0) {
	SetCPU (CPU_65816);
#ifdef SUNPLUS
    } else if (strcmp (Arg, "sunplus") == 0) {
	SetCPU (CPU_SUNPLUS);
#endif
    } else {
	fprintf (stderr, "Invalid CPU: `%s'\n", Arg);
	exit (EXIT_FAILURE);
    }
}



static void OptPageLength (const char* Opt, const char* Arg)
/* Handle the --pagelength option */
{
    int Len;
    if (Arg == 0) {
	NeedArg (Opt);
    }
    Len = atoi (Arg);
    if (Len != -1 && (Len < MIN_PAGE_LEN || Len > MAX_PAGE_LEN)) {
	fprintf (stderr, "Invalid page length: %d\n", Len);
	exit (EXIT_FAILURE);
    }
    PageLength = Len;
}



static void OptSmart (const char* Opt)
/* Handle the -s/--smart options */
{
    SmartMode = 1;
}



static void LongOption (int* ArgNum, char* argv [])
/* Handle a long command line option */
{
    const char* Opt = argv [*ArgNum];
    const char* Arg = argv [*ArgNum+1];

    if (strcmp (Opt, "--cpu") == 0) {
    	OptCPU (Opt, Arg);
    	++(*ArgNum);
    } else if (strcmp (Opt, "--pagelength") == 0) {
    	OptPageLength (Opt, Arg);
    	++(*ArgNum);
    } else if (strcmp (Opt, "--smart") == 0) {
       	OptSmart (Opt);
    } else {
        UnknownOption (Opt);
    }
}



static void OneLine (void)
/* Assemble one line */
{
    char Ident [MAX_STR_LEN+1];
    int Done = 0;

    /* Initialize the listing line */
    InitListingLine ();

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
       	    if (Tok == TOK_EQ) {
		/* Skip the '=' */
		NextTok ();
		/* Define the symbol with the expression following the
		 * '='
		 */
		SymDef (Ident, Expression (), 0);
		/* Don't allow anything after a symbol definition */
		Done = 1;
	    } else {
		/* Define a label */
		SymDef (Ident, CurrentPC (), IsZPSeg ());
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
	}
    }

    /* Calling InitListingLine again here is part of a hack that introduces
     * enough magic to make the PC output in the listing work.
     */
    InitListingLine ();

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

    /* Write an updated header and close the file */
    ObjClose ();
}



int main (int argc, char* argv [])
/* Assembler main program */
{
    int I;

    /* Set the program name */
    ProgName = argv [0];

    /* We must have a file name */
    if (argc < 2) {
    	Usage ();
    }

    /* Enter the base lexical level. We must do that here, since we may
     * define symbols using -D.
     */
    SymEnterLevel ();

    /* Check the parameters */
    I = 1;
    while (I < argc) {

       	/* Get the argument */
       	const char* Arg = argv [I];

       	/* Check for an option */
       	if (Arg [0] == '-') {
       	    switch (Arg [1]) {

		case '-':
		    LongOption (&I, argv);
		    break;

       		case 'g':
       		    DbgSyms = 1;
       		    break;

       	        case 'i':
       		    IgnoreCase = 1;
       		    break;

       		case 'l':
       		    Listing = 1;
       		    break;

       	        case 'o':
       		    OutFile = GetArg (&I, argv, 2);
       		    break;

       		case 's':
       		    OptSmart (Arg);
       		    break;

       	       	case 'v':
       	       	    ++Verbose;
       	       	    break;

	        case 'D':
		    DefineSymbol (GetArg (&I, argv, 2));
		    break;

       	        case 'U':
       		    AutoImport = 1;
       		    break;

       	        case 'V':
       		    fprintf (stderr,
       			     "ca65 V%u.%u.%u - (C) Copyright 1998-2000 Ullrich von Bassewitz\n",
       			     VER_MAJOR, VER_MINOR, VER_PATCH);
       		    break;

       	        case 'W':
       		    WarnLevel = atoi (GetArg (&I, argv, 2));
       		    break;

       	       	default:
       	       	    UnknownOption (Arg);
		    break;

     	    }
       	} else {
    	    /* Filename. Check if we already had one */
    	    if (InFile) {
    	       	fprintf (stderr, "Don't know what to do with `%s'\n", Arg);
    		Usage ();
    	    } else {
		InFile = Arg;
	    }
     	}

	/* Next argument */
	++I;
    }

    /* Do we have an input file? */
    if (InFile == 0) {
	fprintf (stderr, "No input file\n");
	exit (EXIT_FAILURE);
    }

    /* Initialize the scanner, open the input file */
    InitScanner (InFile);

    /* Define the default options */
    SetOptions ();

    /* Assemble the input */
    Assemble ();

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

    /* Dump the data */
    if (Verbose >= 2) {
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



