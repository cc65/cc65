/*****************************************************************************/
/*                                                                           */
/*                                 istack.c                                  */
/*                                                                           */
/*                        Input stack for the scanner                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2003 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
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
#include "xmalloc.h"

/* ca65 */
#include "error.h"
#include "istack.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Size of the stack (== maximum nested macro or repeat count) */
#define ISTACK_MAX      256

/* Structure holding a stack element */
typedef struct IElement IElement;
struct IElement {
    IElement*   Next;           /* Next stack element */
    int         (*Func)(void*); /* Function called for input */
    void*       Data;           /* User data given as argument */
    const char* Desc;           /* Description */
};

/* The stack */
static IElement* IStack = 0;    /* Input stack pointer */
static unsigned  ICount = 0;    /* Number of items on the stack */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void PushInput (int (*Func) (void*), void* Data, const char* Desc)
/* Push an input function onto the input stack */
{
    IElement* E;

    /* Check for a stack overflow */
    if (ICount > ISTACK_MAX) {
        Fatal ("Maximum input stack nesting exceeded");
    }                                                

    /* Create a new stack element */
    E = xmalloc (sizeof (*E));

    /* Initialize it */
    E->Func = Func;
    E->Data = Data;
    E->Desc = Desc;

    /* Push it */
    E->Next = IStack;
    IStack  = E;
}



void PopInput (void)
/* Pop the current input function from the input stack */
{
    IElement* E;

    /* We cannot pop from an empty stack */
    PRECONDITION (IStack != 0);

    /* Remember the last element */
    E = IStack;

    /* Pop it */
    IStack = IStack->Next;

    /* And delete it */
    xfree (E);
}



int InputFromStack (void)
/* Try to get input from the input stack. Return true if we had such input,
** return false otherwise.
*/
{
    /* Repeatedly call the TOS routine until we have a token or if run out of
    ** routines.
    */
    while (IStack) {
        if (IStack->Func (IStack->Data) != 0) {
            /* We have a token */
            return 1;
        }
    }

    /* Nothing is on the stack */
    return 0;
}



int HavePushedInput (void)
/* Return true if we have stacked input available, return false if not */
{
    return (IStack != 0);
}



void CheckInputStack (void)
/* Called from the scanner before closing an input file. Will check for any
** stuff on the input stack.
*/
{
    if (IStack) {
        Error ("Open %s", IStack->Desc);
    }
}
