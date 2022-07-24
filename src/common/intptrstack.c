/*****************************************************************************/
/*                                                                           */
/*                                intptrstack.c                              */
/*                                                                           */
/*                  Integer+ptr stack used for program settings              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2017, Mega Cat Studios                                                */
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
#include "check.h"
#include "intptrstack.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void IPS_Get (const IntPtrStack* S, long *Val, void **Ptr)
/* Get the value on top of an int stack */
{
    PRECONDITION (S->Count > 0);
    if (Val) *Val = S->Stack[S->Count-1].val;
    if (Ptr) *Ptr = S->Stack[S->Count-1].ptr;
}



void IPS_Set (IntPtrStack* S, long Val, void *Ptr)
/* Set the value on top of an int stack */
{
    PRECONDITION (S->Count > 0);
    S->Stack[S->Count-1].val = Val;
    S->Stack[S->Count-1].ptr = Ptr;
}



void IPS_Drop (IntPtrStack* S)
/* Drop a value from an int stack */
{
    PRECONDITION (S->Count > 0);
    --S->Count;
}



void IPS_Push (IntPtrStack* S, long Val, void *Ptr)
/* Push a value onto an int stack */
{
    PRECONDITION (S->Count < sizeof (S->Stack) / sizeof (S->Stack[0]));
    S->Stack[S->Count].val = Val;
    S->Stack[S->Count++].ptr = Ptr;
}



void IPS_Pop (IntPtrStack* S, long *Val, void **Ptr)
/* Pop a value from an int stack */
{
    PRECONDITION (S->Count > 0);
    if (Val) *Val = S->Stack[--S->Count].val;
    if (Ptr) *Ptr = S->Stack[S->Count].ptr;
}
