/*****************************************************************************/
/*                                                                           */
/*                                   o65.c                                   */
/*                                                                           */
/*                  Module to handle the o65 binary format                   */
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
#include <limits.h>
#include <errno.h>
#include <time.h>

/* common */
#include "chartype.h"
#include "check.h"
#include "fname.h"
#include "print.h"
#include "version.h"
#include "xmalloc.h"

/* ld65 */
#include "error.h"
#include "exports.h"
#include "expr.h"
#include "fileio.h"
#include "global.h"
#include "lineinfo.h"
#include "memarea.h"
#include "o65.h"
#include "spool.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Header mode bits */
#define MF_CPU_65816    0x8000          /* Executable is for 65816 */
#define MF_CPU_6502     0x0000          /* Executable is for the 6502 */
#define MF_CPU_MASK     0x8000          /* Mask to extract CPU type */

#define MF_RELOC_PAGE   0x4000          /* Page wise relocation */
#define MF_RELOC_BYTE   0x0000          /* Byte wise relocation */
#define MF_RELOC_MASK   0x4000          /* Mask to extract relocation type */

#define MF_SIZE_32BIT   0x2000          /* All size words are 32bit */
#define MF_SIZE_16BIT   0x0000          /* All size words are 16bit */
#define MF_SIZE_MASK    0x2000          /* Mask to extract size */

#define MF_FTYPE_OBJ    0x1000          /* Object file */
#define MF_FTYPE_EXE    0x0000          /* Executable file */
#define MF_FTYPE_MASK   0x1000          /* Mask to extract type */

#define MF_ADDR_SIMPLE  0x0800          /* Simple addressing */
#define MF_ADDR_DEFAULT 0x0000          /* Default addressing */
#define MF_ADDR_MASK    0x0800          /* Mask to extract addressing */

#define MF_ALIGN_1      0x0000          /* Bytewise alignment */
#define MF_ALIGN_2      0x0001          /* Align words */
#define MF_ALIGN_4      0x0002          /* Align longwords */
#define MF_ALIGN_256    0x0003          /* Align pages (256 bytes) */
#define MF_ALIGN_MASK   0x0003          /* Mask to extract alignment */

/* The four o65 segment types. Note: These values are identical to the values
** needed for the segmentID in the o65 spec.
*/
#define O65SEG_UNDEF    0x00
#define O65SEG_ABS      0x01
#define O65SEG_TEXT     0x02
#define O65SEG_DATA     0x03
#define O65SEG_BSS      0x04
#define O65SEG_ZP       0x05

/* Relocation type codes for the o65 format */
#define O65RELOC_WORD   0x80
#define O65RELOC_HIGH   0x40
#define O65RELOC_LOW    0x20
#define O65RELOC_SEGADR 0xC0
#define O65RELOC_SEG    0xA0
#define O65RELOC_MASK   0xE0

/* O65 executable file header */
typedef struct O65Header O65Header;
struct O65Header {
    unsigned        Version;            /* Version number for o65 format */
    unsigned        Mode;               /* Mode word */
    unsigned long   TextBase;           /* Base address of text segment */
    unsigned long   TextSize;           /* Size of text segment */
    unsigned long   DataBase;           /* Base of data segment */
    unsigned long   DataSize;           /* Size of data segment */
    unsigned long   BssBase;            /* Base of bss segment */
    unsigned long   BssSize;            /* Size of bss segment */
    unsigned long   ZPBase;             /* Base of zeropage segment */
    unsigned long   ZPSize;             /* Size of zeropage segment */
    unsigned long   StackSize;          /* Requested stack size */
};

/* An o65 option */
typedef struct O65Option O65Option;
struct O65Option {
    O65Option*      Next;               /* Next in option list */
    unsigned char   Type;               /* Type of option */
    unsigned char   Len;                /* Data length */
    unsigned char   Data [1];           /* Data, dynamically allocated */
};

/* A o65 relocation table */
typedef struct O65RelocTab O65RelocTab;
struct O65RelocTab {
    unsigned        Size;               /* Size of the table */
    unsigned        Fill;               /* Amount used */
    unsigned char*  Buf;                /* Buffer, dynamically allocated */
};

/* Structure describing the format */
struct O65Desc {
    O65Header       Header;             /* File header */
    O65Option*      Options;            /* List of file options */
    ExtSymTab*      Exports;            /* Table with exported symbols */
    ExtSymTab*      Imports;            /* Table with imported symbols */
    unsigned        Undef;              /* Count of undefined symbols */
    FILE*           F;                  /* The file we're writing to */
    const char*     Filename;           /* Name of the output file */
    O65RelocTab*    TextReloc;          /* Relocation table for text segment */
    O65RelocTab*    DataReloc;          /* Relocation table for data segment */

    unsigned        TextCount;          /* Number of segments assigned to .text */
    SegDesc**       TextSeg;            /* Array of text segments */
    unsigned        DataCount;          /* Number of segments assigned to .data */
    SegDesc**       DataSeg;            /* Array of data segments */
    unsigned        BssCount;           /* Number of segments assigned to .bss */
    SegDesc**       BssSeg;             /* Array of bss segments */
    unsigned        ZPCount;            /* Number of segments assigned to .zp */
    SegDesc**       ZPSeg;              /* Array of zp segments */

