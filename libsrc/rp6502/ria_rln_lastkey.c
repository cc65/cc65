#include <rp6502.h>

int __fastcall__ ria_rln_lastkey (char* key, unsigned char* action)
{
    int i, ax;
    ax = ria_call_int (RIA_OP_RLN_LASTKEY);
    if (ax > 0) {
        *action = ria_pop_char ();
        for (i = 0; i < ax; i++) {
            key[i] = ria_pop_char ();
        }
    }
    return ax;
}
