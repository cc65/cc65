#include <rp6502.h>
#include <errno.h>
#include <string.h>

int __fastcall__ _sysremove (const char* name)
{
    size_t namelen;
    namelen = strlen (name);
    if (namelen > 255) {
        RIA.errno_ = EINVAL;
        return -1;
    }
    while (namelen) {
        ria_push_char (name[--namelen]);
    }
    return ria_call_int (RIA_OP_UNLINK);
}
