/*****************************************************************************/
/*                                                                           */
/*				   litpool.c				     */
/*                                                                           */
/*		Literal string handling for the cc65 C compiler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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
#include "strbuf.h"
#include "tgttrans.h"

/* cc65 */
#include "asmlabel.h"
#include "codegen.h"
#include "error.h"
#include "global.h"
#include "litpool.h"



/*****************************************************************************/
/*  		    	  	     Data		     		     */
/*****************************************************************************/



unsigned 	      LiteralPoolLabel 	= 0;	/* Pool asm label */
static StrBuf         LiteralPool       = STATIC_STRBUF_INITIALIZER;



/*****************************************************************************/
/*  	       	       	   	     Code		     		     */
/*****************************************************************************/



void InitLiteralPool (void)
/* Initialize the literal pool */
{
    /* Get the pool label */
    LiteralPoolLabel = GetLocalLabel ();
}



void TranslateLiteralPool (unsigned Offs)
/* Translate the literals starting from the given offset into the target
 * charset.
 */
{
    TgtTranslateBuf (SB_GetBuf (&LiteralPool) + Offs, SB_GetLen (&LiteralPool) - Offs);
}



void DumpLiteralPool (void)
/* Dump the literal pool */
{
    /* If nothing there, exit... */
    if (SB_GetLen (&LiteralPool) == 0) {
	return;
    }

    /* Switch to the data segment */
    if (WriteableStrings) {
     	g_usedata ();
    } else {
       	g_userodata ();
    }

    /* Define the label */
    g_defdatalabel (LiteralPoolLabel);

    /* Translate the buffer contents into the target charset */
    TranslateLiteralPool (0);

    /* Output the buffer data */
    g_defbytes (SB_GetConstBuf (&LiteralPool), SB_GetLen (&LiteralPool));
}



unsigned GetLiteralPoolOffs (void)
/* Return the current offset into the literal pool */
{
    return SB_GetLen (&LiteralPool);
}



void ResetLiteralPoolOffs (unsigned Offs)
/* Reset the offset into the literal pool to some earlier value, effectively
 * removing values from the pool.
 */
{
    CHECK (Offs <= SB_GetLen (&LiteralPool));
    SB_Cut (&LiteralPool, Offs);
}



void AddLiteralChar (char C)
/* Add one character to the literal pool */
{
    SB_AppendChar (&LiteralPool, C);
}



unsigned AddLiteral (const char* S)
/* Add a literal string to the literal pool. Return the starting offset into
 * the pool
 */
{
    /* Remember the starting offset */
    unsigned Start = SB_GetLen (&LiteralPool);

    /* Copy the string including the terminator growing the buffer if needed */
    SB_AppendBuf (&LiteralPool, S, strlen (S) + 1);

    /* Return the starting offset */
    return Start;
}



const char* GetLiteral (unsigned Offs)
/* Get a pointer to the literal with the given offset in the pool */
{
    CHECK (Offs < SB_GetLen (&LiteralPool));
    return SB_GetConstBuf (&LiteralPool) + Offs;
}



void PrintLiteralPoolStats (FILE* F)
/* Print statistics about the literal space used */
{
    fprintf (F, "Literal space used: %u bytes\n", SB_GetLen (&LiteralPool));
}



