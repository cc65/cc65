/*
 * Sim65 trace functionality example.
 *
 * Description
 * -----------
 *
 * The easiest way to use tracing in sim65 is to pass the '--trace' option
 * to sim65 while starting a program.
 *
 * However, it is also possiblke to enable and disable the trace functionality
 * at runtime, from within the C code itself. This can be useful to produce
 * runtime traces of small code fragments for debugging purposes.
 *
 * In this example, We use the TRACE_ON and TRACE_OFF macros provided in sim65.h
 * to trace what the CPU is doing during a single statement: the assignment of
 * a constant to a global variable.
 *
 * Running the example
 * -------------------
 *
 * cl65 -t sim6502 -O trace_example.c -o trace_example.prg
 * sim65 trace_example.prg
 *
 * Compiling and running the program like this will produce a trace of six 6502 instructions.
 * The first four instructions correspond to the 'x = 0x1234' assignment statement.
 * The last two instructions (ending in a store to address $FFCB) disable the trace facility.
 *
 */

#include <sim65.h>

unsigned x;

int main(void)
{
    TRACE_ON();
    x = 0x1234;
    TRACE_OFF();
    return 0;
}
