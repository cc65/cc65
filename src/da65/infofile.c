/*****************************************************************************/
/*                                                                           */
/*                                infofile.h                                 */
/*                                                                           */
/*                      Disassembler info file handling                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2003 Ullrich von Bassewitz                                       */
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
#include <string.h>
#if defined(_MSC_VER)
/* Microsoft compiler */
#  include <io.h>
#else
/* Anyone else */
#  include <unistd.h>
#endif

/* common */
#include "cpu.h"
#include "xmalloc.h"

/* da65 */
#include "attrtab.h"
#include "error.h"
#include "global.h"
#include "infofile.h"
#include "opctable.h"
#include "scanner.h"



/*****************************************************************************/
/*     	       	       	       	     Code     				     */
/*****************************************************************************/



static void GlobalSection (void)
/* Parse a global section */
{
    static const IdentTok GlobalDefs[] = {
       	{   "CPU",     	        INFOTOK_CPU     	},
       	{   "INPUTNAME",  	INFOTOK_INPUTNAME	},
	{   "OUTPUTNAME",      	INFOTOK_OUTPUTNAME	},
	{   "PAGELENGTH",      	INFOTOK_PAGELENGTH	},
	{   "STARTADDR",	INFOTOK_STARTADDR	},
    };

    /* Skip the token */
    InfoNextTok ();

    /* Expect the opening curly brace */
    InfoConsumeLCurly ();

    /* Look for section tokens */
    while (InfoTok != INFOTOK_RCURLY) {

	/* Convert to special token */
       	InfoSpecialToken (GlobalDefs, ENTRY_COUNT (GlobalDefs), "Global directive");

	/* Look at the token */
	switch (InfoTok) {

            case INFOTOK_CPU:
                InfoNextTok ();
                InfoAssureStr ();
                if (CPU != CPU_UNKNOWN) {
                    InfoError ("CPU already specified");
                }
                CPU = FindCPU (InfoSVal);
                SetOpcTable (CPU);
                InfoNextTok ();
                break;

	    case INFOTOK_INPUTNAME:
	        InfoNextTok ();
		InfoAssureStr ();
		if (InFile) {
		    InfoError ("Input file name already given");
		}
		InFile = xstrdup (InfoSVal);
		InfoNextTok ();
		break;

	    case INFOTOK_OUTPUTNAME:
		InfoNextTok ();
		InfoAssureStr ();
		if (OutFile) {
		    InfoError ("Output file name already given");
		}
		OutFile = xstrdup (InfoSVal);
		InfoNextTok ();
		break;

	    case INFOTOK_PAGELENGTH:
		InfoNextTok ();
		InfoAssureInt ();
		if (InfoIVal != -1) {
		    InfoRangeCheck (MIN_PAGE_LEN, MAX_PAGE_LEN);
		}
		PageLength = InfoIVal;
		InfoNextTok ();
		break;

	    case INFOTOK_STARTADDR:
		InfoNextTok ();
		InfoAssureInt ();
		InfoRangeCheck (0x0000, 0xFFFF);
		StartAddr = InfoIVal;
		InfoNextTok ();
		break;

	}

	/* Directive is followed by a semicolon */
	InfoConsumeSemi ();

    }

    /* Consume the closing brace */
    InfoConsumeRCurly ();
}



