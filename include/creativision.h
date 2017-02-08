/* CreatiVision Header */

#ifndef _CVISION_H

#define _CVISION_H

#define CH_VLINE 33
#define CH_HLINE 34
#define CH_ULCORNER 35
#define CH_URCORNER 36
#define CH_LLCORNER 37
#define CH_LRCORNER 38

#define DYN_DRV 0

/* Colours - from TMS9918 */
#define C_TRANSPARENT   0
#define C_BLACK         1
#define C_MED_GREEN     2 
#define C_LIGHT_GREEN   3
#define C_DARK_BLUE     4
#define C_LIGHT_BLUE    5
#define C_DARK_RED      6
#define C_CYAN          7
#define C_MED_RED       8
#define C_LIGHT_RED     9
#define C_DARK_YELLOW   10
#define C_LIGHT_YELLOW  11
#define C_DARK_GREEN    12
#define C_MAGENTA       13
#define C_GREY          14
#define C_WHITE         15

/* Joystick states */
#define JOY_UP          5
#define JOY_DOWN        1
#define JOY_LEFT        7
#define JOY_RIGHT       3
#define JOY_LEFT_UP     6
#define JOY_LEFT_DOWN   8
#define JOY_RIGHT_UP    4
#define JOY_RIGHT_DOWN  2
#define JOY_LBUTTON     1
#define JOY_RBUTTON     2

/* Joystick values */
#define JOY_LEFT_DIR      1
#define JOY_RIGHT_DIR     2
#define JOY_LEFT_BUTTONS  3
#define JOY_RIGHT_BUTTONS 4                     

/* Protos */
void __fastcall__ psg_outb( unsigned char b );
void __fastcall__ psg_delay( unsigned char b );
void psg_silence( void );
void __fastcall__ bios_playsound( void *a, unsigned char b);
unsigned char __fastcall__ joystate( unsigned char which );

#endif
