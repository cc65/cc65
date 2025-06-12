/*****************************************************************************/
/*                                                                           */
/*                             peripherals.c                                 */
/*                                                                           */
/*         Memory-mapped peripheral subsystem for the 6502 simulator         */
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

#include <stdbool.h>
#include <time.h>
#if defined(__MINGW64__) || defined(__MINGW32__)
/* For gettimeofday() */
#include <sys/time.h>
#endif


#include "peripherals.h"
#include "trace.h"
#include "6502.h"


/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* The system-wide state of the peripherals */
Sim65Peripherals Peripherals;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/

static bool GetWallclockTime (struct timespec * ts)
/* Get the wallclock time with nanosecond resolution. */
{
    /* Note: the 'struct timespec' type is available on all compilers we want to support. */

    bool time_valid;

#if defined(__MINGW64__)
    /* When using the MinGW64 compiler, neither timespec_get() nor clock_gettime()
     * are available; using either of them makes the Linux PR build workflow build fail.
     * The gettimeofday() function does work, so use that; its microsecond resolution
     * is fine for most applications.
     */
    struct timeval tv;
    time_valid = (gettimeofday(&tv, NULL) == 0);
    if (time_valid) {
        ts->tv_sec = tv.tv_sec;
        ts->tv_nsec = tv.tv_usec * 1000;
    }
#elif defined(__MINGW32__)
    /* Note: we test for MinGW32 after the test for MinGW64, as the __MINGW32__ symbol is also
     * defined in MinGW64. This allows us to distinguish MinGW32 and MinGW64 build.
     *
     * When using the MinGW32 compiler, neither timespec_get() nor clock_gettime()
     * are available; using either of them makes the Linux snapshot workflow build fail.
     * The gettimeofday() function does work, so use that; its microsecond resolution
     * is fine for most applications.
     */
    struct timeval tv;
    time_valid = (gettimeofday(&tv, NULL) == 0);
    if (time_valid) {
        ts->tv_sec = tv.tv_sec;
        ts->tv_nsec = tv.tv_usec * 1000;
    }
#elif defined(_MSC_VER)
    /* Using the Microsoft C++ compiler.
     * clock_gettime() is not available; use timespec_get() instead.
     */
    time_valid = timespec_get(ts, TIME_UTC) == TIME_UTC;
#else
    /* On all other compilers, assume that clock_gettime() is available.
     * This is true on Linux and MacOS, at least.
     */
    time_valid = clock_gettime(CLOCK_REALTIME, ts) == 0;
#endif

    return time_valid;
}



void PeripheralsWriteByte (uint8_t Addr, uint8_t Val)
/* Write a byte to a memory location in the peripherals address aperture. */
{
    switch (Addr) {

        /* Handle writes to the Counter peripheral. */

        case PERIPHERALS_COUNTER_ADDRESS_OFFSET_LATCH: {

            /* A write to the "latch" register performs a simultaneous latch of all registers. */

            /* Latch the current wallclock time before doing anything else. */

            struct timespec ts;
            bool time_valid = GetWallclockTime (&ts);

            if (time_valid) {
                /* Wallclock time: number of nanoseconds since 1-1-1970. */
                Peripherals.Counter.LatchedWallclockTime = 1000000000 * (uint64_t)ts.tv_sec + ts.tv_nsec;
                /* Wallclock time, split: high word is number of seconds since 1-1-1970,
                 * low word is number of nanoseconds since the start of that second. */
                Peripherals.Counter.LatchedWallclockTimeSplit = (uint64_t)ts.tv_sec << 32 | ts.tv_nsec;
            } else {
                /* Unable to get time. Report max uint64 value for both fields. */
                Peripherals.Counter.LatchedWallclockTime = -1;
                Peripherals.Counter.LatchedWallclockTimeSplit = -1;
            }

            /* Latch the counters that reflect the state of the processor. */
            Peripherals.Counter.LatchedClockCycles = Peripherals.Counter.ClockCycles;
            Peripherals.Counter.LatchedCpuInstructions = Peripherals.Counter.CpuInstructions;
            Peripherals.Counter.LatchedIrqEvents = Peripherals.Counter.IrqEvents;
            Peripherals.Counter.LatchedNmiEvents = Peripherals.Counter.NmiEvents;
            break;
        }
        case PERIPHERALS_COUNTER_ADDRESS_OFFSET_SELECT: {
            /* Set the value of the visibility-selection register. */
            Peripherals.Counter.LatchedValueSelected = Val;
            break;
        }

        /* Handle writes to the SimControl peripheral. */

        case PERIPHERALS_SIMCONTROL_ADDRESS_OFFSET_CPUMODE: {
            if (Val == CPU_6502 || Val == CPU_65C02 || Val == CPU_6502X) {
                CPU = Val;
            }
            break;
        }

        case PERIPHERALS_SIMCONTROL_ADDRESS_OFFSET_TRACEMODE: {
            TraceMode = Val;
            break;
        }

        /* Handle writes to unused and read-only peripheral addresses. */

        default: {
            /* No action. */
        }
    }
}



