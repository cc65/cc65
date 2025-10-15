#include <rp6502.h>
#include <errno.h>
#include <string.h>

int __fastcall__ f_readdir (f_stat_t *dirent, int dirdes)
{
    int i, ax;
    ria_set_ax (dirdes);
    ax = ria_call_int (RIA_OP_READDIR);
    for (i = 0; i < sizeof (f_stat_t); i++) {
        ((char*)dirent)[i] = ria_pop_char ();
    }
    return ax;
}
