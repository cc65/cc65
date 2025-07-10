#include <rp6502.h>
#include <time.h>

clock_t __fastcall__ clock (void)
{
    return ria_call_long (RIA_OP_CLOCK);
}
