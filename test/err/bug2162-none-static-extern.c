/* Bug #2162 - conflicting declarations in functions */

int main(void)
{
    static int i = 42;
    extern int i;      /* Error */
    return i;
}