    /* Temporary data for writing segments */
    unsigned long   SegSize;
    O65RelocTab*    CurReloc;
    long            LastOffs;
};

/* Structure for parsing expression trees */
typedef struct ExprDesc ExprDesc;
struct ExprDesc {
    O65Desc*        D;                  /* File format descriptor */
    long            Val;                /* The offset value */
    int             TooComplex;         /* Expression too complex */
    MemoryArea*     MemRef;             /* Memory reference if any */
    Segment*        SegRef;             /* Segment reference if any */
    Section*        SecRef;             /* Section reference if any */
    ExtSym*         ExtRef;             /* External reference if any */
};



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



static ExprDesc* InitExprDesc (ExprDesc* ED, O65Desc* D)
/* Initialize an ExprDesc structure for use with O65ParseExpr */
{
    ED->D          = D;
    ED->Val        = 0;
    ED->TooComplex = 0;
    ED->MemRef     = 0;
    ED->SegRef     = 0;
    ED->SecRef     = 0;
    ED->ExtRef     = 0;
    return ED;
}



static void WriteSize (const O65Desc* D, unsigned long Val)
/* Write a "size" word to the file */
{
    switch (D->Header.Mode & MF_SIZE_MASK) {
        case MF_SIZE_16BIT:     Write16 (D->F, (unsigned) Val); break;
        case MF_SIZE_32BIT:     Write32 (D->F, Val);            break;
        default:                Internal ("Invalid size in header: %04X", D->Header.Mode);
    }
}



static unsigned O65SegType (const SegDesc* S)
/* Map our own segment types into something o65 compatible */
{
    /* Check the segment type. Readonly segments are assign to the o65
    ** text segment, writeable segments that contain data are assigned
    ** to data, bss and zp segments are handled respectively.
    ** Beware: Zeropage segments have the SF_BSS flag set, so be sure
    ** to check SF_ZP first.
    */
    if (S->Flags & SF_RO) {
        return O65SEG_TEXT;
    } else if (S->Flags & SF_ZP) {
        return O65SEG_ZP;
    } else if (S->Flags & SF_BSS) {
        return O65SEG_BSS;
    } else {
        return O65SEG_DATA;
    }
}



static void CvtMemoryToSegment (ExprDesc* ED)
/* Convert a memory area into a segment by searching the list of run segments
** in this memory area and assigning the nearest one.
*/
{
    /* Get the memory area from the expression */
    MemoryArea* M = ED->MemRef;

    /* Remember the "nearest" segment and its offset */
    Segment* Nearest   = 0;
    unsigned long Offs = ULONG_MAX;

    /* Walk over all segments */
    unsigned I;
    for (I = 0; I < CollCount (&M->SegList); ++I) {

        /* Get the segment and check if it's a run segment */
        SegDesc* S = CollAtUnchecked (&M->SegList, I);
        if (S->Run == M) {

            unsigned long O;

            /* Get the segment from the segment descriptor */
            Segment* Seg = S->Seg;

            /* Check the PC. */
            if ((long) Seg->PC <= ED->Val && (O = (ED->Val - Seg->PC)) < Offs) {
                /* This is the nearest segment for now */
                Offs = O;
                Nearest = Seg;

                /* If we found an exact match, don't look further */
                if (Offs == 0) {
                    break;
                }
            }
        }
    }

    /* If we found a segment, use it and adjust the offset */
    if (Nearest) {
        ED->SegRef = Nearest;
        ED->MemRef = 0;
        ED->Val    -= Nearest->PC;
    }
}



static const SegDesc* FindSeg (SegDesc** const List, unsigned Count, const Segment* S)
/* Search for a segment in the given list of segment descriptors and return
** the descriptor for a segment if we found it, and NULL if not.
*/
{
    unsigned I;

    for (I = 0; I < Count; ++I) {
        if (List[I]->Seg == S) {
            /* Found */
            return List[I];
        }
    }

    /* Not found */
    return 0;
}



static const SegDesc* O65FindSeg (const O65Desc* D, const Segment* S)
/* Search for a segment in the segment lists and return it's segment descriptor */
{
    const SegDesc* SD;

    if ((SD = FindSeg (D->TextSeg, D->TextCount, S)) != 0) {
        return SD;
    }
    if ((SD = FindSeg (D->DataSeg, D->DataCount, S)) != 0) {
        return SD;
    }
    if ((SD = FindSeg (D->BssSeg, D->BssCount, S)) != 0) {
        return SD;
    }
    if ((SD = FindSeg (D->ZPSeg, D->ZPCount, S)) != 0) {
        return SD;
    }

    /* Not found */
    return 0;
}



/*****************************************************************************/
/*                            Expression handling                            */
/*****************************************************************************/



