/*****************************************************************************/
/*                                                                           */
/*                                intptrstack.h                              */
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



#ifndef INTPTRSTACK_H
#define INTPTRSTACK_H



#include "inline.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



typedef struct IntPtrStack IntPtrStack;
struct IntPtrInner {
        long val;
        void *ptr;
};
struct IntPtrStack {
    unsigned    Count;
    struct IntPtrInner Stack[8];
};

/* An initializer for an empty int stack */
#define STATIC_INTPTRSTACK_INITIALIZER     { 0, { 0, 0 }, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} } }

/* Declare an int stack with the given value as first element */
#define INTPTRSTACK(Val, Ptr)   { 1, { {Val, Ptr}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} } }



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



#if defined(HAVE_INLINE)
INLINE int IPS_IsFull (const IntPtrStack* S)
/* Return true if there is no space left on the given int stack */
{
    return (S->Count >= sizeof (S->Stack) / sizeof (S->Stack[0]));
}
#else
#  define IPS_IsFull(S)  ((S)->Count >= sizeof ((S)->Stack) / sizeof ((S)->Stack[0]))
#endif

#if defined(HAVE_INLINE)
INLINE int IPS_IsEmpty (const IntPtrStack* S)
/* Return true if there are no values on the given int stack */
{
    return (S->Count == 0);
}
#else
#  define IPS_IsEmpty(S)  ((S)->Count == 0)
#endif

#if defined(HAVE_INLINE)
INLINE unsigned IPS_GetCount (const IntPtrStack* S)
/* Return the number of elements on the given int stack */
{
    return S->Count;
}
#else
#  define IPS_GetCount(S)        (S)->Count
#endif

void IPS_Get (const IntPtrStack* S, long *Val, void **Ptr);
/* Get the value on top of an int stack */

void IPS_Set (IntPtrStack* S, long Val, void *Ptr);
/* Set the value on top of an int stack */

void IPS_Drop (IntPtrStack* S);
/* Drop a value from an int stack */

void IPS_Push (IntPtrStack* S, long Val, void *Ptr);
/* Push a value onto an int stack */

void IPS_Pop (IntPtrStack* S, long *Val, void **Ptr);
/* Pop a value from an int stack */



/* End of IntPtrStack.h */

#endif
