/*****************************************************************************/
/*                                                                           */
/*				    main.c				     */
/*                                                                           */
/*		    Main program for the da65 disassembler		     */
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
#include <ctype.h>
#include <time.h>

/* common */
#include "abend.h"
#include "cmdline.h"
#include "cpu.h"
#include "fname.h"
#include "print.h"
#include "version.h"

/* da65 */
#include "attrtab.h"
#include "code.h"
#include "data.h"
#include "error.h"
#include "global.h"
#include "infofile.h"
#include "opctable.h"
#include "output.h"
#include "scanner.h"



/*****************************************************************************/
/*     	       	       	       	     Code 			  	     */
/*****************************************************************************/



static void Usage (void)
/* Print usage information and exit */
{
    fprintf (stderr,
    	     "Usage: %s [options] [inputfile]\n"
    	     "Short options:\n"
       	     "  -g\t\t\tAdd debug info to object file\n"
       	     "  -h\t\t\tHelp (this text)\n"
             "  -i name\t\tSpecify an info file\n"
       	     "  -o name\t\tName the output file\n"
       	     "  -v\t\t\tIncrease verbosity\n"
       	     "  -F\t\t\tAdd formfeeds to the output\n"
	     "  -S addr\t\tSet the start/load address\n"
       	     "  -V\t\t\tPrint the disassembler version\n"
	     "\n"
	     "Long options:\n"
             "  --comments n\t\tSet the comment level for the output\n"
       	     "  --cpu type\t\tSet cpu type\n"
       	     "  --debug-info\t\tAdd debug info to object file\n"
	     "  --formfeeds\t\tAdd formfeeds to the output\n"
	     "  --help\t\tHelp (this text)\n"
             "  --info name\t\tSpecify an info file\n"
       	     "  --pagelength n\tSet the page length for the listing\n"
       	     "  --start-addr addr\tSet the start/load address\n"
       	     "  --verbose\t\tIncrease verbosity\n"
       	     "  --version\t\tPrint the disassembler version\n",
    	     ProgName);
}



static unsigned long CvtNumber (const char* Arg, const char* Number)
/* Convert a number from a string. Allow '$' and '0x' prefixes for hex
 * numbers.
 */
{
    unsigned long Val;
    int 	  Converted;
    char          BoundsCheck;

    /* Convert */
    if (*Number == '$') {
	++Number;
      	Converted = sscanf (Number, "%lx%c", &Val, &BoundsCheck);
    } else {
      	Converted = sscanf (Number, "%li%c", (long*)&Val, &BoundsCheck);
    }

    /* Check if we do really have a number */
    if (Converted != 1) {
       	Error ("Invalid number given in argument: %s\n", Arg);
    }

    /* Return the result */
    return Val;
}



static void OptComments (const char* Opt, const char* Arg)
/* Handle the --comments option */
{
    /* Convert the argument to a number */
    unsigned long Val = CvtNumber (Opt, Arg);

    /* Check for a valid range */
    if (Val > MAX_COMMENTS) {
        Error ("Argument for %s outside valid range (%d-%d)",
               Opt, MIN_COMMENTS, MAX_COMMENTS);
    }

    /* Use the value */
    Comments = (unsigned char) Val;
}



