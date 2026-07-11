#include <rp6502.h>

int __fastcall__ time_set (unsigned long time)
{
    ria_push_long (time);
    return ria_call_int (RIA_OP_TIME_SET);
}
