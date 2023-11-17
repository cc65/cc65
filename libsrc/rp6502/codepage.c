#include <rp6502.h>

int __fastcall__ codepage (void)
{
    return ria_call_int (RIA_OP_CODEPAGE);
}
