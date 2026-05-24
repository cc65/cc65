#include <rp6502.h>
#include <errno.h>
#include <string.h>

int __fastcall__ ria_rln_poke (const char* poke)
{
    int i;
    size_t pokelen = strlen (poke);
    if (pokelen > 254) {
        errno = EINVAL;
        return -1;
    }
    ria_push_char (0);
    for (i = pokelen; i--; ) {
        ria_push_char (poke[i]);
    }
    return ria_call_int (RIA_OP_RLN_POKE);
}
