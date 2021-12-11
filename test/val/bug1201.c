
/* bug #1201 - The unary operators +, - and ~ should do integer promote on the result types. */

char a;
short b;
int c;
long d;
enum E {
    Z
} e;
struct S {
    int a : 1;
} f;

_Static_assert(sizeof(+a) == sizeof(int), "Result type should be int");
_Static_assert(sizeof(+b) == sizeof(int), "Result type should be int");
_Static_assert(sizeof(+c) == sizeof(int), "Result type should be int");
_Static_assert(sizeof(+d) == sizeof(long), "Result type should be long");
_Static_assert(sizeof(+e) == sizeof(int), "Result type should be int");
_Static_assert(sizeof(+f.a) == sizeof(int), "Result type should be int");

int main(void)
{
    return 0;
}
