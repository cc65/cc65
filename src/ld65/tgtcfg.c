/*****************************************************************************/
/*                                                                           */
/*				 tgtcfg.c				     */
/*                                                                           */
/*		 Target machine configurations the ld65 linker		     */
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



#include "binfmt.h"
#include "tgtcfg.h"



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
       	"ZP: start = $D2, size = $1C, type = rw;"
        "HEADER: start = $0000, size = $6, file = %O;"
        "RAM: start = $1F00, size = $9D1F, file = %O;"   /* 9D1F: matches upper bound BC1F */
    "}"
    "SEGMENTS {"
        "EXEHDR: load = HEADER, type = wprot;"
        "CODE: load = RAM, type = wprot, define = yes;"
        "RODATA: load = RAM, type = wprot;"
        "DATA: load = RAM, type = rw;"
        "BSS: load = RAM, type = bss, define = yes;"
	"ZEROPAGE: load = ZP, type = zp;"
        "AUTOSTRT: load = RAM, type = wprot;"
    "}";

static const char CfgC64 [] =
    "MEMORY {"
	"ZP: start = $02, size = $1A, type = rw;"
	"RAM: start = $7FF, size = $c801, file = %O;"
    "}"
    "SEGMENTS {"
       	"CODE: load = RAM, type = wprot;"
	"RODATA: load = RAM, type = wprot;"
	"DATA: load = RAM, type = rw;"
	"BSS: load = RAM, type = bss, define = yes;"
	"ZEROPAGE: load = ZP, type = zp;"
    "}";

static const char CfgC128 [] =
    "MEMORY {"
	"ZP: start = $02, size = $1A, type = rw;"
	"RAM: start = $1bff, size = $a401, file = %O;"
    "}"
    "SEGMENTS {"
       	"CODE: load = RAM, type = wprot;"
	"RODATA: load = RAM, type = wprot;"
	"DATA: load = RAM, type = rw;"
	"BSS: load = RAM, type = bss, define = yes;"
	"ZEROPAGE: load = ZP, type = zp;"
    "}";

static const char CfgAce [] =
    "";

static const char CfgPlus4 [] =
    "MEMORY {"
	"ZP: start = $02, size = $1A, type = rw;"
	"RAM: start = $0fff, size = $7001, file = %O;"
    "}"
    "SEGMENTS {"
       	"CODE: load = RAM, type = wprot;"
	"RODATA: load = RAM, type = wprot;"
	"DATA: load = RAM, type = rw;"
	"BSS: load = RAM, type = bss, define = yes;"
	"ZEROPAGE: load = ZP, type = zp;"
    "}";

static const char CfgCBM610 [] =
    "MEMORY {"
	"ZP: start = $02, size = $1A, type = rw;"
	"RAM: start = $0001, size = $FFF0, file = %O;"
    "}"
    "SEGMENTS {"
       	"CODE: load = RAM, type = wprot;"
	"RODATA: load = RAM, type = wprot;"
    	"DATA: load = RAM, type = rw;"
	"BSS: load = RAM, type = bss, define = yes;"
	"ZEROPAGE: load = ZP, type = zp;"
    "}";

static const char CfgPET [] =
    "MEMORY {"
	"ZP: start = $02, size = $1A, type = rw;"
	"RAM: start = $03FF, size = $7BFF, file = %O;"
    "}"
    "SEGMENTS {"
       	"CODE: load = RAM, type = wprot;"
	"RODATA: load = RAM, type = wprot;"
	"DATA: load = RAM, type = rw;"
	"BSS: load = RAM, type = bss, define = yes;"
	"ZEROPAGE: load = ZP, type = zp;"
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
       	"ZP: start = $00, size = $1A, type = rw;"
	"RAM: start = $800, size = $8E00, file = %O;"
    "}"
    "SEGMENTS { "
        "CODE: load = RAM, type = ro;"
        "RODATA: load = RAM, type = ro;"
        "DATA: load = RAM, type = rw;"
        "BSS: load = RAM, type = bss, define = yes;"
	"ZEROPAGE: load = ZP, type = zp;"
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



/* Target configurations for all systems */
const TargetDesc Targets [TGT_COUNT] = {
    {  	BINFMT_BINARY,  CfgNone 	},
    {  	BINFMT_BINARY,	CfgAtari	},
    {  	BINFMT_BINARY,	CfgC64 		},
    {  	BINFMT_BINARY,	CfgC128		},
    {  	BINFMT_BINARY,	CfgAce		},
    {  	BINFMT_BINARY,	CfgPlus4	},
    {  	BINFMT_BINARY,  CfgCBM610	},
    {  	BINFMT_BINARY,	CfgPET		},
    {  	BINFMT_BINARY,	CfgNES 	       	},
#if 0
    {   BINFMT_O65,     CfgLunix	},
    {   BINFMT_O65,	CfgOSA65	},
#endif
    {   BINFMT_BINARY,	CfgApple2	},
    {   BINFMT_BINARY,  CfgGeos		},
};



