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

#include "asmlabel.h"
#include "check.h"
#include "ctrans.h"
#include "codegen.h"
#include "error.h"
#include "global.h"
#include "litpool.h"



/*****************************************************************************/
/*  		    	 	     Data				     */
/*****************************************************************************/



#define LITPOOL_SIZE   	4096   	       		/* Max strings per function */
static unsigned char LiteralPool[LITPOOL_SIZE]; /* The literal pool */
static unsigned LiteralOffs	= 0;  		/* Current pool offset */
static unsigned LiteralSpace   	= 0;  		/* Space used (stats only) */

unsigned LiteralLabel  		= 1;  		/* Pool asm label */



/*****************************************************************************/
/*  		       	   	     Code				     */
/*****************************************************************************/



void TranslateLiteralPool (unsigned Offs)
/* Translate the literals starting from the given offset into the target
 * charset.
 */
{
    while (Offs < LiteralOffs) {
     	LiteralPool[Offs] = ctrans (LiteralPool[Offs]);
	++Offs;
    }
}



void DumpLiteralPool (void)
/* Dump the literal pool */
{
    /* if nothing there, exit... */
    if (LiteralOffs == 0) {
	return;
    }

    /* Switch to the data segment */
    if (WriteableStrings) {
     	g_usedata ();
    } else {
       	g_userodata ();
    }

    /* Define the label */
    g_defloclabel (LiteralLabel);

    /* Translate the buffer contents into the target charset */
    TranslateLiteralPool (0);

    /* Output the buffer data */
    g_defbytes (LiteralPool, LiteralOffs);

    /* Switch back to the code segment */
    g_usecode ();

    /* Reset the buffer */
    LiteralSpace += LiteralOffs;	/* Count literal bytes emitted */
    LiteralLabel  = GetLabel ();       	/* Get a new pool label */
    LiteralOffs	  = 0;
}



unsigned GetLiteralOffs (void)
/* Return the current offset into the literal pool */
{
    return LiteralOffs;
}



void ResetLiteralOffs (unsigned Offs)
/* Reset the offset into the literal pool to some earlier value, effectively
 * removing values from the pool.
 */
{
    CHECK (Offs <= LiteralOffs);
    LiteralOffs = Offs;
}



void AddLiteralChar (char C)
/* Add one character to the literal pool */
{
    if (LiteralOffs >= LITPOOL_SIZE) {
	Fatal (FAT_OUT_OF_STRSPACE);
    }
    LiteralPool[LiteralOffs++] = C;
}



unsigned AddLiteral (const char* S)
/* Add a literal string to the literal pool. Return the starting offset into
 * the pool
 */
{
    /* Remember the starting offset */
    unsigned Start = LiteralOffs;

    /* Copy the string doing a range check */
    do {
	AddLiteralChar (*S);
    } while (*S++);

    /* Return the starting offset */
    return Start;
}



const char* GetLiteral (unsigned Offs)
/* Get a pointer to the literal with the given offset in the pool */
{
    CHECK (Offs < LiteralOffs);
    return &LiteralPool[Offs];
}



void PrintLiteralStats (FILE* F)
/* Print statistics about the literal space used */
{
    fprintf (F, "Literal space used: %d bytes\n", LiteralSpace);
}



