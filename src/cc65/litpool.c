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
#include "tgttrans.h"
#include "xmalloc.h"

/* cc65 */
#include "asmlabel.h"
#include "codegen.h"
#include "error.h"
#include "global.h"
#include "litpool.h"



/*****************************************************************************/
/*  		    	 	     Data		     		     */
/*****************************************************************************/



static unsigned char* LiteralPoolBuf 	= 0;	/* Pointer to buffer */
static unsigned       LiteralPoolSize	= 0;	/* Size of pool */
static unsigned	      LiteralPoolOffs	= 0;	/* Current offset into pool */
unsigned 	      LiteralPoolLabel 	= 0;	/* Pool asm label */



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
    TgtTranslateBuf (LiteralPoolBuf + Offs, LiteralPoolOffs - Offs);
}



void DumpLiteralPool (void)
/* Dump the literal pool */
{
    /* If nothing there, exit... */
    if (LiteralPoolOffs == 0) {
	return;
    }

    /* Switch to the data segment */
    if (WriteableStrings) {
     	g_usedata ();
    } else {
       	g_userodata ();
    }

    /* Define the label */
    g_defloclabel (LiteralPoolLabel);

    /* Translate the buffer contents into the target charset */
    TranslateLiteralPool (0);

    /* Output the buffer data */
    g_defbytes (LiteralPoolBuf, LiteralPoolOffs);

    /* Switch back to the code segment */
    g_usecode ();
}



unsigned GetLiteralPoolOffs (void)
/* Return the current offset into the literal pool */
{
    return LiteralPoolOffs;
}



void ResetLiteralPoolOffs (unsigned Offs)
/* Reset the offset into the literal pool to some earlier value, effectively
 * removing values from the pool.
 */
{
    CHECK (Offs <= LiteralPoolOffs);
    LiteralPoolOffs = Offs;
}



void AddLiteralChar (char C)
/* Add one character to the literal pool */
{
    /* Grow the buffer if needed */
    if (LiteralPoolOffs >= LiteralPoolSize) {
	if (LiteralPoolSize == 0) {
	    /* First call */
	    LiteralPoolSize = 256;
	} else {
	    LiteralPoolSize *= 2;
	}
       	LiteralPoolBuf = xrealloc (LiteralPoolBuf, LiteralPoolSize);
    }

    /* Store the character */
    LiteralPoolBuf[LiteralPoolOffs++] = C;
}



unsigned AddLiteral (const char* S)
/* Add a literal string to the literal pool. Return the starting offset into
 * the pool
 */
{
    /* Remember the starting offset */
    unsigned Start = LiteralPoolOffs;

    /* Copy the string including the terminator growing the buffer if needed */
    do {
	AddLiteralChar (*S);
    } while (*S++);

    /* Return the starting offset */
    return Start;
}



const char* GetLiteral (unsigned Offs)
/* Get a pointer to the literal with the given offset in the pool */
{
    CHECK (Offs < LiteralPoolOffs);
    return (const char*) &LiteralPoolBuf[Offs];
}



void PrintLiteralPoolStats (FILE* F)
/* Print statistics about the literal space used */
{
    fprintf (F, "Literal space used: %u bytes\n", LiteralPoolOffs);
}



