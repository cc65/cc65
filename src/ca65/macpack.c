/*****************************************************************************/
/*                                                                           */
/*				   macpack.c				     */
/*                                                                           */
/*	     Predefined macro packages for the ca65 macroassembler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998     Ullrich von Bassewitz                                        */
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
static const char MacGeneric [] = 	/* Generic macros */
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



static const char MacLongBranch [] =	/* Long branch macros */
    ".macro  jeq     Target\n"
    "        .if     .match(Target, 0)\n"
    "        bne     *+5\n"
    "        jmp     Target\n"
    "        .elseif .def(Target,local) .and ((*+2)-(Target) <= 127)\n"
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
    "        .elseif .def(Target,local) .and ((*+2)-(Target) <= 127)\n"
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
    "        .elseif .def(Target,local) .and ((*+2)-(Target) <= 127)\n"
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
    "        .elseif .def(Target,local) .and ((*+2)-(Target) <= 127)\n"
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
    "        .elseif .def(Target,local) .and ((*+2)-(Target) <= 127)\n"
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
    "        .elseif .def(Target,local) .and ((*+2)-(Target) <= 127)\n"
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
    "        .elseif .def(Target,local) .and ((*+2)-(Target) <= 127)\n"
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
    "        .elseif .def(Target,local) .and ((*+2)-(Target) <= 127)\n"
    "        bvc     Target\n"
    "        .else\n"
    "        bvs     *+5\n"
    "        jmp     Target\n"
    "        .endif\n"
    ".endmacro\n";



/* Table with pointers to the different packages */
static const char* MacPackages [] = {
    MacGeneric,
    MacLongBranch,
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



