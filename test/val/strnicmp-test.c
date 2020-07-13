#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>

int fails = 0;

static int do_test(const char *s1, const char *s2, size_t n)
{
    printf("strnicmp(\"%s\", \"%s\", %d):  ", s1, s2, (int)n);
    return strncasecmp(s1, s2, n);
}

static void printresult(int ret)
{
    if (ret) {
        printf("fail (%d)\n", ret);
        fails++;
    } else {
        printf("OK (%d)\n", ret);
    }
}

static void printresultgt(int ret)
{
    if (ret >= 0) {
        printf("fail (%d)\n", ret);
        fails++;
    } else {
        printf("OK (%d)\n", ret);
    }
}

static void printresultlt(int ret)
{
    if (ret <= 0) {
        printf("fail (%d)\n", ret);
        fails++;
    } else {
        printf("OK (%d)\n", ret);
    }
}

int main(void)
{
    int ret;

    ret = do_test("Wurzl", "wURZL", 5);
    printresult(ret);

    ret = do_test("Wurzl", "wURZL", 6);
    printresult(ret);

    ret = do_test("Wurzl", "wURZL", 10);
    printresult(ret);

    ret = do_test("Wurzla", "wURZLB", 10);
    printresultgt(ret);

    ret = do_test("Wurzla", "wURZLb", 5);
    printresult(ret);

    ret = do_test("BLI", "bla", 5);
    printresultlt(ret);

    ret = do_test("", "bla", 5);
    printresultgt(ret);

    ret = do_test("BLI", "", 5);
    printresultlt(ret);

    ret = do_test("", "", 5);
    printresult(ret);
    
    printf("fails: %d\n", fails);
    
#if defined(__CC65__) && !defined(__SIM6502__) && !defined(__SIM65C02__)
    cgetc();
#endif
    return fails;
}
