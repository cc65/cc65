/*
  !!DESCRIPTION!! C-Manual Chapter 7.1: primary expressions
  !!ORIGIN!!      LCC 4.1 Testsuite
  !!LICENCE!!     own, freely distributeable for non-profit. read CPYRIGHT.LCC
*/

#include "common.h"

/*include "cq26.c"*/ /* hardware check */

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

int extvar;

#ifndef NO_OLD_FUNC_DECL
s71(pd0)          /*         7.1  Primary expressions   */
struct defs *pd0;
{
#else
int s71(struct defs *pd0){
#endif
   static char s71er[] = "s71,er%d\n";
   static char qs71[8] = "s71    ";
   int rc;
   char *ps, *pt;
   static char q = 'q';
#ifndef NO_SLOPPY_EXTERN
   int x[10], McCarthy(), clobber(), a, b, *p;
#else
   int x[10], a, b, *p;
#endif
   ps = qs71;
   pt = pd0->rfs;
   rc = 0;
   while (*pt++ = *ps++);

/*   Testing of expressions and operators is quite complicated,
     because (a) problems are apt to surface in queer combinations
     of operators and operands, rather than in isolation,
     and (b) the number of expressions needed to provoke a case
     of improper behaviour may be quite large. Hence, we take the
     following approach: for this section, and for subsequent
     sections through 7.15, we will check the primitive operations
     in isolation, thus verifying that the primitives work,
     after a fashion. The job of testing combinations, we will
     leave to a separate, machine-generated program, to be included
     in the C test package at some later date.
                                                                */

/*   A string is a primary expression. The identifier points to
     the first character of a string.
                                                                  */

   if(*"queep" != q){
     rc = rc+1;
     if(pd0->flgd  != 0) printf(s71er,1);
   }
/*   A parenthesized expression is a primary expression whose
     type and value are the same as those of the unadorned
     expression.
                                                                */
   if((2+3) != 2+3) {
     rc = rc+2;
     if(pd0->flgd != 0) printf(s71er,2);
   }

/*   A primary expression followed by an expression in square
     brackets is a primary expression. The intuitive meaning is
     that of a subscript. The expression E1[E2] is identical
     (by definition) to *((E1)+(E2)).
                                                                */

   x[5] = 1942;
   if(x[5] != 1942 || x[5] != *((x)+(5))){
     rc = rc+4;
     if(pd0->flgd != 0) printf(s71er,4);
   }

/*   If the various flavors of function calls didn't work, we
     would never have gotten this far; however, we do need to
     show that functions can be recursive...
                                                               */

   if ( McCarthy(-5) != 91){
     rc = rc+8;
     if(pd0->flgd != 0) printf(s71er,8);
   }

/*   and that argument passing is strictly by value.           */

   a = 2;
   b = 3;
   p = &b;

   clobber(a,p);

   if(a != 2 || b != 2){
     rc = rc+16;
     if(pd0->flgd != 0) printf(s71er,16);
   }

/*   Finally, structures and unions are addressed thusly:      */

   #ifndef NO_FLOATS

   if(pd0->dprec != (*pd0).dprec){
     rc = rc+32;
     if(pd0->flgd != 0) printf(s71er,32);
   }

   #endif

   return rc;
}
#ifndef NO_OLD_FUNC_DECL
McCarthy(x)
int x;
{
#else
int McCarthy(int x){
#endif
   if(x>100) return x-10;
   else return McCarthy( McCarthy(x+11));
}

#ifndef NO_OLD_FUNC_DECL
clobber(x,y)
int x,*y;
#else
int clobber(int x,int *y)
#endif

/*
#ifndef NO_OLD_FUNC_DECL
clobber(x,y)
int x,
#ifdef NO_TYPELESS_INT_PTR
int
#endif
*y;
{
#else
int clobber(int x,
#ifdef NO_TYPELESS_INT_PTR
int
#endif
*y
){
#endif
*/

{
   x = 3;
   *y = 2;
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
                case 0: return s71(pd0);
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
