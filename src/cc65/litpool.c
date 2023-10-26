/*****************************************************************************/
/*                                                                           */
/*                                 litpool.c                                 */
/*                                                                           */
/*              Literal string handling for the cc65 C compiler              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2013, Ullrich von Bassewitz                                      */
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

/* common */
#include "attrib.h"
#include "check.h"
#include "coll.h"
#include "tgttrans.h"
#include "xmalloc.h"

/* cc65 */
#include "asmlabel.h"
#include "codegen.h"
#include "error.h"
#include "global.h"
#include "litpool.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Definition of a literal */
struct Literal {
    unsigned    Label;                  /* Asm label for this literal */
    int         RefCount;               /* Reference count */
    int         Output;                 /* True if output has been generated */
    StrBuf      Data;                   /* Literal data */
};

/* Definition of the literal pool */
struct LiteralPool {
    struct SymEntry*    Func;               /* Function that owns the pool */
    Collection          WritableLiterals;   /* Writable literals in the pool */
    Collection          ReadOnlyLiterals;   /* Readonly literals in the pool */
};

/* The global and current literal pool */
static LiteralPool*     GlobalPool = 0;
static LiteralPool*     LP         = 0;

/* Stack that contains the nested literal pools. Since TOS is in LiteralPool
** and functions aren't nested in C, the maximum depth is 1. I'm using a
** collection anyway, so the code is prepared for nested functions or
** whatever.
*/
static Collection       LPStack  = STATIC_COLLECTION_INITIALIZER;



/*****************************************************************************/
/*                              struct Literal                               */
/*****************************************************************************/



static Literal* NewLiteral (const void* Buf, unsigned Len)
/* Create a new literal and return it */
{
    /* Allocate memory */
    Literal* L = xmalloc (sizeof (*L));

    /* Initialize the fields */
    L->Label    = GetPooledLiteralLabel ();
    L->RefCount = 0;
    L->Output   = 0;
    SB_Init (&L->Data);
    SB_AppendBuf (&L->Data, Buf, Len);

    /* Return the new literal */
    return L;
}



static void FreeLiteral (Literal* L)
/* Free a literal */
{
    /* Free the literal data */
    SB_Done (&L->Data);

    /* Free the structure itself */
    xfree (L);
}



static void OutputLiteral (Literal* L)
/* Output one literal to the currently active data segment */
{
    /* Define the label for the literal */
    g_defliterallabel (L->Label);

    /* Output the literal data */
    g_defbytes (SB_GetConstBuf (&L->Data), SB_GetLen (&L->Data));

    /* Mark the literal as output */
    L->Output = 1;
}



Literal* UseLiteral (Literal* L)
/* Increase the reference counter for the literal and return it */
{
    /* Increase the reference count */
    ++L->RefCount;

    /* Return the literal */
    return L;
}



void ReleaseLiteral (Literal* L)
/* Decrement the reference counter for the literal */
{
    --L->RefCount;
    CHECK (L->RefCount >= 0);
}



void TranslateLiteral (Literal* L)
/* Translate a literal into the target charset */
{
    TgtTranslateBuf (SB_GetBuf (&L->Data), SB_GetLen (&L->Data));
}



void ConcatLiteral (Literal* L, const Literal* Appended)
/* Concatenate string literals */
{
    if (SB_GetLen (&L->Data) > 0 && SB_LookAtLast (&L->Data) == '\0') {
        SB_Drop (&L->Data, 1);
    }
    SB_Append (&L->Data, &Appended->Data);
}



unsigned GetLiteralLabel (const Literal* L)
/* Return the asm label for a literal */
{
    return L->Label;
}



const char* GetLiteralStr (const Literal* L)
/* Return the data for a literal as pointer to char */
{
    return SB_GetConstBuf (&L->Data);
}



const StrBuf* GetLiteralStrBuf (const Literal* L)
/* Return the data for a literal as pointer to the string buffer */
{
    return &L->Data;
}



