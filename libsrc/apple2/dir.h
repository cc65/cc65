/*****************************************************************************/
/*                                                                           */
/*                                    dir.h                                  */
/*                                                                           */
/*                       Apple ][ system specific DIR                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2005  Oliver Schmidt, <ol.sc@web.de>                                  */
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



#ifndef _DIR_H
#define _DIR_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



struct DIR {
    int           fd;
    unsigned char entry_length;
    unsigned char entries_per_block;
    unsigned char current_entry;
    union {
        unsigned char bytes[512];
        struct {
            unsigned      prev_block;
            unsigned      next_block;
            unsigned char entries[1];
        } content;
    } block;
};



/* End of dir.h */
#endif
