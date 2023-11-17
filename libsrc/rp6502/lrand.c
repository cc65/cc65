#include <rp6502.h>

long __fastcall__ lrand (void)
{
    return ria_call_long (RIA_OP_LRAND);
}
