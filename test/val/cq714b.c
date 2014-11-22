/*
  !!DESCRIPTION!! C-Manual Chapter 9: Statements
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

   il = 5; cr = 2;
   il %= cr;
   if(il != 1){
     lrc = 256;
     if(prlc) printf(f,lrc);
   }
   il = 5; sr = 2;
   il %= sr;
   if(il != 1){
     lrc = 257;
     if(prlc) printf(f,lrc);
   }
   il = 5; ir = 2;
   il %= ir;
   if(il != 1){
     lrc = 258;
     if(prlc) printf(f,lrc);
   }
   il = 5; lr = 2;
   il %= lr;
   if(il != 1){
     lrc = 259;
     if(prlc) printf(f,lrc);
   }
   il = 5; ur = 2;
   il %= ur;
   if(il != 1){
     lrc = 260;
     if(prlc) printf(f,lrc);
   }
   ll = 5; cr = 2;
   ll %= cr;
   if(ll != 1){
     lrc = 261;
     if(prlc) printf(f,lrc);
   }
   ll = 5; sr = 2;
   ll %= sr;
   if(ll != 1){
     lrc = 262;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ir = 2;
   ll %= ir;
   if(ll != 1){
     lrc = 263;
     if(prlc) printf(f,lrc);
   }
   ll = 5; lr = 2;
   ll %= lr;
   if(ll != 1){
     lrc = 264;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ur = 2;
   ll %= ur;
   if(ll != 1){
     lrc = 265;
     if(prlc) printf(f,lrc);
   }
   ul = 5; cr = 2;
   ul %= cr;
   if(ul != 1){
     lrc = 266;
     if(prlc) printf(f,lrc);
   }
   ul = 5; sr = 2;
   ul %= sr;
   if(ul != 1){
     lrc = 267;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ir = 2;
   ul %= ir;
   if(ul != 1){
     lrc = 268;
     if(prlc) printf(f,lrc);
   }
   ul = 5; lr = 2;
   ul %= lr;
   if(ul != 1){
     lrc = 269;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ur = 2;
   ul %= ur;
   if(ul != 1){
     lrc = 270;
     if(prlc) printf(f,lrc);
   }
   cl = 5; cr = 2;
   cl >>= cr;
   if(cl != 1){
     lrc = 271;
     if(prlc) printf(f,lrc);
   }
   cl = 5; sr = 2;
   cl >>= sr;
   if(cl != 1){
     lrc = 272;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ir = 2;
   cl >>= ir;
   if(cl != 1){
     lrc = 273;
     if(prlc) printf(f,lrc);
   }
   cl = 5; lr = 2;
   cl >>= lr;
   if(cl != 1){
     lrc = 274;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ur = 2;
   cl >>= ur;
   if(cl != 1){
     lrc = 275;
     if(prlc) printf(f,lrc);
   }
   sl = 5; cr = 2;
   sl >>= cr;
   if(sl != 1){
     lrc = 276;
     if(prlc) printf(f,lrc);
   }
   sl = 5; sr = 2;
   sl >>= sr;
   if(sl != 1){
     lrc = 277;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ir = 2;
   sl >>= ir;
   if(sl != 1){
     lrc = 278;
     if(prlc) printf(f,lrc);
   }
   sl = 5; lr = 2;
   sl >>= lr;
   if(sl != 1){
     lrc = 279;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ur = 2;
   sl >>= ur;
   if(sl != 1){
     lrc = 280;
     if(prlc) printf(f,lrc);
   }
   il = 5; cr = 2;
   il >>= cr;
   if(il != 1){
     lrc = 281;
     if(prlc) printf(f,lrc);
   }
   il = 5; sr = 2;
   il >>= sr;
   if(il != 1){
     lrc = 282;
     if(prlc) printf(f,lrc);
   }
   il = 5; ir = 2;
   il >>= ir;
   if(il != 1){
     lrc = 283;
     if(prlc) printf(f,lrc);
   }
   il = 5; lr = 2;
   il >>= lr;
   if(il != 1){
     lrc = 284;
     if(prlc) printf(f,lrc);
   }
   il = 5; ur = 2;
   il >>= ur;
   if(il != 1){
     lrc = 285;
     if(prlc) printf(f,lrc);
   }
   ll = 5; cr = 2;
   ll >>= cr;
   if(ll != 1){
     lrc = 286;
     if(prlc) printf(f,lrc);
   }
   ll = 5; sr = 2;
   ll >>= sr;
   if(ll != 1){
     lrc = 287;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ir = 2;
   ll >>= ir;
   if(ll != 1){
     lrc = 288;
     if(prlc) printf(f,lrc);
   }
   ll = 5; lr = 2;
   ll >>= lr;
   if(ll != 1){
     lrc = 289;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ur = 2;
   ll >>= ur;
   if(ll != 1){
     lrc = 290;
     if(prlc) printf(f,lrc);
   }
   ul = 5; cr = 2;
   ul >>= cr;
   if(ul != 1){
     lrc = 291;
     if(prlc) printf(f,lrc);
   }
   ul = 5; sr = 2;
   ul >>= sr;
   if(ul != 1){
     lrc = 292;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ir = 2;
   ul >>= ir;
   if(ul != 1){
     lrc = 293;
     if(prlc) printf(f,lrc);
   }
   ul = 5; lr = 2;
   ul >>= lr;
   if(ul != 1){
     lrc = 294;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ur = 2;
   ul >>= ur;
   if(ul != 1){
     lrc = 295;
     if(prlc) printf(f,lrc);
   }
   cl = 5; cr = 2;
   cl <<= cr;
   if(cl != 20){
     lrc = 296;
     if(prlc) printf(f,lrc);
   }
   cl = 5; sr = 2;
   cl <<= sr;
   if(cl != 20){
     lrc = 297;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ir = 2;
   cl <<= ir;
   if(cl != 20){
     lrc = 298;
     if(prlc) printf(f,lrc);
   }
   cl = 5; lr = 2;
   cl <<= lr;
   if(cl != 20){
     lrc = 299;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ur = 2;
   cl <<= ur;
   if(cl != 20){
     lrc = 300;
     if(prlc) printf(f,lrc);
   }
   sl = 5; cr = 2;
   sl <<= cr;
   if(sl != 20){
     lrc = 301;
     if(prlc) printf(f,lrc);
   }
   sl = 5; sr = 2;
   sl <<= sr;
   if(sl != 20){
     lrc = 302;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ir = 2;
   sl <<= ir;
   if(sl != 20){
     lrc = 303;
     if(prlc) printf(f,lrc);
   }
   sl = 5; lr = 2;
   sl <<= lr;
   if(sl != 20){
     lrc = 304;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ur = 2;
   sl <<= ur;
   if(sl != 20){
     lrc = 305;
     if(prlc) printf(f,lrc);
   }
   il = 5; cr = 2;
   il <<= cr;
   if(il != 20){
     lrc = 306;
     if(prlc) printf(f,lrc);
   }
   il = 5; sr = 2;
   il <<= sr;
   if(il != 20){
     lrc = 307;
     if(prlc) printf(f,lrc);
   }
   il = 5; ir = 2;
   il <<= ir;
   if(il != 20){
     lrc = 308;
     if(prlc) printf(f,lrc);
   }
   il = 5; lr = 2;
   il <<= lr;
   if(il != 20){
     lrc = 309;
     if(prlc) printf(f,lrc);
   }
   il = 5; ur = 2;
   il <<= ur;
   if(il != 20){
     lrc = 310;
     if(prlc) printf(f,lrc);
   }
   ll = 5; cr = 2;
   ll <<= cr;
   if(ll != 20){
     lrc = 311;
     if(prlc) printf(f,lrc);
   }
   ll = 5; sr = 2;
   ll <<= sr;
   if(ll != 20){
     lrc = 312;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ir = 2;
   ll <<= ir;
   if(ll != 20){
     lrc = 313;
     if(prlc) printf(f,lrc);
   }
   ll = 5; lr = 2;
   ll <<= lr;
   if(ll != 20){
     lrc = 314;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ur = 2;
   ll <<= ur;
   if(ll != 20){
     lrc = 315;
     if(prlc) printf(f,lrc);
   }
   ul = 5; cr = 2;
   ul <<= cr;
   if(ul != 20){
     lrc = 316;
     if(prlc) printf(f,lrc);
   }
   ul = 5; sr = 2;
   ul <<= sr;
   if(ul != 20){
     lrc = 317;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ir = 2;
   ul <<= ir;
   if(ul != 20){
     lrc = 318;
     if(prlc) printf(f,lrc);
   }
   ul = 5; lr = 2;
   ul <<= lr;
   if(ul != 20){
     lrc = 319;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ur = 2;
   ul <<= ur;
   if(ul != 20){
     lrc = 320;
     if(prlc) printf(f,lrc);
   }
   cl = 12; cr = 10;
   cl &= cr;
   if(cl != 8){
     lrc = 321;
     if(prlc) printf(f,lrc);
   }
   cl = 12; sr = 10;
   cl &= sr;
   if(cl != 8){
     lrc = 322;
     if(prlc) printf(f,lrc);
   }
   cl = 12; ir = 10;
   cl &= ir;
   if(cl != 8){
     lrc = 323;
     if(prlc) printf(f,lrc);
   }
   cl = 12; lr = 10;
   cl &= lr;
   if(cl != 8){
     lrc = 324;
     if(prlc) printf(f,lrc);
   }
   cl = 12; ur = 10;
   cl &= ur;
   if(cl != 8){
     lrc = 325;
     if(prlc) printf(f,lrc);
   }
   sl = 12; cr = 10;
   sl &= cr;
   if(sl != 8){
     lrc = 326;
     if(prlc) printf(f,lrc);
   }
   sl = 12; sr = 10;
   sl &= sr;
   if(sl != 8){
     lrc = 327;
     if(prlc) printf(f,lrc);
   }
   sl = 12; ir = 10;
   sl &= ir;
   if(sl != 8){
     lrc = 328;
     if(prlc) printf(f,lrc);
   }
   sl = 12; lr = 10;
   sl &= lr;
   if(sl != 8){
     lrc = 329;
     if(prlc) printf(f,lrc);
   }
   sl = 12; ur = 10;
   sl &= ur;
   if(sl != 8){
     lrc = 330;
     if(prlc) printf(f,lrc);
   }
   il = 12; cr = 10;
   il &= cr;
   if(il != 8){
     lrc = 331;
     if(prlc) printf(f,lrc);
   }
   il = 12; sr = 10;
   il &= sr;
   if(il != 8){
     lrc = 332;
     if(prlc) printf(f,lrc);
   }
   il = 12; ir = 10;
   il &= ir;
   if(il != 8){
     lrc = 333;
     if(prlc) printf(f,lrc);
   }
   il = 12; lr = 10;
   il &= lr;
   if(il != 8){
     lrc = 334;
     if(prlc) printf(f,lrc);
   }
   il = 12; ur = 10;
   il &= ur;
   if(il != 8){
     lrc = 335;
     if(prlc) printf(f,lrc);
   }
   ll = 12; cr = 10;
   ll &= cr;
   if(ll != 8){
     lrc = 336;
     if(prlc) printf(f,lrc);
   }
   ll = 12; sr = 10;
   ll &= sr;
   if(ll != 8){
     lrc = 337;
     if(prlc) printf(f,lrc);
   }
   ll = 12; ir = 10;
   ll &= ir;
   if(ll != 8){
     lrc = 338;
     if(prlc) printf(f,lrc);
   }
   ll = 12; lr = 10;
   ll &= lr;
   if(ll != 8){
     lrc = 339;
     if(prlc) printf(f,lrc);
   }
   ll = 12; ur = 10;
   ll &= ur;
   if(ll != 8){
     lrc = 340;
     if(prlc) printf(f,lrc);
   }
   ul = 12; cr = 10;
   ul &= cr;
   if(ul != 8){
     lrc = 341;
     if(prlc) printf(f,lrc);
   }
   ul = 12; sr = 10;
   ul &= sr;
   if(ul != 8){
     lrc = 342;
     if(prlc) printf(f,lrc);
   }
   ul = 12; ir = 10;
   ul &= ir;
   if(ul != 8){
     lrc = 343;
     if(prlc) printf(f,lrc);
   }
   ul = 12; lr = 10;
   ul &= lr;
   if(ul != 8){
     lrc = 344;
     if(prlc) printf(f,lrc);
   }
   ul = 12; ur = 10;
   ul &= ur;
   if(ul != 8){
     lrc = 345;
     if(prlc) printf(f,lrc);
   }
   cl = 12; cr = 10;
   cl ^= cr;
   if(cl != 6){
     lrc = 346;
     if(prlc) printf(f,lrc);
   }
   cl = 12; sr = 10;
   cl ^= sr;
   if(cl != 6){
     lrc = 347;
     if(prlc) printf(f,lrc);
   }
   cl = 12; ir = 10;
   cl ^= ir;
   if(cl != 6){
     lrc = 348;
     if(prlc) printf(f,lrc);
   }
   cl = 12; lr = 10;
   cl ^= lr;
   if(cl != 6){
     lrc = 349;
     if(prlc) printf(f,lrc);
   }
   cl = 12; ur = 10;
   cl ^= ur;
   if(cl != 6){
     lrc = 350;
     if(prlc) printf(f,lrc);
   }
   sl = 12; cr = 10;
   sl ^= cr;
   if(sl != 6){
     lrc = 351;
     if(prlc) printf(f,lrc);
   }
   sl = 12; sr = 10;
   sl ^= sr;
   if(sl != 6){
     lrc = 352;
     if(prlc) printf(f,lrc);
   }
   sl = 12; ir = 10;
   sl ^= ir;
   if(sl != 6){
     lrc = 353;
     if(prlc) printf(f,lrc);
   }
   sl = 12; lr = 10;
   sl ^= lr;
   if(sl != 6){
     lrc = 354;
     if(prlc) printf(f,lrc);
   }
   sl = 12; ur = 10;
   sl ^= ur;
   if(sl != 6){
     lrc = 355;
     if(prlc) printf(f,lrc);
   }
   il = 12; cr = 10;
   il ^= cr;
   if(il != 6){
     lrc = 356;
     if(prlc) printf(f,lrc);
   }
   il = 12; sr = 10;
   il ^= sr;
   if(il != 6){
     lrc = 357;
     if(prlc) printf(f,lrc);
   }
   il = 12; ir = 10;
   il ^= ir;
   if(il != 6){
     lrc = 358;
     if(prlc) printf(f,lrc);
   }
   il = 12; lr = 10;
   il ^= lr;
   if(il != 6){
     lrc = 359;
     if(prlc) printf(f,lrc);
   }
   il = 12; ur = 10;
   il ^= ur;
   if(il != 6){
     lrc = 360;
     if(prlc) printf(f,lrc);
   }
   ll = 12; cr = 10;
   ll ^= cr;
   if(ll != 6){
     lrc = 361;
     if(prlc) printf(f,lrc);
   }
   ll = 12; sr = 10;
   ll ^= sr;
   if(ll != 6){
     lrc = 362;
     if(prlc) printf(f,lrc);
   }
   ll = 12; ir = 10;
   ll ^= ir;
   if(ll != 6){
     lrc = 363;
     if(prlc) printf(f,lrc);
   }
   ll = 12; lr = 10;
   ll ^= lr;
   if(ll != 6){
     lrc = 364;
     if(prlc) printf(f,lrc);
   }
   ll = 12; ur = 10;
   ll ^= ur;
   if(ll != 6){
     lrc = 365;
     if(prlc) printf(f,lrc);
   }
   ul = 12; cr = 10;
   ul ^= cr;
   if(ul != 6){
     lrc = 366;
     if(prlc) printf(f,lrc);
   }
   ul = 12; sr = 10;
   ul ^= sr;
   if(ul != 6){
     lrc = 367;
     if(prlc) printf(f,lrc);
   }
   ul = 12; ir = 10;
   ul ^= ir;
   if(ul != 6){
     lrc = 368;
     if(prlc) printf(f,lrc);
   }
   ul = 12; lr = 10;
   ul ^= lr;
   if(ul != 6){
     lrc = 369;
     if(prlc) printf(f,lrc);
   }
   ul = 12; ur = 10;
   ul ^= ur;
   if(ul != 6){
     lrc = 370;
     if(prlc) printf(f,lrc);
   }
   cl = 12; cr = 10;
   cl |= cr;
   if(cl != 14){
     lrc = 371;
     if(prlc) printf(f,lrc);
   }
   cl = 12; sr = 10;
   cl |= sr;
   if(cl != 14){
     lrc = 372;
     if(prlc) printf(f,lrc);
   }
   cl = 12; ir = 10;
   cl |= ir;
   if(cl != 14){
     lrc = 373;
     if(prlc) printf(f,lrc);
   }
   cl = 12; lr = 10;
   cl |= lr;
   if(cl != 14){
     lrc = 374;
     if(prlc) printf(f,lrc);
   }
   cl = 12; ur = 10;
   cl |= ur;
   if(cl != 14){
     lrc = 375;
     if(prlc) printf(f,lrc);
   }
   sl = 12; cr = 10;
   sl |= cr;
   if(sl != 14){
     lrc = 376;
     if(prlc) printf(f,lrc);
   }
   sl = 12; sr = 10;
   sl |= sr;
   if(sl != 14){
     lrc = 377;
     if(prlc) printf(f,lrc);
   }
   sl = 12; ir = 10;
   sl |= ir;
   if(sl != 14){
     lrc = 378;
     if(prlc) printf(f,lrc);
   }
   sl = 12; lr = 10;
   sl |= lr;
   if(sl != 14){
     lrc = 379;
     if(prlc) printf(f,lrc);
   }
   sl = 12; ur = 10;
   sl |= ur;
   if(sl != 14){
     lrc = 380;
     if(prlc) printf(f,lrc);
   }
   il = 12; cr = 10;
   il |= cr;
   if(il != 14){
     lrc = 381;
     if(prlc) printf(f,lrc);
   }
   il = 12; sr = 10;
   il |= sr;
   if(il != 14){
     lrc = 382;
     if(prlc) printf(f,lrc);
   }
   il = 12; ir = 10;
   il |= ir;
   if(il != 14){
     lrc = 383;
     if(prlc) printf(f,lrc);
   }
   il = 12; lr = 10;
   il |= lr;
   if(il != 14){
     lrc = 384;
     if(prlc) printf(f,lrc);
   }
   il = 12; ur = 10;
   il |= ur;
   if(il != 14){
     lrc = 385;
     if(prlc) printf(f,lrc);
   }
   ll = 12; cr = 10;
   ll |= cr;
   if(ll != 14){
     lrc = 386;
     if(prlc) printf(f,lrc);
   }
   ll = 12; sr = 10;
   ll |= sr;
   if(ll != 14){
     lrc = 387;
     if(prlc) printf(f,lrc);
   }
   ll = 12; ir = 10;
   ll |= ir;
   if(ll != 14){
     lrc = 388;
     if(prlc) printf(f,lrc);
   }
   ll = 12; lr = 10;
   ll |= lr;
   if(ll != 14){
     lrc = 389;
     if(prlc) printf(f,lrc);
   }
   ll = 12; ur = 10;
   ll |= ur;
   if(ll != 14){
     lrc = 390;
     if(prlc) printf(f,lrc);
   }
   ul = 12; cr = 10;
   ul |= cr;
   if(ul != 14){
     lrc = 391;
     if(prlc) printf(f,lrc);
   }
   ul = 12; sr = 10;
   ul |= sr;
   if(ul != 14){
     lrc = 392;
     if(prlc) printf(f,lrc);
   }
   ul = 12; ir = 10;
   ul |= ir;
   if(ul != 14){
     lrc = 393;
     if(prlc) printf(f,lrc);
   }
   ul = 12; lr = 10;
   ul |= lr;
   if(ul != 14){
     lrc = 394;
     if(prlc) printf(f,lrc);
   }
   ul = 12; ur = 10;
   ul |= ur;
   if(ul != 14){
     lrc = 395;
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
