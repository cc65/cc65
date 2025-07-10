#include <rp6502.h>
#include <unistd.h>

off_t __fastcall__ lseek (int fd, off_t offset, int whence)
{
    /* Modified argument order for short stacking offset */
    ria_push_long (offset);
    ria_push_char (whence);
    ria_set_ax (fd);
    return ria_call_long_errno (RIA_OP_LSEEK);
}
