/*****************************************************************************/
/*                                                                           */
/*                                 mmodel.h                                  */
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



#ifndef MMODEL_H
#define MMODEL_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Memory models */
typedef enum {
    MMODEL_UNKNOWN  = -1,
    MMODEL_NEAR,                /* Code: near, data: near */
    MMODEL_FAR,                 /* Code: far, data: near */
    MMODEL_HUGE,                /* Code: far, data: far */
    MMODEL_COUNT                /* Number of memory models */
} mmodel_t;

/* Memory model in use */
extern mmodel_t MemoryModel;

/* Address sizes for the segments */
extern unsigned char CodeAddrSize;
extern unsigned char DataAddrSize;
extern unsigned char ZpAddrSize;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



mmodel_t FindMemoryModel (const char* Name);
/* Find a memory model by name. Return MMODEL_UNKNOWN for an unknown name. */

void SetMemoryModel (mmodel_t Model);
/* Set the memory model updating the MemoryModel variables and the address
** sizes for the segments.
*/



/* End of mmodel.h */

#endif
