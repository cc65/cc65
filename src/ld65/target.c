/*****************************************************************************/
/*                                                                           */
/*				 target.c				     */
/*                                                                           */
/*		   Target system support for the ld65 linker		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
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



#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "error.h"
#include "global.h"
#include "binfmt.h"
#include "scanner.h"
#include "config.h"
#include "target.h"



/*****************************************************************************/
/*			     Target configurations			     */
/*****************************************************************************/



static const char CfgNone [] =
    "MEMORY {"
     	"RAM: start = %S, size = $10000, file = %O;"
    "}"
    "SEGMENTS {"
        "CODE: load = RAM, type = rw;"
	"RODATA: load = RAM, type = rw;"
	"DATA: load = RAM, type = rw;"
	"BSS: load = RAM, type = bss, define = yes;"
    "}";

static const char CfgAtari [] =
    "MEMORY {"
        "HEADER: start = $0000, size = $6, file = %O;"
        "RAM: start = $1F00, size = $6100, file = %O;"
    "}"
    "SEGMENTS {"
        "EXEHDR: load = HEADER, type = wprot;"
        "CODE: load = RAM, type = wprot, define = yes;"
        "RODATA: load = RAM, type = wprot;"
        "DATA: load = RAM, type = rw;"
        "BSS: load = RAM, type = bss, define = yes;"
        "AUTOSTRT: load = RAM, type = wprot;"
    "}";

static const char CfgC64 [] =
    "MEMORY {"
	"RAM: start = $7FF, size = $c801, file = %O;"
    "}"
    "SEGMENTS {"
       	"CODE: load = RAM, type = wprot;"
	"RODATA: load = RAM, type = wprot;"
	"DATA: load = RAM, type = rw;"
	"BSS: load = RAM, type = bss, define = yes;"
    "}";

static const char CfgC128 [] =
    "MEMORY {"
	"RAM: start = $1bff, size = $a401, file = %O;"
    "}"
    "SEGMENTS {"
       	"CODE: load = RAM, type = wprot;"
	"RODATA: load = RAM, type = wprot;"
	"DATA: load = RAM, type = rw;"
	"BSS: load = RAM, type = bss, define = yes;"
    "}";

static const char CfgAce [] =
    "";

static const char CfgPlus4 [] =
    "MEMORY {"
	"RAM: start = $0fff, size = $7001, file = %O;"
    "}"
    "SEGMENTS {"
       	"CODE: load = RAM, type = wprot;"
	"RODATA: load = RAM, type = wprot;"
	"DATA: load = RAM, type = rw;"
	"BSS: load = RAM, type = bss, define = yes;"
    "}";

static const char CfgCBM610 [] =
    "MEMORY {"
	"RAM: start = $0001, size = $FFF0, file = %O;"
    "}"
    "SEGMENTS {"
       	"CODE: load = RAM, type = wprot;"
	"RODATA: load = RAM, type = wprot;"
    	"DATA: load = RAM, type = rw;"
	"BSS: load = RAM, type = bss, define = yes;"
    "}";

static const char CfgPET [] =
    "MEMORY {"
	"RAM: start = $03FF, size = $7BFF, file = %O;"
    "}"
    "SEGMENTS {"
       	"CODE: load = RAM, type = wprot;"
	"RODATA: load = RAM, type = wprot;"
	"DATA: load = RAM, type = rw;"
	"BSS: load = RAM, type = bss, define = yes;"
    "}";

static const char CfgNES [] =
    "MEMORY {"
	"RAM: start = $0200, size = $0600, file = \"\";"
       	"ROM: start = $8000, size = $8000, file = %O;"
    "}"
    "SEGMENTS {"
       	"CODE: load = ROM, type = ro;"
	"RODATA: load = ROM, type = ro;"
	"DATA: load = ROM, run = RAM, type = rw, define = yes;"
	"BSS: load = RAM, type = bss, define = yes;"
	"VECTORS: load = ROM, type = ro, start = $FFFA;"
    "}";

static const char CfgLunix [] =
    "MEMORY {"
	"COMBINED: start = $0000, size = $FFFF, file = %O;"
	"ZEROPAGE: start = $0000, size = $0100, file = %O;"
    "}"
    "SEGMENTS {"
       	"CODE: load = COMBINED, type = wprot;"
     	"RODATA: load = COMBINED, type = wprot;"
     	"DATA: load = COMBINED, type = rw, define = yes;"
     	"BSS: load = COMBINED, type = bss, define = yes;"
	"ZEROPAGE: load = ZEROPAGE, type = zp;"
    "}"
    "FILES {"
       	"%O: format = o65;"
    "}"
    "FORMATS {"
       	"o65: os = lunix, type = small,"
       	      "extsym = \"LUNIXKERNAL\", extsym = \"LIB6502\";"
    "}";

static const char CfgOSA65 [] =
    "MEMORY {"
       	"COMBINED: start = $0000, size = $FFFF, file = %O;"
	"ZEROPAGE: start = $0000, size = $0100, file = %O;"
    "}"
    "SEGMENTS {"
       	"CODE: load = COMBINED, type = wprot;"
       	"RODATA: load = COMBINED, type = wprot;"
       	"DATA: load = COMBINED, type = rw, define = yes;"
       	"BSS: load = COMBINED, type = bss, define = yes;"
	"ZEROPAGE: load = ZEROPAGE, type = zp;"
    "}"
    "FILES {"
	"%O: format = o65;"
    "}"
    "FORMATS {"
	"o65: os = osa65, type = small,"
	      "extsym = \"OSA2KERNAL\", extsym = \"LIB6502\";"
    "}";

