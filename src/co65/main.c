/*****************************************************************************/
/*                                                                           */
/*                                  main.c                                   */
/*                                                                           */
/*              Main program for the co65 object file converter              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2009, Ullrich von Bassewitz                                      */
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

/* common */
#include "chartype.h"
#include "cmdline.h"
#include "debugflag.h"
#include "fname.h"
#include "print.h"
#include "segnames.h"
#include "version.h"
#include "xmalloc.h"
#include "xsprintf.h"

/* co65 */
#include "convert.h"
#include "error.h"
#include "global.h"
#include "model.h"
#include "o65.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void Usage (void)
/* Print usage information and exit */
{
    printf ("Usage: %s [options] file\n"
            "Short options:\n"
            "  -V\t\t\tPrint the version number\n"
            "  -g\t\t\tAdd debug info to object file\n"
            "  -h\t\t\tHelp (this text)\n"
            "  -m model\t\tOverride the o65 model\n"
            "  -n\t\t\tDon't generate an output file\n"
            "  -o name\t\tName the output file\n"
            "  -v\t\t\tIncrease verbosity\n"
            "\n"
            "Long options:\n"
            "  --bss-label name\tDefine and export a BSS segment label\n"
            "  --bss-name seg\tSet the name of the BSS segment\n"
            "  --code-label name\tDefine and export a CODE segment label\n"
            "  --code-name seg\tSet the name of the CODE segment\n"
            "  --data-label name\tDefine and export a DATA segment label\n"
            "  --data-name seg\tSet the name of the DATA segment\n"
            "  --debug-info\t\tAdd debug info to object file\n"
            "  --help\t\tHelp (this text)\n"
            "  --no-output\t\tDon't generate an output file\n"
            "  --o65-model model\tOverride the o65 model\n"
            "  --verbose\t\tIncrease verbosity\n"
            "  --version\t\tPrint the version number\n"
            "  --zeropage-label name\tDefine and export a ZEROPAGE segment label\n"
            "  --zeropage-name seg\tSet the name of the ZEROPAGE segment\n",
            ProgName);
}



static void CheckLabelName (const char* Label)
/* Check if the given label is a valid label name */
{
    const char* L = Label;

    if (strlen (L) < 256 && (IsAlpha (*L) || *L== '_')) {
        while (*++L) {
            if (!IsAlNum (*L) && *L != '_') {
                break;
            }
        }
    }

    if (*L) {
        Error ("Label name `%s' is invalid", Label);
    }
}



static void CheckSegName (const char* Seg)
/* Abort if the given name is not a valid segment name */
{
    /* Print an error and abort if the name is not ok */
    if (!ValidSegName (Seg)) {
        Error ("Segment name `%s' is invalid", Seg);
    }
}



static void OptBssLabel (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --bss-label option */
{
    /* Check for a label name */
    CheckLabelName (Arg);

    /* Set the label */
    BssLabel = xstrdup (Arg);
}



static void OptBssName (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --bss-name option */
{
    /* Check for a valid name */
    CheckSegName (Arg);

    /* Set the name */
    BssSeg = xstrdup (Arg);
}



static void OptCodeLabel (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --code-label option */
{
    /* Check for a label name */
    CheckLabelName (Arg);

    /* Set the label */
    CodeLabel = xstrdup (Arg);
}



static void OptCodeName (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --code-name option */
{
    /* Check for a valid name */
    CheckSegName (Arg);

    /* Set the name */
    CodeSeg = xstrdup (Arg);
}



static void OptDataLabel (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --data-label option */
{
    /* Check for a label name */
    CheckLabelName (Arg);

    /* Set the label */
    DataLabel = xstrdup (Arg);
}



static void OptDataName (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --data-name option */
{
    /* Check for a valid name */
    CheckSegName (Arg);

    /* Set the name */
    DataSeg = xstrdup (Arg);
}



static void OptDebug (const char* Opt attribute ((unused)),
                      const char* Arg attribute ((unused)))
/* Enable debugging code */
{
    ++Debug;
}



static void OptDebugInfo (const char* Opt attribute ((unused)),
                          const char* Arg attribute ((unused)))
/* Add debug info to the object file */
{
    DebugInfo = 1;
}



static void OptHelp (const char* Opt attribute ((unused)),
                     const char* Arg attribute ((unused)))
/* Print usage information and exit */
{
    Usage ();
    exit (EXIT_SUCCESS);
}



static void OptNoOutput (const char* Opt attribute ((unused)),
                         const char* Arg attribute ((unused)))
/* Handle the --no-output option */
{
    NoOutput = 1;
}



static void OptO65Model (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --o65-model option */
{
    /* Search for the model name */
    Model = FindModel (Arg);
    if (Model == O65_MODEL_INVALID) {
        Error ("Unknown o65 model `%s'", Arg);
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
    fprintf (stderr, "co65 V%s\n", GetVersionAsString ());
}



static void OptZeropageLabel (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --zeropage-label option */
{
    /* Check for a label name */
    CheckLabelName (Arg);

    /* Set the label */
    ZeropageLabel = xstrdup (Arg);
}



static void OptZeropageName (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --zeropage-name option */
{
    /* Check for a valid name */
    CheckSegName (Arg);

    /* Set the name */
    ZeropageSeg = xstrdup (Arg);
}



static void DoConversion (void)
/* Do file conversion */
{
    /* Read the o65 file into memory */
    O65Data* D = ReadO65File (InputName);

    /* Do the conversion */
    Convert (D);

    /* Free the o65 module data */
    /* ### */

}



int main (int argc, char* argv [])
/* Converter main program */
{
    /* Program long options */
    static const LongOpt OptTab[] = {
        { "--bss-label",        1,      OptBssLabel             },
        { "--bss-name",         1,      OptBssName              },
        { "--code-label",       1,      OptCodeLabel            },
        { "--code-name",        1,      OptCodeName             },
        { "--data-label",       1,      OptDataLabel            },
        { "--data-name",        1,      OptDataName             },
        { "--debug",            0,      OptDebug                },
        { "--debug-info",       0,      OptDebugInfo            },
        { "--help",             0,      OptHelp                 },
        { "--no-output",        0,      OptNoOutput             },
        { "--o65-model",        1,      OptO65Model             },
        { "--verbose",          0,      OptVerbose              },
        { "--version",          0,      OptVersion              },
        { "--zeropage-label",   1,      OptZeropageLabel        },
        { "--zeropage-name",    1,      OptZeropageName         },
    };

    unsigned I;

    /* Initialize the cmdline module */
    InitCmdLine (&argc, &argv, "co65");

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

                case 'm':
                    OptO65Model (Arg, GetArg (&I, 2));
                    break;

                case 'n':
                    OptNoOutput (Arg, 0);
                    break;

                case 'o':
                    OutputName = GetArg (&I, 2);
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
            if (InputName) {
                Error ("Don't know what to do with `%s'", Arg);
            } else {
                InputName = Arg;
            }
        }

        /* Next argument */
        ++I;
    }

    /* Do we have an input file? */
    if (InputName == 0) {
        Error ("No input file");
    }

    /* Generate the name of the output file if none was specified */
    if (OutputName == 0) {
        OutputName = MakeFilename (InputName, AsmExt);
    }

    /* Do the conversion */
    DoConversion ();

    /* Return an apropriate exit code */
    return EXIT_SUCCESS;
}
