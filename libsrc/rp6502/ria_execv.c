#include <rp6502.h>
#include <errno.h>
#include <string.h>

int __fastcall__ ria_execv (const char* path, char* const argv[])
{
    const char* ptrs[16];
    unsigned int lens[16];
    int argc;
    unsigned int total_str;
    unsigned int offset;
    int i, j;

    /* path becomes argv[0] */
    ptrs[0] = path;
    lens[0] = (unsigned int)strlen (path) + 1U;
    argc = 1;
    total_str = lens[0];

    /* Collect argv[0], argv[1], ... */
    i = 0;
    while (argv[i] != NULL) {
        if (argc >= 16) {
            errno = EINVAL;
            return -1;
        }
        ptrs[argc] = argv[i];
        lens[argc] = (unsigned int)strlen (argv[i]) + 1U;
        total_str += lens[argc];
        if (total_str > 512U) {
            errno = EINVAL;
            return -1;
        }
        argc++;
        i++;
    }

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
