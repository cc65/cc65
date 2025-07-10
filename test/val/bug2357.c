/* bug #2357 - Compiler produces invalid code after d8a3938
*/

unsigned long test;

unsigned long longarray[7];

void jsr_threebytes(void) {

}

/* having replaced two sty $zp with two sta $abs, but forgetting
 * to update the instruction size, coptlong.c could cause a build
 * error "Error: Range error (131 not in [-128..127])" if the
 * computed codesize was under 126, but the real codesize was above
 * 127.
 * This tests verifies that the bug is fixed.
 */
unsigned char __fastcall__ foo (unsigned char res)
{
    if (res == 0) {
      longarray[1]=test; /* 24 bytes  - but the compiler thought 22 */
      longarray[2]=test; /* 48 bytes  - but 44 */
      longarray[3]=test; /* 72 bytes  - 66 */
      longarray[4]=test; /* 96 bytes  - 88 */
      longarray[6]=test; /* 120 bytes - 110 */
      jsr_threebytes();  /* 123       - 113 */
      jsr_threebytes();  /* 126       - 116 */
      jsr_threebytes();  /* 129       - 119 */
    }
    return 0;
}

int main (void)
{
    foo(42);
    return 0;
}