unsigned GetLiteralSize (const Literal* L)
/* Get the size of a literal string */
{
    return SB_GetLen (&L->Data);
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static LiteralPool* NewLiteralPool (struct SymEntry* Func)
/* Create a new literal pool and return it */
{
    /* Allocate memory */
    LiteralPool* LP = xmalloc (sizeof (*LP));

    /* Initialize the fields */
    LP->Func  = Func;
    InitCollection (&LP->WritableLiterals);
    InitCollection (&LP->ReadOnlyLiterals);

    /* Return the new pool */
    return LP;
}



static void FreeLiteralPool (LiteralPool* LP)
/* Free a LiteralPool structure */
{
    /* Free the collections contained within the struct */
    DoneCollection (&LP->WritableLiterals);
    DoneCollection (&LP->ReadOnlyLiterals);

    /* Free the struct itself */
    xfree (LP);
}



static int Compare (void* Data attribute ((unused)),
                    const void* Left, const void* Right)
/* Compare function used when sorting the literal pool */
{
    /* Larger strings are considered "smaller" */
    return (int) GetLiteralSize (Right) - (int) GetLiteralSize (Left);
}



void InitLiteralPool (void)
/* Initialize the literal pool */
{
    /* Create the global literal pool */
    GlobalPool = LP = NewLiteralPool (0);
}



void PushLiteralPool (struct SymEntry* Func)
/* Push the current literal pool onto the stack and create a new one */
{
    /* We must have a literal pool to push! */
    PRECONDITION (LP != 0);

    /* Push the old pool */
    CollAppend (&LPStack, LP);

    /* Create a new one */
    LP = NewLiteralPool (Func);
}



LiteralPool* PopLiteralPool (void)
/* Pop the last literal pool from TOS and activate it. Return the old
** literal pool.
*/
{
    /* Remember the current literal pool */
    LiteralPool* Old = LP;

    /* Pop one from stack */
    LP = CollPop (&LPStack);

    /* Return the old one */
    return Old;
}



static void MoveLiterals (Collection* Source, Collection* Target)
/* Move referenced literals from Source to Target, delete unreferenced ones */
{
    unsigned I;

    /* Move referenced literals, remove unreferenced ones */
    for (I = 0; I < CollCount (Source); ++I) {

        /* Get the literal */
        Literal* L = CollAt (Source, I);

        /* If it is referenced and not output, add it to the Target pool,
        ** otherwise free it
        */
        if (L->RefCount && !L->Output) {
            CollAppend (Target, L);
        } else {
            FreeLiteral (L);
        }
    }
}



void MoveLiteralPool (LiteralPool* LocalPool)
/* Move all referenced literals in LocalPool to the global literal pool. This
** function will free LocalPool after moving the used string literals.
*/
{
    /* Move the literals */
    MoveLiterals (&LocalPool->WritableLiterals, &GlobalPool->WritableLiterals);
    MoveLiterals (&LocalPool->ReadOnlyLiterals, &GlobalPool->ReadOnlyLiterals);

    /* Free the local literal pool */
    FreeLiteralPool (LocalPool);
}



static void OutputWritableLiterals (Collection* Literals)
/* Output the given writable literals */
{
    unsigned I;

    /* If nothing there, exit... */
    if (CollCount (Literals) == 0) {
        return;
    }

    /* Switch to the correct segment */
    g_usedata ();

    /* Emit all literals that have a reference */
    for (I = 0; I < CollCount (Literals); ++I) {

        /* Get a pointer to the literal */
        Literal* L = CollAtUnchecked (Literals, I);

        /* Output this one, if it has references and wasn't already output */
        if (L->RefCount > 0 && !L->Output) {
            OutputLiteral (L);
        }

    }
}



static void OutputReadOnlyLiterals (Collection* Literals)
/* Output the given readonly literals merging (even partial) duplicates */
{
    unsigned I;

    /* If nothing there, exit... */
    if (CollCount (Literals) == 0) {
        return;
    }

    /* Switch to the correct segment */
    g_userodata ();

    /* Sort the literal pool by literal size. Larger strings go first */
    CollSort (Literals, Compare, 0);

    /* Emit all literals that have a reference */
    for (I = 0; I < CollCount (Literals); ++I) {

        unsigned J;
        Literal* C;

        /* Get the next literal */
        Literal* L = CollAt (Literals, I);

        /* Ignore it, if it doesn't have references or was already output */
        if (L->RefCount == 0 || L->Output) {
            continue;
        }

        /* Check if this literal is part of another one. Since the literals
        ** are sorted by size (larger ones first), it can only be part of a
        ** literal with a smaller index.
        ** Beware: Only check literals that have actually been referenced.
        */
        C = 0;
        for (J = 0; J < I; ++J) {

            const void* D;

            /* Get a pointer to the compare literal */
            Literal* L2 = CollAt (Literals, J);

            /* Ignore literals that have no reference */
            if (L2->RefCount == 0) {
                continue;
            }

            /* Get a pointer to the data */
            D = SB_GetConstBuf (&L2->Data) + SB_GetLen (&L2->Data) - SB_GetLen (&L->Data);

            /* Compare the data */
            if (memcmp (D, SB_GetConstBuf (&L->Data), SB_GetLen (&L->Data)) == 0) {
                /* Remember the literal and terminate the loop */
                C = L2;
                break;
            }
        }

        /* Check if we found a match */
        if (C != 0) {

            /* This literal is part of a longer literal, merge them */
            g_aliasliterallabel (L->Label, C->Label, GetLiteralSize (C) - GetLiteralSize (L));

        } else {

            /* Define the label for the literal */
            g_defliterallabel (L->Label);

            /* Output the literal data */
            g_defbytes (SB_GetConstBuf (&L->Data), SB_GetLen (&L->Data));

        }

        /* Mark the literal */
        L->Output = 1;
    }
}



void OutputLocalLiteralPool (LiteralPool* Pool)
/* Output the local literal pool */
{
    /* Output both sorts of literals */
    OutputWritableLiterals (&Pool->WritableLiterals);
    OutputReadOnlyLiterals (&Pool->ReadOnlyLiterals);
}



void OutputGlobalLiteralPool (void)
/* Output the global literal pool */
{
    OutputLocalLiteralPool (GlobalPool);
}



Literal* AddLiteral (const char* S)
/* Add a literal string to the literal pool. Return the literal. */
{
    return AddLiteralBuf (S, strlen (S) + 1);
}



Literal* AddLiteralBuf (const void* Buf, unsigned Len)
/* Add a buffer containing a literal string to the literal pool. Return the
** literal.
*/
{
    /* Create a new literal */
    Literal* L = NewLiteral (Buf, Len);

    /* Add the literal to the correct pool */
    if (IS_Get (&WritableStrings)) {
        CollAppend (&LP->WritableLiterals, L);
    } else {
        CollAppend (&LP->ReadOnlyLiterals, L);
    }

    /* Return the new literal */
    return L;
}



Literal* AddLiteralStr (const StrBuf* S)
/* Add a literal string to the literal pool. Return the literal. */
{
    return AddLiteralBuf (SB_GetConstBuf (S), SB_GetLen (S));
}
