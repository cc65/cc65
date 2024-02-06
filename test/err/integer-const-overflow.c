/* Integer constant overflow warnings. */

/* Warnings as errors. */
#pragma warn(error,on)

/* Warn on const overflow */
#pragma warn(const-overflow,on)

unsigned char      a = 256;
signed char        b = 128;
unsigned char      c = -129;
unsigned short int d = 0x00010000;
unsigned short int e = 0x80000000;
signed short int   f = 32768L;
signed short int   g = -32769L;

int main(void)
{
    return 0;
}
