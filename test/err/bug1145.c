
/* bug #1145 - Internal error with function type object */

void f()
{
    f = 0; /* internal error */
}

int main(void)
{
    f();
    return 0;
}
