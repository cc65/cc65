/*
  !!DESCRIPTION!! C-Manual Chapter 6.1: characters and integers
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

int extvar;

#ifndef NO_OLD_FUNC_DECL
s61(pd0)          /* Characters and integers */
struct defs *pd0;
{
#else
int s61(struct defs *pd0){
#endif
   static char s61er[] = "s61,er%d\n";
   static char s61ok[] = "s61,ok%d\n";
   static char qs61[8] = "s61    ";
   short from, shortint;
   long int to, longint;
   int rc, lrc;
   int j;
   char fromc, charint;
   char *wd, *pc[6];

   static char upper_alpha[]             = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   static char lower_alpha[]             = "abcdefghijklmnopqrstuvwxyz";
   static char numbers[]               = "0123456789";
   static char special_characters[]    = "~!\"#%&()_=-^|{}[]+;*:<>,.?/";
   static char extra_special_characters[] = "\n\t\b\r\f\\\'";
   static char blank_and_NUL[]            = " \0";

   char *ps, *pt;
   ps = qs61;
   pt = pd0->rfs;
   rc = 0;

   printf(s61ok,0);

   while (*pt++ = *ps++);

/*      A character or a short integer may be used wherever
an integer may be used. In all cases, the value is converted
to integer. This principle is extensively used throughout this
program, and will not be explicitly tested here.        */

/*      Conversion of a shorter integer to a longer always
involves sign extension.                                */

   from = -19;
   to = from;

   if(to != -19){
     rc = rc+1;
     if(pd0->flgd != 0) printf(s61er,1);
   }
   else if(pd0->flgd != 0) printf(s61ok,1);

/*      It is guaranteed that a member of the standard char-
acter set is nonnegative.                               */

   pc[0] = upper_alpha;
   pc[1] = lower_alpha;
   pc[2] = numbers;
   pc[3] = special_characters;
   pc[4] = extra_special_characters;
   pc[5] = blank_and_NUL;

   lrc = 0;
   for (j=0; j<6; j++)
     while(*pc[j]) if(*pc[j]++ < 0) lrc =1;

   if(lrc != 0){
     rc=rc+2;
     if(pd0->flgd != 0) printf(s61er,2);
   }
   else if(pd0->flgd != 0) printf(s61ok,2);

/*      When a longer integer is converted to a shorter or
to  a char, it is truncated on the left; excess bits are
simply discarded.                                       */

   longint = 1048579;           /* =2**20+3 */
   shortint = longint;
   charint = longint;

   if((shortint != longint && shortint != 3) ||
      (charint  != longint && charint  != 3)) {
     rc = rc+8;
     if(pd0->flgd != 0) printf(s61er,8);
   }
   else if(pd0->flgd != 0) printf(s61ok,8);

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
                /*case 0: return s26(pd0);*/
                case 0: return s61(pd0);
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
