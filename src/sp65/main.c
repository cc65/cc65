/*****************************************************************************/
/*                                                                           */
/*                                  main.c                                   */
/*                                                                           */
/*            Main program of the sp65 sprite and bitmap utility             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2012,      Ullrich von Bassewitz                                      */
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
#include "abend.h"
#include "cmdline.h"
#include "print.h"
#include "version.h"

/* sp65 */
#include "attr.h"
#include "convert.h"
#include "error.h"
#include "input.h"
#include "output.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Bitmap first read */
static Bitmap* B;

/* Bitmap working copy */
static Bitmap* C;

/* Output data from convertion */
static StrBuf* D;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void Usage (void)
/* Print usage information and exit */
{
    printf (
            "Usage: %s [options] file [options] [file]\n"
            "Short options:\n"
            "  -V\t\t\t\tPrint the version number and exit\n"
            "  -c fmt[,attrlist]\t\tConvert into target format\n"
            "  -h\t\t\t\tHelp (this text)\n"
            "  -lc\t\t\t\tList all possible conversions\n"
            "  -r file[,attrlist]\t\tRead an input file\n"
            "  -v\t\t\t\tIncrease verbosity\n"
            "  -w file[,attrlist]\t\tWrite the output to a file\n"
            "\n"
            "Long options:\n"
            "  --convert-to fmt[,attrlist]\tConvert into target format\n"
            "  --help\t\t\tHelp (this text)\n"
            "  --list-conversions\t\tList all possible conversions\n"
            "  --pop\t\t\t\tRestore the original loaded image\n"
            "  --read file[,attrlist]\tRead an input file\n"
            "  --slice x,y,w,h\t\tGenerate a slice from the loaded bitmap\n"
            "  --verbose\t\t\tIncrease verbosity\n"
            "  --version\t\t\tPrint the version number and exit\n"
            "  --write file[,attrlist]\tWrite the output to a file\n",
            ProgName);
}



static void SetWorkBitmap (Bitmap* N)
/* Delete an old working bitmap and set a new one. The new one may be NULL
** to clear it.
*/
{
    /* If we have a distinct work bitmap, delete it */
    if (C != 0 && C != B) {
        FreeBitmap (C);
    }

    /* Set the new one */
    C = N;
}



static void SetOutputData (StrBuf* N)
/* Delete the old output data and replace it by the given one. The new one
** may be NULL to clear it.
*/
{
    /* Delete the old output data */
    if (D != 0) {
        FreeStrBuf (D);
    }

    /* Set the new one */
    D = N;
}



static void OptConvertTo (const char* Opt attribute ((unused)), const char* Arg)
/* Convert the bitmap into a target format */
{
    static const char* NameList[] = {
        "format"
    };

    /* Parse the argument */
    Collection* A = ParseAttrList (Arg, NameList, 2);

    /* We must have a bitmap */
    if (C == 0) {
        Error ("No bitmap to convert");
    }

    /* Convert the bitmap */
    SetOutputData (ConvertTo (C, A));

    /* Delete the attribute list */
    FreeAttrList (A);
}



static void OptDumpPalette (const char* Opt attribute ((unused)),
                            const char* Arg attribute ((unused)))
/* Dump the palette of the current work bitmap */
{
    /* We must have a bitmap ... */
    if (C == 0) {
        Error ("No bitmap");
    }

    /* ... which must be indexed */
    if (!BitmapIsIndexed (C)) {
        Error ("Current bitmap is not indexed");
    }

    /* Dump the palette */
    DumpPalette (stdout, GetBitmapPalette (C));
}



static void OptHelp (const char* Opt attribute ((unused)),
                     const char* Arg attribute ((unused)))
/* Print usage information and exit */
{
    Usage ();
    exit (EXIT_SUCCESS);
}



static void OptListConversions (const char* Opt attribute ((unused)),
                                const char* Arg attribute ((unused)))
/* Print a list of all conversions */
{
    ListConversionTargets (stdout);
    exit (EXIT_SUCCESS);
}



static void OptPop (const char* Opt attribute ((unused)),
                    const char* Arg attribute ((unused)))
/* Restore the original image */
{
    /* C and B must differ and we must have an original */
    if (B == 0 || C == 0 || C == B) {
        Error ("Nothing to pop");
    }

    /* Delete the changed image and restore the original one */
    SetWorkBitmap (B);
}



