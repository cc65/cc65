/*****************************************************************************/
/*                                                                           */
/*                                  main.c                                   */
/*                                                                           */
/*             Main program of the od65 object file dump utility             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2012, Ullrich von Bassewitz                                      */
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

/* common */
#include "cmdline.h"
#include "objdefs.h"
#include "version.h"

/* od65 */
#include "dump.h"
#include "error.h"
#include "fileio.h"
#include "global.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



static unsigned FilesProcessed = 0;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void Usage (void)
/* Print usage information and exit */
{
    printf ("Usage: %s [options] file [options] [file]\n"
            "Short options:\n"
            "  -h\t\t\tHelp (this text)\n"
            "  -H\t\t\tDump the object file header\n"
            "  -S\t\t\tDump segments sizes\n"
            "  -V\t\t\tPrint the version number and exit\n"
            "\n"
            "Long options:\n"
            "  --dump-all\t\tDump all object file information\n"
            "  --dump-dbgsyms\tDump debug symbols\n"
            "  --dump-exports\tDump exported symbols\n"
            "  --dump-files\t\tDump the source files\n"
            "  --dump-header\t\tDump the object file header\n"
            "  --dump-imports\tDump imported symbols\n"
            "  --dump-lineinfo\tDump line information\n"
            "  --dump-options\tDump object file options\n"
            "  --dump-segments\tDump the segments in the file\n"
            "  --dump-segsize\tDump segments sizes\n"
            "  --help\t\tHelp (this text)\n"
            "  --version\t\tPrint the version number and exit\n",
            ProgName);
}



static void OptDumpAll (const char* Opt attribute ((unused)),
                        const char* Arg attribute ((unused)))
/* Dump all object file information */
{
    What |= D_ALL;
}



static void OptDumpDbgSyms (const char* Opt attribute ((unused)),
                            const char* Arg attribute ((unused)))
/* Dump debug symbols contained in the object file */
{
    What |= D_DBGSYMS;
}



static void OptDumpExports (const char* Opt attribute ((unused)),
                            const char* Arg attribute ((unused)))
/* Dump the exported symbols */
{
    What |= D_EXPORTS;
}



static void OptDumpFiles (const char* Opt attribute ((unused)),
                          const char* Arg attribute ((unused)))
/* Dump the source files */
{
    What |= D_FILES;
}



static void OptDumpHeader (const char* Opt attribute ((unused)),
                           const char* Arg attribute ((unused)))
/* Dump the object file header */
{
    What |= D_HEADER;
}



static void OptDumpImports (const char* Opt attribute ((unused)),
                            const char* Arg attribute ((unused)))
/* Dump the imported symbols */
{
    What |= D_IMPORTS;
}



static void OptDumpLineInfo (const char* Opt attribute ((unused)),
                             const char* Arg attribute ((unused)))
/* Dump the line infos */
{
    What |= D_LINEINFO;
}



static void OptDumpOptions (const char* Opt attribute ((unused)),
                            const char* Arg attribute ((unused)))
/* Dump the object file options */
{
    What |= D_OPTIONS;
}



static void OptDumpScopes (const char* Opt attribute ((unused)),
                           const char* Arg attribute ((unused)))
/* Dump the scopes in the object file */
{
    What |= D_SCOPES;
}



static void OptDumpSegments (const char* Opt attribute ((unused)),
                             const char* Arg attribute ((unused)))
/* Dump the segments in the object file */
{
    What |= D_SEGMENTS;
}



static void OptDumpSegSize (const char* Opt attribute ((unused)),
                            const char* Arg attribute ((unused)))
/* Dump the segments in the object file */
{
    What |= D_SEGSIZE;
}



static void OptHelp (const char* Opt attribute ((unused)),
                     const char* Arg attribute ((unused)))
/* Print usage information and exit */
{
    Usage ();
    exit (EXIT_SUCCESS);
}



static void OptVersion (const char* Opt attribute ((unused)),
                        const char* Arg attribute ((unused)))
/* Print the assembler version */
{
    fprintf (stderr, "%s V%s\n", ProgName, GetVersionAsString ());
}



static void DumpFile (const char* Name)
/* Dump information from the named file */
{
    unsigned long Magic;

    /* Try to open the file */
    FILE* F = fopen (Name, "rb");
    if (F == 0) {
        Error ("Cannot open `%s': %s", Name, strerror (errno));
    }

    /* Read the magic word */
    Magic = Read32 (F);

    /* Do we know this type of file? */
    if (Magic != OBJ_MAGIC) {

        /* Unknown format */
        printf ("%s: (no xo65 object file)\n", Name);

    } else if (What == 0) {

        /* Special handling if no info was requested */
        printf ("%s: (no information requested)\n", Name);

    } else {

        /* Print the filename */
        printf ("%s:\n", Name);

        /* Check what to dump */
        if (What & D_HEADER) {
            DumpObjHeader (F, 0);
        }
        if (What & D_OPTIONS) {
            DumpObjOptions (F, 0);
        }
        if (What & D_FILES) {
            DumpObjFiles (F, 0);
        }
        if (What & D_SEGMENTS) {
            DumpObjSegments (F, 0);
        }
        if (What & D_IMPORTS) {
            DumpObjImports (F, 0);
        }
        if (What & D_EXPORTS) {
            DumpObjExports (F, 0);
        }
        if (What & D_DBGSYMS) {
            DumpObjDbgSyms (F, 0);
        }
        if (What & D_LINEINFO) {
            DumpObjLineInfo (F, 0);
        }
        if (What & D_SCOPES) {
            DumpObjScopes (F, 0);
        }
        if (What & D_SEGSIZE) {
            DumpObjSegSize (F, 0);
        }
    }

    /* Close the file */
    fclose (F);
}



int main (int argc, char* argv [])
/* Assembler main program */
{
    /* Program long options */
    static const LongOpt OptTab[] = {
        { "--dump-all",         0,      OptDumpAll              },
        { "--dump-dbgsyms",     0,      OptDumpDbgSyms          },
        { "--dump-exports",     0,      OptDumpExports          },
        { "--dump-files",       0,      OptDumpFiles            },
        { "--dump-header",      0,      OptDumpHeader           },
        { "--dump-imports",     0,      OptDumpImports          },
        { "--dump-lineinfo",    0,      OptDumpLineInfo         },
        { "--dump-options",     0,      OptDumpOptions          },
        { "--dump-scopes",      0,      OptDumpScopes           },
        { "--dump-segments",    0,      OptDumpSegments         },
        { "--dump-segsize",     0,      OptDumpSegSize          },
        { "--help",             0,      OptHelp                 },
        { "--version",          0,      OptVersion              },
    };

    unsigned I;

    /* Initialize the cmdline module */
    InitCmdLine (&argc, &argv, "od65");

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

                case 'h':
                    OptHelp (Arg, 0);
                    break;

                case 'H':
                    OptDumpHeader (Arg, 0);
                    break;

                case 'S':
                    OptDumpSegSize (Arg, 0);
                    break;

                case 'V':
                    OptVersion (Arg, 0);
                    break;

                default:
                    UnknownOption (Arg);
                    break;

            }
        } else {
            /* Filename. Dump it. */
            DumpFile (Arg);
            ++FilesProcessed;
        }

        /* Next argument */
        ++I;
    }

    /* Print a message if we did not process any files */
    if (FilesProcessed == 0) {
        fprintf (stderr, "%s: No input files\n", ProgName);
    }

    /* Success */
    return EXIT_SUCCESS;
}
