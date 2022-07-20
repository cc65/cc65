
// test minimal float support.
// this should test everything that does NOT require linking a library
// - float literals in code are recognized as such
// - float variables are converted into the float binary format
// - taking address from float and assigning via pointer works

#ifdef CONIO
#include <conio.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <_float.h>

float fp1 = 42.01002f;
float fp2;  // non initialized
float *fp_p; // non initialized

uintptr_t p;

float fp3;
float fp4 = 23.12f;

char buf[0x10];

signed char var_schar;
unsigned char var_uchar;
signed int var_sint;
unsigned int var_uint;
signed long var_slong;
unsigned long var_ulong;

int main(void)
{
    float fp2 = 23.1234f;

    printf("float-minimal\n");

    printf("fp2:0x%08lx [0x41b8f5c3] %s (23.1234)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    printf("fp4:0x%08lx [0x41b8f5c3] %s (23.12)\n", *((uint32_t*)&fp4), _ftostr(buf, fp4));
#if 1
    printf("(global) get address, read via ptr\n");
    // get address of global (works)
    p = (uintptr_t)&fp1;
    fp_p = (float*)p;
    // read fp via pointer and assign local
    fp2 = *fp_p;

    printf("fp1:0x%08lx [0x42280a43] %s (42.01002)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("fp2:0x%08lx [0x42280a43] %s (42.01002)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
#endif
#if 1
    printf("(local) get address, read via ptr\n");
    fp2 = 23.1234f;

    // get address of local
    p = (uintptr_t)&fp2;
    fp_p = (float*)p;
    // read fp via pointer and assign global
    fp3 = *fp_p;

    printf("fp2:0x%08lx [] %s (23.1234)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    printf("fp3:0x%08lx [] %s (23.1234)\n", *((uint32_t*)&fp3), _ftostr(buf, fp3));
#endif
    // addition
#if 1
    printf("constant + constant\n");
    fp1 = 0.1f;
    fp2 = 0.2f;
    printf("    0x%08lx [] %s (0.1)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("    0x%08lx [] %s (0.2)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));

    fp3 = 0.1f + 0.2f;
    printf("fp3:0x%08lx [] %s (0.3)\n", *((uint32_t*)&fp3), _ftostr(buf, fp3));
#endif
    // substraction
#if 0
    printf("constant - constant\n");
    fp1 = 0.1f;
    fp2 = 0.2f;
    fp3 = 0.1f - 0.2f; //FIXME: Invalid operands for binary operator '-'

    printf("    0x%08lx [] %s (0.1)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("    0x%08lx [] %s (0.2)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    printf("fp3:0x%08lx [] %s (0.3)\n", *((uint32_t*)&fp3), _ftostr(buf, fp3));
#endif
    // multiplication
#if 0
    printf("constant * constant\n");
    fp1 = 0.1f;
    fp2 = 0.2f;
    fp3 = 0.1f * 0.2f; // FIXME: Precondition violated: IsClassInt (T), file 'cc65/datatype.c', line 1008

    printf("    0x%08lx [] %s (0.1)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("    0x%08lx [] %s (0.2)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    printf("fp3:0x%08lx [] %s (0.3)\n", *((uint32_t*)&fp3), _ftostr(buf, fp3));
#endif

    // division
#if 0
    printf("constant / constant\n");
    fp1 = 0.1f;
    fp2 = 0.2f;
    fp3 = 0.1f / 0.2f; // FIXME: Precondition violated: IsClassInt (T), file 'cc65/datatype.c', line 1008

    printf("    0x%08lx [] %s (0.1)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("    0x%08lx [] %s (0.2)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    printf("fp3:0x%08lx [] %s (0.3)\n", *((uint32_t*)&fp3), _ftostr(buf, fp3));
#endif
#ifdef CONIO
    cgetc();
#endif

    // comparisons
#if 1
    /* FIXME: this does not work yet */
    printf("0.1f == 0.1f is "); if (0.1f == 0.1f) { printf("true\n"); } else { printf("false\n"); }
    printf("0.2f == 0.1f is "); if (0.2f == 0.1f) { printf("true\n"); } else { printf("false\n"); }
    printf("0.1f != 0.1f is "); if (0.1f != 0.1f) { printf("true\n"); } else { printf("false\n"); }
    printf("0.2f != 0.1f is "); if (0.2f != 0.1f) { printf("true\n"); } else { printf("false\n"); }
#endif

#ifdef CONIO
    cgetc();
#endif

    // conversions
#if 1
    printf("conversions (integer constant to float)\n");
    fp1 = -12;
    fp2 = 199;
    printf("fp1 0x%08lx [] %s (-12)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("fp2 0x%08lx [] %s (199)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    fp1 = -4711;
    fp2 = 42000;
    printf("fp1 0x%08lx [] %s (-4711)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("fp2 0x%08lx [] %s (42000)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
#endif
#if 1
    printf("conversions (float constant to integer)\n");
    var_schar = (signed char)12.3f;
    printf("%s (12.3) schar:%d (12)\n", _ftostr(buf, 12.3f), (int)var_schar);
    var_uchar = (unsigned char)19.9f;
    printf("%s (19.9) uchar:%u (19)\n", _ftostr(buf, 19.9f), (int)var_uchar);

    var_sint = (signed short)1234.5f;
    printf("%s (1234.5) sint:%d (1234)\n", _ftostr(buf, 1234.5f), var_sint);
    var_uint = (unsigned short)1999.9f;
    printf("%s (1999.9) uint:%u (1999)\n", _ftostr(buf, 1999.9f), var_uint);

    var_slong = (signed long)123456.5f;
    printf("%s (123456.5f) slong:%ld (123456)\n", _ftostr(buf, 123456.5f), var_slong);
    var_ulong = (unsigned long)199988.9f;
    printf("%s (199988.9) ulong:%lu (199988)\n", _ftostr(buf, 199988.9f), var_ulong);
#endif
    return 0;
}
