/*****************************************************************************/
/*                                                                           */
/*				    code.c				     */
/*                                                                           */
/*			    Binary code management			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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
#include "check.h"

/* da65 */
#include "error.h"
#include "code.h"



/*****************************************************************************/
/*	    			     Data				     */
/*****************************************************************************/



static unsigned char CodeBuf [0x10000];		/* Code buffer */
static unsigned long CodeStart;			/* Start address */
static unsigned long CodeEnd;	  		/* End address */
static unsigned long PC;			/* Current PC */



/*****************************************************************************/
/*	       		  	     Code				     */
/*****************************************************************************/



void LoadCode (const char* Name, unsigned long StartAddress)
/* Load the code from the given file */
{
    unsigned Count, MaxCount;
    FILE* F;


    PRECONDITION (StartAddress < 0x10000);

    /* Calculate the maximum code size */
    MaxCount = 0x10000 - StartAddress;

    /* Open the file */
    F = fopen (Name, "rb");
    if (F == 0) {
	Error ("Cannot open `%s': %s", Name, strerror (errno));
    }

    /* Read from the file and remember the number of bytes read */
    Count = fread (CodeBuf + StartAddress, 1, MaxCount, F);
    if (ferror (F)) {
	Error ("Error reading from `%s': %s", Name, strerror (errno));
    }
    if (Count == 0) {
	Error ("File `%s' contains no data", Name);
    }

    /* Set the buffer variables */
    CodeStart = PC = StartAddress;
    CodeEnd = CodeStart + Count - 1;	/* CodeEnd is inclusive */
}



unsigned GetPC (void)
/* Get the current program counter */
{
    return PC;
}



unsigned char PeekCodeByte (void)
/* Peek at the byte at the current PC */
{
    PRECONDITION (PC <= CodeEnd);
    return CodeBuf [PC];
}



unsigned char GetCodeByte (void)
/* Get a byte from the PC and increment it */
{
    PRECONDITION (PC <= CodeEnd);
    return CodeBuf [PC++];
}



unsigned GetCodeWord (void)
/* Get a word from the current PC and increment it */
{
    unsigned Lo = GetCodeByte ();
    unsigned Hi = GetCodeByte ();
    return Lo | (Hi << 8);
}



unsigned GetRemainingBytes (void)
/* Return the number of remaining code bytes */
{
    if (CodeEnd >= PC) {
	return (CodeEnd - PC + 1);
    } else {
	return 0;
    }
}



void ResetCode (void)
/* Reset the code input to start over for the next pass */
{
    PC = CodeStart;
}



