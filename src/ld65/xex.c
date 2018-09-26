/*****************************************************************************/
/*                                                                           */
/*                                   xex.c                                   */
/*                                                                           */
/*               Module to handle the Atari XEX binary format                */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2018 Daniel Serpell                                                   */
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
#include "xex.h"
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



struct XexDesc {
    unsigned    Undef;          /* Count of undefined externals */
    FILE*       F;              /* Output file */
    const char* Filename;       /* Name of output file */
    Import*     RunAd;          /* Run Address */
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



XexDesc* NewXexDesc (void)
/* Create a new XEX format descriptor */
{
    /* Allocate memory for a new XexDesc struct */
    XexDesc* D = xmalloc (sizeof (XexDesc));

    /* Initialize the fields */
    D->Undef    = 0;
    D->F        = 0;
    D->Filename = 0;
    D->RunAd    = 0;

    /* Return the created struct */
    return D;
}



void FreeXexDesc (XexDesc* D)
/* Free a XEX format descriptor */
{
    xfree (D);
}



void XexSetRunAd (XexDesc* D, Import *RunAd)
/* Set the RUNAD export */
{
    D->RunAd = RunAd;
}



static unsigned XexWriteExpr (ExprNode* E, int Signed, unsigned Size,
                              unsigned long Offs attribute ((unused)),
                              void* Data)
/* Called from SegWrite for an expression. Evaluate the expression, check the
** range and write the expression value to the file.
*/
{
    /* There's a predefined function to handle constant expressions */
    return SegWriteConstExpr (((XexDesc*)Data)->F, E, Signed, Size);
}



static void PrintNumVal (const char* Name, unsigned long V)
/* Print a numerical value for debugging */
{
    Print (stdout, 2, "      %s = 0x%lx\n", Name, V);
}



static void XexWriteMem (XexDesc* D, MemoryArea* M)
/* Write the segments of one memory area to a file */
{
    unsigned I;

    /* Get the start address and size of this memory area */
    unsigned long Addr = M->Start;

    /* Real size of the memory area, either the FillLevel or the Size */
    unsigned long Size = M->FillLevel;
    if ((M->Flags & MF_FILL) != 0 && M->FillLevel < M->Size)
        Size = M->Size;

    /* Write header */
    if (ftell (D->F) == 0)
        Write16(D->F, 0xFFFF);
    Write16(D->F, Addr);
    Write16(D->F, Addr + Size - 1);

    /* Walk over all segments in this memory area */
    for (I = 0; I < CollCount (&M->SegList); ++I) {

        int DoWrite;

        /* Get the segment */
        SegDesc* S = CollAtUnchecked (&M->SegList, I);

        /* Keep the user happy */
        Print (stdout, 1, "    Allocating `%s'\n", GetString (S->Name));

        /* Writes do only occur in the load area and not for BSS segments */
        DoWrite = (S->Flags & SF_BSS) == 0      &&      /* No BSS segment */
                   S->Load == M                 &&      /* LOAD segment */
                   S->Seg->Dumped == 0;                 /* Not already written */

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
                    /* "overwrite" segments are not supported */
                    if (S->Flags & SF_OVERWRITE) {
                        Error ("ATARI file format does not support overwrite for segment '%s'.",
                               GetString (S->Name));
                    } else {
                        WriteMult (D->F, M->FillVal, NewAddr-Addr);
                        PrintNumVal ("SF_OFFSET", NewAddr - Addr);
                    }
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
            SegWrite (D->Filename, D->F, S->Seg, XexWriteExpr, D);
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



static int XexUnresolved (unsigned Name attribute ((unused)), void* D)
/* Called if an unresolved symbol is encountered */
{
    /* Unresolved symbols are an error in XEX format. Bump the counter
    ** and return zero telling the caller that the symbol is indeed
    ** unresolved.
    */
    ((XexDesc*) D)->Undef++;
    return 0;
}



void XexWriteTarget (XexDesc* D, struct File* F)
/* Write a XEX output file */
{
    unsigned I;

    /* Place the filename in the control structure */
    D->Filename = GetString (F->Name);

    /* Check for unresolved symbols. The function XexUnresolved is called
    ** if we get an unresolved symbol.
    */
    D->Undef = 0;               /* Reset the counter */
    CheckUnresolvedImports (XexUnresolved, D);
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
        Print (stdout, 1, "  XEX Dumping `%s'\n", GetString (M->Name));
        XexWriteMem (D, M);
    }

    /* Write RUNAD at file end */
    if (D->RunAd) {
        Write16 (D->F, 0x2E0);
        Write16 (D->F, 0x2E1);
        Write16 (D->F, GetExportVal (D->RunAd->Exp));
    }

    /* Close the file */
    if (fclose (D->F) != 0) {
        Error ("Cannot write to `%s': %s", D->Filename, strerror (errno));
    }

    /* Reset the file and filename */
    D->F        = 0;
    D->Filename = 0;
}
