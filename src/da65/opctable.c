/*****************************************************************************/
/*                                                                           */
/*                                opctable.c                                 */
/*                                                                           */
/*                   Disassembler opcode description table                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2011, Ullrich von Bassewitz                                      */
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



/* da65 */
#include "error.h"
#include "opc6502.h"
#include "opc6502x.h"
#include "opc65816.h"
#include "opc65c02.h"
#include "opc65sc02.h"
#include "opchuc6280.h"
#include "opcm740.h"
#include "opctable.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Descriptions for all opcodes */
const OpcDesc* OpcTable = OpcTable_6502;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void SetOpcTable (cpu_t CPU)
/* Set the correct opcode table for the given CPU */
{
    switch (CPU) {
        case CPU_6502:    OpcTable = OpcTable_6502;     break;
        case CPU_6502X:   OpcTable = OpcTable_6502X;    break;
        case CPU_65SC02:  OpcTable = OpcTable_65SC02;   break;
        case CPU_65C02:   OpcTable = OpcTable_65C02;    break;
        case CPU_HUC6280: OpcTable = OpcTable_HuC6280;  break;
        case CPU_M740:    OpcTable = OpcTable_M740;     break;
        default:          Error ("Unsupported CPU");
    }
}
