/*
  !!DESCRIPTION!! C-Manual Chapter 7.15: Comma operator
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

/*#include "cq26.c"*/ /* hardware check */
#ifdef NO_IMPLICIT_FUNC_PROTOTYPES
s715f(int x,int y,int z);
#endif

#ifndef NO_OLD_FUNC_DECL
s715(pd0)          /*  7.15 Comma operator     */
struct defs *pd0;
{
#else
int s715(struct defs *pd0) {
#endif
   static char s715er[] = "s715,er%d\n";
   static char qs715[8] = "s715   ";
   int rc;
   char *ps, *pt;
   int a, t, c, i;
   a = c = 0;
   ps = qs715;
   pt = pd0->rfs;
   rc = 0;
   while (*pt++ = *ps++);

        /* A pair of expressions separated by a comma is
        evaluated left to right and the value of the left
        expression is discarded.
                                                                */
   i = 1;
   if( i++,i++,i++,i++,++i != 6 ){
     if(pd0->flgd != 0) printf(s715er,1);
     rc = rc+1;
   }

        /* In contexts where the comma is given a special mean-
        ing, for example in a list of actual arguments to
        functions (sic) and lists of initializers, the comma
        operator as described in this section can only appear
        in parentheses; for example

                f( a, (t=3, t+2), c)

        has three arguments, the second of which has the
        value 5.
                                                                */

   if(s715f(a, (t=3, t+2), c) != 5){
     if(pd0->flgd != 0) printf(s715er,2);
     rc = rc+2;
   }
   return rc;
}
s715f(x,y,z)
int x, y, z;
{
   return y;
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
                /*case 0: return s26(pd0);*/
                case 0: return s715(pd0);
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
