int a;

static int f1(void)
{
    for (;;) {
        ++a;
    }
    /* Unreachable */
    a = 2;
    return a;
}

static int f2(void)
{
    for (;1;) {
        ++a;
    }
    /* Unreachable */
    a = 2;
    return a;
}

static int f3(void)
{
    for (;;) {
        ++a;
        if (a == 5) break;
    }
    /* Reachable */
    a = 2;
    return a;
}

static int f4(void)
{
    for (;;) {
        ++a;
        return a;
    }
    /* Unreachable */
    a = 2;
}

static int f5(void)
{
    for (;0;) {
        /* Unreachable */
        ++a;
        return a;
    }
    /* Reachable */
    a = 2;
    return 0;
}

static int f6(void)
{
    for (;;) {
        ++a;
        if (a == 4) goto L;
        return a;
    }
    /* Reachable via L */
L:  a = 2;
}

static int f7(void)
{
    for (;0;) {
        /* Unreachable but no warning */
    }
    a = 2;
    return a;
}

static int f8(void)
{
    for (;a;) {
        return a;
    }
    /* Reachable */
    a = 2;
    return a;
}

int main(void)
{
    return f1() + f2() + f3() + f4() + f5() + f6() + f7() + f8();
}

