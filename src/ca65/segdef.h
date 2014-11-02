/*****************************************************************************/
/*                                                                           */
/*                                 segdef.h                                  */
/*                                                                           */
/*              Segment definitions for the ca65 assembler                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2012, Ullrich von Bassewitz                                      */
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



#ifndef SEGDEF_H
#define SEGDEF_H



/* common */
#include "addrsize.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Segment definition */
typedef struct SegDef SegDef;
struct SegDef {
    char*         Name;         /* Segment name */
    unsigned char AddrSize;     /* Default address size */
};

/* Initializer for static SegDefs */
#define STATIC_SEGDEF_INITIALIZER(name, addrsize) { name, addrsize }



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



SegDef* NewSegDef (const char* Name, unsigned char AddrSize);
/* Create a new segment definition and return it */

void FreeSegDef (SegDef* D);
/* Free a segment definition */

SegDef* DupSegDef (const SegDef* D);
/* Duplicate a segment definition and return it */



/* End of segdef.h */

#endif
