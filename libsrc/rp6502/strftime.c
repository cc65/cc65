#include <rp6502.h>
#include <errno.h>
#include <string.h>
#include <time.h>

size_t __fastcall__ strftime (char* buf, size_t bufsize, const char* format,
    const struct tm* tm)
{
    int ax;
    unsigned i;
    size_t len = strlen (format);
    if (bufsize == 0) {
        return 0;
    }
    if (len > 255) {
        errno = EINVAL;
        return 0;
    }
    for (i = sizeof (struct tm); i; ) {
        ria_push_char (((const char*)tm)[--i]);
    }
    ria_push_char ('\0');
    while (len) {
        ria_push_char (format[--len]);
    }
    ax = ria_call_int (RIA_OP_STRFTIME);
    if (ax < 0) {
        return 0;               /* errno set by OS */
    }
    if ((unsigned)ax >= bufsize) {
        RIA.op = RIA_OP_ZXSTACK;
        return 0;               /* did not fit, per ISO C */
    }
    for (i = 0; i < (unsigned)ax; ++i) {
        buf[i] = ria_pop_char ();
    }
    buf[ax] = '\0';
    return ax;
}
