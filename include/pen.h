/*****************************************************************************/
/*                                                                           */
/*                                   pen.h                                   */
/*                                                                           */
/*                               Lightpen API                                */
/*                                                                           */
/*                                                                           */
/* This software is provided "as-is", without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software.  If you use this software  */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated, but is not required.                                      */
/* 2. Altered source versions must be marked plainly as such; and, must not  */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/



#ifndef _PEN_H
#define _PEN_H



/*****************************************************************************/
/*                               Declarations                                */
/*****************************************************************************/



/* A program optionally can set this pointer to a function that gives
** a calibration value to a driver.  If this pointer isn't NULL,
** then a driver that wants a value can call that function.
** pen_adjuster must be set before the driver is installed.
*/
extern void __fastcall__ (*pen_adjuster) (unsigned char *pValue);



/*****************************************************************************/
/*                                 Functions                                 */
/*****************************************************************************/



void __fastcall__ pen_calibrate (unsigned char *XOffset);
/* Ask the user to help to calibrate a lightpen.  Changes the screen!
** A pointer to this function can be put into pen_adjuster.
*/

void __fastcall__ pen_adjust (const char *filename);
/* Get a lightpen calibration value from a file if it exists.  Otherwise, call
** pen_calibrate() to create a value; then, write it into a file, so that it
** will be available at the next time that the lightpen is used.
** Might change the screen.
** pen_adjust() is optional; if you want to use its feature,
** then it must be called before a driver is installed.
** Note:  This function merely saves the file-name pointer, and sets
** the pen_adjuster pointer.  The file will be read only when a driver
** is installed, and only if that driver wants to be calibrated.
*/



/* End of pen.h */
#endif



