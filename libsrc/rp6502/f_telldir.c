#include <rp6502.h>

long __fastcall__ f_telldir (int dirdes)
{
    ria_set_ax (dirdes);
    return ria_call_long (RIA_OP_TELLDIR);
}
