/*****************************************************************************/
/*                                                                           */
/*                                 hlldbgsym.h                               */
/*                                                                           */
/*              Definitions for high level language debug symbols            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2011,      Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
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



/* This module contains definitions for high level language symbols passed
 * down from the C compiler. They're collected in the assembler and written to
 * the object file in binary form, then again read by the linker and finally
 * placed in the debug info file.
 */



#ifndef HLLDBGSYM_H
#define HLLDBGSYM_H



/*****************************************************************************/
/*     	      	       		     Data		   		     */
/*****************************************************************************/



/* Flag bits that tell something about the symbol */
#define HLL_TYPE_FUNC   0x0000U                 /* Function */
#define HLL_TYPE_SYM    0x0001U                 /* Symbol */
#define HLL_TYPE_MASK   0x0007U
#define HLL_GET_TYPE(x) ((x) & HL_TYPE_MASK)
#define HLL_IS_FUNC(x)  (HL_GET_TYPE(x) == HL_TYPE_FUNC)
#define HLL_IS_SYM(x)   (HL_GET_TYPE(x) == HL_TYPE_SYM)

/* Storage class */
#define HLL_SC_AUTO     0x0000U                 /* On stack */
#define HLL_SC_REG      0x0008U                 /* Register */
#define HLL_SC_STATIC   0x0010U                 /* Static linkage */
#define HLL_SC_EXTERN   0x0018U                 /* External linkage */
#define HLL_SC_MASK     0x0078U
#define HLL_GET_SC(x)   ((x) & HL_SC_MASK)

/* Structure used for a high level language function or symbol */
typedef struct HLLDbgSym HLLDbgSym;
struct HLLDbgSym {
    unsigned    Flags;                  /* See above */
    unsigned    Name;                   /* String id of name */
    unsigned    AsmName;                /* String id of assembler name */
    int         Offs;                   /* Offset if any */
    unsigned    Type;                   /* String id of type */
    unsigned    ScopeId;                /* Id of parent scope */
};



/* End of hlldbgsyms.h */

#endif