static void O65ParseExpr (ExprNode* Expr, ExprDesc* D, int Sign)
/* Extract and evaluate all constant factors in an subtree that has only
** additions and subtractions. If anything other than additions and
** subtractions are found, D->TooComplex is set to true.
*/
{
    Export* E;

    switch (Expr->Op) {

        case EXPR_LITERAL:
            D->Val += (Sign * Expr->V.IVal);
            break;

        case EXPR_SYMBOL:
            /* Get the referenced Export */
            E = GetExprExport (Expr);
            /* If this export has a mark set, we've already encountered it.
            ** This means that the export is used to define it's own value,
            ** which in turn means, that we have a circular reference.
            */
            if (ExportHasMark (E)) {
                CircularRefError (E);
            } else if (E->Expr == 0) {
                /* Dummy export, must be an o65 imported symbol */
                ExtSym* S = O65GetImport (D->D, E->Name);
                CHECK (S != 0);
                if (D->ExtRef) {
                    /* We cannot have more than one external reference in o65 */
                    D->TooComplex = 1;
                } else {
                    /* Remember the external reference */
                    D->ExtRef = S;
                }
            } else {
                MarkExport (E);
                O65ParseExpr (E->Expr, D, Sign);
                UnmarkExport (E);
            }
            break;

        case EXPR_SECTION:
            if (D->SecRef) {
                /* We cannot handle more than one segment reference in o65 */
                D->TooComplex = 1;
            } else {
                /* Remember the segment reference */
                D->SecRef = GetExprSection (Expr);
                /* Add the offset of the section to the constant value */
                D->Val += Sign * (D->SecRef->Offs + D->SecRef->Seg->PC);
            }
            break;

        case EXPR_SEGMENT:
            if (D->SegRef) {
                /* We cannot handle more than one segment reference in o65 */
                D->TooComplex = 1;
            } else {
                /* Remember the segment reference */
                D->SegRef = Expr->V.Seg;
                /* Add the offset of the segment to the constant value */
                D->Val += (Sign * D->SegRef->PC);
            }
            break;

        case EXPR_MEMAREA:
            if (D->MemRef) {
                /* We cannot handle more than one memory reference in o65 */
                D->TooComplex = 1;
            } else {
                /* Remember the memory area reference */
                D->MemRef = Expr->V.Mem;
                /* Add the start address of the memory area to the constant
                ** value
                */
                D->Val += (Sign * D->MemRef->Start);
            }
            break;

        case EXPR_PLUS:
            O65ParseExpr (Expr->Left, D, Sign);
            O65ParseExpr (Expr->Right, D, Sign);
            break;

        case EXPR_MINUS:
            O65ParseExpr (Expr->Left, D, Sign);
            O65ParseExpr (Expr->Right, D, -Sign);
            break;

        default:
            /* Expression contains illegal operators */
            D->TooComplex = 1;
            break;

    }
}



/*****************************************************************************/
/*                             Relocation tables                             */
/*****************************************************************************/



static O65RelocTab* NewO65RelocTab (void)
/* Create a new relocation table */
{
    /* Allocate a new structure */
    O65RelocTab* R = xmalloc (sizeof (O65RelocTab));

    /* Initialize the data */
    R->Size = 0;
    R->Fill = 0;
    R->Buf  = 0;

    /* Return the created struct */
    return R;
}



static void FreeO65RelocTab (O65RelocTab* R)
/* Free a relocation table */
{
    xfree (R->Buf);
    xfree (R);
}



static void O65RelocPutByte (O65RelocTab* R, unsigned B)
/* Put the byte into the relocation table */
{
    /* Do we have enough space in the buffer? */
    if (R->Fill == R->Size) {
        /* We need to grow the buffer */
        if (R->Size) {
            R->Size *= 2;
        } else {
            R->Size = 1024;     /* Initial size */
        }
        R->Buf = xrealloc (R->Buf, R->Size);
    }

    /* Put the byte into the buffer */
    R->Buf [R->Fill++] = (unsigned char) B;
}



static void O65RelocPutWord (O65RelocTab* R, unsigned W)
/* Put a word into the relocation table */
{
    O65RelocPutByte (R, W);
    O65RelocPutByte (R, W >> 8);
}



static void O65WriteReloc (O65RelocTab* R, FILE* F)
/* Write the relocation table to the given file */
{
    WriteData (F, R->Buf, R->Fill);
}



/*****************************************************************************/
/*                              Option handling                              */
/*****************************************************************************/



static O65Option* NewO65Option (unsigned Type, const void* Data, unsigned DataLen)
/* Allocate and initialize a new option struct */
{
    O65Option* O;

    /* Check the length */
    CHECK (DataLen <= 253);

    /* Allocate memory */
    O = xmalloc (sizeof (O65Option) - 1 + DataLen);

    /* Initialize the structure */
    O->Next     = 0;
    O->Type     = Type;
    O->Len      = DataLen;
    memcpy (O->Data, Data, DataLen);

    /* Return the created struct */
    return O;
}



static void FreeO65Option (O65Option* O)
/* Free an O65Option struct */
{
    xfree (O);
}



/*****************************************************************************/
/*                     Subroutines to write o65 sections                     */
/*****************************************************************************/



static void O65WriteHeader (O65Desc* D)
/* Write the header of the executable to the given file */
{
    static unsigned char Trailer [5] = {
        0x01, 0x00, 0x6F, 0x36, 0x35
    };

    O65Option* O;

    /* Write the fixed header */
    WriteData (D->F, Trailer, sizeof (Trailer));
    Write8    (D->F, D->Header.Version);
    Write16   (D->F, D->Header.Mode);
    WriteSize (D, D->Header.TextBase);
    WriteSize (D, D->Header.TextSize);
    WriteSize (D, D->Header.DataBase);
    WriteSize (D, D->Header.DataSize);
    WriteSize (D, D->Header.BssBase);
    WriteSize (D, D->Header.BssSize);
    WriteSize (D, D->Header.ZPBase);
    WriteSize (D, D->Header.ZPSize);
    WriteSize (D, D->Header.StackSize);

    /* Write the options */
    O = D->Options;
    while (O) {
        Write8 (D->F, O->Len + 2);              /* Account for len and type bytes */
        Write8 (D->F, O->Type);
        if (O->Len) {
            WriteData (D->F, O->Data, O->Len);
        }
        O = O->Next;
    }

    /* Write the end-of-options byte */
    Write8 (D->F, 0);
}



