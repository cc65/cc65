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
    /* Unreachable but no warning */
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
    /* Unreachable but no warning */
    return a;
}

int main(void)
{
    return f1() + f2() + f3() + f4();
}
