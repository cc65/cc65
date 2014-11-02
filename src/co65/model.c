/*****************************************************************************/
/*                                                                           */
/*                                  model.c                                  */
/*                                                                           */
/*         o65 model definitions for the co65 object file converter          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
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
#include "strutil.h"

/* co65 */
#include "error.h"
#include "model.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Current model */
O65Model Model = O65_MODEL_NONE;

/* Name table */
static const char* NameTable[O65_MODEL_COUNT] = {
    "none",    
    "os/a65",
    "lunix",
    "cc65-module"
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



const char* GetModelName (O65Model M)
/* Map the model to its name. */
{
    if (M < 0 || M >= O65_MODEL_COUNT) {
        Internal ("O65 Model %d not found", M);
    }
    return NameTable[M];
}



O65Model FindModel (const char* ModelName)
/* Map a model name to its identifier. Return O65_MODEL_INVALID if the name
** could not be found. Case is ignored when comparing names.
*/
{
    O65Model M;
    for (M = O65_MODEL_NONE; M < O65_MODEL_COUNT; ++M) {
        if (StrCaseCmp (ModelName, NameTable[M]) == 0) {
            return M;
        }
    }
    return O65_MODEL_INVALID;
}
