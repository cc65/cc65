/*****************************************************************************/
/*                                                                           */
/*				    main.c				     */
/*                                                                           */
/*		    Main program for the da65 disassembler		     */
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

/* common */
#include "abend.h"
#include "cmdline.h"
#include "fname.h"
#include "version.h"

/* da65 */
#include "attrtab.h"
#include "code.h"
#include "config.h"
#include "cpu.h"
#include "data.h"
#include "error.h"
#include "global.h"
#include "opctable.h"
#include "output.h"
#include "scanner.h"



/*****************************************************************************/
/*     	       	       	       	     Code			  	     */
/*****************************************************************************/



static void Usage (void)
/* Print usage information and exit */
{
    fprintf (stderr,
    	     "Usage: %s [options] file\n"
    	     "Short options:\n"
       	     "  -g\t\t\tAdd debug info to object file\n"
       	     "  -h\t\t\tHelp (this text)\n"
       	     "  -o name\t\tName the output file\n"
       	     "  -v\t\t\tIncrease verbosity\n"
       	     "  -F\t\t\tAdd formfeeds to the output\n"
       	     "  -V\t\t\tPrint the assembler version\n"
	     "\n"
	     "Long options:\n"
       	     "  --cpu type\t\tSet cpu type\n"
	     "  --formfeeds\t\tAdd formfeeds to the output\n"
	     "  --help\t\tHelp (this text)\n"
       	     "  --pagelength n\tSet the page length for the listing\n"
       	     "  --verbose\t\tIncrease verbosity\n"
       	     "  --version\t\tPrint the assembler version\n",
    	     ProgName);
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
	AbEnd ("Invalid CPU: `%s'", Arg);
    }
}



static void OptFormFeeds (const char* Opt, const char* Arg)
/* Add form feeds to the output */
{
    FormFeeds = 1;
}



static void OptHelp (const char* Opt, const char* Arg)
/* Print usage information and exit */
{
    Usage ();
    exit (EXIT_SUCCESS);
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
	AbEnd ("Invalid page length: %d", Len);
    }
    PageLength = Len;
}



static void OptVerbose (const char* Opt, const char* Arg)
/* Increase verbosity */
{
    ++Verbosity;
}



static void OptVersion (const char* Opt, const char* Arg)
/* Print the disassembler version */
{
    fprintf (stderr,
       	     "da65 V%u.%u.%u - (C) Copyright 2000 Ullrich von Bassewitz\n",
       	     VER_MAJOR, VER_MINOR, VER_PATCH);
}



static void OneOpcode (unsigned RemainingBytes)
/* Disassemble one opcode */
{
    /* Get the opcode from the current address */
    unsigned char OPC = GetCodeByte (PC);

    /* Get the opcode description for the opcode byte */
    const OpcDesc* D = &OpcTable[OPC];

    /* If we have a label at this address, output the label */
    const char* Label = GetLabel (PC);
    if (Label) {
    	DefLabel (Label);
    }

    /* Check...
     *   - ...if we have enough bytes remaining for the code at this address.
     *   - ...if the current instruction is valid for the given CPU.
     *   - ...if there is no label somewhere between the instruction bytes.
     * If any of these conditions is true, switch to data mode.
     */
    if (GetStyle (PC) == atDefault) {
	if (D->Size > RemainingBytes) {
	    MarkAddr (PC, atIllegal);
       	} else if ((D->CPU & CPU) != CPU) {
	    MarkAddr (PC, atIllegal);
	} else {
	    unsigned I;
	    for (I = 1; I < D->Size; ++I) {
		if (HaveLabel (PC+I)) {
     		    MarkAddr (PC, atIllegal);
		    break;
		}
	    }
	}
    }

    /* Disassemble the line */
    switch (GetStyle (PC)) {

	case atDefault:
	case atCode:
	    D->Handler (D);
	    PC += D->Size;
	    break;

	case atByteTab:
	    ByteTable (RemainingBytes);
	    break;

	case atWordTab:
	    WordTable (RemainingBytes);
	    break;

	case atAddrTab:
	    AddrTable (RemainingBytes);
	    break;

	default:
	    DataByteLine (1);
	    ++PC;
	    break;

    }
}



static void OnePass (void)
/* Make one pass through the code */
{
    unsigned Count;

    /* Disassemble until nothing left */
    while ((Count = GetRemainingBytes()) > 0) {
      	OneOpcode (Count);
    }
}



static void Disassemble (void)
/* Disassemble the code */
{
    /* Pass 1 */
    Pass = 1;
    OnePass ();

    Output ("---------------------------");
    LineFeed ();

    /* Pass 2 */
    Pass = 2;
    ResetCode ();
    DefOutOfRangeLabels ();
    OnePass ();
}



int main (int argc, char* argv [])
/* Assembler main program */
{
    /* Program long options */
    static const LongOpt OptTab[] = {
        { "--cpu",     	       	1,	OptCPU 			},
	{ "--formfeeds",	0,	OptFormFeeds		},
      	{ "--help",    		0,	OptHelp			},
      	{ "--pagelength",      	1,	OptPageLength		},
      	{ "--verbose", 	       	0,	OptVerbose		},
      	{ "--version", 	       	0,	OptVersion		},
    };

    int I;

    /* Initialize the cmdline module */
    InitCmdLine (argc, argv, "da65");

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

       	        case 'o':
       		    OutFile = GetArg (&I, 2);
       		    break;

       	       	case 'v':
		    OptVerbose (Arg, 0);
       	       	    break;

       	        case 'V':
    	    	    OptVersion (Arg, 0);
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

    /* Must have an input file */
    if (InFile == 0) {
     	AbEnd ("No input file");
    }

    /* Make the config file name from the input file if none was given */
    if (!CfgAvail ()) {
	CfgSetName (MakeFilename (InFile, CfgExt));
    }

    /* Try to read the configuration file */
    CfgRead ();

    /* Make the output file name from the input file name if none was given */
    if (OutFile == 0) {
	OutFile = MakeFilename (InFile, OutExt);
    }

    /* Load the input file */
    LoadCode (InFile, 0xE000);	/* ### */

    /* Open the output file */
    OpenOutput (OutFile);

    /* Disassemble the code */
    Disassemble ();

    /* Close the output file */
    CloseOutput ();

    /* Done */
    return EXIT_SUCCESS;
}



