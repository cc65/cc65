/*****************************************************************************/
/*                                                                           */
/*                                  tpool.c                                  */
/*                                                                           */
/*                          Pool for generic types                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2011,      Ullrich von Bassewitz                                      */
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



/* common */
#include "gentype.h"

/* ld65 */
#include "tpool.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* The string pool we're using */
StringPool* TypePool = 0;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void PrintDbgTypes (FILE* F)
/* Output the types to a debug info file */
{
    StrBuf Type = STATIC_STRBUF_INITIALIZER;

    /* Get the number of strings in the type pool */
    unsigned Count = SP_GetCount (TypePool);

    /* Output all of them */
    unsigned Id;
    for (Id = 0; Id < Count; ++Id) {

        /* Output it */
        fprintf (F, "type\tid=%u,val=\"%s\"\n", Id,
                 GT_AsString (SP_Get (TypePool, Id), &Type));

    }

    /* Free the memory for the temporary string */
    SB_Done (&Type);
}



void InitTypePool (void)
/* Initialize the type pool */
{
    /* Allocate a type pool */
    TypePool = NewStringPool (137);
}
