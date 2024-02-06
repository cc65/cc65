#include <rp6502.h>
#include <time.h>

int clock_gettimezone (clockid_t clock_id, struct _timezone* tz)
{
    int ax;
    ria_set_ax (clock_id);
    ax = ria_call_int_errno (RIA_OP_CLOCK_GETTIMEZONE);
    if (ax >= 0) {
        char i;
        for (i = 0; i < sizeof (struct _timezone); i++) {
            ((char*)tz)[i] = ria_pop_char ();
        }
    }
    return ax;
}