static unsigned O65WriteExpr (ExprNode* E, int Signed, unsigned Size,
                              unsigned long Offs, void* Data)
/* Called from SegWrite for an expression. Evaluate the expression, check the
** range and write the expression value to the file, update the relocation
** table.
*/
{
    long          Diff;
    unsigned      RefCount;
    long          BinVal;
    ExprNode*     Expr;
    ExprDesc      ED;
    unsigned char RelocType;

    /* Cast the Data pointer to its real type, an O65Desc */
    O65Desc* D = (O65Desc*) Data;

    /* Check for a constant expression */
    if (IsConstExpr (E)) {
        /* Write out the constant expression */
        return SegWriteConstExpr (((O65Desc*)Data)->F, E, Signed, Size);
    }

    /* We have a relocatable expression that needs a relocation table entry.
    ** Calculate the number of bytes between this entry and the last one, and
    ** setup all necessary intermediate bytes in the relocation table.
    */
    Offs += D->SegSize;         /* Calulate full offset */
    Diff = ((long) Offs) - D->LastOffs;
    while (Diff > 0xFE) {
        O65RelocPutByte (D->CurReloc, 0xFF);
        Diff -= 0xFE;
    }
    O65RelocPutByte (D->CurReloc, (unsigned char) Diff);

    /* Remember this offset for the next time */
    D->LastOffs = Offs;

    /* Determine the expression to relocate */
    Expr = E;
    if (E->Op == EXPR_BYTE0   || E->Op == EXPR_BYTE1 ||
        E->Op == EXPR_BYTE2   || E->Op == EXPR_BYTE3 ||
        E->Op == EXPR_WORD0   || E->Op == EXPR_WORD1 ||
        E->Op == EXPR_FARADDR || E->Op == EXPR_DWORD) {
        /* Use the real expression */
        Expr = E->Left;
    }

    /* Recursively collect information about this expression */
    O65ParseExpr (Expr, InitExprDesc (&ED, D), 1);

    /* We cannot handle more than one external reference */
    RefCount = (ED.MemRef != 0) + (ED.SegRef != 0) +
               (ED.SecRef != 0) + (ED.ExtRef != 0);
    if (RefCount > 1) {
        ED.TooComplex = 1;
    }

    /* If we have a memory area reference, we need to convert it into a
    ** segment reference. If we cannot do that, we cannot handle the
    ** expression.
    */
    if (ED.MemRef) {
        CvtMemoryToSegment (&ED);
        if (ED.SegRef == 0) {
            return SEG_EXPR_TOO_COMPLEX;
        }
    }

    /* Bail out if we cannot handle the expression */
    if (ED.TooComplex) {
        return SEG_EXPR_TOO_COMPLEX;
    }

    /* Safety: Check that we have exactly one reference */
    CHECK (RefCount == 1);

    /* Write out the offset that goes into the segment. */
    BinVal = ED.Val;
    switch (E->Op) {
        case EXPR_BYTE0:    BinVal &= 0xFF;                     break;
        case EXPR_BYTE1:    BinVal = (BinVal >>  8) & 0xFF;     break;
        case EXPR_BYTE2:    BinVal = (BinVal >> 16) & 0xFF;     break;
        case EXPR_BYTE3:    BinVal = (BinVal >> 24) & 0xFF;     break;
        case EXPR_WORD0:    BinVal &= 0xFFFF;                   break;
        case EXPR_WORD1:    BinVal = (BinVal >> 16) & 0xFFFF;   break;
        case EXPR_FARADDR:  BinVal &= 0xFFFFFFUL;               break;
        case EXPR_DWORD:    BinVal &= 0xFFFFFFFFUL;             break;
    }
    WriteVal (D->F, BinVal, Size);

    /* Determine the actual type of relocation entry needed from the
    ** information gathered about the expression.
    */
    if (E->Op == EXPR_BYTE0) {
        RelocType = O65RELOC_LOW;
    } else if (E->Op == EXPR_BYTE1) {
        RelocType = O65RELOC_HIGH;
    } else if (E->Op == EXPR_BYTE2) {
        RelocType = O65RELOC_SEG;
    } else {
        switch (Size) {

            case 1:
                RelocType = O65RELOC_LOW;
                break;

            case 2:
                RelocType = O65RELOC_WORD;
                break;

            case 3:
                RelocType = O65RELOC_SEGADR;
                break;

            case 4:
                /* 4 byte expression not supported by o65 */
                return SEG_EXPR_TOO_COMPLEX;

            default:
                Internal ("O65WriteExpr: Invalid expression size: %u", Size);
                RelocType = 0;          /* Avoid gcc warnings */
        }
    }

    /* Determine which segment we're referencing */
    if (ED.SegRef || ED.SecRef) {

        const SegDesc* Seg;

        /* Segment or section reference. */
        if (ED.SecRef) {
            /* Get segment from section */
            ED.SegRef = ED.SecRef->Seg;
        }

        /* Search for the segment and map it to it's o65 segmentID */
        Seg = O65FindSeg (D, ED.SegRef);
        if (Seg == 0) {
            /* For some reason, we didn't find this segment in the list of
            ** segments written to the o65 file.
            */
            return SEG_EXPR_INVALID;
        }
        RelocType |= O65SegType (Seg);
        O65RelocPutByte (D->CurReloc, RelocType);

        /* Output additional data if needed */
        switch (RelocType & O65RELOC_MASK) {
            case O65RELOC_HIGH:
                O65RelocPutByte (D->CurReloc, ED.Val & 0xFF);
                break;
            case O65RELOC_SEG:
                O65RelocPutWord (D->CurReloc, ED.Val & 0xFFFF);
                break;
        }

    } else if (ED.ExtRef) {
        /* Imported symbol */
        RelocType |= O65SEG_UNDEF;
        O65RelocPutByte (D->CurReloc, RelocType);
        /* Put the number of the imported symbol into the table */
        O65RelocPutWord (D->CurReloc, ExtSymNum (ED.ExtRef));

    } else {

        /* OOPS - something bad happened */
        Internal ("External reference not handled");

    }

    /* Success */
    return SEG_EXPR_OK;
}



