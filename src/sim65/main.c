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
#include <stdbool.h>
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
#include "peripherals.h"
#include "paravirt.h"
#include "trace.h"
#include "profile.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Name of program file */
const char* ProgramFile;

/* Set to True if CPU mode override is in effect. If set, the CPU is not read from the program file. */
static bool CPUOverrideActive = false;

/* exit simulator after MaxCycles Cccles */
unsigned long long MaxCycles = 0;

/* countdown from MaxCycles */
unsigned long long RemainCycles;

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
            "  -p <map, vice, or dbginfo file>\t\tEnable profiler\n"
            "  -v\t\t\tIncrease verbosity\n"
            "  -V\t\t\tPrint the simulator version number\n"
            "  -x <num>\t\tExit simulator after <num> cycles\n"
            "\n"
            "Long options:\n"
            "  --help\t\tHelp (this text)\n"
            "  --cycles\t\tPrint amount of executed CPU cycles\n"
            "  --cpu <type>\t\tOverride CPU type (6502, 65C02, 6502X)\n"
            "  --trace\t\tEnable CPU trace\n"
            "  --profile <mapfile>\t\tEnable profiler\n"
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



static void OptCPU (const char* Opt, const char* Arg)
/* Set CPU type */
{
    /* Don't use FindCPU here. Enum constants would clash. */
    if (strcmp(Arg, "6502") == 0) {
        CPU = CPU_6502;
        CPUOverrideActive = true;
    } else if (strcmp(Arg, "65C02") == 0 || strcmp(Arg, "65c02") == 0) {
        CPU = CPU_65C02;
        CPUOverrideActive = true;
    } else if (strcmp(Arg, "6502X") == 0 || strcmp(Arg, "6502x") == 0) {
        CPU = CPU_6502X;
        CPUOverrideActive = true;
    } else {
        AbEnd ("Invalid argument for %s: '%s'", Opt, Arg);
    }
}



static void OptTrace (const char* Opt attribute ((unused)),
                      const char* Arg attribute ((unused)))
/* Enable trace mode */
{
    TraceMode = TRACE_ENABLE_FULL; /* Enable full trace mode. */
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



static void OptProfile (const char* Opt attribute ((unused)),
                        const char* Arg)
/* Set flag to enable profiling at the end */
{
    enableProfiling = 1;
    symInfoFile = strdup(Arg);
}



static void OptVersion (const char* Opt attribute ((unused)),
                        const char* Arg attribute ((unused)))
/* Print the simulator version */
{
    fprintf (stderr, "%s V%s\n", ProgName, GetVersionAsString ());
    exit (EXIT_SUCCESS);
}



static void OptQuitXIns (const char* Opt attribute ((unused)),
                        const char* Arg)
/* Quit after MaxCycles cycles */
{
    MaxCycles = strtoull(Arg, NULL, 0);
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

    /* Get the CPU type from the file header.
     * Use it to set the CPU type, unless CPUOverrideActive is set.
     */
    if ((Val = fgetc(F)) != EOF) {
        if (!CPUOverrideActive) {
            switch (Val) {
            case CPU_6502:
            case CPU_65C02:
            case CPU_6502X:
                CPU = Val;
                break;
            default:
                Error ("'%s': Invalid CPU type", ProgramFile);
            }
        }
    }

    /* Get the address of c_sp from the file header */
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
        { "--help",             0,      OptHelp      },
        { "--cycles",           0,      OptCycles    },
        { "--cpu",              1,      OptCPU       },
        { "--trace",            0,      OptTrace     },
        { "--profile",          1,      OptProfile   },
        { "--verbose",          0,      OptVerbose   },
        { "--version",          0,      OptVersion   },
    };

    unsigned I;
    unsigned char SPAddr;
    unsigned int Cycles;

    /* Set reasonable defaults. */
    CPU = CPU_6502;
    TraceMode = TRACE_DISABLED; /* Disabled by default */

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

                case 'p':
                    OptProfile (Arg, GetArg (&I, 2));
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
    if (ProgramFile == NULL) {
        AbEnd ("No program file");
    }

    /* Reset memory */
    MemInit ();

    /* Reset peripherals. */
    PeripheralsInit ();

    /* Read program file into memory.
     * This also sets the CPU type, unless a CPU override is in effect.
     */
    SPAddr = ReadProgramFile ();

    /* Initialize the paravirtualization subsystem. It requires the stack pointer address, to be able to
     * simulate 6502 subroutine calls.
     */

    TraceInit(SPAddr);
    ParaVirtInit (I, SPAddr);

    /* Reset the CPU */
    Reset ();

    RemainCycles = MaxCycles;
    while (1) {
        Cycles = ExecuteInsn ();
        if (MaxCycles) {
            if (Cycles > RemainCycles) {
                ErrorCode (SIM65_ERROR_TIMEOUT, "Maximum number of cycles reached.");
            }
            RemainCycles -= Cycles;
        }
    }

    /* Unreachable. sim65 program must exit through paravirtual PVExit
    ** or timeout from MaxCycles producing an error.
    */
    return SIM65_ERROR;
}
