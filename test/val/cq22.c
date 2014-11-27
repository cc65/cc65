/*
  !!DESCRIPTION!! C-Manual Chapter 2.2: identifiers (names)
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
        2.2 Identifiers (Names)
*/

#ifndef NO_OLD_FUNC_DECL
s22(pd0)
struct defs *pd0;
{
#else
int s22(struct defs *pd0)
{
#endif

   int a234, a;
   int _, _234, A, rc;

   static char s22er[] = "s22,er%d\n";
   static char qs22[8] = "s22    ";

   char *ps, *pt;
                         /* Initialize                      */

   rc = 0;
   ps = qs22;
   pt = pd0 -> rfs;
   while (*pt++ = *ps++);

     /* An identifier is a sequence of letters and digits;
        the first character must be a letter. The under-
        score _ counts as a letter.                        */

   a=1;
   _=2;
   _234=3;
   a234=4;
   if(a+_+_234+a234 != 10) {
     rc = rc+1;
     if(pd0->flgd != 0) printf(s22er,1);
   }

   /* Upper and lower case letters are different.     */

   A = 2;
   if (A == a) {
     rc = rc+4;
     if (pd0->flgd != 0) printf(s22er,4);
   }

   return(rc);
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
                case 0: return s22(pd0);
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
