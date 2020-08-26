
/* bug #1113 - Compiler crashes when calling functions "redefined" as other types */

void f() {}

int f;

int main(void)
{
    f();
    return 0;
}
