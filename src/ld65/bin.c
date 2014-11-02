/*****************************************************************************/
/*                                                                           */
/*                                   bin.c                                   */
/*                                                                           */
/*                  Module to handle the raw binary format                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1999-2012, Ullrich von Bassewitz                                      */
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
#include "alignment.h"
#include "print.h"
#include "xmalloc.h"

/* ld65 */
#include "bin.h"
#include "config.h"
#include "exports.h"
#include "expr.h"
#include "error.h"
#include "global.h"
#include "fileio.h"
#include "lineinfo.h"
#include "memarea.h"
#include "segments.h"
#include "spool.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



struct BinDesc {
    unsigned    Undef;          /* Count of undefined externals */
    FILE*       F;              /* Output file */
    const char* Filename;       /* Name of output file */
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



BinDesc* NewBinDesc (void)
/* Create a new binary format descriptor */
{
    /* Allocate memory for a new BinDesc struct */
    BinDesc* D = xmalloc (sizeof (BinDesc));

    /* Initialize the fields */
    D->Undef    = 0;
    D->F        = 0;
    D->Filename = 0;

    /* Return the created struct */
    return D;
}



void FreeBinDesc (BinDesc* D)
/* Free a binary format descriptor */
{
    xfree (D);
}



static unsigned BinWriteExpr (ExprNode* E, int Signed, unsigned Size,
                              unsigned long Offs attribute ((unused)),
                              void* Data)
/* Called from SegWrite for an expression. Evaluate the expression, check the
** range and write the expression value to the file.
*/
{
    /* There's a predefined function to handle constant expressions */
    return SegWriteConstExpr (((BinDesc*)Data)->F, E, Signed, Size);
}



static void PrintBoolVal (const char* Name, int B)
/* Print a boolean value for debugging */
{
    Print (stdout, 2, "      %s = %s\n", Name, B? "true" : "false");
}



static void PrintNumVal (const char* Name, unsigned long V)
/* Print a numerical value for debugging */
{
    Print (stdout, 2, "      %s = 0x%lx\n", Name, V);
}



static void BinWriteMem (BinDesc* D, MemoryArea* M)
/* Write the segments of one memory area to a file */
{
    unsigned I;

    /* Get the start address of this memory area */
    unsigned long Addr = M->Start;

    /* Debugging: Check that the file offset is correct */
    if (ftell (D->F) != (long)M->FileOffs) {
        Internal ("Invalid file offset for memory area %s: %ld/%lu",
                  GetString (M->Name), ftell (D->F), M->FileOffs);
    }

    /* Walk over all segments in this memory area */
    for (I = 0; I < CollCount (&M->SegList); ++I) {

        int DoWrite;

        /* Get the segment */
        SegDesc* S = CollAtUnchecked (&M->SegList, I);

        /* Keep the user happy */
        Print (stdout, 1, "    Writing `%s'\n", GetString (S->Name));

        /* Writes do only occur in the load area and not for BSS segments */
        DoWrite = (S->Flags & SF_BSS) == 0      &&      /* No BSS segment */
                   S->Load == M                 &&      /* LOAD segment */
                   S->Seg->Dumped == 0;                 /* Not already written */

        /* Output debugging stuff */
        PrintBoolVal ("bss", S->Flags & SF_BSS);
        PrintBoolVal ("LoadArea", S->Load == M);
        PrintBoolVal ("Dumped", S->Seg->Dumped);
        PrintBoolVal ("DoWrite", DoWrite);
        PrintNumVal  ("Address", Addr);
        PrintNumVal  ("FileOffs", (unsigned long) ftell (D->F));

        /* Check if the alignment for the segment from the linker config is
        ** a multiple for that of the segment.
        */
        if ((S->RunAlignment % S->Seg->Alignment) != 0) {
            /* Segment requires another alignment than configured
            ** in the linker.
            */
            Warning ("Segment `%s' is not aligned properly. Resulting "
                     "executable may not be functional.",
                     GetString (S->Name));
        }

        /* If this is the run memory area, we must apply run alignment. If
        ** this is not the run memory area but the load memory area (which
        ** means that both are different), we must apply load alignment.
        ** Beware: DoWrite may be true even if this is the run memory area,
        ** because it may be also the load memory area.
        */
        if (S->Run == M) {

            /* Handle ALIGN and OFFSET/START */
            if (S->Flags & SF_ALIGN) {
                /* Align the address */
                unsigned long NewAddr = AlignAddr (Addr, S->RunAlignment);
                if (DoWrite || (M->Flags & MF_FILL) != 0) {
                    WriteMult (D->F, M->FillVal, NewAddr - Addr);
                    PrintNumVal ("SF_ALIGN", NewAddr - Addr);
                }
                Addr = NewAddr;
            } else if (S->Flags & (SF_OFFSET | SF_START)) {
                unsigned long NewAddr = S->Addr;
                if (S->Flags & SF_OFFSET) {
                    /* It's an offset, not a fixed address, make an address */
                    NewAddr += M->Start;
                }
                if (DoWrite || (M->Flags & MF_FILL) != 0) {
                    WriteMult (D->F, M->FillVal, NewAddr-Addr);
                    PrintNumVal ("SF_OFFSET", NewAddr - Addr);
                }
                Addr = NewAddr;
            }

        } else if (S->Load == M) {

            /* Handle ALIGN_LOAD */
            if (S->Flags & SF_ALIGN_LOAD) {
                /* Align the address */
                unsigned long NewAddr = AlignAddr (Addr, S->LoadAlignment);
                if (DoWrite || (M->Flags & MF_FILL) != 0) {
                    WriteMult (D->F, M->FillVal, NewAddr - Addr);
                    PrintNumVal ("SF_ALIGN_LOAD", NewAddr - Addr);
                }
                Addr = NewAddr;
            }

        }

        /* Now write the segment to disk if it is not a BSS type segment and
        ** if the memory area is the load area.
        */
        if (DoWrite) {
            unsigned long P = ftell (D->F);
            SegWrite (D->Filename, D->F, S->Seg, BinWriteExpr, D);
            PrintNumVal ("Wrote", (unsigned long) (ftell (D->F) - P));
        } else if (M->Flags & MF_FILL) {
            WriteMult (D->F, S->Seg->FillVal, S->Seg->Size);
            PrintNumVal ("Filled", (unsigned long) S->Seg->Size);
        }

        /* If this was the load memory area, mark the segment as dumped */
        if (S->Load == M) {
            S->Seg->Dumped = 1;
        }

        /* Calculate the new address */
        Addr += S->Seg->Size;
    }

    /* If a fill was requested, fill the remaining space */
    if ((M->Flags & MF_FILL) != 0 && M->FillLevel < M->Size) {
        unsigned long ToFill = M->Size - M->FillLevel;
        Print (stdout, 2, "    Filling 0x%lx bytes with 0x%02x\n",
               ToFill, M->FillVal);
        WriteMult (D->F, M->FillVal, ToFill);
        M->FillLevel = M->Size;
    }
}



static int BinUnresolved (unsigned Name attribute ((unused)), void* D)
/* Called if an unresolved symbol is encountered */
{
    /* Unresolved symbols are an error in binary format. Bump the counter
    ** and return zero telling the caller that the symbol is indeed
    ** unresolved.
    */
    ((BinDesc*) D)->Undef++;
    return 0;
}



void BinWriteTarget (BinDesc* D, struct File* F)
/* Write a binary output file */
{
    unsigned I;

    /* Place the filename in the control structure */
    D->Filename = GetString (F->Name);

    /* Check for unresolved symbols. The function BinUnresolved is called
    ** if we get an unresolved symbol.
    */
    D->Undef = 0;               /* Reset the counter */
    CheckUnresolvedImports (BinUnresolved, D);
    if (D->Undef > 0) {
        /* We had unresolved symbols, cannot create output file */
        Error ("%u unresolved external(s) found - cannot create output file", D->Undef);
    }

    /* Open the file */
    D->F = fopen (D->Filename, "wb");
    if (D->F == 0) {
        Error ("Cannot open `%s': %s", D->Filename, strerror (errno));
    }

    /* Keep the user happy */
    Print (stdout, 1, "Opened `%s'...\n", D->Filename);

    /* Dump all memory areas */
    for (I = 0; I < CollCount (&F->MemoryAreas); ++I) {
        /* Get this entry */
        MemoryArea* M = CollAtUnchecked (&F->MemoryAreas, I);
        Print (stdout, 1, "  Dumping `%s'\n", GetString (M->Name));
        BinWriteMem (D, M);
    }

    /* Close the file */
    if (fclose (D->F) != 0) {
        Error ("Cannot write to `%s': %s", D->Filename, strerror (errno));
    }

    /* Reset the file and filename */
    D->F        = 0;
    D->Filename = 0;
}
