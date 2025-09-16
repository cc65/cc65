char a, b;

static int f1(void)
{
    switch (a) {
        case 1:         return 1;
        case 0xFF:      break;
        default:        return 2;
    }
    /* Reachable */
    return a;
}

static int f2(void)
{
    switch (a) {
        case 1:         return 1;
        case 0x100:     break;          /* Unreachable */
        default:        return 2;
    }
    /* Unreachable despite the "break" above */
    return a;
}

static int f3(void)
{

    switch (a) {
        case 1:
            if (b > 128) {
                a = 2;
                break;
            } else {
                a = 1;
            }
            return a;
        default:
            return 2;
    }
    /* Reachable */
    return a;
}

static int f4(void)
{

    switch (a) {
        case 1:
            if (b > 255) {
                /* Unreachable */
                a = 2;
                break;
            } else {
                a = 1;
            }
            return a;
        default:
            return 2;
    }
    /* Unreachable despite the "break" above */
    return a;
}

static int f5(void)
{

    switch (a) {
        case 1:
            if (b >= 0) {
                a = 2;
            } else {
                /* Unreachable */
                a = 1;
                break;
            }
            return a;
        default:
            return 2;
    }
    /* Unreachable despite the "break" above */
    return a;
}

static int f6(void)
{

    switch (a) {
        case 1:
            while (0) {
                /* Unreachable */
                if (b >= 128) {
                    a = 2;
                    break;
                }
            }
            return a;
        default:
            return 2;
    }
    /* Unreachable despite the "break" above */
    return a;
}

int main(void)
{
    return f1() + f2() + f3() + f4() + f5() + f6();
}
