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

/* Declarations for the COUNTER peripheral (currently the only peripheral). */

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
    uint64_t clock_cycles;
    uint64_t cpu_instructions;
    uint64_t irq_events;
    uint64_t nmi_events;
    /* Latched counters upon a write to the PERIPHERALS_COUNTER_LATCH address.
     * One of these will be visible (read only) through an eight-byte aperture.
     * The purpose of these latched registers is to read 64-bit values one byte
     * at a time, without having to worry that their content will change along
     * the way.
     */
    uint64_t latched_clock_cycles;
    uint64_t latched_cpu_instructions;
    uint64_t latched_irq_events;
    uint64_t latched_nmi_events;
    uint64_t latched_wallclock_time;
    uint64_t latched_wallclock_time_split;
    /* Select which of the six latched registers will be visible.
     * This is a single byte, read/write register, accessible via address PERIPHERALS_COUNTER_SELECT.
     * If a non-existent latch register is selected, the PERIPHERALS_REGS64 value will be zero.
     */
    uint8_t  visible_latch_register;
} CounterPeripheral;



/* Declare the 'Sim65Peripherals' type and its single instance 'Peripherals'. */

typedef struct {
    /* State of the peripherals simulated by sim65.
     * Currently, there is only one: the COUNTER peripheral. */
    CounterPeripheral Counter;
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
/* Initialize the peripheral registers */



/* End of peripherals.h */

#endif
