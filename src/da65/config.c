/*****************************************************************************/
/*                                                                           */
/*		    		   config.c				     */
/*                                                                           */
/*		   Disassembler configuration file handling		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000     Ullrich von Bassewitz                                        */
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
#include <string.h>
#if defined(_MSC_VER)
/* Microsoft compiler */
#  include <io.h>
#else
/* Anyone else */
#  include <unistd.h>
#endif

/* common */
#include "xmalloc.h"

/* da65 */
#include "attrtab.h"
#include "error.h"
#include "global.h"
#include "scanner.h"
#include "config.h"



/*****************************************************************************/
/*     	       	       	       	     Code     				     */
/*****************************************************************************/



static void GlobalSection (void)
/* Parse a global section */
{
    static const IdentTok GlobalDefs[] = {
       	{   "INPUTNAME",  	CFGTOK_INPUTNAME	},
	{   "OUTPUTNAME",      	CFGTOK_OUTPUTNAME	},
	{   "PAGELENGTH",      	CFGTOK_PAGELENGTH	},
	{   "STARTADDR",	CFGTOK_STARTADDR	},
    };

    /* Skip the token */
    CfgNextTok ();

    /* Expect the opening curly brace */
    CfgConsumeLCurly ();

    /* Look for section tokens */
    while (CfgTok != CFGTOK_RCURLY) {

	/* Convert to special token */
       	CfgSpecialToken (GlobalDefs, ENTRY_COUNT (GlobalDefs), "Global directive");

	/* Look at the token */
	switch (CfgTok) {

	    case CFGTOK_INPUTNAME:
	        CfgNextTok ();
		CfgAssureStr ();
		if (InFile) {
		    CfgError ("Input file name already given");
		}
		InFile = xstrdup (CfgSVal);
		CfgNextTok ();
		break;

	    case CFGTOK_OUTPUTNAME:
		CfgNextTok ();
		CfgAssureStr ();
		if (OutFile) {
		    CfgError ("Output file name already given");
		}
		OutFile = xstrdup (CfgSVal);
		CfgNextTok ();
		break;

	    case CFGTOK_PAGELENGTH:
		CfgNextTok ();
		CfgAssureInt ();
		if (CfgIVal != -1) {
		    CfgRangeCheck (MIN_PAGE_LEN, MAX_PAGE_LEN);
		}
		PageLength = CfgIVal;
		CfgNextTok ();
		break;

	    case CFGTOK_STARTADDR:
		CfgNextTok ();
		CfgAssureInt ();
		CfgRangeCheck (0x0000, 0xFFFF);
		StartAddr = CfgIVal;
		CfgNextTok ();
		break;

	}

	/* Directive is followed by a semicolon */
	CfgConsumeSemi ();

    }

    /* Consume the closing brace */
    CfgConsumeRCurly ();
}



static void RangeSection (void)
/* Parse a range section */
{
    static const IdentTok RangeDefs[] = {
       	{   "START",   	    	CFGTOK_START	},
	{   "END",	    	CFGTOK_END 	},
	{   "TYPE",            	CFGTOK_TYPE	},
    };

    static const IdentTok TypeDefs[] = {
	{   "CODE",	    	CFGTOK_CODE	},
	{   "BYTETABLE",    	CFGTOK_BYTETAB	},
	{   "WORDTABLE",    	CFGTOK_WORDTAB	},
	{   "DWORDTABLE",	CFGTOK_DWORDTAB	},
	{   "ADDRTABLE",	CFGTOK_ADDRTAB	},
	{   "RTSTABLE",	    	CFGTOK_RTSTAB	},
	{   "TEXTTABLE",        CFGTOK_TEXTTAB  },
    };


    /* Which values did we get? */
    enum {
	tNone	= 0x00,
	tStart	= 0x01,
	tEnd	= 0x02,
	tType	= 0x04,
	tAll	= 0x07
    } Needed = tNone;

    /* Locals - initialize to avoid gcc warnings */
    unsigned Start	= 0;
    unsigned End	= 0;
    unsigned char Type	= 0;

    /* Skip the token */
    CfgNextTok ();

    /* Expect the opening curly brace */
    CfgConsumeLCurly ();

    /* Look for section tokens */
    while (CfgTok != CFGTOK_RCURLY) {

	/* Convert to special token */
       	CfgSpecialToken (RangeDefs, ENTRY_COUNT (RangeDefs), "Range directive");

	/* Look at the token */
	switch (CfgTok) {

	    case CFGTOK_START:
	        CfgNextTok ();
		CfgAssureInt ();
		CfgRangeCheck (0x0000, 0xFFFF);
		Start = CfgIVal;
	       	Needed |= tStart;
	     	CfgNextTok ();
	     	break;

	    case CFGTOK_END:
	     	CfgNextTok ();
		CfgAssureInt ();
		CfgRangeCheck (0x0000, 0xFFFF);
		End = CfgIVal;
	       	Needed |= tEnd;
	     	CfgNextTok ();
	     	break;

	    case CFGTOK_TYPE:
	     	CfgNextTok ();
		CfgSpecialToken (TypeDefs, ENTRY_COUNT (TypeDefs), "Type");
		switch (CfgTok) {
		    case CFGTOK_CODE:		Type = atCode;		break;
		    case CFGTOK_BYTETAB:	Type = atByteTab;	break;
		    case CFGTOK_WORDTAB:	Type = atWordTab;	break;
		    case CFGTOK_DWORDTAB:	Type = atDWordTab;	break;
		    case CFGTOK_ADDRTAB:	Type = atAddrTab;	break;
       		    case CFGTOK_RTSTAB:		Type = atRtsTab;	break;
		    case CFGTOK_TEXTTAB:       	Type = atTextTab;       break;
		}
		Needed |= tType;
		CfgNextTok ();
		break;
	}

	/* Directive is followed by a semicolon */
	CfgConsumeSemi ();

    }

    /* Did we get all required values? */
    if (Needed != tAll) {
    	CfgError ("Required values missing from this section");
    }

    /* Start must be less than end */
    if (Start > End) {
	CfgError ("Start value must not be greater than end value");
    }

    /* Set the range */
    MarkRange (Start, End, Type);

    /* Consume the closing brace */
    CfgConsumeRCurly ();
}



