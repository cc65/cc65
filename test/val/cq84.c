/*
  !!DESCRIPTION!! C-Manual Chapter 8.4: meaning of declarators
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

#ifdef NO_SLOPPY_EXTERN
   int *fip(int x);
   int array(int a[],int size,int start);
   int glork(int x);
#endif

#ifndef NO_OLD_FUNC_DECL
s84(pd0)          /*  8.4 Meaning of declarators   */
struct defs *pd0;
{
#else
int s84(struct defs *pd0){
#endif
#ifndef NO_SLOPPY_EXTERN
   int *ip, i, *fip(), (*pfi)(), j, k, array(), glork();
#else
   int *ip, i, j, k,(*pfi)();
/*
   extern int
   *fip(),
   array(),
   glork();
   int *fip(int x);
   int array(int a[],int size,int start);
*/
#endif
   static int x3d[3][5][7];
   #ifndef NO_FLOATS
   float fa[17], *afp[17], sum;
   #else
   signed fa[17], *afp[17], sum;
   #endif
   static char s84er[] = "s84,er%d\n";
   static char qs84[8] = "s84    ";
   int rc;
   char *ps, *pt;
   ps = qs84;
   pt = pd0->rfs;
   rc = 0;
   while (*pt++ = *ps++);

        /* The more common varieties of declarators have al-
        ready been touched upon, some more than others. It
        is useful to compare *fip() and (*pfi)().
                                                                */

   ip = fip(3);
   if(*ip != 3){
     if(pd0->flgd != 0) printf(s84er,1);
     rc = rc+1;
   }

   /* kludges */
   #if defined(FORCE_POINTERS) | defined(NO_OLD_FUNC_DECL)
   if(glork(4) != 4){
     if(pd0->flgd != 0) printf(s84er,2);
     rc = rc+2;
   }
	#else
   pfi = glork;
   if((*pfi)(4) != 4){
     if(pd0->flgd != 0) printf(s84er,2);
     rc = rc+2;
   }
	#endif

        /* Float fa[17] declares an array of floating point
        numbers, and *afp[17] declares an array of pointers
        to floats.
                                                                */

   for(j=0; j<17; j++){
     fa[j] = j;
     afp[j] = &fa[j];
   }

   #ifndef NO_FLOATS
   sum = 0.;
   #else
   sum = 0;
   #endif
   for(j=0; j<17; j++) sum += *afp[j];
   if(sum != 136){
     if(pd0->flgd != 0) printf(s84er,4);
     rc = rc+4;
   }

        /*  static int x3d[3][5][7] declares a static three
        dimensional array of integers, with rank 3x5x7.
        In complete detail, x3d is an array of three items;
        each item is an array of five arrays, and each of
        the latter arrays is an array of seven integers.
        Any of the expressions x3d, x3d[i], x3d[i][j],
        and x3d[i][j][k] may reasonably appear in an express-
        ion. The first three have type "array"; the last has
        type int.
                                                                */

   for (i=0; i<3; i++)
     for (j=0; j<5; j++)
       for (k=0; k<7; k++)
         x3d[i][j][k] = i*35+j*7+k;

   i = 1; j = 2; k = 3;

   /* kludges */
   #if defined(FORCE_POINTERS) | defined(NO_OLD_FUNC_DECL)
   if( array((int*)x3d,105,0)
      +array((int*)x3d[i],35,35)
   #else
   if( array(x3d,105,0)
      +array(x3d[i],35,35)
   #endif
      +array(x3d[i][j],7,49)
      +      x3d[i][j][k]-52){
      if(pd0->flgd != 0) printf(s84er,8);
      rc = rc+8;
   }

   return rc;
}

#ifndef NO_OLD_FUNC_DECL
array(a,size,start)
int a[],size,start;
#else
int array(int a[],int size,int start)
#endif
{
/*
#ifndef NO_OLD_FUNC_DECL
array(a,size,start)
int a[],
#else
int array(int a[],
#endif
#ifdef NO_TYPELESS_INT
int
#endif
#ifdef NO_TYPELESS_INT
int
#endif

#ifndef NO_OLD_FUNC_DECL
start; {
#else
start){
#endif
*/
   int i;
   for(i=0; i<size; i++)
     if(a[i] != i+start) return 1;

   return 0;
}
#ifndef NO_OLD_FUNC_DECL
int *fip(x)
int x;
{
#else
int *fip(int x){
#endif
   static int y;
   y = x;
   return &y;
}
#ifndef NO_OLD_FUNC_DECL
glork(x)
int x;
{
#else
int glork(int x){
#endif
return x;}

/*********************************************************************************************
 the main loop that launches the sections
*********************************************************************************************/

#ifndef NO_TYPELESS_STRUCT_PTR
	int section(int j,struct* pd0){
#else
	int section(int j,void* pd0){
#endif
        switch(j){
                case 0: return s84(pd0);
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
