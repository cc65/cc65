#include <rp6502.h>
#include <time.h>

int clock_gettimezone (clockid_t clock_id, struct _timezone* tz);

void tzset (void)
{
    clock_gettimezone (CLOCK_REALTIME, &_tz);
}
