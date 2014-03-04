/*****************************************************************************/
/*                                                                           */
/*                                 opcdesc.h                                 */
/*                                                                           */
/*                  Disassembler description for one opcode                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2003 Ullrich von Bassewitz                                       */
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



#ifndef OPCDESC_H
#define OPCDESC_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Constants for Flags */
enum {
    flNone          = 0x00,                     /* No flags given */
    flNoLabel       = 0x00,                     /* Don't use a label */
    flGenLabel      = 0x01,                     /* Generate a label */
    flUseLabel      = 0x02,                     /* Use a label if there is one */
    flLabel         = flUseLabel|flGenLabel,    /* Generate and use a label */
    flIllegal       = 0x10,                     /* Illegal instruction */
    flAbsOverride   = 0x20,                     /* Need a: override */
    flFarOverride   = 0x40                      /* Need f: override */
};

/* Forward/typedef for struct OpcDesc */
typedef struct OpcDesc OpcDesc;

/* Type of pointer to a function that handles opcode output */
typedef void (*OpcHandler) (const OpcDesc*);

/* Description for one opcode */
struct OpcDesc {
    char                Mnemo [6];      /* Mnemonic */
    unsigned char       Size;           /* Size of this command */
    unsigned char       Flags;          /* Flags */
    OpcHandler          Handler;        /* Handler routine */
};



/* End of opcdesc.h */

#endif
