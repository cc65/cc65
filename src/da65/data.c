/*****************************************************************************/
/*                                                                           */
/*				    data.c				     */
/*                                                                           */
/*			     Data output routines			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2007 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
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
#include "labels.h"
#include "output.h"
#include "data.h"



/*****************************************************************************/
/*   	     	    		     Code				     */
/*****************************************************************************/



static unsigned GetSpan (attr_t Style)
/* Get the number of bytes for a given style */
{
    /* Get the number of bytes still available */
    unsigned RemainingBytes = GetRemainingBytes ();

    /* Count how many bytes are available. This number is limited by the
     * number of remaining bytes, a label, a segment change, or the end of
     * the given Style attribute.
     */
    unsigned Count = 1;
    while (Count < RemainingBytes) {
        attr_t Attr;
       	if (MustDefLabel(PC+Count)) {
	    break;
     	}           
        Attr = GetAttr (PC+Count);
        if ((Attr & atStyleMask) != Style) {
            break;
        }
        if ((Attr & atSegmentChange)) {
            break;
        }
	++Count;
    }

    /* Return the number of bytes */
    return Count;
}



static unsigned DoTable (attr_t Style, unsigned MemberSize, void (*TableFunc) (unsigned))
/* Output a table of bytes */
{
    unsigned BytesLeft;

    /* Count how many bytes may be output. */
    unsigned Count = GetSpan (Style);

    /* If the count is less than the member size, print a row of Count data
     * bytes. We assume here that there is no member with a size that is less
     * than BytesPerLine.
     */
    if (Count < MemberSize) {
	DataByteLine (Count);
        PC += Count;
	return Count;
    }

    /* Make Count an even number of multiples of MemberSize */
    Count &= ~(MemberSize-1);

    /* Output as many data bytes lines as needed */
    BytesLeft = Count;
    while (BytesLeft > 0) {

     	/* Calculate the number of bytes for the next line */
     	unsigned Chunk = (BytesLeft > BytesPerLine)? BytesPerLine : BytesLeft;

     	/* Output a line with these bytes */
     	TableFunc (Chunk);

     	/* Next line */
     	BytesLeft -= Chunk;
     	PC        += Chunk;
    }

    /* If the next line is not the same style, add a separator */
    if (CodeLeft() && GetStyleAttr (PC) != Style) {
	SeparatorLine ();
    }

    /* Return the number of bytes output */
    return Count;
}



unsigned ByteTable (void)
/* Output a table of bytes */
{
    /* Call the low level function */
    return DoTable (atByteTab, 1, DataByteLine);
}



unsigned DByteTable (void)
/* Output a table of dbytes */
{
    /* Call the low level function */
    return DoTable (atDByteTab, 2, DataDByteLine);
}



unsigned WordTable (void)
/* Output a table of words */
{
    /* Call the low level function */
    return DoTable (atWordTab, 2, DataWordLine);
}



unsigned DWordTable (void)
/* Output a table of double words */
{
    /* Call the low level function */
    return DoTable (atDWordTab, 4, DataDWordLine);
}



unsigned AddrTable (void)
/* Output a table of addresses */
{
    unsigned long BytesLeft = GetRemainingBytes ();
    unsigned long Start = PC;

    /* Loop while table bytes left and we don't need to create a label at the
     * current position.
     */
    while (BytesLeft && GetStyleAttr (PC) == atAddrTab) {

        unsigned Addr;

        /* If just one byte is left, define it and bail out */
        if (BytesLeft == 1 || GetStyleAttr (PC+1) != atAddrTab) {
            DataByteLine (1);
            ++PC;
            break;
        }

        /* More than one byte left. Define a forward label if necessary */
        ForwardLabel (1);

        /* Now get the address from the PC */
        Addr = GetCodeWord (PC);

	/* In pass 1, define a label, in pass 2 output the line */
	if (Pass == 1) {
	    if (!HaveLabel (Addr)) {
	    	AddIntLabel (Addr);
	    }
	} else {
	    const char* Label = GetLabel (Addr, PC);
	    if (Label == 0) {
	    	/* OOPS! Should not happen */
	    	Internal ("OOPS - Label for address 0x%06X disappeard!", Addr);
	    }
	    Indent (MCol);
	    Output (".addr");
	    Indent (ACol);
	    Output ("%s", Label);
	    LineComment (PC, 2);
	    LineFeed ();
	}

	/* Next table entry */
	PC        += 2;
       	BytesLeft -= 2;

        /* If we must define a label here, bail out */
        if (BytesLeft && MustDefLabel (PC)) {
            break;
        }
    }

    /* If the next line is not an address table line, add a separator */
    if (CodeLeft() && GetStyleAttr (PC) != atAddrTab) {
	SeparatorLine ();
    }

    /* Return the number of bytes output */
    return PC - Start;
}



