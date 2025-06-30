/*****************************************************************************/
/*                                                                           */
/*                              arpa/inet.h                                  */
/*                                                                           */
/*                     Endianness utilities for cc65                         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2023 Colin Leroy-Mira, <colin@colino.net>                             */
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



#ifndef _ARPA_INET_H
#define _ARPA_INET_H



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/


#if (__CPU__ & __CPU_ISET_65SC02__)
/* Always inline, three bytes is not more than a jsr */

#define ntohs(x)                \
    (                           \
        __AX__=(x),             \
        asm("phx"),             \
        asm("tax"),             \
        asm("pla"),             \
        __AX__                  \
    )
#define htons(x) ntohs(x)

#else

#if (__OPT_i__ < 200)
int __fastcall__ ntohs (int val);
int __fastcall__ htons (int val);
#else

#define ntohs(x)                \
    (                           \
        __AX__=(x),             \
        asm("sta tmp1"),        \
        asm("txa"),             \
        asm("ldx tmp1"),        \
        __AX__                  \
    )
#define htons(x) ntohs(x)

#endif /* __OPT_i__ < 200 */

#endif /* __CPU__ & __CPU_ISET_65SC02__ */

long __fastcall__ ntohl (long val);
long __fastcall__ htonl (long val);

/* End of arpa/inet.h */
#endif
