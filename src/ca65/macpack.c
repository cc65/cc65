/*****************************************************************************/
/*                                                                           */
/*				   macpack.c				     */
/*                                                                           */
/*	     Predefined macro packages for the ca65 macroassembler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2005, Ullrich von Bassewitz                                      */
/*                Römerstrasse 52                                            */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
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



/* Predefined macro packages converted into C strings by a perl script */
#include "cbm.inc"
#include "cpu.inc"
#include "generic.inc"
#include "longbranch.inc"

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
    NewInputData (MacPackages[Id], 0);
}



