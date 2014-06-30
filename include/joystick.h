/*****************************************************************************/
/*                                                                           */
/*                                joystick.h                                 */
/*                                                                           */
/*               Read the joystick on systems that support it                */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2011, Ullrich von Bassewitz                                      */
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



#ifndef _JOYSTICK_H
#define _JOYSTICK_H



/*****************************************************************************/
/*                                  Definitions                              */
/*****************************************************************************/



/* Error codes */
#define JOY_ERR_OK              0       /* No error */
#define JOY_ERR_NO_DRIVER       1       /* No driver available */
#define JOY_ERR_CANNOT_LOAD     2       /* Error loading driver */
#define JOY_ERR_INV_DRIVER      3       /* Invalid driver */
#define JOY_ERR_NO_DEVICE       4       /* Device (hardware) not found */

/* Argument for the joy_read function */
#define JOY_1           0
#define JOY_2           1

/* The following codes are *indices* into the joy_masks array */
#define JOY_UP          0
#define JOY_DOWN        1
#define JOY_LEFT        2
#define JOY_RIGHT       3
#define JOY_FIRE        4
#define JOY_FIRE2       5               /* Second fire button if available */

/* Array of masks used to check the return value of joy_read for a state */
extern const unsigned char joy_masks[8];

/* Macros that evaluate the return code of joy_read */
#define JOY_BTN_UP(v)           ((v) & joy_masks[JOY_UP])
#define JOY_BTN_DOWN(v)         ((v) & joy_masks[JOY_DOWN])
#define JOY_BTN_LEFT(v)         ((v) & joy_masks[JOY_LEFT])
#define JOY_BTN_RIGHT(v)        ((v) & joy_masks[JOY_RIGHT])
#define JOY_BTN_FIRE(v)         ((v) & joy_masks[JOY_FIRE])
#define JOY_BTN_FIRE2(v)        ((v) & joy_masks[JOY_FIRE2])

/* The name of the standard joystick driver for a platform */
extern const char joy_stddrv[];

/* The address of the static standard joystick driver for a platform */
extern const void joy_static_stddrv[];



/*****************************************************************************/
/*                                 Functions                                 */
/*****************************************************************************/



unsigned char __fastcall__ joy_load_driver (const char* driver);
/* Load and install a joystick driver. Return an error code. */

unsigned char joy_unload (void);
/* Uninstall, then unload the currently loaded driver. */

unsigned char __fastcall__ joy_install (void* driver);
/* Install an already loaded driver. Return an error code. */

unsigned char joy_uninstall (void);
/* Uninstall the currently loaded driver and return an error code.
** Note: This call does not free allocated memory.
*/

unsigned char joy_count (void);
/* Return the number of joysticks supported by the driver */

unsigned char __fastcall__ joy_read (unsigned char joystick);
/* Read a particular joystick */



/* End of joystick.h */
#endif



