/*****************************************************************************/
/*                                                                           */
/*                                callback.c                                 */
/*                                                                           */
/*                              Chip callbacks                               */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
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
#include "xmalloc.h"

/* sim65 */
#include "error.h"
#include "callback.h"



/*****************************************************************************/
/*  				     Data				     */
/*****************************************************************************/



struct Callback {
    Callback*           Next;           /* Next entry in list */
    unsigned            Ticks;          /* Remaining ticks */
    CallbackFunc        UserFunc;       /* User function */
    void*               UserData;       /* User data */
};

/* Delta list that keeps existing callbacks */
static Callback* List = 0;



/*****************************************************************************/
/*                    Routines that handle the delta list                    */
/*****************************************************************************/



static void InsertCallback (Callback* C, unsigned Ticks)
/* Insert the callback C into the delta list */
{
    /* Search for the insertion point */
    Callback*  N;
    Callback** L = &List;
    while ((N = *L) != 0) {
        /* Check if the next callback in the list has a higher wait time */
        if (N->Ticks > Ticks) {
            /* Insert before this callback */
            N->Ticks -= Ticks;
            break;
        } else {
            /* Insert behind this callback */
            Ticks -= N->Ticks;
            L = &N->Next;
        }
    }

    /* Insert the new task */
    C->Ticks = Ticks;
    C->Next  = N;
    *L       = C;
}



static void RemoveCallback (Callback* C)
/* Remove a callback from the list. If the callback is not in the list, this
 * is a fatal error.
 */
{
    Callback* N;
    Callback** L = &List;
    while ((N = *L) != 0) {
       	if (N == C) {
       	    /* Found, remove it */
       	    if (C->Next) {
	       	/* Adjust the counter of the following callback */
       	       	C->Next->Ticks += C->Ticks;
	    }
     	    *L = C->Next;
	    return;
     	} else {
     	    L = &N->Next;
     	}
    }

    /* Callback was not found */
    Internal ("RemoveCallback: Callback not found in list!");
}



/*****************************************************************************/
/*  	     	    	       	     Code				     */
/*****************************************************************************/



Callback* NewCallback (unsigned Ticks, CallbackFunc Func, void* Data)
/* Create a callback for function F to be called in Ticks ticks. */
{
    /* Allocate memory */
    Callback* C = xmalloc (sizeof (Callback));

    /* Initialize the fields */
    C->UserFunc = Func;
    C->UserData = Data;

    /* Insert the callback into the delta list */
    InsertCallback (C, Ticks);

    /* Return the new callback */
    return C;
}



void FreeCallback (Callback* C)
/* Delete a callback (remove from the queue) */
{
    /* Remove the callback from the list */
    RemoveCallback (C);

    /* Delete it */
    xfree (C);
}



void HandleCallbacks (unsigned TicksSinceLastCall)
/* Handle the callback queue */
{
    while (List) {

        /* Check if this one is due */
        if (List->Ticks <= TicksSinceLastCall) {

            /* Calclulate the tick offset */
            int TickOffs = ((int) List->Ticks) - ((int) TicksSinceLastCall);

            /* Retrieve the first callback from the list */
            Callback* C = List;
            List        = C->Next;

            /* Call the user function */
            C->UserFunc (TickOffs, C->UserData);

            /* Delete the callback */
            xfree (C);

        } else {

            List->Ticks -= TicksSinceLastCall;
            break;

        }
    }
}