static void O65WriteSeg (O65Desc* D, SegDesc** Seg, unsigned Count, int DoWrite)
/* Write one segment to the o65 output file */
{
    SegDesc* S;
    unsigned I;

    /* Initialize variables */
    D->SegSize  = 0;
    D->LastOffs = -1;

    /* Write out all segments */
    for (I = 0; I < Count; ++I) {

        /* Get the segment from the list node */
        S = Seg [I];

        /* Keep the user happy */
        Print (stdout, 1, "    Writing `%s'\n", GetString (S->Name));

        /* Write this segment */
        if (DoWrite) {
            SegWrite (D->Filename, D->F, S->Seg, O65WriteExpr, D);
        }

        /* Mark the segment as dumped */
        S->Seg->Dumped = 1;

        /* Calculate the total size */
        D->SegSize += S->Seg->Size;
    }

    /* Terminate the relocation table for this segment */
    if (D->CurReloc) {
        O65RelocPutByte (D->CurReloc, 0);
    }

    /* Check the size of the segment for overflow */
    if ((D->Header.Mode & MF_SIZE_MASK) == MF_SIZE_16BIT && D->SegSize > 0xFFFF) {
        Error ("Segment overflow in file `%s'", D->Filename);
    }

}



static void O65WriteTextSeg (O65Desc* D)
/* Write the code segment to the o65 output file */
{
    /* Initialize variables */
    D->CurReloc = D->TextReloc;

    /* Dump all text segments */
    O65WriteSeg (D, D->TextSeg, D->TextCount, 1);

    /* Set the size of the segment */
    D->Header.TextSize = D->SegSize;
}



static void O65WriteDataSeg (O65Desc* D)
/* Write the data segment to the o65 output file */
{
    /* Initialize variables */
    D->CurReloc = D->DataReloc;

    /* Dump all data segments */
    O65WriteSeg (D, D->DataSeg, D->DataCount, 1);

    /* Set the size of the segment */
    D->Header.DataSize = D->SegSize;
}



static void O65WriteBssSeg (O65Desc* D)
/* "Write" the bss segments to the o65 output file. This will only update
** the relevant header fields.
*/
{
    /* Initialize variables */
    D->CurReloc = 0;

    /* Dump all bss segments */
    O65WriteSeg (D, D->BssSeg, D->BssCount, 0);

    /* Set the size of the segment */
    D->Header.BssSize = D->SegSize;
}



static void O65WriteZPSeg (O65Desc* D)
/* "Write" the zeropage segments to the o65 output file. This will only update
** the relevant header fields.
*/
{
    /* Initialize variables */
    D->CurReloc = 0;

    /* Dump all zp segments */
    O65WriteSeg (D, D->ZPSeg, D->ZPCount, 0);

    /* Set the size of the segment */
    D->Header.ZPSize = D->SegSize;
}



static void O65WriteImports (O65Desc* D)
/* Write the list of imported symbols to the O65 file */
{
    const ExtSym* S;

    /* Write the number of imports */
    WriteSize (D, ExtSymCount (D->Imports));

    /* Write out the symbol names, zero terminated */
    S = ExtSymList (D->Imports);
    while (S) {
        /* Get the name */
        const char* Name = GetString (ExtSymName (S));
        /* And write it to the output file */
        WriteData (D->F, Name, strlen (Name) + 1);
        /* Next symbol */
        S = ExtSymNext (S);
    }
}



static void O65WriteTextReloc (O65Desc* D)
/* Write the relocation for the text segment to the output file */
{
    O65WriteReloc (D->TextReloc, D->F);
}



static void O65WriteDataReloc (O65Desc* D)
/* Write the relocation for the data segment to the output file */
{
    O65WriteReloc (D->DataReloc, D->F);
}



