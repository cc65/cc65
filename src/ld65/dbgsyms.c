/*****************************************************************************/
/*                                                                           */
/*                                 dbgsyms.c                                 */
/*                                                                           */
/*                 Debug symbol handling for the ld65 linker                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2011, Ullrich von Bassewitz                                      */
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

/* common */
#include "addrsize.h"
#include "attrib.h"
#include "check.h"
#include "hlldbgsym.h"
#include "symdefs.h"
#include "xmalloc.h"

/* ld65 */
#include "dbgsyms.h"
#include "error.h"
#include "exports.h"
#include "expr.h"
#include "fileio.h"
#include "global.h"
#include "lineinfo.h"
#include "objdata.h"
#include "spool.h"
#include "tpool.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Definition of the debug symbol structure */
struct DbgSym {
    unsigned            Id;             /* Id of debug symbol */
    DbgSym*             Next;           /* Pool linear list link */
    ObjData*            Obj;            /* Object file that exports the name */
    Collection          DefLines;       /* Line infos for definition */
    Collection          RefLines;       /* Line infos for references */
    ExprNode*           Expr;           /* Expression (0 if not def'd) */
    unsigned            Size;           /* Symbol size if any */
    unsigned            OwnerId;        /* Id of parent/owner */
    unsigned            ImportId;       /* Id of import if this is one */
    unsigned            Name;           /* Name */
    unsigned short      Type;           /* Type of symbol */
    unsigned short      AddrSize;       /* Address size of symbol */
};

/* Structure used for a high level language function or symbol */
typedef struct HLLDbgSym HLLDbgSym;
struct HLLDbgSym {
    unsigned            Flags;          /* See above */
    unsigned            Name;           /* String id of name */
    DbgSym*             Sym;            /* Assembler symbol */
    int                 Offs;           /* Offset if any */
    unsigned            Type;           /* String id of type */
    unsigned            ScopeId;        /* Parent scope */
};

/* We will collect all debug symbols in the following array and remove
** duplicates before outputing them into a label file.
*/
static DbgSym*  DbgSymPool[256];



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static DbgSym* NewDbgSym (unsigned Id, unsigned Type, unsigned char AddrSize,
                          ObjData* O)
/* Create a new DbgSym and return it */
{
    /* Allocate memory */
    DbgSym* D     = xmalloc (sizeof (DbgSym));

    /* Initialize the fields */
    D->Id         = Id;
    D->Next       = 0;
    D->Obj        = O;
    D->DefLines   = EmptyCollection;
    D->RefLines   = EmptyCollection;
    D->Expr       = 0;
    D->Size       = 0;
    D->OwnerId    = ~0U;
    D->ImportId   = ~0U;
    D->Name       = 0;
    D->Type       = Type;
    D->AddrSize   = AddrSize;

    /* Return the new entry */
    return D;
}



static HLLDbgSym* NewHLLDbgSym (void)
/* Create a new HLLDbgSym and return it */
{
    /* Allocate memory and return it */
    return xmalloc (sizeof (HLLDbgSym));
}



static DbgSym* GetDbgSym (DbgSym* D, long Val)
/* Check if we find the same debug symbol in the table. If we find it, return
** a pointer to the other occurrence, if we didn't find it, return NULL.
*/
{
    /* Create the hash. We hash over the symbol value */
    unsigned Hash = ((Val >> 24) & 0xFF) ^
                    ((Val >> 16) & 0xFF) ^
                    ((Val >>  8) & 0xFF) ^
                    ((Val >>  0) & 0xFF);

    /* Check for this symbol */
    DbgSym* Sym = DbgSymPool[Hash];
    while (Sym) {
        /* Is this symbol identical? */
        if (Sym->Name == D->Name && EqualExpr (Sym->Expr, D->Expr)) {
            /* Found */
            return Sym;
        }

        /* Next symbol */
        Sym = Sym->Next;
    }

    /* This is the first symbol of it's kind */
    return 0;
}



static void InsertDbgSym (DbgSym* D, long Val)
/* Insert the symbol into the hashed symbol pool */
{
    /* Create the hash. We hash over the symbol value */
    unsigned Hash = ((Val >> 24) & 0xFF) ^
                    ((Val >> 16) & 0xFF) ^
                    ((Val >>  8) & 0xFF) ^
                    ((Val >>  0) & 0xFF);

    /* Insert the symbol */
    D->Next = DbgSymPool [Hash];
    DbgSymPool [Hash] = D;
}



