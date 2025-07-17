int a;

static int f1(void)
{
    do {
        return a;
    } while (a == 2);
    /* Unreachable */
    a = 2;
    return a;
}

static int f2(void)
{
    do {
        return a;
    } while (1);
    /* Unreachable */
    a = 2;
    return a;
}

static int f3(void)
{
    do {
        return a;
    } while (0);
    /* Unreachable */
    a = 2;
    return a;
}

static int f4(void)
{
    do {
        continue;       /* Turns do/while into an endless loop */
    } while (0);
    /* Unreachable */
    a = 2;
    return a;
}

static int f5(void)
{
    do {
       	if (a == 2) {
            break;
        }
        return a;
    } while (0);
    /* Reachable */
    a = 2;
    return a;
}

static int f6(void)
{
    do {
       	if (a == 2) {
            break;
        }
        continue;
    } while (0);
    /* Reachable */
    a = 2;
    return a;
}

static int f7(void)
{
    do {
       	if (a == 2) {
            return a;
        } else {
            continue;
	}
    } while (1);
    /* Unreachable */
    a = 2;
    return a;
}

int main(void)
{
    return f1() + f2() + f3() + f4() + f5() + f6() + f7();
}