static void O65WriteExports (O65Desc* D)
/* Write the list of exports */
{
    const ExtSym* S;

    /* Write the number of exports */
    WriteSize (D, ExtSymCount (D->Exports));

    /* Write out the symbol information */
    S = ExtSymList (D->Exports);
    while (S) {

        ExprNode* Expr;
        unsigned char SegmentID;
        ExprDesc ED;

        /* Get the name */
        unsigned NameIdx = ExtSymName (S);
        const char* Name = GetString (NameIdx);

        /* Get the export for this symbol. We've checked before that this
        ** export does really exist, so if it is unresolved, or if we don't
        ** find it, there is an error in the linker code.
        */
        Export* E = FindExport (NameIdx);
        if (E == 0 || IsUnresolvedExport (E)) {
            Internal ("Unresolved export `%s' found in O65WriteExports", Name);
        }

        /* Get the expression for the symbol */
        Expr = E->Expr;

        /* Recursively collect information about this expression */
        O65ParseExpr (Expr, InitExprDesc (&ED, D), 1);

        /* We cannot handle expressions with imported symbols, or expressions
        ** with more than one segment reference here
        */
        if (ED.ExtRef != 0 || (ED.SegRef != 0 && ED.SecRef != 0)) {
            ED.TooComplex = 1;
        }

        /* Bail out if we cannot handle the expression */
        if (ED.TooComplex) {
            Error ("Expression for symbol `%s' is too complex", Name);
        }

        /* Determine the segment id for the expression */
        if (ED.SegRef != 0 || ED.SecRef != 0) {

            const SegDesc* Seg;

            /* Segment or section reference */
            if (ED.SecRef != 0) {
                ED.SegRef = ED.SecRef->Seg;     /* Get segment from section */
            }

            /* Search for the segment and map it to it's o65 segmentID */
            Seg = O65FindSeg (D, ED.SegRef);
            if (Seg == 0) {
                /* For some reason, we didn't find this segment in the list of
                ** segments written to the o65 file.
                */
                Error ("Segment for symbol `%s' is undefined", Name);
            }
            SegmentID = O65SegType (Seg);

        } else {

            /* Absolute value */
            SegmentID = O65SEG_ABS;

        }

        /* Write the name to the output file */
        WriteData (D->F, Name, strlen (Name) + 1);

        /* Output the segment id followed by the literal value */
        Write8 (D->F, SegmentID);
        WriteSize (D, ED.Val);

        /* Next symbol */
        S = ExtSymNext (S);
    }
}



/*****************************************************************************/
/*                                Public code                                */
/*****************************************************************************/



O65Desc* NewO65Desc (void)
/* Create, initialize and return a new O65 descriptor struct */
{
    /* Allocate a new structure */
    O65Desc* D = xmalloc (sizeof (O65Desc));

    /* Initialize the header */
    D->Header.Version   = 0;
    D->Header.Mode      = 0;
    D->Header.TextBase  = 0;
    D->Header.TextSize  = 0;
    D->Header.DataBase  = 0;
    D->Header.DataSize  = 0;
    D->Header.BssBase   = 0;
    D->Header.BssSize   = 0;
    D->Header.ZPBase    = 0;
    D->Header.ZPSize    = 0;
    D->Header.StackSize = 0;            /* Let OS choose a good value */

    /* Initialize other data */
    D->Options          = 0;
    D->Exports          = NewExtSymTab ();
    D->Imports          = NewExtSymTab ();
    D->Undef            = 0;
    D->F                = 0;
    D->Filename         = 0;
    D->TextReloc        = NewO65RelocTab ();
    D->DataReloc        = NewO65RelocTab ();
    D->TextCount        = 0;
    D->TextSeg          = 0;
    D->DataCount        = 0;
    D->DataSeg          = 0;
    D->BssCount         = 0;
    D->BssSeg           = 0;
    D->ZPCount          = 0;
    D->ZPSeg            = 0;

    /* Return the created struct */
    return D;
}



void FreeO65Desc (O65Desc* D)
/* Delete the descriptor struct with cleanup */
{
    /* Free the segment arrays */
    xfree (D->ZPSeg);
    xfree (D->BssSeg);
    xfree (D->DataSeg);
    xfree (D->TextSeg);

    /* Free the relocation tables */
    FreeO65RelocTab (D->DataReloc);
    FreeO65RelocTab (D->TextReloc);

    /* Free the option list */
    while (D->Options) {
        O65Option* O = D->Options;
        D->Options = D->Options->Next;
        FreeO65Option (O);
    }

    /* Free the external symbol tables */
    FreeExtSymTab (D->Exports);
    FreeExtSymTab (D->Imports);

    /* Free the struct itself */
    xfree (D);
}



void O65Set6502 (O65Desc* D)
/* Enable 6502 mode */
{
    D->Header.Mode = (D->Header.Mode & ~MF_CPU_MASK) | MF_CPU_6502;
}



void O65Set65816 (O65Desc* D)
/* Enable 816 mode */
{
    D->Header.Mode = (D->Header.Mode & ~MF_CPU_MASK) | MF_CPU_65816;
}



void O65SetSmallModel (O65Desc* D)
/* Enable a small memory model executable */
{
    D->Header.Mode = (D->Header.Mode & ~MF_SIZE_MASK) | MF_SIZE_16BIT;
}



