#include <rp6502.h>

int __fastcall__ ria_rln_peek (char* peek, unsigned char* pos)
{
    int i, ax;
    ax = ria_call_int (RIA_OP_RLN_PEEK);
    if (ax > 0) {
        *pos = ria_pop_char ();
        for (i = 0; i < ax; i++) {
            peek[i] = ria_pop_char ();
        }
    } else {
        *pos = 0;
    }
    peek[ax] = 0;
    return ax;
}
