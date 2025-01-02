/*****************************************************************************/
/*                                                                           */
/*                             peripherals.h                                 */
/*                                                                           */
/*        Memory-mapped peripheral subsystem for the 6502 simulator          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2024-2025, Sidney Cadot                                               */
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



#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include <stdint.h>

/* The memory range where the memory-mapped peripherals can be accessed. */

#define PERIPHERALS_APERTURE_BASE_ADDRESS  0xffc0
#define PERIPHERALS_APERTURE_LAST_ADDRESS  0xffc9

/* Declarations for the COUNTER peripheral */

#define PERIPHERALS_COUNTER_ADDRESS_OFFSET_LATCH   0x00
#define PERIPHERALS_COUNTER_ADDRESS_OFFSET_SELECT  0x01
#define PERIPHERALS_COUNTER_ADDRESS_OFFSET_VALUE   0x02

#define PERIPHERALS_COUNTER_LATCH   (PERIPHERALS_APERTURE_BASE_ADDRESS + PERIPHERALS_ADDRESS_OFFSET_COUNTER_LATCH)
#define PERIPHERALS_COUNTER_SELECT  (PERIPHERALS_APERTURE_BASE_ADDRESS + PERIPHERALS_ADDRESS_OFFSET_COUNTER_SELECT)
#define PERIPHERALS_COUNTER_VALUE   (PERIPHERALS_APERTURE_BASE_ADDRESS + PERIPHERALS_ADDRESS_OFFSET_COUNTER)

#define PERIPHERALS_COUNTER_SELECT_CLOCKCYCLE_COUNTER    0x00
#define PERIPHERALS_COUNTER_SELECT_INSTRUCTION_COUNTER   0x01
#define PERIPHERALS_COUNTER_SELECT_IRQ_COUNTER           0x02
#define PERIPHERALS_COUNTER_SELECT_NMI_COUNTER           0x03
#define PERIPHERALS_COUNTER_SELECT_WALLCLOCK_TIME        0x80
#define PERIPHERALS_COUNTER_SELECT_WALLCLOCK_TIME_SPLIT  0x81

typedef struct {
    /* The invisible counters that keep processor state. */
    uint64_t ClockCycles;
    uint64_t CpuInstructions;
    uint64_t IrqEvents;
    uint64_t NmiEvents;
    /* The 'latched_...' fields below hold values that are sampled upon a write
     * to the PERIPHERALS_COUNTER_LATCH address.
     * One of these will be visible (read only) through an eight-byte aperture.
     * The purpose of these latched registers is to read 64-bit values one byte
     * at a time, without having to worry that their content will change along
     * the way.
     */
    uint64_t LatchedClockCycles;
    uint64_t LatchedCpuInstructions;
    uint64_t LatchedIrqEvents;
    uint64_t LatchedNmiEvents;
    uint64_t LatchedWallclockTime;
    uint64_t LatchedWallclockTimeSplit;
    /* Select which of the six latched registers will be visible.
     * This is a single byte, read/write register, accessible via address
     * PERIPHERALS_COUNTER_SELECT. If a non-existent latch register is selected,
     * the PERIPHERALS_COUNTER_VALUE will be zero.
     */
    uint8_t LatchedValueSelected;
} CounterPeripheral;

/* Declarations for the SIMCONTROL peripheral. */

#define PERIPHERALS_SIMCONTROL_ADDRESS_OFFSET_CPUMODE   0x0A
#define PERIPHERALS_SIMCONTROL_ADDRESS_OFFSET_TRACEMODE 0x0B

#define PERIPHERALS_SIMCONTROL_CPUMODE   (PERIPHERALS_APERTURE_BASE_ADDRESS + PERIPHERALS_SIMCONTROL_ADDRESS_OFFSET_CPUMODE)
#define PERIPHERALS_SIMCONTROL_TRACEMODE (PERIPHERALS_APERTURE_BASE_ADDRESS + PERIPHERALS_SIMCONTROL_ADDRESS_OFFSET_TRACEMODE)

typedef struct {
    /* The SimControl peripheral has no state. */
} SimControlPeripheral;

/* Declare the 'Sim65Peripherals' type and its single instance 'Peripherals'. */

typedef struct {
    /* State of the peripherals available in sim65. */
    CounterPeripheral Counter;
    SimControlPeripheral SimControl;
} Sim65Peripherals;

extern Sim65Peripherals Peripherals;

/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void PeripheralsWriteByte (uint8_t Addr, uint8_t Val);
/* Write a byte to a memory location in the peripheral address aperture. */


uint8_t PeripheralsReadByte (uint8_t Addr);
/* Read a byte from a memory location in the peripheral address aperture. */


void PeripheralsInit (void);
/* Initialize the peripherals. */



/* End of peripherals.h */

#endif