static void LabelSection (void)
/* Parse a label section */
{
    static const IdentTok LabelDefs[] = {
       	{   "NAME",	CFGTOK_NAME	},
	{   "ADDR",	CFGTOK_ADDR	},
       	{   "SIZE",    	CFGTOK_SIZE	},
    };

    /* Locals - initialize to avoid gcc warnings */
    char* Name = 0;
    long Value = -1;
    long Size  = -1;

    /* Skip the token */
    CfgNextTok ();

    /* Expect the opening curly brace */
    CfgConsumeLCurly ();

    /* Look for section tokens */
    while (CfgTok != CFGTOK_RCURLY) {

	/* Convert to special token */
       	CfgSpecialToken (LabelDefs, ENTRY_COUNT (LabelDefs), "Label directive");

	/* Look at the token */
	switch (CfgTok) {

	    case CFGTOK_NAME:
	        CfgNextTok ();
	       	if (Name) {
	       	    CfgError ("Name already given");
	       	}
	       	CfgAssureStr ();
		if (CfgSVal[0] == '\0') {
		    CfgError ("Name may not be empty");
		}
	       	Name = xstrdup (CfgSVal);
	       	CfgNextTok ();
	       	break;

	    case CFGTOK_ADDR:
	       	CfgNextTok ();
	       	if (Value >= 0) {
	       	    CfgError ("Value already given");
	       	}
	       	CfgAssureInt ();
		CfgRangeCheck (0, 0xFFFF);
	       	Value = CfgIVal;
	       	CfgNextTok ();
	       	break;

	    case CFGTOK_SIZE:
	       	CfgNextTok ();
	       	if (Size >= 0) {
	       	    CfgError ("Size already given");
	       	}
	       	CfgAssureInt ();
		CfgRangeCheck (1, 0x10000);
	       	Size = CfgIVal;
	       	CfgNextTok ();
	       	break;

	}

	/* Directive is followed by a semicolon */
	CfgConsumeSemi ();
    }

    /* Did we get the necessary data */
    if (Name == 0) {
	CfgError ("Label name is missing");
    }
    if (Value < 0) {
	CfgError ("Label value is missing");
    }
    if (Size < 0) {
	/* Use default */
	Size = 1;
    }
    if (Value + Size > 0x10000) {
	CfgError ("Invalid size (address out of range)");
    }
    if (HaveLabel ((unsigned) Value)) {
	CfgError ("Label for address $%04lX already defined", Value);
    }

    /* Define the label */
    AddLabel ((unsigned) Value, atExtLabel, Name);

    /* Define dependent labels if necessary */
    if (Size > 1) {
	unsigned Offs;

	/* Allocate memory for the dependent label names */
	unsigned NameLen = strlen (Name);
	char*	 DepName = xmalloc (NameLen + 7);
	char*	 DepOffs = DepName + NameLen + 1;

	/* Copy the original name into the buffer */
	memcpy (DepName, Name, NameLen);
	DepName[NameLen] = '+';

	/* Define the labels */
	for (Offs = 1; Offs < (unsigned) Size; ++Offs) {
	    sprintf (DepOffs, "%u", Offs);
	    AddLabel ((unsigned) Value+Offs, atDepLabel, DepName);
	}

	/* Free the name buffer */
	xfree (DepName);
    }

    /* Delete the dynamically allocated memory for Name */
    xfree (Name);

    /* Consume the closing brace */
    CfgConsumeRCurly ();
}



static void CfgParse (void)
/* Parse the config file */
{
    static const IdentTok Globals[] = {
     	{   "GLOBAL", 	CFGTOK_GLOBAL	},
     	{   "RANGE",	CFGTOK_RANGE	},
	{   "LABEL",	CFGTOK_LABEL	},
    };

    while (CfgTok != CFGTOK_EOF) {

	/* Convert an identifier into a token */
	CfgSpecialToken (Globals, ENTRY_COUNT (Globals), "Config directive");

	/* Check the token */
	switch (CfgTok) {

	    case CFGTOK_GLOBAL:
		GlobalSection ();
		break;

	    case CFGTOK_RANGE:
		RangeSection ();
		break;

	    case CFGTOK_LABEL:
		LabelSection ();
		break;

	}

	/* Semicolon expected */
	CfgConsumeSemi ();
    }
}



void CfgRead (void)
/* Read the configuration if a configuration file exists */
{
    /* Check if we have a config file given */
    if (!CfgAvail() || access (CfgGetName(), 0) != 0) {
	/* No name given or file not found */
    	return;
    }

    /* Open the config file */
    CfgOpenInput ();

    /* Parse the config file */
    CfgParse ();

    /* Close the file */
    CfgCloseInput ();
}






