/*****************************************************************************/
/*                                                                           */
/*				    mouse.h				     */
/*                                                                           */
/*				   Mouse API				     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1999-2000 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



#ifndef _MOUSE_H
#define _MOUSE_H



/* Define __MOUSE__ for systems that support a mouse */
#ifdef __C64__
#  define __MOUSE__
#endif
#ifdef __C128__
#  define __MOUSE__
#endif
#ifdef __ATARI__
#  define __MOUSE__
#endif



/*****************************************************************************/
/* 	      			  Definitions	     			     */
/*****************************************************************************/



/* The different mouse types */
#define MOUSE_TRAKBALL 		0
#define MOUSE_ST		1
#define MOUSE_AMIGA		2
#define MOUSE_C64		3      /* 1351 mouse */

/* Mouse button masks */
#define MOUSE_BTN_LEFT	     0x10



/*****************************************************************************/
/* 	      			   Functions	     			     */
/*****************************************************************************/



unsigned char __fastcall__ mouse_init (unsigned char port, 
				       unsigned char sprite, 
				       unsigned char type);
/* Setup the mouse interrupt handler. If the sprite value is != zero, the
 * mouse routines will manage the sprite with this number. That means, it
 * is moved if the mouse is moved (provided that the mouse cursor is visible),
 * and switched on and off in the show and hide functions.
 * The port parameter gives the joystick port used for the mouse and is only
 * needed to read the mouse button state.
 * The type parameter is needed on some systems to determine the type of
 * the mouse connected to the given port.
 * After calling this function, the mouse is invisble, the cursor is placed
 * at 0/0 (upper left corner), and the bounding box is reset to cover the
 * whole screen. Call mouse_show once to make the mouse cursor visible.
 * The function will return zero if a mouse was not found and a non zero
 * value if the mouse was found and initialized (or if there is no way to
 * detect a mouse reliably).
 */

void mouse_done (void);
/* Disable the mouse, remove the interrupt handler. This function MUST be
 * called before terminating the program, otherwise odd things may happen.
 * If in doubt, install an exit handler (using atexit) that calls this
 * function.
 */

void mouse_hide (void);
/* Hide the mouse. This function doesn't do anything visible if no sprite is
 * used. The function manages a counter and may be called more than once.
 * For each call to mouse_hide there must be a call to mouse_show to make
 * the mouse visible again.
 */

void mouse_show (void);
/* Show the mouse. This function doesn't do anything visible if no sprite is
 * used. See mouse_hide for more information.
 */

void __fastcall__ mouse_box (int minx, int miny, int maxx, int maxy);
/* Set the bounding box for the mouse pointer movement. The mouse X and Y
 * coordinates will never go outside the given box.
 * NOTE: The function does *not* check if the mouse is currently inside the
 * given margins. The proper way to use this function therefore is:
 *
 * 	- Hide the mouse
 * 	- Set the bounding box
 * 	- Place the mouse at the desired position
 * 	- Show the mouse again.
 *
 * NOTE2: When setting the box to something that is larger than the actual
 * screen, the positioning of the mouse cursor will fail. If such margins
 * are really what you want, you have to use your own cursor routines.
 */

void __fastcall__ mouse_move (int x, int y);
/* Set the mouse cursor to the given position. If a mouse cursor is defined
 * and currently visible, the mouse cursor is also moved.
 * NOTE: This function does not check if the given position is valid and
 * inside the bounding box.
 */

unsigned char mouse_buttons (void);
/* Return a bit mask encoding the states of the mouse buttons. Use the
 * MOUSE_BTN_XXX flags to decode a specific button.
 */

void mouse_info (void);
/* Hmmm...
 */


/* End of mouse.h */
#endif

