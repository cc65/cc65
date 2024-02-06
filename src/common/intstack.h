/*****************************************************************************/
/*                                                                           */
/*                                intstack.h                                 */
/*                                                                           */
/*                  Integer stack used for program settings                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2004-2010, Ullrich von Bassewitz                                      */
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



#ifndef INTSTACK_H
#define INTSTACK_H



#include "inline.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



typedef struct IntStack IntStack;
struct IntStack {
    unsigned    Count;
    long        Stack[8];
};

/* An initializer for an empty int stack */
#define STATIC_INTSTACK_INITIALIZER     { 0, { 0, 0, 0, 0, 0, 0, 0, 0 } }

/* Declare an int stack with the given value as first element */
#define INTSTACK(Val)   { 1, { Val, 0, 0, 0, 0, 0, 0, 0 } }



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



#if defined(HAVE_INLINE)
INLINE int IS_IsFull (const IntStack* S)
/* Return true if there is no space left on the given int stack */
{
    return (S->Count >= sizeof (S->Stack) / sizeof (S->Stack[0]));
}
#else
#  define IS_IsFull(S)  ((S)->Count >= sizeof ((S)->Stack) / sizeof ((S)->Stack[0]))
#endif

#if defined(HAVE_INLINE)
INLINE int IS_IsEmpty (const IntStack* S)
/* Return true if there are no values on the given int stack */
{
    return (S->Count == 0);
}
#else
#  define IS_IsEmpty(S)  ((S)->Count == 0)
#endif

#if defined(HAVE_INLINE)
INLINE unsigned IS_GetCount (const IntStack* S)
/* Return the number of elements on the given int stack */
{
    return S->Count;
}
#else
#  define IS_GetCount(S)        (S)->Count
#endif

long IS_Get (const IntStack* S);
/* Get the value on top of an int stack */

void IS_Set (IntStack* S, long Val);
/* Set the value on top of an int stack */

void IS_Drop (IntStack* S);
/* Drop a value from an int stack */

void IS_Push (IntStack* S, long Val);
/* Push a value onto an int stack */

long IS_Pop (IntStack* S);
/* Pop a value from an int stack */



/* End of intstack.h */

#endif
