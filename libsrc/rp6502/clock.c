#include <rp6502.h>
#include <time.h>

clock_t __fastcall__ clock (void)
{
    return ria_attr_get (RIA_ATTR_CLK_RUN_MS);
}
