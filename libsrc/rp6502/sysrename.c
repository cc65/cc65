#include <rp6502.h>
#include <errno.h>
#include <string.h>

unsigned char __fastcall__ _sysrename (const char* oldpath, const char* newpath)
{
    size_t oldpathlen, newpathlen;
    oldpathlen = strlen (oldpath);
    newpathlen = strlen (newpath);
    if (oldpathlen + newpathlen > 254) {
        return _mappederrno (EINVAL);
    }
    while (oldpathlen) {
        ria_push_char (oldpath[--oldpathlen]);
    }
    ria_push_char (0);
    while (newpathlen) {
        ria_push_char (newpath[--newpathlen]);
    }
    return ria_call_int_errno (RIA_OP_RENAME);
}
