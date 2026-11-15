/* bug #3047 - Deferred post-inc/dec operations produce an lvalue */

int i;

void f(void)
{
    /* Error: Assignment to rvalue */
    i++ = 42;
}
