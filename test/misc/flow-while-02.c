int a;

static int f1(void)
{
    while (1) {
        while (0) {
            /* Unreachable */
            ++a;
        }
    }
    /* Unreachable */
    a = 2;
    return a;
}

static int f2(void)
{
    while (1) {
        do {
            return a;
        } while (0);
        /* Unreachable */
        break;
    }
    /* Unreachable but no warning */
    a = 2;
    return a;
}

static int f3(void)
{
    do {
        while (1) {
            break;
        }
    } while (1);
    /* Unreachable */
    a = 2;
    return a;
}

static int f4(void)
{
    do {
        while (1) {
            return a;
        }
    } while (0);
    /* Unreachable */
    a = 2;
    return a;
}

static int f5(void)
{
    do {
        do {
            if (a == 2) {
                return a;
            } else {
                continue;
            }
        } while (0);
    } while (0);
    /* Unreachable */
    a = 2;
    return a;
}

int main(void)
{
    return f1()/ + f2() + f3() + f4() + f5();
}

