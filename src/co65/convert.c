/*****************************************************************************/
/*                                                                           */
/*                                 convert.c                                 */
/*                                                                           */
/*       Actual conversion routines for the co65 object file converter       */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2011, Ullrich von Bassewitz                                      */
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
#include <string.h>
#include <errno.h>

/* common */
#include "debugflag.h"
#include "print.h"
#include "version.h"
#include "xmalloc.h"
#include "xsprintf.h"

/* co65 */
#include "error.h"
#include "global.h"
#include "model.h"
#include "o65.h"
#include "convert.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void PrintO65Stats (const O65Data* D)
/* Print information about the O65 file if --verbose is given */
{
    Print (stdout, 1, "Size of text segment:               %5lu\n", D->Header.tlen);
    Print (stdout, 1, "Size of data segment:               %5lu\n", D->Header.dlen);
    Print (stdout, 1, "Size of bss segment:                %5lu\n", D->Header.blen);
    Print (stdout, 1, "Size of zeropage segment:           %5lu\n", D->Header.zlen);
    Print (stdout, 1, "Number of imports:                  %5u\n", CollCount (&D->Imports));
    Print (stdout, 1, "Number of exports:                  %5u\n", CollCount (&D->Exports));
    Print (stdout, 1, "Number of text segment relocations: %5u\n", CollCount (&D->TextReloc));
    Print (stdout, 1, "Number of data segment relocations: %5u\n", CollCount (&D->DataReloc));
}



static void SetupSegLabels (FILE* F)
/* Setup the segment label names */
{
    if (BssLabel) {
        fprintf (F, ".export\t\t%s\n", BssLabel);
    } else {
        BssLabel = xstrdup ("BSS");
    }
    if (CodeLabel) {
        fprintf (F, ".export\t\t%s\n", CodeLabel);
    } else {
        CodeLabel = xstrdup ("CODE");
    }
    if (DataLabel) {
        fprintf (F, ".export\t\t%s\n", DataLabel);
    } else {
        DataLabel = xstrdup ("DATA");
    }
    if (ZeropageLabel) {
        fprintf (F, ".export\t\t%s\n", ZeropageLabel);
    } else {
        ZeropageLabel = xstrdup ("ZEROPAGE");
    }
}



static const char* LabelPlusOffs (const char* Label, long Offs)
/* Generate "Label+xxx" in a static buffer and return a pointer to the buffer */
{
    static char Buf[256];
    xsprintf (Buf, sizeof (Buf), "%s%+ld", Label, Offs);
    return Buf;
}



static const char* RelocExpr (const O65Data* D, unsigned char SegID,
                              unsigned long Val, const O65Reloc* R)
/* Generate the segment relative relocation expression. R is only used if the
** expression contains am import, and may be NULL if this is an error (which
** is then flagged).
*/
{
    const O65Import* Import;

    switch (SegID) {

        case O65_SEGID_UNDEF:
            if (R) {
                if (R->SymIdx >= CollCount (&D->Imports)) {
                    Error ("Import index out of range (input file corrupt)");
                }
                Import = CollConstAt (&D->Imports, R->SymIdx);
                return LabelPlusOffs (Import->Name, Val);
            } else {
                Error ("Relocation references an import which is not allowed here");
                return 0;
            }
            break;

        case O65_SEGID_TEXT:
            return LabelPlusOffs (CodeLabel, Val - D->Header.tbase);

        case O65_SEGID_DATA:
            return LabelPlusOffs (DataLabel, Val - D->Header.dbase);

        case O65_SEGID_BSS:
            return LabelPlusOffs (BssLabel, Val - D->Header.bbase);

        case O65_SEGID_ZP:
            return LabelPlusOffs (ZeropageLabel, Val - D->Header.zbase);

        case O65_SEGID_ABS:
            return LabelPlusOffs ("", Val);

        default:
            Internal ("Cannot handle this segment reference in reloc entry");
    }

    /* NOTREACHED */
    return 0;
}



static void ConvertImports (FILE* F, const O65Data* D)
/* Convert the imports */
{
    unsigned I;

    if (CollCount (&D->Imports) > 0) {
        for (I = 0; I < CollCount (&D->Imports); ++I) {

            /* Get the next import */
            const O65Import* Import = CollConstAt (&D->Imports, I);

            /* Import it by name */
            fprintf (F, ".import\t%s\n", Import->Name);
        }
        fprintf (F, "\n");
    }
}



