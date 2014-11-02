/*****************************************************************************/
/*                                                                           */
/*                                 filepos.c                                 */
/*                                                                           */
/*                       File position data structure                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2011, Ullrich von Bassewitz                                      */
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
#include "filepos.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void InitFilePos (FilePos* P)
/* Initialize the file position (set all fields to zero) */
{
    P->Line = 0;
    P->Col  = 0;
    P->Name = 0;
}



int CompareFilePos (const FilePos* P1, const FilePos* P2)
/* Compare two file positions. Return zero if both are equal, return a value
** > 0 if P1 is greater and P2, and a value < 0 if P1 is less than P2. The
** compare rates file index over line over column.
*/
{
    if (P1->Name > P2->Name) {    
        return 1;
    } else if (P1->Name < P2->Name) {
        return -1;
    } else if (P1->Line > P2->Line) {
        return 1;
    } else if (P1->Line < P2->Line) {
        return -1;
    } else if (P1->Col > P2->Col) {
        return 1;
    } else if (P1->Col < P2->Col) {
        return -1;
    } else {
        return 0;
    }
}
