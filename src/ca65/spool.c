/*****************************************************************************/
/*                                                                           */
/*                                  spool.c                                  */
/*                                                                           */
/*              Id and message pool for the ca65 macroassembler              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2011, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
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



/* ca65 */
#include "objfile.h"
#include "spool.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



StringPool* StrPool = 0;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void WriteStrPool (void)
/* Write the string pool to the object file */
{
    unsigned I;

    /* Get the number of strings in the string pool */
    unsigned Count = SP_GetCount (StrPool);

    /* Tell the object file module that we're about to start the string pool */
    ObjStartStrPool ();

    /* Write the string count to the list */
    ObjWriteVar (Count);

    /* Write the strings in id order */
    for (I = 0; I < Count; ++I) {
        ObjWriteBuf (SP_Get (StrPool, I));
    }

    /* Done writing the string pool */
    ObjEndStrPool ();
}



void InitStrPool (void)
/* Initialize the string pool */
{
    /* Create a string pool */
    StrPool = NewStringPool (1103);

    /* Insert an empty string. It will have string id 0 */
    SP_AddStr (StrPool, "");
}
