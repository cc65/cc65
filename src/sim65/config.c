/*****************************************************************************/
/*                                                                           */
/*				   config.c				     */
/*                                                                           */
/*            Configuration file parsing for the sim65 6502 simulator        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2002 Ullrich von Bassewitz                                       */
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
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* common */
#include "check.h"
#include "bitops.h"
#include "print.h"
#include "xmalloc.h"

/* sim65 */
#include "chip.h"
#include "error.h"
#include "global.h"
#include "scanner.h"
#include "config.h"



/*****************************************************************************/
/*     	      	    		     Data				     */
/*****************************************************************************/



static void FlagAttr (unsigned* Flags, unsigned Mask, const char* Name)
/* Check if the item is already defined. Print an error if so. If not, set
 * the marker that we have a definition now.
 */
{
    if (*Flags & Mask) {
    	CfgError ("%s is already defined", Name);
    }
    *Flags |= Mask;
}



static void AttrCheck (unsigned Attr, unsigned Mask, const char* Name)
/* Check that a mandatory attribute was given */
{
    if ((Attr & Mask) == 0) {
	CfgError ("%s attribute is missing", Name);
    }
}



static void ParseChips (void)
/* Parse a CHIPS section */
{
    static const IdentTok Attributes [] = {
       	{   "ADDR",    	CFGTOK_ADDR     },
	{   "RANGE",   	CFGTOK_RANGE    },
    };

    /* Bits and stuff to remember which attributes we have read */
    enum {
	CA_ADDR  = 0x01,
	CA_RANGE = 0x02
    };
    unsigned Attr;

    /* Attribute values. Initialize to make gcc happy. */
    const Chip* C;
    unsigned Addr  = 0;
    unsigned Range = 0;

    while (CfgTok == CFGTOK_IDENT) {

	/* Search the chip with the given name */
	C = FindChip (CfgSVal);
	if (C == 0) {
	    CfgError ("No such chip: `%s'", CfgSVal);
	}

	/* Skip the name plus the following colon */
	CfgNextTok ();
	CfgConsumeColon ();

       	/* Read the attributes */
	Attr = 0;
	while (CfgTok == CFGTOK_IDENT) {

	    /* Map the identifier to a token */
	    cfgtok_t AttrTok;
	    CfgSpecialToken (Attributes, ENTRY_COUNT (Attributes), "Attribute");
	    AttrTok = CfgTok;

	    /* An optional assignment follows */
	    CfgNextTok ();
	    CfgOptionalAssign ();

	    /* Check which attribute was given */
	    switch (AttrTok) {

		case CFGTOK_ADDR:
		    CfgAssureInt ();
 		    CfgRangeCheck (0, 0xFFFF);
	      	    FlagAttr (&Attr, CA_ADDR, "ADDR");
		    Addr = (unsigned) CfgIVal;
		    break;

		case CFGTOK_RANGE:
		    CfgAssureInt ();
		    CfgRangeCheck (0, 0xFFFF);
      		    FlagAttr (&Attr, CA_RANGE, "RANGE");
 		    Range = (unsigned) CfgIVal;
		    break;

		default:
       	       	    FAIL ("Unexpected attribute token");

	    }

	    /* Skip the attribute value and an optional comma */
	    CfgNextTok ();
	    CfgOptionalComma ();
	}

	/* Skip the semicolon */
	CfgConsumeSemi ();

	/* Check for mandatory parameters */
	AttrCheck (Attr, CA_ADDR, "ADDR");
	AttrCheck (Attr, CA_RANGE, "RANGE");

	/* Address + Range may not exceed 16 bits */
       	if (((unsigned long) Range) > 0x10000UL - Addr) {
	    CfgError ("Range error");
	}

	/* Create the chip ## */

    }
}



static void ParseConfig (void)
/* Parse the config file */
{
    static const IdentTok BlockNames [] = {
       	{   "CHIPS",   	CFGTOK_CHIPS	},
    };
    cfgtok_t BlockTok;

    do {

	/* Read the block ident */
       	CfgSpecialToken (BlockNames, ENTRY_COUNT (BlockNames), "Block identifier");
	BlockTok = CfgTok;
	CfgNextTok ();

	/* Expected a curly brace */
	CfgConsume (CFGTOK_LCURLY, "`{' expected");

	/* Read the block */
	switch (BlockTok) {

	    case CFGTOK_CHIPS:
       	       	ParseChips ();
	     	break;

	    default:
	     	FAIL ("Unexpected block token");

	}

	/* Skip closing brace */
	CfgConsume (CFGTOK_RCURLY, "`}' expected");

    } while (CfgTok != CFGTOK_EOF);
}



void CfgRead (void)
/* Read the configuration */
{
    /* If we have a config name given, open the file, otherwise we will read
     * from a buffer.
     */
    CfgOpenInput ();

    /* Parse the file */
    ParseConfig ();

    /* Close the input file */
    CfgCloseInput ();
}



