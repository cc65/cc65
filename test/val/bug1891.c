/* bug #1891 - backslash/newline sequence in string constants is treated wrong */

#include <stdio.h>
#include <string.h>

const char* a = "hello \
world";
const char* b = \
"hello world";

int main(void)
{
    if (strcmp(a, b) != 0) {
        printf("a:\n%s\n", a);
        printf("b:\n%s\n", b);
        return 1;
    }
    return 0;
}
