/*****************************************************************************/
/*                                                                           */
/*				    data.c				     */
/*                                                                           */
/*			     Data output routines			     */
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



/* da65 */
#include "attrtab.h"
#include "code.h"  
#include "error.h"
#include "global.h"
#include "output.h"
#include "data.h"



/*****************************************************************************/
/*   	     	    		     Code				     */
/*****************************************************************************/



void ByteTable (unsigned RemainingBytes)
/* Output a table of bytes */
{
    /* Count how many bytes may be output. This number is limited by the
     * number of remaining bytes, a label, or the end of the ByteTable
     * attribute.
     */
    unsigned Count = 1;
    while (Count < RemainingBytes) {
	if (HaveLabel(PC+Count) || GetStyle (PC+Count) != atByteTab) {
	    break;
     	}
	++Count;
    }
    RemainingBytes -= Count;

    /* Output as many data bytes lines as needed */
    while (Count > 0) {

     	/* Calculate the number of bytes for the next line */
     	unsigned Chunk = (Count > BytesPerLine)? BytesPerLine : Count;

     	/* Output a line with these bytes */
     	DataByteLine (Chunk);

     	/* Next line */
     	Count -= Chunk;
     	PC    += Chunk;
    }

    /* If the next line is not a byte table line, add a separator */
    if (RemainingBytes > 0 && GetStyle (PC) != atByteTab) {
	SeparatorLine ();
    }
}



void WordTable (unsigned RemainingBytes)
/* Output a table of words */
{
    /* Count how many bytes may be output. This number is limited by the
     * number of remaining bytes, a label, or the end of the WordTable
     * attribute.
     */
    unsigned Count = 1;
    while (Count < RemainingBytes) {
	if (HaveLabel(PC+Count) || GetStyle (PC+Count) != atWordTab) {
	    break;
     	}
	++Count;
    }
    RemainingBytes -= Count;

    /* Make the given number even */
    Count &= ~1U;

    /* Output as many data word lines as needed */
    while (Count > 0) {

	/* Calculate the number of bytes for the next line */
	unsigned Chunk = (Count > BytesPerLine)? BytesPerLine : Count;

	/* Output a line with these bytes */
       	DataWordLine (Chunk);

	/* Next line */
	PC    += Chunk;
       	Count -= Chunk;
    }

    /* If the next line is not a byte table line, add a separator */
    if (RemainingBytes > 0 && GetStyle (PC) != atWordTab) {
	SeparatorLine ();
    }
}



void AddrTable (unsigned RemainingBytes)
/* Output a table of addresses */
{
    /* Count how many bytes may be output. This number is limited by the
     * number of remaining bytes, a label, or the end of the WordTable
     * attribute.
     */
    unsigned Count = 1;
    while (Count < RemainingBytes) {
	if (HaveLabel(PC+Count) || GetStyle (PC+Count) != atAddrTab) {
	    break;
     	}
	++Count;
    }
    RemainingBytes -= Count;

    /* Make the given number even */
    Count &= ~1U;

    /* Output as many data bytes lines as needed. For addresses, each line
     * will hold just one address.
     */
    while (Count > 0) {

	/* Get the address */
	unsigned Addr = GetCodeWord (PC);

	/* In pass 1, define a label, in pass 2 output the line */
	if (Pass == 1) {
	    if (!HaveLabel (Addr)) {
		AddLabel (Addr, MakeLabelName (Addr));
	    }
	} else {
	    const char* Label = GetLabel (Addr);
	    if (Label == 0) {
		/* OOPS! Should not happen */
		Internal ("OOPS - Label for address %04X disappeard!", Addr);
	    }
	    Indent (MIndent);
	    Output (".word");
	    Indent (AIndent);
	    Output ("%s", Label);
	    LineComment (PC, 2);
	    LineFeed ();
	}

	/* Next line */
	PC    += 2;
       	Count -= 2;
    }

    /* If the next line is not a byte table line, add a separator */
    if (RemainingBytes > 0 && GetStyle (PC) != atAddrTab) {
	SeparatorLine ();
    }
}