static void OptRead (const char* Opt attribute ((unused)), const char* Arg)
/* Read an input file */
{
    static const char* NameList[] = {
        "name", "format"
    };


    /* Parse the argument */
    Collection* A = ParseAttrList (Arg, NameList, 2);

    /* Clear the working copy */
    SetWorkBitmap (0);

    /* Delete the original */
    FreeBitmap (B);

    /* Read the file and use it as original and as working copy */
    B = C = ReadInputFile (A);

    /* Delete the attribute list */
    FreeAttrList (A);
}



static void OptSlice (const char* Opt attribute ((unused)), const char* Arg)
/* Generate a slice of a bitmap */
{
    unsigned X, Y, W, H;
    unsigned char T;

    /* We must have a bitmap otherwise we cannot slice */
    if (C == 0) {
        Error ("Nothing to slice");
    }

    /* The argument is X,Y,W,H */
    if (sscanf (Arg, "%u,%u,%u,%u,%c", &X, &Y, &W, &H, &T) != 4) {
        Error ("Invalid argument. Slice must be given as X,Y,W,H");
    }

    /* Check the coordinates to be within the original bitmap */
    if (W > BM_MAX_WIDTH || H > BM_MAX_HEIGHT ||
        X + W > GetBitmapWidth (C) ||
        Y + H > GetBitmapHeight (C)) {
        Error ("Invalid slice coordinates and/or size");
    }

    /* Create the slice */
    SetWorkBitmap (SliceBitmap (C, X, Y, W, H));
}



static void OptVerbose (const char* Opt attribute ((unused)),
                        const char* Arg attribute ((unused)))
/* Increase versbosity */
{
    ++Verbosity;
}



static void OptVersion (const char* Opt attribute ((unused)),
                        const char* Arg attribute ((unused)))
/* Print the assembler version */
{
    fprintf (stderr, "%s V%s\n", ProgName, GetVersionAsString ());
}



static void OptWrite (const char* Opt attribute ((unused)), const char* Arg)
/* Write an output file */
{
    static const char* NameList[] = {
        "name", "format"
    };


    /* Parse the argument */
    Collection* A = ParseAttrList (Arg, NameList, 2);

    /* We must have output data */
    if (D == 0) {
        Error ("No conversion, so there's nothing to write");
    }

    /* Write the file */
    WriteOutputFile (D, A, C);

    /* Delete the attribute list */
    FreeAttrList (A);
}



int main (int argc, char* argv [])
/* sp65 main program */
{
    /* Program long options */
    static const LongOpt OptTab[] = {
        { "--convert-to",       1,      OptConvertTo            },
        { "--dump-palette",     0,      OptDumpPalette          },
        { "--help",             0,      OptHelp                 },
        { "--list-conversions", 0,      OptListConversions      },
        { "--pop",              0,      OptPop                  },
        { "--read",             1,      OptRead                 },
        { "--slice",            1,      OptSlice                },
        { "--verbose",          0,      OptVerbose              },
        { "--version",          0,      OptVersion              },
        { "--write",            1,      OptWrite                },
    };

    unsigned I;

    /* Initialize the cmdline module */
    InitCmdLine (&argc, &argv, "sp65");

    /* Check the parameters */
    I = 1;
    while (I < ArgCount) {

        /* Get the argument */
        const char* Arg = ArgVec[I];

        /* Check for an option */
        if (Arg[0] == '-') {
            switch (Arg[1]) {

                case '-':
                    LongOption (&I, OptTab, sizeof(OptTab)/sizeof(OptTab[0]));
                    break;

                case 'V':
                    OptVersion (Arg, 0);
                    break;

                case 'c':
                    OptConvertTo (Arg, GetArg (&I, 2));
                    break;

                case 'h':
                    OptHelp (Arg, 0);
                    break;

                case 'l':
                    if (Arg[2] == 'c') {
                        OptListConversions (Arg, 0);
                    } else {
                        UnknownOption (Arg);
                    }
                    break;

                case 'r':
                    OptRead (Arg, GetArg (&I, 2));
                    break;

                case 'v':
                    OptVerbose (Arg, 0);
                    break;

                case 'w':
                    OptWrite (Arg, GetArg (&I, 2));
                    break;

                default:
                    UnknownOption (Arg);
                    break;

            }
        } else {
            /* We don't accept anything else */
            AbEnd ("Don't know what to do with `%s'", Arg);
        }

        /* Next argument */
        ++I;
    }

    /* Cleanup data */
    SetWorkBitmap (C);
    FreeBitmap (B);
    FreeStrBuf (D);

    /* Success */
    return EXIT_SUCCESS;
}
