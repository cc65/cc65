/*
  !!DESCRIPTION!! C-Manual Chapter 4: what's in a name?
  !!ORIGIN!!      LCC 4.1 Testsuite
  !!LICENCE!!     own, freely distributeable for non-profit. read CPYRIGHT.LCC
*/

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

#ifdef NO_IMPLICIT_FUNC_PROTOTYPES
int s4(struct defs *pd0);
int svtest(int n);
zero();
testev();
setev();
#endif

#ifndef NO_OLD_FUNC_DECL
s4(pd0)                    /* 4. What's in a name?             */
struct defs *pd0;
{
#else
int s4(struct defs *pd0) {
#endif
   static char s4er[] = "s4,er%d\n";
   static char qs4[8] = "s4     ";
   char *ps, *pt;
   int j, rc;

   short sint;             /* short integer, for size test      */
   int pint;               /* plain                             */
   long lint;              /* long                              */
   unsigned target;
   unsigned int mask;

   rc = 0;
   ps = qs4;
   pt = pd0->rfs;

   while(*pt++ = *ps++);

/*   There are four declarable storage classes: automatic,
static, external, and register. Automatic variables have
been dealt with extensively thus far, and will not be specif-
ically treated in this section. Register variables are treated
in section s81.

     Static variables are local to a block, but retain their
values upon reentry to a block, even after control has left
the block.                                                     */

   for (j=0; j<3; j++)
     if(svtest(j) != zero()){
       rc = 1;
       if(pd0->flgd != 0) printf(s4er,1);
     }
   ;

/*   External variables exist and retain their values throughout
the execution of the entire program, and may be used for comm-
unication between functions, even separately compiled functions.
                                                                */

   setev();
   if(testev() != 0){
     rc=rc+2;
     if(pd0->flgd != 0) printf(s4er,2);
   }
/*
     Characters have been tested elsewhere (in s243).

     Up to three sizes of integer, declared short int, int, and
long int, are available. Longer integers provide no less storage
than shorter ones, but implementation may make either short
integers, or long integers, or both, equivalent to plain
integers.
                                                                */

   if(sizeof lint < sizeof pint || sizeof pint < sizeof sint){
     rc = rc+4;
     if(pd0->flgd != 0) printf(s4er,4);
   }

/*   Unsigned integers, declared unsigned, obey the laws of
arithmetic modulo 2**n, where n is the number of bits in the
implementation                                                  */

   target = ~0U;
   mask = 1;

   for(j=0; j<(sizeof target)*pd0->cbits; j++){
     mask = mask&target;
     target = target>>1;
   }

   if(mask != 1 || target != 0){
     rc = rc+8;
     if(pd0->flgd != 0) printf(s4er,8);
   }

   return rc;
}

#ifndef NO_OLD_FUNC_DECL
svtest(n)
int n;
{
#else
int svtest(int n) {
#endif

   static k;
   int rc;
   switch (n) {
     case 0: k = 1978;
             rc = 0;
             break;

     case 1: if(k != 1978) rc = 1;
             else{
              k = 1929;
              rc = 0;
             }
             break;

     case 2: if(k != 1929) rc = 1;
             else rc = 0;
             break;
   }
   return rc;
}
zero(){                 /* Returns a value of zero, possibly */
   static k;            /* with side effects, as it's called */
   int rc;              /* alternately with svtest, above,   */
   k = 2;               /* and has the same internal storage */
   rc = 0;              /* requirements.                     */
   return rc;
}
testev(){
   if(extvar != 1066) return 1;
   else return 0;
}

/* Sets an external variable. Used  */
/* by s4, and should be compiled    */
/* separately from s4.              */

setev(){
#ifndef NO_SLOPPY_EXTERN
   extern int extvar;
#endif
   extvar = 1066;
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
                case 0: return s4(pd0);
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