static void OptCPU (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --cpu option */
{
    /* Find the CPU from the given name */
    CPU = FindCPU (Arg);
    SetOpcTable (CPU);
}



static void OptDebugInfo (const char* Opt attribute ((unused)),
      		    	  const char* Arg attribute ((unused)))
/* Add debug info to the object file */
{
    DebugInfo = 1;
}



static void OptFormFeeds (const char* Opt attribute ((unused)),
			  const char* Arg attribute ((unused)))
/* Add form feeds to the output */
{
    FormFeeds = 1;
}



static void OptHelp (const char* Opt attribute ((unused)),
		     const char* Arg attribute ((unused)))
/* Print usage information and exit */
{
    Usage ();
    exit (EXIT_SUCCESS);
}



static void OptInfo (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --info option */
{
    InfoSetName (Arg);
}



static void OptPageLength (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --pagelength option */
{
    int Len = atoi (Arg);
    if (Len != 0 && (Len < MIN_PAGE_LEN || Len > MAX_PAGE_LEN)) {
	AbEnd ("Invalid page length: %d", Len);
    }
    PageLength = Len;
}



static void OptStartAddr (const char* Opt, const char* Arg)
/* Set the default start address */
{
    StartAddr = CvtNumber (Opt, Arg);
}



static void OptVerbose (const char* Opt attribute ((unused)),
			const char* Arg attribute ((unused)))
/* Increase verbosity */
{
    ++Verbosity;
}



static void OptVersion (const char* Opt attribute ((unused)),
			const char* Arg attribute ((unused)))
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
    if (MustDefLabel (PC)) {
	DefLabel (GetLabel (PC));
    }

    /* Check...
     *   - ...if we have enough bytes remaining for the code at this address.
     *   - ...if the current instruction is valid for the given CPU.
     *   - ...if there is no label somewhere between the instruction bytes.
     * If any of these conditions is true, switch to data mode.
     */
    if (GetStyleAttr (PC) == atDefault) {
	if (D->Size > RemainingBytes) {
	    MarkAddr (PC, atIllegal);
       	} else if (D->Flags & flIllegal) {
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
    switch (GetStyleAttr (PC)) {

	case atDefault:
	case atCode:
	    D->Handler (D);
	    PC += D->Size;
	    break;

	case atByteTab:
	    ByteTable ();
	    break;

        case atDByteTab:
            DByteTable ();
            break;

	case atWordTab:
	    WordTable ();
	    break;

	case atDWordTab:
	    DWordTable ();
	    break;

	case atAddrTab:
	    AddrTable ();
	    break;

	case atRtsTab:
	    RtsTable ();
	    break;

	case atTextTab:
	    TextTable ();
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
    OutputSettings ();
    DefOutOfRangeLabels ();
    OnePass ();
}



int main (int argc, char* argv [])
/* Assembler main program */
{
    /* Program long options */
    static const LongOpt OptTab[] = {
        { "--comments",         1,      OptComments             },
        { "--cpu",     	       	1,	OptCPU 			},
       	{ "--debug-info",      	0,     	OptDebugInfo            },
	{ "--formfeeds",  	0,	OptFormFeeds		},
      	{ "--help",    	  	0,	OptHelp			},
       	{ "--info",    	       	1,     	OptInfo                 },
      	{ "--pagelength",      	1,	OptPageLength		},
	{ "--start-addr", 	1,	OptStartAddr		},
      	{ "--verbose", 	       	0,	OptVerbose		},
      	{ "--version", 	       	0,	OptVersion		},
    };

    unsigned I;

    /* Initialize the cmdline module */
    InitCmdLine (&argc, &argv, "da65");

    /* Check the parameters */
    I = 1;
    while (I < ArgCount) {

       	/* Get the argument */
       	const char* Arg = ArgVec[I];

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
       		    OptInfo (Arg, GetArg (&I, 2));
       		    break;

       	        case 'o':
       		    OutFile = GetArg (&I, 2);
       		    break;

       	       	case 'v':
		    OptVerbose (Arg, 0);
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

    /* Try to read the info file */
    ReadInfoFile ();

    /* Must have an input file */
    if (InFile == 0) {
     	AbEnd ("No input file");
    }

    /* Make the output file name from the input file name if none was given */
    if (OutFile == 0) {
	OutFile = MakeFilename (InFile, OutExt);
    }

    /* If no CPU given, use the default CPU */
    if (CPU == CPU_UNKNOWN) {
        CPU = CPU_6502;
    }

    /* Load the input file */
    LoadCode ();

    /* Open the output file */
    OpenOutput (OutFile);

    /* Disassemble the code */
    Disassemble ();

    /* Close the output file */
    CloseOutput ();

    /* Done */
    return EXIT_SUCCESS;
}



