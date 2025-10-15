#include <rp6502.h>
#include <fcntl.h>

int __fastcall__ f_closedir (int dirdes)
{
    ria_set_ax (dirdes);
    return ria_call_int (RIA_OP_CLOSEDIR);
}
