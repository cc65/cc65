int a;

static int f1(void)
{
    switch (a) {
        /* Unreachable */
        a = 3;
        case 1:
            a = 2;
            break;
        case 2:
            a = 1;
            break;
        default:
            a = 0;
            break;
    }
    /* Reachable */
    return a;
}

static int f2(void)
{
    switch (a) {
        /* Reachable */
L:      a = 3;
        case 1:
            goto L;
        case 2:
            a = 1;
            break;
        default:
            a = 0;
            break;
    }
    /* Reachable */
    return a;
}

static int f3(void)
{
    switch (a) {
        case 1:         return a;
        case 2:         return a+1;
        default:        return a+2;
    }
    /* Unreachable */
    return a;
}

static int f4(void)
{
    switch (a) {
        /* No warning */
        do {
            case 1:         ++a; continue;
            case 2:         return a+1;
            default:        return a+2;
        } while (1);
    }
    /* Unreachable */
    return a;
}

static int f5(void)
{
    do {
        switch (a) {
            case 1:     return a;
            case 2:     ++a; continue;
            default:    return 1;
        }
    } while (0);
    /* Unreachable */
    return 2;
}

static int f6(void)
{
    do {
L:      switch (a) {
            case 1:     return a;
            case 2:     ++a; goto L;
            default:    return 1;
        }
    } while (0);
    /* Unreachable but no warning because of "goto" */
    return 2;
}

static int f7(void)
{
    do {
        switch (a) {
            /* Unreachable */
            a = 3;
            case 1:     return a;
            case 2:     ++a; continue;
            default:    return 1;
        }
    } while (0);
    /* Unreachable but no warning because of weird switch */
    return 2;
}

static void duff(char* to, char* from, unsigned count)
{
    unsigned n = (count+7)/8;
    switch (count%8) {
        case 0: do {    *to = *from++;
        case 7:         *to = *from++;
        case 6:         *to = *from++;
        case 5:         *to = *from++;
        case 4:         *to = *from++;
        case 3:         *to = *from++;
        case 2:         *to = *from++;
        case 1:         *to = *from++;
        } while (--n>0);
    }
}

int main(void)
{
    char x[11];
    char y[11];
    duff(x, y, 11);
    return f1() + f2() + f3() + f4() + f5() + f6() + f7();
}
