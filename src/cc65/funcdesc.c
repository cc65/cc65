/*****************************************************************************/
/*                                                                           */
/*                                funcdesc.c                                 */
/*                                                                           */
/*           Function descriptor structure for the cc65 C compiler           */
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



/* common */
#include "xmalloc.h"

/* cc65 */
#include "funcdesc.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



FuncDesc* NewFuncDesc (void)
/* Create a new symbol table with the given name */
{
    /* Create a new function descriptor */
    FuncDesc* F = (FuncDesc*) xmalloc (sizeof (FuncDesc));

    /* Nullify the fields */
    F->Flags           = 0;
    F->SymTab          = 0;
    F->TagTab          = 0;
    F->ParamCount      = 0;
    F->ParamSize       = 0;
    F->LastParam       = 0;
    F->FuncDef         = 0;

    /* Return the new struct */
    return F;
}



void FreeFuncDesc (FuncDesc* F)
/* Free a function descriptor */
{
    /* Free the structure */
    xfree (F);
}
