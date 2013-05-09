/*****************************************************************************/
/*                                                                           */
/*                                 _pokey.h                                  */
/*                                                                           */
/*                Internal include file, do not use directly                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000 Freddy Offenga <taf_offenga@yahoo.com>                           */
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

/* End of _pokey.h */
#endif /* #ifndef __POKEY_H */

