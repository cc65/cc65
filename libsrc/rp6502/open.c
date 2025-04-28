#include <rp6502.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

int __cdecl__ open (const char* name, int flags, ...)
{
    size_t namelen = strlen (name);
    if (namelen > 255) {
        RIA.errno = EINVAL;
        return _mappederrno(RIA.errno);
    }
    while (namelen) {
        ria_push_char (name[--namelen]);
    }
    ria_set_ax (flags);
    return ria_call_int_errno (RIA_OP_OPEN);
}
