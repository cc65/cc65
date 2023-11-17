#include <rp6502.h>
#include <stdlib.h>

// Non-standard cc65
void _randomize (void)
{
    srand (ria_call_int (RIA_OP_LRAND));
}
