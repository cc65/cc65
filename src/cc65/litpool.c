/*****************************************************************************/
/*                                                                           */
/*				   litpool.c				     */
/*                                                                           */
/*		Literal string handling for the cc65 C compiler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2009, Ullrich von Bassewitz                                      */
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

/* common */
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
/*  		    	  	     Data		     		     */
/*****************************************************************************/



/* Forward for struct SymEntry */
struct SymEntry;

/* Definition of the literal pool */
typedef struct LiteralPool LiteralPool;
struct LiteralPool {
    int                 Writable;       /* True if strings are writable */
    unsigned            Label;          /* Pool asm label */
    struct SymEntry*    Func;           /* Function that contains the pool */
    StrBuf              Pool;           /* The pool itself */
};

/* The current literal pool */
static LiteralPool*     LP = 0;

/* Stack that contains the nested literal pools. Since TOS is in LiteralPool
 * and functions aren't nested in C, the maximum depth is 1. I'm using a
 * collection anyway, so the code is prepared for nested functions or
 * whatever.
 */
static Collection       LPStack  = STATIC_COLLECTION_INITIALIZER;



/*****************************************************************************/
/*  	       	       	   	     Code		     		     */
/*****************************************************************************/



static LiteralPool* NewLiteralPool (struct SymEntry* Func)
/* Create a new literal pool and return it */
{
    /* Allocate memory */
    LiteralPool* LP = xmalloc (sizeof (*LP));

    /* Initialize the fields */
    LP->Writable = IS_Get (&WritableStrings);
    LP->Label = GetLocalLabel ();
    LP->Func  = Func;
    SB_Init (&LP->Pool);

    /* Return the new pool */
    return LP;
}



static void FreeLiteralPool (LiteralPool* LP)
/* Free a LiteralPool structure */
{
    /* Free the string buffer contained within the struct */
    SB_Done (&LP->Pool);

    /* Free the struct itself */
    xfree (LP);
}



void InitLiteralPool (void)
/* Initialize the literal pool */
{
    /* Create a new pool */
    LP = NewLiteralPool (0);
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



void PopLiteralPool (void)
/* Free the current literal pool and restore the one from TOS */
{
    /* Free the current literal pool */
    FreeLiteralPool (LP);

    /* Pop one from stack */
    LP = CollPop (&LPStack);
}



void TranslateLiteralPool (unsigned Offs)
/* Translate the literals starting from the given offset into the target
 * charset.
 */
{
    TgtTranslateBuf (SB_GetBuf (&LP->Pool) + Offs, SB_GetLen (&LP->Pool) - Offs);
}



void DumpLiteralPool (void)
/* Dump the literal pool */
{
    /* If nothing there, exit... */
    if (SB_GetLen (&LP->Pool) == 0) {
	return;
    }

    /* Switch to the correct segment */
    if (LP->Writable) {
     	g_usedata ();
    } else {
       	g_userodata ();
    }

    /* Define the label */
    g_defdatalabel (LP->Label);

    /* Translate the buffer contents into the target charset */
    TranslateLiteralPool (0);

    /* Output the buffer data */
    g_defbytes (SB_GetConstBuf (&LP->Pool), SB_GetLen (&LP->Pool));
}



unsigned GetLiteralPoolLabel (void)
/* Return the asm label for the current literal pool */
{
    return LP->Label;
}



unsigned GetLiteralPoolOffs (void)
/* Return the current offset into the literal pool */
{
    return SB_GetLen (&LP->Pool);
}



void ResetLiteralPoolOffs (unsigned Offs)
/* Reset the offset into the literal pool to some earlier value, effectively
 * removing values from the pool.
 */
{
    CHECK (Offs <= SB_GetLen (&LP->Pool));
    SB_Cut (&LP->Pool, Offs);
}



unsigned AddLiteral (const char* S)
/* Add a literal string to the literal pool. Return the starting offset into
 * the pool
 */
{
    return AddLiteralBuf (S, strlen (S) + 1);
}



unsigned AddLiteralBuf (const void* Buf, unsigned Len)
/* Add a buffer containing a literal string to the literal pool. Return the
 * starting offset into the pool for this string.
 */
{
    /* Remember the starting offset */
    unsigned Start = SB_GetLen (&LP->Pool);

    /* Append the buffer */
    SB_AppendBuf (&LP->Pool, Buf, Len);

    /* Return the starting offset */
    return Start;
}



unsigned AddLiteralStr (const StrBuf* S)
/* Add a literal string to the literal pool. Return the starting offset into
 * the pool for this string.
 */
{
    return AddLiteralBuf (SB_GetConstBuf (S), SB_GetLen (S));
}



const char* GetLiteral (unsigned Offs)
/* Get a pointer to the literal with the given offset in the pool */
{
    CHECK (Offs < SB_GetLen (&LP->Pool));
    return SB_GetConstBuf (&LP->Pool) + Offs;
}



void GetLiteralStrBuf (StrBuf* Target, unsigned Offs)
/* Copy the string starting at Offs and lasting to the end of the buffer
 * into Target.
 */
{
    CHECK (Offs <= SB_GetLen (&LP->Pool));
    SB_Slice (Target, &LP->Pool, Offs, SB_GetLen (&LP->Pool) - Offs);
}



void PrintLiteralPoolStats (FILE* F)
/* Print statistics about the literal space used */
{
    fprintf (F, "Literal space used: %u bytes\n", SB_GetLen (&LP->Pool));
}



