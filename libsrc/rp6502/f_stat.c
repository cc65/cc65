#include <rp6502.h>
#include <errno.h>
#include <string.h>

int __fastcall__ f_stat (const char* path, f_stat_t* dirent)
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
    ax = ria_call_int (RIA_OP_STAT);
    for (i = 0; i < sizeof (f_stat_t); i++) {
        ((char*)dirent)[i] = ria_pop_char ();
    }
    return ax;
}
