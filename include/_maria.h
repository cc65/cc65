/*****************************************************************************/
/*                                                                           */
/*                                _maria.h                                   */
/*                                                                           */
/* Atari 7800, Maria chip register hardware structures                       */
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

/*
 * MARIA registers
 */
struct __maria {
    unsigned char bkgrnd;
    unsigned char p0c1;
    unsigned char p0c2;
    unsigned char p0c3;
    unsigned char wsync;
    unsigned char p1c1;
    unsigned char p1c2;
    unsigned char p1c3;
    unsigned char mstat;
    unsigned char p2c1;
    unsigned char p2c2;
    unsigned char p2c3;
    unsigned char dpph;
    unsigned char p3c1;
    unsigned char p3c2;
    unsigned char p3c3;
    unsigned char dppl;
    unsigned char p4c1;
    unsigned char p4c2;
    unsigned char p4c3;
    unsigned char chbase;
    unsigned char p5c1;
    unsigned char p5c2;
    unsigned char p5c3;
    unsigned char offset;
    unsigned char p6c1;
    unsigned char p6c2;
    unsigned char p6c3;
    unsigned char ctrl;
    unsigned char p7c1;
    unsigned char p7c2;
    unsigned char p7c3;
};
