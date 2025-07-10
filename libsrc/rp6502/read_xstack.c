#include <rp6502.h>

int __fastcall__ read_xstack (void* buf, unsigned count, int fildes)
{
    int i, ax;
    ria_push_int (count);
    ria_set_ax (fildes);
    ax = ria_call_int_errno (RIA_OP_READ_XSTACK);
    for (i = 0; i < ax; i++) {
        ((char*)buf)[i] = ria_pop_char ();
    }
    return ax;
}
