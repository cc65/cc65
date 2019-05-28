/*****************************************************************************/
/*                                                                           */
/*                                  main.c                                   */
/*                                                                           */
/*                              sim65 main program                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2009, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
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



#include <string.h>
#include <stdlib.h>
#include <errno.h>

/* common */
#include "abend.h"
#include "cmdline.h"
#include "print.h"
#include "version.h"

/* sim65 */
#include "6502.h"
#include "error.h"
#include "memory.h"
#include "paravirt.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Name of program file */
const char* ProgramFile;

/* exit simulator after MaxCycles Cycles */
unsigned long MaxCycles;

/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void Usage (void)
{
    printf ("Usage: %s [options] file [arguments]\n"
            "Short options:\n"
            "  -h\t\t\tHelp (this text)\n"
            "  -c\t\t\tPrint amount of executed CPU cycles\n"
            "  -v\t\t\tIncrease verbosity\n"
            "  -V\t\t\tPrint the simulator version number\n"
            "  -x <num>\t\tExit simulator after <num> cycles\n"
            "\n"
            "Long options:\n"
            "  --help\t\tHelp (this text)\n"
            "  --cycles\t\tPrint amount of executed CPU cycles\n"
            "  --verbose\t\tIncrease verbosity\n"
            "  --version\t\tPrint the simulator version number\n",
            ProgName);
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



static void OptCycles (const char* Opt attribute ((unused)),
                       const char* Arg attribute ((unused)))
/* Set flag to print amount of cycles at the end */
{
    PrintCycles = 1;
}



static void OptVersion (const char* Opt attribute ((unused)),
                        const char* Arg attribute ((unused)))
/* Print the simulator version */
{
    fprintf (stderr, "%s V%s\n", ProgName, GetVersionAsString ());
    exit(EXIT_SUCCESS);
}

static void OptQuitXIns (const char* Opt attribute ((unused)),
                        const char* Arg attribute ((unused)))
/* quit after MaxCycles cycles */
{
    MaxCycles = strtoul(Arg, NULL, 0);
}

static unsigned char ReadProgramFile (void)
/* Load program into memory */
{
    int Val;
    unsigned Addr = 0x0200;
    unsigned char SPAddr = 0x0000;

    /* Open the file */
    FILE* F = fopen (ProgramFile, "rb");
    if (F == 0) {
        Error ("Cannot open '%s': %s", ProgramFile, strerror (errno));
    }

    /* Get the CPU type from the file header */
    if ((Val = fgetc(F)) != EOF) {
        if (Val != CPU_6502 && Val != CPU_65C02) {
            Error ("'%s': Invalid CPU type", ProgramFile);
        }
        CPU = Val;
    }

    /* Get the address of sp from the file header */
    if ((Val = fgetc(F)) != EOF) {
        SPAddr = Val;
    }

    /* Read the file body into memory */
    while ((Val = fgetc(F)) != EOF) {
        if (Addr == 0xFF00) {
            Error ("'%s': To large to fit into $0200-$FFF0", ProgramFile);
        }
        MemWriteByte (Addr++, (unsigned char) Val);
    }

    /* Check for errors */
    if (ferror (F)) {
        Error ("Error reading from '%s': %s", ProgramFile, strerror (errno));
    }

    /* Close the file */
    fclose (F);

    Print (stderr, 1, "Loaded '%s' at $0200-$%04X\n", ProgramFile, Addr - 1);

    return SPAddr;
}



int main (int argc, char* argv[])
{
    /* Program long options */
    static const LongOpt OptTab[] = {
        { "--help",             0,      OptHelp                 },
        { "--cycles",           0,      OptCycles               },
        { "--verbose",          0,      OptVerbose              },
        { "--version",          0,      OptVersion              },
    };

    unsigned I;
    unsigned char SPAddr;

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

                case 'h':
                case '?':
                    OptHelp (Arg, 0);
                    break;

                case 'c':
                    OptCycles (Arg, 0);
                    break;

                case 'v':
                    OptVerbose (Arg, 0);
                    break;

                case 'V':
                    OptVersion (Arg, 0);
                    break;

                case 'x':
                    OptQuitXIns (Arg, GetArg (&I, 2));
                    break;

                default:
                    UnknownOption (Arg);
                    break;
            }
        } else {
            ProgramFile = Arg;
            break;
        }

        /* Next argument */
        ++I;
    }

    /* Do we have a program file? */
    if (ProgramFile == 0) {
        AbEnd ("No program file");
    }

    MemInit ();

    SPAddr = ReadProgramFile ();

    ParaVirtInit (I, SPAddr);

    Reset ();

    while (1) {
        ExecuteInsn ();
        if (MaxCycles && (GetCycles () >= MaxCycles)) {
            Error ("Maximum number of cycles reached.");
            exit (-99); /* do not use EXIT_FAILURE to avoid conflicts with the
                           same value being used in a test program */
        }
    }

    /* Return an apropriate exit code */
    return EXIT_SUCCESS;
}
