/* bug #1893 - Compiler accepts a ternary expression where it shouldn't */

int main(void)
{
    int a, b, c;
    a == 1? b : c = 3;
    return 0;
}