uint8_t PeripheralsReadByte (uint8_t Addr)
/* Read a byte from a memory location in the peripherals address aperture. */
{
    switch (Addr) {

        /* Handle reads from the Counter peripheral. */

        case PERIPHERALS_COUNTER_ADDRESS_OFFSET_SELECT: {
            return Peripherals.Counter.LatchedValueSelected;
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
            unsigned SelectedByteIndex = Addr - PERIPHERALS_COUNTER_ADDRESS_OFFSET_VALUE; /* 0 .. 7 */
            uint64_t Value;
            switch (Peripherals.Counter.LatchedValueSelected) {
                case PERIPHERALS_COUNTER_SELECT_CLOCKCYCLE_COUNTER: Value = Peripherals.Counter.LatchedClockCycles; break;
                case PERIPHERALS_COUNTER_SELECT_INSTRUCTION_COUNTER: Value = Peripherals.Counter.LatchedCpuInstructions; break;
                case PERIPHERALS_COUNTER_SELECT_IRQ_COUNTER: Value = Peripherals.Counter.LatchedIrqEvents; break;
                case PERIPHERALS_COUNTER_SELECT_NMI_COUNTER: Value = Peripherals.Counter.LatchedNmiEvents; break;
                case PERIPHERALS_COUNTER_SELECT_WALLCLOCK_TIME: Value = Peripherals.Counter.LatchedWallclockTime; break;
                case PERIPHERALS_COUNTER_SELECT_WALLCLOCK_TIME_SPLIT: Value = Peripherals.Counter.LatchedWallclockTimeSplit; break;
                default: Value = 0; /* Reading from a non-existent latch register will yield 0. */
            }
            /* Return the desired byte of the latched counter; 0==LSB, 7==MSB. */
            return (uint8_t)(Value >> (SelectedByteIndex * 8));
        }

        /* Handle reads from the SimControl peripheral. */

        case PERIPHERALS_SIMCONTROL_ADDRESS_OFFSET_CPUMODE: {
            return CPU;
        }

        case PERIPHERALS_SIMCONTROL_ADDRESS_OFFSET_TRACEMODE: {
            return TraceMode;
        }

        /* Handle reads from unused peripheral and write-only addresses. */

        default: {
            /* Return zero value. */
            return 0;
        }
    }
}



void PeripheralsInit (void)
/* Initialize the peripherals. */
{
    /* Initialize the Counter peripheral */

    Peripherals.Counter.ClockCycles = 0;
    Peripherals.Counter.CpuInstructions = 0;
    Peripherals.Counter.IrqEvents = 0;
    Peripherals.Counter.NmiEvents = 0;

    Peripherals.Counter.LatchedClockCycles = 0;
    Peripherals.Counter.LatchedCpuInstructions = 0;
    Peripherals.Counter.LatchedIrqEvents = 0;
    Peripherals.Counter.LatchedNmiEvents = 0;
    Peripherals.Counter.LatchedWallclockTime = 0;
    Peripherals.Counter.LatchedWallclockTimeSplit = 0;

    Peripherals.Counter.LatchedValueSelected = 0;
}
