/*****************************************************************************/
/*                                                                           */
/*                                 stdbool.h                                 */
/*                                                                           */
/*                          C99 Boolean definitions                          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002 Greg King                                                        */
/*                                                                           */
/*                                                                           */
/* This software is provided "as-is," without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter and redistribute it       */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software.  If you use this software  */
/*    in a product, an acknowledgment, in the product's documentation,       */
/*    would be appreciated, but is not required.                             */
/* 2. Alterred source versions must be marked plainly as such,               */
/*    and must not be misrepresented as being the original software.         */
/* 3. This notice may not be removed or alterred                             */
/*    from any source distribution.                                          */
/*****************************************************************************/



#ifndef _STDBOOL_H
#define _STDBOOL_H



#define bool _Bool
typedef unsigned char _Bool;

/* Standard test-results. */
#define false 0
#define true  1

/* All three names are macroes. */
#define __bool_true_false_are_defined 1



/* End of stdbool.h */
#endif



