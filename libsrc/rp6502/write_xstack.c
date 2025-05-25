#include <rp6502.h>
#include <errno.h>

int __fastcall__ write_xstack (const void* buf, unsigned count, int fildes)
{
    unsigned i;
    if (count > 512) {
        RIA.errno_ = EINVAL;
        return __mappederrno (RIA.errno_);
    }
    for (i = count; i;) {
        ria_push_char (((char*)buf)[--i]);
    }
    ria_set_ax (fildes);
    return ria_call_int_errno (RIA_OP_WRITE_XSTACK);
}
