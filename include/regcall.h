/*****************************************************************************/
/*                                                                           */
/*                                regcall.h                                  */
/*                                                                           */
/*           Imports to tie C macros to 'registers' for calling              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (c) Christian Krüger, latest change: 03-Jul-2013                          */
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

#ifndef _REGCALL_H
#define _REGCALL_H

#include <stddef.h>

typedef union
{
    void* pV;           // declare all possible data types here
    char* pC;           // to keep compiler calm and ease use
    int n;
    size_t st;
}
Register;

/* import of zeropage symbols */

extern Register R0;
#pragma zpsym ("R0");

extern Register R1;
#pragma zpsym ("R1");

extern Register R2;
#pragma zpsym ("R2");

extern Register R3;
#pragma zpsym ("R3");

extern Register R4;
#pragma zpsym ("R4");

/* End of regcall.h */

#endif