void O65SetLargeModel (O65Desc* D)
/* Enable a large memory model executable */
{
    D->Header.Mode = (D->Header.Mode & ~MF_SIZE_MASK) | MF_SIZE_32BIT;
}



void O65SetAlignment (O65Desc* D, unsigned Alignment)
/* Set the executable alignment */
{
    /* Remove all alignment bits from the mode word */
    D->Header.Mode &= ~MF_ALIGN_MASK;

    /* Set the alignment bits */
    switch (Alignment) {
        case 1:   D->Header.Mode |= MF_ALIGN_1;   break;
        case 2:   D->Header.Mode |= MF_ALIGN_2;   break;
        case 4:   D->Header.Mode |= MF_ALIGN_4;   break;
        case 256: D->Header.Mode |= MF_ALIGN_256; break;
        default:  Error ("Invalid alignment for O65 format: %u", Alignment);
    }
}



void O65SetOption (O65Desc* D, unsigned Type, const void* Data, unsigned DataLen)
/* Set an o65 header option */
{
    /* Create a new option structure */
    O65Option* O = NewO65Option (Type, Data, DataLen);

    /* Insert it into the linked list */
    O->Next = D->Options;
    D->Options = O;
}



void O65SetOS (O65Desc* D, unsigned OS, unsigned Version, unsigned Id)
/* Set an option describing the target operating system */
{
    /* Setup the option data */
    unsigned char Opt[4];
    Opt[0] = OS;
    Opt[1] = Version;

    /* Write the correct option length */
    switch (OS) {

        case O65OS_CC65:
            /* Set the 16 bit id */
            Opt[2] = (unsigned char) Id;
            Opt[3] = (unsigned char) (Id >> 8);
            O65SetOption (D, O65OPT_OS, Opt, 4);
            break;

        default:
            /* No id for OS/A65, Lunix, and unknown OSes */
            O65SetOption (D, O65OPT_OS, Opt, 2);
            break;

    }
}



ExtSym* O65GetImport (O65Desc* D, unsigned Ident)
/* Return the imported symbol or NULL if not found */
{
    /* Retrieve the symbol from the table */
    return GetExtSym (D->Imports, Ident);
}



void O65SetImport (O65Desc* D, unsigned Ident)
/* Set an imported identifier */
{
    /* Insert the entry into the table */
    NewExtSym (D->Imports, Ident);
}



ExtSym* O65GetExport (O65Desc* D, unsigned Ident)
/* Return the exported symbol or NULL if not found */
{
    /* Retrieve the symbol from the table */
    return GetExtSym (D->Exports, Ident);
}



void O65SetExport (O65Desc* D, unsigned Ident)
/* Set an exported identifier */
{
    /* Get the export for this symbol and check if it does exist and is
    ** a resolved symbol.
    */
    Export* E = FindExport (Ident);
    if (E == 0 || IsUnresolvedExport (E)) {
        Error ("Unresolved export: `%s'", GetString (Ident));
    }

    /* Insert the entry into the table */
    NewExtSym (D->Exports, Ident);
}



static void O65SetupSegments (O65Desc* D, File* F)
/* Setup segment assignments */
{
    unsigned I;
    unsigned TextIdx, DataIdx, BssIdx, ZPIdx;

    /* Initialize the counters */
    D->TextCount = 0;
    D->DataCount = 0;
    D->BssCount  = 0;
    D->ZPCount   = 0;

    /* Walk over the memory list */
    for (I = 0; I < CollCount (&F->MemoryAreas); ++I) {
        /* Get this entry */
        MemoryArea* M = CollAtUnchecked (&F->MemoryAreas, I);

        /* Walk through the segment list and count the segment types */
        unsigned J;
        for (J = 0; J < CollCount (&M->SegList); ++J) {

            /* Get the segment */
            SegDesc* S = CollAtUnchecked (&M->SegList, J);

            /* Check the segment type. */
            switch (O65SegType (S)) {
                case O65SEG_TEXT:   D->TextCount++; break;
                case O65SEG_DATA:   D->DataCount++; break;
                case O65SEG_BSS:    D->BssCount++;  break;
                case O65SEG_ZP:     D->ZPCount++;   break;
                default:            Internal ("Invalid return from O65SegType");
            }
        }
    }

    /* Allocate memory according to the numbers */
    D->TextSeg = xmalloc (D->TextCount * sizeof (SegDesc*));
    D->DataSeg = xmalloc (D->DataCount * sizeof (SegDesc*));
    D->BssSeg  = xmalloc (D->BssCount  * sizeof (SegDesc*));
    D->ZPSeg   = xmalloc (D->ZPCount   * sizeof (SegDesc*));

    /* Walk again through the list and setup the segment arrays */
    TextIdx = DataIdx = BssIdx = ZPIdx = 0;
    for (I = 0; I < CollCount (&F->MemoryAreas); ++I) {
        /* Get this entry */
        MemoryArea* M = CollAtUnchecked (&F->MemoryAreas, I);

        /* Walk over the segment list and check the segment types */
        unsigned J;
        for (J = 0; J < CollCount (&M->SegList); ++J) {

            /* Get the segment */
            SegDesc* S = CollAtUnchecked (&M->SegList, J);

            /* Check the segment type. */
            switch (O65SegType (S)) {
                case O65SEG_TEXT:   D->TextSeg [TextIdx++] = S; break;
                case O65SEG_DATA:   D->DataSeg [DataIdx++] = S; break;
                case O65SEG_BSS:    D->BssSeg [BssIdx++]   = S; break;
                case O65SEG_ZP:     D->ZPSeg [ZPIdx++]     = S; break;
                default:            Internal ("Invalid return from O65SegType");
            }
        }
    }
}



