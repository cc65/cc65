/* bug #3041 - By-ref struct assignment is an lvalue */

struct foo { char z[3]; };

void f(void)
{
    struct foo s1, s2, s3;
    /* Error: Assignment to rvalue */
    (s1 = s2) = s3;
}