static const char CfgApple2 [] =
    "MEMORY {"
	"RAM: start = $800, size = $8E00, file = %O;"
    "}"
    "SEGMENTS { "
        "CODE: load = RAM, type = ro;"
        "RODATA: load = RAM, type = ro;"
        "DATA: load = RAM, type = rw;"
        "BSS: load = RAM, type = bss, define = yes;"
    "}";

static const char CfgGeos [] =
    "MEMORY {"
	"HEADER: start = $204, size = 508, file = %O;"
       	"RAM: start = $400, size = $7C00, file = %O;"
    "}"
    "SEGMENTS { "
	"HEADER: load = HEADER, type = ro;"
        "CODE: load = RAM, type = ro;"
        "RODATA: load = RAM, type = ro;"
        "DATA: load = RAM, type = rw;"
        "BSS: load = RAM, type = bss, define = yes;"
    "}";



/*****************************************************************************/
/*     	      	    		     Data     	 			     */
/*****************************************************************************/



/* Supported systems */
#define TGT_NONE   	 0
#define TGT_ATARI  	 1 		/* Atari 8 bit machines */
#define TGT_C64	   	 2
#define TGT_C128   	 3
#define TGT_ACE	   	 4
#define TGT_PLUS4  	 5
#define TGT_CBM610 	 6 		/* CBM 600/700 family */
#define TGT_PET	   	 7 	   	/* CBM PET family */
#define TGT_NES	   	 8 		/* Nintendo Entertainment System */
#define TGT_LUNIX	 9
#define TGT_OSA65	10
#define TGT_APPLE2	11
#define TGT_GEOS	12
#define TGT_COUNT  	13 		/* Count of supported systems */



/* Structure describing a target */
typedef struct TargetCfg_ TargetCfg;
struct TargetCfg_ {
    const char*	   	Name;		/* Name of the system */
    unsigned char      	BinFmt;		/* Default binary format for the target */
    const char*	   	Cfg;		/* Pointer to configuration */
};

static const TargetCfg Targets [TGT_COUNT] = {
    {  	"none",	  BINFMT_BINARY,   	CfgNone 	},
    {  	"atari",  BINFMT_BINARY,	CfgAtari	},
    {  	"c64",    BINFMT_BINARY,	CfgC64 		},
    {  	"c128",   BINFMT_BINARY,	CfgC128		},
    {  	"ace",    BINFMT_BINARY,	CfgAce		},
    {  	"plus4",  BINFMT_BINARY,	CfgPlus4	},
    {  	"cbm610", BINFMT_BINARY,   	CfgCBM610	},
    {  	"pet", 	  BINFMT_BINARY,	CfgPET		},
    {  	"nes", 	  BINFMT_BINARY,	CfgNES 	       	},
    {   "lunix",  BINFMT_O65,           CfgLunix	},
    {   "osa65",  BINFMT_O65,		CfgOSA65	},
    {   "apple2", BINFMT_BINARY,	CfgApple2	},
    {   "geos",	  BINFMT_BINARY,       	CfgGeos		},
};

/* Selected target system type */
static const TargetCfg* Target;



/*****************************************************************************/
/*     	      	    	 	     Code			 	     */
/*****************************************************************************/



static int StrICmp (const char* S1, const char* S2)
/* Compare two strings case insensitive */
{
    int Diff = 0;
    while (1) {
	Diff = tolower (*S1) - tolower (*S2);
	if (Diff != 0 || *S1 == '\0') {
	    return Diff;
	}
    	++S1;
	++S2;
    }
}



static int TgtMap (const char* Name)
/* Map a target name to a system code. Return -1 in case of an error */
{
    unsigned I;

    /* Check for a numeric target */
    if (isdigit (*Name)) {
 	int Target = atoi (Name);
 	if (Target >= 0 && Target < TGT_COUNT) {
 	    return Target;
      	}
    }

    /* Check for a target string */
    for (I = 0; I < TGT_COUNT; ++I) {
       	if (StrICmp (Targets [I].Name, Name) == 0) {
 	    return I;
 	}
    }

    /* Not found */
    return -1;
}



void TgtSet (const char* T)
/* Set the target system, initialize internal stuff for this target */
{
    /* Map the target to a number */
    int TgtNum = TgtMap (T);
    if (TgtNum == -1) {
	Error ("Invalid target system: %s", T);
    }
    Target = &Targets [TgtNum];

    /* Set the target data */
    DefaultBinFmt = Target->BinFmt;
    CfgSetBuf (Target->Cfg);
}



void TgtPrintList (FILE* F)
/* Print a list of the available target systems */
{
    unsigned I;

    /* Print a header */
    fprintf (F, "Available targets:\n");

    /* Print a list of the target systems */
    for (I = 0; I < TGT_COUNT; ++I) {
	fprintf (F, "  %s\n", Targets [I].Name);
    }
}



