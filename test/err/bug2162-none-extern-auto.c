/* Bug #2162 - conflicting declarations in functions */

int main(void)
{
    extern int i;
    int i = 42;   /* Error */
    return i;
}
