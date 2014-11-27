/*
  !!DESCRIPTION!! C-Manual Chapter 7.5: Shift operators, 7.6 Relational operators, 7.7 Equality operator
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

#ifndef NO_OLD_FUNC_DECL
s757(pd0)          /* 7.5 Shift operators          */
                   /* 7.6 Relational operators     */
                   /* 7.7 Equality operator        */
struct defs *pd0;
{
#else
int s757(struct defs *pd0){
#endif
   static char s757er[] = "s757,er%d\n";
   static char qs757[8] = "s757   ";
   int rc;
   char *ps, *pt;
   int t,lrc,k,j,a,b,c,d,x[16],*p;
   unsigned rs, ls, rt, lt;
   ps = qs757;
   pt = pd0->rfs;
   rc = 0;
   while (*pt++ = *ps++);

        /* The shift operators << and >> group left-to-right.
                                                                */

   t = 40;
   if(t<<3<<2 != 1280 || t>>3>>2 != 1){
     rc = rc+1;
     if(pd0->flgd != 0) printf(s757er,1);
   }

        /* In the following test, an n-bit unsigned consisting
        of all 1s is shifted right (resp. left) k bits, 0<=k<n.
        We expect to find k 0s followed by n-k 1s (resp. n-k 1s
        followed by k 0s). If not, we complain.
                                                                */

   lrc = 0;
   for(k=0; k<pd0->ubits; k++){
     rs = 1;
     ls = rs<<(pd0->ubits-1);

     rt = 0;
     lt = ~rt>>k;
     rt = ~rt<<k;

     for(j=0; j<pd0->ubits;j++){
       if((j<k) != ((rs&rt) == 0) || (j<k) != ((ls&lt) == 0)) lrc = 1;
       rs = rs<<1;
       ls = ls>>1;
     }
   }

   if(lrc != 0){
     rc = rc+2;
     if(pd0->flgd != 0) printf(s757er,2);
   }

        /* The relational operators group left-to-right, but this
        fact is not very useful; a<b<c does not mean what it
        seems to...
                                                                */

   a = 3;
   b = 2;
   c = 1;

   if((a<b<c) != 1){
     rc = rc+4;
     if(pd0->flgd != 0) printf(s757er,4);
   }

        /* In general, we take note of the fact that if we got this
        far the relational operators have to be working. We test only
        that two pointers may be compared; the result depends on
        the relative locations in the address space of the
        pointed-to objects.
                                                                */
   if( &x[1] == &x[0] ){
     rc = rc+8;
     if(pd0->flgd != 0) printf(s757er,8);
   }

   if( &x[1] < &x[0] ) if(pd0->flgm != 0)
     printf("Increasing array elements assigned to decreasing locations\n");

        /* a<b == c<d whenever a<b and c<d have the same
        truth value.                                            */

   lrc = 0;

   for(j=0;j<16;j++) x[j] = 1;
   x[1] = 0;
   x[4] = 0;
   x[6] = 0;
   x[7] = 0;
   x[9] = 0;
   x[13] = 0;

   for(a=0;a<2;a++)
     for(b=0;b<2;b++)
       for(c=0;c<2;c++)
         for(d=0;d<2;d++)
           if((a<b==c<d) != x[8*a+4*b+2*c+d] ) lrc = 1;

   if(lrc != 0){
     rc = rc+16;
     if(pd0->flgd != 0) printf(s757er,16);
   }

        /* A pointer to which zero has been assigned will
        appear to be equal to zero.
                                                                */

   p = 0;

   if(p != 0){
     rc = rc+32;
     if(pd0->flgd != 0) printf(s757er,32);
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
                case 1: return s757(pd0);
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
