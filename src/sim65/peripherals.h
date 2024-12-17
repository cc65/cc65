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

#define PERIPHERALS_APERTURE_BASE_ADDRESS   0xffc0
#define PERIPHERALS_APERTURE_LAST_ADDRESS   0xffc9

#define PERIPHERALS_ADDRESS_OFFSET_LATCH  0x00
#define PERIPHERALS_ADDRESS_OFFSET_SELECT 0x01
#define PERIPHERALS_ADDRESS_OFFSET_REG64  0x02

#define PERIPHERALS_LATCH  (PERIPHERALS_APERTURE_BASE_ADDRESS + PERIPHERALS_ADDRESS_OFFSET_LATCH)
#define PERIPHERALS_SELECT (PERIPHERALS_APERTURE_BASE_ADDRESS + PERIPHERALS_ADDRESS_OFFSET_SELECT)
#define PERIPHERALS_REG64  (PERIPHERALS_APERTURE_BASE_ADDRESS + PERIPHERALS_ADDRESS_OFFSET_REG64)

#define PERIPHERALS_REG64_SELECT_CLOCKCYCLE_COUNTER   0x00
#define PERIPHERALS_REG64_SELECT_INSTRUCTION_COUNTER  0x01
#define PERIPHERALS_REG64_SELECT_IRQ_COUNTER          0x02
#define PERIPHERALS_REG64_SELECT_NMI_COUNTER          0x03
#define PERIPHERALS_REG64_SELECT_WALLCLOCK_TIME       0x80

typedef struct {
    /* the invisible counters that are continuously updated */
    uint64_t counter_clock_cycles;
    uint64_t counter_instructions;
    uint64_t counter_irq_events;
    uint64_t counter_nmi_events;
    /* latched counters upon a write to the 'latch' address.
     * One of these will be visible (read only) through an each-byte aperture. */
    uint64_t latched_counter_clock_cycles;
    uint64_t latched_counter_instructions;
    uint64_t latched_counter_irq_events;
    uint64_t latched_counter_nmi_events;
    uint64_t latched_wallclock_time;
    /* Select which of the five latched registers will be visible.
     * This is a Read/Write byte-wide register.
     * If a non-existent register is selected, the 8-byte aperture will read as zero.
     */
    uint8_t  visible_latch_register;
} PeripheralRegs;

extern PeripheralRegs PRegs;

/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void PeripheralWriteByte (uint8_t Addr, uint8_t Val);
/* Write a byte to a memory location in the peripheral address aperture. */


uint8_t PeripheralReadByte (uint8_t Addr);
/* Read a byte from a memory location in the peripheral address aperture. */


void PeripheralsInit (void);
/* Initialize the peripheral registers */



/* End of peripherals.h */

#endif
