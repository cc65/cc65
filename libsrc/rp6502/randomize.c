#include <rp6502.h>
#include <stdlib.h>

void _randomize (void)
{
    srand (ria_attr_get (RIA_ATTR_LRAND));
}
