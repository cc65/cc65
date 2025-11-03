#include <rp6502.h>
#include <errno.h>
#include <string.h>

int __fastcall__ f_utime (const char* path, unsigned fdate, unsigned ftime, unsigned crdate, unsigned crtime)
{
    size_t pathlen;
    ria_set_ax (crtime);
    pathlen = strlen (path);
    if (pathlen > 255) {
        errno = EINVAL;
        return -1;
    }
    while (pathlen) {
        ria_push_char (path[--pathlen]);
    }
    ria_push_int (fdate);
    ria_push_int (ftime);
    ria_push_int (crdate);
    return ria_call_int (RIA_OP_UTIME);
}
