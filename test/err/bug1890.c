/* bug #1890 - Overflow in enumerator value is not detected */

#include <limits.h>
enum { a = ULONG_MAX, b } c = b;

int main(void)
{
    return 0;
}
