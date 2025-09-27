#include <rp6502.h>
#include <fcntl.h>

int __fastcall__ close (int fd)
{
    ria_set_ax (fd);
    return ria_call_int (RIA_OP_CLOSE);
}
