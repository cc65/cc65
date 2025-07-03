/*****************************************************************************/
/*                                                                           */
/*                                 sim65.h                                   */
/*                                                                           */
/*              Definitions for the sim6502 and sim65c02 targets             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2025 Sidney Cadot                                                     */
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

#ifndef _SIM65_H
#define _SIM65_H

/* Check that we include this file  while compiling to a compatible target. */
#if !defined(__SIM6502__) && !defined(__SIM65C02__)
#  error This module may only be used when compiling for the sim6502 or sim65c02 targets!
#endif

#include <stdint.h>

/* The sim65 targets (sim6502, sim65c02) have a peripheral memory aperture located at
 * address range 0xFFC0 .. 0xFFDF. Currently, the following peripherals are located
 * inside that memory apeture:
 *
 * $FFC0 .. $FFC9      "counter" peripheral
 * $FFCA .. $FFCB      "sim65 control" peripheral
 * $FFCC .. $FFDF      (currently unused)
 *
 * The "peripherals" structure below corresponds to the register layout of the currently
 * defined peripherals in this memory range. Combined with the fact that the sim6502 and
 * sim65c02 linker configuration files define the "peripherals" symbol to be fixed at
 * address $FFC0, this provides easy-to-use and efficient access to the peripheral registers.
 *
 * After including "sim65.h", it is possible for a C program to do things like:
 *
 * {
 *     peripherals.counter.latch = 0;
 *     peripherals.sim65.cpu_mode = SIM65_CPU_MODE_6502X;
 *     peripherals.sim65.trace_mode = SIM65_TRACE_MODE_ENABLE_FULL;
 * }
 *
 * Note that "peripherals" variable is declared volatile. This instructs a C compiler to
 * forego optimizations on memory accesses to the variable. However, CC65 currently ignores
 * the volatile attribute. Fortunately, it is not smart with respect to optimizing
 * memory accesses, so accessing the "peripherals" fields works fine in practice.
 */

extern volatile struct {
    struct {
        uint8_t  latch;
        uint8_t  select;
        union {
            uint8_t  value   [8]; /* Access value as eight separate bytes.  */
            uint16_t value16 [4]; /* Access value as four 16-bit words.     */
            uint32_t value32 [2]; /* Access value as two 32-bit long words. */
        };
    } counter;
    struct {
        uint8_t  cpu_mode;
        uint8_t  trace_mode;
    } sim65;
} peripherals;

/* Values for the peripherals.counter.select field. */
#define COUNTER_SELECT_CLOCKCYCLE_COUNTER    0x00
#define COUNTER_SELECT_INSTRUCTION_COUNTER   0x01
#define COUNTER_SELECT_IRQ_COUNTER           0x02
#define COUNTER_SELECT_NMI_COUNTER           0x03
#define COUNTER_SELECT_WALLCLOCK_TIME        0x80
#define COUNTER_SELECT_WALLCLOCK_TIME_SPLIT  0x81

/* Values for the peripherals.sim65.cpu_mode field. */
#define SIM65_CPU_MODE_6502                  0x00
#define SIM65_CPU_MODE_65C02                 0x01
#define SIM65_CPU_MODE_6502X                 0x02

/* Bitfield values for the peripherals.sim65.trace_mode field. */
#define SIM65_TRACE_MODE_FIELD_INSTR_COUNTER   0x40
#define SIM65_TRACE_MODE_FIELD_CLOCK_COUNTER   0x20
#define SIM65_TRACE_MODE_FIELD_PC              0x10
#define SIM65_TRACE_MODE_FIELD_INSTR_BYTES     0x08
#define SIM65_TRACE_MODE_FIELD_INSTR_ASSEMBLY  0x04
#define SIM65_TRACE_MODE_FIELD_CPU_REGISTERS   0x02
#define SIM65_TRACE_MODE_FIELD_CC65_SP         0x01

/* Values for the peripherals.sim65.trace_mode field that fully disable / enable tracing. */
#define SIM65_TRACE_MODE_DISABLE               0x00
#define SIM65_TRACE_MODE_ENABLE_FULL           0x7F

/* Convenience macros to enable / disable tracing at runtime. */
#define TRACE_ON()  do peripherals.sim65.trace_mode = SIM65_TRACE_MODE_ENABLE_FULL; while(0)
#define TRACE_OFF() do peripherals.sim65.trace_mode = SIM65_TRACE_MODE_DISABLE;     while(0)

/* Convenience macro to query the CPU mode at runtime. */
#define GET_CPU_MODE() peripherals.sim65.cpu_mode

/* Convenience macro to set the CPU mode at runtime.
 *
 * Use SIM65_CPU_MODE_6502, SIM65_CPU_MODE_65C02, or SIM65_CPU_MODE_6502 as argument.
 *
 * Important Note:
 *
 * When running in a program compiled for the "sim6502" target, it is safe to switch to
 * 65C02 or 6502X mode, since the runtime library will only use plain 6502 opcodes, and
 * those work the same in 65C02 and 6502X mode.
 *
 * However, when running in a program compiled for the "sim65c02" target, it is NOT safe
 * to switch to 6502 or 6502X mode, since many routines in the runtime library use
 * 65C02-specific opcodes, and these will not work as expected when the CPU is switched
 * to 6502 or 6502X mode. When such an instruction is encountered, the program will
 * exhibit undefined behavior.
 */
#define SET_CPU_MODE(mode)  do peripherals.sim65.cpu_mode = mode; while(0)

/* End of sim65.h */
#endif
