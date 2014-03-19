/*****************************************************************************/
/*                                                                           */
/*                                strstack.c                                 */
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


                    
/* common */
#include "check.h"
#include "strstack.h"
#include "xmalloc.h"
            


/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



const char* SS_Get (const StrStack* S)
/* Get the value on top of a string stack */
{
    CHECK (S->Count > 0);
    return S->Stack[S->Count-1];
}



void SS_Set (StrStack* S, const char* Val)
/* Set the value on top of a string stack */
{
    CHECK (S->Count > 0);
    xfree (S->Stack[S->Count-1]);
    S->Stack[S->Count-1] = xstrdup (Val);
}



void SS_Drop (StrStack* S)
/* Drop a value from a string stack */
{
    CHECK (S->Count > 1);
    xfree (S->Stack[--S->Count]);
}



void SS_Push (StrStack* S, const char* Val)
/* Push a value onto a string stack */
{
    CHECK (S->Count < sizeof (S->Stack) / sizeof (S->Stack[0]));
    S->Stack[S->Count++] = xstrdup (Val);
}
