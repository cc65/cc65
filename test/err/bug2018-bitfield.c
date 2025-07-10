/* Bug #2018 - Compiler has problems with const struct fields */

typedef union U {
    int a : 16;
    const int b : 16;
} U;

int main(void)
{
    U x = { 42 };
    x.b = 0;

    return 0;
}
