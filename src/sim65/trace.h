/*****************************************************************************/
/*                                                                           */
/*                                 trace.h                                   */
/*                                                                           */
/*             Instruction tracing functionality sim65 6502 simulator        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2025, Sidney Cadot                                                    */
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


#ifndef TRACE_H
#define TRACE_H


#include <stdint.h>


#include "6502.h"

/* The trace mode is a bitfield that determines how trace lines are displayed.
 *
 * The value zero indicates that tracing is disabled (the default).
 *
 * In case TraceMode is not equal to zero, the value is interpreted as a bitfield:
 *
 * Bit    Bit value     Enables
 * ---    -----------   -------------------------------
 *  6      0x40 ( 64)   Print the instruction counter.
 *  5      0x20 ( 32)   Print the clock cycle counter.
 *  4      0x10 ( 16)   Print the PC (program counter).
 *  3      0x08 (  8)   Print the instruction bytes.
 *  2      0x04 (  4)   Print the instruction assembly.
 *  1      0x02 (  2)   Print the CPU registers.
 *  0      0x01 (  1)   Print the CC65 stack pointer.
 *
 */

#define TRACE_FIELD_INSTR_COUNTER   0x40
#define TRACE_FIELD_CLOCK_COUNTER   0x20
#define TRACE_FIELD_PC              0x10
#define TRACE_FIELD_INSTR_BYTES     0x08
#define TRACE_FIELD_INSTR_ASSEMBLY  0x04
#define TRACE_FIELD_CPU_REGISTERS   0x02
#define TRACE_FIELD_CC65_SP         0x01

#define TRACE_DISABLED              0x00
#define TRACE_ENABLE_FULL           0x7f

/* Currently active tracing mode. */
extern uint8_t TraceMode;

void TraceInit (uint8_t SPAddr);
/* Initialize the trace subsystem. */

void PrintTraceNMI(void);
/* Print trace line for an NMI interrupt. */

void PrintTraceIRQ(void);
/* Print trace line for an IRQ interrupt. */

void PrintTraceInstruction (void);
/* Print trace line for the instruction at the currrent program counter. */



/* End of trace.h */

#endif
