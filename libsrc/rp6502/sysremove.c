#include <rp6502.h>
#include <errno.h>
#include <string.h>

unsigned char __fastcall__ _sysremove (const char* name)
{
    size_t namelen;
    namelen = strlen (name);
    if (namelen > 255) {
        return _mappederrno (EINVAL);
    }
    while (namelen) {
        ria_push_char (name[--namelen]);
    }
    return ria_call_int_errno (RIA_OP_UNLINK);
}
