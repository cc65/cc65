/*
  !!DESCRIPTION!! C-Manual Chapter 2.41: integer constants, 2.42: explizit long constants
  !!ORIGIN!!      LCC 4.1 Testsuite
  !!LICENCE!!     own, freely distributeable for non-profit. read CPYRIGHT.LCC
*/

#include "common.h"

struct defs {
     int cbits;          /* No. of bits per char           */
     int ibits;          /*                 int            */
     int sbits;          /*                 short          */
     int lbits;          /*                 long           */
     int ubits;          /*                 unsigned       */
     int fbits;          /*                 float          */
     int dbits;          /*                 double         */
     #ifndef NO_FLOATS
        float fprec;        /* Smallest number that can be    */
        float dprec;        /* significantly added to 1.      */
     #endif
     int flgs;           /* Print return codes, by section */
     int flgm;           /* Announce machine dependencies  */
     int flgd;           /* give explicit diagnostics      */
     int flgl;           /* Report local return codes.     */
     int rrc;            /* recent return code             */
     int crc;            /* Cumulative return code         */
     char rfs[8];        /* Return from section            */
};

     int lbits;          /*                 long           */
     int ubits;          /*                 unsigned       */
     int fbits;          /*                 float          */
     int dbits;          /*                 double         */
     #ifndef NO_FLOATS
        float fprec;        /* Smallest number that can be    */
        float dprec;        /* significantly added to 1.      */
     #endif
     int flgs;           /* Print return codes, by section */
     int flgm;           /* Announce machine dependencies  */
     int flgd;           /* give explicit diagnostics      */
     int flgl;           /* Report local return codes.     */
     int rrc;            /* recent return code             */
     int crc;            /* Cumulative return code         */
     char rfs[8];        /* Return from section            */

/*
        2.4.1 Integer constants
        2.4.2 Explicit long constants
*/

