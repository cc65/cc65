/*****************************************************************************/
/*                                                                           */
/*                                wrappedcall.h                              */
/*                                                                           */
/*                            Wrapped-call management                        */
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



#ifndef WRAPPEDCALL_H
#define WRAPPEDCALL_H



#include <stdio.h>

/* common */
#include "attrib.h"

/* cc65 */
#include "opcodes.h"


/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void PushWrappedCall (void *Ptr, unsigned char Val);
/* Push the current WrappedCall */

void PopWrappedCall (void);
/* Pop the current WrappedCall */

void GetWrappedCall (void **Ptr, unsigned char *Val);
/* Get the current WrappedCall, if any */


/* End of wrappedcall.h */

#endif
