/* bug #3041 - By-ref struct member access after assignment is an lvalue */

struct foo { char a, b, c, d; };

void f(void)
{
    struct foo s1, s2;
    /* Error: Assignment to rvalue */
    (s1 = s2).b += 1;
}