static int O65Unresolved (unsigned Name, void* D)
/* Called if an unresolved symbol is encountered */
{
    /* Check if the symbol is an imported o65 symbol */
    if (O65GetImport (D, Name) != 0) {
        /* This is an external symbol, relax... */
        return 1;
    } else {
        /* This is actually an unresolved external. Bump the counter */
        ((O65Desc*) D)->Undef++;
        return 0;
    }
}



static void O65SetupHeader (O65Desc* D)
/* Set additional stuff in the header */
{
    /* Set the base addresses of the segments */
    if (D->TextCount > 0) {
        SegDesc* FirstSeg  = D->TextSeg [0];
        D->Header.TextBase = FirstSeg->Seg->PC;
    }
    if (D->DataCount > 0) {
        SegDesc* FirstSeg  = D->DataSeg [0];
        D->Header.DataBase = FirstSeg->Seg->PC;
    }
    if (D->BssCount > 0) {
        SegDesc* FirstSeg  = D->BssSeg [0];
        D->Header.BssBase = FirstSeg->Seg->PC;
    }
    if (D->ZPCount > 0) {
        SegDesc* FirstSeg = D->ZPSeg [0];
        D->Header.ZPBase  = FirstSeg->Seg->PC;
    }
}



static void O65UpdateHeader  (O65Desc* D)
/* Update mode word, currently only the "simple" bit */
{
    /* If we have byte wise relocation and an alignment of 1, and text
    ** and data are adjacent, we can set the "simple addressing" bit
    ** in the header.
    */
    if ((D->Header.Mode & MF_RELOC_MASK) == MF_RELOC_BYTE &&
        (D->Header.Mode & MF_ALIGN_MASK) == MF_ALIGN_1 &&
        D->Header.TextBase + D->Header.TextSize == D->Header.DataBase &&
        D->Header.DataBase + D->Header.DataSize == D->Header.BssBase) {
        D->Header.Mode = (D->Header.Mode & ~MF_ADDR_MASK) | MF_ADDR_SIMPLE;
    }
}


void O65WriteTarget (O65Desc* D, File* F)
/* Write an o65 output file */
{
    char        OptBuf [256];   /* Buffer for option strings */
    unsigned    OptLen;
    time_t      T;
    const char* Name;

    /* Place the filename in the control structure */
    D->Filename = GetString (F->Name);

    /* Check for unresolved symbols. The function O65Unresolved is called
    ** if we get an unresolved symbol.
    */
    D->Undef = 0;               /* Reset the counter */
    CheckUnresolvedImports (O65Unresolved, D);
    if (D->Undef > 0) {
        /* We had unresolved symbols, cannot create output file */
        Error ("%u unresolved external(s) found - cannot create output file", D->Undef);
    }

    /* Setup the segment arrays */
    O65SetupSegments (D, F);

    /* Setup additional stuff in the header */
    O65SetupHeader (D);

    /* Open the file */
    D->F = fopen (D->Filename, "wb");
    if (D->F == 0) {
        Error ("Cannot open `%s': %s", D->Filename, strerror (errno));
    }

    /* Keep the user happy */
    Print (stdout, 1, "Opened `%s'...\n", D->Filename);

    /* Define some more options: A timestamp, the linker version and the
    ** filename
    */
    T = time (0);
    strcpy (OptBuf, ctime (&T));
    OptLen = strlen (OptBuf);
    while (OptLen > 0 && IsControl (OptBuf[OptLen-1])) {
        --OptLen;
    }
    OptBuf[OptLen] = '\0';
    O65SetOption (D, O65OPT_TIMESTAMP, OptBuf, OptLen + 1);
    sprintf (OptBuf, "ld65 V%s", GetVersionAsString ());
    O65SetOption (D, O65OPT_ASM, OptBuf, strlen (OptBuf) + 1);
    Name = FindName (D->Filename);
    O65SetOption (D, O65OPT_FILENAME, Name, strlen (Name) + 1);

    /* Write the header */
    O65WriteHeader (D);

    /* Write the text segment */
    O65WriteTextSeg (D);

    /* Write the data segment */
    O65WriteDataSeg (D);

    /* "Write" the bss segments */
    O65WriteBssSeg (D);

    /* "Write" the zeropage segments */
    O65WriteZPSeg (D);

    /* Write the undefined references list */
    O65WriteImports (D);

    /* Write the text segment relocation table */
    O65WriteTextReloc (D);

    /* Write the data segment relocation table */
    O65WriteDataReloc (D);

    /* Write the list of exports */
    O65WriteExports (D);

    /* Update header flags */
    O65UpdateHeader (D);

    /* Seek back to the start and write the updated header */
    fseek (D->F, 0, SEEK_SET);
    O65WriteHeader (D);

    /* Close the file */
    if (fclose (D->F) != 0) {
        Error ("Cannot write to `%s': %s", D->Filename, strerror (errno));
    }

    /* Reset the file and filename */
    D->F        = 0;
    D->Filename = 0;
}
