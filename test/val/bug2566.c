
/* Regression test for https://github.com/cc65/cc65/issues/2566
 *
 * The issue was introduced in an innocious-looking commit back in 2020:
 *
 * https://github.com/cc65/cc65/commit/c3a6b399456937093eda9994f19b7f722731528d
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    unsigned long Field1;
    unsigned long Field2;
} RecordType;

typedef struct {
    char dummy; // -- Offsets the Rec field by 1 byte.
    RecordType Rec;
} StructTypeA;

typedef struct {
    RecordType Rec;
} StructTypeB;

int main(void)
{
    StructTypeA A, *Aptr;
    StructTypeB B;
    bool ok;

    A.Rec.Field1 = 0x11111111;
    A.Rec.Field2 = 0x22222222;

    Aptr = &A;
    B.Rec = Aptr->Rec;

    /* These print statements give some clues as to what's going on. */
    /*
    printf("A.Rec: %lx, %lx\n", A.Rec.Field1, A.Rec.Field2);
    printf("B.Rec: %lx, %lx\n", B.Rec.Field1, B.Rec.Field2);
    */

    ok = (A.Rec.Field1 == B.Rec.Field1 && A.Rec.Field2 == B.Rec.Field2);

    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
