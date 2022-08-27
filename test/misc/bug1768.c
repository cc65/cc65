
#include <stdlib.h>

int a = 1 || (8 / 0);
int b = 0 && (8 % 0);
int c = 1 ? 42 : (0 % 0);
int d = 1 || a / 0;
int e = 0 && b % 0;
int f = 1 ? 42 : (a %= 0, b /= 0);

int main(void)
{
    return EXIT_SUCCESS;
}
