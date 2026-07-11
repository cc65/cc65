#include <rp6502.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>

int __cdecl__ ria_execl (const char* path, ...)
{
    va_list ap;
    const char* ptrs[16];
    unsigned int lens[16];
    int argc;
    unsigned int total_str;
    unsigned int offset;
    int i, j;
    const char* s;

    /* path becomes argv[0] */
    ptrs[0] = path;
    lens[0] = (unsigned int)strlen (path) + 1U;
    argc = 1;
    total_str = lens[0];

    /* Collect varargs: argv[1], argv[2], ..., NULL. */
    va_start (ap, path);
    for (;;) {
        s = va_arg (ap, const char*);
        if (s == NULL) {
            break;
        }
        if (argc >= 16) {
            va_end (ap);
            errno = EINVAL;
            return -1;
        }
        ptrs[argc] = s;
        lens[argc] = (unsigned int)strlen (s) + 1U;
        total_str += lens[argc];
        if (total_str > 512U) {
            va_end (ap);
            errno = EINVAL;
            return -1;
        }
        argc++;
    }
    va_end (ap);

    /* Int table: (argc+1) * 2 bytes */
    if ((unsigned int)(argc + 1) * 2U + total_str > 512U) {
        errno = EINVAL;
        return -1;
    }

    /* Push string data */
    for (i = argc - 1; i >= 0; --i) {
        for (j = (int)lens[i] - 1; j >= 0; --j) {
            ria_push_char (ptrs[i][j]);
        }
    }

    /* Push the int table */
    ria_push_int (0);
    offset = (unsigned int)(argc + 1) * 2U + total_str;
    for (i = argc - 1; i >= 0; --i) {
        offset -= lens[i];
        ria_push_int (offset);
    }

    return ria_call_int (RIA_OP_EXEC);
}
