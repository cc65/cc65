/* bug #1221 - Structs/unions as ternary operands */

int a;
struct S { int a; } s1, s2;
struct U { int a; } u1, u2;

int main()
{
    a ? s1 : s2; /* BUG: should be OK */
    a ? u1 : u2; /* BUG: should be OK */
    return 0;
}
