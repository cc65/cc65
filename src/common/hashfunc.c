/*****************************************************************************/
/*                                                                           */
/*                                hashfunc.c                                 */
/*                                                                           */
/*                              Hash functions                               */
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
#include "hashfunc.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned HashInt (unsigned V)
/* Return a hash value for the given integer. The function uses Robert
** Jenkins' 32 bit integer hash function taken from
**     http://www.concentric.net/~ttwang/tech/inthash.htm
** For 16 bit integers, the function may be suboptimal.
*/
{
   V = (V + 0x7ed55d16) + (V << 12);
   V = (V ^ 0xc761c23c) ^ (V >> 19);
   V = (V + 0x165667b1) + (V << 5);
   V = (V + 0xd3a2646c) ^ (V << 9);
   V = (V + 0xfd7046c5) + (V << 3);
   V = (V ^ 0xb55a4f09) ^ (V >> 16);
   return V;
}



unsigned HashStr (const char* S)
/* Return a hash value for the given string */
{
    unsigned L, H;

    /* Do the hash */
    H = L = 0;
    while (*S) {
        H = ((H << 3) ^ ((unsigned char) *S++)) + L++;
    }
    return H;
}



unsigned HashBuf (const StrBuf* S)
/* Return a hash value for the given string buffer */
{
    unsigned I, L, H;

    /* Do the hash */
    H = L = 0;
    for (I = 0; I < SB_GetLen (S); ++I) {
        H = ((H << 3) ^ ((unsigned char) SB_AtUnchecked (S, I))) + L++;
    }
    return H;
}
