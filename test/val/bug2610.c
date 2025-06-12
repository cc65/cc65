#include <stdio.h>
#if '\x0A' != 0x0A
#error "Suspicious character set translation"
#endif
int main()
{
    char c = '\x0A';
    if (c == 0x0A) {
        printf("Ok\n");
        return 0;
    } else {
        printf("Failed\n");
        return 1;
    }
}
