/*
  !!DESCRIPTION!! C-Manual Chapter 8.8: typedef
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

one(){
   return 1;
}
int *metricp;
#ifndef NO_OLD_FUNC_DECL
s88(pd0)          /*  8.8 Typedef  */
struct defs *pd0;
{
#else
int s88(struct defs *pd0){
#endif
   static char s88er[] = "s88,er%d\n";
   static char qs88[8] = "s88    ";
   int rc;
   char *ps, *pt;

        /* Declarations whose "storage class" is typdef do not
        define storage, but instead define identifiers which
        can later be used as if they were type keywords naming
        fundamental or derived types.
                                                                */

   typedef int MILES, *KLICKSP;

   #ifndef NO_FLOATS
   typedef struct {double re, im;} complex;
   #else
   typedef struct {signed re, im;} complex;
   #endif

   MILES distance;
   #ifndef NO_SLOPPY_EXTERN
   extern KLICKSP metricp;
   #else
   KLICKSP metricp;
   #endif
   complex z, *zp;

   ps = qs88;
   pt = pd0->rfs;
   rc = 0;
   while(*pt++ = *ps++);

        /* Hopefully, all of this stuff will compile. After that,
        we can only make some superficial tests.

        The type of distance is int,
                                                                */

   if(sizeof distance != sizeof(int)){
     if(pd0->flgd != 0) printf(s88er,1);
     rc = rc+1;
   }

        /* that of metricp is "pointer to int",                 */

   metricp = &distance;
   distance = 2;
   *metricp = 3;

   if(distance != 3){
     if(pd0->flgd != 0) printf(s88er,2);
     rc = rc+2;
   }

        /* and that of z is the specified structure. zp is a
        pointer to such a structure.
                                                                */

   #ifndef NO_FLOATS
   z.re = 0.;
   z.im = 0.;
   zp = &z;
   zp->re = 1.;
   zp->im = 1.;
   if(z.re+z.im != 2.){
   #else
   z.re = 0;
   z.im = 0;
   zp = &z;
   zp->re = 1;
   zp->im = 1;
   if(z.re+z.im != 2){
   #endif
     if(pd0->flgd != 0) printf(s88er,4);
     rc = rc+4;
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
                case 0: return s88(pd0);
        }
}

#define cq_sections 1

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
