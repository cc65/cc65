/*****************************************************************************/
/*                                                                           */
/*				  opctable.h				     */
/*                                                                           */
/*		     Disassembler opcode description table		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



#ifndef OPCTABLE_H
#define OPCTABLE_H



/*****************************************************************************/
/*				     Data				     */
/*****************************************************************************/



/* Constants for LabelFlag */
enum {
    lfNoLabel	= 0x00,			/* Don't use a label */
    lfGenLabel 	= 0x01,			/* Generate a label */
    lfUseLabel	= 0x02,			/* Use a label if there is one */
    lfLabel	= lfUseLabel|lfGenLabel /* Generate and use a label */
};

/* Forward/typedef for struct OpcDesc */
typedef struct OpcDesc OpcDesc;

/* Type of pointer to a function that handles opcode output */
typedef void (*OpcHandler) (const OpcDesc*);

/* Description for one opcode */
struct OpcDesc {
    char       	      	Mnemo [4]; 	/* Mnemonic */
    unsigned char     	Size;		/* Size of this command */
    unsigned char     	LabelFlag;	/* Generate/use label? */
    unsigned char     	CPU;		/* Available for which CPU? */
    OpcHandler		Handler;	/* Handler routine */
};

/* Descriptions for all opcodes */
extern const OpcDesc OpcTable[256];



/* End of opctable.h */
#endif



