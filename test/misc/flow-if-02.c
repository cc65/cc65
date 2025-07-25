int a, b;

static int f1(void)
{
    if (0) {
        /* Unreachable but no warning */
    } else {
        a = 2;
    }
    return a;
}

static int f2(void)
{
    if (0) {
        /* Unreachable */
        a = 1;
    } else {
        a = 2;
    }
    return a;
}

static int f3(void)
{
    if (1) {
        a = 2;
    } else {
        /* Unreachable but no warning */
    }
    return a;
}

static int f4(void)
{
    if (1) {
        a = 2;
    } else {
        /* Unreachable */
        a = 1;
    }
    return a;
}

int main(void)
{
    return f1() + f2() + f3() + f4();
}

