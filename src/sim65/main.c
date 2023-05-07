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
#include <limits.h>

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

/* maximum number of cycles that can be tested,
** requires overhead for longest possible instruction,
** which should be 7, using 16 for safety.
*/
#define MAXCYCLES_LIMIT (ULONG_MAX-16)

/* Header signature 'sim65' */
static const unsigned char HeaderSignature[] = {
    0x73, 0x69, 0x6D, 0x36, 0x35
};
#define HEADER_SIGNATURE_LENGTH (sizeof(HeaderSignature)/sizeof(HeaderSignature[0]))

static const unsigned char HeaderVersion = 2;


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
    /* Guard against overflow. */
    if (MaxCycles >= MAXCYCLES_LIMIT) {
        Error("'-x parameter out of range. Max: %lu",MAXCYCLES_LIMIT);
    }
}

static unsigned char ReadProgramFile (void)
/* Load program into memory */
{
    unsigned I;
    int Val, Val2;
    int Version;
    unsigned Addr;
    unsigned Load, Reset;
    unsigned char SPAddr = 0x00;

    /* Open the file */
    FILE* F = fopen (ProgramFile, "rb");
    if (F == 0) {
        Error ("Cannot open '%s': %s", ProgramFile, strerror (errno));
    }

    /* Verify the header signature */
    for (I = 0; I < HEADER_SIGNATURE_LENGTH; ++I) {
        if ((Val = fgetc(F)) != HeaderSignature[I]) {
            Error ("'%s': Invalid header signature.", ProgramFile);
        }
    }

    /* Get header version */
    if ((Version = fgetc(F)) != HeaderVersion) {
        Error ("'%s': Invalid header version.", ProgramFile);
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

    /* Get load address */
    Val2 = 0; /* suppress uninitialized variable warning */
    if (((Val = fgetc(F)) == EOF) ||
        ((Val2 = fgetc(F)) == EOF)) {
        Error ("'%s': Header missing load address", ProgramFile);
    }
    Load = Val | (Val2 << 8);

    /* Get reset address */
    if (((Val = fgetc(F)) == EOF) ||
        ((Val2 = fgetc(F)) == EOF)) {
        Error ("'%s': Header missing reset address", ProgramFile);
    }
    Reset = Val | (Val2 << 8);

    /* Read the file body into memory */
    Addr = Load;
    while ((Val = fgetc(F)) != EOF) {
        if (Addr >= PARAVIRT_BASE) {
            Error ("'%s': To large to fit into $%04X-$%04X", ProgramFile, Addr, PARAVIRT_BASE);
        }
        MemWriteByte (Addr++, (unsigned char) Val);
    }

    /* Check for errors */
    if (ferror (F)) {
        Error ("Error reading from '%s': %s", ProgramFile, strerror (errno));
    }

    /* Close the file */
    fclose (F);

    Print (stderr, 1, "Loaded '%s' at $%04X-$%04X\n", ProgramFile, Load, Addr - 1);
    Print (stderr, 1, "File version: %d\n", Version);
    Print (stderr, 1, "Reset: $%04X\n", Reset);

    MemWriteWord(0xFFFC, Reset);
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
            ErrorCode (SIM65_ERROR_TIMEOUT, "Maximum number of cycles reached.");
        }
    }

    /* Return an apropriate exit code */
    return EXIT_SUCCESS;
}
