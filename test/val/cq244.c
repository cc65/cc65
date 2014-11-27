/*
  !!DESCRIPTION!! C-Manual Chapter 2.44: floating point constants
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

#ifndef NO_OLD_FUNC_DECL
s244(pd0)
struct defs *pd0;
{
#else
s244(struct defs *pd0) {
#endif

   #ifndef NO_FLOATS
        double a[8];

   int rc, lrc, j;
   static char s244er[] = "s244,er%d\n";
   static char qs244[8] = "s244   ";
   char *ps, *pt;

   ps = qs244;
   pt = pd0->rfs;
   while(*pt++ = *ps++);
   rc = 0;
   lrc = 0;

   /* Unfortunately, there's not a lot we can do with floating constants.
      We can check to see that the various representations can be com-
      piled, that the conversion is such that they yield the same hard-
      ware representations in all cases, and that all representations
      thus checked are double precision.              */

   a[0] = .1250E+04;
   a[1] = 1.250E3;
   a[2] = 12.50E02;
   a[3] = 125.0e+1;
   a[4] = 1250e00;
   a[5] = 12500.e-01;
   a[6] = 125000e-2;
   a[7] = 1250.;

   lrc = 0;
   for (j=0; j<7; j++) if(a[j] != a[j+1]) lrc = 1;

   if(lrc != 0) {
     if(pd0->flgd != 0) printf(s244er,1);
     rc = rc+1;
   }

   if ( (sizeof .1250E+04 ) != sizeof(double)
     || (sizeof 1.250E3   ) != sizeof(double)
     || (sizeof 12.50E02  ) != sizeof(double)
     || (sizeof 1.250e+1  ) != sizeof(double)
     || (sizeof 1250e00   ) != sizeof(double)
     || (sizeof 12500.e-01) != sizeof(double)
     || (sizeof 125000e-2 ) != sizeof(double)
     || (sizeof 1250.     ) != sizeof(double)){
     if(pd0->flgd != 0) printf(s244er,2);
     rc = rc+2;
   }

   return rc;

   #else

   return 0;

   #endif
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
                case 0: return s244(pd0);
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