DbgSym* ReadDbgSym (FILE* F, ObjData* O, unsigned Id)
/* Read a debug symbol from a file, insert and return it */
{
    /* Read the type and address size */
    unsigned Type = ReadVar (F);
    unsigned char AddrSize = Read8 (F);

    /* Create a new debug symbol */
    DbgSym* D = NewDbgSym (Id, Type, AddrSize, O);

    /* Read the id of the owner scope/symbol */
    D->OwnerId = ReadVar (F);

    /* Read and assign the name */
    D->Name = MakeGlobalStringId (O, ReadVar (F));

    /* Read the value */
    if (SYM_IS_EXPR (D->Type)) {
        D->Expr = ReadExpr (F, O);
    } else {
        D->Expr = LiteralExpr (Read32 (F), O);
    }

    /* Read the size */
    if (SYM_HAS_SIZE (D->Type)) {
        D->Size = ReadVar (F);
    }

    /* If this is an import, the file contains its id */
    if (SYM_IS_IMPORT (D->Type)) {
        D->ImportId = ReadVar (F);
    }

    /* If its an exports, there's also the export id, but we don't remember
    ** it but use it to let the export point back to us.
    */
    if (SYM_IS_EXPORT (D->Type)) {
        /* Get the export from the export id, then set the our id */
        GetObjExport (O, ReadVar (F))->DbgSymId = Id;
    }

    /* Last is the list of line infos for this symbol */
    ReadLineInfoList (F, O, &D->DefLines);
    ReadLineInfoList (F, O, &D->RefLines);

    /* Return the new DbgSym */
    return D;
}



HLLDbgSym* ReadHLLDbgSym (FILE* F, ObjData* O, unsigned Id attribute ((unused)))
/* Read a hll debug symbol from a file, insert and return it */
{
    unsigned SC;

    /* Create a new HLLDbgSym */
    HLLDbgSym* S = NewHLLDbgSym ();

    /* Read the data */
    S->Flags    = ReadVar (F);
    SC          = HLL_GET_SC (S->Flags);
    S->Name     = MakeGlobalStringId (O, ReadVar (F));
    if (HLL_HAS_SYM (S->Flags)) {
        S->Sym = GetObjDbgSym (O, ReadVar (F));
    } else {
        /* Auto variables aren't attached to asm symbols */
        S->Sym = 0;
    }
    if (SC == HLL_SC_AUTO || SC == HLL_SC_REG) {
        S->Offs = ReadVar (F);
    } else {
        S->Offs = 0;
    }
    S->Type     = GetTypeId (GetObjString (O, ReadVar (F)));
    S->ScopeId  = ReadVar (F);

    /* Return the (now initialized) hll debug symbol */
    return S;
}



static void ClearDbgSymTable (void)
/* Clear the debug symbol table */
{
    unsigned I;
    for (I = 0; I < sizeof (DbgSymPool) / sizeof (DbgSymPool[0]); ++I) {
        DbgSym* Sym = DbgSymPool[I];
        DbgSymPool[I] = 0;
        while (Sym) {
            DbgSym* NextSym = Sym->Next;
            Sym->Next = 0;
            Sym = NextSym;
        }
    }
}



static long GetDbgSymVal (const DbgSym* D)
/* Get the value of this symbol */
{
    CHECK (D->Expr != 0);
    return GetExprVal (D->Expr);
}



static void PrintLineInfo (FILE* F, const Collection* LineInfos, const char* Format)
/* Output an attribute with line infos */
{
    if (CollCount (LineInfos) > 0) {
        unsigned I;
        const LineInfo* LI = CollConstAt (LineInfos, 0);
        fprintf (F, Format, LI->Id);
        for (I = 1; I < CollCount (LineInfos); ++I) {
            LI = CollConstAt (LineInfos, I);
            fprintf (F, "+%u", LI->Id);
        }
    }
}



unsigned DbgSymCount (void)
/* Return the total number of debug symbols */
{
    /* Walk over all object files */
    unsigned I;
    unsigned Count = 0;
    for (I = 0; I < CollCount (&ObjDataList); ++I) {

        /* Get this object file */
        const ObjData* O = CollAtUnchecked (&ObjDataList, I);

        /* Count debug symbols */
        Count += CollCount (&O->DbgSyms);
    }
    return Count;
}



unsigned HLLDbgSymCount (void)
/* Return the total number of high level language debug symbols */
{
    /* Walk over all object files */
    unsigned I;
    unsigned Count = 0;
    for (I = 0; I < CollCount (&ObjDataList); ++I) {

        /* Get this object file */
        const ObjData* O = CollAtUnchecked (&ObjDataList, I);

        /* Count debug symbols */
        Count += CollCount (&O->HLLDbgSyms);
    }
    return Count;
}



