#include <rp6502.h>
#include <errno.h>
#include <string.h>

int __fastcall__ f_getlabel (const char* path, char* label)
{
    int i, ax;
    size_t pathlen = strlen (path);
    if (pathlen > 255) {
        errno = EINVAL;
        return -1;
    }
    while (pathlen) {
        ria_push_char (path[--pathlen]);
    }
    ax = ria_call_int (RIA_OP_GETLABEL);
    for (i = 0; i < ax; i++) {
        label[i] = ria_pop_char ();
    }
    return ax;
}
