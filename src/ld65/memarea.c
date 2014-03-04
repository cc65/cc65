/*****************************************************************************/
/*                                                                           */
/*                                 memarea.c                                 */
/*                                                                           */
/*                Memory area definition for the ld65 linker                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2010-2012, Ullrich von Bassewitz                                      */
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
#include "xmalloc.h"

/* ld65 */
#include "memarea.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



MemoryArea* NewMemoryArea (const FilePos* Pos, unsigned Name)
/* Create a new memory area and insert it into the list */
{
    /* Allocate memory */
    MemoryArea* M = xmalloc (sizeof (MemoryArea));

    /* Initialize the fields ... */
    M->LI          = GenLineInfo (Pos);
    M->Name        = Name;
    M->Attr        = 0;
    M->Flags       = 0;
    M->FileOffs    = ~0UL;
    M->StartExpr   = 0;
    M->Start       = 0;
    M->SizeExpr    = 0;
    M->Size        = 0;
    M->BankExpr    = 0;
    M->FillLevel   = 0;
    M->FillVal     = 0;
    M->Relocatable = 0;
    M->SegList     = EmptyCollection;
    M->F           = 0;

    /* ...and return it */
    return M;
}
