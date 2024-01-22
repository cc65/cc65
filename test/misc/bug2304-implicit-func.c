/* Bug 2304 - Visibility of objects/functions undeclared in file scope but 'extern'-declared in unrelated block scopes */

/* This one should fail even in C89 */

void f1(void)
{
    extern unsigned int f();
}

/* 'f' is still invisible in the file scope */

int main(void)
{
    f();        /* Should be a conflict since the implicit function type is incompatible */
    return 0;
}

unsigned int f()
{
    return 42;
}
