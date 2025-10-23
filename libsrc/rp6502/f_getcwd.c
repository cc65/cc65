#include <rp6502.h>
#include <errno.h>
#include <string.h>

int __fastcall__ f_getcwd (char* name, int size)
{
    int i, ax;
    ax = ria_call_int (RIA_OP_GETCWD);
    if (ax > size) {
        RIA.op = RIA_OP_ZXSTACK;
        errno = ENOMEM;
        return -1;
    }
    for (i = 0; i < ax; i++) {
        name[i] = ria_pop_char ();
    }
    return ax;
}
