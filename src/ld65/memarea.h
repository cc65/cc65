/*****************************************************************************/
/*                                                                           */
/*                                 memarea.h                                 */
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



#ifndef MEMAREA_H
#define MEMAREA_H



/* common */
#include "coll.h"

/* ld65 */
#include "lineinfo.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Forwards for structures */
struct ExprNode;
struct File;

/* Memory area entry */
typedef struct MemoryArea MemoryArea;
struct MemoryArea {
    LineInfo*           LI;             /* Where was the area was defined? */
    unsigned            Name;           /* Name index of the memory area */
    unsigned            Attr;           /* Which values are valid? */
    unsigned            Flags;          /* Set of bitmapped flags */
    unsigned long       FileOffs;       /* Offset in output file */
    struct ExprNode*    StartExpr;      /* Expression for start address */
    unsigned long       Start;          /* Start address */
    struct ExprNode*    SizeExpr;       /* Expression for size */
    unsigned long       Size;           /* Length of memory section */
    struct ExprNode*    BankExpr;       /* Expression for bank */
    unsigned long       FillLevel;      /* Actual fill level of segment */
    unsigned char       FillVal;        /* Value used to fill rest of seg */
    unsigned char       Relocatable;    /* Memory area is relocatable */
    Collection          SegList;        /* List of segments for this area */
    struct File*        F;              /* Output file for this area */
};

/* Memory flags */
#define MF_DEFINE       0x0001          /* Define start and size */
#define MF_FILL         0x0002          /* Fill segment */
#define MF_RO           0x0004          /* Read only memory area */
#define MF_OVERFLOW     0x0008          /* Memory area overflow */
#define MF_PLACED       0x0010          /* Memory area was placed */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



MemoryArea* NewMemoryArea (const FilePos* Pos, unsigned Name);
/* Create a new memory area and insert it into the list */



/* End of memarea.h */

#endif
