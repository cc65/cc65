#include <rp6502.h>
#include <errno.h>
#include <string.h>

int __fastcall__ f_chmod (const char* path, unsigned char attr, unsigned char mask)
{
    size_t pathlen;
    ria_set_a (mask);
    pathlen = strlen (path);
    if (pathlen > 255) {
        errno = EINVAL;
        return -1;
    }
    while (pathlen) {
        ria_push_char (path[--pathlen]);
    }
    ria_push_char (attr);
    return ria_call_int (RIA_OP_CHMOD);
}
