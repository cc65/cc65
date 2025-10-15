#include <rp6502.h>

int __fastcall__ f_rewinddir (int dirdes)
{
    ria_set_ax (dirdes);
    return ria_call_int (RIA_OP_REWINDDIR);
}
