/*
  !!DESCRIPTION!! C-Manual Chapter 8.6: Initialization
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

#ifdef NO_LOCAL_PROTOTYPES
int one();
#endif

#ifndef NO_OLD_FUNC_DECL
s86(pd0)          /*  8.6 Initialization  */
struct defs *pd0;
{
#else
int s86(struct defs *pd0){
#endif
   static char s86er[] = "s86,er%d\n";
   static char qs86[8] = "s86    ";
   int lrc, rc;
   char *ps, *pt;
   #ifndef NO_LOCAL_PROTOTYPES
   int one();
   #endif
   int i, j, k;
   static int x[] = {1,3,5};
   static int *pint = x+2;
   static int zero[10];
   int *apint = pint-1;
   register int *rpint = apint+one();

   #ifndef NO_FLOATS
   static float y0[] = {1,3,5,2,4,6,3,5,7,0,0,0};
   static float y1[4][3] = {
     {1,3,5},
     {2,4,6},
     {3,5,7},
   };
   static float y2[4][3] = {1,3,5,2,4,6,3,5,7};
   static float y3[4][3] = {
     {1},{2},{3},{4}
   };
   #else
   static signed y0[] = {1,3,5,2,4,6,3,5,7,0,0,0};
   static signed y1[4][3] = {
     {1,3,5},
     {2,4,6},
     {3,5,7},
   };
   #ifndef NO_SLOPPY_STRUCT_INIT
   static signed y2[4][3] = {1,3,5,2,4,6,3,5,7};
   #else
   static signed y2[4][3] = {{1,3,5},{2,4,6},{3,5,7}};
   #endif
   static signed y3[4][3] = {
     {1},{2},{3},{4}
   };
   #endif

   ps = qs86;
   pt = pd0->rfs;
   rc = 0;
   while (*pt++ = *ps++);

        /* The expression in an initializer for a static or
        external variable must be a constant expression or
        an expression that reduces to the address of a pre-
        viously declared variable, possibly offset by a
        constant expression.
                                                                */

   if(*pint != 5){
     if(pd0->flgd != 0) printf(s86er,1);
     rc = rc+1;
   }

        /* Automatic and register variables may be initialized
        by arbitrary expressions involving constants and previously
        declared variables and functions.
                                                                */

   if(*apint != 3){
     if(pd0->flgd != 0) printf(s86er,2);
     rc = rc+2;
   }

   if(*rpint != 5){
     if(pd0->flgd != 0) printf(s86er,4);
     rc = rc+4;
   }

        /* Static variables that are not initialized are guar-
        anteed to start off as zero.
                                                        */

   lrc = 0;
   for(j=0; j<10; j++)
     if(zero[j] != 0) lrc = 1;
   if(lrc != 0){
     if(pd0->flgd != 0) printf(s86er,8);
     rc = rc+8;
   }

        /* y0, y1, and y2, as declared, should define and
        initialize identical arrays.
                                                                */
   lrc = 0;
   for(i=0; i<4; i++)
     for(j=0; j<3; j++){
       k = 3*i+j;
       if( y1[i][j] != y2[i][j]
         ||y1[i][j] != y0[k]) lrc = 1;
     }

   if(lrc != 0){
     if(pd0->flgd != 0) printf(s86er,16);
     rc = rc+16;
   }

        /* y3 initializes the first column of the array and
        leaves the rest zero.
                                                                */

   lrc = 0;
   for(j=0; j<4; j++) if(y3[j][0] != j+1) lrc = 1;

   if(lrc != 0){
     if(pd0->flgd != 0) printf(s86er,32);
     rc = rc+32;
   }
   return rc;
}
#ifndef NO_OLD_FUNC_DECL
one(){
#else
int one(){
#endif
   return 1;
}
int *metricp;

/*********************************************************************************************
 the main loop that launches the sections
*********************************************************************************************/

#ifndef NO_TYPELESS_STRUCT_PTR
	int section(int j,struct* pd0){
#else
	int section(int j,void* pd0){
#endif
        switch(j){
                case 0: return s86(pd0);
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
