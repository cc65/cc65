/*****************************************************************************/
/*                                                                           */
/*				    main.c				     */
/*                                                                           */
/*                              sim65 main program                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002      Ullrich von Bassewitz                                       */
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
#include "cmdline.h"
#include "print.h"
#include "version.h"

/* sim65 */
#include "chip.h"
#include "chiplib.h"
#include "chippath.h"
#include "cpucore.h"
#include "cputype.h"
#include "global.h"
#include "memory.h"


/*****************************************************************************/
/*				     Code				     */
/*****************************************************************************/



static void Usage (void)
{
    fprintf (stderr,
	     "Usage: %s [options] file\n"
	     "Short options:\n"
       	     "  -V\t\t\tPrint the simulator version number\n"
       	     "  -d\t\t\tDebug mode\n"
       	     "  -h\t\t\tHelp (this text)\n"
       	     "  -v\t\t\tIncrease verbosity\n"
	     "\n"
	     "Long options:\n"
       	     "  --cpu type\t\tSet cpu type\n"
       	     "  --debug\t\tDebug mode\n"
	     "  --help\t\tHelp (this text)\n"
       	     "  --verbose\t\tIncrease verbosity\n"
       	     "  --version\t\tPrint the simulator version number\n",
	     ProgName);
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



static void OptDebug (const char* Opt attribute ((unused)),
		      const char* Arg attribute ((unused)))
/* Simulator debug mode */
{
    Debug = 1;
}



static void OptHelp (const char* Opt attribute ((unused)),
		     const char* Arg attribute ((unused)))
/* Print usage information and exit */
{
    Usage ();
    exit (EXIT_SUCCESS);
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
        { "--cpu",     	       	1, 	OptCPU 	     		},
       	{ "--debug",           	0,     	OptDebug     		},
	{ "--help",	 	0, 	OptHelp	     		},
	{ "--verbose",	       	0, 	OptVerbose   	       	},
	{ "--version",	       	0,	OptVersion   	       	},
    };

    unsigned I;

    /* Initialize the output file name */
    const char* InputFile  = 0;

    /* Initialize the cmdline module */
    InitCmdLine (&argc, &argv, "sim65");

    /* Parse the command line */
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

		case 'd':
		    OptDebug (Arg, 0);
		    break;

		case 'h':
		case '?':
	       	    OptHelp (Arg, 0);
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
       	    if (InputFile) {
       	       	fprintf (stderr, "additional file specs ignored\n");
       	    } else {
       	       	InputFile = Arg;
	    }
	}

	/* Next argument */
	++I;
    }

    /* Initialize modules */
    AddChipPath ("chips");
    LoadChipLibrary ("ram.so");
    LoadChips ();
    MemInit ();
    MemLoad ("uz.bin", 0x200, 0);
    CPUInit ();
    CPURun ();

    /* Return an apropriate exit code */
    return EXIT_SUCCESS;
}



