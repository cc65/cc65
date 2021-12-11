/* pr #1423 - Codegen fix for certain cases of object addresses as boolean */

unsigned char fails = 0;

void test1(void)
{
    int a;
    while (&a) {
        return;
    }
    fails++;
    return;
} 

void test2(void)
{
    int a;
    do {
        return;
    } while (&a);
    fails++;
    return;
}

void test3(void)
{
    int a;
    for (;&a;) {
        return;
    }
    fails++;
    return;
}

int main(void)
{
    test1();
    test2();
    test3();
    return fails;
}
