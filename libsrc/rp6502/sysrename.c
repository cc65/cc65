#include <rp6502.h>
#include <errno.h>
#include <string.h>

int __fastcall__ _sysrename (const char* oldpath, const char* newpath)
{
    size_t oldpathlen, newpathlen;
    oldpathlen = strlen (oldpath);
    newpathlen = strlen (newpath);
    if (oldpathlen + newpathlen > 510) {
        errno = EINVAL;
        return -1;
    }
    while (oldpathlen) {
        ria_push_char (oldpath[--oldpathlen]);
    }
    ria_push_char (0);
    while (newpathlen) {
        ria_push_char (newpath[--newpathlen]);
    }
    return ria_call_int (RIA_OP_RENAME);
}
