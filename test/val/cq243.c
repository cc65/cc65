/*
  !!DESCRIPTION!! C-Manual Chapter 2.43: character constants
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

/*********************************************************************************************
        2.4.3 Character constants
**********************************************************************************************/

#ifndef NO_OLD_FUNC_DECL
zerofill(x)
char *x;
{
#else
void zerofill(char *x) {
#endif
   int j;

   for (j=0; j<256; j++) *x++ = 0;
}

#ifndef NO_OLD_FUNC_DECL
sumof(x)
char *x;
{
#else
int sumof(char *x) {
#endif
   char *p;
   int total, j;

   p = x;
   total = 0;

   for(j=0; j<256; j++) total = total+ *p++;
   return total;
}

char chars[256];

#ifndef NO_OLD_FUNC_DECL
s243(pd0)
struct defs *pd0;
{
#else
int s243(struct defs *pd0) {
#endif
   static char s243er[] = "s243,er%d\n";
   static char qs243[8] = "s243   ";
   char *ps, *pt;
   int rc;
/*   char chars[256]; */

   rc = 0;
   ps = qs243;
   pt = pd0->rfs;
   while(*pt++ = *ps++);

     /* One of the problems that arises when testing character constants
        is that of definition: What, exactly, is the character set?
        In order to guarantee a certain amount of machine independence,
        the character set we will use here is the set of characters writ-
        able as escape sequences in C, plus those characters used in writ-
        ing C programs, i.e.,

        letters:
                   ABCDEFGHIJKLMNOPQRSTUVWXYZ      26
                   abcdefghijklmnopqrstuvwxyz      26
        numbers:
                   0123456789                      10
        special characters:
                   ~!"#%&()_=-^|{}[]+;*:<>,.?/     27
        extra special characters:
                   newline           \n
                   horizontal tab    \t
                   backspace         \b
                   carriage return   \r
                   form feed         \f
                   backslash         \\
                   single quote      \'             7
        blank & NUL                                 2
                                                  ---
                                                   98

        Any specific implementation of C may of course support additional
        characters.                                       */

        /* Since the value of a character constant is the numerical value
           of the character in the machine's character set, there should
           be a one-to-one correspondence between characters and values. */

   zerofill(chars);

   chars['a'] = 1;   chars['A'] = 1;   chars['~'] = 1;   chars['0'] = 1;
   chars['b'] = 1;   chars['B'] = 1;   chars['!'] = 1;   chars['1'] = 1;
   chars['c'] = 1;   chars['C'] = 1;   chars['"'] = 1;   chars['2'] = 1;
   chars['d'] = 1;   chars['D'] = 1;   chars['#'] = 1;   chars['3'] = 1;
   chars['e'] = 1;   chars['E'] = 1;   chars['%'] = 1;   chars['4'] = 1;
   chars['f'] = 1;   chars['F'] = 1;   chars['&'] = 1;   chars['5'] = 1;
   chars['g'] = 1;   chars['G'] = 1;   chars['('] = 1;   chars['6'] = 1;
   chars['h'] = 1;   chars['H'] = 1;   chars[')'] = 1;   chars['7'] = 1;
   chars['i'] = 1;   chars['I'] = 1;   chars['_'] = 1;   chars['8'] = 1;
   chars['j'] = 1;   chars['J'] = 1;   chars['='] = 1;   chars['9'] = 1;
   chars['k'] = 1;   chars['K'] = 1;   chars['-'] = 1;
   chars['l'] = 1;   chars['L'] = 1;   chars['^'] = 1;
   chars['m'] = 1;   chars['M'] = 1;   chars['|'] = 1;   chars['\n'] = 1;
   chars['n'] = 1;   chars['N'] = 1;                     chars['\t'] = 1;
   chars['o'] = 1;   chars['O'] = 1;   chars['{'] = 1;   chars['\b'] = 1;
   chars['p'] = 1;   chars['P'] = 1;   chars['}'] = 1;   chars['\r'] = 1;
   chars['q'] = 1;   chars['Q'] = 1;   chars['['] = 1;   chars['\f'] = 1;
   chars['r'] = 1;   chars['R'] = 1;   chars[']'] = 1;
   chars['s'] = 1;   chars['S'] = 1;   chars['+'] = 1;   chars['\\'] = 1;
   chars['t'] = 1;   chars['T'] = 1;   chars[';'] = 1;   chars['\''] = 1;
   chars['u'] = 1;   chars['U'] = 1;   chars['*'] = 1;
   chars['v'] = 1;   chars['V'] = 1;   chars[':'] = 1;   chars['\0'] = 1;
   chars['w'] = 1;   chars['W'] = 1;   chars['<'] = 1;   chars[' '] = 1;
   chars['x'] = 1;   chars['X'] = 1;   chars['>'] = 1;
   chars['y'] = 1;   chars['Y'] = 1;   chars[','] = 1;
   chars['z'] = 1;   chars['Z'] = 1;   chars['.'] = 1;
                                       chars['?'] = 1;
                                       chars['/'] = 1;

   if(sumof(chars) != 98){
     rc = rc+1;
     if(pd0->flgd != 0) printf(s243er,1);
   }

   #ifndef NO_BACKSLASH_CHARCODE

   /* Finally, the escape \ddd consists of the backslash followed
      by 1, 2, or 3 octal digits which are taken to specify  the
      desired character.                           */

/*
    this test is non portable and inaccurate, we replace it
    by a more failproof version

   if(
   	'\0'    !=   0 ||
   	'\01'   !=   1 ||
	'\02'   !=   2 ||
	'\03'   !=   3 ||
    	'\04'   !=   4 ||
	'\05'   !=   5 ||
	'\06'   !=   6 ||
    	'\07'   !=   7 ||
	'\10'   !=   8 ||
    	'\17'   !=  15 ||
	'\20'   !=  16 ||
	'\77'   !=  63 ||
    	'\100'  !=  64 ||
	'\177'  != 127
	)
*/
    if(
     ('0' != '\60') ||
     ('9' != '\71') ||
     ('A' != '\101') ||
     ('Z' != '\132') ||
     ('a' != '\141') ||
     ('z' != '\172')
      )

	{
     rc = rc+8;
     if(pd0->flgd != 0)
     {
        printf(s243er,8);
     }
   }

   #endif

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
                case 0: return s243(pd0);
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
