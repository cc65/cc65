
/* bug #1143 - Multiple storage class specifiers in one declaration? */

static static void* y[1];   /* warning */

int main(void)
{
    return 0;
}
