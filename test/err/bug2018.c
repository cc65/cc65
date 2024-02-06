/* Bug #2018 - Compiler has problems with const struct fields */

struct X {
    struct {
        int a;
    } a;
    union {
        int a;
        const int b;
    } b;
};

struct X f(void)
{
    struct X x = { 42 };
    return x;
}

int main(void)
{
    struct X x = { 0 };
    x = f();            /* Error since X is read only */

    return 0;
}
