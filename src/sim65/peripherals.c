/*****************************************************************************/
/*                                                                           */
/*                             peripherals.c                                 */
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



#include <time.h>
#include "peripherals.h"


/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* The peripheral registers. */
PeripheralRegs PRegs;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static uint64_t get_uint64_wallclock_time(void)
{
    struct timespec ts;
    int result = clock_gettime(CLOCK_REALTIME, &ts);
    if (result != 0)
    {
        // On failure, time will be set to the max value.
        return 0xffffffffffffffff;
    }

    /* Return time since the 1-1-1970 epoch, in nanoseconds.
     * Note that this time may be off by an integer number of seconds, as POSIX
     * maintaines that all days are 86,400 seconds long, which is not true due to
     * leap seconds.
     */
    return ts.tv_sec * 1000000000 + ts.tv_nsec;
}



void PeripheralWriteByte (uint8_t Addr, uint8_t Val)
/* Write a byte to a memory location in the peripheral address aperture. */
{
    switch (Addr) {
        case PERIPHERALS_ADDRESS_OFFSET_LATCH: {
            /* A write to the "latch" register performs a simultaneous latch of all registers */

            /* Latch the current wallclock time first. */
            PRegs.latched_wallclock_time = get_uint64_wallclock_time();

            /* Now latch all the cycles maintained by the processor. */
            PRegs.latched_counter_clock_cycles = PRegs.counter_clock_cycles;
            PRegs.latched_counter_instructions = PRegs.counter_instructions;
            PRegs.latched_counter_irq_events = PRegs.counter_irq_events;
            PRegs.latched_counter_nmi_events = PRegs.counter_nmi_events;
            break;
        }
        case PERIPHERALS_ADDRESS_OFFSET_SELECT: {
            /* Set the value of the visibility-selection register. */
            PRegs.visible_latch_register = Val;
            break;
        }
        default: {
            /* Any other write is ignored */
        }
    }
}



uint8_t PeripheralReadByte (uint8_t Addr)
/* Read a byte from a memory location in the peripheral address aperture. */
{
    switch (Addr) {
        case PERIPHERALS_ADDRESS_OFFSET_SELECT: {
            return PRegs.visible_latch_register;
        }
        case PERIPHERALS_ADDRESS_OFFSET_REG64 + 0:
        case PERIPHERALS_ADDRESS_OFFSET_REG64 + 1:
        case PERIPHERALS_ADDRESS_OFFSET_REG64 + 2:
        case PERIPHERALS_ADDRESS_OFFSET_REG64 + 3:
        case PERIPHERALS_ADDRESS_OFFSET_REG64 + 4:
        case PERIPHERALS_ADDRESS_OFFSET_REG64 + 5:
        case PERIPHERALS_ADDRESS_OFFSET_REG64 + 6:
        case PERIPHERALS_ADDRESS_OFFSET_REG64 + 7: {
            /* Read from any of the eight counter bytes.
             * The first byte is the 64 bit value's LSB, the seventh byte is its MSB.
             */
            unsigned byte_select = Addr - PERIPHERALS_ADDRESS_OFFSET_REG64; /* 0 .. 7 */
            uint64_t value;
            switch (PRegs.visible_latch_register) {
                case PERIPHERALS_REG64_SELECT_CLOCKCYCLE_COUNTER: value = PRegs.latched_counter_clock_cycles; break;
                case PERIPHERALS_REG64_SELECT_INSTRUCTION_COUNTER: value = PRegs.latched_counter_instructions; break;
                case PERIPHERALS_REG64_SELECT_IRQ_COUNTER: value = PRegs.latched_counter_irq_events; break;
                case PERIPHERALS_REG64_SELECT_NMI_COUNTER: value = PRegs.latched_counter_nmi_events; break;
                case PERIPHERALS_REG64_SELECT_WALLCLOCK_TIME: value = PRegs.latched_wallclock_time; break;
                default: value = 0; /* Reading from a non-supported register will yield 0. */
            }
            /* Return the desired byte of the latched counter. 0==LSB, 7==MSB. */
            return value >> (byte_select * 8);
        }
        default: {
            /* Any other read yields a zero value. */
            return 0;
        }
    }
}



void PeripheralsInit (void)
/* Initialize the peripheral registers */
{
    PRegs.counter_clock_cycles = 0;
    PRegs.counter_instructions = 0;
    PRegs.counter_irq_events = 0;
    PRegs.counter_nmi_events = 0;

    PRegs.latched_counter_clock_cycles = 0;
    PRegs.latched_counter_instructions = 0;
    PRegs.latched_counter_irq_events = 0;
    PRegs.latched_counter_nmi_events = 0;
    PRegs.latched_wallclock_time = 0;

    PRegs.visible_latch_register = 0;
}
