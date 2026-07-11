#include <rp6502.h>

long __fastcall__ ria_attr_get (unsigned char id)
{
    ria_set_a (id);
    return ria_call_long (RIA_OP_ATTR_GET);
}

int __fastcall__ ria_attr_set (long val, unsigned char id)
{
    ria_set_a (id);
    ria_push_long (val);
    return ria_call_int (RIA_OP_ATTR_SET);
}
