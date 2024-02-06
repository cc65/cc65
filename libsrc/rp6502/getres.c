#include <rp6502.h>
#include <time.h>

extern int __clock_gettimespec (struct timespec* ts, unsigned char op);

int clock_getres (clockid_t clock_id, struct timespec* res)
{
    ria_set_ax (clock_id);
    return __clock_gettimespec (res, RIA_OP_CLOCK_GETRES);
}
