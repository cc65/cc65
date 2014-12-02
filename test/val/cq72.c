/*
  !!DESCRIPTION!! C-Manual Chapter 7.2: Unary Operators
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
s72(pd0)          /*  7.2  Unary operators  */
struct defs *pd0;
{
#else
int s72(struct defs *pd0){
#endif
   static char s72er[] = "s72,er%d\n";
   static char qs72[8] = "s72    ";
   int rc;
   char *ps, *pt;
   int k, j, i, lrc;
   char c;
   short s;
   long l;
   unsigned u;

   #ifndef NO_FLOATS
        double d;
        float f;
   #else
        signed d;
        signed f;
   #endif

   ps = qs72;
   pt = pd0->rfs;
   rc = 0;
   while (*pt++ = *ps++);

        /* The *, denoting indirection, and the &, denoting a
        pointer, are duals of each other, and ought to behave as
        such...                                                 */

   k = 2;
   if(*&*&k != 2){
     rc = rc+1;
     printf(s72er,1);
   }

        /* The unary minus has the conventional meaning.        */

   if(k+(-k) != 0){
     rc = rc+2;
     printf(s72er,2);
   }

        /*  The negation operator (!) has been thoroughly checked out,
        perhaps more thoroughly than any of the others. The ~ oper-
        ator gets us a ones complement.                         */

   k = 0;
   for(j=0;j<pd0->ibits;j++) k = (k<<1)|1;
   if(~k != 0){
     rc = rc+4;
     printf(s72er,4);
   }

        /*  Now we look at the ++ and -- operators, which can be
        used in either prefix or suffix form. With side
        effects they're loaded.                                 */

   k = 5;

   if( ++k != 6 || --k != 5
    || k++ != 5 || k-- != 6
    ||   k != 5 ){
     rc = rc+8;
     printf(s72er,8);
   }

        /*  An expression preceded by the parenthesised name of a
        data type causes conversion of the value of the expression
        to the named type. This construction is called a cast.
        Here, we check to see that all of the possible casts and
        their simple combinations are accepted by the compiler,
        and that they all produce a correct result for this sample
        of size one.                                            */

   c = 26;  l = 26;
   s = 26;  u = 26;
   i = 26;
   #ifndef NO_FLOATS
   f = 26.;
   d = 26.;
   #else
   f = 26;
   d = 26;
   #endif

   lrc = 0;

   if( (char)s != 26 || (char)i != 26
    || (char)l != 26 || (char)u != 26
    || (char)f != 26 || (char)d != 26 ) lrc = lrc+1;

   if( (short)c != 26 || (short)i != 26
    || (short)l != 26 || (short)u != 26
    || (short)f != 26 || (short)d != 26) lrc = lrc+2;

   if( (int)c != 26 || (int)s != 26
    || (int)l != 26 || (int)u != 26
    || (int)f != 26 || (int)d != 26 ) lrc = lrc+4;

   if( (long)c != 26 || (long)s != 26
    || (long)i != 26 || (long)u != 26
    || (long)f != 26 || (long)d != 26 ) lrc = lrc+8;

   if( (unsigned)c != 26 || (unsigned)s != 26
    || (unsigned)i != 26 || (unsigned)l != 26
    || (unsigned)f != 26 || (unsigned)d != 26 ) lrc = lrc+16;

   #ifndef NO_FLOATS
   if( (float)c != 26. || (float)s != 26.
    || (float)i != 26. || (float)l != 26.
    || (float)u != 26. || (float)d != 26. ) lrc = lrc+32;

   if( (double)c != 26. || (double)s != 26.
    || (double)i != 26. || (double)l != 26.
    || (double)u != 26. || (double)f != 26. ) lrc = lrc+64;
   #endif

   if(lrc != 0){
     rc = rc+16;
     printf(s72er,16);
   }

        /*  The sizeof operator has been tested previously.     */

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
                case 1: return s72(pd0);
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
