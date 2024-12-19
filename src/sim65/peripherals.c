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



/* The system-wide state of the peripherals */
Sim65Peripherals Peripherals;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void PeripheralsWriteByte (uint8_t Addr, uint8_t Val)
/* Write a byte to a memory location in the peripherals address aperture. */
{
    switch (Addr) {
        case PERIPHERALS_COUNTER_ADDRESS_OFFSET_LATCH: {
            /* A write to the "latch" register performs a simultaneous latch of all registers. */

            /* Latch the current wallclock time first. */
	    struct timespec ts;
            int result = clock_gettime(CLOCK_REALTIME, &ts);
	    if (result != 0) {
	        /* Unable to read time. Report max uint64 value for both fields. */
  	        Peripherals.Counter.latched_wallclock_time = 0xffffffffffffffff;
                Peripherals.Counter.latched_wallclock_time_split = 0xffffffffffffffff;
	    } else {
	        /* Number of nanoseconds since 1-1-1970. */
                Peripherals.Counter.latched_wallclock_time = 1000000000u * ts.tv_sec + ts.tv_nsec;
	        /* High word is number of seconds, low word is number of nanoseconds. */
                Peripherals.Counter.latched_wallclock_time_split = (ts.tv_sec << 32) | ts.tv_nsec;
            }

            /* Latch the counters that reflect the state of the processor. */
            Peripherals.Counter.latched_clock_cycles = Peripherals.Counter.clock_cycles;
            Peripherals.Counter.latched_cpu_instructions = Peripherals.Counter.cpu_instructions;
            Peripherals.Counter.latched_irq_events = Peripherals.Counter.irq_events;
            Peripherals.Counter.latched_nmi_events = Peripherals.Counter.nmi_events;
            break;
        }
        case PERIPHERALS_COUNTER_ADDRESS_OFFSET_SELECT: {
            /* Set the value of the visibility-selection register. */
            Peripherals.Counter.visible_latch_register = Val;
            break;
        }
        default: {
            /* Any other write is ignored */
        }
    }
}



uint8_t PeripheralsReadByte (uint8_t Addr)
/* Read a byte from a memory location in the peripherals address aperture. */
{
    switch (Addr) {
        case PERIPHERALS_COUNTER_ADDRESS_OFFSET_SELECT: {
            return Peripherals.Counter.visible_latch_register;
        }
        case PERIPHERALS_COUNTER_ADDRESS_OFFSET_VALUE + 0:
        case PERIPHERALS_COUNTER_ADDRESS_OFFSET_VALUE + 1:
        case PERIPHERALS_COUNTER_ADDRESS_OFFSET_VALUE + 2:
        case PERIPHERALS_COUNTER_ADDRESS_OFFSET_VALUE + 3:
        case PERIPHERALS_COUNTER_ADDRESS_OFFSET_VALUE + 4:
        case PERIPHERALS_COUNTER_ADDRESS_OFFSET_VALUE + 5:
        case PERIPHERALS_COUNTER_ADDRESS_OFFSET_VALUE + 6:
        case PERIPHERALS_COUNTER_ADDRESS_OFFSET_VALUE + 7: {
            /* Read from any of the eight counter bytes.
             * The first byte is the 64 bit value's LSB, the seventh byte is its MSB.
             */
            unsigned byte_select = Addr - PERIPHERALS_COUNTER_ADDRESS_OFFSET_VALUE; /* 0 .. 7 */
            uint64_t value;
            switch (Peripherals.Counter.visible_latch_register) {
                case PERIPHERALS_COUNTER_SELECT_CLOCKCYCLE_COUNTER: value = Peripherals.Counter.latched_clock_cycles; break;
                case PERIPHERALS_COUNTER_SELECT_INSTRUCTION_COUNTER: value = Peripherals.Counter.latched_cpu_instructions; break;
                case PERIPHERALS_COUNTER_SELECT_IRQ_COUNTER: value = Peripherals.Counter.latched_irq_events; break;
                case PERIPHERALS_COUNTER_SELECT_NMI_COUNTER: value = Peripherals.Counter.latched_nmi_events; break;
                case PERIPHERALS_COUNTER_SELECT_WALLCLOCK_TIME: value = Peripherals.Counter.latched_wallclock_time; break;
                case PERIPHERALS_COUNTER_SELECT_WALLCLOCK_TIME_SPLIT: value = Peripherals.Counter.latched_wallclock_time_split; break;
                default: value = 0; /* Reading from a non-existent register will yield 0. */
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
    /* Initialize the COUNTER peripheral */

    Peripherals.Counter.clock_cycles = 0;
    Peripherals.Counter.cpu_instructions = 0;
    Peripherals.Counter.irq_events = 0;
    Peripherals.Counter.nmi_events = 0;

    Peripherals.Counter.latched_clock_cycles = 0;
    Peripherals.Counter.latched_cpu_instructions = 0;
    Peripherals.Counter.latched_irq_events = 0;
    Peripherals.Counter.latched_nmi_events = 0;
    Peripherals.Counter.latched_wallclock_time = 0;
    Peripherals.Counter.latched_wallclock_time_split = 0;

    Peripherals.Counter.visible_latch_register = 0;
}
