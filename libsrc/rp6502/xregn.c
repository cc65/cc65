#include <rp6502.h>
#include <stdarg.h>

int __cdecl__ xregn (char device, char channel, unsigned char address, unsigned count,
    ...)
{
    va_list args;
    va_start (args, count);
    RIA.xstack = device;
    RIA.xstack = channel;
    RIA.xstack = address;
    while (count--) {
        unsigned v = va_arg (args, unsigned);
        RIA.xstack = v >> 8;
        RIA.xstack = v;
    }
    va_end (args);
    return ria_call_int (RIA_OP_XREG);
}
