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
#include "scanner.h"
#include "config.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* List of all memory locations */
static Collection Locations;

/* One memory location */
typedef struct Location Location;
struct Location {
    unsigned long       Start;          /* Start of memory location */
    unsigned long       End;            /* End memory location */
    Collection          Attributes;     /* Attributes given */
    unsigned            Line;           /* Line in config file */
    unsigned            Col;            /* Column in config file */
};



/*****************************************************************************/
/*		      		struct CfgData		       		     */
/*****************************************************************************/



static CfgData* NewCfgData (void)
/* Create and intialize a new CfgData struct, then return it. The function
 * uses the current output of the config scanner.
 */
{
    /* Get the length of the identifier */
    unsigned AttrLen = strlen (CfgSVal);

    /* Allocate memory */
    CfgData* D = xmalloc (sizeof (CfgData) + AttrLen);

    /* Initialize the fields */
    D->Type = CfgDataInvalid;                 
    D->Line = CfgErrorLine;
    D->Col  = CfgErrorCol;
    memcpy (D->Attr, CfgSVal, AttrLen+1);

    /* Return the new struct */
    return D;
}



/*****************************************************************************/
/*                              struct Location                              */
/*****************************************************************************/



static Location* NewLocation (unsigned long Start, unsigned long End)
/* Create a new location, initialize and return it */
{
    /* Allocate memory */
    Location* L = xmalloc (sizeof (Location));

    /* Initialize the fields */
    L->Start      = Start;
    L->End        = End;
    L->Attributes = EmptyCollection;
    L->Line       = CfgErrorLine;
    L->Col        = CfgErrorCol;

    /* Return the new struct */
    return L;
}



static int CmpLocations (void* Data attribute ((unused)),
		         const void* lhs, const void* rhs)
/* Compare function for CollSort */
{
    /* Cast the object pointers */
    const Location* Left  = (const Location*) rhs;
    const Location* Right = (const Location*) lhs;

    /* Do the compare */
    if (Left->Start < Right->Start) {
        return 1;
    } else if (Left->Start > Right->Start) {
        return -1;
    } else {
        return 0;
    }
}



static const CfgData* LocationFindAttr (const Location* L, const char* AttrName)
/* Find the attribute with the given name and return it. Return NULL if the
 * attribute was not found.
 */
{
    unsigned I;

    /* Walk through the attributes checking for a "mirror" attribute */
    for (I = 0; I < CollCount (&L->Attributes); ++I) {

        /* Get the next attribute */
        const CfgData* D = CollConstAt (&L->Attributes, I);

        /* Compare the name */
        if (StrCaseCmp (D->Attr, AttrName) == 0) {
            /* Found */
            return D;
        }
    }

    /* Not found */
    return 0;
}



static int LocationIsMirror (const Location* L)
/* Return true if the given location is a mirror of another one. */
{
    /* Find the "mirror" attribute */
    return (LocationFindAttr (L, "mirror") != 0);
}



/*****************************************************************************/
/*     	      	    		     Code				     */
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



static void ParseMemory (void)
/* Parse a MEMORY section */
{
    unsigned I;
    const Location* Last;


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
    CollSort (&Locations, CmpLocations, 0);

    /* Check for overlaps and other problems */
    Last = 0;
    for (I = 0; I < CollCount (&Locations); ++I) {

        /* Get this location */
        const Location* L = CollAtUnchecked (&Locations, I);

        /* Check for an overlap with the following location */
        if (Last && Last->End >= L->Start) {
            Error ("%s(%u): Address range overlap (overlapping entry is in line %u)",
                   CfgGetName(), L->Line, Last->Line);
        }

        /* If the location is a mirror, it must not have other attributes,
         * and the mirror attribute must be an integer.
         */
        if (LocationIsMirror (L)) {
            const CfgData* D;
            if (CollCount (&L->Attributes) > 1) {
                Error ("%s(%u): Location at address $%06lX is a mirror "
                       "but has attributes", CfgGetName(), L->Line, L->Start);
            }
            D = CollConstAt (&L->Attributes, 0);
            if (D->Type != CfgDataNumber) {
                Error ("%s(%u): Mirror attribute is not an integer",
                       CfgGetName (), L->Line);
            }
        }

        /* Remember this entry */
        Last = L;
    }
}



static void ParseConfig (void)
/* Parse the config file */
{
    static const IdentTok BlockNames [] = {
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

            case CFGTOK_MEMORY:
                ParseMemory ();
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



