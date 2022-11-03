
/* bug #1888 - The compiler doesn't accept valid data declarations */

/* The following is a valid declaration but not accepted by the compiler */
int static a;

int main(void)
{
    return 0;
}
