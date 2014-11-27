/*
  !!DESCRIPTION!! C-Manual Chapter 8.5: Structure and Union declarations
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
s85(pd0)          /*  8.5 Structure and union declarations   */
struct defs *pd0;
{
#else
int s85(struct defs *pd0){
#endif
   static char s85er[] = "s85,er%d\n";
   static char qs85[8] = "s85    ";
   int rc;
   char *ps, *pt;

   struct tnode {
     char tword[20];
     int count;
     struct tnode *left;
     struct tnode *right;
   };

   struct tnode s1, s2, *sp;

   struct{
     char cdummy;
     char c;
   } sc;

   struct{
     char cdummy;
     short s;
   } ss;

   struct{
     char cdummy;
     int i;
   } si;

   struct{
     char cdummy;
     long l;
   } sl;

   struct{
     char cdummy;
     unsigned u;
   } su;

   struct{
     char cdummy;
     #ifndef NO_FLOATS
     float f;
     #else
     signed f;
     #endif
   } sf;

   struct{
     char cdummy;
     #ifndef NO_FLOATS
     double d;
     #else
     signed d;
     #endif
   } sd;

   int diff[7], j;

   static char *type[] = {
     "char",
     "short",
     "int",
     "long",
     "unsigned",
     #ifdef NO_FLOATS
     "signed",
     "signed",
	 #else
     "float",
     "double"
	 #endif
   };

   static char aln[] = " alignment: ";

   #ifndef NO_BITFIELDS
   struct{
     int twobit:2;
     int       :1;
     int threebit:3;
     int onebit:1;
   } s3;
   #else
   struct{
     unsigned char twobit;
     unsigned char threebit;
     unsigned char onebit;
   } s3;
   #endif

   union{
     char u1[30];
     short u2[30];
     int u3[30];
     long u4[30];
     unsigned u5[30];
     #ifndef NO_FLOATS
     float u6[30];
     double u7[30];
     #else
     signed u6[30];
     signed u7[30];
     #endif
   } u0;

   ps = qs85;
   pt = pd0->rfs;
   rc = 0;
   while (*pt++ = *ps++);

        /* Within a structure, the objects declared have
        addresses which increase as their declarations are
        read left to right.
                                                                */

   if( (char *)&s1.count - &s1.tword[0] <= 0
     ||(char *)&s1.left - (char *)&s1.count <= 0
     ||(char *)&s1.right - (char *)&s1.left <= 0){
     if(pd0->flgd != 0) printf(s85er,1);
     rc = rc+1;
   }

        /* Each non-field member of a structure begins on an
        addressing boundary appropriate to its type.
                                                                */

   diff[0] = &sc.c - &sc.cdummy;
   diff[1] = (char *)&ss.s - &ss.cdummy;
   diff[2] = (char *)&si.i - &si.cdummy;
   diff[3] = (char *)&sl.l - &sl.cdummy;
   diff[4] = (char *)&su.u - &su.cdummy;
   diff[5] = (char *)&sf.f - &sf.cdummy;
   diff[6] = (char *)&sd.d - &sd.cdummy;

   if(pd0->flgm != 0)
    for(j=0; j<7; j++)
     printf("%s%s%d\n",type[j],aln,diff[j]);

        /* Field specifications are highly implementation de-
        pendent. About the only thing we can do here is to
        check is that the compiler accepts the field constructs,
        and that they seem to work, after a fashion, at
        run time...
                                                                */

   s3.threebit = 7;
   s3.twobit = s3.threebit;
   s3.threebit = s3.twobit;

   if(s3.threebit != 3){
     if(s3.threebit == -1){
       if(pd0->flgm != 0) printf("Sign extension in fields\n");
     }
     else{
   	   #ifdef NO_BITFIELDS
       		if(pd0->flgd != 0) printf("NO_BITFIELDS\n");
	   #else
       		if(pd0->flgd != 0) printf(s85er,2);
       		rc = rc+2;
	   #endif
     }
   }

   s3.onebit = 1;
   if(s3.onebit != 1){
     if(pd0->flgm != 0)
      printf("Be especially careful with 1-bit fields!\n");
   }

        /* A union may be thought of as a structure all of whose
        members begin at offset 0 and whose size is sufficient
        to contain any of its members.
                                                                */

   if( (char *)u0.u1 - (char *)&u0 != 0
     ||(char *)u0.u2 - (char *)&u0 != 0
     ||(char *)u0.u3 - (char *)&u0 != 0
     ||(char *)u0.u4 - (char *)&u0 != 0
     ||(char *)u0.u5 - (char *)&u0 != 0
     ||(char *)u0.u6 - (char *)&u0 != 0
     ||(char *)u0.u7 - (char *)&u0 != 0){
     if(pd0->flgd != 0) printf(s85er,4);
     rc = rc+4;
   }

   if( sizeof u0 < sizeof u0.u1
     ||sizeof u0 < sizeof u0.u2
     ||sizeof u0 < sizeof u0.u3
     ||sizeof u0 < sizeof u0.u4
     ||sizeof u0 < sizeof u0.u5
     ||sizeof u0 < sizeof u0.u6
     ||sizeof u0 < sizeof u0.u7){
     if(pd0->flgd != 0) printf(s85er,8);
     rc = rc+8;
   }

        /* Finally, we check that the pointers work.            */

   s1.right = &s2;
   s2.tword[0] = 2;
   s1.right->tword[0] += 1;
   if(s2.tword[0] != 3){
     if(pd0->flgd != 0) printf(s85er,16);
     rc = rc+16;
   }
   return rc;
}

#ifdef NO_LOCAL_PROTOTYPES
int one();
#endif

/*********************************************************************************************
 the main loop that launches the sections
*********************************************************************************************/

#ifndef NO_TYPELESS_STRUCT_PTR
	int section(int j,struct* pd0){
#else
	int section(int j,void* pd0){
#endif
        switch(j){
                case 0: return s85(pd0);
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
