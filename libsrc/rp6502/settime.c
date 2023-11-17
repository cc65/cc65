#include <rp6502.h>
#include <time.h>

int clock_settime (clockid_t clock_id, const struct timespec* tp)
{
    ria_set_ax (clock_id);
    ria_push_long (tp->tv_nsec);
    ria_push_long (tp->tv_sec);
    return ria_call_int_errno (RIA_OP_CLOCK_SETTIME);
}
