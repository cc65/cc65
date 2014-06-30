/*****************************************************************************/
/*                                                                           */
/*                                   fp.h                                    */
/*                                                                           */
/*                          Floating point support                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2008      Ullrich von Bassewitz                                       */
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



/* The compiler must use the same floating point arithmetic as the target
** platform, otherwise expressions will yield a different result when
** evaluated in the compiler or on the target platform. Since writing a target
** and source library is almost double the work, we will at least add the
** hooks here, and define functions for a plug in library that may be added
** at a later time. Currently we use the builtin data types of the compiler
** that translates cc65.
**
** BEWARE: This code will currently only work on little endian systems!
*/



#ifndef FP_H
#define FP_H



#include <stddef.h>



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/


typedef union Float Float;
union Float {
    float         V;
};



typedef union Double Double;
union Double {
    double        V;
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



size_t FP_F_Size (void);
/* Return the size of the data type float */

unsigned char* FP_F_Data (Float Val);
/* Return the raw data of a float in a malloc'ed buffer. Free after use. */

Float FP_F_Make (float Val);
/* Make a floating point variable from a float value */

Float FP_F_FromInt (long Val);
/* Convert an integer into a floating point variable */

float FP_F_ToFloat (Float Val);
/* Convert a Float into a native float */

Float FP_F_Add (Float Left, Float Right);
/* Add two floats */

Float FP_F_Sub (Float Left, Float Right);
/* Subtract two floats */

Float FP_F_Mul (Float Left, Float Right);
/* Multiplicate two floats */

Float FP_F_Div (Float Left, Float Right);
/* Divide two floats */

size_t FP_D_Size (void);
/* Return the size of the data type double */

unsigned char* FP_D_Data (Double Val);
/* Return the raw data of a double in a malloc'ed buffer. Free after use. */

Double FP_D_Make (double Val);
/* Make a floating point variable from a float value */

Double FP_D_FromInt (long Val);
/* Convert an integer into a floating point variable */

double FP_D_ToFloat (Double Val);
/* Convert a Double into a native double */

Double FP_D_Add (Double Left, Double Right);
/* Add two floats */

Double FP_D_Sub (Double Left, Double Right);
/* Subtract two floats */

Double FP_D_Mul (Double Left, Double Right);
/* Multiplicate two floats */

Double FP_D_Div (Double Left, Double Right);
/* Divide two floats */



/* End of fp.h */

#endif
