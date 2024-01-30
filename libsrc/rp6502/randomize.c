#include <rp6502.h>
#include <stdlib.h>

void _randomize (void)
{
    srand (ria_call_int (RIA_OP_LRAND));
}
