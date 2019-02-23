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

/* Linked list of memory area initialization addresses */
typedef struct XexInitAd {
    MemoryArea *InitMem;
    Import *InitAd;
    struct XexInitAd *next;
} XexInitAd;


struct XexDesc {
    unsigned    Undef;          /* Count of undefined externals */
    FILE*       F;              /* Output file */
    const char* Filename;       /* Name of output file */
    Import*     RunAd;          /* Run Address */
    XexInitAd*  InitAds;        /* List of Init Addresses */
    unsigned long HeadPos;      /* Position in the file of current header */
    unsigned long HeadEnd;      /* End address of current header */
    unsigned long HeadSize;     /* Last header size, can be removed if zero */
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
    D->InitAds  = 0;
    D->HeadPos  = 0;
    D->HeadEnd  = 0;
    D->HeadSize = 0;

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

XexInitAd* XexSearchInitMem(XexDesc* D, MemoryArea *InitMem)
{
    XexInitAd* I;
    for (I=D->InitAds; I != 0; I=I->next)
    {
        if (I->InitMem == InitMem)
            return I;
    }
    return NULL;
}


int XexAddInitAd (XexDesc* D, MemoryArea *InitMem, Import *InitAd)
/* Sets and INITAD for the given memory area */
{
    XexInitAd* I;

    /* Search for repeated entry */
    if (XexSearchInitMem (D, InitMem))
        return 1;

    I = xmalloc (sizeof (XexInitAd));
    I->InitAd  = InitAd;
    I->InitMem = InitMem;
    I->next    = D->InitAds;
    D->InitAds = I;
    return 0;
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



static void XexStartSegment (XexDesc *D, unsigned long Addr, unsigned long Size)
{
    /* Skip segment without size */
    if (!Size)
        return;

    /* Store current position */
    unsigned long Pos = ftell (D->F);
    unsigned long End = Addr + Size - 1;

    /* See if last header can be expanded into this one */
    if (D->HeadPos && ((D->HeadEnd + 1) == Addr)) {
        /* Expand current header */
        D->HeadEnd = End;
        D->HeadSize += Size;
        fseek (D->F, D->HeadPos + 2, SEEK_SET);
        Write16 (D->F, End);
        /* Seek to old position */
        fseek (D->F, Pos, SEEK_SET);
    }
    else
    {
        if (D->HeadSize == 0) {
            /* Last header had no data, replace */
            Pos = D->HeadPos;
            fseek (D->F, Pos, SEEK_SET);
        }

        /* If we are at start of file, write XEX heder */
        if (Pos == 0)
            Write16 (D->F, 0xFFFF);

        /* Writes a new segment header */
        D->HeadPos = ftell (D->F);
        D->HeadEnd = End;
        D->HeadSize = Size;
        Write16 (D->F, Addr);
        Write16 (D->F, End);
    }
}



static void XexFakeSegment (XexDesc *D, unsigned long Addr)
{
    /* See if last header can be expanded into this one, we are done */
    if (D->HeadPos && ((D->HeadEnd + 1) == Addr))
        return;

    /* If we are at start of file, write XEX heder */
    if (ftell (D->F) == 0)
        Write16 (D->F, 0xFFFF);

    /* Writes a new (invalid) segment header */
    D->HeadPos = ftell (D->F);
    D->HeadEnd = Addr - 1;
    D->HeadSize = 0;
    Write16 (D->F, Addr);
    Write16 (D->F, D->HeadEnd);
}



static unsigned long XexWriteMem (XexDesc* D, MemoryArea* M)
/* Write the segments of one memory area to a file */
{
    unsigned I;

    /* Store initial position to get total file size */
    unsigned long StartPos = ftell (D->F);

    /* Always write a segment header for each memory area */
    D->HeadPos = 0;

    /* Get the start address and size of this memory area */
    unsigned long Addr = M->Start;

    /* Walk over all segments in this memory area */
    for (I = 0; I < CollCount (&M->SegList); ++I) {

        int DoWrite;

        /* Get the segment */
        SegDesc* S = CollAtUnchecked (&M->SegList, I);

        /* Keep the user happy */
        Print (stdout, 1, "    ATARI EXE Writing `%s'\n", GetString (S->Name));

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
                    XexStartSegment (D, Addr, NewAddr - Addr);
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
                        XexStartSegment (D, Addr, NewAddr - Addr);
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
                    XexStartSegment (D, Addr, NewAddr - Addr);
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
            /* Start a segment with only one byte, will fix later */
            XexFakeSegment (D, Addr);
            unsigned long P = ftell (D->F);
            SegWrite (D->Filename, D->F, S->Seg, XexWriteExpr, D);
            unsigned long Size = ftell (D->F) - P;
            /* Fix segment size */
            XexStartSegment (D, Addr, Size);
            PrintNumVal ("Wrote", Size);
        } else if (M->Flags & MF_FILL) {
            XexStartSegment (D, Addr, S->Seg->Size);
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
        XexStartSegment (D, Addr, ToFill);
        WriteMult (D->F, M->FillVal, ToFill);
        M->FillLevel = M->Size;
    }

    /* If the last segment is empty, remove */
    if (D->HeadSize == 0 && D->HeadPos) {
        fseek (D->F, D->HeadPos, SEEK_SET);
    }

    return ftell (D->F) - StartPos;
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
        /* See if we have an init address for this area */
        XexInitAd* I = XexSearchInitMem (D, M);
        Print (stdout, 1, "  ATARI EXE Dumping `%s'\n", GetString (M->Name));
        if (XexWriteMem (D, M) && I) {
            Write16 (D->F, 0x2E2);
            Write16 (D->F, 0x2E3);
            Write16 (D->F, GetExportVal (I->InitAd->Exp));
        }
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
