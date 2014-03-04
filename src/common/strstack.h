/*****************************************************************************/
/*                                                                           */
/*                                strstack.h                                 */
/*                                                                           */
/*                  String stack used for program settings                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2004      Ullrich von Bassewitz                                       */
/*               Römerstraße 52                                              */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
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



#ifndef STRSTACK_H
#define STRSTACK_H



#include "inline.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



typedef struct StrStack StrStack;
struct StrStack {
    unsigned    Count;
    char*       Stack[8];
};

/* Declare an empty string stack */
#define STRSTACK()      { 0 };



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



#if defined(HAVE_INLINE)
INLINE int SS_IsFull (const StrStack* S)
/* Return true if there is no space left on the given string stack */
{
    return (S->Count >= sizeof (S->Stack) / sizeof (S->Stack[0]));
}
#else
#  define SS_IsFull(S)  ((S)->Count >= sizeof ((S)->Stack) / sizeof ((S)->Stack[0]))
#endif

#if defined(HAVE_INLINE)
INLINE unsigned SS_GetCount (const StrStack* S)
/* Return the number of elements on the given string stack */
{
    return S->Count;
}
#else
#  define SS_GetCount(S)        (S)->Count
#endif

const char* SS_Get (const StrStack* S);
/* Get the value on top of a string stack */

void SS_Set (StrStack* S, const char* Val);
/* Set the value on top of a string stack */

void SS_Drop (StrStack* S);
/* Drop a value from a string stack */

void SS_Push (StrStack* S, const char* Val);
/* Push a value onto a string stack */



/* End of strstack.h */

#endif
