/*
  !!DESCRIPTION!! C-Manual Chapter 6.2: Float and double, 6.3 Floating and integral, 6.4 Pointers and integers, 6.5 Unsigned, 6.6 Arithmetic conversions
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

#define CQ26_INCLUDED
/*
  section s26, which pokes around at the hardware
  trying to figure out the characteristics of the machine that
  it is running on, saves information that is subsequently
  used by sections s626, s72, and s757. If this program is
  to be broken up into smallish pieces, say for running on
  a microcomputer, take care to see that s26 is called before
  calling any of the latter three sections.
*/

/*
        2.6  Hardware Characteristics
*/

#ifndef NO_OLD_FUNC_DECL
s26(pd0)
struct defs *pd0;
{
#else
s26(struct defs *pd0) {
#endif
   static char qs26[8] = "s26    ";
   char *ps, *pt;
   char c0, c1;
   #ifndef NO_FLOATS
        float temp, one, delta;
        double tempd, oned;
   #endif
   static char s[] = "%3d bits in %ss.\n";
   static char s2[] = "%e is the least number that can be added to 1. (%s).\n";

   ps = qs26;
   pt = pd0->rfs;

   while(*pt++ = *ps++);

          /* Here, we shake the machinery a little to see what falls
             out.  First, we find out how many bits are in a char.  */

   pd0->cbits = 0;
   c0 = 0;
   c1 = 1;

   while(c0 != c1) {
     c1 = c1<<1;
     pd0->cbits = pd0->cbits+1;
   }
          /* That information lets us determine the size of everything else. */

   pd0->ibits = pd0->cbits * sizeof(int);
   pd0->sbits = pd0->cbits * sizeof(short);
   pd0->lbits = pd0->cbits * sizeof(long);
   pd0->ubits = pd0->cbits * sizeof(unsigned);
   #ifndef NO_FLOATS
        pd0->fbits = pd0->cbits * sizeof(float);
        pd0->dbits = pd0->cbits * sizeof(double);
   #endif

          /* We have now almost reconstructed the table in section 2.6, the
             exception being the range of the floating point hardware.
             Now there are just so many ways to conjure up a floating point
             representation system that it's damned near impossible to guess
             what's going on by writing a program to interpret bit patterns.
             Further, the information isn't all that useful, if we consider
             the fact that machines that won't handle numbers between 10**30
             and 10**-30 are very hard to find, and that people playing with
             numbers outside that range have a lot more to worry about than
             just the capacity of the characteristic.

             A much more useful measure is the precision, which can be ex-
             pressed in terms of the smallest number that can be added to
             1. without loss of significance. We calculate that here, for
             float and double.                       */

#ifndef NO_FLOATS
   one = 1.;
   delta = 1.;
   temp = 0.;
   while(temp != one) {
     temp = one+delta;
     delta = delta/2.;
   }
   pd0->fprec = delta * 4.;
   oned = 1.;
   delta = 1.;
   tempd = 0.;
   while(tempd != oned) {
     tempd = oned+delta;
     delta = delta/2.;
   }
   pd0->dprec = delta * 4.;
#endif

          /* Now, if anyone's interested, we publish the results.       */

#ifndef CQ26_INCLUDED
   if(pd0->flgm != 0) {
     printf(s,pd0->cbits,"char");
     printf(s,pd0->ibits,"int");
     printf(s,pd0->sbits,"short");
     printf(s,pd0->lbits,"long");
     printf(s,pd0->ubits,"unsigned");
     printf(s,pd0->fbits,"float");
     printf(s,pd0->dbits,"double");
     #ifndef NO_FLOATS
        printf(s2,pd0->fprec,"float");
        printf(s2,pd0->dprec,"double");
     #else
        printf("NO_FLOATS\n");
     #endif
   }
#endif
          /* Since we are only exploring and perhaps reporting, but not
             testing any features, we cannot return an error code.  */

   return 0;
}

int extvar;

#ifndef NO_OLD_FUNC_DECL
s626(pd0)          /* 6.2 Float and double                  */
                   /* 6.3 Floating and integral                 */
                   /* 6.4 Pointers and integers                 */
                   /* 6.5 Unsigned                              */
                   /* 6.6 Arithmetic conversions                */
struct defs *pd0;
{
#else
int s626(struct defs *pd0){
#endif
   static char s626er[] = "s626,er%d\n";
   static char qs626[8] = "s626   ";
   int rc;
   char *ps, *pt;
   #ifndef NO_FLOATS
   float eps, f1, f2, f3, f4, f;
   #endif
   long lint1, lint2, l, ls;
   char c, t[28], t0;
   short s;
   int is, i, j;
   unsigned u, us;
   #ifndef NO_FLOATS
        double d, ds;
   #endif
   ps = qs626;
   pt = pd0->rfs;
   rc = 0;
   while (*pt++ = *ps++);

   #ifndef NO_FLOATS

        /* Conversions of integral values to floating type are
        well-behaved.                                           */

   f1 = 1.;
   lint1 = 1.;
   lint2 = 1.;

   for(j=0;j<pd0->lbits-2;j++){
     f1 = f1*2;
     lint2 = (lint2<<1)|lint1;
   }
   f2 = lint2;
   f1 = (f1-f2)/f1;
   if(f1>2.*pd0->fprec){
     rc = rc+2;
     if(pd0->flgd != 0) printf(s626er,2);
   }

        /* Pointer-integer combinations are discussed in s74,
        "Additive operators". The unsigned-int combination
        appears below.                                          */

   c = 125;
   s = 125;
   i = 125;     is = 15625;
   u = 125;     us = 15625;
   l = 125;     ls = 15625;
   f = 125.;
   d = 125.;    ds = 15625.;

   for(j=0;j<28;j++) t[j] = 0;

   if(c*c != is) t[ 0] = 1;
   if(s*c != is) t[ 1] = 1;
   if(s*s != is) t[ 2] = 1;
   if(i*c != is) t[ 3] = 1;
   if(i*s != is) t[ 4] = 1;
   if(i*i != is) t[ 5] = 1;
   if(u*c != us) t[ 6] = 1;
   if(u*s != us) t[ 7] = 1;
   if(u*i != us) t[ 8] = 1;
   if(u*u != us) t[ 9] = 1;
   if(l*c != ls) t[10] = 1;
   if(l*s != ls) t[11] = 1;
   if(l*i != ls) t[12] = 1;
   if(l*u != us) t[13] = 1;
   if(l*l != ls) t[14] = 1;
   if(f*c != ds) t[15] = 1;
   if(f*s != ds) t[16] = 1;
   if(f*i != ds) t[17] = 1;
   if(f*u != ds) t[18] = 1;
   if(f*l != ds) t[19] = 1;
   if(f*f != ds) t[20] = 1;
   if(d*c != ds) t[21] = 1;
   if(d*s != ds) t[22] = 1;
   if(d*i != ds) t[23] = 1;
   if(d*u != ds) t[24] = 1;
   if(d*l != ds) t[25] = 1;
   if(d*f != ds) t[26] = 1;
   if(d*d != ds) t[27] = 1;

   t0 = 0;
   for(j=0; j<28; j++) t0 = t0+t[j];

   if(t0 != 0){
     rc = rc+4;
     if(pd0->flgd != 0){
       printf(s626er,4);
       printf("   key=");
       for(j=0;j<28;j++) printf("%d",t[j]);
       printf("\n");
     }
   }

   #endif

        /* When an unsigned integer is converted to long,
           the value of the result is the same numerically
           as that of the unsigned integer.               */

   l = (unsigned)0100000;
   if((long)l > (unsigned)0100000){
      rc = rc+8;
      if(pd0->flgd != 0) printf(s626er,8);
   }

   return rc;
}

/*********************************************************************************************
 the main loop that launches the sections
*********************************************************************************************/

#ifndef NO_TYPELESS_STRUCT_PTR
	int section(int j,struct* pd0){
#else
	int section(int j,void* pd0){
#endif
        switch(j){
                case 0: return s26(pd0);
                case 1: return s626(pd0);
        }
}

#define cq_sections 2

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
