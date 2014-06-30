/*****************************************************************************/
/*                                                                           */
/*                                 mmodel.c                                  */
/*                                                                           */
/*                         Memory model definitions                          */
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



#include <string.h>

/* common */
#include "addrsize.h"
#include "mmodel.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Memory model in use */
mmodel_t MemoryModel = MMODEL_UNKNOWN;

/* Table with memory model names */
static const char* MemoryModelNames[MMODEL_COUNT] = {
    "near",
    "far",
    "huge",
};

/* Address sizes for the segments */
unsigned char CodeAddrSize = ADDR_SIZE_ABS;
unsigned char DataAddrSize = ADDR_SIZE_ABS;
unsigned char ZpAddrSize   = ADDR_SIZE_ZP;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



mmodel_t FindMemoryModel (const char* Name)
/* Find a memory model by name. Return MMODEL_UNKNOWN for an unknown name. */
{
    unsigned I;

    /* Check all CPU names */
    for (I = 0; I < MMODEL_COUNT; ++I) {
        if (strcmp (MemoryModelNames[I], Name) == 0) {
            return (mmodel_t)I;
        }
    }

    /* Not found */
    return MMODEL_UNKNOWN;
}



void SetMemoryModel (mmodel_t Model)
/* Set the memory model updating the MemoryModel variables and the address
** sizes for the segments.
*/
{
    /* Remember the memory model */
    MemoryModel = Model;

    /* Set the address sizes for the segments */
    switch (MemoryModel) {

        case MMODEL_NEAR:
            /* Code: near, data: near */
            CodeAddrSize = ADDR_SIZE_ABS;
            DataAddrSize = ADDR_SIZE_ABS;
            break;

        case MMODEL_FAR:
            /* Code: far, data: near */
            CodeAddrSize = ADDR_SIZE_FAR;
            DataAddrSize = ADDR_SIZE_ABS;
            break;

        case MMODEL_HUGE:
            /* Code: far, data: far */
            CodeAddrSize = ADDR_SIZE_FAR;
            DataAddrSize = ADDR_SIZE_FAR;
            break;

        default:
            break;
    }

    /* Zeropage is always zeropage */
    ZpAddrSize = ADDR_SIZE_ZP;
}
