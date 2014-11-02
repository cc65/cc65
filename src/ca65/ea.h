/*****************************************************************************/
/*                                                                           */
/*                                  ea65.h                                   */
/*                                                                           */
/*                  Effective address structure definition                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2004 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
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



#ifndef EA_H
#define EA_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* GetEA result struct */
typedef struct EffAddr EffAddr;
struct EffAddr {
    /* First three fields get filled when calling GetEA */
    unsigned long       AddrModeSet;    /* Possible addressing modes */
    struct ExprNode*    Expr;           /* Expression if any (NULL otherwise) */
    unsigned            Reg;            /* Register number in sweet16 mode */

    /* The following fields are used inside instr.c */
    unsigned            AddrMode;       /* Actual addressing mode used */
    unsigned long       AddrModeBit;    /* Addressing mode as bit mask */
    unsigned char       Opcode;         /* Opcode */
};



/* End of ea.h */

#endif