/* Calculate 2**n by multiplying, not shifting  */
#ifndef NO_OLD_FUNC_DECL
long pow2(n)
long n;
{
#else
long pow2(long n) {
#endif
   long s;
   s = 1;
   while(n--) s = s*2;
   return s;
}

   long d[39], o[39], x[39];

#ifndef NO_OLD_FUNC_DECL
s241(pd0)
struct defs *pd0;
{
#else
int s241(struct defs *pd0) {
#endif

/*   long pow2(); */
   static char s241er[] = "s241,er%d\n";
   static char qs241[8] = "s241   ";
   char *ps, *pt;
   int rc, j, lrc;
   static long g[39] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                        0,6,0,8,0,12,0,16,0,18,0,20,0,24,
                        0,28,0,30,0,32,0,36};
/*   long d[39], o[39], x[39]; */

   rc = 0;
   lrc = 0;
   ps = qs241;
   pt = pd0 -> rfs;
   while (*pt++ = *ps++);

     /* An integer constant consisting of a sequence of digits is
        taken to be octal if it begins with 0 (digit zero), decimal
        otherwise.                                            */

   if (   8 !=  010
     ||  16 !=  020
     ||  24 !=  030
     ||  32 !=  040
     ||  40 !=  050
     ||  48 !=  060
     ||  56 !=  070
     ||  64 != 0100
     ||  72 != 0110
     ||  80 != 0120
     ||   9 != 0011
     ||  17 != 0021
     ||  25 != 0031
     ||  33 != 0041
     ||  41 != 0051
     ||  49 != 0061
     ||  57 != 0071
     ||  65 != 0101
     ||  73 != 0111
     ||  81 != 0121 ){
     rc = rc+1;
     if( pd0->flgd != 0 ) printf(s241er,1);
   }

     /* A sequence of digits preceded by 0x or 0X (digit zero)
        is taken to be a hexadecimal integer. The hexadecimal
        digits include a or A through f or F with values 10
        through 15.     */

   if ( 0x00abcdef != 0xabcdef
     || 0xabcdef != 0Xabcdef || 0Xabcdef != 0XAbcdef
     || 0XAbcdef != 0XABcdef || 0XABcdef != 0XABCdef
     || 0XABCdef != 0XABCDef || 0XABCDef != 0XABCDEf
     || 0XABCDEf != 0XABCDEF || 0xABCDEF != 11259375 ){
     rc = rc+2;
     if( pd0->flgd != 0 ) printf(s241er,2);
   }

     /* A decimal constant whose value exceeds the largest signed
        machine integer is taken to be long; an octal or hex con-
        stant which exceeds the largest unsigned machine integer
        is likewise taken to be long.     */
#if defined(REFERENCE) && defined(REFCC_SIZEOF_LONG_64BIT)
/*#warning "sizeof(long)!=4, skipping test"*/
#else
   if ( sizeof 010000000000 != sizeof(long)      /* 2**30 */
     || sizeof 1073741824   != sizeof(long)      /* ditto */
     || sizeof 0x40000000   != sizeof(long) ){   /*   "   */

     rc = rc+4;
     if( pd0->flgd != 0 ) printf(s241er,4);
   }
#endif
     /* A decimal, octal, or hexadecimal constant immediately followed
        by l (letter ell) or L is a long constant.    */

   if ( sizeof   67l != sizeof(long)
     || sizeof   67L != sizeof(long)
     || sizeof  067l != sizeof(long)
     || sizeof  067L != sizeof(long)
     || sizeof 0X67l != sizeof(long)
     || sizeof 0x67L != sizeof(long) ){
     rc = rc+8;
     if( pd0 -> flgd != 0 ) printf(s241er,8);
   }

     /* Finally, we test to see that decimal (d), octal (o),
        and hexadecimal (x) constants representing the same values
        agree among themselves, and with computed values, at spec-
        ified points over an appropriate range. The points select-
        ed here are those with the greatest potential for caus-
        ing trouble, i.e., zero, 1-16, and values of 2**n and
        2**n - 1 where n is some multiple of 4 or 6. Unfortunately,
        just what happens when a value is too big to fit in a
        long is undefined; however, it would be nice if what
        happened were at least consistent...      */

   for ( j=0; j<17; j++ ) g[j] = j;
   for ( j=18; j<39; ) {
     g[j] = pow2(g[j]);
     g[j-1] = g[j] - 1;
     j = j+2;
   }

   d[0] = 0;                o[0] = 00;               x[0] = 0x0;
   d[1] = 1;                o[1] = 01;               x[1] = 0x1;
   d[2] = 2;                o[2] = 02;               x[2] = 0x2;
   d[3] = 3;                o[3] = 03;               x[3] = 0x3;
   d[4] = 4;                o[4] = 04;               x[4] = 0x4;
   d[5] = 5;                o[5] = 05;               x[5] = 0x5;
   d[6] = 6;                o[6] = 06;               x[6] = 0x6;
   d[7] = 7;                o[7] = 07;               x[7] = 0x7;
   d[8] = 8;                o[8] = 010;              x[8] = 0x8;
   d[9] = 9;                o[9] = 011;              x[9] = 0x9;
   d[10] = 10;              o[10] = 012;             x[10] = 0xa;
   d[11] = 11;              o[11] = 013;             x[11] = 0xb;
   d[12] = 12;              o[12] = 014;             x[12] = 0xc;
   d[13] = 13;              o[13] = 015;             x[13] = 0xd;
   d[14] = 14;              o[14] = 016;             x[14] = 0xe;
   d[15] = 15;              o[15] = 017;             x[15] = 0xf;
   d[16] = 16;              o[16] = 020;             x[16] = 0x10;
   d[17] = 63;              o[17] = 077;             x[17] = 0x3f;
   d[18] = 64;              o[18] = 0100;            x[18] = 0x40;
   d[19] = 255;             o[19] = 0377;            x[19] = 0xff;
   d[20] = 256;             o[20] = 0400;            x[20] = 0x100;
   d[21] = 4095;            o[21] = 07777;           x[21] = 0xfff;
   d[22] = 4096;            o[22] = 010000;          x[22] = 0x1000;
   d[23] = 65535;           o[23] = 0177777;         x[23] = 0xffff;
   d[24] = 65536;           o[24] = 0200000;         x[24] = 0x10000;
   d[25] = 262143;          o[25] = 0777777;         x[25] = 0x3ffff;
   d[26] = 262144;          o[26] = 01000000;        x[26] = 0x40000;
   d[27] = 1048575;         o[27] = 03777777;        x[27] = 0xfffff;
   d[28] = 1048576;         o[28] = 04000000;        x[28] = 0x100000;
   d[29] = 16777215;        o[29] = 077777777;       x[29] = 0xffffff;
   d[30] = 16777216;        o[30] = 0100000000;      x[30] = 0x1000000;
   d[31] = 268435455;       o[31] = 01777777777;     x[31] = 0xfffffff;
   d[32] = 268435456;       o[32] = 02000000000;     x[32] = 0x10000000;
   d[33] = 1073741823;      o[33] = 07777777777;     x[33] = 0x3fffffff;
   d[34] = 1073741824;      o[34] = 010000000000;    x[34] = 0x40000000;
   d[35] = 4294967295;      o[35] = 037777777777;    x[35] = 0xffffffff;
   d[36] = 4294967296;      o[36] = 040000000000;    x[36] = 0x100000000;
   d[37] = 68719476735;     o[37] = 0777777777777;   x[37] = 0xfffffffff;
   d[38] = 68719476736;     o[38] = 01000000000000;  x[38] = 0x1000000000;

   /* WHEW! */

   for (j=0; j<39; j++){
     if ( g[j] != d[j]
       || d[j] != o[j]
       || o[j] != x[j]) {
       if( pd0 -> flgm != 0 ) {
/*       printf(s241er,16);          save in case opinions change...     */
         printf("Decimal and octal/hex constants sometimes give\n");
         printf("   different results when assigned to longs.\n");
       }
/*     lrc = 1;   save...   */
     }
   }

   if (lrc != 0) rc =16;

   return rc;
}

/*********************************************************************************************
 the main loop that launches the sections
*********************************************************************************************/

#define cq_sections 1

#ifndef NO_TYPELESS_STRUCT_PTR
	int section(int j,struct* pd0){
#else
	int section(int j,void* pd0){
#endif
        switch(j){
                case 0: return s241(pd0);
        }
}

/*
        C REFERENCE MANUAL (main)
*/

#ifndef NO_OLD_FUNC_DECL
main(n,args)
int n;
char **args;
{
#else
int main(int n,char **args) {
#endif

int j;
static struct defs d0, *pd0;
	
   d0.flgs = 1;          /* These flags dictate            */
   d0.flgm = 1;          /*     the verbosity of           */
   d0.flgd = 1;          /*         the program.           */
   d0.flgl = 1;

   pd0 = &d0;

   for (j=0; j<cq_sections; j++) {
     d0.rrc=section(j,pd0);
     d0.crc=d0.crc+d0.rrc;
     if(d0.flgs != 0) printf("Section %s returned %d.\n",d0.rfs,d0.rrc);
   }

   if(d0.crc == 0) printf("\nNo errors detected.\n");
   else printf("\nFailed.\n");

   return d0.crc;
}
