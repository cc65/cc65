#include <rp6502.h>

int __fastcall__ codepage (int cp)
{
    ria_set_ax (cp);
    return ria_call_int (RIA_OP_CODEPAGE);
}
