#include <rp6502.h>
#include <time.h>
#include <stdbool.h>

static bool tzset_set = false;

struct tm* __fastcall__ _time_t_to_tm (const time_t t);

struct tm* __fastcall__ _localtime (const time_t* timep)
{
    long time = *timep;
    struct tm* tm;
    ria_set_axsreg (*timep);
    time += ria_call_long (RIA_OP_TZQUERY);
    tm = _time_t_to_tm (time);
    tm->tm_isdst = ria_pop_char ();
    if (!tzset_set) { tzset (); }
    return tm;
}

void tzset (void)
{
    int ax = ria_call_int (RIA_OP_TZSET);
    if (ax >= 0) {
        char i;
        for (i = 0; i < sizeof (struct _timezone); i++) {
            ((char*)&_tz)[i] = ria_pop_char ();
        }
        tzset_set = true;
    }
}
