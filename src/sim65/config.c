/*****************************************************************************/
/*                                                                           */
/*				   config.c				     */
/*                                                                           */
/*            Configuration file parsing for the sim65 6502 simulator        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* common */
#include "check.h"
#include "bitops.h"
#include "print.h"
#include "strutil.h"
#include "xmalloc.h"

/* sim65 */
#include "cfgdata.h"
#include "chip.h"
#include "error.h"
#include "global.h"
#include "memory.h"
#include "location.h"
#include "scanner.h"
#include "config.h"



/*****************************************************************************/
/*     	      	    		     Code		  		     */
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



static void ParseCPU (void)
/* Parse a CPU section */
{
    static const IdentTok Attributes [] = {
        {   "TYPE",             CFGTOK_TYPE             },
        {   "ADDRSPACE",        CFGTOK_ADDRSPACE        },
    };

    enum {
        atNone      = 0x0000,
        atType      = 0x0001,
        atAddrSpace = 0x0002
    };
    unsigned Attr = 0;
    unsigned long Size = 0;


    while (CfgTok == CFGTOK_IDENT) {

        cfgtok_t AttrTok;
        CfgSpecialToken (Attributes, ENTRY_COUNT (Attributes), "Attribute");
        AttrTok = CfgTok;

        /* An optional assignment follows */
        CfgNextTok ();
        CfgOptionalAssign ();

        /* Check which attribute was given */
        switch (AttrTok) {

            case CFGTOK_TYPE:
                FlagAttr (&Attr, atType, "TYPE");
                CfgAssureIdent ();
                /* ### */
                break;

            case CFGTOK_ADDRSPACE:
                FlagAttr (&Attr, atAddrSpace, "ADDRSPACE");
                CfgAssureInt ();
                CfgRangeCheck (0x1000, 0x1000000);
                Size = CfgIVal;
                break;

            default:
                FAIL ("Unexpected attribute token");

        }

        /* Skip the attribute value and an optional comma */
        CfgNextTok ();
        CfgOptionalComma ();
    }

    /* Must have some attributes */
    AttrCheck (Attr, atType, "TYPE");
    AttrCheck (Attr, atAddrSpace, "ADDRSPACE");

    /* Skip the semicolon */
    CfgConsumeSemi ();
}



