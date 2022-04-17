/*
** Ullrich von Bassewitz, 2012-11-26
**
** Minimum value of a long. Is used in ascii conversions, since this value
** has no positive counterpart than can be represented in 32 bits. In C,
** since the compiler will convert to the correct character set for the
** target platform.
*/



const unsigned char _longminstr[] = "-2147483648";