unsigned RtsTable (void)
/* Output a table of RTS addresses (address - 1) */
{
    unsigned long BytesLeft = GetRemainingBytes ();
    unsigned long Start = PC;

    /* Loop while table bytes left and we don't need to create a label at the
     * current position.
     */
    while (BytesLeft && GetStyleAttr (PC) == atRtsTab) {

        unsigned Addr;

        /* If just one byte is left, define it and bail out */
        if (BytesLeft == 1 || GetStyleAttr (PC+1) != atRtsTab) {
            DataByteLine (1);
            ++PC;
            break;
        }

        /* More than one byte left. Define a forward label if necessary */
        ForwardLabel (1);

        /* Now get the address from the PC */
	Addr = (GetCodeWord (PC) + 1) & 0xFFFF;

	/* In pass 1, define a label, in pass 2 output the line */
	if (Pass == 1) {
	    if (!HaveLabel (Addr)) {
	    	AddIntLabel (Addr);
	    }
	} else {
	    const char* Label = GetLabel (Addr, PC);
	    if (Label == 0) {
	    	/* OOPS! Should not happen */
	     	Internal ("OOPS - Label for address 0x%06X disappeard!", Addr);
	    }
	    Indent (MCol);
	    Output (".word");
	    Indent (ACol);
	    Output ("%s-1", Label);
	    LineComment (PC, 2);
	    LineFeed ();
	}

	/* Next table entry */
	PC        += 2;
       	BytesLeft -= 2;

        /* If we must define a label here, bail out */
        if (BytesLeft && MustDefLabel (PC)) {
            break;
        }
    }

    /* If the next line is not a return address table line, add a separator */
    if (CodeLeft() && GetStyleAttr (PC) != atRtsTab) {
	SeparatorLine ();
    }

    /* Return the number of bytes output */
    return PC - Start;
}



unsigned TextTable (void)
/* Output a table of text messages */
{
    /* Count how many bytes may be output. */
    unsigned ByteCount = GetSpan (atTextTab);

    /* Output as many data bytes lines as needed. */
    unsigned BytesLeft = ByteCount;
    while (BytesLeft > 0) {

	unsigned I;

	/* Count the number of characters that can be output as such */
	unsigned Count = 0;
	while (Count < BytesLeft && Count < BytesPerLine*4-1) {
	    unsigned char C = GetCodeByte (PC + Count);
	    if (C >= 0x20 && C <= 0x7E && C != '\"') {
	    	++Count;
	    } else {
	    	break;
	    }
	}

	/* If we have text, output it */
	if (Count > 0) {
	    unsigned CBytes;
	    Indent (MCol);
	    Output (".byte");
	    Indent (ACol);
	    Output ("\"");
	    for (I = 0; I < Count; ++I) {
	     	Output ("%c", GetCodeByte (PC+I));
	    }
	    Output ("\"");
	    CBytes = Count;
	    while (CBytes > 0) {
		unsigned Chunk = CBytes;
		if (Chunk > BytesPerLine) {
		    Chunk = BytesPerLine;
		}
		LineComment (PC, Chunk);
		LineFeed ();
	     	CBytes -= Chunk;
		PC += Chunk;
	    }
	    BytesLeft -= Count;
	}

	/* Count the number of bytes that must be output as bytes */
	Count = 0;
	while (Count < BytesLeft && Count < BytesPerLine) {
	    unsigned char C = GetCodeByte (PC + Count);
       	    if (C < 0x20 || C > 0x7E || C == '\"') {
	    	++Count;
	    } else {
	    	break;
	    }
	}

	/* If we have raw output bytes, print them */
	if (Count > 0) {
	    DataByteLine (Count);
	    PC += Count;
	    BytesLeft -= Count;
	}

    }

    /* If the next line is not a byte table line, add a separator */
    if (CodeLeft() && GetStyleAttr (PC) != atTextTab) {
	SeparatorLine ();
    }

    /* Return the number of bytes output */
    return ByteCount;
}



