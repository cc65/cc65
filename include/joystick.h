/*
 * joystick.h
 *
 * Ullrich von Bassewitz, 24.09.1998
 *
 * Read the joystick on systems that support it.
 *
 */



#ifndef _JOYSTICK_H
#define _JOYSTICK_H



/* Define __JOYSTICK__ for systems that support a joystick */
#ifdef __C64__
#  define __JOYSTICK__
#endif
#ifdef __C128__
#  define __JOYSTICK__
#endif
#ifdef __PLUS4__
#  define __JOYSTICK__
#endif
#ifdef __NES__
#  define __JOYSTICK__
#endif

/* Argument for the function */
#define JOY_1		0
#define JOY_2		1

/* Result codes of the function. The actual code is a bitwise or
 * of one or more of the following values.
 */
#ifdef __NES__
#  define JOY_A		0x01
#  define JOY_B		0x02
#  define JOY_SELECT	0x04
#  define JOY_START	0x08
#  define JOY_UP	0x10
#  define JOY_DOWN	0x20
#  define JOY_LEFT	0x40
#  define JOY_RIGHT	0x80
#else
#  define JOY_UP       	0x01
#  define JOY_DOWN	0x02
#  define JOY_LEFT	0x04
#  define JOY_RIGHT	0x08
#  define JOY_FIRE	0x10
#endif



unsigned __fastcall__ readjoy (unsigned char joy);
/* Read the joystick. The argument is one of JOY_1/JOY2 */



/* End of joystick.h */
#endif



