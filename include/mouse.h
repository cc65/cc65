/*****************************************************************************/
/*                                                                           */
/*				    mouse.h				     */
/*                                                                           */
/*				   Mouse API				     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2009, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
/*                                                                           */
/*                                                                           */
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



/*****************************************************************************/
/* 	      			  Definitions	     			     */
/*****************************************************************************/



/* Error codes */
#define MOUSE_ERR_OK            0       /* No error */
#define MOUSE_ERR_NO_DRIVER     1       /* No driver available */
#define MOUSE_ERR_CANNOT_LOAD   2       /* Error loading driver */
#define MOUSE_ERR_INV_DRIVER    3       /* Invalid driver */
#define MOUSE_ERR_NO_DEVICE     4       /* Mouse hardware not found */
#define MOUSE_ERR_INV_IOCTL     5       /* Invalid ioctl code */

/* Mouse button masks */
#define MOUSE_BTN_LEFT	     0x10
#define MOUSE_BTN_RIGHT      0x01

/* Structure containing the mouse coordinates */
struct mouse_pos {
    int 		x;
    int 		y;
};

/* Structure containing information about the mouse */
struct mouse_info {
    struct mouse_pos    pos;           /* Mouse position */
    unsigned char       buttons;       /* Mouse button mask */
};

/* Structure used for getbox/setbox */
struct mouse_box {
    int         	minx;
    int         	miny;
    int         	maxx;
    int         	maxy;
};

/* Structure containing mouse callback functions. These functions are declared
 * in C notation here, but they cannot be C functions (at least not easily),
 * since they may be called from within an interrupt.
 */
struct mouse_callbacks {

    void (*hide) (void);
    /* Hide the mouse cursor. */

    void (*show) (void);
    /* Show the mouse cursor */

    void __fastcall__ (*movex) (int x);
    /* Move the mouse cursor to the new X coordinate. This function is called,
     * even when the cursor is currently invisible.
     */

    void __fastcall__ (*movey) (int y);
    /* Move the mouse cursor to the new Y coordinate. This function is called,
     * even when the cursor is currently invisible.
     */
};

/* The default mouse callbacks */
extern const struct mouse_callbacks mouse_def_callbacks;

/* The name of the standard mouse driver for a platform */
extern const char mouse_stddrv[];



/*****************************************************************************/
/* 	      			   Functions	     			     */
/*****************************************************************************/



unsigned char __fastcall__ mouse_load_driver (const struct mouse_callbacks* c,
                                              const char* driver);
/* Load and install a mouse driver, return an error code. */

unsigned char __fastcall__ mouse_unload (void);
/* Uninstall, then unload the currently loaded driver. */

unsigned char __fastcall__ mouse_install (const struct mouse_callbacks* c,
                                          void* driver);
/* Install an already loaded driver. Returns an error code. */

unsigned char __fastcall__ mouse_uninstall (void);
/* Uninstall the currently loaded driver. Returns an error code. */

const char* __fastcall__ mouse_geterrormsg (unsigned char code);
/* Get an error message describing the error in code. */

void __fastcall__ mouse_hide (void);
/* Hide the mouse. The function manages a counter and may be called more than
 * once. For each call to mouse_hide there must be a call to mouse_show to make
 * the mouse visible again.
 */

void __fastcall__ mouse_show (void);
/* Show the mouse. See mouse_hide for more information. */

void __fastcall__ mouse_setbox (const struct mouse_box* box);
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
 * screen, the positioning of the mouse cursor can fail. If such margins
 * are really what you want, you have to use your own cursor routines.
 */

void __fastcall__ mouse_getbox (struct mouse_box* box);
/* Get the current bounding box for the mouse pointer movement. */

void __fastcall__ mouse_move (int x, int y);
/* Set the mouse cursor to the given position. If a mouse cursor is defined
 * and currently visible, the mouse cursor is also moved.
 * NOTE: This function does not check if the given position is valid and
 * inside the bounding box.
 */

unsigned char __fastcall__ mouse_buttons (void);
/* Return a bit mask encoding the states of the mouse buttons. Use the
 * MOUSE_BTN_XXX flags to decode a specific button.
 */

void __fastcall__ mouse_pos (struct mouse_pos* pos);
/* Return the current mouse position */

void __fastcall__ mouse_info (struct mouse_info* info);
/* Return the state of the mouse buttons and the position of the mouse */

unsigned char __fastcall__ mouse_ioctl (unsigned char code, void* data);
/* Call the driver specific ioctl function. NON PORTABLE! Returns an error
 * code.
 */



/* End of mouse.h */
#endif



