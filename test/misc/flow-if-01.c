int a, b;

static int f1(void)
{
    if (a == 1) {
        return 1;
    }
    /* Reachable */
    a = 2;
    return a;
}

static int f2(void)
{
    if (a == 1) {
        a = 2;
    } else {
        return 1;
    }
    /* Reachable */
    return a;
}

static int f3(void)
{
    if (a == 1) {
        return 1;
    } else {
        a = 2;
    }
    /* Reachable */
    return a;
}

static int f4(void)
{
    if (a == 1) {
        return 1;
    } else {
        return 0;
    }
    /* Unreachable */
    a = 2;
}

static int f5(void)
{
    if (1) {
        return 1;
    } else {
        /* Unreachable */
        return 0;
    }
    /* Unreachable */
    a = 2;
}

int main(void)
{
    return f1() + f2() + f3() + f4() + f5();
}