static void ConvertExports (FILE* F, const O65Data* D)
/* Convert the exports */
{
    unsigned I;

    if (CollCount (&D->Exports) > 0) {
        for (I = 0; I < CollCount (&D->Exports); ++I) {

            /* Get the next import */
            const O65Export* Export = CollConstAt (&D->Exports, I);

            /* First define it */
            fprintf (F, "%s = %s\n",
                     Export->Name,
                     RelocExpr (D, Export->SegID, Export->Val, 0));

            /* Then export it by name */
            fprintf (F, ".export\t%s\n", Export->Name);
        }
        fprintf (F, "\n");
    }
}



static void ConvertSeg (FILE* F, const O65Data* D, const Collection* Relocs,
                        const unsigned char* Data, unsigned long Size)
/* Convert one segment */
{
    const O65Reloc* R;
    unsigned        RIdx;
    unsigned long   Byte;

    /* Get the pointer to the first relocation entry if there are any */
    R = (CollCount (Relocs) > 0)? CollConstAt (Relocs, 0) : 0;

    /* Initialize for the loop */
    RIdx = 0;
    Byte = 0;

    /* Walk over the segment data */
    while (Byte < Size) {

        if (R && R->Offs == Byte) {
            /* We've reached an entry that must be relocated */
            unsigned long Val;
            switch (R->Type) {

                case O65_RTYPE_WORD:
                    if (Byte >= Size - 1) {
                        Error ("Found WORD relocation, but not enough bytes left");
                    } else {
                        Val = (Data[Byte+1] << 8) + Data[Byte];
                        Byte += 2;
                        fprintf (F, "\t.word\t%s\n", RelocExpr (D, R->SegID, Val, R));
                    }
                    break;

                case O65_RTYPE_HIGH:
                    Val = (Data[Byte++] << 8) + R->Val;
                    fprintf (F, "\t.byte\t>(%s)\n", RelocExpr (D, R->SegID, Val, R));
                    break;

                case O65_RTYPE_LOW:
                    Val = Data[Byte++];
                    fprintf (F, "\t.byte\t<(%s)\n", RelocExpr (D, R->SegID, Val, R));
                    break;

                case O65_RTYPE_SEGADDR:
                    if (Byte >= Size - 2) {
                        Error ("Found SEGADDR relocation, but not enough bytes left");
                    } else {
                        Val = (((unsigned long) Data[Byte+2]) << 16) +
                              (((unsigned long) Data[Byte+1]) <<  8) +
                              (((unsigned long) Data[Byte+0]) <<  0) +
                              R->Val;
                        Byte += 3;
                        fprintf (F, "\t.faraddr\t%s\n", RelocExpr (D, R->SegID, Val, R));
                    }
                    break;

                case O65_RTYPE_SEG:
                    /* FALLTHROUGH for now */
                default:
                    Internal ("Cannot handle relocation type %d at %lu",
                              R->Type, Byte);
            }

            /* Get the next relocation entry */
            if (++RIdx < CollCount (Relocs)) {
                R = CollConstAt (Relocs, RIdx);
            } else {
                R = 0;
            }

        } else {
            /* Just a constant value */
            fprintf (F, "\t.byte\t$%02X\n", Data[Byte++]);
        }
    }

    fprintf (F, "\n");
}



static void ConvertCodeSeg (FILE* F, const O65Data* D)
/* Do code segment conversion */
{
    /* Header */
    fprintf (F,
             ";\n; CODE SEGMENT\n;\n"
             ".segment\t\"%s\"\n"
             "%s:\n",
             CodeSeg,
             CodeLabel);

    /* Segment data */
    ConvertSeg (F, D, &D->TextReloc, D->Text, D->Header.tlen);
}



static void ConvertDataSeg (FILE* F, const O65Data* D)
/* Do data segment conversion */
{
    /* Header */
    fprintf (F,
             ";\n; DATA SEGMENT\n;\n"
             ".segment\t\"%s\"\n"
             "%s:\n",
             DataSeg,
             DataLabel);

    /* Segment data */
    ConvertSeg (F, D, &D->DataReloc, D->Data, D->Header.dlen);
}



static void ConvertBssSeg (FILE* F, const O65Data* D)
/* Do bss segment conversion */
{
    /* Header */
    fprintf (F,
             ";\n; BSS SEGMENT\n;\n"
             ".segment\t\"%s\"\n"
             "%s:\n",
             BssSeg,
             BssLabel);

    /* Segment data */
    fprintf (F, "\t.res\t%lu\n", D->Header.blen);
    fprintf (F, "\n");
}



