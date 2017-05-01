/*****************************************************************************/
/*                                                                           */
/*                                trampoline.c                               */
/*                                                                           */
/*                          Trampoline management                            */
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



#include <stdarg.h>
#include <string.h>

/* common */
#include "chartype.h"
#include "check.h"
#include "coll.h"
#include "scanner.h"
#include "intptrstack.h"
#include "xmalloc.h"

/* cc65 */
#include "codeent.h"
#include "error.h"
#include "trampoline.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/


/* Trampolines */
static IntPtrStack Trampolines;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void PushTrampoline (void *Ptr, unsigned char Val)
/* Push the current trampoline */
{
    if (IPS_IsFull (&Trampolines)) {
        Error ("Trampoline stack overflow");
    } else {
        IPS_Push (&Trampolines, Val, Ptr);
    }
}



void PopTrampoline (void)
/* Remove the current trampoline */
{
    if (IPS_GetCount (&Trampolines) < 1) {
        Error ("Trampoline stack is empty");
    } else {
        IPS_Drop (&Trampolines);
    }
}



void GetTrampoline (void **Ptr, unsigned char *Val)
/* Get the current trampoline */
{
    if (IPS_GetCount (&Trampolines) < 1) {
        *Ptr = NULL;
        *Val = 0;
    } else {
        long Temp;
        IPS_Get (&Trampolines, &Temp, Ptr);
        *Val = Temp;
    }
}
