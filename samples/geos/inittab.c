/* Note:
** This is just a sample piece of code that shows how to use some structs -
** it may not even run.
*/


#include <geos.h>


static const void myTab = {
        0xd020, (char)2,
        (char)0, (char)2,
        0x4000, (char)5,
        (char)0, (char)1, (char)2, (char)3, (char)4,
        0x0000
        };

int main (void)
{
    InitRam(&myTab);
}

