/*****************************************************************************/
/*                                                                           */
/*                                 _pokey.h                                  */
/*                                                                           */
/*                Internal include file, do not use directly                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000 Freddy Offenga <taf_offenga@yahoo.com>                           */
/* 06-Nov-2018: Christian Krueger: Added defines for keyboard codes          */
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



#ifndef __POKEY_H
#define __POKEY_H



/* Define a structure with the pokey register offsets */
struct __pokey_write {
    unsigned char   audf1;  /* audio channel #1 frequency */
    unsigned char   audc1;  /* audio channel #1 control */
    unsigned char   audf2;  /* audio channel #2 frequency */
    unsigned char   audc2;  /* audio channel #2 control */
    unsigned char   audf3;  /* audio channel #3 frequency */
    unsigned char   audc3;  /* audio channel #3 control */
    unsigned char   audf4;  /* audio channel #4 frequency */
    unsigned char   audc4;  /* audio channel #4 control */
    unsigned char   audctl; /* audio control */
    unsigned char   stimer; /* start pokey timers */
    unsigned char   skrest; /* reset serial port status reg. */
    unsigned char   potgo;  /* start paddle scan sequence */
    unsigned char   unuse1; /* unused */
    unsigned char   serout; /* serial port data output */
    unsigned char   irqen;  /* interrupt request enable */
    unsigned char   skctl;  /* serial port control */
};
struct __pokey_read {
    unsigned char   pot0;   /* paddle 0 value */
    unsigned char   pot1;   /* paddle 1 value */
    unsigned char   pot2;   /* paddle 2 value */
    unsigned char   pot3;   /* paddle 3 value */
    unsigned char   pot4;   /* paddle 4 value */
    unsigned char   pot5;   /* paddle 5 value */
    unsigned char   pot6;   /* paddle 6 value */
    unsigned char   pot7;   /* paddle 7 value */
    unsigned char   allpot; /* eight paddle port status */
    unsigned char   kbcode; /* keyboard code */
    unsigned char   random; /* random number generator */
    unsigned char   unuse2; /* unused */
    unsigned char   unuse3; /* unused */
    unsigned char   serin;  /* serial port input */
    unsigned char   irqst;  /* interrupt request status */
    unsigned char   skstat; /* serial port status */
};


/* Keyboard values returned by kbcode */

#define KEY_NONE        (unsigned char) 0xFF

#define KEY_0           (unsigned char) 0x32
#define KEY_1           (unsigned char) 0x1F
#define KEY_2           (unsigned char) 0x1E
#define KEY_3           (unsigned char) 0x1A
#define KEY_4           (unsigned char) 0x18
#define KEY_5           (unsigned char) 0x1D
#define KEY_6           (unsigned char) 0x1B
#define KEY_7           (unsigned char) 0x33
#define KEY_8           (unsigned char) 0x35
#define KEY_9           (unsigned char) 0x30

#define KEY_A           (unsigned char) 0x3F
#define KEY_B           (unsigned char) 0x15
#define KEY_C           (unsigned char) 0x12
#define KEY_D           (unsigned char) 0x3A
#define KEY_E           (unsigned char) 0x2A
#define KEY_F           (unsigned char) 0x38
#define KEY_G           (unsigned char) 0x3D
#define KEY_H           (unsigned char) 0x39
#define KEY_I           (unsigned char) 0x0D
#define KEY_J           (unsigned char) 0x01
#define KEY_K           (unsigned char) 0x05
#define KEY_L           (unsigned char) 0x00
#define KEY_M           (unsigned char) 0x25
#define KEY_N           (unsigned char) 0x23
#define KEY_O           (unsigned char) 0x08
#define KEY_P           (unsigned char) 0x0A
#define KEY_Q           (unsigned char) 0x2F
#define KEY_R           (unsigned char) 0x28
#define KEY_S           (unsigned char) 0x3E
#define KEY_T           (unsigned char) 0x2D
#define KEY_U           (unsigned char) 0x0B
#define KEY_V           (unsigned char) 0x10
#define KEY_W           (unsigned char) 0x2E
#define KEY_X           (unsigned char) 0x16
#define KEY_Y           (unsigned char) 0x2B
#define KEY_Z           (unsigned char) 0x17

