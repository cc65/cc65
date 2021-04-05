/* Bug #1451 - local struct field access via the address of the struct */

#include <stdio.h>

typedef struct {
    int a;
    int b;
} S;

int failures = 0;

int main(void)
{
    S a = {2, 5};
    S b = {1, 4};
    S m[1] = {{6, 3}};
    S *p = &a;
	
    (&a)->a += b.a;
    p->b += b.b;
    m->a += b.a;

    if ((&a)->a != 3) {
        ++failures;
        printf("Expected 3, got %d\n", (&a)->a);
    }

    if (p->b != 9) {
        ++failures;
        printf("Expected 9, got %d\n", p->b);
    }

    if (m->a != 7) {
        ++failures;
        printf("Expected 7, got %d\n", m->a);
    }

    return failures;
}