static void RangeSection (void)
/* Parse a range section */
{
    static const IdentTok RangeDefs[] = {
       	{   "START",   	    	INFOTOK_START	},
	{   "END",	    	INFOTOK_END 	},
	{   "TYPE",            	INFOTOK_TYPE	},
    };

    static const IdentTok TypeDefs[] = {
	{   "CODE",	    	INFOTOK_CODE	 },
	{   "BYTETABLE",    	INFOTOK_BYTETAB	 },
	{   "WORDTABLE",    	INFOTOK_WORDTAB	 },
	{   "DWORDTABLE",	INFOTOK_DWORDTAB },
	{   "ADDRTABLE",	INFOTOK_ADDRTAB	 },
	{   "RTSTABLE",	    	INFOTOK_RTSTAB	 },
	{   "TEXTTABLE",        INFOTOK_TEXTTAB  },
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
    InfoNextTok ();

    /* Expect the opening curly brace */
    InfoConsumeLCurly ();

    /* Look for section tokens */
    while (InfoTok != INFOTOK_RCURLY) {

	/* Convert to special token */
       	InfoSpecialToken (RangeDefs, ENTRY_COUNT (RangeDefs), "Range directive");

	/* Look at the token */
	switch (InfoTok) {

	    case INFOTOK_START:
	        InfoNextTok ();
		InfoAssureInt ();
		InfoRangeCheck (0x0000, 0xFFFF);
		Start = InfoIVal;
	       	Needed |= tStart;
	     	InfoNextTok ();
	     	break;

	    case INFOTOK_END:
	     	InfoNextTok ();
		InfoAssureInt ();
		InfoRangeCheck (0x0000, 0xFFFF);
		End = InfoIVal;
	       	Needed |= tEnd;
	     	InfoNextTok ();
	     	break;

	    case INFOTOK_TYPE:
	     	InfoNextTok ();
		InfoSpecialToken (TypeDefs, ENTRY_COUNT (TypeDefs), "Type");
		switch (InfoTok) {
		    case INFOTOK_CODE:		Type = atCode;		break;
		    case INFOTOK_BYTETAB:	Type = atByteTab;	break;
		    case INFOTOK_WORDTAB:	Type = atWordTab;	break;
		    case INFOTOK_DWORDTAB:	Type = atDWordTab;	break;
		    case INFOTOK_ADDRTAB:	Type = atAddrTab;	break;
       		    case INFOTOK_RTSTAB:       	Type = atRtsTab;	break;
		    case INFOTOK_TEXTTAB:       Type = atTextTab;       break;
		}
		Needed |= tType;
		InfoNextTok ();
		break;
	}

	/* Directive is followed by a semicolon */
	InfoConsumeSemi ();

    }

    /* Did we get all required values? */
    if (Needed != tAll) {
    	InfoError ("Required values missing from this section");
    }

    /* Start must be less than end */
    if (Start > End) {
	InfoError ("Start value must not be greater than end value");
    }

    /* Set the range */
    MarkRange (Start, End, Type);

    /* Consume the closing brace */
    InfoConsumeRCurly ();
}



static void LabelSection (void)
/* Parse a label section */
{
    static const IdentTok LabelDefs[] = {
       	{   "NAME",	INFOTOK_NAME	},
	{   "ADDR",	INFOTOK_ADDR	},
       	{   "SIZE",    	INFOTOK_SIZE	},
    };

    /* Locals - initialize to avoid gcc warnings */
    char* Name = 0;
    long Value = -1;
    long Size  = -1;

    /* Skip the token */
    InfoNextTok ();

    /* Expect the opening curly brace */
    InfoConsumeLCurly ();

    /* Look for section tokens */
    while (InfoTok != INFOTOK_RCURLY) {

	/* Convert to special token */
       	InfoSpecialToken (LabelDefs, ENTRY_COUNT (LabelDefs), "Label directive");

	/* Look at the token */
	switch (InfoTok) {

	    case INFOTOK_NAME:
	        InfoNextTok ();
	       	if (Name) {
	       	    InfoError ("Name already given");
	       	}
	       	InfoAssureStr ();
		if (InfoSVal[0] == '\0') {
		    InfoError ("Name may not be empty");
		}
	       	Name = xstrdup (InfoSVal);
	       	InfoNextTok ();
	       	break;

	    case INFOTOK_ADDR:
	       	InfoNextTok ();
	       	if (Value >= 0) {
	       	    InfoError ("Value already given");
	       	}
	       	InfoAssureInt ();
		InfoRangeCheck (0, 0xFFFF);
	       	Value = InfoIVal;
	       	InfoNextTok ();
	       	break;

	    case INFOTOK_SIZE:
	       	InfoNextTok ();
	       	if (Size >= 0) {
	       	    InfoError ("Size already given");
	       	}
	       	InfoAssureInt ();
		InfoRangeCheck (1, 0x10000);
	       	Size = InfoIVal;
	       	InfoNextTok ();
	       	break;

	}

	/* Directive is followed by a semicolon */
	InfoConsumeSemi ();
    }

    /* Did we get the necessary data */
    if (Name == 0) {
	InfoError ("Label name is missing");
    }
    if (Value < 0) {
	InfoError ("Label value is missing");
    }
    if (Size < 0) {
	/* Use default */
	Size = 1;
    }
    if (Value + Size > 0x10000) {
	InfoError ("Invalid size (address out of range)");
    }
    if (HaveLabel ((unsigned) Value)) {
	InfoError ("Label for address $%04lX already defined", Value);
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
    InfoConsumeRCurly ();
}



static void InfoParse (void)
/* Parse the config file */
{
    static const IdentTok Globals[] = {
     	{   "GLOBAL", 	INFOTOK_GLOBAL	},
     	{   "RANGE",	INFOTOK_RANGE	},
	{   "LABEL",	INFOTOK_LABEL	},
    };

    while (InfoTok != INFOTOK_EOF) {

	/* Convert an identifier into a token */
	InfoSpecialToken (Globals, ENTRY_COUNT (Globals), "Config directive");

	/* Check the token */
	switch (InfoTok) {

	    case INFOTOK_GLOBAL:
		GlobalSection ();
		break;

	    case INFOTOK_RANGE:
		RangeSection ();
		break;

	    case INFOTOK_LABEL:
		LabelSection ();
		break;

	}

	/* Semicolon expected */
	InfoConsumeSemi ();
    }
}



void ReadInfoFile (void)
/* Read the info file */
{
    /* Check if we have a info file given */
    if (InfoAvail()) {
        /* Open the config file */
        InfoOpenInput ();

        /* Parse the config file */
        InfoParse ();

        /* Close the file */
        InfoCloseInput ();
    }
}






