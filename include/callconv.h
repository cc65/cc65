/*****************************************************************************/
/*                                                                           */
/*                             callconv.h                                    */
/*                                                                           */
/*                 Calling convertor, register call to fastcall              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (c) Christian Krüger, latest change: 22-Jul-2013                          */
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

#ifndef _CALLCONV_H
#define _CALLCONV_H

#include <stddef.h>

typedef void __fastcall__ (* RegCallFuncRetV)(void);
typedef int __fastcall__ (* RegCallFuncRetN)(void);
typedef size_t __fastcall__ (* RegCallFuncRetSt)(void);
typedef void* __fastcall__ (* RegCallFuncRetPv)(void);
typedef char* __fastcall__ (* RegCallFuncRetPc)(void);

int __fastcall__
RegCallPcPcRetN(RegCallFuncRetN func, char* param1, char* param2);

size_t __fastcall__
RegCallPcPcRetSt(RegCallFuncRetSt func, char* param1, char* param2);

void __fastcall__
RegCallPvPvRetV(RegCallFuncRetPv func, void* param1, void* param2);

void __fastcall__
RegCallPvStRetV(RegCallFuncRetV func, void* param1, size_t param2);

void* __fastcall__
RegCallPvPvRetPv(RegCallFuncRetPv func, void* param1, void* param2);

void* __fastcall__
RegCallPvStRetPv(RegCallFuncRetPv func, void* param1, size_t param2);

char* __fastcall__
RegCallPcPcRetPc(RegCallFuncRetPc func, char* param1, char* param2);

char* __fastcall__
RegCallPcNRetPc(RegCallFuncRetPc func, char* param1, int param2);

void* __fastcall__
RegCallPvNStRetPv(RegCallFuncRetPv func, void* param1, int param2, size_t param3);

void* __fastcall__
RegCallPvStNRetPv(RegCallFuncRetPv func, void* param1, size_t param2, int param3);

int __fastcall__
RegCallPvPvStRetN(RegCallFuncRetN func, void* param1, void* param2, size_t param3);

void* __fastcall__
RegCallPvPvStRetPv(RegCallFuncRetPv func, void* param1, void* param2, size_t param3);

int __fastcall__
RegCallPcPcStRetN(RegCallFuncRetN func, char* param1, char* param2, size_t param3);

size_t __fastcall__
RegCallPcPcStRetSt(RegCallFuncRetSt func, char* param1, char* param2, size_t param3);

char* __fastcall__
RegCallPcPcStRetPc(RegCallFuncRetPc func, char* param1, char* param2, size_t param3);

/* End of callconv.h */
#endif



