#include <rp6502.h>

long __fastcall__ lrand (void)
{
    return ria_attr_get (RIA_ATTR_LRAND);
}
