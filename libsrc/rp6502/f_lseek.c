#include <rp6502.h>

long __fastcall__ f_lseek (long offset, int whence, int fildes)
{
    ria_set_ax (fildes);
    ria_push_long (offset);
    ria_push_char (whence);
    return ria_call_long (RIA_OP_LSEEK);
}
