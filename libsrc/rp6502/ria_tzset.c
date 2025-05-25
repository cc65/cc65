#include <rp6502.h>
#include <time.h>

int clock_gettimezone (time_t time, clockid_t clock_id, struct _timezone* tz);

void ria_tzset (unsigned long time)
{
    clock_gettimezone (time, CLOCK_REALTIME, &_tz);
}
