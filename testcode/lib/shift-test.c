#include <stdio.h>
#include <stdlib.h>




static unsigned UnsignedShiftLeft1 (unsigned Val)
/* Shift an unsigned left by 1 */
{
    __AX__ = Val;
    asm ("stx tmp1");
    asm ("asl a");
    asm ("rol tmp1");
    asm ("ldx tmp1");
    return __AX__;
}



static unsigned UnsignedShiftRight1 (unsigned Val)
/* Shift an unsigned right by 1 */
{
    __AX__ = Val;
    asm ("stx tmp1");
    asm ("lsr tmp1");
    asm ("ror a");
    asm ("ldx tmp1");
    return __AX__;
}



static int SignedShiftRight1 (int Val)
/* Shift a signed right by 1 */
{
    __AX__ = Val;
    asm ("stx tmp1");
    asm ("cpx #$80");
    asm ("ror tmp1");
    asm ("ror a");
    asm ("ldx tmp1");
    return __AX__;
}



static void TestUnsignedLeftShift (void)
/* Test left shift. This is identical for signed and unsigned ints */
{
    unsigned L, R, V;
    printf ("Testing unsigned left shift:\n");
    L = 0;
    do {
        V = L;
        for (R = 0; R < 16; ++R) {
            /* Check it */
            if ((L << R) != V) {
                fprintf (stderr,
                         "Failed: %u << %u != %u (%u)\n",
                         L, R, V, L << R);
                exit (1);
            }
            V = UnsignedShiftLeft1 (V);
        }
        if ((L & 0xFF) == 0) {
            printf ("%04X ", L);
        }
    } while (++L != 0);
    printf ("\n");
}



static void TestUnsignedRightShift (void)
/* Test unsigned right shift. */
{
    unsigned L, R, V;
    printf ("Testing unsigned right shift:\n");
    L = 0;
    do {
        V = L;
        for (R = 0; R < 16; ++R) {
            /* Check it */
            if ((L >> R) != V) {
                fprintf (stderr,
                         "Failed: %u >> %u != %u (%u)\n",
                         L, R, V, L >> R);
                exit (1);
            }
            V = UnsignedShiftRight1 (V);
        }
        if ((L & 0xFF) == 0) {
            printf ("%04X ", L);
        }
    } while (++L != 0);
    printf ("\n");
}



static void TestSignedRightShift (void)
/* Test signed right shift. */
{
    int L, R, V;
    printf ("Testing signed right shift:\n");
    L = 0;
    do {
        V = L;
        for (R = 0; R < 16; ++R) {
            /* Check it */
            if ((L >> R) != V) {
                fprintf (stderr,
                         "Failed: %d >> %d != %d (%d)\n",
                         L, R, V, L >> R);
                exit (1);
            }
            V = SignedShiftRight1 (V);
        }
        if ((L & 0xFF) == 0) {
            printf ("%04X ", L);
        }
    } while (++L != 0);
    printf ("\n");
}



int main (void)
{
    TestUnsignedLeftShift ();
    TestUnsignedRightShift ();
    TestSignedRightShift ();
    printf ("\nOk!\n");

    return 0;
}
