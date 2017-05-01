/*****************************************************************************/
/*                                                                           */
/*                                trampoline.h                               */
/*                                                                           */
/*                            Trampoline management                          */
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



#ifndef TRAMPOLINE_H
#define TRAMPOLINE_H



#include <stdio.h>

/* common */
#include "attrib.h"

/* cc65 */
#include "opcodes.h"


/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void PushTrampoline (void *Ptr, unsigned char Val);
/* Push the current trampoline */

void PopTrampoline (void);
/* Pop the current trampoline */

void GetTrampoline (void **Ptr, unsigned char *Val);
/* Get the current trampoline, if any */


/* End of trampoline.h */

#endif
