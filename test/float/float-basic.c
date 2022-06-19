
// test basic arithmetic operations
// WIP WIP WIP

#ifdef CONIO
#include <conio.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <_float.h>

float fp1 = 12.34f;
float fp2;  // non initialized
float fp3, fp4 = 55.55f;

char buf[0x10];
char buf2[0x10];
char buf3[0x10];

unsigned long l1,l2;

signed char var_schar;
unsigned char var_uchar;
signed int var_sint;
unsigned int var_uint;
signed long var_slong;
unsigned long var_ulong;

int main(void)
{
    float fp2 = 43.21f;

    printf("float-basic\n");
    printf("fp1:0x%08lx [0x414570a4] %s (12.340000)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("fp2:0x%08lx [0x422cd70a] %s (43.209999)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));

    /* addition, variable + variable */
    printf("addition, variable + variable\n");
    fp1 = 12.34f;
    fp2 = 43.21f;
    fp3 = fp1 + fp2;
    printf("%s+%s=%s\n", _ftostr(buf, fp1), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
    printf("fp3:0x%08lx [0x425e3333] %s (55.549999)\n", *((uint32_t*)&fp3), _ftostr(buf, fp3));

    /* addition, variable + constant */
    printf("addition, variable + constant\n");
    fp3 = 55.549999f;
    fp1 = fp3 + 0.05f;
    printf("%s+%s=%s\n", _ftostr(buf, fp3), _ftostr(buf3, 0.05f), _ftostr(buf2, fp1));
    printf("fp3:0x%08lx [0x425e3333] %s\n", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    printf("fp1:0x%08lx [0x425e6666] %s\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
#if 1
    /* addition, constant + variable */
    printf("addition, constant + variable\n");
    fp2 = 43.21f;
    fp1 = 12.7f + fp2;  // FIXME: wrong, the add is dropped?
    printf("%s+%s=%s\n", _ftostr(buf3, 12.7f), _ftostr(buf, fp2), _ftostr(buf2, fp1));
    printf("fp3:0x%08lx [0x425e3333] %s\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    printf("fp1:0x%08lx [0x425e6666] %s\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
#endif
    /* addition, constant + constant (already tested by minimal) (omitted) */
#ifdef CONIO
    cgetc();
#endif

#if 0
    /* substraction, variable - constant */
    printf("substraction, variable - constant\n");
    fp1 = 12.34;
    fp3 = fp1 - 11.5f;  // FIXME: Invalid operands for binary operator '-'
    printf("%s-%s=%s\n", _ftostr(buf, fp1), _ftostr(buf2, 0x11.5f), _ftostr(buf3, fp3));
    printf("fp3:0x%08lx [] %s ()\n", *((uint32_t*)&fp3), _ftostr(buf, fp3));
#endif

#if 1
    /* substraction, constant - variable */
    printf("substraction, constant - variable\n");
    fp2 = 12.34;
    fp3 = 11.5f - fp2;
    printf("%s-%s=%s\n", _ftostr(buf, 11.5f), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
    printf("fp3:0x%08lx [] %s ()\n", *((uint32_t*)&fp3), _ftostr(buf, fp3));
#endif

#if 1
    /* substraction, variable - variable */
    printf("substraction, variable - variable\n");
    fp3 = fp1 - fp2;
    printf("%s-%s=%s\n", _ftostr(buf, fp1), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
    printf("fp3:0x%08lx [0x41463d70] %s (12.389999)\n", *((uint32_t*)&fp3), _ftostr(buf, fp3));
#endif

#if 1
    printf("conversions (integer variable to float)\n");
    var_schar = -12;
    fp1 = var_schar;
    var_uchar = 199;
    fp2 = var_uchar;
    printf("fp1 0x%08lx [] %s (-12)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("fp2 0x%08lx [] %s (199)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    var_sint = -4711;
    fp1 = var_sint;
    var_uint = 42000;
    fp2 = var_uint;
    printf("fp1 0x%08lx [] %s (-4711)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("fp2 0x%08lx [] %s (42000)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    var_slong = -4711456;
    fp1 = var_slong;
    var_ulong = 42000456;
    fp2 = var_ulong;
    printf("fp1 0x%08lx [] %s (-4711456)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("fp2 0x%08lx [] %s (42000456)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
#ifdef CONIO
    cgetc();
#endif
#endif

    printf("conversions (float variable to integer)\n");
    fp1 = -12.3f;
    var_schar = (signed char)fp1;
    printf("fp1 0x%08lx %s (12.3) schar:%d (12)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1), (int)var_schar);
    fp2 = 19.9f;
    var_uchar = (unsigned char)fp2;
    printf("fp2 0x%08lx %s (19.9) uchar:%u (19)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2), (int)var_uchar);

    fp1 = 1234.5f;
    var_sint = (signed short)fp1;
    printf("fp1 0x%08lx %s (1234.5) sint:%d (1234)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1), var_sint);
    fp2 = 1999.9f;
    var_uint = (unsigned short)fp2;
    printf("fp2 0x%08lx %s (1999.9) uint:%u (1999)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2), var_uint);

    fp1 = 123456.5f;
    var_slong = (signed long)fp1;
    printf("fp1 0x%08lx %s (1234.5) slong:%ld (1234)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1), var_slong);
    fp2 = 199988.9f;
    var_ulong = (unsigned long)fp2;
    printf("fp2 0x%08lx %s (1999.9) ulong:%lu (1999)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2), var_ulong);

#if 1
    printf("multiplication, variable * variable\n");
    fp1 = 25.2f;
    fp2 = 2.3f;
    fp3 = fp1 * fp2;
    printf("%s*%s=%s\n", _ftostr(buf, fp1), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
#endif

#if 1
    printf("division, variable / variable\n");
    fp1 = 25.2f;
    fp2 = 2.3f;
    fp3 = fp1 / fp2;
    printf("%s/%s=%s\n", _ftostr(buf, fp1), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
#endif

// FIXME: does not compile
#if 0
    fp1 = (fp2 == 2.5f) ? 1.5f : 0.5f;
#endif

    // NOT
    printf("binary negate (not)\n");
    fp1 = 12.6f;
    fp2 = !fp1;
    var_sint = !fp1;
    fp3 = !fp2;
    printf("fp2 0x%08lx [] %s () %d\n", *((uint32_t*)&fp2), _ftostr(buf, fp2), var_sint);
    printf("fp3 0x%08lx [] %s ()\n", *((uint32_t*)&fp3), _ftostr(buf, fp3));

    fp1 = -12.6f;
    fp1 = 0.0f - fp1;
    fp2 = !fp1;
    var_sint = !fp1;
    fp3 = !fp2;
    printf("fp2 0x%08lx [] %s () %d\n", *((uint32_t*)&fp2), _ftostr(buf, fp2), var_sint);
    printf("fp2 0x%08lx [] %s ()\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    printf("fp3 0x%08lx [] %s ()\n", *((uint32_t*)&fp3), _ftostr(buf, fp3));

    
    return 0;
}
