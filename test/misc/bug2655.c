void f1(void)
{
    if (1) {
        f1();
    } else {
        f1();
    }
}

void f2(void)
{
    if (0) {
        f2();
    } else {
        f2();
    }
}

