/*
  !!DESCRIPTION!! C-Manual Chapter 4: what's in a name?
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

#ifdef NO_IMPLICIT_FUNC_PROTOTYPES
int s4(struct defs *pd0);
int svtest(int n);
zero();
testev();
setev();
#endif

#ifndef NO_OLD_FUNC_DECL
s4(pd0)                    /* 4. What's in a name?             */
struct defs *pd0;
{
#else
int s4(struct defs *pd0) {
#endif
   static char s4er[] = "s4,er%d\n";
   static char qs4[8] = "s4     ";
   char *ps, *pt;
   int j, rc;

   short sint;             /* short integer, for size test      */
   int pint;               /* plain                             */
   long lint;              /* long                              */
   unsigned target;
   unsigned int mask;

   rc = 0;
   ps = qs4;
   pt = pd0->rfs;

   while(*pt++ = *ps++);

/*   There are four declarable storage classes: automatic,
static, external, and register. Automatic variables have
been dealt with extensively thus far, and will not be specif-
ically treated in this section. Register variables are treated
in section s81.

     Static variables are local to a block, but retain their
values upon reentry to a block, even after control has left
the block.                                                     */

   for (j=0; j<3; j++)
     if(svtest(j) != zero()){
       rc = 1;
       if(pd0->flgd != 0) printf(s4er,1);
     }
   ;

/*   External variables exist and retain their values throughout
the execution of the entire program, and may be used for comm-
unication between functions, even separately compiled functions.
                                                                */

   setev();
   if(testev() != 0){
     rc=rc+2;
     if(pd0->flgd != 0) printf(s4er,2);
   }
/*
     Characters have been tested elsewhere (in s243).

     Up to three sizes of integer, declared short int, int, and
long int, are available. Longer integers provide no less storage
than shorter ones, but implementation may make either short
integers, or long integers, or both, equivalent to plain
integers.
                                                                */

   if(sizeof lint < sizeof pint || sizeof pint < sizeof sint){
     rc = rc+4;
     if(pd0->flgd != 0) printf(s4er,4);
   }

/*   Unsigned integers, declared unsigned, obey the laws of
arithmetic modulo 2**n, where n is the number of bits in the
implementation                                                  */

   target = ~0U;
   mask = 1;
   printf("sizeof target: %08x pd0->cbits: %08x\n", sizeof target, pd0->cbits);
   printf("mask: %08x target: %08x\n", mask, target);

   for(j=0; j<(sizeof target)*pd0->cbits; j++){
     mask = mask&target;
     target = target>>1;
     printf("mask: %08x target: %08x\n", mask, target);
   }

   if(mask != 1 || target != 0){
     rc = rc+8;
     if(pd0->flgd != 0) printf(s4er,8);
   }

   return rc;
}

#ifndef NO_OLD_FUNC_DECL
svtest(n)
int n;
{
#else
int svtest(int n) {
#endif

   static k;
   int rc;
   switch (n) {
     case 0: k = 1978;
             rc = 0;
             break;

     case 1: if(k != 1978) rc = 1;
             else{
              k = 1929;
              rc = 0;
             }
             break;

     case 2: if(k != 1929) rc = 1;
             else rc = 0;
             break;
   }
   return rc;
}
zero(){                 /* Returns a value of zero, possibly */
   static k;            /* with side effects, as it's called */
   int rc;              /* alternately with svtest, above,   */
   k = 2;               /* and has the same internal storage */
   rc = 0;              /* requirements.                     */
   return rc;
}
testev(){
   if(extvar != 1066) return 1;
   else return 0;
}

/* Sets an external variable. Used  */
/* by s4, and should be compiled    */
/* separately from s4.              */

setev(){
#ifndef NO_SLOPPY_EXTERN
   extern int extvar;
#endif
   extvar = 1066;
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
                case 1: return s4(pd0);
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
