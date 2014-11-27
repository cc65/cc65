/*
  !!DESCRIPTION!! C-Manual Chapter 2.5: strings
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
s25(pd0)
struct defs *pd0;
{
#else
int s25(struct defs *pd0) {
#endif
   char *s, *s2;
   int rc, lrc, j;
   static char s25er[] = "s25,er%d\n";
   static char qs25[8] = "s25    ";
   char *ps, *pt;

   ps = qs25;
   pt = pd0->rfs;
   while(*pt++ = *ps++);
   rc = 0;

   /* A string is a sequence of characters surrounded by double
      quotes, as in "...".                         */

   s = "...";

   /* A string has type "array of characters" and storage class
      static and is initialized with the given characters.  */

   if ( s[0] != s[1] || s[1] != s[2]
     || s[2] != '.' ) {
    rc = rc+1;
     if(pd0->flgd != 0) printf(s25er,1);
   }

   /* The compiler places a null byte \0 at the end of each string
      so the program which scans the string can find its end.   */

   if( s[3] != '\0' ){
     rc = rc+4;
     if(pd0->flgd != 0) printf(s25er,4);
   }

   /* In a string, the double quote character " must be preceded
      by a \.                                               */

   if( ".\"."[1] != '"' ){
    rc = rc+8;
     if(pd0->flgd != 0) printf(s25er,8);
   }

   /* In addition, the same escapes described for character constants
      may be used.                                            */

   s = "\n\t\b\r\f\\\'";

   if( s[0] != '\n'
    || s[1] != '\t'
    || s[2] != '\b'
    || s[3] != '\r'
    || s[4] != '\f'
    || s[5] != '\\'
    || s[6] != '\'' ){
     rc = rc+16;
     if( pd0->flgd != 0) printf(s25er,16);
   }

   /* Finally, a \ and an immediately following newline are ignored */

   s2 = "queep!";
   s = "queep!";

   lrc = 0;
   for (j=0; j<sizeof "queep!"; j++) if(s[j] != s2[j]) lrc = 1;
   if (lrc != 0){
     rc = rc+32;
     if(pd0->flgd != 0) printf(s25er,32);
   }
   return rc;
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
                case 0: return s25(pd0);
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