#define KEY_COMMA       (unsigned char) 0x20
#define KEY_PERIOD      (unsigned char) 0x22
#define KEY_SLASH       (unsigned char) 0x26
#define KEY_SEMICOLON   (unsigned char) 0x02
#define KEY_PLUS        (unsigned char) 0x06
#define KEY_ASTERISK    (unsigned char) 0x07
#define KEY_DASH        (unsigned char) 0x0E
#define KEY_EQUALS      (unsigned char) 0x0F
#define KEY_LESSTHAN    (unsigned char) 0x36
#define KEY_GREATERTHAN (unsigned char) 0x37

#define KEY_ESC         (unsigned char) 0x1C
#define KEY_TAB         (unsigned char) 0x2C
#define KEY_SPACE       (unsigned char) 0x21
#define KEY_RETURN      (unsigned char) 0x0C
#define KEY_DELETE      (unsigned char) 0x34
#define KEY_CAPS        (unsigned char) 0x3C
#define KEY_INVERSE     (unsigned char) 0x27
#define KEY_HELP        (unsigned char) 0x11

#define KEY_F1          (unsigned char) 0x03
#define KEY_F2          (unsigned char) 0x04
#define KEY_F3          (unsigned char) 0x13
#define KEY_F4          (unsigned char) 0x14

#define KEY_CTRL        (unsigned char) 0x80
#define KEY_SHIFT       (unsigned char) 0x40

/* Composed keys */

#define KEY_EXCLAMATIONMARK     (KEY_1 | KEY_SHIFT)
#define KEY_QUOTE               (KEY_2 | KEY_SHIFT)
#define KEY_HASH                (KEY_3 | KEY_SHIFT)
#define KEY_DOLLAR              (KEY_4 | KEY_SHIFT)
#define KEY_PERCENT             (KEY_5 | KEY_SHIFT)
#define KEY_AMPERSAND           (KEY_6 | KEY_SHIFT)
#define KEY_APOSTROPHE          (KEY_7 | KEY_SHIFT)
#define KEY_AT                  (KEY_8 | KEY_SHIFT)
#define KEY_OPENINGPARAN        (KEY_9 | KEY_SHIFT)
#define KEY_CLOSINGPARAN        (KEY_0 | KEY_SHIFT)
#define KEY_UNDERLINE           (KEY_DASH | KEY_SHIFT)
#define KEY_BAR                 (KEY_EQUALS | KEY_SHIFT)
#define KEY_COLON               (KEY_SEMICOLON | KEY_SHIFT)
#define KEY_BACKSLASH           (KEY_PLUS | KEY_SHIFT)
#define KEY_CIRCUMFLEX          (KEY_ASTERISK | KEY_SHIFT)
#define KEY_OPENINGBRACKET      (KEY_COMMA | KEY_SHIFT)
#define KEY_CLOSINGBRACKET      (KEY_PERIOD | KEY_SHIFT)
#define KEY_QUESTIONMARK        (KEY_SLASH | KEY_SHIFT)
#define KEY_CLEAR               (KEY_LESSTHAN | KEY_SHIFT)
#define KEY_INSERT              (KEY_GREATERTHAN | KEY_SHIFT)

#define KEY_UP      (KEY_UNDERLINE | KEY_CTRL)
#define KEY_DOWN    (KEY_EQUALS | KEY_CTRL)
#define KEY_LEFT    (KEY_PLUS | KEY_CTRL)
#define KEY_RIGHT   (KEY_ASTERISK | KEY_CTRL)

/* End of _pokey.h */
#endif /* #ifndef __POKEY_H */

