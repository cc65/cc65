/*****************************************************************************/
/*                                                                           */
/*				   config.c				     */
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
       	{   "START",   		CFGTOK_START	},
	{   "END",		CFGTOK_END	},
	{   "TYPE",            	CFGTOK_TYPE	},
    };

    /* Skip the token */
    CfgNextTok ();

    /* Expect the opening curly brace */
    CfgConsumeLCurly ();

    /* Look for section tokens */
    while (CfgTok != CFGTOK_RCURLY) {


    }

    /* Consume the closing brace */
    CfgConsumeRCurly ();
}



static void LabelSection (void)
/* Parse a label section */
{
    static const IdentTok Globals[] = {
       	{   "INPUTNAMEL",	CFGTOK_INPUTNAME	},
	{   "OUTPUTNAME",      	CFGTOK_OUTPUTNAME	},
	{   "PAGELENGTH",      	CFGTOK_PAGELENGTH	},
    };

    /* Skip the token */
    CfgNextTok ();

    /* Expect the opening curly brace */
    CfgConsumeLCurly ();

    /* Look for section tokens */
    while (CfgTok != CFGTOK_RCURLY) {


    }

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






