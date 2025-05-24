/* Bug 2304 - Visibility of objects/functions undeclared in file scope but 'extern'-declared in unrelated block scopes */

void f1(void)
{
    extern int a;
}

/* 'a' is still invisible in the file scope */

int main(void)
{
    return a * 0;   /* Usage of 'a' should be an error */
}

int a = 42;