static void ParseMemory (void)
/* Parse a MEMORY section */
{
    unsigned I;


    while (CfgTok == CFGTOK_INTCON) {

        Location* L;

        /* Remember the start address and skip it */
        unsigned long Start = CfgIVal;
        CfgNextTok ();

        /* .. must follow */
        CfgConsume (CFGTOK_DOTDOT, "`..' expected");

        /* End address must follow and must be greater than start */
        CfgAssureInt ();
        if (CfgIVal < Start) {
            CfgError ("Start address must be greater than end address");
        }

        /* Create a new location and add it to the list */
        L = NewLocation (Start, CfgIVal);
        CollAppend (&Locations, L);

        /* Skip the end address and the following colon */
        CfgNextTok ();
        CfgConsumeColon ();

        /* Parse attributes terminated by a semicolon */
        while (CfgTok == CFGTOK_IDENT) {

            /* Generate a new attribute with the given name, then skip it */
            CfgData* D = NewCfgData ();
            CfgNextTok ();

	    /* An optional assignment follows */
	    CfgOptionalAssign ();

            /* Check and assign the attribute value */
            switch (CfgTok) {

                case CFGTOK_INTCON:
                    D->Type   = CfgDataNumber;
                    D->V.IVal = CfgIVal;
                    break;

                case CFGTOK_STRCON:
                    D->Type   = CfgDataString;
                    D->V.SVal = xstrdup (CfgSVal);
                    break;

                case CFGTOK_IDENT:
                    D->Type   = CfgDataId;
                    D->V.SVal = xstrdup (CfgSVal);
                    break;

                default:
                    CfgError ("Invalid attribute type");
            }

            /* Add the attribute to the location */
            CollAppend (&L->Attributes, D);

	    /* Skip the attribute value and an optional comma */
	    CfgNextTok ();
	    CfgOptionalComma ();
	}

	/* Skip the semicolon */
	CfgConsumeSemi ();
    }

    /* Sort all memory locations */
    LocationSort (&Locations);

    /* Check the locations for overlaps and other problems */
    LocationCheck (&Locations);

    /* Now create the chip instances. Since we can only mirror existing chips,
     * we will first create all real chips and the mirrors in a second run.
     */
    for (I = 0; I < CollCount (&Locations); ++I) {

        int Index;
        CfgData* D;
        unsigned Range;         /* Address range for this chip */
        ChipInstance* CI;

        /* Get this location */
        Location* L = CollAtUnchecked (&Locations, I);

        /* Skip mirrors */
        if (LocationIsMirror (L)) {
            continue;
        }

        /* The chip must have an attribute "name" of type string */
        Index = LocationGetAttr (L, "name");
        D = CollAt (&L->Attributes, Index);
        CfgDataCheckType (D, CfgDataString);

        /* Remove the "name" attribute from the attribute list */
        CollDelete (&L->Attributes, Index);

        /* Create the chip instance for the address range */
        Range = L->End - L->Start + 1;
        CI = NewChipInstance (D->V.SVal, L->Start, Range, &L->Attributes);

        /* Delete the "name" attribute */
        FreeCfgData (D);

        /* Assign the chip instance to memory */
        MemAssignChip (CI, L->Start, Range);
    }

    /* Create the mirrors */
    for (I = 0; I < CollCount (&Locations); ++I) {

        const CfgData* D;
        unsigned MirrorAddr;    /* Mirror address */
        unsigned Range;         /* Address range for this chip */
        unsigned Offs;          /* Offset of the mirror */
        const ChipInstance* CI; /* Original chip instance */
        ChipInstance* MCI;      /* Mirrored chip instance */

        /* Get this location */
        const Location* L = CollAtUnchecked (&Locations, I);

        /* Skip non mirrors */
        if (!LocationIsMirror (L)) {
            continue;
        }

        /* Calculate the address range */
        Range = L->End - L->Start;

        /* Get the mirror address */
        D = CollConstAt (&L->Attributes, 0);
        MirrorAddr = (unsigned) D->V.IVal;

        /* For simplicity, get the chip instance we're mirroring from the
         * memory, instead of searching for the range in the list.
         */
        CI = MemGetChip (MirrorAddr);
        if (CI == 0) {
            /* We are mirroring an unassigned address */
            Error ("%s(%u): Mirroring an unassigned address",
                   CfgGetName (), L->Line);
        }

        /* Make sure we're mirroring the correct chip */
        CHECK (MirrorAddr >= CI->Addr && MirrorAddr < CI->Addr + CI->Size);

        /* Calculate the offset of the mirror */
        Offs = MirrorAddr - CI->Addr;

        /* Check if the mirror range is ok */
        if (Offs + Range > CI->Size) {
            Error ("%s(%u): Mirror range is too large", CfgGetName (), L->Line);
        }

        /* Clone the chip instance for the new location */
        MCI = MirrorChipInstance (CI, L->Start - Offs);

        /* Assign the chip instance to memory */
        MemAssignChip (MCI, L->Start, Range);
    }
}



static void ParseConfig (void)
/* Parse the config file */
{
    static const IdentTok BlockNames [] = {
       	{   "CPU",     	CFGTOK_CPU      },
       	{   "MEMORY",  	CFGTOK_MEMORY   },
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

            case CFGTOK_CPU:
                ParseCPU ();
                break;

            case CFGTOK_MEMORY:
                ParseMemory ();
                break;

	    default:
	     	FAIL ("Unexpected block token");

	}

	/* Skip closing brace */
       	CfgConsumeRCurly ();

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