void PrintDbgSyms (FILE* F)
/* Print the debug symbols in a debug file */
{
    unsigned I, J;

    for (I = 0; I < CollCount (&ObjDataList); ++I) {

        /* Get the object file */
        ObjData* O = CollAtUnchecked (&ObjDataList, I);

        /* Walk through all debug symbols in this module */
        for (J = 0; J < CollCount (&O->DbgSyms); ++J) {

            /* Get the next debug symbol */
            const DbgSym* S = CollConstAt (&O->DbgSyms, J);

            /* Emit the base data for the entry */
            fprintf (F,
                     "sym\tid=%u,name=\"%s\",addrsize=%s",
                     O->SymBaseId + J,
                     GetString (S->Name),
                     AddrSizeToStr ((unsigned char) S->AddrSize));

            /* Emit the size only if we know it */
            if (S->Size != 0) {
                fprintf (F, ",size=%u", S->Size);
            }

            /* For cheap local symbols, add the owner symbol, for others,
            ** add the owner scope.
            */
            if (SYM_IS_STD (S->Type)) {
                fprintf (F, ",scope=%u", O->ScopeBaseId + S->OwnerId);
            } else {
                fprintf (F, ",parent=%u", O->SymBaseId + S->OwnerId);
            }

            /* Output line infos */
            PrintLineInfo (F, &S->DefLines, ",def=%u");
            PrintLineInfo (F, &S->RefLines, ",ref=%u");

            /* If this is an import, output the id of the matching export.
            ** If this is not an import, output its value and - if we have
            ** it - the segment.
            */
            if (SYM_IS_IMPORT (S->Type)) {

                /* Get the import */
                const Import* Imp = GetObjImport (O, S->ImportId);

                /* Get the export from the import */
                const Export* Exp = Imp->Exp;

                /* Output the type */
                fputs (",type=imp", F);

                /* If this is not a linker generated symbol, and the module
                ** that contains the export has debug info, output the debug
                ** symbol id for the export
                */
                if (Exp->Obj && OBJ_HAS_DBGINFO (Exp->Obj->Header.Flags)) {
                    fprintf (F, ",exp=%u", Exp->Obj->SymBaseId + Exp->DbgSymId);
                }

            } else {

                SegExprDesc D;

                /* Get the symbol value */
                long Val = GetDbgSymVal (S);

                /* Output it */
                fprintf (F, ",val=0x%lX", Val);

                /* Check for a segmented expression and add the segment id to
                ** the debug info if we have one.
                */
                GetSegExprVal (S->Expr, &D);
                if (!D.TooComplex && D.Seg != 0) {
                    fprintf (F, ",seg=%u", D.Seg->Id);
                }

                /* Output the type */
                fprintf (F, ",type=%s", SYM_IS_LABEL (S->Type)? "lab" : "equ");
            }

            /* Terminate the output line */
            fputc ('\n', F);
        }
    }
}



void PrintHLLDbgSyms (FILE* F)
/* Print the high level language debug symbols in a debug file */
{
    unsigned I, J;

    for (I = 0; I < CollCount (&ObjDataList); ++I) {

        /* Get the object file */
        ObjData* O = CollAtUnchecked (&ObjDataList, I);

        /* Walk through all hll debug symbols in this module */
        for (J = 0; J < CollCount (&O->HLLDbgSyms); ++J) {

            /* Get the next debug symbol */
            const HLLDbgSym* S = CollConstAt (&O->HLLDbgSyms, J);

            /* Get the storage class */
            unsigned SC = HLL_GET_SC (S->Flags);

            /* Output the base info */
            fprintf (F, "csym\tid=%u,name=\"%s\",scope=%u,type=%u,sc=",
                     O->HLLSymBaseId + J,
                     GetString (S->Name),
                     O->ScopeBaseId + S->ScopeId,
                     S->Type);
            switch (SC) {
                case HLL_SC_AUTO:       fputs ("auto", F);      break;
                case HLL_SC_REG:        fputs ("reg", F);       break;
                case HLL_SC_STATIC:     fputs ("static", F);    break;
                case HLL_SC_EXTERN:     fputs ("ext", F);       break;
                default:
                    Error ("Invalid storage class %u for hll symbol", SC);
                    break;
            }

            /* Output the offset if it is not zero */
            if (S->Offs) {
                fprintf (F, ",offs=%d", S->Offs);
            }

            /* For non auto symbols output the debug symbol id of the asm sym */
            if (HLL_HAS_SYM (S->Flags)) {
                fprintf (F, ",sym=%u", O->SymBaseId + S->Sym->Id);
            }

            /* Terminate the output line */
            fputc ('\n', F);
        }
    }
}



void PrintDbgSymLabels (FILE* F)
/* Print the debug symbols in a VICE label file */
{
    unsigned I, J;

    /* Clear the symbol table */
    ClearDbgSymTable ();

    /* Create labels from all modules we have linked into the output file */
    for (I = 0; I < CollCount (&ObjDataList); ++I) {

        /* Get the object file */
        ObjData* O = CollAtUnchecked (&ObjDataList, I);

        /* Walk through all debug symbols in this module */
        for (J = 0; J < CollCount (&O->DbgSyms); ++J) {

            long Val;

            /* Get the next debug symbol */
            DbgSym* D = CollAt (&O->DbgSyms, J);

            /* Emit this symbol only if it is a label (ignore equates and imports) */
            if (SYM_IS_EQUATE (D->Type) || SYM_IS_IMPORT (D->Type)) {
                continue;
            }

            /* Get the symbol value */
            Val = GetDbgSymVal (D);

            /* Lookup this symbol in the table. If it is found in the table, it was
            ** already written to the file, so don't emit it twice. If it is not in
            ** the table, insert and output it.
            */
            if (GetDbgSym (D, Val) == 0) {

                /* Emit the VICE label line */
                fprintf (F, "al %06lX .%s\n", Val, GetString (D->Name));

                /* Insert the symbol into the table */
                InsertDbgSym (D, Val);
            }
        }
    }
}