static void ConvertZeropageSeg (FILE* F, const O65Data* D)
/* Do zeropage segment conversion */
{
    /* Header */
    fprintf (F, ";\n; ZEROPAGE SEGMENT\n;\n");

    if (Model == O65_MODEL_CC65_MODULE) {
        /* o65 files of type cc65-module are linked together with a definition
        ** file for the zero page, but the zero page is not allocated in the
        ** module itself, but the locations are mapped to the zp locations of
        ** the main file.
        */
        fprintf (F, ".import\t__ZP_START__\t\t; Linker generated symbol\n");
        fprintf (F, "%s = __ZP_START__\n", ZeropageLabel);
    } else {
        /* Header */
        fprintf (F, ".segment\t\"%s\": zeropage\n%s:\n", ZeropageSeg, ZeropageLabel);

        /* Segment data */
        fprintf (F, "\t.res\t%lu\n", D->Header.zlen);
    }
    fprintf (F, "\n");
}



void Convert (const O65Data* D)
/* Convert the o65 file in D using the given output file. */
{
    FILE*       F;
    unsigned    I;
    char*       Author = 0;

    /* For now, we do only accept o65 files generated by the ld65 linker which
    ** have a specific format.
    */
    if (!Debug && D->Header.mode != O65_MODE_CC65) {
        Error ("Cannot convert o65 files of this type");
    }

    /* Output statistics */
    PrintO65Stats (D);

    /* Walk through the options and print them if verbose mode is enabled.
    ** Check for a os=cc65 option and bail out if we didn't find one (for
    ** now - later we switch to special handling).
    */
    for (I = 0; I < CollCount (&D->Options); ++I) {

        /* Get the next option */
        const O65Option* O = CollConstAt (&D->Options, I);

        /* Check the type of the option */
        switch (O->Type) {

            case O65_OPT_FILENAME:
                Print (stdout, 1, "O65 filename option:         `%s'\n",
                       GetO65OptionText (O));
                break;

            case O65_OPT_OS:
                if (O->Len == 2) {
                    Warning ("Operating system option without data found");
                } else {
                    Print (stdout, 1, "O65 operating system option: `%s'\n",
                           GetO65OSName (O->Data[0]));
                    switch (O->Data[0]) {
                        case O65_OS_CC65_MODULE:
                            if (Model != O65_MODEL_NONE &&
                                Model != O65_MODEL_CC65_MODULE) {
                                Warning ("Wrong o65 model for input file specified");
                            } else {
                                Model = O65_MODEL_CC65_MODULE;
                            }
                            break;
                    }
                }
                break;

            case O65_OPT_ASM:
                Print (stdout, 1, "O65 assembler option:        `%s'\n",
                       GetO65OptionText (O));
                break;

            case O65_OPT_AUTHOR:
                if (Author) {
                    xfree (Author);
                }
                Author = xstrdup (GetO65OptionText (O));
                Print (stdout, 1, "O65 author option:           `%s'\n", Author);
                break;

            case O65_OPT_TIMESTAMP:
                Print (stdout, 1, "O65 timestamp option:        `%s'\n",
                       GetO65OptionText (O));
                break;

            default:
                Warning ("Found unknown option, type %d, length %d",
                         O->Type, O->Len);
                break;
        }
    }

    /* If we shouldn't generate output, we're done here */
    if (NoOutput) {
        return;
    }

    /* Open the output file */
    F = fopen (OutputName, "w");
    if (F == 0) {
        Error ("Cannot open `%s': %s", OutputName, strerror (errno));
    }

    /* Create a header */
    fprintf (F, ";\n; File generated by co65 v %s using model `%s'\n;\n",
             GetVersionAsString (), GetModelName (Model));

    /* Select the CPU */
    if ((D->Header.mode & O65_CPU_MASK) == O65_CPU_65816) {
        fprintf (F, ".p816\n");
    }

    /* Object file options */
    fprintf (F, ".fopt\t\tcompiler,\"co65 v %s\"\n", GetVersionAsString ());
    if (Author) {
        fprintf (F, ".fopt\t\tauthor, \"%s\"\n", Author);
        xfree (Author);
        Author = 0;
    }

    /* Several other assembler options */
    fprintf (F, ".case\t\ton\n");
    fprintf (F, ".debuginfo\t%s\n", (DebugInfo != 0)? "on" : "off");

    /* Setup/export the segment labels */
    SetupSegLabels (F);

    /* End of header */
    fprintf (F, "\n");

    /* Imported identifiers */
    ConvertImports (F, D);

    /* Exported identifiers */
    ConvertExports (F, D);

    /* Code segment */
    ConvertCodeSeg (F, D);

    /* Data segment */
    ConvertDataSeg (F, D);

    /* BSS segment */
    ConvertBssSeg (F, D);

    /* Zero page segment */
    ConvertZeropageSeg (F, D);

    /* End of data */
    fprintf (F, ".end\n");
    fclose (F);
}
