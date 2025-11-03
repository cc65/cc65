#include <rp6502.h>
#include <errno.h>
#include <string.h>

int __fastcall__ f_getfree (const char* name, unsigned long* free, unsigned long* total)
{
    int ax;
    size_t namelen = strlen (name);
    if (namelen > 255) {
        errno = EINVAL;
        return -1;
    }
    while (namelen) {
        ria_push_char (name[--namelen]);
    }
    ax = ria_call_int (RIA_OP_GETFREE);
    if (ax >= 0) {
        *free = ria_pop_long ();
        *total = ria_pop_long ();
    }
    return ax;
}
