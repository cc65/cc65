/*
  !!DESCRIPTION!! C-Manual Chapter 9: Statements
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
s9(pd0)          /*  9  Statements  */
struct defs *pd0;
{
#else
int s9(struct defs *pd0){
#endif
   static char s9er[] = "s9,er%d\n";
   static char qs9[8] = "s9     ";
   int rc;
   char *ps, *pt;
   int lrc, i;

   ps = qs9;
   pt = pd0->rfs;
   rc = 0;
   while (*pt++ = *ps++);

        /* One would think that the section on statements would
        provide the most variety in the entire sequence of tests.
        As it turns out, most of the material in this section has
        already been checked in the process of checking out
        everything else, and the section at this point is somewhat
        anticlimactic. For this reason, we restrict ourselves
        to testing two features not already covered.

        Compound statements are delimited by braces. They have the
        nice property that identifiers of the auto and register
        variety are pushed and popped. It is currently legal to
        transfer into a block, but we wont...
                                                                */

   lrc = 0;
   for(i=0; i<2; i++){
     int j;
     register int k;
     j = k = 2;
       {
       int j;
       register int k;
       j = k = 3;
       if((j != 3) || (k != 3)) lrc = 1;
       }
     if((j != 2) || (k != 2)) lrc = 1;
   }

   if(lrc != 0){
     if(pd0->flgd != 0) printf(s9er,1);
     rc = rc+1;
   }

        /* Goto statements go to labeled statements, we hope.   */

   goto nobarf;
     if(pd0->flgd != 0) printf(s9er,2);
     rc = rc+2;
   nobarf:;

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
                case 0: return s9(pd0);
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
