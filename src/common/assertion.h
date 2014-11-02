/*****************************************************************************/
/*                                                                           */
/*                                assertion.h                                */
/*                                                                           */
/*                     Definitions for linker assertions                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2009, Ullrich von Bassewitz                                      */
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



#ifndef ASSERTION_H
#define ASSERTION_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Assertion actions. The second two are linker only (not evaluated by ca65) */
typedef enum {
    ASSERT_ACT_WARN     =   0x00U,  /* Print a warning */
    ASSERT_ACT_ERROR    =   0x01U,  /* Print an error */
    ASSERT_ACT_LDWARN   =   0x02U,  /* Print a warning (linker only) */
    ASSERT_ACT_LDERROR  =   0x03U,  /* Print an error (linker only) */
} AssertAction;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int AssertAtLinkTime (AssertAction A);
/* Return true if this assertion should be evaluated at link time */

int AssertAtAsmTime (AssertAction A);
/* Return true if this assertion should be evaluated at assembly time */



/* End of assertion.h */

#endif
