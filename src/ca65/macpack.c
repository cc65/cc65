/*****************************************************************************/
/*                                                                           */
/*				   macpack.c				     */
/*                                                                           */
/*	     Predefined macro packages for the ca65 macroassembler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2004 Ullrich von Bassewitz                                       */
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



/* common */
#include "check.h"

/* ca65 */
#include "error.h"
#include "scanner.h"
#include "macpack.h"



/*****************************************************************************/
/*     	       	    	       	     Data				     */
/*****************************************************************************/



/* Predefined packages */

/* Generic macros */
static char MacGeneric[] =
    ".macro  add     Arg1, Arg2\n"
    "        clc\n"
    "        .if .paramcount = 2\n"
    "        adc     Arg1, Arg2\n"
    "        .else\n"
    "        adc     Arg1\n"
    "        .endif\n"
    ".endmacro\n"
    ".macro  sub     Arg1, Arg2\n"
    "        sec\n"
    "        .if .paramcount = 2\n"
    "        sbc     Arg1, Arg2\n"
    "        .else\n"
    "        sbc     Arg1\n"
    "        .endif\n"
    ".endmacro\n";

/* Long branch macros */
static char MacLongBranch[] =
    ".macro  jeq     Target\n"
    "        .if     .match(Target, 0)\n"
    "        bne     *+5\n"
    "        jmp     Target\n"
    "        .elseif .def(Target) .and .const(Target) .and ((*+2)-(Target) <= 127)\n"
    "        beq     Target\n"
    "        .else\n"
    "        bne     *+5\n"
    "        jmp     Target\n"
    "        .endif\n"
    ".endmacro\n"
    ".macro  jne     Target\n"
    "        .if     .match(Target, 0)\n"
    "        beq     *+5\n"
    "        jmp     Target\n"
    "        .elseif .def(Target) .and .const(Target) .and ((*+2)-(Target) <= 127)\n"
    "        bne     Target\n"
    "        .else\n"
    "        beq     *+5\n"
    "        jmp     Target\n"
    "        .endif\n"
    ".endmacro\n"
    ".macro  jmi     Target\n"
    "        .if     .match(Target, 0)\n"
    "        bpl     *+5\n"
    "        jmp     Target\n"
    "        .elseif .def(Target) .and .const(Target) .and ((*+2)-(Target) <= 127)\n"
    "        bmi     Target\n"
    "        .else\n"
    "        bpl     *+5\n"
    "        jmp     Target\n"
    "        .endif\n"
    ".endmacro\n"
    ".macro  jpl     Target\n"
    "        .if     .match(Target, 0)\n"
    "        bmi     *+5\n"
    "        jmp     Target\n"
    "        .elseif .def(Target) .and .const(Target) .and ((*+2)-(Target) <= 127)\n"
    "        bpl     Target\n"
    "        .else\n"
    "        bmi     *+5\n"
    "        jmp     Target\n"
    "        .endif\n"
    ".endmacro\n"
    ".macro  jcs     Target\n"
    "        .if     .match(Target, 0)\n"
    "        bcc     *+5\n"
    "        jmp     Target\n"
    "        .elseif .def(Target) .and .const(Target) .and ((*+2)-(Target) <= 127)\n"
    "        bcs     Target\n"
    "        .else\n"
    "        bcc     *+5\n"
    "        jmp     Target\n"
    "        .endif\n"
    ".endmacro\n"
    ".macro  jcc     Target\n"
    "        .if     .match(Target, 0)\n"
    "        bcs     *+5\n"
    "        jmp     Target\n"
    "        .elseif .def(Target) .and .const(Target) .and ((*+2)-(Target) <= 127)\n"
    "        bcc     Target\n"
    "        .else\n"
    "        bcs     *+5\n"
    "        jmp     Target\n"
    "        .endif\n"
    ".endmacro\n"
    ".macro  jvs     Target\n"
    "        .if     .match(Target, 0)\n"
    "        bvc     *+5\n"
    "        jmp     Target\n"
    "        .elseif .def(Target) .and .const(Target) .and ((*+2)-(Target) <= 127)\n"
    "        bvs     Target\n"
    "        .else\n"
    "        bvc     *+5\n"
    "        jmp     Target\n"
    "        .endif\n"
    ".endmacro\n"
    ".macro  jvc     Target\n"
    "        .if     .match(Target, 0)\n"
    "        bvs     *+5\n"
    "        jmp     Target\n"
    "        .elseif .def(Target) .and .const(Target) .and ((*+2)-(Target) <= 127)\n"
    "        bvc     Target\n"
    "        .else\n"
    "        bvs     *+5\n"
    "        jmp     Target\n"
    "        .endif\n"
    ".endmacro\n";

/* Commodore specific macros */
static char MacCBM[] =
    ".macro scrcode str\n"
    "        .repeat .strlen(str), i\n"
    "                .if (.strat(str, i) >= '@' .and .strat(str, i) <= 'z')\n"
    "                        .byte .strat(str, i) - '@'\n"
    "                .elseif (.strat(str, i) >= 'A' .and .strat(str, i) <= 'Z')\n"
    "                        .byte .strat(str, i) - 'A' + 65\n"
    "                .elseif (.strat(str, i) = '[')\n"
    "                        .byte 27\n"
    "                .elseif (.strat(str, i) = ']')\n"
    "                        .byte 29\n"
    "                .elseif (.strat(str, i) = '^')\n"
    "                        .byte 30\n"
    "                .elseif (.strat(str, i) = '_')\n"
    "                        .byte 31\n"
    "                .else\n"
    "                        .byte .strat(str, i)\n"
    "                .endif\n"
    "        .endrepeat\n"
    ".endmacro\n";

/* CPU defines */
static char MacCPU[] =
    "CPU_ISET_6502      = $01\n"
    "CPU_ISET_6502X     = $02\n"
    "CPU_ISET_65SC02    = $04\n"
    "CPU_ISET_65C02     = $08\n"
    "CPU_ISET_65816     = $10\n"
    "CPU_ISET_SUNPLUS   = $20\n"
    "CPU_6502           = CPU_ISET_6502\n"
    "CPU_6502X          = CPU_ISET_6502|CPU_ISET_6502X\n"
    "CPU_65SC02         = CPU_ISET_6502|CPU_ISET_65SC02\n"
    "CPU_65C02          = CPU_ISET_6502|CPU_ISET_65SC02|CPU_ISET_65C02\n"
    "CPU_65816          = CPU_ISET_6502|CPU_ISET_65SC02|CPU_ISET_65816\n"
    "CPU_SUNPLUS        = CPU_ISET_SUNPLUS\n";



/* Table with pointers to the different packages */
static char* MacPackages [] = {
    MacGeneric,
    MacLongBranch,
    MacCBM,
    MacCPU
};



/*****************************************************************************/
/*     	       	       	       	     Code    				     */
/*****************************************************************************/



void InsertMacPack (unsigned Id)
/* Insert the macro package with the given id in the input stream */
{
    /* Check the parameter */
    CHECK (Id < sizeof (MacPackages) / sizeof (MacPackages [0]));

    /* Insert the package */
    NewInputData (MacPackages [Id], 0);
}



