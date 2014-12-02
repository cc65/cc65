/*
  !!DESCRIPTION!! C-Manual Chapter 7.14: assignment operators
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
s714(pd0)          /*  7.14  Assignment operators       */
struct defs *pd0;
{
#else
int s714(struct defs *pd0){
#endif
   static char f[] = "Local error %d.\n";
   static char s714er[] = "s714,er%d\n";
   static char qs714[8] = "s714   ";
   register int prlc, lrc;
   int rc;
   char cl, cr;
   short sl, sr;
   int il, ir;
   long ll, lr;
   unsigned ul, ur;
   #ifndef NO_FLOATS
   float fl, fr;
   double dl, dr;
   #else
   signed fl, fr;
   signed dl, dr;
   #endif
   char *ps, *pt;
   ps = qs714;
   pt = pd0->rfs;
   rc = 0;
   lrc = 0;
   prlc = pd0->flgl;
   while (*pt++ = *ps++);

        /* This section tests the assignment operators.

        It is an exhaustive test of all assignment statements
        of the form:

                vl op vr

        where vl and vr are variables from the set
        {char,short,int,long,unsigned,float,double} and op is
        one of the assignment operators. There are 395 such
        statements.

        The initial values for the variables have been chosen
        so that both the initial values and the results will
        "fit" in just about any implementation, and that the re-
        sults will be such that they test for the proper form-
        ation of composite operators, rather than checking for
        the valid operation of those operators' components.
        For example, in checking >>=, we want to verify that
        a right shift and a move take place, rather than
        whether or not there may be some peculiarities about
        the right shift. Such tests have been made previously,
        and to repeat them here would be to throw out a red
        herring.

        The table below lists the operators, assignment targets,
        initial values for left and right operands, and the
        expected values of the results.

          =  +=  -=  *=  /=  %=  >>=  <<=  &=  ^=  |=
char      2   7   3  10   2   1   1    20   8   6  14
short     2   7   3  10   2   1   1    20   8   6  14
int       2   7   3  10   2   1   1    20   8   6  14
long      2   7   3  10   2   1   1    20   8   6  14
unsigned  2   7   3  10   2   1   1    20   8   6  14
float     2   7   3  10 2.5 |             |
double    2   7   3  10 2.5 |             |
                            |             |
initial         (5,2)       |    (5,2)    |  (12,10)

        The following machine-generated program reflects the
        tests described in the table.
                                                                */

   cl = 5; cr = 2;
   cl = cr;
   if(cl != 2){
     lrc = 1;
     if(prlc) printf(f,lrc);
   }
   cl = 5; sr = 2;
   cl = sr;
   if(cl != 2){
     lrc = 2;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ir = 2;
   cl = ir;
   if(cl != 2){
     lrc = 3;
     if(prlc) printf(f,lrc);
   }
   cl = 5; lr = 2;
   cl = lr;
   if(cl != 2){
     lrc = 4;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ur = 2;
   cl = ur;
   if(cl != 2){
     lrc = 5;
     if(prlc) printf(f,lrc);
   }
   cl = 5; fr = 2;
   cl = fr;
   if(cl != 2){
     lrc = 6;
     if(prlc) printf(f,lrc);
   }
   cl = 5; dr = 2;
   cl = dr;
   if(cl != 2){
     lrc = 7;
     if(prlc) printf(f,lrc);
   }
   sl = 5; cr = 2;
   sl = cr;
   if(sl != 2){
     lrc = 8;
     if(prlc) printf(f,lrc);
   }
   sl = 5; sr = 2;
   sl = sr;
   if(sl != 2){
     lrc = 9;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ir = 2;
   sl = ir;
   if(sl != 2){
     lrc = 10;
     if(prlc) printf(f,lrc);
   }
   sl = 5; lr = 2;
   sl = lr;
   if(sl != 2){
     lrc = 11;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ur = 2;
   sl = ur;
   if(sl != 2){
     lrc = 12;
     if(prlc) printf(f,lrc);
   }
   sl = 5; fr = 2;
   sl = fr;
   if(sl != 2){
     lrc = 13;
     if(prlc) printf(f,lrc);
   }
   sl = 5; dr = 2;
   sl = dr;
   if(sl != 2){
     lrc = 14;
     if(prlc) printf(f,lrc);
   }
   il = 5; cr = 2;
   il = cr;
   if(il != 2){
     lrc = 15;
     if(prlc) printf(f,lrc);
   }
   il = 5; sr = 2;
   il = sr;
   if(il != 2){
     lrc = 16;
     if(prlc) printf(f,lrc);
   }
   il = 5; ir = 2;
   il = ir;
   if(il != 2){
     lrc = 17;
     if(prlc) printf(f,lrc);
   }
   il = 5; lr = 2;
   il = lr;
   if(il != 2){
     lrc = 18;
     if(prlc) printf(f,lrc);
   }
   il = 5; ur = 2;
   il = ur;
   if(il != 2){
     lrc = 19;
     if(prlc) printf(f,lrc);
   }
   il = 5; fr = 2;
   il = fr;
   if(il != 2){
     lrc = 20;
     if(prlc) printf(f,lrc);
   }
   il = 5; dr = 2;
   il = dr;
   if(il != 2){
     lrc = 21;
     if(prlc) printf(f,lrc);
   }
   ll = 5; cr = 2;
   ll = cr;
   if(ll != 2){
     lrc = 22;
     if(prlc) printf(f,lrc);
   }
   ll = 5; sr = 2;
   ll = sr;
   if(ll != 2){
     lrc = 23;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ir = 2;
   ll = ir;
   if(ll != 2){
     lrc = 24;
     if(prlc) printf(f,lrc);
   }
   ll = 5; lr = 2;
   ll = lr;
   if(ll != 2){
     lrc = 25;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ur = 2;
   ll = ur;
   if(ll != 2){
     lrc = 26;
     if(prlc) printf(f,lrc);
   }
   ll = 5; fr = 2;
   ll = fr;
   if(ll != 2){
     lrc = 27;
     if(prlc) printf(f,lrc);
   }
   ll = 5; dr = 2;
   ll = dr;
   if(ll != 2){
     lrc = 28;
     if(prlc) printf(f,lrc);
   }
   ul = 5; cr = 2;
   ul = cr;
   if(ul != 2){
     lrc = 29;
     if(prlc) printf(f,lrc);
   }
   ul = 5; sr = 2;
   ul = sr;
   if(ul != 2){
     lrc = 30;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ir = 2;
   ul = ir;
   if(ul != 2){
     lrc = 31;
     if(prlc) printf(f,lrc);
   }
   ul = 5; lr = 2;
   ul = lr;
   if(ul != 2){
     lrc = 32;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ur = 2;
   ul = ur;
   if(ul != 2){
     lrc = 33;
     if(prlc) printf(f,lrc);
   }
   ul = 5; fr = 2;
   ul = fr;
   if(ul != 2){
     lrc = 34;
     if(prlc) printf(f,lrc);
   }
   ul = 5; dr = 2;
   ul = dr;
   if(ul != 2){
     lrc = 35;
     if(prlc) printf(f,lrc);
   }
   fl = 5; cr = 2;
   fl = cr;
   if(fl != 2){
     lrc = 36;
     if(prlc) printf(f,lrc);
   }
   fl = 5; sr = 2;
   fl = sr;
   if(fl != 2){
     lrc = 37;
     if(prlc) printf(f,lrc);
   }
   fl = 5; ir = 2;
   fl = ir;
   if(fl != 2){
     lrc = 38;
     if(prlc) printf(f,lrc);
   }
   fl = 5; lr = 2;
   fl = lr;
   if(fl != 2){
     lrc = 39;
     if(prlc) printf(f,lrc);
   }
   fl = 5; ur = 2;
   fl = ur;
   if(fl != 2){
     lrc = 40;
     if(prlc) printf(f,lrc);
   }
   fl = 5; fr = 2;
   fl = fr;
   if(fl != 2){
     lrc = 41;
     if(prlc) printf(f,lrc);
   }
   fl = 5; dr = 2;
   fl = dr;
   if(fl != 2){
     lrc = 42;
     if(prlc) printf(f,lrc);
   }
   dl = 5; cr = 2;
   dl = cr;
   if(dl != 2){
     lrc = 43;
     if(prlc) printf(f,lrc);
   }
   dl = 5; sr = 2;
   dl = sr;
   if(dl != 2){
     lrc = 44;
     if(prlc) printf(f,lrc);
   }
   dl = 5; ir = 2;
   dl = ir;
   if(dl != 2){
     lrc = 45;
     if(prlc) printf(f,lrc);
   }
   dl = 5; lr = 2;
   dl = lr;
   if(dl != 2){
     lrc = 46;
     if(prlc) printf(f,lrc);
   }
   dl = 5; ur = 2;
   dl = ur;
   if(dl != 2){
     lrc = 47;
     if(prlc) printf(f,lrc);
   }
   dl = 5; fr = 2;
   dl = fr;
   if(dl != 2){
     lrc = 48;
     if(prlc) printf(f,lrc);
   }
   dl = 5; dr = 2;
   dl = dr;
   if(dl != 2){
     lrc = 49;
     if(prlc) printf(f,lrc);
   }
   cl = 5; cr = 2;
   cl += cr;
   if(cl != 7){
     lrc = 50;
     if(prlc) printf(f,lrc);
   }
   cl = 5; sr = 2;
   cl += sr;
   if(cl != 7){
     lrc = 51;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ir = 2;
   cl += ir;
   if(cl != 7){
     lrc = 52;
     if(prlc) printf(f,lrc);
   }
   cl = 5; lr = 2;
   cl += lr;
   if(cl != 7){
     lrc = 53;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ur = 2;
   cl += ur;
   if(cl != 7){
     lrc = 54;
     if(prlc) printf(f,lrc);
   }
   cl = 5; fr = 2;
   cl += fr;
   if(cl != 7){
     lrc = 55;
     if(prlc) printf(f,lrc);
   }
   cl = 5; dr = 2;
   cl += dr;
   if(cl != 7){
     lrc = 56;
     if(prlc) printf(f,lrc);
   }
   sl = 5; cr = 2;
   sl += cr;
   if(sl != 7){
     lrc = 57;
     if(prlc) printf(f,lrc);
   }
   sl = 5; sr = 2;
   sl += sr;
   if(sl != 7){
     lrc = 58;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ir = 2;
   sl += ir;
   if(sl != 7){
     lrc = 59;
     if(prlc) printf(f,lrc);
   }
   sl = 5; lr = 2;
   sl += lr;
   if(sl != 7){
     lrc = 60;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ur = 2;
   sl += ur;
   if(sl != 7){
     lrc = 61;
     if(prlc) printf(f,lrc);
   }
   sl = 5; fr = 2;
   sl += fr;
   if(sl != 7){
     lrc = 62;
     if(prlc) printf(f,lrc);
   }
   sl = 5; dr = 2;
   sl += dr;
   if(sl != 7){
     lrc = 63;
     if(prlc) printf(f,lrc);
   }
   il = 5; cr = 2;
   il += cr;
   if(il != 7){
     lrc = 64;
     if(prlc) printf(f,lrc);
   }
   il = 5; sr = 2;
   il += sr;
   if(il != 7){
     lrc = 65;
     if(prlc) printf(f,lrc);
   }
   il = 5; ir = 2;
   il += ir;
   if(il != 7){
     lrc = 66;
     if(prlc) printf(f,lrc);
   }
   il = 5; lr = 2;
   il += lr;
   if(il != 7){
     lrc = 67;
     if(prlc) printf(f,lrc);
   }
   il = 5; ur = 2;
   il += ur;
   if(il != 7){
     lrc = 68;
     if(prlc) printf(f,lrc);
   }
   il = 5; fr = 2;
   il += fr;
   if(il != 7){
     lrc = 69;
     if(prlc) printf(f,lrc);
   }
   il = 5; dr = 2;
   il += dr;
   if(il != 7){
     lrc = 70;
     if(prlc) printf(f,lrc);
   }
   ll = 5; cr = 2;
   ll += cr;
   if(ll != 7){
     lrc = 71;
     if(prlc) printf(f,lrc);
   }
   ll = 5; sr = 2;
   ll += sr;
   if(ll != 7){
     lrc = 72;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ir = 2;
   ll += ir;
   if(ll != 7){
     lrc = 73;
     if(prlc) printf(f,lrc);
   }
   ll = 5; lr = 2;
   ll += lr;
   if(ll != 7){
     lrc = 74;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ur = 2;
   ll += ur;
   if(ll != 7){
     lrc = 75;
     if(prlc) printf(f,lrc);
   }
   ll = 5; fr = 2;
   ll += fr;
   if(ll != 7){
     lrc = 76;
     if(prlc) printf(f,lrc);
   }
   ll = 5; dr = 2;
   ll += dr;
   if(ll != 7){
     lrc = 77;
     if(prlc) printf(f,lrc);
   }
   ul = 5; cr = 2;
   ul += cr;
   if(ul != 7){
     lrc = 78;
     if(prlc) printf(f,lrc);
   }
   ul = 5; sr = 2;
   ul += sr;
   if(ul != 7){
     lrc = 79;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ir = 2;
   ul += ir;
   if(ul != 7){
     lrc = 80;
     if(prlc) printf(f,lrc);
   }
   ul = 5; lr = 2;
   ul += lr;
   if(ul != 7){
     lrc = 81;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ur = 2;
   ul += ur;
   if(ul != 7){
     lrc = 82;
     if(prlc) printf(f,lrc);
   }
   ul = 5; fr = 2;
   ul += fr;
   if(ul != 7){
     lrc = 83;
     if(prlc) printf(f,lrc);
   }
   ul = 5; dr = 2;
   ul += dr;
   if(ul != 7){
     lrc = 84;
     if(prlc) printf(f,lrc);
   }
   fl = 5; cr = 2;
   fl += cr;
   if(fl != 7){
     lrc = 85;
     if(prlc) printf(f,lrc);
   }
   fl = 5; sr = 2;
   fl += sr;
   if(fl != 7){
     lrc = 86;
     if(prlc) printf(f,lrc);
   }
   fl = 5; ir = 2;
   fl += ir;
   if(fl != 7){
     lrc = 87;
     if(prlc) printf(f,lrc);
   }
   fl = 5; lr = 2;
   fl += lr;
   if(fl != 7){
     lrc = 88;
     if(prlc) printf(f,lrc);
   }
   fl = 5; ur = 2;
   fl += ur;
   if(fl != 7){
     lrc = 89;
     if(prlc) printf(f,lrc);
   }
   fl = 5; fr = 2;
   fl += fr;
   if(fl != 7){
     lrc = 90;
     if(prlc) printf(f,lrc);
   }
   fl = 5; dr = 2;
   fl += dr;
   if(fl != 7){
     lrc = 91;
     if(prlc) printf(f,lrc);
   }
   dl = 5; cr = 2;
   dl += cr;
   if(dl != 7){
     lrc = 92;
     if(prlc) printf(f,lrc);
   }
   dl = 5; sr = 2;
   dl += sr;
   if(dl != 7){
     lrc = 93;
     if(prlc) printf(f,lrc);
   }
   dl = 5; ir = 2;
   dl += ir;
   if(dl != 7){
     lrc = 94;
     if(prlc) printf(f,lrc);
   }
   dl = 5; lr = 2;
   dl += lr;
   if(dl != 7){
     lrc = 95;
     if(prlc) printf(f,lrc);
   }
   dl = 5; ur = 2;
   dl += ur;
   if(dl != 7){
     lrc = 96;
     if(prlc) printf(f,lrc);
   }
   dl = 5; fr = 2;
   dl += fr;
   if(dl != 7){
     lrc = 97;
     if(prlc) printf(f,lrc);
   }
   dl = 5; dr = 2;
   dl += dr;
   if(dl != 7){
     lrc = 98;
     if(prlc) printf(f,lrc);
   }
   cl = 5; cr = 2;
   cl -= cr;
   if(cl != 3){
     lrc = 99;
     if(prlc) printf(f,lrc);
   }
   cl = 5; sr = 2;
   cl -= sr;
   if(cl != 3){
     lrc = 100;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ir = 2;
   cl -= ir;
   if(cl != 3){
     lrc = 101;
     if(prlc) printf(f,lrc);
   }
   cl = 5; lr = 2;
   cl -= lr;
   if(cl != 3){
     lrc = 102;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ur = 2;
   cl -= ur;
   if(cl != 3){
     lrc = 103;
     if(prlc) printf(f,lrc);
   }
   cl = 5; fr = 2;
   cl -= fr;
   if(cl != 3){
     lrc = 104;
     if(prlc) printf(f,lrc);
   }
   cl = 5; dr = 2;
   cl -= dr;
   if(cl != 3){
     lrc = 105;
     if(prlc) printf(f,lrc);
   }
   sl = 5; cr = 2;
   sl -= cr;
   if(sl != 3){
     lrc = 106;
     if(prlc) printf(f,lrc);
   }
   sl = 5; sr = 2;
   sl -= sr;
   if(sl != 3){
     lrc = 107;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ir = 2;
   sl -= ir;
   if(sl != 3){
     lrc = 108;
     if(prlc) printf(f,lrc);
   }
   sl = 5; lr = 2;
   sl -= lr;
   if(sl != 3){
     lrc = 109;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ur = 2;
   sl -= ur;
   if(sl != 3){
     lrc = 110;
     if(prlc) printf(f,lrc);
   }
   sl = 5; fr = 2;
   sl -= fr;
   if(sl != 3){
     lrc = 111;
     if(prlc) printf(f,lrc);
   }
   sl = 5; dr = 2;
   sl -= dr;
   if(sl != 3){
     lrc = 112;
     if(prlc) printf(f,lrc);
   }
   il = 5; cr = 2;
   il -= cr;
   if(il != 3){
     lrc = 113;
     if(prlc) printf(f,lrc);
   }
   il = 5; sr = 2;
   il -= sr;
   if(il != 3){
     lrc = 114;
     if(prlc) printf(f,lrc);
   }
   il = 5; ir = 2;
   il -= ir;
   if(il != 3){
     lrc = 115;
     if(prlc) printf(f,lrc);
   }
   il = 5; lr = 2;
   il -= lr;
   if(il != 3){
     lrc = 116;
     if(prlc) printf(f,lrc);
   }
   il = 5; ur = 2;
   il -= ur;
   if(il != 3){
     lrc = 117;
     if(prlc) printf(f,lrc);
   }
   il = 5; fr = 2;
   il -= fr;
   if(il != 3){
     lrc = 118;
     if(prlc) printf(f,lrc);
   }
   il = 5; dr = 2;
   il -= dr;
   if(il != 3){
     lrc = 119;
     if(prlc) printf(f,lrc);
   }
   ll = 5; cr = 2;
   ll -= cr;
   if(ll != 3){
     lrc = 120;
     if(prlc) printf(f,lrc);
   }
   ll = 5; sr = 2;
   ll -= sr;
   if(ll != 3){
     lrc = 121;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ir = 2;
   ll -= ir;
   if(ll != 3){
     lrc = 122;
     if(prlc) printf(f,lrc);
   }
   ll = 5; lr = 2;
   ll -= lr;
   if(ll != 3){
     lrc = 123;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ur = 2;
   ll -= ur;
   if(ll != 3){
     lrc = 124;
     if(prlc) printf(f,lrc);
   }
   ll = 5; fr = 2;
   ll -= fr;
   if(ll != 3){
     lrc = 125;
     if(prlc) printf(f,lrc);
   }
   ll = 5; dr = 2;
   ll -= dr;
   if(ll != 3){
     lrc = 126;
     if(prlc) printf(f,lrc);
   }
   ul = 5; cr = 2;
   ul -= cr;
   if(ul != 3){
     lrc = 127;
     if(prlc) printf(f,lrc);
   }
   ul = 5; sr = 2;
   ul -= sr;
   if(ul != 3){
     lrc = 128;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ir = 2;
   ul -= ir;
   if(ul != 3){
     lrc = 129;
     if(prlc) printf(f,lrc);
   }
   ul = 5; lr = 2;
   ul -= lr;
   if(ul != 3){
     lrc = 130;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ur = 2;
   ul -= ur;
   if(ul != 3){
     lrc = 131;
     if(prlc) printf(f,lrc);
   }
   ul = 5; fr = 2;
   ul -= fr;
   if(ul != 3){
     lrc = 132;
     if(prlc) printf(f,lrc);
   }
   ul = 5; dr = 2;
   ul -= dr;
   if(ul != 3){
     lrc = 133;
     if(prlc) printf(f,lrc);
   }
   fl = 5; cr = 2;
   fl -= cr;
   if(fl != 3){
     lrc = 134;
     if(prlc) printf(f,lrc);
   }
   fl = 5; sr = 2;
   fl -= sr;
   if(fl != 3){
     lrc = 135;
     if(prlc) printf(f,lrc);
   }
   fl = 5; ir = 2;
   fl -= ir;
   if(fl != 3){
     lrc = 136;
     if(prlc) printf(f,lrc);
   }
   fl = 5; lr = 2;
   fl -= lr;
   if(fl != 3){
     lrc = 137;
     if(prlc) printf(f,lrc);
   }
   fl = 5; ur = 2;
   fl -= ur;
   if(fl != 3){
     lrc = 138;
     if(prlc) printf(f,lrc);
   }
   fl = 5; fr = 2;
   fl -= fr;
   if(fl != 3){
     lrc = 139;
     if(prlc) printf(f,lrc);
   }
   fl = 5; dr = 2;
   fl -= dr;
   if(fl != 3){
     lrc = 140;
     if(prlc) printf(f,lrc);
   }
   dl = 5; cr = 2;
   dl -= cr;
   if(dl != 3){
     lrc = 141;
     if(prlc) printf(f,lrc);
   }
   dl = 5; sr = 2;
   dl -= sr;
   if(dl != 3){
     lrc = 142;
     if(prlc) printf(f,lrc);
   }
   dl = 5; ir = 2;
   dl -= ir;
   if(dl != 3){
     lrc = 143;
     if(prlc) printf(f,lrc);
   }
   dl = 5; lr = 2;
   dl -= lr;
   if(dl != 3){
     lrc = 144;
     if(prlc) printf(f,lrc);
   }
   dl = 5; ur = 2;
   dl -= ur;
   if(dl != 3){
     lrc = 145;
     if(prlc) printf(f,lrc);
   }
   dl = 5; fr = 2;
   dl -= fr;
   if(dl != 3){
     lrc = 146;
     if(prlc) printf(f,lrc);
   }
   dl = 5; dr = 2;
   dl -= dr;
   if(dl != 3){
     lrc = 147;
     if(prlc) printf(f,lrc);
   }
   cl = 5; cr = 2;
   cl *= cr;
   if(cl != 10){
     lrc = 148;
     if(prlc) printf(f,lrc);
   }
   cl = 5; sr = 2;
   cl *= sr;
   if(cl != 10){
     lrc = 149;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ir = 2;
   cl *= ir;
   if(cl != 10){
     lrc = 150;
     if(prlc) printf(f,lrc);
   }
   cl = 5; lr = 2;
   cl *= lr;
   if(cl != 10){
     lrc = 151;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ur = 2;
   cl *= ur;
   if(cl != 10){
     lrc = 152;
     if(prlc) printf(f,lrc);
   }
   cl = 5; fr = 2;
   cl *= fr;
   if(cl != 10){
     lrc = 153;
     if(prlc) printf(f,lrc);
   }
   cl = 5; dr = 2;
   cl *= dr;
   if(cl != 10){
     lrc = 154;
     if(prlc) printf(f,lrc);
   }
   sl = 5; cr = 2;
   sl *= cr;
   if(sl != 10){
     lrc = 155;
     if(prlc) printf(f,lrc);
   }
   sl = 5; sr = 2;
   sl *= sr;
   if(sl != 10){
     lrc = 156;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ir = 2;
   sl *= ir;
   if(sl != 10){
     lrc = 157;
     if(prlc) printf(f,lrc);
   }
   sl = 5; lr = 2;
   sl *= lr;
   if(sl != 10){
     lrc = 158;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ur = 2;
   sl *= ur;
   if(sl != 10){
     lrc = 159;
     if(prlc) printf(f,lrc);
   }
   sl = 5; fr = 2;
   sl *= fr;
   if(sl != 10){
     lrc = 160;
     if(prlc) printf(f,lrc);
   }
   sl = 5; dr = 2;
   sl *= dr;
   if(sl != 10){
     lrc = 161;
     if(prlc) printf(f,lrc);
   }
   il = 5; cr = 2;
   il *= cr;
   if(il != 10){
     lrc = 162;
     if(prlc) printf(f,lrc);
   }
   il = 5; sr = 2;
   il *= sr;
   if(il != 10){
     lrc = 163;
     if(prlc) printf(f,lrc);
   }
   il = 5; ir = 2;
   il *= ir;
   if(il != 10){
     lrc = 164;
     if(prlc) printf(f,lrc);
   }
   il = 5; lr = 2;
   il *= lr;
   if(il != 10){
     lrc = 165;
     if(prlc) printf(f,lrc);
   }
   il = 5; ur = 2;
   il *= ur;
   if(il != 10){
     lrc = 166;
     if(prlc) printf(f,lrc);
   }
   il = 5; fr = 2;
   il *= fr;
   if(il != 10){
     lrc = 167;
     if(prlc) printf(f,lrc);
   }
   il = 5; dr = 2;
   il *= dr;
   if(il != 10){
     lrc = 168;
     if(prlc) printf(f,lrc);
   }
   ll = 5; cr = 2;
   ll *= cr;
   if(ll != 10){
     lrc = 169;
     if(prlc) printf(f,lrc);
   }
   ll = 5; sr = 2;
   ll *= sr;
   if(ll != 10){
     lrc = 170;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ir = 2;
   ll *= ir;
   if(ll != 10){
     lrc = 171;
     if(prlc) printf(f,lrc);
   }
   ll = 5; lr = 2;
   ll *= lr;
   if(ll != 10){
     lrc = 172;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ur = 2;
   ll *= ur;
   if(ll != 10){
     lrc = 173;
     if(prlc) printf(f,lrc);
   }
   ll = 5; fr = 2;
   ll *= fr;
   if(ll != 10){
     lrc = 174;
     if(prlc) printf(f,lrc);
   }
   ll = 5; dr = 2;
   ll *= dr;
   if(ll != 10){
     lrc = 175;
     if(prlc) printf(f,lrc);
   }
   ul = 5; cr = 2;
   ul *= cr;
   if(ul != 10){
     lrc = 176;
     if(prlc) printf(f,lrc);
   }
   ul = 5; sr = 2;
   ul *= sr;
   if(ul != 10){
     lrc = 177;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ir = 2;
   ul *= ir;
   if(ul != 10){
     lrc = 178;
     if(prlc) printf(f,lrc);
   }
   ul = 5; lr = 2;
   ul *= lr;
   if(ul != 10){
     lrc = 179;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ur = 2;
   ul *= ur;
   if(ul != 10){
     lrc = 180;
     if(prlc) printf(f,lrc);
   }
   ul = 5; fr = 2;
   ul *= fr;
   if(ul != 10){
     lrc = 181;
     if(prlc) printf(f,lrc);
   }
   ul = 5; dr = 2;
   ul *= dr;
   if(ul != 10){
     lrc = 182;
     if(prlc) printf(f,lrc);
   }
   fl = 5; cr = 2;
   fl *= cr;
   if(fl != 10){
     lrc = 183;
     if(prlc) printf(f,lrc);
   }
   fl = 5; sr = 2;
   fl *= sr;
   if(fl != 10){
     lrc = 184;
     if(prlc) printf(f,lrc);
   }
   fl = 5; ir = 2;
   fl *= ir;
   if(fl != 10){
     lrc = 185;
     if(prlc) printf(f,lrc);
   }
   fl = 5; lr = 2;
   fl *= lr;
   if(fl != 10){
     lrc = 186;
     if(prlc) printf(f,lrc);
   }
   fl = 5; ur = 2;
   fl *= ur;
   if(fl != 10){
     lrc = 187;
     if(prlc) printf(f,lrc);
   }
   fl = 5; fr = 2;
   fl *= fr;
   if(fl != 10){
     lrc = 188;
     if(prlc) printf(f,lrc);
   }
   fl = 5; dr = 2;
   fl *= dr;
   if(fl != 10){
     lrc = 189;
     if(prlc) printf(f,lrc);
   }
   dl = 5; cr = 2;
   dl *= cr;
   if(dl != 10){
     lrc = 190;
     if(prlc) printf(f,lrc);
   }
   dl = 5; sr = 2;
   dl *= sr;
   if(dl != 10){
     lrc = 191;
     if(prlc) printf(f,lrc);
   }
   dl = 5; ir = 2;
   dl *= ir;
   if(dl != 10){
     lrc = 192;
     if(prlc) printf(f,lrc);
   }
   dl = 5; lr = 2;
   dl *= lr;
   if(dl != 10){
     lrc = 193;
     if(prlc) printf(f,lrc);
   }
   dl = 5; ur = 2;
   dl *= ur;
   if(dl != 10){
     lrc = 194;
     if(prlc) printf(f,lrc);
   }
   dl = 5; fr = 2;
   dl *= fr;
   if(dl != 10){
     lrc = 195;
     if(prlc) printf(f,lrc);
   }
   dl = 5; dr = 2;
   dl *= dr;
   if(dl != 10){
     lrc = 196;
     if(prlc) printf(f,lrc);
   }
   cl = 5; cr = 2;
   cl /= cr;
   if(cl != 2){
     lrc = 197;
     if(prlc) printf(f,lrc);
   }
   cl = 5; sr = 2;
   cl /= sr;
   if(cl != 2){
     lrc = 198;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ir = 2;
   cl /= ir;
   if(cl != 2){
     lrc = 199;
     if(prlc) printf(f,lrc);
   }
   cl = 5; lr = 2;
   cl /= lr;
   if(cl != 2){
     lrc = 200;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ur = 2;
   cl /= ur;
   if(cl != 2){
     lrc = 201;
     if(prlc) printf(f,lrc);
   }
   cl = 5; fr = 2;
   cl /= fr;
   if(cl != 2){
     lrc = 202;
     if(prlc) printf(f,lrc);
   }
   cl = 5; dr = 2;
   cl /= dr;
   if(cl != 2){
     lrc = 203;
     if(prlc) printf(f,lrc);
   }
   sl = 5; cr = 2;
   sl /= cr;
   if(sl != 2){
     lrc = 204;
     if(prlc) printf(f,lrc);
   }
   sl = 5; sr = 2;
   sl /= sr;
   if(sl != 2){
     lrc = 205;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ir = 2;
   sl /= ir;
   if(sl != 2){
     lrc = 206;
     if(prlc) printf(f,lrc);
   }
   sl = 5; lr = 2;
   sl /= lr;
   if(sl != 2){
     lrc = 207;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ur = 2;
   sl /= ur;
   if(sl != 2){
     lrc = 208;
     if(prlc) printf(f,lrc);
   }
   sl = 5; fr = 2;
   sl /= fr;
   if(sl != 2){
     lrc = 209;
     if(prlc) printf(f,lrc);
   }
   sl = 5; dr = 2;
   sl /= dr;
   if(sl != 2){
     lrc = 210;
     if(prlc) printf(f,lrc);
   }
   il = 5; cr = 2;
   il /= cr;
   if(il != 2){
     lrc = 211;
     if(prlc) printf(f,lrc);
   }
   il = 5; sr = 2;
   il /= sr;
   if(il != 2){
     lrc = 212;
     if(prlc) printf(f,lrc);
   }
   il = 5; ir = 2;
   il /= ir;
   if(il != 2){
     lrc = 213;
     if(prlc) printf(f,lrc);
   }
   il = 5; lr = 2;
   il /= lr;
   if(il != 2){
     lrc = 214;
     if(prlc) printf(f,lrc);
   }
   il = 5; ur = 2;
   il /= ur;
   if(il != 2){
     lrc = 215;
     if(prlc) printf(f,lrc);
   }
   il = 5; fr = 2;
   il /= fr;
   if(il != 2){
     lrc = 216;
     if(prlc) printf(f,lrc);
   }
   il = 5; dr = 2;
   il /= dr;
   if(il != 2){
     lrc = 217;
     if(prlc) printf(f,lrc);
   }
   ll = 5; cr = 2;
   ll /= cr;
   if(ll != 2){
     lrc = 218;
     if(prlc) printf(f,lrc);
   }
   ll = 5; sr = 2;
   ll /= sr;
   if(ll != 2){
     lrc = 219;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ir = 2;
   ll /= ir;
   if(ll != 2){
     lrc = 220;
     if(prlc) printf(f,lrc);
   }
   ll = 5; lr = 2;
   ll /= lr;
   if(ll != 2){
     lrc = 221;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ur = 2;
   ll /= ur;
   if(ll != 2){
     lrc = 222;
     if(prlc) printf(f,lrc);
   }
   ll = 5; fr = 2;
   ll /= fr;
   if(ll != 2){
     lrc = 223;
     if(prlc) printf(f,lrc);
   }
   ll = 5; dr = 2;
   ll /= dr;
   if(ll != 2){
     lrc = 224;
     if(prlc) printf(f,lrc);
   }
   ul = 5; cr = 2;
   ul /= cr;
   if(ul != 2){
     lrc = 225;
     if(prlc) printf(f,lrc);
   }
   ul = 5; sr = 2;
   ul /= sr;
   if(ul != 2){
     lrc = 226;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ir = 2;
   ul /= ir;
   if(ul != 2){
     lrc = 227;
     if(prlc) printf(f,lrc);
   }
   ul = 5; lr = 2;
   ul /= lr;
   if(ul != 2){
     lrc = 228;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ur = 2;
   ul /= ur;
   if(ul != 2){
     lrc = 229;
     if(prlc) printf(f,lrc);
   }
   ul = 5; fr = 2;
   ul /= fr;
   if(ul != 2){
     lrc = 230;
     if(prlc) printf(f,lrc);
   }
   ul = 5; dr = 2;
   ul /= dr;
   if(ul != 2){
     lrc = 231;
     if(prlc) printf(f,lrc);
   }

   #ifdef NO_FLOATS
   		fl = 5; cr = 2;
   		fl /= cr;
   		if(fl != 2){
     		lrc = 232;
     		if(prlc) printf(f,lrc);
   		}
   		fl = 5; sr = 2;
   		fl /= sr;
   		if(fl != 2){
     		lrc = 233;
     		if(prlc) printf(f,lrc);
   		}
   		fl = 5; ir = 2;
   		fl /= ir;
   		if(fl != 2){
     		lrc = 234;
     		if(prlc) printf(f,lrc);
   		}
   		fl = 5; lr = 2;
   		fl /= lr;
   		if(fl != 2){
     		lrc = 235;
     		if(prlc) printf(f,lrc);
   		}
   		fl = 5; ur = 2;
   		fl /= ur;
   		if(fl != 2){
     		lrc = 236;
     		if(prlc) printf(f,lrc);
   		}
   		fl = 5; fr = 2;
   		fl /= fr;
   		if(fl != 2){
     		lrc = 237;
     		if(prlc) printf(f,lrc);
   		}
   		fl = 5; dr = 2;
   		fl /= dr;
   		if(fl != 2){
     		lrc = 238;
     		if(prlc) printf(f,lrc);
   		}
   		dl = 5; cr = 2;
   		dl /= cr;
   		if(dl != 2){
     		lrc = 239;
     		if(prlc) printf(f,lrc);
   		}
   		dl = 5; sr = 2;
   		dl /= sr;
   		if(dl != 2){
     		lrc = 240;
     		if(prlc) printf(f,lrc);
   		}
   		dl = 5; ir = 2;
   		dl /= ir;
   		if(dl != 2){
     		lrc = 241;
     		if(prlc) printf(f,lrc);
   		}
   		dl = 5; lr = 2;
   		dl /= lr;
   		if(dl != 2){
     		lrc = 242;
     		if(prlc) printf(f,lrc);
   		}
   		dl = 5; ur = 2;
   		dl /= ur;
   		if(dl != 2){
     		lrc = 243;
     		if(prlc) printf(f,lrc);
   		}
   		dl = 5; fr = 2;
   		dl /= fr;
   		if(dl != 2){
     		lrc = 244;
     		if(prlc) printf(f,lrc);
   		}
   		dl = 5; dr = 2;
   		dl /= dr;
   		if(dl != 2){
     		lrc = 245;
     		if(prlc) printf(f,lrc);
   		}
   #else
   		fl = 5; cr = 2;
   		fl /= cr;
   		if(fl != 2.5){
     		lrc = 232;
     		if(prlc) printf(f,lrc);
   		}
   		fl = 5; sr = 2;
   		fl /= sr;
   		if(fl != 2.5){
     		lrc = 233;
     		if(prlc) printf(f,lrc);
   		}
   		fl = 5; ir = 2;
   		fl /= ir;
   		if(fl != 2.5){
     		lrc = 234;
     		if(prlc) printf(f,lrc);
   		}
   		fl = 5; lr = 2;
   		fl /= lr;
   		if(fl != 2.5){
     		lrc = 235;
     		if(prlc) printf(f,lrc);
   		}
   		fl = 5; ur = 2;
   		fl /= ur;
   		if(fl != 2.5){
     		lrc = 236;
     		if(prlc) printf(f,lrc);
   		}
   		fl = 5; fr = 2;
   		fl /= fr;
   		if(fl != 2.5){
     		lrc = 237;
     		if(prlc) printf(f,lrc);
   		}
   		fl = 5; dr = 2;
   		fl /= dr;
   		if(fl != 2.5){
     		lrc = 238;
     		if(prlc) printf(f,lrc);
   		}
   		dl = 5; cr = 2;
   		dl /= cr;
   		if(dl != 2.5){
     		lrc = 239;
     		if(prlc) printf(f,lrc);
   		}
   		dl = 5; sr = 2;
   		dl /= sr;
   		if(dl != 2.5){
     		lrc = 240;
     		if(prlc) printf(f,lrc);
   		}
   		dl = 5; ir = 2;
   		dl /= ir;
   		if(dl != 2.5){
     		lrc = 241;
     		if(prlc) printf(f,lrc);
   		}
   		dl = 5; lr = 2;
   		dl /= lr;
   		if(dl != 2.5){
     		lrc = 242;
     		if(prlc) printf(f,lrc);
   		}
   		dl = 5; ur = 2;
   		dl /= ur;
   		if(dl != 2.5){
     		lrc = 243;
     		if(prlc) printf(f,lrc);
   		}
   		dl = 5; fr = 2;
   		dl /= fr;
   		if(dl != 2.5){
     		lrc = 244;
     		if(prlc) printf(f,lrc);
   		}
   		dl = 5; dr = 2;
   		dl /= dr;
   		if(dl != 2.5){
     		lrc = 245;
     		if(prlc) printf(f,lrc);
   		}
   #endif
   cl = 5; cr = 2;
   cl %= cr;
   if(cl != 1){
     lrc = 246;
     if(prlc) printf(f,lrc);
   }
   cl = 5; sr = 2;
   cl %= sr;
   if(cl != 1){
     lrc = 247;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ir = 2;
   cl %= ir;
   if(cl != 1){
     lrc = 248;
     if(prlc) printf(f,lrc);
   }
   cl = 5; lr = 2;
   cl %= lr;
   if(cl != 1){
     lrc = 249;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ur = 2;
   cl %= ur;
   if(cl != 1){
     lrc = 250;
     if(prlc) printf(f,lrc);
   }
   sl = 5; cr = 2;
   sl %= cr;
   if(sl != 1){
     lrc = 251;
     if(prlc) printf(f,lrc);
   }
   sl = 5; sr = 2;
   sl %= sr;
   if(sl != 1){
     lrc = 252;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ir = 2;
   sl %= ir;
   if(sl != 1){
     lrc = 253;
     if(prlc) printf(f,lrc);
   }
   sl = 5; lr = 2;
   sl %= lr;
   if(sl != 1){
     lrc = 254;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ur = 2;
   sl %= ur;
   if(sl != 1){
     lrc = 255;
     if(prlc) printf(f,lrc);
   }

   if(lrc != 0) {
     rc = 1;
     if(pd0->flgd != 0) printf(s714er,1);
   }
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
                case 0: return s714(pd0);
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
