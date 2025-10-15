#include <rp6502.h>

int __fastcall__ write_xram (unsigned buf, unsigned count, int fildes)
{
    ria_set_ax (fildes);
    ria_push_int (buf);
    ria_push_int (count);
    return ria_call_int (RIA_OP_WRITE_XRAM);
}
