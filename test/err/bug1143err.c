
/* bug #1143 - Multiple storage class specifiers in one declaration? */

static static void* y[1];   /* warning */
extern static int a;        /* error */
extern typedef int A;       /* error */

int main(void)
{
    return 0;
}
