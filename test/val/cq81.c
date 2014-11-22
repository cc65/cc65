/*
  !!DESCRIPTION!! C-Manual Chapter 8.1: storage class specifiers
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

#ifdef NO_IMPLICIT_FUNC_PROTOTYPES
regc();
regp();
regi();
#endif

#ifndef NO_OLD_FUNC_DECL
s81(pd0)              /* 8.1 Storage Class Specifiers    */
struct defs *pd0;
#else
int s81(struct defs *pd0)
#endif
{
   static char s81er[] = "s81,er%d\n";
   static char qs81[8] = "s81    ";
   char *ps, *pt;
   int k, rc, j, crc, prc, irc;
   register char rchar;
            char nrchar;
   register int *rptr;
            int *nrptr;
   register int rint;
            int nrint;
   static char badtest[] = "Register count for %s is unreliable.\n";
   static char goodtest[] = "%d registers assigned to %s variables.\n";

   rc = 0;
   crc = 0;
   prc = 0;
   irc = 0;
   ps = qs81;
   pt = pd0->rfs;

   while(*pt++ = *ps++);

/*    The storage class specifiers are:

        auto
        static
        extern
        register
        typedef

      The first three of these were treated earlier, in s4. The last
   will be checked in s88. "Register" remains.

      There are three flavors of register, viz., char, int and pointer.
   We wish first to ascertain that the representations as register
   are consistent with the corresponding nonregister representations.
                                                                 */

   k = 1;
   for (j=0; j<50; j++){
     rchar = k;
     nrchar = k;
     rptr = &k;
     nrptr = &k;
     rint = k;
     nrint = k;

     if ( rchar != nrchar ) crc = 1;
     if ( rptr != nrptr ) prc = 1;
     if ( rint != nrint ) irc = 1;
     k = k<<1;
   }

   if ( crc != 0 ) {
     rc = rc+1;
     if( pd0 -> flgd != 0 ) printf(s81er,1);
   }

   if ( prc != 0 ) {
     rc = rc+2;
     if( pd0 -> flgd != 0 ) printf(s81er,2);
   }

   if ( irc != 0 ) {
     rc = rc+4;
     if( pd0 -> flgd != 0 ) printf(s81er,4);
   }

/*   Now we check to see if variables are actually being assigned
     to registers.                       */

   k = regc();
   if ( pd0->flgm != 0 ) {
     if ( k < 0 ) printf(badtest,"char");
     else printf(goodtest,k,"char");
   }

   k = regp();
   if ( pd0->flgm != 0 ) {
     if ( k<0 ) printf(badtest,"pointer");
     else printf(goodtest,k,"pointer");
   }

   k = regi();
   if ( pd0->flgm != 0 ) {
     if ( k<0 ) printf(badtest,"int");
     else printf(goodtest,k,"int");
   }

   return rc;
}
regc() {     /*   char to register assignment   */
/*   Testing a variable whose storage class has been spec-
ified as "register" is somewhat tricky, but it can be done in a
fairly reliable fashion by taking advantage of our knowledge of the
ways in which compilers operate. If we declare a collection of vari-
ables of the same storage class, we would expect that, when storage
for these variables is actually allocated, the variables will be
bunched together and ordered according to one of the following
criteria:

     (a) the order in which they were defined.
     (b) the order in which they are used.
     (c) alphabetically.
     (d) the order in which they appear in the compiler's
         symbol table.
     (e) some other way.

     Hence, if we define a sequence of variables in close alpha-
betical order, and use them in the same order in which we define
them, we would expect the differences between the addresses of
successive variables to be constant, except in case (d) where the
symbol table is a hash table, or in case (e). If a subsequence in
the middle of this sequence is selected, and for this subsequence,
every other variable is specified to be "register", and address
differences are taken between adjacent nonregister variables, we would
still expect to find constant differences if the "register" vari-
ables were actually assigned to registers, and some other diff-
erences if they were not. Specifically, if we had N variables
specified as "register" of which the first n were actually ass-
igned to registers, we would expect the sequence of differences
to consist of a number of occurrences of some number, followed by
N-n occurrences of some other number, followed by several occurr-
ences of the first number. If we get a sequence like this, we can
determine, by simple subtraction, how many (if any) variables are
being assigned to registers. If we get some other sequence, we know
that the test is invalid.                                     */

            char r00;
            char r01;
            char r02;
            char r03;
   register char r04;
            char r05;
   register char r06;
            char r07;
   register char r08;
            char r09;
   register char r10;
            char r11;
   register char r12;
            char r13;
   register char r14;
            char r15;
   register char r16;
            char r17;
   register char r18;
            char r19;
   register char r20;
            char r21;
   register char r22;
            char r23;
   register char r24;
            char r25;
   register char r26;
            char r27;
   register char r28;
            char r29;
   register char r30;
            char r31;
   register char r32;
            char r33;
   register char r34;
            char r35;
            char r36;
            char r37;
            char r38;

   int s, n1, n2, nr, j, d[22];
   r00 = 0;
   r01 = 1;
   r02 = 2;
   r03 = 3;
   r04 = 4;
   r05 = 5;
   r06 = 6;
   r07 = 7;
   r08 = 8;
   r09 = 9;
   r10 = 10;
   r11 = 11;
   r12 = 12;
   r13 = 13;
   r14 = 14;
   r15 = 15;
   r16 = 16;
   r17 = 17;
   r18 = 18;
   r19 = 19;
   r20 = 20;
   r21 = 21;
   r22 = 22;
   r23 = 23;
   r24 = 24;
   r25 = 25;
   r26 = 26;
   r27 = 27;
   r28 = 28;
   r29 = 29;
   r30 = 30;
   r31 = 31;
   r32 = 32;
   r33 = 33;
   r34 = 34;
   r35 = 35;
   r36 = 36;
   r37 = 37;
   r38 = 38;

   d[0] = &r01 - &r00;
   d[1] = &r02 - &r01;
   d[2] = &r03 - &r02;
   d[3] = &r05 - &r03;
   d[4] = &r07 - &r05;
   d[5] = &r09 - &r07;
   d[6] = &r11 - &r09;
   d[7] = &r13 - &r11;
   d[8] = &r15 - &r13;
   d[9] = &r17 - &r15;
   d[10] = &r19 - &r17;
   d[11] = &r21 - &r19;
   d[12] = &r23 - &r21;
   d[13] = &r25 - &r23;
   d[14] = &r27 - &r25;
   d[15] = &r29 - &r27;
   d[16] = &r31 - &r29;
   d[17] = &r33 - &r31;
   d[18] = &r35 - &r33;
   d[19] = &r36 - &r35;
   d[20] = &r37 - &r36;
   d[21] = &r38 - &r37;

/*   The following FSM analyzes the string of differences. It accepts
strings of the form a+b+a+ and returns 16 minus the number of bs,
which is the number of variables that actually got into registers.
Otherwise it signals rejection by returning -1., indicating that the
test is unreliable.              */

   n1 = d[0];
   s = 1;

   for (j=0; j<22; j++)
     switch (s) {
       case 1: if (d[j] != n1) {
                n2 = d[j];
                s = 2;
                nr = 1;
               }
               break;
       case 2: if (d[j] == n1) {
                s = 3;
                break;
               }
               if (d[j] == n2) {
                nr = nr+1;
                break;
               }
               s = 4;
               break;
       case 3: if (d[j] != n1) s = 4;
               break;
     }
   ;

   if (s == 3) return 16-nr;
   else return -1;
}
regi() {     /*   int to register assignment    */
/*   Testing a variable whose storage class has been spec-
ified as "register" is somewhat tricky, but it can be done in a
fairly reliable fashion by taking advantage of our knowledge of the
ways in which compilers operate. If we declare a collection of vari-
ables of the same storage class, we would expect that, when storage
for these variables is actually allocated, the variables will be
bunched together and ordered according to one of the following
criteria:

     (a) the order in which they were defined.
     (b) the order in which they are used.
     (c) alphabetically.
     (d) the order in which they appear in the compiler's
         symbol table.
     (e) some other way.

     Hence, if we define a sequence of variables in close alpha-
betical order, and use them in the same order in which we define
them, we would expect the differences between the addresses of
successive variables to be constant, except in case (d) where the
symbol table is a hash table, or in case (e). If a subsequence in
the middle of this sequence is selected, and for this subsequence,
every other variable is specified to be "register", and address
differences are taken between adjacent nonregister variables, we would
still expect to find constant differences if the "register" vari-
ables were actually assigned to registers, and some other diff-
erences if they were not. Specifically, if we had N variables
specified as "register" of which the first n were actually ass-
igned to registers, we would expect the sequence of differences
to consist of a number of occurrences of some number, followed by
N-n occurrences of some other number, followed by several occurr-
ences of the first number. If we get a sequence like this, we can
determine, by simple subtraction, how many (if any) variables are
being assigned to registers. If we get some other sequence, we know
that the test is invalid.                                     */

            int r00;
            int r01;
            int r02;
            int r03;
   register int r04;
            int r05;
   register int r06;
            int r07;
   register int r08;
            int r09;
   register int r10;
            int r11;
   register int r12;
            int r13;
   register int r14;
            int r15;
   register int r16;
            int r17;
   register int r18;
            int r19;
   register int r20;
            int r21;
   register int r22;
            int r23;
   register int r24;
            int r25;
   register int r26;
            int r27;
   register int r28;
            int r29;
   register int r30;
            int r31;
   register int r32;
            int r33;
   register int r34;
            int r35;
            int r36;
            int r37;
            int r38;

   int s, n1, n2, nr, j, d[22];

   r00 = 0;
   r01 = 1;
   r02 = 2;
   r03 = 3;
   r04 = 4;
   r05 = 5;
   r06 = 6;
   r07 = 7;
   r08 = 8;
   r09 = 9;
   r10 = 10;
   r11 = 11;
   r12 = 12;
   r13 = 13;
   r14 = 14;
   r15 = 15;
   r16 = 16;
   r17 = 17;
   r18 = 18;
   r19 = 19;
   r20 = 20;
   r21 = 21;
   r22 = 22;
   r23 = 23;
   r24 = 24;
   r25 = 25;
   r26 = 26;
   r27 = 27;
   r28 = 28;
   r29 = 29;
   r30 = 30;
   r31 = 31;
   r32 = 32;
   r33 = 33;
   r34 = 34;
   r35 = 35;
   r36 = 36;
   r37 = 37;
   r38 = 38;

   d[0] = &r01 - &r00;
   d[1] = &r02 - &r01;
   d[2] = &r03 - &r02;
   d[3] = &r05 - &r03;
   d[4] = &r07 - &r05;
   d[5] = &r09 - &r07;
   d[6] = &r11 - &r09;
   d[7] = &r13 - &r11;
   d[8] = &r15 - &r13;
   d[9] = &r17 - &r15;
   d[10] = &r19 - &r17;
   d[11] = &r21 - &r19;
   d[12] = &r23 - &r21;
   d[13] = &r25 - &r23;
   d[14] = &r27 - &r25;
   d[15] = &r29 - &r27;
   d[16] = &r31 - &r29;
   d[17] = &r33 - &r31;
   d[18] = &r35 - &r33;
   d[19] = &r36 - &r35;
   d[20] = &r37 - &r36;
   d[21] = &r38 - &r37;

/*   The following FSM analyzes the string of differences. It accepts
strings of the form a+b+a+ and returns 16 minus the number of bs,
which is the number of variables that actually got into registers.
Otherwise it signals rejection by returning -1., indicating that the
test is unreliable.              */

   n1 = d[0];
   s = 1;

   for (j=0; j<22; j++)
     switch (s) {
       case 1: if (d[j] != n1) {
                n2 = d[j];
                s = 2;
                nr = 1;
               }
               break;
       case 2: if (d[j] == n1) {
                s = 3;
                break;
               }
               if (d[j] == n2) {
                nr = nr+1;
                break;
               }
               s = 4;
               break;
       case 3: if (d[j] != n1) s = 4;
               break;
     }
   ;

   if (s == 3) return 16-nr;
   else return -1;
}
regp() {     /*   pointer to register assignment   */
/*   Testing a variable whose storage class has been spec-
ified as "register" is somewhat tricky, but it can be done in a
fairly reliable fashion by taking advantage of our knowledge of the
ways in which compilers operate. If we declare a collection of vari-
ables of the same storage class, we would expect that, when storage
for these variables is actually allocated, the variables will be
bunched together and ordered according to one of the following
criteria:

     (a) the order in which they were defined.
     (b) the order in which they are used.
     (c) alphabetically.
     (d) the order in which they appear in the compiler's
         symbol table.
     (e) some other way.

     Hence, if we define a sequence of variables in close alpha-
betical order, and use them in the same order in which we define
them, we would expect the differences between the addresses of
successive variables to be constant, except in case (d) where the
symbol table is a hash table, or in case (e). If a subsequence in
the middle of this sequence is selected, and for this subsequence,
every other variable is specified to be "register", and address
differences are taken between adjacent nonregister variables, we would
still expect to find constant differences if the "register" vari-
ables were actually assigned to registers, and some other diff-
erences if they were not. Specifically, if we had N variables
specified as "register" of which the first n were actually ass-
igned to registers, we would expect the sequence of differences
to consist of a number of occurrences of some number, followed by
N-n occurrences of some other number, followed by several occurr-
ences of the first number. If we get a sequence like this, we can
determine, by simple subtraction, how many (if any) variables are
being assigned to registers. If we get some other sequence, we know
that the test is invalid.                                     */

            int *r00;
            int *r01;
            int *r02;
            int *r03;
   register int *r04;
            int *r05;
   register int *r06;
            int *r07;
   register int *r08;
            int *r09;
   register int *r10;
            int *r11;
   register int *r12;
            int *r13;
   register int *r14;
            int *r15;
   register int *r16;
            int *r17;
   register int *r18;
            int *r19;
   register int *r20;
            int *r21;
   register int *r22;
            int *r23;
   register int *r24;
            int *r25;
   register int *r26;
            int *r27;
   register int *r28;
            int *r29;
   register int *r30;
            int *r31;
   register int *r32;
            int *r33;
   register int *r34;
            int *r35;
            int *r36;
            int *r37;
            int *r38;

   int s, n1, n2, nr, j, d[22];

   r00 = (int *)&r00;
   r01 = (int *)&r01;
   r02 = (int *)&r02;
   r03 = (int *)&r03;
   r04 = (int *)&r05;
   r05 = (int *)&r05;
   r06 = (int *)&r07;
   r07 = (int *)&r07;
   r08 = (int *)&r09;
   r09 = (int *)&r09;
   r10 = (int *)&r11;
   r11 = (int *)&r11;
   r12 = (int *)&r13;
   r13 = (int *)&r13;
   r14 = (int *)&r15;
   r15 = (int *)&r15;
   r16 = (int *)&r17;
   r17 = (int *)&r17;
   r18 = (int *)&r19;
   r19 = (int *)&r19;
   r20 = (int *)&r21;
   r21 = (int *)&r21;
   r22 = (int *)&r23;
   r23 = (int *)&r23;
   r24 = (int *)&r25;
   r25 = (int *)&r25;
   r26 = (int *)&r27;
   r27 = (int *)&r27;
   r28 = (int *)&r29;
   r29 = (int *)&r29;
   r30 = (int *)&r31;
   r31 = (int *)&r31;
   r32 = (int *)&r33;
   r33 = (int *)&r33;
   r34 = (int *)&r35;
   r35 = (int *)&r35;
   r36 = (int *)&r36;
   r37 = (int *)&r37;
   r38 = (int *)&r38;

   d[0] = &r01 - &r00;
   d[1] = &r02 - &r01;
   d[2] = &r03 - &r02;
   d[3] = &r05 - &r03;
   d[4] = &r07 - &r05;
   d[5] = &r09 - &r07;
   d[6] = &r11 - &r09;
   d[7] = &r13 - &r11;
   d[8] = &r15 - &r13;
   d[9] = &r17 - &r15;
   d[10] = &r19 - &r17;
   d[11] = &r21 - &r19;
   d[12] = &r23 - &r21;
   d[13] = &r25 - &r23;
   d[14] = &r27 - &r25;
   d[15] = &r29 - &r27;
   d[16] = &r31 - &r29;
   d[17] = &r33 - &r31;
   d[18] = &r35 - &r33;
   d[19] = &r36 - &r35;
   d[20] = &r37 - &r36;
   d[21] = &r38 - &r37;

/*   The following FSM analyzes the string of differences. It accepts
strings of the form a+b+a+ and returns 16 minus the number of bs,
which is the number of variables that actually got into registers.
Otherwise it signals rejection by returning -1., indicating that the
test is unreliable.              */

   n1 = d[0];
   s = 1;
   for (j=0; j<22; j++)
     switch (s) {
       case 1: if (d[j] != n1) {
                n2 = d[j];
                s = 2;
                nr = 1;
               }
               break;
       case 2: if (d[j] == n1) {
                s = 3;
                break;
               }
               if (d[j] == n2) {
                nr = nr+1;
                break;
               }
               s = 4;
               break;
       case 3: if (d[j] != n1) s = 4;
               break;
     }
   ;

   if (s == 3) return 16-nr;
   else return -1;
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
                case 0: return s81(pd0);
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
