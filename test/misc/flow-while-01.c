int a;

static int f1(void)
{
    while (1) {
        ++a;
    }
    /* Unreachable */
    a = 2;
    return a;
}

static int f2(void)
{
    while (1) {
        ++a;
        if (a == 5) break;
    }
    /* Reachable */
    a = 2;
    return a;
}

static int f3(void)
{
    while (1) {
        ++a;
        return a;
    }
    /* Unreachable */
    a = 2;
}

static int f4(void)
{
    while (0) {
        /* Unreachable */
        ++a;
        return a;
    }
    /* Reachable */
    a = 2;
    return 0;
}

static int f5(void)
{
    while (1) {
        ++a;
       	if (a == 4) goto L;
        return a;
    }
    /* Reachable via L */
L:  a = 2;
}

static int f6(void)
{
    while (0) { 
	/* Unreachable but no warning */
    }
    a = 2;
    return a;
}

int main(void)
{
    return f1() + f2() + f3() + f4() + f5() + f6();
}

