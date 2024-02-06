/* Demonstrates that floating point constants are allowed in a limited way.
   Value will be converted to an int, with a warning if precision is lost. */

int a = 3.0;
int b = 23.1;
int c = -5.0;

int main(void)
{
    if (a != 3) return 1;
    if (b != 23) return 2;
    if (c != -5) return 3;
    return 0;
}
