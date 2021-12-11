
/* pr #1425 - Ternary fixes */

unsigned char fails = 0;

void test1(void)
{
    int x = 0;
    x ? (void)x-- : (void)1;
    if (x != 0) {
        fails++;
    }
}

int test2(void)
{
    int x = 0, y = 0;
    x ? (void)x--, (void)y++ : (void)1;
    if (x != 0) {
        fails++;
    }
    if (y != 0) {
        fails++;
    }
}

void test3(void)
{
    int x = 0, y = 0;
    x ? ((void)x--, (void)y++) : (void)1;
    if (x != 0) {
        fails++;
    }
    if (y != 0) {
        fails++;
    }
}

int main(void)
{
    test1();
    test2();
    test3();
    return fails;
}
