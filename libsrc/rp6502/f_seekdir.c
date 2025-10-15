#include <rp6502.h>

int __fastcall__ f_seekdir (long offs, int dirdes)
{
    ria_set_ax (dirdes);
    ria_push_long (offs);
    return ria_call_int (RIA_OP_SEEKDIR);
}
