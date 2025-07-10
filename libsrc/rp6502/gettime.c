#include <rp6502.h>
#include <time.h>

extern int __clock_gettimespec (struct timespec* ts, unsigned char op);

int clock_gettime (clockid_t clock_id, struct timespec* tp)
{
    (void)clock_id;
    /* time.s doesn't set the stack value for clock_id (bug?) */
    ria_set_ax (CLOCK_REALTIME);
    return __clock_gettimespec (tp, RIA_OP_CLOCK_GETTIME);
}
